#pragma once
#include <cstdint>
#include <utility>
#include <vector>
#include <functional>

struct WindowDesc {
    const char* title = "non-title (an-engine)";
    int width = 1280;
    int height = 720;
};

enum class WindowEvent  {
    Resize,
    CloseRequested,
    AppQuit
};

struct WindowEventData {
    WindowEvent type;
    uint32_t window_id = 0;
    int width = 0;
    int height = 0;
};

using WindowHandle = void*;
using ProcLoader = void* (*)(const char*);

class WindowBackend {
public:
    virtual ~WindowBackend() = default;

    virtual bool startup() = 0;
    virtual void shutdown() = 0;

    virtual WindowHandle create_window(const WindowDesc& desc) = 0;
    virtual void destroy_window(WindowHandle native) = 0;

    virtual void make_current(WindowHandle native) = 0;
    virtual void swap_buffers(WindowHandle native) = 0;

    virtual ProcLoader proc_loader() const = 0;
    
    virtual uint32_t get_window_id(WindowHandle handle) const = 0;
    virtual std::vector<WindowEventData> poll_events() = 0;
};

class Window {
public:
    Window(WindowDesc desc, WindowBackend* backend);
    ~Window();

    uint32_t id;

    bool init();
    void shutdown();

    void make_current();
    void swap_buffers();

    WindowHandle get_handle() const { return handle; }
    WindowDesc get_desc() const { return desc; }
    
    int get_width() const { return desc.width; }
    int get_height() const { return desc.height; }
    const char* get_title() const { return desc.title; }

    bool should_close() const { return closing; }
    void request_close() { closing = true; }

    std::function<void(int, int)> on_resize;
    std::function<void()> on_close;
 
    void handle_resize(int width, int height);
    void handle_close_request();

private:
    WindowDesc desc;
    WindowBackend* backend;
    WindowHandle handle;
    bool closing = false;
};
