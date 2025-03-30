#pragma once
#include "SDL_render.h"
#include <string>

class Texture {
public:
  Texture(SDL_Renderer *renderer);
  ~Texture();
  bool load_from_file(std::string path);
  void free();
  void render(int x, int y);
  int get_width();
  int get_height();

private:
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  int width;
  int height;
};
