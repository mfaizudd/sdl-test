#include "Texture.h"
#include <SDL2/SDL_rect.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <cstdint>

Texture::Texture(SDL_Renderer *renderer) : renderer(renderer) {
  this->texture = nullptr;
  this->width = 0;
  this->height = 0;
}

Texture::~Texture() { free(); }

bool Texture::load_from_file(std::string path) {
  if (this->renderer == nullptr) {
    SDL_Log("Unable to load image, renderer is not set.\n");
    return false;
  }
  this->free();

  SDL_Texture *new_texture = nullptr;
  SDL_Surface *loaded_surface = IMG_Load(path.c_str());
  if (loaded_surface == nullptr) {
    SDL_Log("Unable to load image %s. SDL_Image error: %s\n", path.c_str(),
            SDL_GetError());
    return false;
  }

  SDL_SetSurfaceColorKey(loaded_surface, true,
                  SDL_MapSurfaceRGB(loaded_surface, 0, 0xFF, 0xFF));

  new_texture = SDL_CreateTextureFromSurface(this->renderer, loaded_surface);
  if (new_texture == nullptr) {
    SDL_DestroySurface(loaded_surface);
    loaded_surface = nullptr;
    SDL_Log("Unable to create texture from %s. SDL Error: %s\n", path.c_str(),
            SDL_GetError());
    return false;
  }
  this->width = loaded_surface->w;
  this->height = loaded_surface->h;
  SDL_DestroySurface(loaded_surface);
  loaded_surface = nullptr;

  this->texture = new_texture;
  return this->texture != nullptr;
}

void Texture::free() {
  if (this->texture == nullptr) {
    return;
  }
  SDL_DestroyTexture(this->texture);
  this->texture = nullptr;
  this->width = 0;
  this->height = 0;
}

void Texture::set_color(uint8_t r, uint8_t g, uint8_t b) {
  SDL_SetTextureColorMod(this->texture, r, g, b);
}

void Texture::set_blend_mode(SDL_BlendMode blending) {
  SDL_SetTextureBlendMode(this->texture, blending);
}

void Texture::set_alpha(uint8_t alpha) {
  SDL_SetTextureAlphaMod(this->texture, alpha);
}

void Texture::render(float x, float y, const SDL_FRect *clip) {
  if (this->renderer == nullptr) {
    SDL_Log("Unable to render texture. Renderer is not set");
    return;
  }
  SDL_FRect quad{x, y, this->width, this->height};
  if (clip != nullptr) {
    quad.w = clip->w;
    quad.h = clip->h;
  }
  SDL_RenderTexture(this->renderer, this->texture, clip, &quad);
}

float Texture::get_width() { return this->width; }
float Texture::get_height() { return this->height; }
