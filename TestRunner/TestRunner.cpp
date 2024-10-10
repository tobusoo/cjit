#include "TestRunner.h"
#include "Benchmarks/Sink/SinkBenchmark.h"

#include <llvm/ExecutionEngine/Orc/DebugUtils.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ObjectTransformLayer.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>

#include <Optimizer.h>

#include <cassert>
#include <memory>
#include <numeric>

using namespace llvm;
using namespace llvm::orc;
using namespace tr;
using namespace opt;
namespace fs = std::filesystem;

extern cl::OptionCategory TestRunnerCat;

cl::opt<unsigned>
    IterationDuration("iter-duration",
                      cl::desc("Number of seconds to run each iteration"),
                      cl::cat(TestRunnerCat), cl::init(1));

cl::opt<unsigned>
    Iterations("iters", cl::desc("Number of iterations to run each benchmark"),
               cl::cat(TestRunnerCat), cl::init(10));

bool tr::initLLVM() {
  if (InitializeNativeTarget()) {
    errs() << "Failed to initialize native target\n";
    return false;
  }
  if (InitializeNativeTargetAsmPrinter()) {
    errs() << "Failed to initialize native target asm printer\n";
    return false;
  }
  if (InitializeNativeTargetAsmParser()) {
    errs() << "Failed to initialize native target asm parser\n";
    return false;
  }
  return true;
}

static std::unique_ptr<llvm::Module> parseIRFromFile(LLVMContext &Ctx,
                                                     std::filesystem::path P) {
  SMDiagnostic Err;
  auto TheModule = llvm::parseIRFile(P.string(), Err, Ctx);
  if (!TheModule) {
    errs() << "Failed to parse IR file " << P << ": ";
    Err.print("cjit", errs());
    return nullptr;
  }
  return TheModule;
}

static std::string dumpModuleToString(std::unique_ptr<Module> Mod) {
  std::string ModuleStr;
  llvm::raw_string_ostream OS(ModuleStr);
  Mod->print(OS, /*AAW=*/nullptr);
  return ModuleStr;
}

TestRunner::TestRunner(std::vector<fs::path> &&Benchmarks)
    : Benchmarks(std::move(Benchmarks)),
      JIT(ExitOnErr(LLJITBuilder().create())) {
  initRunners();
}

void TestRunner::initRunners() {
  for (auto &B : Benchmarks) {
    auto BenchName = B.filename().string();
    if (BenchName == "sink") {
      Runners["sink"] = std::make_unique<SinkBenchmark>(*JIT);
      continue;
    }
    errs() << "Runner not found for benchmark '" << BenchName << "'\n";
  }
}

bool TestRunner::run() {
  for (auto B : Benchmarks)
    if (!runBenchmark(B))
      return false;
  return true;
}

bool TestRunner::runBenchmark(std::filesystem::path BenchDir) {
  auto Context = std::make_unique<LLVMContext>();

  auto BenchName = BenchDir.filename().string();

  auto It = Runners.find(BenchName);
  if (It == Runners.end()) {
    errs() << "Don't know how to run benchmark '" << BenchName << "'...\n";
    return false;
  }

  auto &Runner = *It->getValue();

  auto IRName = BenchName + ".ll";
  auto IRPath = BenchDir / IRName;
  assert(fs::exists(IRPath) && "IR must exist");

  outs() << "Preparing to run benchmark '" << BenchName << "'...\n";
  auto BenchModule = parseIRFromFile(*Context, IRPath);
  if (!BenchModule)
    return false;

  BenchModule->setTargetTriple(sys::getProcessTriple());

  outs() << "Optimizing '" << BenchName << "'...\n";
  Optimizer Opt(*BenchModule);
  if (!Opt.optimizeIR()) {
    errs() << "Failed to optimize IR\n";
    return false;
  }

  Runner.init(std::move(BenchModule), std::move(Context));

  outs() << "Running '" << BenchName << "':\n";
  auto ScoreOpt = Runner.run(Iterations, IterationDuration);
  if (!ScoreOpt) {
    errs() << "Benchmark run failed.\n";
    return false;
  }
  outs() << "Average score: " << format("%15.1lf", *ScoreOpt) << " ops/s\n";
  return true;
}
