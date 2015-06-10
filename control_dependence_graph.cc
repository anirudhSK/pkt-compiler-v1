#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "control_dependence_graph.h"
#include "graph.cc"
#include "dominator_utility.cc"

using namespace llvm;

bool ControlDependenceGraph::runOnFunction(Function & func) {
  // Setup control flow graph container
  Graph<const BasicBlock*> cfg([] (const BasicBlock * basic_block) {
      std::string str;
      llvm::raw_string_ostream rso(str);
      basic_block->printAsOperand(rso);
      return str;
  });

  // First pass: Just get nodes alone
  for (auto it = func.begin(); it != func.end(); it++) {
    cfg.add_node(it);
  }

  // Second pass: Now add edges
  for (auto it = func.begin(); it != func.end(); it++) {
    for (unsigned int i = 0; i < it->getTerminator()->getNumSuccessors(); i++) {
      cfg.add_edge(it, (it->getTerminator()->getSuccessor(i)));
    }
  }

  // Augment with entry and exit
  auto augmented_cfg = augment_cfg(cfg, func.begin());

  // Flip graph
  auto flipped_cfg = augmented_cfg.transpose();

  // Get pointer to exit node
  const BasicBlock* exit_node = nullptr;
  for (const auto basic_block : augmented_cfg.node_set()) {
    if (basic_block->getName() == "exit") {
      exit_node = basic_block;
    }
  }
  assert(exit_node != nullptr);

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
  AU.addRequired<UnifyFunctionExitNodes>();
}

Graph<const BasicBlock*> ControlDependenceGraph::augment_cfg(const Graph<const BasicBlock*> & cfg, const BasicBlock * start_node) {
  // Step 1: Create entry and exit blocks
  const BasicBlock * entry_block = BasicBlock::Create(getGlobalContext(), "entry");
  const BasicBlock * exit_block  = BasicBlock::Create(getGlobalContext(), "exit");

  // Step 1.1: Add these nodes the the augmented cfg
  auto augmented_cfg(cfg);
  augmented_cfg.add_node(entry_block);
  augmented_cfg.add_node(exit_block);

  // Step 2.1: Get all return blocks
  std::vector<const BasicBlock*> return_blocks;
  for (const auto basic_block : cfg.node_set()) {
    // Every node should have terminators
    assert (basic_block->getTerminator() != nullptr);
    if (isa<ReturnInst>(basic_block->getTerminator())) {
      return_blocks.emplace_back(basic_block);
    }
  }

  // Step 2.2: Check that we have exactly one because
  // mergereturn should have run before this
  assert(return_blocks.size() == 1);

  // Step 3.1: Connect entry to start_node and exit blocks
  augmented_cfg.add_edge(entry_block, start_node);
  augmented_cfg.add_edge(entry_block, exit_block);

  // Step 3.2: Connect return_blocks.front() to exit block
  augmented_cfg.add_edge(return_blocks.front(), exit_block);

  return augmented_cfg;
}

char ControlDependenceGraph::ID = 0;
static RegisterPass<ControlDependenceGraph> X("control_dependence_graph", "Compute control dependence graph of a function", false, false);
