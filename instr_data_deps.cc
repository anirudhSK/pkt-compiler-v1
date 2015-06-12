#include "instr_data_deps.h"
#include "graph.cc"

using namespace llvm;

bool InstrDataDeps::runOnFunction(Function &F) {
  for (auto instr = inst_begin(F); instr != inst_end(F); ++instr) {
    iddg_.add_node(&*instr);
  }

  for(auto instr = inst_begin(F); instr != inst_end(F); ++instr) {
    for(const auto & i : instr->users()){
      // From definition to use
      assert(isa<Instruction>(i));
      iddg_.add_edge(&*instr, dyn_cast<Instruction>(i));
    }
  }

  std::cout << "iddg_ is \n" << iddg_ << "\n";
  return false;
}

char InstrDataDeps::ID = 0;
static RegisterPass<InstrDataDeps> X("instr_data_deps", "Instruction-level data dependencies", false, false);
