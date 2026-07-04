#pragma once

class Renderer;

struct Window {
    const char* title;
    int width, height;

    void* handle = nullptr;
    void* context = nullptr;
};

class Platform {
public:
    Platform(Window window) : window(window) {
        init();
    }
    ~Platform()  {
        shutdown();
    }

    bool init();
    void poll_events(Renderer* renderer);
    void swap_buffers();
    void shutdown();

    bool is_running() const { return running; }
    Window& get_window() { return window; };

private:
    Window window;
    bool running = false;
};
