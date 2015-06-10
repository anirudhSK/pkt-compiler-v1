#ifndef CONTROL_DEPENDENCE_GRAPH_H_
#define CONTROL_DEPENDENCE_GRAPH_H_

#include <iostream>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "graph.h"

/// Retrieve control dependence graph from CFG
struct ControlDependenceGraph : public llvm::FunctionPass {
 public:
  static char ID;
  ControlDependenceGraph()
    : llvm::FunctionPass(ID) {}

  /// 1. Take augmented control flow graph with entry and exit nodes
  /// 2. Flip the graph
  /// 3. Compute postdom frontiers
  /// 4. Compute control dependence graph from postdom frontiers
  bool runOnFunction(llvm::Function & func) override;

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

  auto cdg() const { return cdg_; }
 private:
  Graph<const llvm::BasicBlock*> cdg_ = {};

  /// 1. Add two fake basic blocks: "entry" and "exit".
  /// 2. Append "exit" block to the block containing the return statement.
  /// We assume there's only one return, because this pass depends on mergereturn
  /// 3. Create dummy branch instruction from "entry" to
  ///    -> first block of actual code and
  ///    -> exit block created above
  /// N.B. Bolting on blocks like this can violate the SSA property causing
  /// a use to dominate an instruction. This is ok,
  /// because we 'll this code is never parsed; it's just used for analysis.
  Graph<const llvm::BasicBlock*> augment_cfg(const Graph<const llvm::BasicBlock*> & cfg, const llvm::BasicBlock * start_node);
};

#endif  // CONTROL_DEPENDENCE_GRAPH_H_
