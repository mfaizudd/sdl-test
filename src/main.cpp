#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_log.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <SDL.h>
#include <stdio.h>
#include <string>

enum KeyPressSurfaces {
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,
  KEY_PRESS_SURFACE_TOTAL
};
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
bool init();
bool loadMedia();
void close();
SDL_Surface *loadSurface(std::string path);
SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gKeyPressSurface[KeyPressSurfaces::KEY_PRESS_SURFACE_TOTAL];
SDL_Surface *gCurrentSurface = NULL;

int main(int argc, char *args[]) {
  if (!init()) {
    SDL_Log("Failed to initialize\n");
  } else {
    if (!loadMedia()) {
      SDL_Log("Failed to load media\n");
    } else {
      SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
      SDL_UpdateWindowSurface(gWindow);
      SDL_Event e;
      bool quit = false;
      while (!quit) {
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) {
            quit = true;
          } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_UP:
              gCurrentSurface = gKeyPressSurface[KEY_PRESS_SURFACE_UP];
              break;
            case SDLK_DOWN:
              gCurrentSurface = gKeyPressSurface[KEY_PRESS_SURFACE_DOWN];
              break;
            case SDLK_LEFT:
              gCurrentSurface = gKeyPressSurface[KEY_PRESS_SURFACE_LEFT];
              break;
            case SDLK_RIGHT:
              gCurrentSurface = gKeyPressSurface[KEY_PRESS_SURFACE_RIGHT];
              break;
            default:
              gCurrentSurface = gKeyPressSurface[KEY_PRESS_SURFACE_DEFAULT];
              break;
            }
          }
          SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
          SDL_UpdateWindowSurface(gWindow);
        }
      }
    }
  }

  close();

  return 0;
}

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
  }

  gWindow = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                             SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (gWindow == NULL) {
    SDL_Log("Could not create window. Error: %s\n", SDL_GetError());
    return false;
  }

  gScreenSurface = SDL_GetWindowSurface(gWindow);
  return true;
}

bool loadMedia() {
  gKeyPressSurface[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("assets/press.bmp");
  if (gKeyPressSurface[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
    return false;
  }
  gKeyPressSurface[KEY_PRESS_SURFACE_UP] = loadSurface("assets/up.bmp");
  if (gKeyPressSurface[KEY_PRESS_SURFACE_UP] == NULL) {
    return false;
  }
  gKeyPressSurface[KEY_PRESS_SURFACE_DOWN] = loadSurface("assets/down.bmp");
  if (gKeyPressSurface[KEY_PRESS_SURFACE_DOWN] == NULL) {
    return false;
  }
  gKeyPressSurface[KEY_PRESS_SURFACE_LEFT] = loadSurface("assets/left.bmp");
  if (gKeyPressSurface[KEY_PRESS_SURFACE_LEFT] == NULL) {
    return false;
  }
  gKeyPressSurface[KEY_PRESS_SURFACE_RIGHT] = loadSurface("assets/right.bmp");
  if (gKeyPressSurface[KEY_PRESS_SURFACE_RIGHT] == NULL) {
    return false;
  }
  return true;
}

SDL_Surface *loadSurface(std::string path) {
  SDL_Surface *optimizedSurface = NULL;
  SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
  if (loadedSurface == NULL) {
    SDL_Log("Unable to load image %s. SDL Error: %s\n", path.c_str(),
            SDL_GetError());
  } else {
    optimizedSurface =
        SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
    if (optimizedSurface == NULL) {
      SDL_Log("Unable to optimize image %s. SDL Error: %s\n", path.c_str(),
              SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
  }

  return optimizedSurface;
}

void close() {
  SDL_FreeSurface(gCurrentSurface);
  gCurrentSurface = NULL;
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  SDL_Quit();
}
