#include <iostream>
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/Analysis/CFG.h"
#include "utility_functions.h"
#include "if_conversion.h"

using namespace llvm;

bool IfConversion::runOnFunction(Function & func) {
  errs() << "IfConversion: ";
  errs().write_escaped(func.getName()) << '\n';
  for(auto instr = inst_begin(func); instr != inst_end(func); ++instr) {
    instr->print(errs());
    errs() << "\n";
  }

  // Check that there are no back edges
  SmallVector<std::pair<const BasicBlock *, const BasicBlock *>, 100> result;
  FindFunctionBackedges(func, result);
  if (result.size() > 0) {
    throw std::invalid_argument("Supplied function body has a loop\n");
  }

  // Now propagate path conditions in topologically sorted order
  // Reverse Post Order Traversal gets us topological sort
  ReversePostOrderTraversal<const Function*> rpot(&func);
  for (ReversePostOrderTraversal<const Function *>::rpo_iterator it = rpot.begin(); it != rpot.end(); ++it) {
    const auto * current_bb = *it;

    /// Get "out" vectors of all predecessors
    std::vector<BranchConditions> out_preds;
    for (const_pred_iterator pi = pred_begin(current_bb); pi != pred_end(current_bb); ++pi) {
      out_preds.emplace_back(out_states_.at(*pi));
    }

    in_states_[current_bb]  = join_fn(current_bb, out_preds);
    out_states_[current_bb] = transfer_fn(current_bb, in_states_.at(current_bb));
  }

  return false;
}

IfConversion::BranchConditions IfConversion::transfer_fn(const BasicBlock * bb, const BoolExpr & in) const {
  const auto * terminator_inst = bb->getTerminator();
  std::cout << "Incoming edge " << in << "\n";

  if (isa<BranchInst>(terminator_inst)) {
    const auto * branch = dyn_cast<BranchInst>(terminator_inst);
    if (branch->isConditional()) {
      assert(branch->getNumSuccessors() == 2);

      // TODO: 0 and 1 are assumed to point to true and false respectively
      const auto true_edge  = std::make_pair(branch->getSuccessor(0), in * Conjunction(Atom(value_printer(branch->getCondition()), true)));
      const auto false_edge = std::make_pair(branch->getSuccessor(1), in * Conjunction(Atom(value_printer(branch->getCondition()), false)));

      std::cout << "true_edge is " << true_edge.second << "\n";
      std::cout << "false_edge is " << false_edge.second << "\n";
      return {true_edge, false_edge};
    } else {
      assert(branch->getNumSuccessors() == 1);
      const auto single_outgoing_edge = std::make_pair(branch->getSuccessor(0), in);

      std::cout << "single_outgoing_edge is " << single_outgoing_edge.second << "\n";
      return {single_outgoing_edge};
    }
  } else if (isa<ReturnInst>(bb->getTerminator())) {
    std::cout << "Within a return instruction\n";
    return BranchConditions();
  } else {
    throw std::logic_error("Some other kind of branch\n");
    return BranchConditions();
  }
}

IfConversion::BoolExpr IfConversion::join_fn(const BasicBlock * bb,
                                             const std::vector<BranchConditions> & outp) const {
  BoolExpr in;
  in = in + Conjunction(Atom::make_literal(false));
  for (const auto & br_conds : outp) {
    for (const auto & br_edge : br_conds) {
      // If edge points to this basic block, add its condition to a list of or operands
      if (br_edge.first == bb) {
        in = in + br_edge.second;
      }
    }
  }
  return in;
}

char IfConversion::ID = 0;
static RegisterPass<IfConversion> X("if_conversion", "Convert all branches into guarded statements.", false, false);
