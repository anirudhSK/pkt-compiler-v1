#ifndef INSTR_CTRL_DEPS_H_ 
#define INSTR_CTRL_DEPS_H_ 

#include <iostream>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "control_dependence_graph.h"

/// Lower control dependences to the instruction level
/// using the control dependence graph at the basic-block level
/// computed by the ControlDependenceGraph pass
struct InstrCtrlDeps: public llvm::FunctionPass {
 public:
  static char ID;
  InstrCtrlDeps() : llvm::FunctionPass(ID) {};

  /// Lower control dependences to the level of instructions.
  /// Instruction A is control dependent on Instruction B if its
  /// enclosing basic block BB{A} is control dependent on B's
  /// enclosing basic block BB{B}.
  bool runOnFunction(llvm::Function & func) override;

  /// Specify that we need ControlDependenceGraph to be run
  /// before we run this pass.
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

  const auto icdg() const { return icdg_; }
 private:
  // Instruction Control Dependence Graph
  Graph<const llvm::Instruction*> icdg_ = Graph<const llvm::Instruction*>([] (const llvm::Instruction * instruction) {
      std::string str;
      llvm::raw_string_ostream rso(str);
      instruction->print(rso);
      return str;
  });
};

#endif  // INSTR_CTRL_DEPS_H_
