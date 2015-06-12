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
 public:
  static char ID;
  InstrDataDeps() : llvm::FunctionPass(ID) {}

  /// Compute data dependence graph from def-use chains
  bool runOnFunction(llvm::Function &F) override;

  const auto iddg() const { return iddg_; }
 private:
  // Instruction data dependence graph
  Graph<const llvm::Instruction*> iddg_  = Graph<const llvm::Instruction*>([] (const llvm::Instruction * instruction) {
      std::string str;
      llvm::raw_string_ostream rso(str);
      instruction->print(rso);
      return str;
  });
};

#endif  // INSTR_DATA_DEPS_H_
