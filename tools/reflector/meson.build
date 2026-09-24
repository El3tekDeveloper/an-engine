llvm_config = find_program('llvm-config', required: true)
python3     = find_program('python3', required: true)

llvm_cxxflags = run_command(llvm_config, '--cxxflags',    check: true).stdout().strip().split()
llvm_ldflags  = run_command(llvm_config, '--ldflags',     check: true).stdout().strip().split()
llvm_syslibs  = run_command(llvm_config, '--system-libs', check: true).stdout().strip().split()
llvm_libs     = run_command(llvm_config, '--libs',        check: true).stdout().strip().split()

flags_to_remove = ['-fno-exceptions', '-fno-rtti', '-std=c++17', '-std=c++14', '-std=c++11']

llvm_cxxflags_filtered = []
foreach flag : llvm_cxxflags
    if flag not in flags_to_remove
        llvm_cxxflags_filtered += flag
    endif
endforeach

llvm_dep = declare_dependency(
    compile_args: llvm_cxxflags_filtered,
    link_args:    llvm_ldflags + llvm_syslibs + llvm_libs + [
        '-lclang-cpp',
    ],
)

src += files(
    'runtime/type_registry.cpp'
)

plugins_inc = custom_target(
    'reflector_plugins_inc',
    output: 'reflector_plugins.inc',
    command: [
        python3,
        files('scripts/plugins_includer.py'),
        meson.project_source_root(),
        '@OUTPUT@',
    ],
    build_always_stale: true,
)

reflect_tool_exe = executable(
    'reflect_tool',
    'tool/main.cpp',
    plugins_inc,
    include_directories: [includes, include_directories('.', 'tool', 'runtime')],
    dependencies: [llvm_dep],
    install: false,
)

