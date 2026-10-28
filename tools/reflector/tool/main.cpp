#include "class_finder.h"
#include "reflector_plugins.inc"

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <memory>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

namespace {

llvm::cl::OptionCategory ReflectToolCategory("reflect-tool options");

llvm::cl::opt<std::string> OutputOption(
    "output",
    llvm::cl::desc("Generated registry file (default: <root>/project.sln.hxx)"),
    llvm::cl::cat(ReflectToolCategory));

llvm::cl::opt<std::string> RootOption(
    "root",
    llvm::cl::desc("Only reflect classes declared under this folder (default: engine root)"),
    llvm::cl::cat(ReflectToolCategory));

void register_matchers(MatchFinder& finder, ClassFinder& class_finder) {
    finder.addMatcher(
        cxxRecordDecl(hasAttr(attr::Annotate), isDefinition()).bind("id"),
        &class_finder);
    finder.addMatcher(
        fieldDecl(hasAttr(attr::Annotate)).bind("id"),
        &class_finder);
    finder.addMatcher(
        cxxMethodDecl(hasAttr(attr::Annotate)).bind("id"),
        &class_finder);
}

} // namespace

int main(int argc, const char** argv) {
    auto expected_parser = CommonOptionsParser::create(argc, argv, ReflectToolCategory);
    if (!expected_parser) {
        llvm::errs() << "reflect_tool: " << llvm::toString(expected_parser.takeError()) << "\n";
        return 1;
    }
    CommonOptionsParser& options_parser = expected_parser.get();

    registry_file::init(OutputOption, RootOption);
    registry_file::ensure_exists();

    ClangTool tool(options_parser.getCompilations(), options_parser.getSourcePathList());
    
    MatchFinder finder;
    ClassFinder class_finder;

    register_matchers(finder, class_finder);
    std::unique_ptr<clang::tooling::FrontendActionFactory> factory = newFrontendActionFactory(&finder);
    
    return tool.run(factory.get());
}
