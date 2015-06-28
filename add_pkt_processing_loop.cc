#include <string>
#include <iostream>

#include "clang/Lex/Lexer.h"
#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ReplacementsYaml.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLTraits.h"
#include "clang_utility_functions.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory AddPktProcessingLoop("Add stylized packet-processing loop to packet processing function body");

class PacketProcessingCodeHandler : public MatchFinder::MatchCallback {
 public:
  /// Constructor: Pass Refactoring tool as argument
  PacketProcessingCodeHandler(Replacements & t_replace) : Replace(t_replace) {}

  /// Callback whenever there's a match
  virtual void run(const MatchFinder::MatchResult &Result) override {
    const FunctionDecl *function_decl_expr = Result.Nodes.getNodeAs<clang::FunctionDecl>("packetProcessingCode");
    assert(function_decl_expr != nullptr);

    /// Find location just after opening brace of function body
    auto start_loc = Lexer::getLocForEndOfToken(function_decl_expr->getBody()->getLocStart(), 0, *Result.SourceManager, Result.Context->getLangOpts());
    Replacement rep_start(*(Result.SourceManager), start_loc, 0, "\n while(true) {\n");
    Replace.insert(rep_start);

    /// Find location just before closing brace of function body
    auto end_loc = Lexer::GetBeginningOfToken(function_decl_expr->getBody()->getLocEnd(), *Result.SourceManager, Result.Context->getLangOpts());
    Replacement rep_end(*(Result.SourceManager), end_loc, 0, "\n }\n");
    Replace.insert(rep_end);
  }

 private:
  Replacements & Replace;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, AddPktProcessingLoop);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callback for function declaration
  PacketProcessingCodeHandler packet_processing_code_handler(Tool.getReplacements());
  MatchFinder find_packet_processing_code;
  find_packet_processing_code.addMatcher(functionDecl().bind("packetProcessingCode"), &packet_processing_code_handler);
  Tool.run(newFrontendActionFactory(&find_packet_processing_code).get());

  // Write into YAML object
  TranslationUnitReplacements replace_yaml;
  replace_yaml.MainSourceFile = argv[1];
  replace_yaml.Context = "some context";
  for (const auto &r : Tool.getReplacements())
    replace_yaml.Replacements.push_back(r);

  // Serialize to stdout
  llvm::yaml::Output yaml_stream(llvm::outs());
  yaml_stream << replace_yaml;

  return 0;
}
