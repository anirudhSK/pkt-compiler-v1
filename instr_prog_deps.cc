#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "utility_functions.h"
#include "instr_prog_deps.h"
#include "graph.cc"
#include "dominator_utility.cc"

using namespace llvm;

auto InstrProgDeps::get_all_non_branch_inst(const Function & func) const {
  std::vector<const Instruction*> ret;
  for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    if (isa<TerminatorInst>(&*instr)) {
      assert(isa<ReturnInst>(&*instr) or isa<BranchInst>(&*instr));
      // Only add return inst, TODO: At some future point return void
      if (isa<ReturnInst>(&*instr)) {
        ret.emplace_back(&*instr);
      }
    } else {
      ret.emplace_back(&*instr);
    }
  }
  return ret;
}

auto InstrProgDeps::get_instr_data_dep(const Function & func) const {
  // Instruction-level data dependence graph
  Graph<const llvm::Instruction*> iddg(instr_printer);

  for (auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    iddg.add_node(&*instr);
  }

  for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    for(const auto & i : instr->users()){
      // From definition to use
      assert(isa<Instruction>(i));
      iddg.add_edge(&*instr, dyn_cast<Instruction>(i));
    }
  }

  std::cout << "iddg is \n" << iddg << "\n";
  return iddg;
}

auto InstrProgDeps::augment_cfg(const Graph<const BasicBlock*> & cfg, const BasicBlock * start_node) const {
  // Step 1: Create entry and exit blocks
  const auto * entry_block = BasicBlock::Create(getGlobalContext(), "entry");
  const auto * exit_block  = BasicBlock::Create(getGlobalContext(), "exit");

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


auto InstrProgDeps::get_block_ctrl_dep(const Function & func) const {
  // Setup control flow graph container
  Graph<const BasicBlock*> cfg(bb_printer);

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
  auto cdg = flipped_cfg.copy_and_clear();
  for (const auto & y : postdom_frontier) {
    for (const auto & x : y.second) {
      // Item # 5 on page 426 of Appel's book
      cdg.add_edge(x, y.first);
    }
  }

  std::cout << "Control dependence graph \n" << cdg << "\n";
  return cdg;
}


auto InstrProgDeps::get_instr_ctrl_dep(const Function & func) const {
  // Instruction-level control dependence graph
  Graph<const Instruction*> icdg(instr_printer);

  // Get all non-branch instructions.
  for (const auto & inst : get_all_non_branch_inst(func)) {
    icdg.add_node(inst);
  }

  // Get control dependence graph of basic blocks
  const auto cdg = get_block_ctrl_dep(func);

  for (const auto instr_a : icdg.node_set()) {
    for (const auto instr_b : icdg.node_set()) {
      if (cdg.exists_edge(instr_a->getParent(), instr_b->getParent())) {
        icdg.add_edge(instr_a, instr_b);
      }
    }
  }

  std::cout << "icdg is \n" << icdg << "\n";
  return icdg;
}

bool InstrProgDeps::runOnFunction(Function & func) {
  std::cout <<"Instruction-level prog. dep gh \n" << (get_instr_ctrl_dep(func) + get_instr_data_dep(func)) << "\n";
  return false;
}

void InstrProgDeps::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequired<UnifyFunctionExitNodes>();
}

char InstrProgDeps::ID = 0;
static RegisterPass<InstrProgDeps> X("instr_prog_deps", "Compute all dependencies, both control and data for a function at the instruction level", false, false);
