#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace {
  struct DefUse : public FunctionPass {
    static char ID;
    DefUse() : FunctionPass(ID) {}
    bool runOnFunction(Function &F) override {
      for(auto instr = inst_begin(F); instr != inst_end(F); ++instr) {
        errs() << "def: [ " << *instr << " ] uses: ";
        for(const auto & i : instr->users()){
          errs() << " [ " << *i << " ] ";
        }
        errs() << "\n";
      }
      return false;
    }
  };
}

char DefUse::ID = 0;
static RegisterPass<DefUse> X("def_use", "Def-Use Chain pass", false, false);
