#include "core/os/application.h"
#include "core/os/sdl2/sdl2_window.h"
#include "core/debug/logger.h"
#include <glad/glad.h>
#include <algorithm>
#include <memory>

Application::Application() {
    backend = std::make_unique<SDL2WindowBackend>();
    running = backend->startup();
}

Application::~Application() {
    windows.clear();
    backend->shutdown();
}

Window* Application::create_window(WindowDesc desc) {
    auto window = std::make_unique<Window>(desc, backend.get());
    if (!window->init()) {
        return nullptr;
    }

    if (!proc_loaded) {
        window->make_current();
        if (!gladLoadGLLoader((GLADloadproc)backend->proc_loader())) {
            LOG_ERROR("Failed to initialize GLAD");
            return nullptr;
        }
        proc_loaded = true;
    }

    Window* ptr = window.get();
    windows.push_back(std::move(window));
    return ptr;
}

void Application::destroy_window(Window* window) {
    windows.erase(
        std::remove_if(windows.begin(), windows.end(),
            [window](const std::unique_ptr<Window>& w) { return w.get() == window; }),
        windows.end()
    );
}

void Application::poll_events() {
    for (const WindowEventData& event : backend->poll_events()) {
        if (event.type == WindowEvent::AppQuit) {
            running = false;
            continue;
        }

        auto it = std::find_if(windows.begin(), windows.end(),
            [&](const std::unique_ptr<Window>& w) {
                return w->id == event.window_id;
            });
        if (it == windows.end()) continue;

        if (event.type == WindowEvent::Resize) {
            (*it)->handle_resize(event.width, event.height);
        } else if (event.type == WindowEvent::CloseRequested) {
            (*it)->handle_close_request();
        }
    }

    // windows.erase(
    //     std::remove_if(windows.begin(), windows.end(),
    //         [](const std::unique_ptr<Window>& w) { return w->should_close(); }),
    //     windows.end()
    // );
}
