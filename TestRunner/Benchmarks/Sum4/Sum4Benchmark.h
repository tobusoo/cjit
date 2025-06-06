#ifndef TESTRUNNER_BENCHMARKS_SUM4BENCHMARK_H
#define TESTRUNNER_BENCHMARKS_SUM4BENCHMARK_H

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>

#include "../BenchRunner.h"

namespace tr {
class Sum4Benchmark : public BenchRunner {
public:
    Sum4Benchmark(llvm::orc::LLJIT& JIT) : BenchRunner(JIT)
    {
    }

    std::optional<double> run(unsigned NumIters, unsigned IterLength) override;
};
} // namespace tr

#endif // TESTRUNNER_BENCHMARKS_SUM4BENCHMARK_H
