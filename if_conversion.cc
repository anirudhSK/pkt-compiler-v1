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

  return false;
}

char IfConversion::ID = 0;
static RegisterPass<IfConversion> X("if_conversion", "Convert all branches into guarded statements.", false, false);
