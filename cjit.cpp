#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>

#include <clang/Basic/DiagnosticOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/Driver.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/PreprocessorOptions.h>

#include <iostream>

using namespace llvm;
using namespace clang;

int main(int argc, char **argv) {
  // Path to the C file
  std::string InputPath = "simple.c";

  // Arguments to pass to the clang frontend
  std::vector<const char *> Args;
  Args.push_back(InputPath.c_str());

  // Prepare DiagnosticEngine
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

  auto Context = std::make_unique<LLVMContext>();

  // Create an action and make the compiler instance carry it out
  auto Act = std::make_unique<EmitLLVMOnlyAction>(&*Context);

  if (!Clang.ExecuteAction(*Act)) {
    std::cerr << "Failed to emit module LLVM\n";
    Diags->dump();
    return 1;
  }

  // Grab the module built by the EmitLLVMOnlyAction
  auto Module = Act->takeModule();
  Module->print(dbgs(), /*AAW=*/nullptr);
  return 0;
}
