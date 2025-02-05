project('adapt', 'c', default_options: {
  'c_std': 'c23',
  'optimization': '1',
  'warning_level': '3',
})

python_mod = import('python')
threads_dep = dependency('threads')
python_inst = python_mod.find_installation()
adapt_inc_dir = include_directories('include')

subdir('client')

libclient = static_library(
  'adapt_client',
  libclient_srcs,
  dependencies: threads_dep,
  include_directories: adapt_inc_dir,
  implicit_include_directories: false,
  c_args: '-D_XOPEN_SOURCE=600',
)
libclient_dep = declare_dependency(
  link_with: libclient, include_directories: adapt_inc_dir)

client_cpyx = python_inst.extension_module(
  '_adapt_client',
  client_cpyx_srcs,
  dependencies: libclient_dep,
  implicit_include_directories: false,
)

executable(
  'adapt_client',
  client_srcs,
  dependencies: libclient_dep,
  include_directories: adapt_inc_dir,
  implicit_include_directories: false,
  c_args: '-D_GNU_SOURCE',
)
