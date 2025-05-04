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
SDL_Mutex *g_buffer_lock = nullptr;
SDL_Condition *g_can_produce = nullptr;
SDL_Condition *g_can_consume = nullptr;
int g_data = -1;
void produce() {
  SDL_LockMutex(g_buffer_lock);

  if (g_data != -1) {
    printf("Producer encountered full buffer. Waiting for consumer to empty "
           "buffer...\n");
    SDL_WaitCondition(g_can_produce, g_buffer_lock);
  }

  g_data = rand() % 256;
  printf("Produced %d\n", g_data);

  SDL_UnlockMutex(g_buffer_lock);

  SDL_SignalCondition(g_can_consume);
}

void consume() {
  SDL_LockMutex(g_buffer_lock);

  if (g_data == -1) {
    printf("Consumer encountered empty buffer. Waiting for producer to fill "
           "it...\n");
    SDL_WaitCondition(g_can_consume, g_buffer_lock);
  }

  printf("Consumed %d\n", g_data);
  g_data = -1;
  SDL_UnlockMutex(g_buffer_lock);
  SDL_SignalCondition(g_can_produce);
}

int producer(void *data) {
  printf("Producer started...\n");
  srand(SDL_GetTicks());

  for (int i = 0; i < 5; i++) {
    SDL_Delay(rand() % 1000);
    produce();
  }

  printf("Producer finished\n");
  return 0;
}

int consumer(void *data) {
  printf("Consumer started...\n");
  srand(SDL_GetTicks());
  for (int i = 0; i < 5; i++) {
    SDL_Delay(rand() % 1000);
    consume();
  }

  printf("Consumer finished\n");
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

  g_buffer_lock = SDL_CreateMutex();
  g_can_produce = SDL_CreateCondition();
  g_can_consume = SDL_CreateCondition();

  auto thread_a = SDL_CreateThread(producer, "Producer", (void *)"Producer");
  auto thread_b = SDL_CreateThread(consumer, "Consumer", (void *)"Consumer");

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

  SDL_DestroyMutex(g_buffer_lock);
  g_buffer_lock = nullptr;
  SDL_DestroyCondition(g_can_produce);
  SDL_DestroyCondition(g_can_consume);
  g_can_produce = nullptr;
  g_can_consume = nullptr;
  SDL_WaitThread(thread_a, nullptr);
  thread_a = nullptr;
  SDL_WaitThread(thread_b, nullptr);
  thread_b = nullptr;
  g_close();
  return 0;
}
