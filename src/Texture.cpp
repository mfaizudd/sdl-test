#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>

Texture::Texture(SDL_Renderer *renderer) : m_renderer(renderer) {
  m_texture = nullptr;
  m_width = 0;
  m_height = 0;
}

Texture::~Texture() { free(); }

bool Texture::load_from_file(std::string path) {
  if (m_renderer == nullptr) {
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

  new_texture = SDL_CreateTextureFromSurface(m_renderer, loaded_surface);
  if (new_texture == nullptr) {
    SDL_DestroySurface(loaded_surface);
    loaded_surface = nullptr;
    SDL_Log("Unable to create texture from %s. SDL Error: %s\n", path.c_str(),
            SDL_GetError());
    return false;
  }
  m_width = loaded_surface->w;
  m_height = loaded_surface->h;
  SDL_DestroySurface(loaded_surface);
  loaded_surface = nullptr;

  m_texture = new_texture;
  return m_texture != nullptr;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool Texture::load_from_rendered_text(std::string text, SDL_Color color,
                                      TTF_Font *font) {
  if (m_renderer == nullptr) {
    SDL_Log("Unable to load rendered text. Renderer is not set.\n");
    return false;
  }
  this->free();
  auto surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
  if (surface == nullptr) {
    SDL_Log("Unable to render text. %s\n", SDL_GetError());
    return false;
  }
  m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
  if (m_texture == nullptr) {
    SDL_Log("Unable to create texture from rendered text. %s\n",
            SDL_GetError());
    SDL_DestroySurface(surface);
    return false;
  }
  m_width = surface->w;
  m_height = surface->h;
  SDL_DestroySurface(surface);
  return true;
}
#endif // defined (SDL_TTF_MAJOR_VERSION)

void Texture::free() {
  if (m_texture == nullptr) {
    return;
  }
  SDL_DestroyTexture(m_texture);
  m_texture = nullptr;
  m_width = 0;
  m_height = 0;
}

void Texture::set_color(uint8_t r, uint8_t g, uint8_t b) {
  SDL_SetTextureColorMod(m_texture, r, g, b);
}

void Texture::set_blend_mode(SDL_BlendMode blending) {
  SDL_SetTextureBlendMode(m_texture, blending);
}

void Texture::set_alpha(uint8_t alpha) {
  SDL_SetTextureAlphaMod(m_texture, alpha);
}

void Texture::render(float x, float y, const SDL_FRect *clip, double angle,
                     const SDL_FPoint *center, SDL_FlipMode flip) {
  if (m_renderer == nullptr) {
    SDL_Log("Unable to render texture. Renderer is not set");
    return;
  }
  SDL_FRect quad{x, y, m_width, m_height};
  if (clip != nullptr) {
    quad.w = clip->w;
    quad.h = clip->h;
  }
  SDL_RenderTextureRotated(m_renderer, m_texture, clip, &quad, angle, center,
                           flip);
}

float Texture::width() { return m_width; }
float Texture::height() { return m_height; }
