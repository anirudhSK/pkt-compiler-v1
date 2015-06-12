#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "graph.cc"
#include "instr_ctrl_deps.h"

using namespace llvm;

bool InstrCtrlDeps::runOnFunction(Function & func) {
  // Get all instructions.
  for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    if (isa<TerminatorInst>(&*instr)) {
      assert(isa<ReturnInst>(&*instr) or isa<BranchInst>(&*instr));
      // Only add return inst, TODO: At some future point return void
      if (isa<ReturnInst>(&*instr)) {
        icdg_.add_node(&*instr);
      }
    } else {
      icdg_.add_node(&*instr);
    }
  }

  // Get control dependence graph of basic blocks
  const auto cdg = getAnalysis<ControlDependenceGraph>().cdg();

  for (const auto instr_a : icdg_.node_set()) {
    for (const auto instr_b : icdg_.node_set()) {
      if (cdg.exists_edge(instr_a->getParent(), instr_b->getParent())) {
        icdg_.add_edge(instr_a, instr_b);
      }
    }
  }

  std::cout << "icdg_ is \n" << icdg_ << "\n";
  return false;
}

void InstrCtrlDeps::getAnalysisUsage(AnalysisUsage & AU) const {
  AU.setPreservesAll();
  AU.addRequired<ControlDependenceGraph>();
}

char InstrCtrlDeps::ID = 0;
static RegisterPass<InstrCtrlDeps> X("instr_ctrl_deps", "Compute control dependence graph at instruction-level", false, false);
