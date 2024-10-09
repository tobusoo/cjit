#ifndef TESTRUNNER_TESTRUNNER
#define TESTRUNNER_TESTRUNNER

#include <filesystem>
#include <vector>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Error.h>

namespace tr {
bool initLLVM();

class TestRunner {
public:
  TestRunner(std::vector<std::filesystem::path> &&Benchmarks);

  bool run();

private:
  bool runBenchmark(std::filesystem::path BenchDir);

  std::vector<std::filesystem::path> Benchmarks;
  llvm::ExitOnError ExitOnErr;
  std::unique_ptr<llvm::orc::LLJIT> JIT;
};
} // namespace tr

#endif
