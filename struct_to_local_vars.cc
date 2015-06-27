#include <string>
#include <iostream>

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

static llvm::cl::OptionCategory StructToLocalVars("Replace structs with local variables");

class MemberExprHandler : public MatchFinder::MatchCallback {
 public:
  /// Constructor: Pass Refactoring tool as argument
  MemberExprHandler(Replacements & t_replace) : Replace(t_replace) {}

  /// Callback whenever there's a match
  virtual void run(const MatchFinder::MatchResult &Result) override {
    llvm::errs() << "Within run\n";

    const MemberExpr *member_expr = Result.Nodes.getNodeAs<clang::MemberExpr>("memberExpr");
    assert(member_expr != nullptr);
    const auto * base        = member_expr->getBase();
    const auto * member_decl = member_expr->getMemberDecl();

    // Now, create replacement text
    Replacement Rep(*(Result.SourceManager), member_expr,
                    clang_stmt_printer(base) + clang_value_decl_printer(member_decl));

    // Insert into this Replace
    Replace.insert(Rep);
  }

 private:
  Replacements & Replace;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, StructToLocalVars);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  MemberExprHandler HandlerForMemberExpr(Tool.getReplacements());

  MatchFinder Finder;
  Finder.addMatcher(memberExpr().bind("memberExpr"), &HandlerForMemberExpr);

  // Run the tool and collect a list of replacements.
  if (int Result = Tool.runAndSave(newFrontendActionFactory(&Finder).get())) {
    return Result;
  }

  llvm::outs() << "Replacements collected by the tool:\n";
  for (auto &r : Tool.getReplacements()) {
    llvm::outs() << r.toString() << "\n";
  }

  return 0;
}
