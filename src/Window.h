#pragma once

#include "Bg.h"
#include "BitmapFont.h"
#include "Camera.h"
#include "Dot.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <memory>

class Window {
public:
  Window();
  ~Window();
  bool init();
  void handle_event(const SDL_Event *e);
  void focus();
  void render();
  void update(float dt);
  int width() const;
  int height() const;
  bool has_mouse_focus() const;
  bool has_keyboard_focus() const;
  bool minimized() const;
  bool shown() const;

private:
  SDL_Window *m_window;
  SDL_Renderer *m_renderer;
  SDL_WindowID m_window_id;
  SDL_DisplayID m_display_id;
  std::shared_ptr<Camera> m_camera = nullptr;
  std::shared_ptr<Bg> m_bg = nullptr;
  std::shared_ptr<Dot> m_dot = nullptr;
  std::shared_ptr<Texture> m_texture = nullptr;
  std::shared_ptr<BitmapFont> m_bitmap_font = nullptr;

  int m_width;
  int m_height;

  bool m_mouse_focus;
  bool m_keyboard_focus;
  bool m_fullscreen;
  bool m_minimized;
  bool m_shown;

  void free();
};
