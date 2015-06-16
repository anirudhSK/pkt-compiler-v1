#include "if_conversion.h"
using namespace llvm;

bool IfConversion::runOnFunction(Function & func) { 
  errs() << "IfConversion: ";
  errs().write_escaped(func.getName()) << '\n';
  for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    instr->print(errs());
    errs() << "\n";
  }
  return false;
}

char IfConversion::ID = 0;
static RegisterPass<IfConversion> X("if_conversion", "Convert all branches into guarded statements.", false, false);
