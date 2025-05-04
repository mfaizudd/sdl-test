#include "Globals.h"
#include "Window.h"
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
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>

uint32_t callback(void *param, SDL_TimerID timer_id, uint32_t interval) {
  printf("Callback called: %s\n", reinterpret_cast<char *>(param));
  printf("Waited for %f seconds\n", interval/1000.0);
  return 0;
}

int main(int argc, char *args[]) {
  if (!g_init()) {
    SDL_Log("Failed to initialize\n");
    g_close();
    return 0;
  }

  Window *window = new Window();
  if (!window->init()) {
    delete window;
    g_close();
    return 0;
  }

  SDL_Event e;
  // main loop
  bool quit = false;
  float last_tick = SDL_GetTicks();
  auto timer = SDL_AddTimer(5000, callback, (void *)"Callback njir");
  while (!quit) {
    auto dt = (SDL_GetTicks() - last_tick) / 1000.0f;
    last_tick = SDL_GetTicks();
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
      window->handle_event(&e);
    }

    // do something
    window->update(dt);
    window->render();
  }
  SDL_RemoveTimer(timer);

  delete window;
  g_close();

  return 0;
}
