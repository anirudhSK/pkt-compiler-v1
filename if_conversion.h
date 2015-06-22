#ifndef IF_CONVERSION_H_
#define IF_CONVERSION_H_

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"

struct IfConversion : public llvm::FunctionPass {
 public:
  static char ID;
  IfConversion() : llvm::FunctionPass(ID) {}
  bool runOnFunction(llvm::Function & func) override;
 private:
  void bb_walk(const llvm::BasicBlock * bb);
};

#endif  // IF_CONVERSION_H_
