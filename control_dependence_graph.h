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
#include "add_entry_exit_nodes.h"
#include "graph.cc"
#include "dominator_utility.cc"

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
};

#endif  // CONTROL_DEPENDENCE_GRAPH_H_
