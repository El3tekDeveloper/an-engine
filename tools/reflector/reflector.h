#pragma once
#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/AttrKinds.h>
#include <clang/Basic/LLVM.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <vector>

using MatchFinder = clang::ast_matchers::MatchFinder;

class ReflectedClass {
public:
    ReflectedClass() = default;
    explicit ReflectedClass(const clang::CXXRecordDecl* rec, std::string file_name)
        : m_record(rec), m_file_name(std::move(file_name)) {}

    const std::string& file_name() const { return m_file_name; }

    void add_field(const clang::FieldDecl*    f) { m_fields.push_back(f); }
    void add_function(const clang::FunctionDecl* f) { m_functions.push_back(f); }

    void generate(clang::ASTContext* ctx, llvm::raw_ostream& os) const {
        if (!m_record) return;

        const std::string type = m_record->getQualifiedNameAsString();
        const std::string type_safe = sanitize(type);
        os << "// Generated reflection for: " << type << "\n";
        os << "// DO NOT EDIT\n";
        os << "#pragma once\n";
        os << "#include \"tools/reflector/type_registry.h\"\n";

        if (ctx) {
            clang::SourceLocation loc = m_record->getLocation();
            clang::SourceManager& sm  = ctx->getSourceManager();
            loc = sm.getFileLoc(loc);
            if (loc.isValid()) {
                llvm::StringRef filePath = sm.getFilename(loc);
                static constexpr llvm::StringLiteral kMarkers[] = { "scene/", "." };
                llvm::StringRef rel = filePath;
                for (auto marker : kMarkers) {
                    auto pos = filePath.find(marker);
                    if (pos != llvm::StringRef::npos) {
                        rel = filePath.substr(pos);
                        break;
                    }
                }
                os << "#include \"" << rel.str() << "\"\n";
            }
        }

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
            os << "    c.parent_type = get_class_by_name(\"" << base_name << "\");\n\n";
            break;
        }

        for (const clang::FieldDecl* field : m_fields) {
            clang::Attr* export_attr = find_annotation(const_cast<clang::FieldDecl*>(field), "reflect-export");
            if (!export_attr) continue;

            const std::string fname = field->getNameAsString();
            clang::QualType qt      = field->getType().getUnqualifiedType();
            const std::string ftype = qt.getAsString();

            std::string range_min, range_max;
            bool has_range = field->getType()->isArithmeticType()
                && extract_range(const_cast<clang::FieldDecl*>(field), range_min, range_max);

            os << "    {\n";
            os << "        static const std::string_view _fname_" << fname << " = \"" << fname << "\";\n";
            os << "        static const std::string_view _ftype_" << fname << " = \"" << ftype << "\";\n";
            os << "        Field f;\n";
            os << "        f.name = _fname_" << fname << ";\n";
            os << "        f.offset = offsetof(" << type << ", " << fname << ");\n";
            os << "        f.type = get_type<"<< ftype <<">();\n";
            os << "        f.type->set_name(_ftype_" << fname << ");\n";
            if (has_range) {
                os << "        f.has_range = true;\n";
                os << "        f.range_min = " << range_min << ";\n";
                os << "        f.range_max = " << range_max << ";\n";
            }
            os << "        c.get_fields().push_back(f);\n";
            os << "    }\n";
        }

        for (const clang::FunctionDecl* func : m_functions) {
            clang::Attr* export_attr = find_annotation(const_cast<clang::FunctionDecl*>(func), "reflect-export");
            if (!export_attr) continue;

            bool is_button = func->getReturnType()->isVoidType()
                && func->getNumParams() == 0
                && find_annotation(const_cast<clang::FunctionDecl*>(func), "reflect-button") != nullptr;

            const std::string fname   = func->getNameAsString();
            clang::QualType   rqt     = func->getReturnType().getUnqualifiedType();
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
            if (is_button) {
                os << "        fn.is_button = true;\n";
            }

            for (unsigned i = 0; i < func->getNumParams(); ++i) {
                const clang::ParmVarDecl* p     = func->getParamDecl(i);
                const std::string         pname = p->getNameAsString();
                clang::QualType           pqt   = p->getType().getUnqualifiedType();
                const std::string         ptype = pqt.getAsString();

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
                const std::string ptype = p->getType().getUnqualifiedType().getAsString();
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

        os << "\n    return &c;\n";
        os << "}\n\n";

        os << "namespace {\n";
        os << "    struct " << type_safe << "_AutoRegister {\n";
        os << "        " << type_safe << "_AutoRegister() {\n";
        os << "            CLASS_REGISTRY[\"" << type << "\"] = get_class_impl<" << type << ">();\n";
        os << "            register_type<"<< type <<">(\""<< type <<"\")\n;";
        os << "        }\n";
        os << "    };\n";
        os << "    inline " << type_safe << "_AutoRegister " << type_safe << "_auto_register_instance;\n";
        os << "}\n\n";
    }

private:
    const clang::CXXRecordDecl* m_record = nullptr;
    std::string m_file_name;
    std::vector<const clang::FieldDecl*> m_fields;
    std::vector<const clang::FunctionDecl*> m_functions;

    static std::string sanitize(const std::string& s) {
        std::string out = s;
        for (char& c : out) if (c == ':') c = '_';
        return out;
    }

    static clang::Attr* find_annotation(clang::Decl* decl, llvm::StringRef prefix) {
        for (clang::Attr* attr : decl->attrs()) {
            auto* annotate = clang::dyn_cast<clang::AnnotateAttr>(attr);
            if (annotate && annotate->getAnnotation().starts_with(prefix))
                return attr;
        }
        return nullptr;
    }

    static bool extract_range(clang::Decl* decl, std::string& out_min, std::string& out_max) {
        clang::Attr* attr = find_annotation(decl, "reflect-range");
        if (!attr) return false;

        auto* annotate = clang::dyn_cast<clang::AnnotateAttr>(attr);
        if (!annotate) return false;

        llvm::StringRef full = annotate->getAnnotation();
        auto semi = full.find(';');
        if (semi == llvm::StringRef::npos) return false;

        llvm::StringRef payload = full.substr(semi + 1);
        auto comma = payload.find(',');
        if (comma == llvm::StringRef::npos) return false;

        out_min = payload.substr(0, comma).str();
        out_max = payload.substr(comma + 1).str();
        return true;
    }
};

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
        for (auto& cls : m_classes) {
            std::error_code ec;
            llvm::raw_fd_ostream file(cls.file_name(), ec);
            if (ec) {
                llvm::errs() << "reflect: cannot open '" << cls.file_name()
                             << "': " << ec.message() << "\n";
                continue;
            }
            cls.generate(m_context, file);
        }
        m_classes.clear();
    }

private:
    clang::ASTContext* m_context = nullptr;
    clang::SourceManager* m_source_manager = nullptr;
    std::vector<ReflectedClass> m_classes;

    void found_record(const clang::CXXRecordDecl* r) {
        std::string file_name = m_source_manager->getFilename(r->getLocation()).str();

        size_t slash = file_name.find_last_of("/\\");
        std::string directory = (slash == std::string::npos)
            ? ""
            : file_name.substr(0, slash);

        std::string filename = (slash == std::string::npos)
            ? file_name
            : file_name.substr(slash + 1);

        size_t dot = filename.rfind('.');
        if (dot != std::string::npos)
            filename.erase(dot);

        std::error_code ec = llvm::sys::fs::create_directories(directory + "/.generated");
        if (ec) {
            llvm::errs() << "Failed to create directory: " << ec.message() << "\n";
        }

        file_name = directory + "/.generated/" + filename + ".generated.hxx";

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

inline ClassFinder class_finder;
inline MatchFinder finder;
