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
#include <sys/wait.h>
#include <unistd.h>

using namespace llvm;
using namespace clang;

// TODO: Use llvm cl::opt's

using OptimizeFuncTy = bool(const char *, const char **, size_t *);

std::unique_ptr<llvm::Module> parseSourceFile(LLVMContext &Ctx,
                                              const std::string &InputPath) {
  // Arguments to pass to the clang frontend
  std::vector<const char *> Args;
  Args.push_back(InputPath.c_str());
  // FIXME: Figure out these paths automatically.
  Args.push_back("-I/usr/lib/gcc/x86_64-linux-gnu/13/include"); // FIXME
  Args.push_back("-I/usr/local/include");                       // FIXME
  Args.push_back("-I/usr/include/x86_64-linux-gnu");            // FIXME
  Args.push_back("-I/usr/include");                             // FIXME
  Args.push_back(
      "-I/home/damakogon/work/projects/cjit/examples/curl/include"); // FIXME
  Args.push_back("-disable-O0-optnone");

  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts(new DiagnosticOptions());
  auto *TextDiagPrinter = new TextDiagnosticPrinter(errs(), &*DiagOpts);
  IntrusiveRefCntPtr<DiagnosticIDs> DiagIDs;
  auto *Diags = new DiagnosticsEngine(DiagIDs, &*DiagOpts, TextDiagPrinter);

  // Create the compiler invocation
  auto CI = std::make_shared<CompilerInvocation>();
  bool Created = CompilerInvocation::CreateFromArgs(*CI, Args, *Diags);
  if (!Created) {
    std::cerr << "Failed to create compiler\n";
    return nullptr;
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
    return nullptr;

  // auto Context = std::make_unique<LLVMContext>();

  // Create an action and make the compiler instance carry it out
  auto Act = std::make_unique<EmitLLVMOnlyAction>(&Ctx);

  if (!Clang.ExecuteAction(*Act)) {
    std::cerr << "Failed to emit module LLVM\n";
    return nullptr;
  }

  // Grab the module built by the EmitLLVMOnlyAction
  return Act->takeModule();
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " /path/to/optimizer /path/to/src.c\n";
    return 1;
  }

  LLVMContext Ctx;

  std::string InputPath = argv[2];
  auto Module = parseSourceFile(Ctx, InputPath);
  if (!Module)
    return 1;

  std::cerr << "Got the module\n";
  std::string ModuleStr;
  llvm::raw_string_ostream OS(ModuleStr);
  Module->print(OS, /*AAW=*/nullptr);
  std::cerr << ModuleStr << "\n";

  std::string InitialIRFile = "init.ll";
  std::error_code EC;
  llvm::raw_fd_ostream InitialIROS(InitialIRFile, EC);
  if (EC) {
    return 1;
  }

  const char *Optimizer = argv[1];

  int pipeP2C[2], pipeC2P[2];
  if (pipe(pipeP2C) != 0 || pipe(pipeC2P) != 0) {
    // error
    // TODO: appropriate handling
    return 1;
  }
  int pid = fork();
  if (pid < 0) {
    return 1;
  }
  if (pid > 0) {
    // parent
    // close unused ends:
    close(pipeP2C[0]); // read end
    close(pipeC2P[1]); // write end

    // use pipes to communicate with child...

    const char *Buf = ModuleStr.data();
    size_t BytesLeft = ModuleStr.size();
    ssize_t Written = write(pipeP2C[1], Buf, BytesLeft);
    if (Written != BytesLeft) {
      std::cerr << "Failed to write initial IR to child\n";
      return 1;
    }
    close(pipeP2C[1]);
    std::cerr << "Sent initial IR to optimizer\n";

    std::string FromOpt;

    while (true) {
      char RecvBuf[1024] = {'\0'};
      ssize_t Read = read(pipeC2P[0], RecvBuf, 1023);
      if (Read == -1) {
        std::cerr << "Failed to read optimized IR from child\n";
        return 1;
      }
      if (Read == 0)
        break;
      FromOpt += std::string(RecvBuf, Read);
    }

    int status;
    waitpid(pid, &status, 0);

    std::cerr << "Got IR from optimizer:\n" << FromOpt << "\n";
    // TODO: Proper cleanup
  } else {
    // child
    close(pipeP2C[1]); // write end
    close(pipeC2P[0]); // read end
    dup2(pipeP2C[0], STDIN_FILENO);
    dup2(pipeC2P[1], STDOUT_FILENO);
    // you should be able now to close the two remaining
    // pipe file desciptors as well as you dup'ed them already
    // (confirmed that it is working)
    close(pipeP2C[0]);
    close(pipeC2P[1]);

    char *const args[] = {strdup("optimizer"), strdup("-v"), strdup("-dump-ir"),
                          (char *)0};
    if (-1 == execvp(Optimizer, args))
      std::cerr << "execvp error: " << strerror(errno) << "\n";
  }
  std::cerr << "Finished cjit\n";
  return 0;
}
