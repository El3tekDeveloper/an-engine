#include "window.h"

Window::Window(WindowDesc desc, WindowBackend* backend)
    : desc(desc), backend(backend) {}

Window::~Window() {
    shutdown();
}

bool Window::init() {
    handle = backend->create_window(desc);
    id = backend->get_window_id(handle);
 
    backend->make_current(handle);
    return true;
}

void Window::make_current() {
    backend->make_current(handle);
}
 
void Window::swap_buffers() {
    backend->swap_buffers(handle);
}

void Window::shutdown() {
    closing = false;
 
    if (handle) {
        backend->destroy_window(handle);
        handle = {};
    }
}
 
void Window::handle_resize(int width, int height) {
    desc.width = width;
    desc.height = height;
    if (on_resize) on_resize(width, height);
}
 
void Window::handle_close_request() {
    closing = true;
    if (on_close) on_close();
}
