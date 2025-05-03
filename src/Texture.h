#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <optional>
#include <string>

class Texture {
public:
  Texture(SDL_Renderer *renderer);
  ~Texture();
  bool load_from_file(std::string path);
  bool load_pixels_from_file(std::string path);
  bool load_from_pixels();
#if defined(SDL_TTF_MAJOR_VERSION)
  bool load_from_rendered_text(std::string text, SDL_Color color,
                               TTF_Font *font);
#endif // defined (SDL_TTF_MAJOR_VERSION)
  void free();
  void set_color(uint8_t r, uint8_t g, uint8_t b);
  void set_blend_mode(SDL_BlendMode blending);
  void set_alpha(uint8_t alpha);
  void render(float x, float y, const SDL_FRect *clip = nullptr,
              double angle = .0, const SDL_FPoint *center = nullptr,
              SDL_FlipMode flip = SDL_FLIP_NONE);
  float width();
  float height();
  uint32_t *get_pixels_32();
  uint32_t get_pixel_32(uint32_t x, uint32_t y);
  uint32_t get_pitch_32();
  uint32_t map_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

  std::optional<SDL_PixelFormat> pixel_format = std::nullopt;

private:
  SDL_Texture *m_texture;
  SDL_Renderer *m_renderer;
  SDL_Surface *m_surface_pixels;
  float m_width;
  float m_height;
};
