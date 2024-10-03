#!/bin/sh

clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone simple.c
