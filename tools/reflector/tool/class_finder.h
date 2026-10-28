#pragma once
#include "registry_file.h"
#include "reflected_class.h"
#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclCXX.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <string>
#include <vector>

using MatchFinder = clang::ast_matchers::MatchFinder;

class ClassFinder : public MatchFinder::MatchCallback {
public:
    ~ClassFinder() override = default;

    void run(const MatchFinder::MatchResult& result) override {
        m_context = result.Context;
        m_source_manager = result.SourceManager;

        if (auto* r = result.Nodes.getNodeAs<clang::CXXRecordDecl>("id"))
            return found_record(r);
        if (auto* f = result.Nodes.getNodeAs<clang::FieldDecl>("id"))
            return found_field(f);
        if (auto* f = result.Nodes.getNodeAs<clang::FunctionDecl>("id"))
            return found_function(f);
    }

    void onEndOfTranslationUnit() override {
        std::map<std::string, std::string> decls_by_file, body_by_file;
        for (auto& cls : m_classes) {
            llvm::raw_string_ostream decls_os(decls_by_file[cls.file_name()]);
            llvm::raw_string_ostream body_os(body_by_file[cls.file_name()]);
            cls.generate(m_context, decls_os, body_os);
        }

        std::map<std::string, std::string> updates;
        for (auto& [header, text] : decls_by_file)
            updates[registry_file::decl_prefix + header] = text;
        for (auto& [header, text] : body_by_file)
            updates[registry_file::body_prefix + header] = text;

        registry_file::update(updates);
        m_classes.clear();
    }

private:
    clang::ASTContext* m_context = nullptr;
    clang::SourceManager* m_source_manager = nullptr;
    std::vector<ReflectedClass> m_classes;

    bool m_in_root_class = false;

    void found_record(const clang::CXXRecordDecl* r) {
        clang::SourceLocation loc = m_source_manager->getFileLoc(r->getLocation());
        std::string file_name = registry_file::canonical(m_source_manager->getFilename(loc).str());
        std::string relative = registry_file::relative_to_root(file_name);

        m_in_root_class = !relative.empty();
        if (m_in_root_class && !llvm::StringRef(relative).ends_with(".h")
                && !llvm::StringRef(relative).ends_with(".hpp")) {
            llvm::errs() << "reflect: skipping '" << r->getQualifiedNameAsString()
                         << "', reflected classes must be declared in a header (" << relative << ")\n";
            m_in_root_class = false;
        }
        if (m_in_root_class)
            m_classes.emplace_back(r, std::move(relative));
    }

    void found_field(const clang::FieldDecl* f) {
        if (m_in_root_class && !m_classes.empty())
            m_classes.back().add_field(f);
    }

    void found_function(const clang::FunctionDecl* f) {
        if (m_in_root_class && !m_classes.empty())
            m_classes.back().add_function(f);
    }
};

