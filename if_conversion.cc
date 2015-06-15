#include "llvm/Pass.h"
#include "llvm/IR/Function.h"

using namespace llvm;

namespace {
  struct IfConversion : public FunctionPass {
    static char ID;
    IfConversion() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << "IfConversion: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char IfConversion::ID = 0;
static RegisterPass<Hello> X("if_conversion", "Convert all branches into guarded statements.", false, false);
