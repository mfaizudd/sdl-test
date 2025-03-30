project('sdl-test', 'cpp')
sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('sdl2_image')
deps = [
  sdl2_dep,
  sdl2_image_dep,
]
includes = include_directories(['src'])
sources = [
  'src/main.cpp',
  'src/Texture.cpp'
]
executable('sdl-test', sources, dependencies: deps, include_directories: includes)
