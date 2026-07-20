project(
    'cpp-kart',
    ['cpp', 'c'],
    version: '0.1.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=1',
        'buildtype=debug'
    ]
)

add_project_arguments('-DENGINE_DEBUG_ENABLED=1', language: ['cpp', 'c'])

src = []

thirdparty_src = [
    'thirdparty/glad/src/glad.c',
    'thirdparty/stb/stb_impl.cpp',
    'thirdparty/cgltf/cgltf_impl.cpp',

    'thirdparty/imgui/src/imgui.cpp',
    'thirdparty/imgui/src/imgui_demo.cpp',
    'thirdparty/imgui/src/imgui_draw.cpp',
    'thirdparty/imgui/src/imgui_impl_opengl3.cpp',
    'thirdparty/imgui/src/imgui_impl_sdl2.cpp',
    'thirdparty/imgui/src/imgui_tables.cpp',
    'thirdparty/imgui/src/imgui_widgets.cpp',
]

includes = include_directories(
    '.',
    'thirdparty/glad/include',
    'thirdparty/stb',
    'thirdparty/cgltf',
    '/usr/include/luajit-2.1',
    'thirdparty/sol2/include',
    'thirdparty/imgui/include'
)

subdir('tools')
subdir('core')
subdir('resources')
subdir('render')
subdir('scene2')
subdir('main')

deps = [
    dependency('sdl2'),
    dependency('gl'),
    dependency('freetype2'),
    dependency('fmt'),
    dependency('luajit'),
]

executable(
    'Game',
    src + thirdparty_src,
    include_directories: includes,
    dependencies: deps,
    cpp_args: [
        '-fplugin=' + reflect_lib.full_path(),
        '-D__REFLECT_GENERATED__',
    ],
    link_depends: reflect_plugin_tgt,
    install: false,
)
