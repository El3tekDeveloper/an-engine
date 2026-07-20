#pragma once
#include "core/os/window.h"
#include <vector>

class SDL2WindowBackend : public WindowBackend {
public:
    bool startup() override;
    void shutdown() override;
 
    WindowHandle create_window(const WindowDesc& desc) override;
    void destroy_window(WindowHandle handle) override;
 
    void make_current(WindowHandle handle) override;
    void swap_buffers(WindowHandle handle) override;
 
    ProcLoader proc_loader() const override;

    uint32_t get_window_id(WindowHandle handle) const override;
    std::vector<WindowEventData> poll_events() override;
};
