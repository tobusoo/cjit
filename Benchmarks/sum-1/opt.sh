#!/bin/bash

OPT=~/llvm-project/build/bin/opt
PASSES=simplifycfg,sroa,early-cse,instcombine,loop-mssa\(loop-simplifycfg,loop-rotate,licm,indvars\),gvn,instcombine,simplifycfg,instcombine,loop-simplify,loop-vectorize,instcombine,simplifycfg

$OPT --passes=$PASSES sum-1.ll.init -S -o out.txt