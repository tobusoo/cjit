#ifndef TESTRUNNER_BENCHMARKS_BENCHRUNNER_H
#define TESTRUNNER_BENCHMARKS_BENCHRUNNER_H

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <filesystem>
#include <optional>

namespace tr {
class BenchRunner {
public:
  BenchRunner(llvm::orc::LLJIT &JIT) : JIT(JIT) {}

  void init(std::unique_ptr<llvm::Module> TheModule,
            std::unique_ptr<llvm::LLVMContext> Ctx) {
    this->TheModule = std::move(TheModule);
    this->Ctx = std::move(Ctx);
  }

  virtual std::optional<double> run(unsigned NumIters, unsigned IterLength) = 0;

  virtual ~BenchRunner() = default;

protected:
  double runIter(unsigned IterNum, unsigned Duration,
                 llvm::function_ref<bool()> IterJob);

  std::unique_ptr<llvm::Module> TheModule;
  std::unique_ptr<llvm::LLVMContext> Ctx;
  llvm::orc::LLJIT &JIT;
};

template <typename RetTy, typename... ArgsTys> class FunctionExecutor {
private:
  using FuncTy = RetTy(ArgsTys...);

public:
  FunctionExecutor(std::unique_ptr<llvm::Module> Module,
                   std::unique_ptr<llvm::LLVMContext> Ctx,
                   llvm::orc::LLJIT &JIT, llvm::StringRef FuncName)
      : JIT(JIT) {
    llvm::orc::ThreadSafeModule TSM(std::move(Module), std::move(Ctx));
    ExitOnErr(JIT.addIRModule(std::move(TSM)));
    auto FuncAddr = ExitOnErr(JIT.lookup(FuncName));
    Func = FuncAddr.template toPtr<FuncTy>();
  }

  template <typename... ActualArgsTys> RetTy execute(ActualArgsTys &&...Args) {
    return Func(std::forward<ActualArgsTys>(Args)...);
  }

private:
  llvm::orc::LLJIT &JIT;
  llvm::ExitOnError ExitOnErr;
  FuncTy *Func;
};
} // namespace tr

#endif // TESTRUNNER_BENCHMARKS_BENCHRUNNER_H
