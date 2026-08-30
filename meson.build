project(
    'an-engine',
    ['cpp', 'c'],
    version: '0.1.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=1',
        'buildtype=debug'
    ]
)

add_project_arguments(
    '-DENGINE_DEBUG_ENABLED=' + (get_option('debug') ? '1' : '0'),
    language: ['cpp', 'c']
)

src = []
thirdparty_src = []
includes = [include_directories('.')]

subdir('thirdparty')
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
    dependency('luajit'),
]

executable(
    'an-engine',
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
