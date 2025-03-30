#pragma once
#include "SDL_rect.h"
#include "SDL_render.h"
#include <cstdint>
#include <string>

class Texture {
public:
  Texture(SDL_Renderer *renderer);
  ~Texture();
  bool load_from_file(std::string path);
  void free();
  void set_color( uint8_t r, uint8_t g, uint8_t b );
  void render(int x, int y, const SDL_Rect *clip = nullptr);
  int get_width();
  int get_height();

private:
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  int width;
  int height;
};
