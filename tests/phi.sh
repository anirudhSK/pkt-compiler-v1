#! /bin/bash
set -e
clang -emit-llvm -c phi.c -o test.bc
opt -load ../.libs/libjayhawk.so -mem2reg -simplifycfg -instr_prog_deps test.bc -o /dev/null > test.output
diff test.output phi.expected_output
rm test.output
rm test.bc
