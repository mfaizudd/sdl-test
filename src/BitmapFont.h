#include "Texture.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <string>
class BitmapFont {
public:
  BitmapFont(SDL_Renderer *renderer, SDL_PixelFormat pixel_format);
  ~BitmapFont(); 
  bool build_font(std::string path);
  void free();
  void render_text(int x, int y, std::string text);
private:
  Texture m_font_texture;
  SDL_FRect m_chars[256];
  int m_new_line, m_space;
};
