#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>

#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/Driver.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/PreprocessorOptions.h>

#include <dlfcn.h>
#include <iostream>

using namespace llvm;
using namespace clang;

using OptimizeFuncTy = bool(const char *, const char **, size_t *);

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " /path/to/optimizer /path/to/src.c\n";
    return 1;
  }

  std::string InputPath = argv[2];

  // Arguments to pass to the clang frontend
  std::vector<const char *> Args;
  Args.push_back(InputPath.c_str());

  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts(new DiagnosticOptions());
  auto *TextDiagPrinter = new TextDiagnosticPrinter(errs(), &*DiagOpts);
  IntrusiveRefCntPtr<DiagnosticIDs> DiagIDs;
  auto *Diags = new DiagnosticsEngine(DiagIDs, &*DiagOpts, TextDiagPrinter);

  // Create the compiler invocation
  auto CI = std::make_shared<CompilerInvocation>();
  bool Created = CompilerInvocation::CreateFromArgs(*CI, Args, *Diags);
  if (!Created) {
    std::cerr << "Failed to create compiler\n";
    return 1;
  }

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  auto TripleStr = sys::getProcessTriple();
  Triple T(TripleStr);
  if (T.isOSBinFormatCOFF())
    T.setObjectFormat(Triple::ELF);

  std::cerr << "Target triple is " << T.str() << "\n";

  using clang::TargetOptions;

  auto TheTargetOptions = std::make_shared<TargetOptions>();
  TheTargetOptions->Triple = T.str();

  auto *TargetInfo = TargetInfo::CreateTargetInfo(*Diags, TheTargetOptions);

  // Create the compiler instance
  auto Clang = CompilerInstance();
  Clang.setInvocation(CI);
  Clang.createDiagnostics();
  Clang.setTarget(TargetInfo);

  // Get ready to report problems
  Clang.createDiagnostics();
  if (!Clang.hasDiagnostics())
    return 1;

  // auto Context = std::make_unique<LLVMContext>();

  // Create an action and make the compiler instance carry it out
  auto Act = std::make_unique<EmitLLVMOnlyAction>();

  if (!Clang.ExecuteAction(*Act)) {
    std::cerr << "Failed to emit module LLVM\n";
    Diags->dump();
    return 1;
  }

  // Grab the module built by the EmitLLVMOnlyAction
  auto Module = Act->takeModule();
  std::cerr << "Got the module\n";
  std::string ModuleStr;
  llvm::raw_string_ostream OS(ModuleStr);
  Module->print(OS, /*AAW=*/nullptr);
  std::cerr << ModuleStr << "\n";

  const char *Optimizer = argv[1];
  void *OptLib = dlopen(Optimizer, RTLD_LOCAL | RTLD_LAZY);
  if (!OptLib) {
    std::cerr << "Failed to open optimizer library " << Optimizer << "\n";
    return 1;
  }

  OptimizeFuncTy *OptFunc = (OptimizeFuncTy *)dlsym(OptLib, "optimize");
  if (!OptFunc) {
    std::cerr << "Failed to find 'optimize' symbol in optimizer library "
              << Optimizer << "\n";
    return 1;
  }

  const char *OptimizedModuleStr;
  size_t OptimizedModuleLen;
  bool Optimized = OptFunc(ModuleStr.c_str(), &OptimizedModuleStr, &OptimizedModuleLen);
  if (!Optimized) {
    std::cerr << "Failed to optimize\n";
    return 1;
  }

  std::cerr << "IR After Optimizations: " << std::string(OptimizedModuleStr, OptimizedModuleLen) << "\n";

  (void)dlclose(OptLib);
  std::cerr << "Finished cjit\n";
  return 0;
}
