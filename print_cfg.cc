#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"

using namespace llvm;

namespace {
  struct PrintCfg : public FunctionPass {
    static char ID;
    PrintCfg() : FunctionPass(ID) {}
    bool runOnFunction(Function & func __attribute__((unused))) override {
      // Iterate over all basic blocks in the function
      for (auto it = func.begin(); it != func.end(); it++) {
        errs() << "Basic block: " << *it << "\n";
      }
      return false;
    }
  };
}

char PrintCfg::ID = 0;
static RegisterPass<PrintCfg> X("print_cfg", "Print CFG of function", false, false);
