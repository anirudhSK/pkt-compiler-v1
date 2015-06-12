#ifndef UTILITY_FUNCTIONS_H_
#define UTILITY_FUNCTIONS_H_

#include <string>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

std::string instr_printer(const llvm::Instruction * instruction) {
  std::string str;
  llvm::raw_string_ostream rso(str);
  instruction->print(rso);
  return str;
}

std::string bb_printer(const llvm::BasicBlock * basic_block) {
  std::string str;
  llvm::raw_string_ostream rso(str);
  basic_block->printAsOperand(rso);
  return str;
}

#endif  // UTILITY_FUNCTIONS_H_
