#include "control_dependence_graph.h"
#include "graph.cc"
#include "dominator_utility.cc"

using namespace llvm;

bool ControlDependenceGraph::runOnFunction(Function & func) {
  // Augmented control flow graph with entry and exit nodes
  Graph<const BasicBlock*> augmented_cfg([] (const BasicBlock * basic_block) {
      std::string str;
      llvm::raw_string_ostream rso(str);
      basic_block->printAsOperand(rso);
      return str;
  });

  // First pass: Just get nodes alone
  for (auto it = func.begin(); it != func.end(); it++) {
    augmented_cfg.add_node(it);
  }

  // Second pass: Now add edges
  for (auto it = func.begin(); it != func.end(); it++) {
    for (unsigned int i = 0; i < it->getTerminator()->getNumSuccessors(); i++) {
      augmented_cfg.add_edge(it, (it->getTerminator()->getSuccessor(i)));
    }
  }

  // Flip graph
  auto flipped_cfg = augmented_cfg.transpose();

  // Get pointer to exit node
  const BasicBlock* exit_node;
  for (auto it = func.begin(); it != func.end(); it++) {
    if (it->getName() == "exit") {
      exit_node = it;
    }
  }

  // Get post dominance frontier
  auto postdom_frontier = DominatorUtility<const BasicBlock*>(flipped_cfg,
                                                              exit_node).dominance_frontier();

  // Get control dependence graph
  cdg_ = flipped_cfg.copy_and_clear();
  for (const auto & y : postdom_frontier) {
    for (const auto & x : y.second) {
      // Item # 5 on page 426 of Appel's book
      cdg_.add_edge(x, y.first);
    }
  }

  std::cout << "Control dependence graph \n" << cdg_ << "\n";
  return false;
}

void ControlDependenceGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequired<AddEntryExitNodes>();
}

char ControlDependenceGraph::ID = 0;
static RegisterPass<ControlDependenceGraph> X("control_dependence_graph", "Compute control dependence graph of a function", false, false);
