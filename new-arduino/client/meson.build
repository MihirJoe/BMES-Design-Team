project('adapt-client', 'c', default_options: {
  'c_std': 'c23',
  'optimization': '1',
  'warning_level': '3',
})

python_mod = import('python')
threads_dep = dependency('threads')
python_inst = python_mod.find_installation()
adapt_inc_dir = include_directories('../include')

subdir('src')

lib = static_library(
  'adapt_client',
  lib_srcs,
  dependencies: threads_dep,
  include_directories: adapt_inc_dir,
  implicit_include_directories: false,
  c_args: '-D_XOPEN_SOURCE=600',
)
lib_dep = declare_dependency(link_with: lib, include_directories: adapt_inc_dir)

cpyx = python_inst.extension_module(
  '_adapt_client',
  cpyx_srcs,
  dependencies: lib_dep,
  implicit_include_directories: false,
)

executable(
  'adapt_client',
  exe_srcs,
  dependencies: lib_dep,
  include_directories: adapt_inc_dir,
  implicit_include_directories: false,
  c_args: '-D_GNU_SOURCE',
)
