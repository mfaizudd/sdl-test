#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

Texture::Texture(SDL_Renderer *renderer) : m_renderer(renderer) {
  m_texture = nullptr;
  m_width = 0;
  m_height = 0;
  m_surface_pixels = nullptr;
}

Texture::~Texture() { free(); }

bool Texture::load_from_file(std::string path) {
  if (m_renderer == nullptr) {
    SDL_Log("Unable to load image, renderer is not set.\n");
    return false;
  }

  if (!load_pixels_from_file(path)) {
    return false;
  }

  if (!load_from_pixels()) {
    return false;
  }

  return m_texture != nullptr;
}

bool Texture::load_pixels_from_file(std::string path) {
  if (!pixel_format.has_value()) {
    SDL_Surface *loaded_surface = IMG_Load(path.c_str());
    if (loaded_surface == nullptr) {
      SDL_Log("Unable to load image %s. SDL_Image error: %s\n", path.c_str(),
              SDL_GetError());
      return false;
    }

    if (!SDL_SetSurfaceColorKey(
            loaded_surface, true,
            SDL_MapSurfaceRGB(loaded_surface, 0, 0xFF, 0xFF))) {
      SDL_Log("Unable to set surface color key: %s\n", SDL_GetError());
      return false;
    }
    m_texture = SDL_CreateTextureFromSurface(m_renderer, loaded_surface);
    SDL_DestroySurface(loaded_surface);
    if (m_texture == nullptr) {
      SDL_Log("Unable to create texture from surface: %s\n", SDL_GetError());
      return false;
    }

    m_width = m_texture->w;
    m_height = m_texture->h;
    return true;
  }
  this->free();
  SDL_Surface *loaded_surface = IMG_Load(path.c_str());
  if (loaded_surface == nullptr) {
    SDL_Log("Unable to load image %s. SDL_Image error: %s\n", path.c_str(),
            SDL_GetError());
    return false;
  }

  m_surface_pixels = SDL_ConvertSurface(loaded_surface, pixel_format.value());
  SDL_DestroySurface(loaded_surface);
  if (m_surface_pixels == nullptr) {
    SDL_Log("Unable to convert surface to display format. SDL Error: %s\n",
            SDL_GetError());
    return false;
  }

  m_width = m_surface_pixels->w;
  m_height = m_surface_pixels->h;
  return true;
}

bool Texture::load_from_pixels() {
  if (m_surface_pixels == nullptr) {
    printf("No pixels loaded\n");
    return true;
  }

  SDL_SetSurfaceColorKey(m_surface_pixels, true,
                         SDL_MapSurfaceRGB(m_surface_pixels, 0, 0xFF, 0xFF));

  m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface_pixels);
  if (m_texture == nullptr) {
    SDL_Log("Unable to create texture from loaded pixels. %s\n",
            SDL_GetError());
    SDL_DestroySurface(m_surface_pixels);
    m_surface_pixels = nullptr;
    return false;
  }

  m_width = m_texture->w;
  m_height = m_texture->h;

  SDL_DestroySurface(m_surface_pixels);
  m_surface_pixels = nullptr;
  return true;
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

bool Texture::create_blank(int width, int height, SDL_TextureAccess access) {
  this->free();

  m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                access, width, height);
  if (m_texture == nullptr) {
    return false;
  }

  m_width = width;
  m_height = height;
  return true;
}

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

void Texture::set_as_render_target() {
  SDL_SetRenderTarget(m_renderer, m_texture);
}

float Texture::width() { return m_width; }
float Texture::height() { return m_height; }

uint32_t *Texture::get_pixels_32() {
  if (m_surface_pixels == nullptr) {
    return nullptr;
  }
  return static_cast<uint32_t *>(m_surface_pixels->pixels);
}
uint32_t Texture::get_pixel_32(uint32_t x, uint32_t y) {
  auto pixels = static_cast<uint32_t *>(m_surface_pixels->pixels);
  return pixels[y * get_pitch_32() + x];
}
uint32_t Texture::get_pitch_32() {
  if (m_surface_pixels == nullptr) {
    return 0;
  }
  return m_surface_pixels->pitch / 4;
}
uint32_t Texture::map_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  if (m_surface_pixels == nullptr) {
    return 0;
  }

  auto pixel_format_details =
      SDL_GetPixelFormatDetails(m_surface_pixels->format);
  if (pixel_format_details == nullptr) {
    SDL_Log("Unable to get pixel format details: %s\n", SDL_GetError());
    return 0;
  }
  return SDL_MapRGBA(pixel_format_details, nullptr, r, g, b, a);
}

void Texture::copy_raw_pixels32(void *pixels) {
  if (m_raw_pixels == nullptr) {
    return;
  }

  memcpy(m_raw_pixels, pixels, m_raw_pitch * m_height);
}

bool Texture::lock_texture() {
  if (m_raw_pixels != nullptr) {
    printf("Texture is already locked.\n");
    return false;
  }

  if (!SDL_LockTexture(m_texture, nullptr, &m_raw_pixels, &m_raw_pitch)) {
    SDL_Log("Unable to log texture: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

bool Texture::unlock_texture() {
  if (m_raw_pixels == nullptr) {
    printf("Texture is not locked.\n");
    return false;
  }

  SDL_UnlockTexture(m_texture);
  m_raw_pixels = nullptr;
  m_raw_pitch = 0;
  return true;
}
