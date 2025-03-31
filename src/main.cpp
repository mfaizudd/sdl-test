#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TOTAL_FRAMES = 4;
bool init();
bool load_media();
void close();
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
Texture *texture = nullptr;
Texture *background = nullptr;
Texture *sheet = nullptr;
SDL_FRect sprite_clips[TOTAL_FRAMES];

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
  int frame_index = 0;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      } else if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.key) {
        case SDLK_Q:
          r += 0x20;
          break;
        case SDLK_W:
          g += 0x20;
          break;
        case SDLK_E:
          b += 0x20;
          break;
        case SDLK_A:
          r -= 0x20;
          break;
        case SDLK_S:
          g -= 0x20;
          break;
        case SDLK_D:
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
    SDL_FRect *frame_clip = &sprite_clips[frame_index / 4];
    texture->render(240, 190, frame_clip);
    SDL_FRect clip1{0, 0, 100, 100};
    sheet->set_blend_mode(SDL_BLENDMODE_ADD);
    sheet->set_alpha(a);
    sheet->render(0, 0, &clip1);
    SDL_FRect clip2{100, 0, 100, 100};
    sheet->render(SCREEN_WIDTH - 100, 0, &clip2);
    SDL_FRect clip3{0, 100, 100, 100};
    sheet->render(0, SCREEN_HEIGHT - 100, &clip3);
    SDL_FRect clip4{100, 100, 100, 100};
    sheet->render(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, &clip4);
    SDL_RenderPresent(g_renderer);
    frame_index = (frame_index + 1) % (TOTAL_FRAMES * 4);
  }

  close();

  return 0;
}

bool init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create window and renderer
  if (!SDL_CreateWindowAndRenderer("SDL Test", SCREEN_WIDTH, SCREEN_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &g_window,
                                   &g_renderer)) {
    SDL_Log("Could not create window and renderer. Error: %s\n",
            SDL_GetError());
    return false;
  }

  SDL_SetRenderVSync(g_renderer, 1);
  SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
  for (int i = 0; i < TOTAL_FRAMES; i++) {
    sprite_clips[i].x = i * 64;
    sprite_clips[i].w = 64;
    sprite_clips[i].h = 205;
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
  SDL_Quit();
}
