#include <iostream>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "graph.h"

using namespace llvm;

namespace {
  struct InstrDataDeps : public FunctionPass {
    static char ID;
    InstrDataDeps() : FunctionPass(ID) {}
    bool runOnFunction(Function &F) override {
      // Instruction data dependence graph
      Graph<const Instruction*> iddg ([] (const Instruction * instruction) {
          std::string str;
          llvm::raw_string_ostream rso(str);
          instruction->print(rso);
          return str;
      });

      for (auto instr = inst_begin(F); instr != inst_end(F); ++instr) {
        iddg.add_node(&*instr);
      }

      for(auto instr = inst_begin(F); instr != inst_end(F); ++instr) {
        for(const auto & i : instr->users()){
          // From definition to use
          assert(isa<Instruction>(i));
          iddg.add_edge(&*instr, dyn_cast<Instruction>(i));
        }
      }

      std::cout << "iddg is \n" << iddg << "\n"; 
      return false;
    }
  };
}

char InstrDataDeps::ID = 0;
static RegisterPass<InstrDataDeps> X("instr_data_deps", "Instruction-level data dependencies", false, false);
