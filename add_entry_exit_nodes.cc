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
#include "llvm/IR/Value.h"

using namespace llvm;

namespace {
  /// Add an entry and an exit node before computing CDG (Figure 19.15 of Appel's book)
  struct AddEntryExitNodes : public FunctionPass {
   public:
    static char ID;
    AddEntryExitNodes()
      : FunctionPass(ID) {}

    /// 1. Add two fake basic blocks: "entry" and "exit".
    /// 2. Append "exit" block to the block containing the return statement.
    /// We assume there's only one return, because this pass depends on mergereturn
    /// 3. Create dummy branch instruction from "entry" to
    ///    -> first block of actual code and
    ///    -> exit block created above
    /// N.B. Bolting on blocks like this can violate the SSA property causing
    /// a use to dominate an instruction. For now, this is temporarily ok,
    /// because we 'll likely strip these entry and exit blocks out in another pass
    bool runOnFunction(Function & f) override {
      // Step 1: Create entry and exit blocks
      auto entry_block = BasicBlock::Create(f.getContext(), "entry", &f);
      auto exit_block  = BasicBlock::Create(f.getContext(), "exit", &f);

      // Step 2.1: Get all return blocks
      std::vector<BasicBlock*> return_blocks;
      for (auto it = f.begin(); it != f.end(); it++) {
        if (it->getTerminator() == nullptr) {
          // Only our fake nodes can have no terminators
          assert(it->getName() == "entry" or it->getName() == "exit");
        } else {
          if (isa<ReturnInst>(it->getTerminator()))
            return_blocks.emplace_back(it);
        }
      }

      // Step 2.2: Check that we have exactly one because mergereturn should have run before this
      assert(return_blocks.size() == 1);

      // Step 2.3: Extract return value
      const auto return_value = return_blocks.front()->getTerminator()->getOperand(0);

      // Step 2.4 Remove the singular return and replace with an unconditional branch to "exit"
      return_blocks.front()->getInstList().pop_back();
      BranchInst::Create(exit_block, return_blocks.front());

      // Step 2.5: Add a return statement to the "exit" basic block to keep LLVM quiet about not having a terminator
      ReturnInst::Create(f.getContext(), return_value, exit_block);

      // Step 3.1: Create a dummy instuction to branch from entry to entry and exit blocks
      auto dummy_inst = BranchInst::Create(&f.getEntryBlock(), exit_block, ConstantInt::getTrue(f.getContext()));

      // Step 3.2: stick dummy_inst inside entry_block
      entry_block->getInstList().push_back(dummy_inst);

      // Step 3.3: Make this the new entry block
      entry_block->moveBefore(&f.getEntryBlock());

      return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<UnifyFunctionExitNodes>();
    }

  };
}

char AddEntryExitNodes::ID = 0;
static RegisterPass<AddEntryExitNodes> X("add_entry_exit_nodes", "Add entry and exit nodes", false, false);
