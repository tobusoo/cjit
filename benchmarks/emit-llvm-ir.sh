#!/bin/sh

/opt/homebrew/opt/llvm/bin/clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone sink.c
# REMOVE TARGET TRIPLE