#ifndef UTILITY_FUNCTIONS_H_
#define UTILITY_FUNCTIONS_H_

#include <string>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

std::string value_printer(const llvm::Value * value);

std::string instr_printer(const llvm::Instruction * instruction);

std::string bb_printer(const llvm::BasicBlock * basic_block);

#endif  // UTILITY_FUNCTIONS_H_
