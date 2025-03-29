#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <SDL.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
bool init();
bool loadMedia();
void close();
SDL_Texture *loadTexture(std::string path);
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gTexture = NULL;

int main(int argc, char *args[]) {
  if (!init()) {
    SDL_Log("Failed to initialize\n");
    close();
    return 0;
  }

  if (!loadMedia()) {
    SDL_Log("Failed to load media\n");
    close();
    return 0;
  }

  SDL_UpdateWindowSurface(gWindow);

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);
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
  gWindow = SDL_CreateWindow(
      "SDL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (gWindow == NULL) {
    SDL_Log("Could not create window. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create renderer
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if (gRenderer == NULL) {
    SDL_Log("Could not create renderer. Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // Initialize SDL_Image
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    SDL_Log("Could not initialize png image. SDL_Image error: %s\n",
            IMG_GetError());
    return false;
  }

  return true;
}

bool loadMedia() {
  gTexture = loadTexture("assets/texture.png");
  if (gTexture == NULL) {
    SDL_Log("Failed to load texture.");
    return false;
  }
  return true;
}

SDL_Texture *loadTexture(std::string path) {
  SDL_Texture *newTexture = NULL;
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    SDL_Log("Unable to load image %s. SDL_Image error: %s\n", path.c_str(),
            IMG_GetError());
    return NULL;
  }

  newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
  SDL_FreeSurface(loadedSurface);
  loadedSurface = NULL;
  if (newTexture == NULL) {
    SDL_Log("Unable to create texture from %s. SDL Error: %s\n", path.c_str(),
            SDL_GetError());
    return NULL;
  }

  return newTexture;
}

void close() {
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;
  SDL_DestroyRenderer(gRenderer);
  gRenderer = NULL;
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  IMG_Quit();
  SDL_Quit();
}
