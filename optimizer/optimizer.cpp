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
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/SROA.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

using namespace llvm;

#define SUCCESS 0
#define ERR_INCORRECT_ARGS 1
#define ERR_FILE_DOESNT_EXIST 2
#define ERR_INITIAL_IR_PARSE_FAILURE 3
#define ERR_LLVM_INIT 4
#define ERR_OPT 5

static cl::OptionCategory Cat("optimizer Options");

static cl::opt<std::string> InputFile(cl::Positional, cl::desc("Input IR file"),
                                      cl::cat(Cat), cl::Optional);

static cl::opt<bool> Verbose("verbose", cl::cat(Cat), cl::init(false));

static cl::alias VerboseAlias("v", cl::desc("alias for --verbose"),
                              cl::aliasopt(Verbose));

static cl::opt<bool> DumpIR("dump-ir", cl::cat(Cat), cl::init(false));

static cl::opt<bool>
    DebugPM("debug-pass-manager", cl::Hidden,
            cl::desc("Print pass management debugging information"),
            cl::init(false));

struct OptimizerOpts {
  bool Verbose = false;
  bool DumpIRToFiles = false;
  bool DebugPM = false;
};

class Optimizer {
public:
  Optimizer(OptimizerOpts Opts) : Opts(std::move(Opts)) {}

  static bool initLLVM();

  bool parseIR(std::istream &IS);

  bool optimizeIR();

  void printModule(llvm::raw_ostream &OS = errs());

private:
  bool createTargetMachine();

  bool dumpIR(StringRef Suffix);

  OptimizerOpts Opts;
  LLVMContext Context;
  std::unique_ptr<MemoryBuffer> InitialIR;
  std::unique_ptr<Module> TheModule;
  std::unique_ptr<TargetMachine> TheTargetMachine;
};

bool Optimizer::initLLVM() {
  if (InitializeNativeTarget()) {
    errs() << "Failed to initialize native target\n";
    return false;
  }
  if (InitializeNativeTargetAsmPrinter()) {
    errs() << "Failed to initialize native target\n";
    return false;
  }
  if (InitializeNativeTargetAsmParser()) {
    errs() << "Failed to initialize native target\n";
    return false;
  }
  return true;
}

bool Optimizer::parseIR(std::istream &IS) {
  assert(IS.good());

  std::string InitialIRStr;

  std::string Input;
  while (std::getline(IS, Input))
    InitialIRStr += Input + "\n";

  SMDiagnostic Err;
  InitialIR = MemoryBuffer::getMemBuffer(InitialIRStr);
  TheModule = llvm::parseIR(*InitialIR, Err, Context);
  if (!TheModule) {
    errs() << "Failed to parse initial IR: ";
    Err.print("", errs());
    return false;
  }

  if (Opts.Verbose) {
    errs() << "Successfully parsed module\n";
    printModule();
  }

  if (Opts.DumpIRToFiles)
    if (!dumpIR("init.ll"))
      return false;

  return true;
}

bool Optimizer::optimizeIR() {
  if (!createTargetMachine())
    return false;

  PipelineTuningOptions PTO;

  LoopAnalysisManager LAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGAM;
  ModuleAnalysisManager MAM;

  PassInstrumentationCallbacks PIC;
  PrintPassOptions PrintPassOpts;

  StandardInstrumentations SI(Context, /*DebugLogging=*/Opts.DebugPM,
                              /*VerifyEach=*/false, PrintPassOpts);
  SI.registerCallbacks(PIC, &MAM);
  PassBuilder PB(&*TheTargetMachine, PTO, /*PGOOpt=*/std::nullopt, &PIC);

  // Register all the basic analyses with the managers.
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  ModulePassManager MPM;
  MPM.addPass(VerifierPass());

  {
    FunctionPassManager FPM;
    FPM.addPass(SROAPass(SROAOptions::ModifyCFG));
    FPM.addPass(InstCombinePass());
    MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
  }

  MPM.run(*TheModule, MAM);

  if (Opts.Verbose) {
    errs() << "Successfully ran optimization pipeline on module\n";
    printModule();
  }

  if (Opts.DumpIRToFiles)
    if (!dumpIR("opt.ll"))
      return false;
  return true;
}

bool Optimizer::createTargetMachine() {
  auto TargetTriple = sys::getDefaultTargetTriple();
  TheModule->setTargetTriple(TargetTriple);

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

  auto CPU = "generic";
  auto Features = "";

  TargetOptions TO;
  auto TheTargetMachine =
      Target->createTargetMachine(TargetTriple, CPU, Features, TO, Reloc::PIC_);
  TheModule->setDataLayout(TheTargetMachine->createDataLayout());
  return true;
}

void Optimizer::printModule(llvm::raw_ostream &OS) {
  TheModule->print(OS, /*AAW=*/nullptr);
}

bool Optimizer::dumpIR(StringRef Suffix) {
  std::string FileName = (TheModule->getName().str() + "." + Suffix).str();
  std::error_code EC;
  llvm::raw_fd_ostream OS(FileName, EC);
  if (EC) {
    errs() << "Failed to dump " << Suffix << " to " << FileName << "\n";
    return false;
  }
  printModule(OS);
  errs() << " -- " << FileName << "\n";
  return true;
}

int main(int argc, char **argv) {
  std::string Errors;
  llvm::raw_string_ostream ErrS(Errors);
  if (!cl::ParseCommandLineOptions(argc, argv, "", &ErrS)) {
    errs() << "Failed to parse args: " << Errors << "\n";
    return ERR_INCORRECT_ARGS;
  }

  OptimizerOpts Opts;
  Opts.DumpIRToFiles = DumpIR;
  Opts.Verbose = Verbose;
  Opts.DebugPM = DebugPM;

  Optimizer Opt(Opts);
  if (!Opt.initLLVM()) {
    errs() << "Failed to initialize LLVM. See error messages above.\n";
    return ERR_LLVM_INIT;
  }

  bool Parsed = false;
  if (InputFile.getNumOccurrences()) {
    std::ifstream InputIRFile;
    InputIRFile.open(InputFile);
    if (!InputIRFile.is_open()) {
      errs() << "Failed to open " << InputFile << "\n";
      return ERR_FILE_DOESNT_EXIST;
    }
    Parsed = Opt.parseIR(InputIRFile);
  } else
    Parsed = Opt.parseIR(std::cin);

  if (!Parsed)
    return ERR_INITIAL_IR_PARSE_FAILURE;

  if (!Opt.optimizeIR()) {
    errs() << "Failed to optimize IR\n";
    return ERR_OPT;
  }

  return SUCCESS;
}
