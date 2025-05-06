#include "Globals.h"
#include <GL/glu.h>
#include <SDL3/SDL_video.h>

SDL_Window *g_window = nullptr;
SDL_GLContext g_context = nullptr;
bool g_render_quad = false;

bool init() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("Unable to initialize sdl. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

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
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  auto error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing opengl. %s\n", gluErrorString(error));
    return false;
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing opengl. %s\n", gluErrorString(error));
    return false;
  }

  glClearColor(0.f, 0.f, 0.f, 1.f);
  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing opengl. %s\n", gluErrorString(error));
    return false;
  }

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
    glBegin(GL_QUADS);
    {
      glVertex2f(-.5f, -.5f);
      glVertex2f(.5f, -.5f);
      glVertex2f(.5f, .5f);
      glVertex2f(-.5f, .5f);
    }
    glEnd();
  }
}

void close() {
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
