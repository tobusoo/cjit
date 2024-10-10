#!/bin/sh

clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone sink.c
# TODO: Remove target triple and other stuff so that IR is platform independent.
