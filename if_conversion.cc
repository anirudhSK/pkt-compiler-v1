#include "if_conversion.h"
#include "llvm/Analysis/CFG.h"

using namespace llvm;

bool IfConversion::runOnFunction(Function & func) { 
  errs() << "IfConversion: ";
  errs().write_escaped(func.getName()) << '\n';
  for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    instr->print(errs());
    errs() << "\n";
  }

  // Check that there are no back edges
  SmallVector<std::pair<const BasicBlock *, const BasicBlock *>, 100> result;
  llvm::FindFunctionBackedges(func, result);
  if (result.size() > 0) {
    throw std::invalid_argument("Supplied function body has a loop\n");
  }

  // Now start at the first block and propagate path conditions
  bb_walk(& func.getEntryBlock());

  return false;
}

void IfConversion::bb_walk(const BasicBlock * bb, const Value * incoming_condition __attribute__ ((unused))) {
  for (unsigned int i = 0; i < bb->getTerminator()->getNumSuccessors(); i++) {
    bb_walk(bb->getTerminator()->getSuccessor(i));
  }
  errs() << "Finished bb_walk for bb " << *bb << "\n";
}

char IfConversion::ID = 0;
static RegisterPass<IfConversion> X("if_conversion", "Convert all branches into guarded statements.", false, false);
