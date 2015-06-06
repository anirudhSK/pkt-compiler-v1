#include "graph.cc"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include <iostream>
using namespace llvm;

namespace {
  struct PrintCfg : public FunctionPass {
    static char ID;
    PrintCfg() : FunctionPass(ID) {}
    bool runOnFunction(Function & func __attribute__((unused))) override {
      // map from BasicBlock to a vector of BasicBlocks
      Graph<BasicBlock*> control_flow_graph;

      // Iterate over all basic blocks in the function to compute control-flow graph
      // First pass: Just get nodes alone
      for (auto it = func.begin(); it != func.end(); it++) {
        control_flow_graph.add_node(it);
      }

      // Second pass: Now add edges
      for (auto it = func.begin(); it != func.end(); it++) {
        for (unsigned int i = 0; i < it->getTerminator()->getNumSuccessors(); i++) {
          control_flow_graph.add_edge(it, (it->getTerminator()->getSuccessor(i)));
        }
      }

      std::cout << "Original CFG\n" << control_flow_graph << "\n";

      std::cout << "Flipped CFG\n" << control_flow_graph.transpose() << "\n";

      auto ret __attribute__ ((unused)) = control_flow_graph.get_dominators(func.begin());
      return false;
    }
   private:
  };
}

char PrintCfg::ID = 0;
static RegisterPass<PrintCfg> X("print_cfg", "Print CFG of function", false, false);
