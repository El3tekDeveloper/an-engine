#pragma once
#include "tool/annotations.h"
#include "tool/generator_plugin.h"

class RangePlugin : public GeneratorPlugin {
public:
    void on_field(const clang::FieldDecl* field, const GenContext& gc) override {
        if (gc.scope_var.empty() || !field->getType()->isArithmeticType())
            return;
 
        std::string range_min, range_max;
        if (!extract_range(const_cast<clang::FieldDecl*>(field), range_min, range_max))
            return;

        *gc.os << "        " << gc.scope_var << ".meta[\"range_min\"] = static_cast<double>(" << range_min << ");\n";
        *gc.os << "        " << gc.scope_var << ".meta[\"range_max\"] = static_cast<double>(" << range_max << ");\n";
    }
};
REGISTER_GENERATOR(RangePlugin)

