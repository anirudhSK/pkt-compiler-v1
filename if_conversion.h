#ifndef IF_CONVERSION_H_
#define IF_CONVERSION_H_

#include <utility>
#include <string>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"

struct IfConversion : public llvm::FunctionPass {
 public:
  /// Type for storing boolean expressions
  typedef std::string BoolExpr;

  /// A single edge to a basic block on a path condition
  typedef std::pair<const llvm::BasicBlock *, const BoolExpr> BranchEdge;

  /// A pair of branch edges to represent a branch
  typedef std::pair<BranchEdge, BranchEdge> BranchConditions;

  /// LLVM book keeping
  static char ID;

  /// Constructor
  IfConversion() : llvm::FunctionPass(ID) {}

  /// Virtual function where all the work happens
  bool runOnFunction(llvm::Function & func) override;

 private:
  void bb_walk(const llvm::BasicBlock * bb, const llvm::Value * incoming_condition = {});

  /// Transfer function for path condition propagation
  BranchConditions transfer_fn(const llvm::BasicBlock * bb, const BoolExpr & in);
};

#endif  // IF_CONVERSION_H_
