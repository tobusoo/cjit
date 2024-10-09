#!/bin/sh

cmake -S . -B build -G Ninja \
    -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm/bin/clang \
    -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1
