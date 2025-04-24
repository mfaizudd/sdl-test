#include "Texture.h"
#include <SDL3/SDL_render.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

TTF_Font *font = nullptr;
Texture *font_texture = nullptr;
Texture *texture_red = nullptr;
Texture *texture_green = nullptr;
Texture *texture_blue = nullptr;
Texture *texture_shimmer = nullptr;

bool g_init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
                SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
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

bool g_load_media(SDL_Renderer *renderer) {
  font_texture = new Texture(renderer);
  texture_red = new Texture(renderer);
  texture_green = new Texture(renderer);
  texture_blue = new Texture(renderer);
  texture_shimmer = new Texture(renderer);
  font = TTF_OpenFont("assets/lazy.ttf", 28);
  if (!font_texture->load_from_rendered_text(
          "The quick brown fox jumps over the lazy dog.", SDL_Color{0, 0, 0},
          font)) {
    return false;
  }
  if (!texture_red->load_from_file("assets/red.bmp")) {
    return false;
  }
  if (!texture_green->load_from_file("assets/green.bmp")) {
    return false;
  }
  if (!texture_blue->load_from_file("assets/blue.bmp")) {
    return false;
  }
  if (!texture_shimmer->load_from_file("assets/shimmer.bmp")) {
    return false;
  }
  texture_red->set_alpha(192);
  texture_green->set_alpha(192);
  texture_blue->set_alpha(192);
  texture_shimmer->set_alpha(192);
  return true;
}

Texture *g_texture_red() {
  return texture_red;
}

Texture *g_texture_green() {
  return texture_green;
}

Texture *g_texture_blue() {
  return texture_blue;
}

Texture *g_texture_shimmer() {
  return texture_shimmer;
}

void g_close() {
  delete texture_red;
  texture_red = nullptr;
  delete texture_green;
  texture_green = nullptr;
  delete texture_blue;
  texture_blue = nullptr;
  delete texture_shimmer;
  texture_shimmer = nullptr;
  delete font_texture;
  font_texture = nullptr;
  TTF_CloseFont(font);
  font = nullptr;

  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}
