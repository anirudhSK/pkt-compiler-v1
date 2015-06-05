#include <map>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

namespace {
  struct PrintCfg : public FunctionPass {
    /// Adjacency list representation of graph
    typedef std::map<BasicBlock*, std::vector<BasicBlock*>> Graph;

    static char ID;
    PrintCfg() : FunctionPass(ID) {}
    bool runOnFunction(Function & func __attribute__((unused))) override {
      // map from BasicBlock to a vector of BasicBlocks
      Graph control_flow_graph;

      // Iterate over all basic blocks in the function to compute control-flow graph
      for (auto it = func.begin(); it != func.end(); it++) {
        if(it->getTerminator()->getNumSuccessors() > 0) {
          control_flow_graph[it] = std::vector<BasicBlock*>();
          for (unsigned int i = 0; i < it->getTerminator()->getNumSuccessors(); i++) {
            control_flow_graph.at(it).emplace_back(it->getTerminator()->getSuccessor(i));
          }
        }
      }

      errs() << "Original CFG\n";
      print_graph(control_flow_graph);

      return false;
    }
   private:
    /// Print out the graph in human-readable form
    void print_graph(const Graph & graph) const {
      for (const auto & node : graph) {
        node.first->printAsOperand(errs());
        errs() << " ---> ";
        for (const auto & neighbor : node.second) {
          neighbor->printAsOperand(errs());
          errs() << ", ";
        }
        errs() << "\n";
      }
    }

  };
}

char PrintCfg::ID = 0;
static RegisterPass<PrintCfg> X("print_cfg", "Print CFG of function", false, false);
