#include "core/input/input.h"
#include "core/os/platform.h"
#include "render/renderer.h"
#include "core/debug/logger.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <glad/glad.h>

bool Platform::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR("SDL Init Failed: {}", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window.handle = SDL_CreateWindow(
        window.title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window.width,
        window.height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window.handle) {
        LOG_ERROR("Window Creation Failed: {}", SDL_GetError());
        return false;
    }
    
#if ENGINE_DEBUG_ENABLED
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    window.context = SDL_GL_CreateContext((SDL_Window*)window.handle);
    if (!window.context) {
        LOG_ERROR("Context Creation Failed: {}", SDL_GetError());
        return false;
    }
    SDL_GL_MakeCurrent((SDL_Window*)window.handle, window.context);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        return false;
    }

    running = true;
    return true;
}

void Platform::poll_events(Renderer* renderer) {
    InputSystem::get_singleton().begin_frame();
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    window.width = event.window.data1;
                    window.height = event.window.data2;
                    renderer->resize(window.width, window.height);
                }
                break;
        }

        InputSystem::get_singleton().poll_event(&event);
    }
}

void Platform::swap_buffers() {
    SDL_GL_SwapWindow((SDL_Window*)window.handle);
}

void Platform::shutdown() {
    running = false;
    if (window.context) {
        SDL_GL_DeleteContext((SDL_GLContext)window.context);
        window.context = nullptr;
    }
    if (window.handle) {
        SDL_DestroyWindow((SDL_Window*)window.handle);
        window.handle = nullptr;
    }
    SDL_Quit();
}
