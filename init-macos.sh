#!/bin/sh

cmake -S . -B build -G Ninja -DClang_DIR=/opt/homebrew/opt/llvm/lib/cmake/clang -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
