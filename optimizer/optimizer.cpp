#include "Optimizer.h"

#include "llvm/ADT/Twine.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/SROA.h>

#include <optional>
#include <string>

using namespace llvm;

#define SUCCESS 0
#define ERR_INCORRECT_ARGS 1
#define ERR_FILE_DOESNT_EXIST 2
#define ERR_INITIAL_IR_PARSE_FAILURE 3
#define ERR_LLVM_INIT 4
#define ERR_OPT 5

static cl::opt<bool> Verbose("verbose", cl::init(false));

static cl::alias VerboseAlias("v", cl::desc("alias for --verbose"),
                              cl::aliasopt(Verbose));

static cl::opt<bool> DumpIR("dump-ir", cl::init(false));

static cl::opt<bool>
    DebugPM("debug-pass-manager", cl::Hidden,
            cl::desc("Print pass management debugging information"),
            cl::init(false));

bool Optimizer::optimizeIR() {
  if (!createTargetMachine())
    return false;

  if (DumpIR && !dumpIR("init.ll"))
    return false;

  PipelineTuningOptions PTO;

  LoopAnalysisManager LAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGAM;
  ModuleAnalysisManager MAM;

  PassInstrumentationCallbacks PIC;
  PrintPassOptions PrintPassOpts;

  StandardInstrumentations SI(Ctx, /*DebugLogging=*/DebugPM,
                              /*VerifyEach=*/false, PrintPassOpts);
  SI.registerCallbacks(PIC, &MAM);
  PassBuilder PB(&*TheTargetMachine, PTO, /*PGOOpt=*/std::nullopt, &PIC);

  // Register all the basic analyses with the managers.
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  /// TODO: Extend pipeline here.
  ModulePassManager MPM;
  MPM.addPass(VerifierPass());

  {
    FunctionPassManager FPM;
    FPM.addPass(SROAPass(SROAOptions::ModifyCFG));
    FPM.addPass(InstCombinePass());
    MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
  }

  if (Verbose)
    errs() << "Running optimization pipeline on module...\n";

  MPM.run(TheModule, MAM);

  if (Verbose)
    errs() << "Successfully ran optimization pipeline on module\n";

  if (DumpIR && !dumpIR("opt.ll"))
    return false;
  return true;
}

bool Optimizer::createTargetMachine() {
  auto TargetTriple = TheModule.getTargetTriple();

  std::string Error;
  auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

  // Print an error and exit if we couldn't find the requested target.
  // This generally occurs if we've forgotten to initialise the
  // TargetRegistry or we have a bogus target triple.
  if (!Target) {
    errs() << "Failed to find target with target triple " << TargetTriple
           << ": " << Error << "\n";
    return false;
  }

  /// TODO: Figure out CPU and features for the current processor.
  auto CPU = "generic";
  auto Features = "";

  TargetOptions TO;
  auto *TM =
      Target->createTargetMachine(TargetTriple, CPU, Features, TO, Reloc::PIC_);
  if (!TM) {
    errs() << "Failed to create target machine\n";
    return false;
  }
  TheTargetMachine.reset(TM);
  return true;
}

void Optimizer::printModule(llvm::raw_ostream &OS) {
  TheModule.print(OS, /*AAW=*/nullptr);
}

bool Optimizer::dumpIR(StringRef Suffix) {
  std::string FileName = (TheModule.getName().str() + "." + Suffix).str();
  std::error_code EC;
  llvm::raw_fd_ostream OS(FileName, EC);
  if (EC) {
    errs() << "Failed to dump " << Suffix << " to " << FileName << ": "
           << EC.message() << "\n";
    return false;
  }
  printModule(OS);
  errs() << " -- " << FileName << "\n";
  return true;
}
