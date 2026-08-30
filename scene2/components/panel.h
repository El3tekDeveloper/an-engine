#pragma once
#include "scene2/components/component.h"
#include "resources/style_box.h"

class [[export]] Panel : public Component {
public:
    Panel() = default;
    explicit Panel(StyleBox& style_box) : style_box(&style_box) {};

    StyleBox* style_box = nullptr;
};

#if __REFLECT_GENERATED__
#include ".generated/panel.generated.hxx"
#endif
