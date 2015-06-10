#ifndef INSTR_DATA_DEPS_H_
#define INSTR_DATA_DEPS_H_

#include <iostream>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "graph.h"

/// LLVM pass to compute instruction data dependence
/// With the SSA form this is simple because there is
/// exactly one definition for each variable and def-use
/// chains are part of the LLVM API
struct InstrDataDeps : public llvm::FunctionPass {
  static char ID;
  InstrDataDeps() : llvm::FunctionPass(ID) {}
  bool runOnFunction(llvm::Function &F) override;
};

#endif  // INSTR_DATA_DEPS_H_
