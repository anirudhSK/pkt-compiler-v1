#ifndef ADD_ENTRY_EXIT_NODES_H_
#define ADD_ENTRY_EXIT_NODES_H_

#include <iostream>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

/// Add an entry and an exit node before computing CDG (Figure 19.15 of Appel's book)
struct AddEntryExitNodes : public llvm::FunctionPass {
 public:
  static char ID;
  AddEntryExitNodes()
    : llvm::FunctionPass(ID) {}

  /// 1. Add two fake basic blocks: "entry" and "exit".
  /// 2. Append "exit" block to the block containing the return statement.
  /// We assume there's only one return, because this pass depends on mergereturn
  /// 3. Create dummy branch instruction from "entry" to
  ///    -> first block of actual code and
  ///    -> exit block created above
  /// N.B. Bolting on blocks like this can violate the SSA property causing
  /// a use to dominate an instruction. For now, this is temporarily ok,
  /// because we 'll likely strip these entry and exit blocks out in another pass
  bool runOnFunction(llvm::Function & f) override;

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
};

#endif  // ADD_ENTRY_EXIT_NODES_H_
