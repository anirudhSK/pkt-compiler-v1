#include <iostream>
#include "llvm/IR/Instructions.h"
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
  llvm::FindFunctionBackedges(func, result);
  if (result.size() > 0) {
    throw std::invalid_argument("Supplied function body has a loop\n");
  }

  // Now start at the first block and propagate path conditions
  transfer_fn(& func.getEntryBlock(), "");

  return false;
}

void IfConversion::bb_walk(const BasicBlock * bb, const Value * incoming_condition __attribute__ ((unused))) {
  for (unsigned int i = 0; i < bb->getTerminator()->getNumSuccessors(); i++) {
    bb_walk(bb->getTerminator()->getSuccessor(i));
  }
  errs() << "Finished bb_walk for bb " << *bb << "\n";
}

IfConversion::BranchConditions IfConversion::transfer_fn(const BasicBlock * bb, const BoolExpr & in) const {
  const auto * terminator_inst = bb->getTerminator();

  if (isa<BranchInst>(terminator_inst)) {
    const auto * branch = dyn_cast<BranchInst>(terminator_inst);
    if (branch->isConditional()) {
      assert(branch->getNumSuccessors() == 2);

      // TODO: 0 and 1 are assumed to point to true and false respectively
      const auto true_edge  = std::make_pair(branch->getSuccessor(0), in + "(" + value_printer(branch->getCondition()) + " == true"  + ")");
      const auto false_edge = std::make_pair(branch->getSuccessor(1), in + "(" + value_printer(branch->getCondition()) + " == false" + ")");

      std::cout << "true_edge is " << true_edge.second << "\n";
      return {true_edge, false_edge};
    } else {
      assert(branch->getNumSuccessors() == 1);
      const auto single_outgoing_edge = std::make_pair(branch->getSuccessor(0), in);

      std::cout << "single_outgoing_edge is " << single_outgoing_edge.second << "\n";
      return {single_outgoing_edge};
    }
  } else if (isa<ReturnInst>(bb->getTerminator())) {
    assert(false);
    return BranchConditions();
  } else {
    assert(false);
    return BranchConditions();
  }
}

IfConversion::BoolExpr IfConversion::join_fn(const BasicBlock * bb,
                                             const std::vector<BranchConditions> & outp) const {
  BoolExpr in;
  for (const auto & br_conds : outp) {
    for (const auto & br_edge : br_conds) {
      // If edge points to this basic block, add its condition to a list of or operands
      if (br_edge.first == bb) {
        in += "(" + br_edge.second + ") or ";
      }
    }
  }
  return in;
}

char IfConversion::ID = 0;
static RegisterPass<IfConversion> X("if_conversion", "Convert all branches into guarded statements.", false, false);
