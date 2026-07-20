#include "core/os/sdl2/sdl2_window.h"
#include "core/input/input.h"
#include "core/debug/logger.h"
#include "core/os/window.h"
#include "imgui_impl_sdl2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

bool SDL2WindowBackend::startup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR("SDL Init Failed: {}", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    return true;
}

void SDL2WindowBackend::shutdown() {
    SDL_Quit();
}

WindowHandle SDL2WindowBackend::create_window(const WindowDesc& desc) {
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_Window* sdl_window = SDL_CreateWindow(
        desc.title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        desc.width,
        desc.height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!sdl_window) {
        LOG_ERROR("Window Creation Failed: {}", SDL_GetError());
        return WindowHandle{};
    }

#if ENGINE_DEBUG_ENABLED
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);
    if (!gl_context) {
        LOG_ERROR("Context Creation Failed: {}", SDL_GetError());
        SDL_DestroyWindow(sdl_window);
        return WindowHandle{};
    }

    return SDL2HandleData{
        .window = sdl_window,
        .context = gl_context
    };
}

void SDL2WindowBackend::destroy_window(WindowHandle handle) {
    auto& data = std::get<SDL2HandleData>(handle);
    if (data.context) {
        SDL_GL_DeleteContext(data.context);
    }
    if (data.window) {
        SDL_DestroyWindow(data.window);
    }
}

void SDL2WindowBackend::make_current(WindowHandle handle) {
    auto& data = std::get<SDL2HandleData>(handle);
    SDL_GL_MakeCurrent(data.window, data.context);
}

void SDL2WindowBackend::swap_buffers(WindowHandle handle) {
    auto& data = std::get<SDL2HandleData>(handle);
    SDL_GL_SwapWindow(data.window);
}

ProcLoader SDL2WindowBackend::proc_loader() const {
    return (ProcLoader)SDL_GL_GetProcAddress;
}

uint32_t SDL2WindowBackend::get_window_id(WindowHandle handle) const {
    auto& data = std::get<SDL2HandleData>(handle);
    return SDL_GetWindowID(data.window);
}

std::vector<WindowEventData> SDL2WindowBackend::poll_events() {
    std::vector<WindowEventData> events;
    InputSystem::get_singleton().begin_frame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                events.push_back({ WindowEvent::AppQuit });
                break;

            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    events.push_back({
                        WindowEvent::Resize,
                        event.window.windowID,
                        event.window.data1,
                        event.window.data2
                    });
                } else if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    events.push_back({
                        WindowEvent::CloseRequested,
                        event.window.windowID
                    });
                }
                break;
            }
        }
        InputSystem::get_singleton().poll_event(&event);
    }
    return events;
}
