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

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory StructToLocalVars("Replace structs with local variables");

class MemberExprHandler : public MatchFinder::MatchCallback {
 public:
  /// Constructor: Pass Refactoring tool as argument
  MemberExprHandler(Replacements *Replace) : Replace(Replace) {}

  /// Callback whenever there's a match
  virtual void run(const MatchFinder::MatchResult &Result) override {
    const MemberExpr *member_expr = Result.Nodes.getNodeAs<clang::MemberExpr>("memberExpr");
    llvm::errs() << "Within run\n";
    assert(member_expr != nullptr);
    const auto * base        = member_expr->getBase();
    const auto * member_decl = member_expr->getMemberDecl();
    llvm::errs() << "Base is ";

    // Required for pretty printing
    clang::LangOptions LangOpts;
    LangOpts.CPlusPlus = true;
    clang::PrintingPolicy Policy(LangOpts);

    base->printPretty(llvm::errs(), nullptr, Policy);
//      const Stmt *Then = IfS->getThen();
//      Replacement Rep(*(Result.SourceManager), Then->getLocStart(), 0,
//                      "// the 'if' part\n");
//      Replace->insert(Rep);
//
//      if (const Stmt *Else = IfS->getElse()) {
//        Replacement Rep(*(Result.SourceManager), Else->getLocStart(), 0,
//                        "// the 'else' part\n");
//        Replace->insert(Rep);
//      }
//    }
  }

 private:
  Replacements *Replace;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, StructToLocalVars);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  MemberExprHandler HandlerForMemberExpr(&Tool.getReplacements());

  MatchFinder Finder;
  Finder.addMatcher(memberExpr().bind("memberExpr"), &HandlerForMemberExpr);

  // Run the tool and collect a list of replacements.
  if (int Result = Tool.run(newFrontendActionFactory(&Finder).get())) {
    return Result;
  }

  llvm::outs() << "Replacements collected by the tool:\n";
  for (auto &r : Tool.getReplacements()) {
    llvm::outs() << r.toString() << "\n";
  }

  return 0;
}
