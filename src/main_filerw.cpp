#include "Globals.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_haptic.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
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
#include <cstdio>
#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>

bool init();
void load_inputs();
bool load_media();
void close();
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
TTF_Font *g_font = nullptr;
Texture *texture = nullptr;
Texture *font_texture = nullptr;
Sint32 g_data[TOTAL_DATA];
Texture *g_data_textures[TOTAL_DATA];

int main(int argc, char *args[]) {
  if (!init()) {
    SDL_Log("Failed to initialize\n");
    close();
    return EXIT_FAILURE;
  }

  SDL_IOStream *file = SDL_IOFromFile("assets/nums.bin", "r+b");
  if (file == nullptr) {
    file = SDL_IOFromFile("assets/nums.bin", "w+b");
    printf("New file created.\n");
    for (int i = 0; i < TOTAL_DATA; i++) {
      g_data[i] = 0;
      SDL_WriteIO(file, &g_data[i], sizeof(Sint32));
    }
    if (!SDL_CloseIO(file)) {
      printf("Failed to close file: %s\n", SDL_GetError());
      close();
      return EXIT_FAILURE;
    };
  } else {
    printf("Reading file...\n");
    for (int i = 0; i < TOTAL_DATA; i++) {
      SDL_ReadIO(file, &g_data[i], sizeof(Sint32));
    }
    if (!SDL_CloseIO(file)) {
      printf("Failed to close file: %s\n", SDL_GetError());
      close();
      return EXIT_FAILURE;
    }
  }

  if (!load_media()) {
    SDL_Log("Failed to load media\n");
    close();
    return EXIT_FAILURE;
  }

  SDL_Event e;
  SDL_Color highlight{0xFF, 0, 0, 0xFF};
  SDL_Color normal{0, 0, 0, 0xFF};
  bool quit = false;
  int current_index = 0;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      } else if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.key) {
        case SDLK_UP:
          current_index--;
          if (current_index < 0) {
            current_index = TOTAL_DATA - 1;
          }
          break;
        case SDLK_DOWN:
          current_index++;
          if (current_index == TOTAL_DATA) {
            current_index = 0;
          }
          break;
        case SDLK_RIGHT:
          g_data[current_index]++;
          break;
        case SDLK_LEFT:
          g_data[current_index]--;
          break;
        }
      }
    }
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    // do something
    texture->render(0, 0);
    for (int i = 0; i < TOTAL_DATA; i++) {
      g_data_textures[i]->load_from_rendered_text(
          std::to_string(g_data[i]), i == current_index ? highlight : normal,
          g_font);
      g_data_textures[i]->render(SCREEN_WIDTH / 2.0 -
                                     g_data_textures[i]->width() / 2.0,
                                 i * g_data_textures[i]->height());
    }

    SDL_RenderPresent(g_renderer);
  }

  close();

  return EXIT_SUCCESS;
}

bool init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
                SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
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
#ifdef SDL_TTF_MAJOR_VERSION
  if (!TTF_Init()) {
    SDL_Log("Could not initialize ttf. Error: %s\n", SDL_GetError());
    return false;
  }
#endif // SDL_TTF_MAJOR_VERSION
  SDL_AudioSpec audio_spec{};
  audio_spec.format = MIX_DEFAULT_FORMAT;
  audio_spec.channels = 2;
  audio_spec.freq = 44100;
  if (!Mix_OpenAudio(0, &audio_spec)) {
    SDL_Log("Could not initialize SDL Mixer. SDL Error: %s\n", SDL_GetError());
    return false;
  }
  texture = new Texture(g_renderer);
  font_texture = new Texture(g_renderer);
  for (int i = 0; i < TOTAL_DATA; i++) {
    g_data_textures[i] = new Texture(g_renderer);
  }
  return true;
}

bool load_media() {
  if (!texture->load_from_file("assets/background.png")) {
    return false;
  }
  g_font = TTF_OpenFont("assets/lazy.ttf", 28);
  if (!font_texture->load_from_rendered_text(
          "The quick brown fox jumps over the lazy dog.", SDL_Color{0, 0, 0},
          g_font)) {
    return false;
  }
  return true;
}

void close() {
  delete texture;
  texture = nullptr;
  for (int i = 0; i < TOTAL_DATA; i++) {
    delete g_data_textures[i];
    texture = nullptr;
  }

  auto file = SDL_IOFromFile("assets/nums.bin", "w+b");
  if (file != nullptr) {
    for (int i = 0; i < TOTAL_DATA; i++) {
      SDL_WriteIO(file, &g_data[i], sizeof(Sint32));
    }
    if (!SDL_CloseIO(file)) {
      printf("Failed to close while saving file: %s\n", SDL_GetError());
    }
  } else {
    printf("Unable to save file: %s\n", SDL_GetError());
  }
#ifdef SDL_TTF_MAJOR_VERSION
  delete font_texture;
  font_texture = nullptr;
  TTF_CloseFont(g_font);
  g_font = nullptr;
#endif // SDL_TTF_MAJOR_VERSION
  SDL_DestroyRenderer(g_renderer);
  g_renderer = nullptr;
  SDL_DestroyWindow(g_window);
  g_window = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
  TTF_Quit();
#endif // SDL_TTF_MAJOR_VERSION
  Mix_Quit();
  SDL_Quit();
}
