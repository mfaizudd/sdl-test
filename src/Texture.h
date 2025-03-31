#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>
#include <cstdint>
#include <string>

class Texture {
public:
  Texture(SDL_Renderer *renderer);
  ~Texture();
  bool load_from_file(std::string path);
  void free();
  void set_color(uint8_t r, uint8_t g, uint8_t b);
  void set_blend_mode(SDL_BlendMode blending);
  void set_alpha(uint8_t alpha);
  void render(float x, float y, const SDL_FRect *clip = nullptr);
  float get_width();
  float get_height();

private:
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  float width;
  float height;
};
