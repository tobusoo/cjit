#ifndef OPTIMIZER_OPTIMIZER_H
#define OPTIMIZER_OPTIMIZER_H

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Debug.h>
#include <llvm/Target/TargetMachine.h>
#include <memory>

namespace opt {
class Optimizer {
public:
  Optimizer(llvm::Module &M) : TheModule(M), Ctx(M.getContext()) {}

  bool optimizeIR();

  void printModule(llvm::raw_ostream &OS = llvm::errs());

private:
  bool createTargetMachine();

  bool dumpIR(llvm::StringRef Suffix);

  llvm::Module &TheModule;
  llvm::LLVMContext &Ctx;
  std::unique_ptr<llvm::TargetMachine> TheTargetMachine;
};
} // namespace opt

#endif // OPTIMIZER_OPTIMIZER_H
