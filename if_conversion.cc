#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;

namespace {
  struct IfConversion : public FunctionPass {
    static char ID;
    IfConversion() : FunctionPass(ID) {}

    bool runOnFunction(Function & func) override {
      errs() << "IfConversion: ";
      errs().write_escaped(func.getName()) << '\n';
      for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
        instr->print(errs());
        errs() << "\n";
      }
      return false;
    }
  };
}

char IfConversion::ID = 0;
static RegisterPass<IfConversion> X("if_conversion", "Convert all branches into guarded statements.", false, false);
