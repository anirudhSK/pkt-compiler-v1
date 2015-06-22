#ifndef IF_CONVERSION_H_
#define IF_CONVERSION_H_

#include <utility>
#include <string>
#include <map>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "boolean_algebra.h"

struct IfConversion : public llvm::FunctionPass {
 public:
  /// Type for storing boolean expressions
  typedef Dnf BoolExpr;

  /// A single edge to a basic block on a path condition
  typedef std::pair<const llvm::BasicBlock *, const BoolExpr> BranchEdge;

  /// A vector of branch edges to represent a branch
  typedef std::vector<BranchEdge> BranchConditions;

  /// LLVM book keeping
  static char ID;

  /// Constructor
  IfConversion() : llvm::FunctionPass(ID) {}

  /// Virtual function where all the work happens
  bool runOnFunction(llvm::Function & func) override;

 private:
  void bb_walk(const llvm::BasicBlock * bb, const llvm::Value * incoming_condition = {});

  /// Transfer function for path condition propagation
  /// Arguments are the current basic block bb and
  /// the Boolean Expression coming into this basic block
  BranchConditions transfer_fn(const llvm::BasicBlock * bb, const BoolExpr & in) const;

  /// Join function for path condition propagation
  /// Arguments are current basic block bb
  /// and a vector of outgoing BranchConditions,
  /// one for each predecessor
  BoolExpr join_fn(const llvm::BasicBlock * bb, const std::vector<BranchConditions> & outp) const;

  /// Out states for each block
  std::map<const llvm::BasicBlock *, BranchConditions> out_states_ = {};

  /// In states for each block
  std::map<const llvm::BasicBlock *, BoolExpr> in_states_ = {};
};

#endif  // IF_CONVERSION_H_
