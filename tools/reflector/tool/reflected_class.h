#pragma once
#include "annotations.h"
#include "generator_plugin.h"
#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>
#include <cctype>
#include <string>
#include <vector>

class ReflectedClass {
public:
    ReflectedClass() = default;
    explicit ReflectedClass(const clang::CXXRecordDecl* rec, std::string file_name)
        : m_record(rec), m_file_name(std::move(file_name)) {}

    const std::string& file_name() const { return m_file_name; }

    void add_field(const clang::FieldDecl*    f) { m_fields.push_back(f); }
    void add_function(const clang::FunctionDecl* f) { m_functions.push_back(f); }

    void generate(clang::ASTContext* ctx, llvm::raw_ostream& decls, llvm::raw_ostream& os) const {
        if (!m_record) return;

        const std::string type = m_record->getQualifiedNameAsString();
        const std::string type_safe = sanitize(type);

        std::string include_root;
        if (ctx) {
            clang::SourceLocation loc = m_record->getLocation();
            clang::SourceManager& sm  = ctx->getSourceManager();
            loc = sm.getFileLoc(loc);
            if (loc.isValid()) {
                llvm::StringRef filePath = sm.getFilename(loc);
                static constexpr llvm::StringLiteral kMarkers[] = { "./" };
                llvm::StringRef rel = filePath;
                for (auto marker : kMarkers) {
                    auto pos = filePath.find(marker);
                    if (pos != llvm::StringRef::npos) {
                        rel = filePath.substr(pos);
                        include_root = filePath.substr(0, pos).str();
                        break;
                    }
                }
                decls << "#include \"" << rel.str() << "\"\n";
            }
        }
        decls << "template<> struct has_reflection<" << type << "> : std::true_type {};\n";

        os << "// " << type << "\n";

        for (auto& plugin : GeneratorRegistry::instance().plugins())
            plugin->on_class_begin(m_record, {ctx, &os, type, type_safe, "", include_root});

        os << "\n";
        os << "template<>\n";
        os << "inline TypeClass* get_class_impl<" << type << ">() {\n";
        os << "    static TypeClass c(\"" << type << "\", sizeof(" << type << "));\n";
        os << "    static bool initialized = false;\n";
        os << "    if (initialized) return &c;\n";
        os << "    initialized = true;\n\n";

        os << "    c.get_impl = []() -> TypeClass* { return get_class_impl<" << type << ">(); };\n";
        os << "    c.create_instance = []() -> void* { return new " << type << "(); };\n\n";

        for (const auto& base : m_record->bases()) {
            std::string base_name = base.getType()->getAsCXXRecordDecl()
                                        ->getQualifiedNameAsString();
            os << "    c.parent_type = static_cast<TypeClass*>(get_type_by_name(\"" << base_name << "\"));\n\n";
            break;
        }

        for (const clang::FieldDecl* field : m_fields) {
            clang::Attr* export_attr = find_annotation(const_cast<clang::FieldDecl*>(field), "reflect-export");
            if (!export_attr) {
                for (auto& plugin : GeneratorRegistry::instance().plugins())
                    plugin->on_field(field, {ctx, &os, type, type_safe, "", include_root});
                continue;
            }

            const std::string fname = field->getNameAsString();
            clang::QualType qt = field->getType().getUnqualifiedType();
            clang::PrintingPolicy policy(ctx->getLangOpts());
            const std::string ftype = qt.getAsString(policy);

            os << "    {\n";
            os << "        static const std::string_view _fname_" << fname << " = \"" << fname << "\";\n";
            os << "        static const std::string_view _ftype_" << fname << " = \"" << ftype << "\";\n";
            os << "        Field f;\n";
            os << "        f.name = _fname_" << fname << ";\n";
            os << "        f.offset = offsetof(" << type << ", " << fname << ");\n";
            os << "        f.type = get_type<"<< ftype <<">();\n";
            os << "        if (f.type) f.type->set_name(_ftype_" << fname << ");\n";
            os << "        else LOG_ERROR(\"reflect: field '" << fname << "' of " << type << " has unresolved type\");\n";

            for (auto& plugin : GeneratorRegistry::instance().plugins())
                plugin->on_field(field, {ctx, &os, type, type_safe, "f", include_root});

            os << "        c.get_fields().push_back(f);\n";
            os << "    }\n";
        }

        for (const clang::FunctionDecl* func : m_functions) {
            clang::Attr* export_attr = find_annotation(const_cast<clang::FunctionDecl*>(func), "reflect-export");
            if (!export_attr) {
                for (auto& plugin : GeneratorRegistry::instance().plugins())
                    plugin->on_function(func, {ctx, &os, type, type_safe, "", include_root});
                continue;
            }

            const std::string fname = func->getNameAsString();
            clang::QualType rqt = func->getReturnType().getNonReferenceType().getUnqualifiedType();
            const std::string rettype = rqt.getAsString();

            os << "    {\n";
            os << "        static const std::string _fnname_" << fname << " = \"" << fname << "\";\n";
            os << "        static const std::string _fnret_"  << fname << " = \"" << rettype << "\";\n";
            os << "        RefFunction fn;\n";
            os << "        fn.name = _fnname_" << fname << ".c_str();\n";
            os << "        fn.return_value.name = _fnname_" << fname << ".c_str();\n";
            os << "        fn.return_value.offset = 0;\n";
            if (rettype != "void") {
                os << "        fn.return_value.type   = get_type<" << rettype << ">();\n";
            }
            
            for (auto& plugin : GeneratorRegistry::instance().plugins())
                plugin->on_function(func, {ctx, &os, type, type_safe, "fn", include_root});

            for (unsigned i = 0; i < func->getNumParams(); ++i) {
                const clang::ParmVarDecl* p = func->getParamDecl(i);
                const std::string pname = p->getNameAsString();
                clang::QualType pqt = p->getType().getUnqualifiedType();
                const std::string ptype = pqt.getNonReferenceType().getUnqualifiedType().getAsString();

                os << "        {\n";
                os << "            static const std::string _pname_" << fname << "_" << i << " = \"" << pname << "\";\n";
                os << "            static const std::string _ptype_" << fname << "_" << i << " = \"" << ptype << "\";\n";
                os << "            Field p;\n";
                os << "            p.name   = _pname_" << fname << "_" << i << ".c_str();\n";
                os << "            p.offset = " << i << ";\n";
                os << "            p.type   = get_type<" << ptype << ">();\n";
                os << "            fn.parameters.push_back(p);\n";
                os << "        }\n";
            }

            os << "        fn.invoke = [](void* self, void** args, void* ret) {\n";
            os << "            auto* obj = static_cast<" << type << "*>(self);\n";

            std::string call_args;
            for (unsigned i = 0; i < func->getNumParams(); ++i) {
                if (i) call_args += ", ";
                const clang::ParmVarDecl* p = func->getParamDecl(i);
                const std::string ptype = p->getType().getNonReferenceType().getUnqualifiedType().getAsString();
                call_args += "*static_cast<" + ptype + "*>(args[" + std::to_string(i) + "])";
            }

            if (rettype == "void") {
                os << "            obj->" << fname << "(" << call_args << ");\n";
            } else {
                os << "            *static_cast<" << rettype << "*>(ret) = obj->" << fname << "(" << call_args << ");\n";
            }
            os << "        };\n";

            os << "        c.get_functions().push_back(fn);\n";
            os << "    }\n";
        }

        for (auto& plugin : GeneratorRegistry::instance().plugins())
            plugin->on_class_end(m_record, {ctx, &os, type, type_safe, "c", include_root});

        os << "\n    return &c;\n";
        os << "}\n\n";

        os << "namespace {\n";
        os << "    struct " << type_safe << "_AutoRegister {\n";
        os << "        " << type_safe << "_AutoRegister() {\n";
        os << "            register_type<" << type << ">(get_class_impl<" << type << ">());\n";
        os << "        }\n";
        os << "    };\n";
        os << "    inline " << type_safe << "_AutoRegister " << type_safe << "_auto_register_instance;\n";
        os << "}\n\n\n";
    }

private:
    const clang::CXXRecordDecl* m_record = nullptr;
    std::string m_file_name;
    std::vector<const clang::FieldDecl*> m_fields;
    std::vector<const clang::FunctionDecl*> m_functions;

    static std::string sanitize(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (char c : s)
            out += (std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_';
        return out;
    }
};

