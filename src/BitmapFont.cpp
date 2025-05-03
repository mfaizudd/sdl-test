#include "BitmapFont.h"
#include <SDL3/SDL_pixels.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>

BitmapFont::BitmapFont(SDL_Renderer *renderer, SDL_PixelFormat pixel_format)
    : m_font_texture(renderer) {
  m_font_texture.pixel_format = pixel_format;
  m_new_line = 0;
  m_space = 0;
}

BitmapFont::~BitmapFont() { this->free(); }

bool BitmapFont::build_font(std::string path) {
  this->free();
  if (!m_font_texture.load_pixels_from_file(path)) {
    printf("Unable to load bitmap font surface.\n");
    return false;
  }

  auto bg_color = m_font_texture.get_pixel_32(0, 0);
  auto cell_w = m_font_texture.width() / 16;
  auto cell_h = m_font_texture.height() / 16;
  int top = cell_h;
  int base_a = cell_h;
  auto current_char = 0;

  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      m_chars[current_char] = {
          .x = col * cell_w,
          .y = row * cell_h,
          .w = cell_w,
          .h = cell_h,
      };

      for (int x_rel = 0; x_rel < cell_w; x_rel++) {
        for (int y_rel = 0; y_rel < cell_h; y_rel++) {
          auto x_abs = (col * cell_w) + x_rel;
          auto y_abs = (row * cell_h) + y_rel;
          auto pixel = m_font_texture.get_pixel_32(x_abs, y_abs);
          if (pixel != bg_color) {
            continue;
          }

          m_chars[current_char].x = x_abs;
          goto find_right;
        }
      }

    find_right:
      for (int x_rel = cell_w - 1; x_rel >= 0; x_rel--) {
        for (int y_rel = 0; y_rel < cell_h; y_rel++) {
          auto x_abs = (col * cell_w) + x_rel;
          auto y_abs = (row * cell_h) + y_rel;
          auto pixel = m_font_texture.get_pixel_32(x_abs, y_abs);
          if (pixel != bg_color) {
            continue;
          }

          m_chars[current_char].w = x_abs - m_chars[current_char].x + 1;
          goto find_top;
        }
      }

    find_top:
      for (int y_rel = 0; y_rel < cell_h; y_rel++) {
        for (int x_rel = 0; x_rel < cell_w; x_rel++) {
          auto x_abs = (col * cell_w) + x_rel;
          auto y_abs = (row * cell_h) + y_rel;
          auto pixel = m_font_texture.get_pixel_32(x_abs, y_abs);
          if (pixel != bg_color) {
            continue;
          }

          if (y_rel < top) {
            top = y_rel;
          }
          goto find_base;
        }
      }

    find_base:
      if (current_char == 'A') {
        for (int y_rel = cell_h; y_rel >= 0; y_rel--) {
          for (int x_rel = 0; x_rel < cell_w; x_rel++) {
            auto x_abs = (col * cell_w) + x_rel;
            auto y_abs = (row * cell_h) + y_rel;
            auto pixel = m_font_texture.get_pixel_32(x_abs, y_abs);
            if (pixel != bg_color) {
              continue;
            }

            base_a = y_rel;
            goto next;
          }
        }
      }

    next:
      current_char++;
    } // for col
  } // for row

  m_space = cell_w / 2;
  m_new_line = base_a - top;
  for (int i = 0; i < 256; i++) {
    m_chars[i].y += top;
    m_chars[i].h -= top;
  }

  if (!m_font_texture.load_from_pixels()) {
    printf("Unable to create font texture.\n");
    return false;
  }

  return true;
}

void BitmapFont::render_text(int x, int y, std::string text) {
  if (m_font_texture.width() <= 0) {
    printf("The font hasn't been built.\n");
    return;
  }

  int curr_x = x, curr_y = y;
  for (size_t i = 0; i < text.length(); i++) {
    if (text[i] == ' ') {
      curr_x += m_space;
    } else if (text[i] == '\n') {
      curr_y += m_new_line;
      curr_x = x;
    } else {
      m_font_texture.render(curr_x, curr_y, &m_chars[(uint8_t)text[i]]);
      curr_x += m_chars[(uint8_t)text[i]].w + 1;
    }
  }
}

void BitmapFont::free() { m_font_texture.free(); }
