#include "SDL_blendmode.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "SDL_log.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "Texture.h"
#include <SDL.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
bool init();
bool load_media();
void close();
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
Texture *texture = nullptr;
Texture *background = nullptr;
Texture *sheet = nullptr;

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

  uint8_t r = 0x80;
  uint8_t g = 0x80;
  uint8_t b = 0x80;
  uint8_t a = 0x00;

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_q:
          r += 0x20;
          break;
        case SDLK_w:
          g += 0x20;
          break;
        case SDLK_e:
          b += 0x20;
          break;
        case SDLK_a:
          r -= 0x20;
          break;
        case SDLK_s:
          g -= 0x20;
          break;
        case SDLK_d:
          b -= 0x20;
          break;
        case SDLK_UP:
          a += 0x20;
          break;
        case SDLK_DOWN:
          a -= 0x20;
          break;
        }
      }
    }

    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    background->set_color(r, g, b);
    background->render(0, 0);
    texture->render(240, 190);
    SDL_Rect clip1{0, 0, 100, 100};
    sheet->set_blend_mode(SDL_BLENDMODE_ADD);
    sheet->set_alpha(a);
    sheet->render(0, 0, &clip1);
    SDL_Rect clip2{100, 0, 100, 100};
    sheet->render(SCREEN_WIDTH - 100, 0, &clip2);
    SDL_Rect clip3{0, 100, 100, 100};
    sheet->render(0, SCREEN_HEIGHT - 100, &clip3);
    SDL_Rect clip4{100, 100, 100, 100};
    sheet->render(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, &clip4);
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
  if (g_window == nullptr) {
    SDL_Log("Could not create window. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create renderer
  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
  if (g_renderer == nullptr) {
    SDL_Log("Could not create renderer. Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // Initialize SDL_Image
  int img_flags = IMG_INIT_PNG;
  if (!(IMG_Init(img_flags) & img_flags)) {
    SDL_Log("Could not initialize png image. SDL_Image error: %s\n",
            IMG_GetError());
    return false;
  }

  texture = new Texture(g_renderer);
  background = new Texture(g_renderer);
  sheet = new Texture(g_renderer);

  return true;
}

bool load_media() {
  if (!background->load_from_file("assets/background.png")) {
    return false;
  }
  if (!texture->load_from_file("assets/foo.png")) {
    return false;
  }
  return sheet->load_from_file("assets/sprites.png");
}

void close() {
  texture->free();
  background->free();
  sheet->free();
  SDL_DestroyRenderer(g_renderer);
  g_renderer = nullptr;
  SDL_DestroyWindow(g_window);
  g_window = nullptr;
  IMG_Quit();
  SDL_Quit();
}
