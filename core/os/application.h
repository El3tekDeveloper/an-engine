#pragma once
#include "core/os/window.h"
#include <memory>
#include <vector>

class Application {
public:
    Application();
    ~Application();

    Window* create_window(WindowDesc desc);
    void destroy_window(Window* window);
    
    void poll_events();

    bool is_running() const { return running; }
    void quit() { running = false; }

    const std::vector<std::unique_ptr<Window>>& get_windows() const { return windows; }

private:
    std::unique_ptr<WindowBackend> backend;
    std::vector<std::unique_ptr<Window>> windows;
    bool running = false;
    bool proc_loaded = false;
};
