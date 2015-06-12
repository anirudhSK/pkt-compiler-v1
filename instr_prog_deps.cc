#include "instr_data_deps.h"
#include "instr_ctrl_deps.h"
#include "instr_prog_deps.h"
#include "graph.cc"

using namespace llvm;

bool InstrProgDeps::runOnFunction(Function &F __attribute__((unused))) {
  const auto iddg = getAnalysis<InstrDataDeps>().iddg();
  const auto icdg = getAnalysis<InstrCtrlDeps>().icdg();
  std::cout <<"Instruction-level prog. dep gh \n" << (iddg + icdg) << "\n";
  return false;
}

void InstrProgDeps::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequired<InstrDataDeps>();
  AU.addRequired<InstrCtrlDeps>();
}

char InstrProgDeps::ID = 0;
static RegisterPass<InstrProgDeps> X("instr_prog_deps", "Compute all dependencies, both control and data for a function at the instruction level", false, false);
