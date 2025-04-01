project('sdl-test', 'cpp')
sdl3_dep = dependency('sdl3')
sdl3_image_dep = dependency('SDL3_image')
sdl3_ttf_dep = dependency('SDL3_ttf')
deps = [
  sdl3_dep,
  sdl3_image_dep,
  sdl3_ttf_dep,
]
includes = include_directories([
  'src',
])
sources = [
  'src/main.cpp',
  'src/Texture.cpp',
  'src/Button.cpp',
]
executable('sdl-test', sources, dependencies: deps, include_directories: includes)
