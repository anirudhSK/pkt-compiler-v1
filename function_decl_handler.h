#ifndef FUNCTION_DECL_HANDLER_H_
#define FUNCTION_DECL_HANDLER_H_

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include "clang_utility_functions.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

class FunctionDeclHandler : public MatchFinder::MatchCallback {
 public:
  /// Constructor: Pass Refactoring tool as argument
  FunctionDeclHandler(Replacements & t_replace) : Replace(t_replace) {}

  /// Callback whenever there's a match
  virtual void run(const MatchFinder::MatchResult &Result) override {
    std::cout << "Within FunctionDeclHandler::run\n";
    const FunctionDecl *function_decl_expr = Result.Nodes.getNodeAs<clang::FunctionDecl>("functionDecl");
    assert(function_decl_expr != nullptr);
  }

 private:
  Replacements & Replace;
};

#endif  // FUNCTION_DECL_HANDLER_H_
