#include "reflector.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Frontend/FrontendAction.h>
#include <memory>

using namespace clang;
using namespace clang::ast_matchers;

static void register_matchers(MatchFinder& finder) {
    finder.addMatcher(
        cxxRecordDecl(hasAttr(attr::Annotate), isDefinition()).bind("id"),
        &class_finder
    );
    finder.addMatcher(
        fieldDecl(hasAttr(attr::Annotate)).bind("id"),
        &class_finder
    );
    finder.addMatcher(
        cxxMethodDecl(hasAttr(attr::Annotate)).bind("id"),
        &class_finder
    );
}

class ReflectAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer>
    CreateASTConsumer(CompilerInstance& /*ci*/, llvm::StringRef /*file*/) override {
        register_matchers(finder);
        return finder.newASTConsumer();
    }

    bool ParseArgs(const CompilerInstance&,
                   const std::vector<std::string>& /*args*/) override {
        return true;
    }

    ActionType getActionType() override {
        return AddAfterMainAction;
    }
};

static FrontendPluginRegistry::Add<ReflectAction>
    X("reflect", "Generate .generated.hxx reflection headers");
