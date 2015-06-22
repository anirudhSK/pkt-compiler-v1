#include "utility_functions.h"

std::string value_printer(const llvm::Value * value) {
  std::string str;
  llvm::raw_string_ostream rso(str);
  value->printAsOperand(rso);
  return str;
}

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
