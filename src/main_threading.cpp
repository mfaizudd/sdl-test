#include "Globals.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_haptic.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>

bool init();
void load_inputs();
bool load_media();
void close();
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
TTF_Font *g_font = nullptr;
Texture *texture = nullptr;

int thread_func(void *data) {
  printf("Running thread with value: %d\n", (int32_t)(size_t)data);
  return 0;
}

int main(int argc, char *args[]) {
  if (!g_init()) {
    SDL_Log("Failed to initialize\n");
    g_close();
    return 0;
  }

  SDL_Window *window;
  SDL_Renderer *renderer;
  if (!SDL_CreateWindowAndRenderer("Window", SCREEN_WIDTH, SCREEN_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_Log("Failed to create window. Error: %s\n", SDL_GetError());
    g_close();
    return EXIT_FAILURE;
  }

  auto data = 'A';
  auto thread = SDL_CreateThread(thread_func, "TestThread", (void *)(size_t)data);

  SDL_Event e;
  // main loop
  bool quit = false;
  // float last_tick = SDL_GetTicks();
  while (!quit) {
    // auto dt = (SDL_GetTicks() - last_tick) / 1000.0f;
    // last_tick = SDL_GetTicks();
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    // do something
  }

  SDL_WaitThread(thread, nullptr);
  g_close();

  return 0;
}
