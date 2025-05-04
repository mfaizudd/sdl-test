#include "Globals.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_atomic.h>
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
#include <SDL3/SDL_mutex.h>
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
SDL_SpinLock g_data_lock = 0;
int g_data = -1;

int worker(void *data) {
  printf("Starting %s\n", reinterpret_cast<char *>(data));
  srand(SDL_GetTicks());
  for (int i = 0; i < 5; i++) {
    SDL_Delay(16 + rand() % 32);
    SDL_LockSpinlock(&g_data_lock);
    printf("%s gets %d\n", reinterpret_cast<char *>(data), g_data);
    g_data = rand() % 256;
    printf("%s sets %d\n", reinterpret_cast<char *>(data), g_data);
    SDL_UnlockSpinlock(&g_data_lock);
    SDL_Delay(16 + rand() % 640);
  }
  printf("%s finished\n", reinterpret_cast<char *>(data));
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

  auto thread_a = SDL_CreateThread(worker, "Thread A", (void *)"Thread A");
  auto thread_b = SDL_CreateThread(worker, "Thread B", (void *)"Thread B");

  SDL_Event e;
  // main loop
  bool quit = false;
  // float last_tick = SDL_GetTicks();
  while (!quit) {
    // auto tick = SDL_GetTicks();
    // auto dt = (tick - last_tick) / 1000.0f;
    // last_tick = tick;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    fflush(stdout);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    // do something
  }

  SDL_WaitThread(thread_a, nullptr);
  thread_a = nullptr;
  SDL_WaitThread(thread_b, nullptr);
  thread_b = nullptr;
  g_close();
  return 0;
}
