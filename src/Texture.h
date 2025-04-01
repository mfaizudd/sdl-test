#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <string>

class Texture {
public:
  Texture(SDL_Renderer *renderer);
  ~Texture();
  bool load_from_file(std::string path);
  bool load_from_rendered_text(std::string text, SDL_Color color,
                               TTF_Font *font);
  void free();
  void set_color(uint8_t r, uint8_t g, uint8_t b);
  void set_blend_mode(SDL_BlendMode blending);
  void set_alpha(uint8_t alpha);
  void render(float x, float y, const SDL_FRect *clip = nullptr,
              double angle = .0, const SDL_FPoint *center = nullptr,
              SDL_FlipMode flip = SDL_FLIP_NONE);
  float get_width();
  float get_height();

private:
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  float width;
  float height;
};
