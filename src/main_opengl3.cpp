#include "Globals.h"
#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_video.h>
#include <cstdio>

SDL_Window *g_window = nullptr;
SDL_GLContext g_context = nullptr;
bool g_render_quad = false;
GLuint g_program_id = 0;
GLint g_vertex_pos2d_loc = -1;
GLuint g_vbo = 0;
GLuint g_ibo = 0;

void print_program_log(GLuint program) {
  if (!glIsProgram(program)) {
    printf("Name %d is not a program\n", program);
    return;
  }

  int info_log_length = 0;
  int max_length = info_log_length;

  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

  char *info_log = new char[max_length];
  glGetProgramInfoLog(program, max_length, &info_log_length, info_log);
  if (info_log_length > 0) {
    printf("%s\n", info_log);
  }
  delete[] info_log;
}
void print_shader_log(GLuint shader) {
  if (!glIsShader(shader)) {
    printf("Name %d is not a shader\n", shader);
    return;
  }

  int info_log_length = 0;
  int max_length = info_log_length;

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

  char *info_log = new char[max_length];
  glGetShaderInfoLog(shader, max_length, &info_log_length, info_log);
  if (info_log_length > 0) {
    printf("%s\n", info_log);
  }
  delete[] info_log;
}

bool init() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("Unable to initialize sdl. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  g_window = SDL_CreateWindow("Title", SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (g_window == nullptr) {
    printf("Unable to create sdl window. Error: %s\n", SDL_GetError());
    return false;
  }

  g_context = SDL_GL_CreateContext(g_window);
  if (g_context == nullptr) {
    printf("Unable to create gl context. Error: %s\n", SDL_GetError());
    return false;
  }

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    printf("Unable to initialize glew: %s\n", glewGetErrorString(glewError));
    return false;
  }

  if (!SDL_GL_SetSwapInterval(-1)) {
    printf("Adaptive vsync not supported. Falling back to vsync.\n");

    if (!SDL_GL_SetSwapInterval(1)) {
      printf("Unable to enable vsync. %s\n", SDL_GetError());
      return false;
    }
  }

  return true;
}

bool init_gl() {
  g_program_id = glCreateProgram();
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  const GLchar *vertex_shader_source[] = {
      "#version 140\n"
      "in vec2 LVertexPos2D;"
      "void main() {"
      "  gl_Position = vec4(LVertexPos2D.x, LVertexPos2D.y, 0, 1);"
      "}",
  };
  glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
  glCompileShader(vertex_shader);
  GLint v_shader_compiled = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &v_shader_compiled);
  if (v_shader_compiled != GL_TRUE) {
    printf("Unable to compile vertex shader %d\n", vertex_shader);
    print_shader_log(vertex_shader);
    return false;
  }

  glAttachShader(g_program_id, vertex_shader);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  const GLchar *fragment_shader_source[] = {
      "#version 140\n"
      "out vec4 LFragment;"
      "void main() {"
      "  LFragment = vec4(1.0, 1.0, 1.0, 1.0);"
      "}"};

  glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
  glCompileShader(fragment_shader);
  GLint f_shader_compiled = GL_FALSE;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &f_shader_compiled);
  if (f_shader_compiled != GL_TRUE) {
    printf("Unable to compile fragment shader %d\n", fragment_shader);
    print_shader_log(fragment_shader);
    return false;
  }

  glAttachShader(g_program_id, fragment_shader);

  glLinkProgram(g_program_id);
  GLint program_success = GL_TRUE;
  glGetProgramiv(g_program_id, GL_LINK_STATUS, &program_success);
  if (program_success != GL_TRUE) {
    printf("Error linking program. %d\n", g_program_id);
    print_program_log(g_program_id);
    return false;
  }

  g_vertex_pos2d_loc = glGetAttribLocation(g_program_id, "LVertexPos2D");
  if (g_vertex_pos2d_loc == -1) {
    printf("LVertexPos2D is not a valid glsl program variable.\n");
    return false;
  }

  glClearColor(0.f, 0.f, 0.f, 1.f);

  GLfloat vertex_data[] = {
      -.5f, -.5f, .5f, -.5f, .5f, .5f, -.5f, .5f,
  };

  GLuint index_data[] = {0, 1, 2, 3};

  glGenBuffers(1, &g_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertex_data,
               GL_STATIC_DRAW);

  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data,
               GL_STATIC_DRAW);

  return true;
}

void handle_keys(uint8_t key, uint32_t x, uint32_t y) {
  if (key == 'q') {
    g_render_quad = !g_render_quad;
  }
}

void update() {}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);

  if (g_render_quad) {
    glUseProgram(g_program_id);
    glEnableVertexAttribArray(g_vertex_pos2d_loc);

    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glVertexAttribPointer(g_vertex_pos2d_loc, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(g_vertex_pos2d_loc);
    glUseProgram(0);
  }
}

void close() {
  glDeleteProgram(g_program_id);
  SDL_GL_DestroyContext(g_context);
  g_context = nullptr;
  SDL_DestroyWindow(g_window);
  g_window = nullptr;
  SDL_Quit();
}

int main(int argc, char *args[]) {
  if (!init()) {
    close();
    return 1;
  }

  if (!init_gl()) {
    close();
    return 1;
  }

  SDL_StartTextInput(g_window);
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      } else if (e.type == SDL_EVENT_TEXT_INPUT) {
        float x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        handle_keys(e.text.text[0], x, y);
      }
    }

    render();
    SDL_GL_SwapWindow(g_window);
  }

  SDL_StopTextInput(g_window);
  close();
  return 0;
}
