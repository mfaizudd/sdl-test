project('sdl-test', 'cpp')
sdl3_dep = dependency('sdl3')
sdl3_image_dep = dependency('SDL3_image')
sdl3_ttf_dep = dependency('SDL3_ttf')
sdl3_mixer_dep = dependency('SDL3_mixer')
glm_dep = dependency('glm')
glu_dep = dependency('glu')
glew_dep = dependency('glew')
deps = [
  sdl3_dep,
  sdl3_image_dep,
  sdl3_ttf_dep,
  sdl3_mixer_dep,
  glm_dep,
  glu_dep,
  glew_dep,
]
includes = include_directories([
  'src',
])
sources = [
  'src/main_opengl3.cpp',
  'src/Texture.cpp',
  'src/Button.cpp',
  'src/Timer.cpp',
  'src/GameObject.cpp',
  'src/BoxCollider.cpp',
  'src/Transform.cpp',
  'src/CircleCollider.cpp',
  'src/Window.cpp',
  'src/Particle.cpp',
  'src/Globals.cpp',
  'src/Dot.cpp',
  'src/Bg.cpp',
  'src/Camera.cpp',
  'src/Tile.cpp',
  'src/BitmapFont.cpp',
  'src/DataStream.cpp',
]
executable('sdl-test', sources, dependencies: deps, include_directories: includes)
