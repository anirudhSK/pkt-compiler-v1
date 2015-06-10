#include "instr_prog_deps.h"

using namespace llvm;

bool InstrProgDeps::runOnFunction(Function &F __attribute__((unused))) {
  return false;
}

char InstrProgDeps::ID = 0;
static RegisterPass<InstrProgDeps> X("instr_prog_deps", "Compute all dependencies, both control and data for a function at the instruction level", false, false);
