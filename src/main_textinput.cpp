#include "Globals.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_error.h>
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
#include <SDL3/SDL_video.h>
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
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
TTF_Font *g_font = nullptr;
#endif // SDL_TTF_MAJOR_VERSION
Texture *texture = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
Texture *font_texture = nullptr;
#endif // SDL_TTF_MAJOR_VERSION

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
  bool quit = false;
  std::string input_text = "Some text";
  if (!SDL_StartTextInput(g_window)) {
    SDL_Log("Failed to start text input: %s\n", SDL_GetError());
    close();
    return 0;
  }
  // float dt = .0f;
  // float last_tick = SDL_GetTicks() / 1000.0f;
  while (!quit) {
    // dt = (SDL_GetTicks() / 1000.0f) - last_tick;
    // last_tick = SDL_GetTicks() / 1000.0f;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
      if (e.type == SDL_EVENT_KEY_DOWN) {
        if (e.key.key == SDLK_BACKSPACE) {
          if (input_text != "") {
            input_text.pop_back();
          }
        } else if (e.key.key == SDLK_C && SDL_GetModState() & SDL_KMOD_CTRL) {
          SDL_SetClipboardText(input_text.c_str());
        } else if (e.key.key == SDLK_V && SDL_GetModState() & SDL_KMOD_CTRL) {
          auto temp_text = SDL_GetClipboardText();
          input_text = temp_text;
          SDL_free(temp_text);
        }
      } else if (e.type == SDL_EVENT_TEXT_INPUT) {
        if (!(SDL_GetModState() & SDL_KMOD_CTRL &&
              (e.text.text[0] != 'c' || e.text.text[0] != 'C' ||
               e.text.text[0] != 'v' || e.text.text[0] != 'V'))) {
          input_text += e.text.text;
        }
      }
    }
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    // do something
    texture->render(0, 0);
    font_texture->load_from_rendered_text("Enter some text",
                                          SDL_Color{0, 0, 0, 0}, g_font);
    font_texture->render(SCREEN_WIDTH / 2.0 - font_texture->width() / 2.0,
                         SCREEN_HEIGHT / 2.0 - font_texture->height() / 2.0);
    if (input_text != "") {
      font_texture->load_from_rendered_text(input_text.c_str(),
                                            SDL_Color{0, 0, 0, 0}, g_font);
    } else {
      font_texture->load_from_rendered_text(" ", SDL_Color{0, 0, 0, 0}, g_font);
    }
    font_texture->render(SCREEN_WIDTH / 2.0 - font_texture->width() / 2.0,
                         SCREEN_HEIGHT / 2.0 - font_texture->height() / 2.0 +
                             font_texture->height());

    SDL_RenderPresent(g_renderer);
  }

  if (!SDL_StopTextInput(g_window)) {
    SDL_Log("Failed to stop text input: %s\n", SDL_GetError());
  }

  close();

  return 0;
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
#ifdef SDL_TTF_MAJOR_VERSION
  font_texture = new Texture(g_renderer);
#endif // SDL_TTF_MAJOR_VERSION
  return true;
}

bool load_media() {
  if (!texture->load_from_file("assets/background.png")) {
    return false;
  }
#ifdef SDL_TTF_MAJOR_VERSION
  g_font = TTF_OpenFont("assets/lazy.ttf", 28);
  if (!font_texture->load_from_rendered_text(
          "The quick brown fox jumps over the lazy dog.", SDL_Color{0, 0, 0},
          g_font)) {
    return false;
  }
#endif // SDL_TTF_MAJOR_VERSION
  return true;
}

void close() {
  delete texture;
  texture = nullptr;

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
