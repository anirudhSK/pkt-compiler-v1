#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "graph.cc"
#include "instr_ctrl_deps.h"

using namespace llvm;

bool InstrCtrlDeps::runOnFunction(Function & func) {
  // Instruction Control Dependence Graph
  Graph<Instruction*> icdg([] (const Instruction * instruction) {
      std::string str;
      llvm::raw_string_ostream rso(str);
      instruction->print(rso);
      return str;
  });

  // Get all meaningful instructions. Ignore entry and exit blocks
  for (Function::iterator bb = func.begin(); bb != func.end(); ++bb) {
    if (bb->getName() == "entry" or bb->getName() == "exit") {
      continue;
    } else {
      for (BasicBlock::iterator i = bb->begin(); i != bb->end(); ++i) {
        if (isa<TerminatorInst>(i)) {
          assert(isa<ReturnInst>(i) or isa<BranchInst>(i));
          // Only add return inst, TODO: At some future point return void
          if (isa<ReturnInst>(i)) {
            icdg.add_node(&*i);
          }
        } else {
          icdg.add_node(&*i);
        }
      }
    }
  }

  // Get control dependence graph of basic blocks
  const auto cdg = getAnalysis<ControlDependenceGraph>().cdg();

  for (const auto instr_a : icdg.node_set()) {
    for (const auto instr_b : icdg.node_set()) {
      if (cdg.exists_edge(instr_a->getParent(), instr_b->getParent())) {
        icdg.add_edge(instr_a, instr_b);
      }
    }
  }

  std::cout << "icdg is \n" << icdg << "\n";
  return false;
}

void InstrCtrlDeps::getAnalysisUsage(AnalysisUsage & AU) const {
  AU.setPreservesAll();
  AU.addRequired<ControlDependenceGraph>();
}

char InstrCtrlDeps::ID = 0;
static RegisterPass<InstrCtrlDeps> X("instr_ctrl_deps", "Compute control dependence graph at instruction-level", false, false);
