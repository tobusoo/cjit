#include "TestRunner.h"

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

template <typename RetTy, typename... ArgsTys> class FunctionExecutor {
private:
  using FuncTy = RetTy(ArgsTys...);

public:
  FunctionExecutor(std::unique_ptr<Module> Module,
                   std::unique_ptr<LLVMContext> Ctx, LLJIT &JIT,
                   ExitOnError &ExitOnErr, StringRef FuncName)
      : JIT(JIT), ExitOnErr(ExitOnErr) {
    ThreadSafeModule TSM(std::move(Module), std::move(Ctx));
    ExitOnErr(JIT.addIRModule(std::move(TSM)));
    auto FuncAddr = ExitOnErr(JIT.lookup(FuncName));
    Func = FuncAddr.toPtr<FuncTy>();
  }

  template <typename... ActualArgsTys> RetTy execute(ActualArgsTys &&...Args);

private:
  LLJIT &JIT;
  ExitOnError &ExitOnErr;
  FuncTy *Func;
};

template <typename RetTy, typename... ArgsTys>
template <typename... ActualArgsTys>
RetTy FunctionExecutor<RetTy, ArgsTys...>::execute(ActualArgsTys &&...Args) {
  return Func(std::forward<ActualArgsTys>(Args)...);
}

TestRunner::TestRunner(std::vector<fs::path> &&Benchmarks)
    : Benchmarks(std::move(Benchmarks)),
      JIT(ExitOnErr(LLJITBuilder().create())) {}

bool TestRunner::run() {
  for (auto B : Benchmarks)
    if (!runBenchmark(B))
      return false;
  return true;
}

bool TestRunner::runBenchmark(std::filesystem::path BenchDir) {
  auto Context = std::make_unique<LLVMContext>();

  auto BenchName = BenchDir.filename().string();
  auto IRName = BenchName + ".ll";
  auto IRPath = BenchDir / IRName;
  assert(fs::exists(IRPath) && "IR must exist");

  errs() << "Preparing to run benchmark '" << BenchName << "'\n";
  auto BenchModule = parseIRFromFile(*Context, IRPath);
  if (!BenchModule)
    return false;

  BenchModule->setTargetTriple(sys::getProcessTriple());

  Optimizer Opt(*BenchModule);
  if (!Opt.optimizeIR()) {
    errs() << "Failed to optimize IR\n";
    return false;
  }

  FunctionExecutor<double, int *, unsigned> Exec(
      std::move(BenchModule), std::move(Context), *JIT, ExitOnErr, "test");

  auto RunIter = [&](unsigned IterNum) {
    errs() << "Iteration #" << IterNum << ": ";
    using ClockTy = std::chrono::high_resolution_clock;
    auto TimeLimit = std::chrono::seconds(IterationDuration);
    auto TimeLimitS =
        std::chrono::duration_cast<std::chrono::seconds>(TimeLimit).count();

    std::size_t NumTimesCalled = 0;
    auto Start = ClockTy::now();

    int arr[] = {5,  15, 3, 20, 8,  12, 25, 30, 2,  7,
                 18, 14, 9, 11, 16, 22, 4,  27, 19, 6}; // Larger input array
    int n = 20; // Number of elements in the array

    if (IterNum % 2)
      for (int i = 0; i < n; i++)
        arr[i] *= -1;

    while (true) {
      NumTimesCalled++;
      double Result = Exec.execute(arr, n);
      // if (Result != 12502) {
      //   errs() << "Got wrong result at iteration #" << NumTimesCalled
      //          << ". Expected 85, got " << Result << "\n";
      //   return 1;
      // }
      if (ClockTy::now() - Start >= TimeLimit)
        break;
    }
    double Score = static_cast<double>(NumTimesCalled) / TimeLimitS;
    outs() << format("%15.1lf", Score) << " ops/s\n";
    return Score;
  };

  outs() << "Executing your code for " << Iterations << " iterations, each for "
         << IterationDuration << " seconds.\n";

  std::vector<double> Scores;
  for (unsigned I = 0; I < Iterations; ++I)
    Scores.push_back(RunIter(I));
  double Avg =
      std::accumulate(Scores.begin(), Scores.end(), 0.0) / Scores.size();
  outs() << "Average score: " << format("%15.1lf", Avg) << " ops/s.\n";
  return true;
}
