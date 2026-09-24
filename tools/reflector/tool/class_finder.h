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

    void found_record(const clang::CXXRecordDecl* r) {
        std::string file_name = m_source_manager->getFilename(r->getLocation()).str();

        llvm::SmallString<256> canonical(file_name);
        if (!llvm::sys::fs::real_path(file_name, canonical, /*expand_tilde=*/true))
            file_name = canonical.str().str();

        m_classes.emplace_back(r, std::move(file_name));
    }

    void found_field(const clang::FieldDecl* f) {
        if (!m_classes.empty())
            m_classes.back().add_field(f);
    }

    void found_function(const clang::FunctionDecl* f) {
        if (!m_classes.empty())
            m_classes.back().add_function(f);
    }
};

