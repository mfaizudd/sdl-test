#include "Texture.h"
#include "SDL_image.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"

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
            IMG_GetError());
    return false;
  }

  SDL_SetColorKey(loaded_surface, SDL_TRUE,
                  SDL_MapRGB(loaded_surface->format, 0, 0xFF, 0xFF));

  new_texture = SDL_CreateTextureFromSurface(this->renderer, loaded_surface);
  if (new_texture == nullptr) {
    SDL_FreeSurface(loaded_surface);
    loaded_surface = nullptr;
    SDL_Log("Unable to create texture from %s. SDL Error: %s\n", path.c_str(),
            SDL_GetError());
    return false;
  }
  this->width = loaded_surface->w;
  this->height = loaded_surface->h;
  SDL_FreeSurface(loaded_surface);
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

void Texture::render(int x, int y) {
  if (this->renderer == nullptr) {
    SDL_Log("Unable to render texture. Renderer is not set");
    return;
  }
  SDL_Rect quad{x, y, this->width, this->height};
  SDL_RenderSetViewport(this->renderer, &quad);
  SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
}

int Texture::get_width() { return this->width; }
int Texture::get_height() { return this->height; }
