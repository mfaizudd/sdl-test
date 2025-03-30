#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "Texture.h"
#include <SDL.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
bool init();
bool load_media();
void close();
SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
Texture *texture = NULL;
Texture *background = NULL;

int main(int argc, char *args[]) {
  if (!init()) {
    SDL_Log("Failed to initialize\n");
    close();
    return 0;
  }

  if (!load_media()) {
    SDL_Log("Failed to load media\n");
    close();
    return 0;
  }

  SDL_UpdateWindowSurface(g_window);

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    background->render(0, 0);
    texture->render(240, 190);
    SDL_RenderPresent(g_renderer);
  }

  close();

  return 0;
}

bool init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create window
  g_window = SDL_CreateWindow(
      "SDL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (g_window == NULL) {
    SDL_Log("Could not create window. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create renderer
  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
  if (g_renderer == NULL) {
    SDL_Log("Could not create renderer. Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // Initialize SDL_Image
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    SDL_Log("Could not initialize png image. SDL_Image error: %s\n",
            IMG_GetError());
    return false;
  }

  texture = new Texture(g_renderer);
  background = new Texture(g_renderer);

  return true;
}

bool load_media() {
  if (!background->load_from_file("assets/background.png")) {
    return false;
  }
  return texture->load_from_file("assets/foo.png");
}

void close() {
  texture->free();
  background->free();
  SDL_DestroyRenderer(g_renderer);
  g_renderer = NULL;
  SDL_DestroyWindow(g_window);
  g_window = NULL;
  IMG_Quit();
  SDL_Quit();
}
