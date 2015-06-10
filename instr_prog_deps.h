#ifndef INSTR_PROG_DEPS_H_
#define INSTR_PROG_DEPS_H_

#include <iostream>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "graph.h"

/// LLVM pass to unify data dependencies and
/// control dependencies for a program into one
/// unified set of dependencies. This is similar
/// to Ferrante's program dependence graph, at the
/// instruction level.
/// http://www.cc.gatech.edu/~harrold/6340/cs6340_fall2009/Readings/ferrante87jul.pdf
struct InstrProgDeps : public llvm::FunctionPass {
  static char ID;
  InstrProgDeps() : llvm::FunctionPass(ID) {}
  bool runOnFunction(llvm::Function &F) override;
};

#endif  // INSTR_PROG_DEPS_H_
