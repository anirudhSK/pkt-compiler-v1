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
 public:
  static char ID;
  InstrProgDeps() : llvm::FunctionPass(ID) {}

  /// Unify control and data dependencies
  bool runOnFunction(llvm::Function &F) override;

  /// Specify that UnifyFunctionExitNodes
  /// is a pre-requisite for this pass because
  /// we assume there's exact one return when implementing
  /// get_block_ctrl_dep()
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

 private:
  /// Get instruction-level data dependence graph
  /// by using the def-use chains that are already
  /// available as part of the LLVM IR
  /// SSA makes def-use almost trivial
  auto get_instr_data_dep(const llvm::Function & func) const;

  /// 1. Add two fake basic blocks: "entry" and "exit".
  /// 2. Append "exit" block to the block containing the return statement.
  /// We assume there's only one return, because this pass depends on mergereturn
  /// 3. Create dummy branch instruction from "entry" to
  ///    -> first block of actual code and
  ///    -> exit block created above
  /// N.B. Bolting on blocks like this can violate the SSA property causing
  /// a use to dominate an instruction. This is ok,
  /// because we 'll this code is never parsed; it's just used for analysis.
  auto augment_cfg(const Graph<const llvm::BasicBlock*> & cfg, const llvm::BasicBlock * start_node) const;

  /// Get block-level control dependnece graph
  /// 1. Take augmented control flow graph with entry and exit nodes
  /// 2. Flip the graph
  /// 3. Compute postdom frontiers
  /// 4. Compute control dependence graph from postdom frontiers
  auto get_block_ctrl_dep(const llvm::Function & func) const;

  /// Lower control dependences to the level of instructions.
  /// Instruction A is control dependent on Instruction B if its
  /// enclosing basic block BB{A} is control dependent on B's
  /// enclosing basic block BB{B}. 
  auto get_instr_ctrl_dep(const llvm::Function & func) const;
};

#endif  // INSTR_PROG_DEPS_H_
