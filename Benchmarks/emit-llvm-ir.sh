#!/bin/bash

clang -S -emit-llvm -Xclang -disable-llvm-passes -o - $1 > ${1%.c}.ll
