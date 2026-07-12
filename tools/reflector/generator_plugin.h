#pragma once
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>
#include <vector>

struct GenContext {
    clang::ASTContext* ast_ctx = nullptr;
    llvm::raw_ostream* os = nullptr;
    std::string type;
    std::string type_safe;
    std::string scope_var;
};

class GeneratorPlugin {
public:
    virtual ~GeneratorPlugin() = default;

    virtual void on_class_begin(const clang::CXXRecordDecl* rec, const GenContext& gc) { (void)rec; (void)gc; }
    virtual void on_field(const clang::FieldDecl* field, const GenContext& gc) { (void)field; (void)gc; }
    virtual void on_function(const clang::FunctionDecl* func, const GenContext& gc) { (void)func; (void)gc; }
    virtual void on_class_end(const clang::CXXRecordDecl* rec, const GenContext& gc) { (void)rec; (void)gc; }
};

class GeneratorRegistry {
public:
    static GeneratorRegistry& instance() {
        static GeneratorRegistry inst;
        return inst;
    }

    void add(std::unique_ptr<GeneratorPlugin> plugin) {
        m_plugins.push_back(std::move(plugin));
    }

    const std::vector<std::unique_ptr<GeneratorPlugin>>& plugins() const {
        return m_plugins;
    }

private:
    std::vector<std::unique_ptr<GeneratorPlugin>> m_plugins;
};

template<typename T>
struct GeneratorRegistrar {
    GeneratorRegistrar() {
        GeneratorRegistry::instance().add(std::make_unique<T>());
    }
};

#define REGISTER_GENERATOR(cls) \
    static GeneratorRegistrar<cls> cls##_registrar_instance;
