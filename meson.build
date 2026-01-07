project(
  '3d',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)

dependencies = [
  m_dep
]

sources = [
  'src/main.c',
  'src/vector.c',
  'src/loader.c',
  'src/msc.c',
  'src/renderer.c',
  'src/canvas.c',
]

exe = executable(
  '3d',
  sources,
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
