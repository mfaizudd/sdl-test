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
#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>

bool init();
void load_inputs();
bool load_media();
void close();
Window *g_windows[TOTAL_WINDOWS] = {};

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

  SDL_Event e;
  // main loop
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }

      for (int i = 0; i < TOTAL_WINDOWS; i++) {
        g_windows[i]->handle_event(&e);
      }

      if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.key) {
        case SDLK_1:
          g_windows[0]->focus();
          break;
        case SDLK_2:
          g_windows[1]->focus();
          break;
        case SDLK_3:
          g_windows[2]->focus();
          break;
        }
      }
    }

    for (int i = 0; i < TOTAL_WINDOWS; i++) {
      g_windows[i]->render();
    }

    bool all_window_closed = true;
    for (const auto &window : g_windows) {
      if (window->shown()) {
        all_window_closed = false;
        break;
      }
    }

    if (all_window_closed) {
      quit = true;
    }
  }

  close();

  return 0;
}

// initialize
bool init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
                SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create window and renderer
  for (int i = 0; i < TOTAL_WINDOWS; i++) {
    g_windows[i] = new Window();
    if (!g_windows[i]->init()) {
      SDL_Log("Could not create window and renderer. Error: %s\n",
              SDL_GetError());
      return false;
    }
  }

  if (!TTF_Init()) {
    SDL_Log("Could not initialize ttf. Error: %s\n", SDL_GetError());
    return false;
  }
  SDL_AudioSpec audio_spec{};
  audio_spec.format = MIX_DEFAULT_FORMAT;
  audio_spec.channels = 2;
  audio_spec.freq = 44100;
  if (!Mix_OpenAudio(0, &audio_spec)) {
    SDL_Log("Could not initialize SDL Mixer. SDL Error: %s\n", SDL_GetError());
    return false;
  }
  return true;
}

// asset loading
bool load_media() { return true; }

// cleanup
void close() {
  for (int i = 0; i < TOTAL_WINDOWS; i++) {
    delete g_windows[i];
    g_windows[i] = nullptr;
  }

  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}
