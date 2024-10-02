#include <clang/Basic/DiagnosticOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <iostream>
#include <llvm/ADT/IntrusiveRefCntPtr.h>

#include <llvm/IR/Module.h>
// #include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>

using clang::CompilerInstance;
using clang::CompilerInvocation;
using clang::DiagnosticConsumer;
using clang::DiagnosticOptions;
using clang::DiagnosticsEngine;
using clang::EmitLLVMOnlyAction;
using clang::TextDiagnosticPrinter;

using clang::DiagnosticIDs;
using llvm::ArrayRef;
using llvm::IntrusiveRefCntPtr;
using llvm::MemoryBuffer;

using namespace std;
using namespace llvm;

int main() {
  // Path to the C file
  string inputPath = "getinmemory.c";

  // Arguments to pass to the clang frontend
  vector<const char *> args;
  args.push_back(inputPath.c_str());

  // Prepare DiagnosticEngine
  DiagnosticOptions DiagOpts;
  TextDiagnosticPrinter *textDiagPrinter =
      new clang::TextDiagnosticPrinter(errs(), &DiagOpts);
  IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
  DiagnosticsEngine *pDiagnosticsEngine =
      new DiagnosticsEngine(pDiagIDs, &DiagOpts, textDiagPrinter);

  // auto DiagOpts = DiagnosticOptions();
  // DiagOpts.ShowLine = true;
  // TextDiagnosticPrinter *DiagClient =
  //     new TextDiagnosticPrinter(llvm::errs(), &DiagOpts);
  // IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  // DiagnosticsEngine Diags(DiagID, &DiagOpts);

  // IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts =
  //     clang::CreateAndPopulateDiagOpts({});

  // TextDiagnosticPrinter *DiagClient
  //   = new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);
  // FixupDiagPrefixExeName(DiagClient, ProgName);

  // IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

  // DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);
  // auto Diags = CompilerInstance::createDiagnostics(new DiagnosticOptions);

  // Create the compiler invocation
  auto CI = std::make_shared<CompilerInvocation>();
  bool Created = clang::CompilerInvocation::CreateFromArgs(*CI, args, *pDiagnosticsEngine);
  if (!Created) {
    std::cerr << "Failed to create compiler\n";
    return 1;
  }

  // Create the compiler instance
  clang::CompilerInstance Clang;
  Clang.setInvocation(CI);
  Clang.createDiagnostics();

  InitializeAllTargets();
  // Initialize
  const std::shared_ptr<clang::TargetOptions> targetOptions =
      std::make_shared<clang::TargetOptions>();
  // targetOptions->Triple = string("x86-64");
  clang::TargetInfo *pTargetInfo =
      clang::TargetInfo::CreateTargetInfo(*pDiagnosticsEngine, targetOptions);
  Clang.setTarget(pTargetInfo);

  // Get ready to report problems
  // Clang.createDiagnostics(DiagOpts.get(), DiagClient);
  // if (!Clang.hasDiagnostics())
  //   return 1;

  // Create an action and make the compiler instance carry it out
  std::unique_ptr<clang::CodeGenAction> Act(new clang::EmitLLVMOnlyAction());
  if (!Clang.ExecuteAction(*Act))
    return 1;

  // Grab the module built by the EmitLLVMOnlyAction
  auto module = Act->takeModule();

  // Print all functions in the module
  for (llvm::Module::FunctionListType::iterator i =
           module->getFunctionList().begin();
       i != module->getFunctionList().end(); ++i)
    printf("%s\n", i->getName().str().c_str());

  return 0;
}
