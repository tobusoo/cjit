#ifndef TESTRUNNER_BENCHMARKS_SINKBENCHMARK_H
#define TESTRUNNER_BENCHMARKS_SINKBENCHMARK_H

#include <cstdlib>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>

#include "../BenchRunner.h"

namespace tr {
class SinkBenchmark : public BenchRunner {
public:
    SinkBenchmark(llvm::orc::LLJIT& JIT) : BenchRunner(JIT)
    {
    }

    std::optional<double> run(unsigned NumIters, unsigned IterLength) override;
};
} // namespace tr

#endif // TESTRUNNER_BENCHMARKS_SINKBENCHMARK_H
