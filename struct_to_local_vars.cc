#include <string>
#include <iostream>
#include <set>

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
#include "function_decl_handler.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory StructToLocalVars("Replace structs with local variables");

class MemberExprHandler : public MatchFinder::MatchCallback {
 public:
  /// Print out all declarations
  void print_all_decls() const {
    for (const auto & decl : decl_strings_)
      llvm::outs() << decl;
  }

  /// Constructor: Pass Refactoring tool as argument
  MemberExprHandler(Replacements & t_replace) : Replace(t_replace) {}

  /// Callback whenever there's a match
  virtual void run(const MatchFinder::MatchResult &Result) override {
    const MemberExpr *member_expr = Result.Nodes.getNodeAs<clang::MemberExpr>("memberExpr");
    assert(member_expr != nullptr);
    const auto * base        = member_expr->getBase();
    const auto * member_decl = member_expr->getMemberDecl();

    // Get type name of member_decl
    auto type_name = member_decl->getType().getAsString();

    // Create declaration as a string, TODO: there's probably a more hygeinic approach
    decl_strings_.emplace(type_name + " " + clang_value_decl_printer(member_decl) + ";\n");

    // Now, create replacement text
    Replacement Rep(*(Result.SourceManager), member_expr,
                    clang_stmt_printer(base) + clang_value_decl_printer(member_decl));

    // Insert into this Replace
    Replace.insert(Rep);
  }

 private:
  Replacements & Replace;
  std::set<std::string> decl_strings_;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, StructToLocalVars);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks for member expressions
  MemberExprHandler member_expr_handler(Tool.getReplacements());
  MatchFinder find_member_expr;
  find_member_expr.addMatcher(memberExpr().bind("memberExpr"), &member_expr_handler);

  // Set up AST matcher callback for function declaration
  FunctionDeclHandler function_decl_handler(Tool.getReplacements());
  MatchFinder find_function_decl;
  find_function_decl.addMatcher(functionDecl().bind("functionDecl"), &function_decl_handler);

  // Run the tool and collect a list of replacements.
  Tool.runAndSave(newFrontendActionFactory(&find_member_expr).get());
  Tool.runAndSave(newFrontendActionFactory(&find_member_expr).get());

  llvm::outs() << "Replacements collected by the tool:\n";
  for (auto &r : Tool.getReplacements()) {
    llvm::outs() << r.toString() << "\n";
  }

  llvm::outs() << "New declarations\n";
  member_expr_handler.print_all_decls();

  return 0;
}
