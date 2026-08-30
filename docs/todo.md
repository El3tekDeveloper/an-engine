# Engine TODO

## Renderer
- [ ] Redesign renderer architecture.
- [ ] Vulkan backend.
- [x] OpenGL backend.
- [x] SSBO-based rendering.
- [x] GPU handles.
- [x] Shared CPU/GPU resources.
- [x] Automatic shader generation.

## Reflection
- [x] Automatic reflection generation.
- [x] Runtime reflection.
- [x] Plugin system.
- [ ] Improve runtime reflection.
- [ ] Automatic Lua bindings.

## ECS
- [x] Basic ECS.
- [ ] Archetypes.
- [ ] Better memory management.
- [ ] Query optimization.

## OS
- [x] Redesign application system.
- [x] Multiple windows.
- [x] Viewports.

## Resources
- [x] GPU handle system.
- [x] Resource Save & Load
- [ ] Resource lifetime management.

## Scripting
- [x] Lua scripting.
- [ ] Evaluate alternatives to Lua.
- [x] Automatic component bindings.
- [ ] Automatic engine bindings.
- [ ] Better native C++ workflow.

## Input
- [x] Keyboard input.
- [x] Mouse input.
- [ ] Input map.
- [ ] Gamepad support.

## Math
- [ ] Cleanup math utilities.

## Memory
- [x] Bump allocator.
- [ ] More allocators.

## IO
- [x] Virtual filesystem (`assets://`, `engine://`, ..).

## Logger
- [ ] Improve logger.
- [ ] Editor console.

## Editor
- [ ] Start editor development.
- [ ] Scene editor.
- [ ] Inspector.
- [ ] Asset browser.
- [ ] Built-in IDE
- [ ] Lua IDE support.
- [ ] C++ IDE support.
- [ ] Automatic build pipeline.
- [ ] Clang integration.
- [ ] Clangd integration.
- [ ] Bundle toolchain (Clang, Clangd, Meson, Ninja).

## Goal
- [x] Native C++ as the primary language.
- [x] Lua for gameplay logic.
- [ ] Zero manual bindings.
- [ ] Zero manual registration.
- [ ] Zero toolchain setup.
