#! /bin/bash
clang -emit-llvm -c phi.c -o test.bc
opt -load ../.libs/libjayhawk.so -mem2reg -instr_prog_deps test.bc
rm test.bc
