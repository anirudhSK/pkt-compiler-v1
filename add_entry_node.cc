#include <iostream>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

namespace {
  /// Add an entry node before computing CDG (Figure 19.15 of Appel's book)
  struct AddEntryNode : public FunctionPass {
   public:
    static char ID;
    AddEntryNode()
      : FunctionPass(ID) {}

    /// Create dummy branch instruction to entry block and exit block
    /// We assume there's only one exit, so this pass depends on mergereturn
    /// N.B. Jamming in a block like this can violate the SSA property causing
    /// a use to dominate an instruction. For now, this is temporarily ok,
    /// because we 'll likely strip this convenience block out in another pass
    bool runOnFunction(Function & f) override {
      // I am also assuming (verified by assert) that the exit block is the last one,
      // so we 'll check that the last instruction in the last block is a return
      auto last_block = &f.back();
      assert(isa<ReturnInst>(last_block->getTerminator()));

      auto dummy_inst = BranchInst::Create(&f.getEntryBlock(), last_block, ConstantInt::getTrue(getGlobalContext()));

      // Create an entry block and stick dummy_inst inside it
      auto entry_block = BasicBlock::Create(getGlobalContext(), "entry", &f, &f.getEntryBlock());
      entry_block->getInstList().push_back(dummy_inst);
      return false;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<UnifyFunctionExitNodes>();
    }

  };
}

char AddEntryNode::ID = 0;
static RegisterPass<AddEntryNode> X("add_entry_node", "Add entry node", false, false);
