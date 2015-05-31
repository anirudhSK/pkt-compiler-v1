#include <iostream>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/Analysis/PostDominators.h"

using namespace llvm;

namespace {
  /// Post-dominance frontiers, required for the control-dependence graph
  struct PostDominanceFrontiers : public FunctionPass {
   public:
    static char ID;
    PostDominanceFrontiers()
      : FunctionPass(ID),
        dominance_frontiers_() {}

    bool runOnFunction(Function &) override {
      dominance_frontiers_.calculate(*getAnalysis<PostDominatorTree>().DT,
                                     getAnalysis<PostDominatorTree>().DT->getRootNode());
      std::cout << "Printing out post dominance frontier\n";
      dominance_frontiers_.print(errs());
      return false;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<PostDominatorTree>();
    }

   private:
     /// Data structure to compute dominance frontier on post dominator tree
     /// Technically, this should be used only for Forward Dominators, but there is nothing "forward" in its implementation,
     /// except this assert: https://github.com/llvm-mirror/llvm/blob/release_35/include/llvm/Analysis/DominanceFrontier.h#L124
     /// We sidestep this by directly calling calculate() instead of analyze()
     ForwardDominanceFrontierBase<BasicBlock> dominance_frontiers_;
  };
}

char PostDominanceFrontiers::ID = 0;
static RegisterPass<PostDominanceFrontiers> X("post_dominance_frontiers", "Postdominance frontiers", false, false);
