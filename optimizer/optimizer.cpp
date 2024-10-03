#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <llvm/Support/SourceMgr.h>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <memory>

using namespace llvm;

extern "C" bool optimize(const char *ModuleSrc, const char **OptimizedModule,
                         size_t *Len) {
  assert(ModuleSrc && OptimizedModule && Len);

  auto Context = std::make_unique<LLVMContext>();
  SMDiagnostic Err;
  auto Buf = MemoryBuffer::getMemBuffer(StringRef(ModuleSrc));
  auto Module = llvm::parseIR(*Buf, Err, *Context);
  dbgs() << "Parsed module in optimizer:\n" << *Module << "\n";
  *OptimizedModule = "hello from optimizer\n";
  *Len = strlen(*OptimizedModule);
  return true;
}
