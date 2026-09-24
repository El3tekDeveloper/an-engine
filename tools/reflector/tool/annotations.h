#pragma once
#include <clang/AST/Attr.h>
#include <clang/AST/DeclBase.h>
#include <llvm/ADT/StringRef.h>
#include <string>

inline clang::Attr* find_annotation(clang::Decl* decl, llvm::StringRef prefix) {
    for (clang::Attr* attr : decl->attrs()) {
        auto* annotate = clang::dyn_cast<clang::AnnotateAttr>(attr);
        if (annotate && annotate->getAnnotation().starts_with(prefix))
            return attr;
    }
    return nullptr;
}

inline bool extract_range(clang::Decl* decl, std::string& out_min, std::string& out_max) {
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
