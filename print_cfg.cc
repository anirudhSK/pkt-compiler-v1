#include "llvm/Pass.h"
#include "llvm/IR/Function.h"

using namespace llvm;

namespace {
  struct PrintCfg : public FunctionPass {
    static char ID;
    PrintCfg() : FunctionPass(ID) {}
    bool runOnFunction(Function & F __attribute__((unused))) override { return false; }
  };
}

char PrintCfg::ID = 0;
static RegisterPass<PrintCfg> X("print_cfg", "Print CFG of function", false, false);
