#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
class Window {
public:
  Window();
  ~Window();
  bool init();
  void handle_event(const SDL_Event *e);
  SDL_Renderer *renderer() const;
  int width() const;
  int height() const;
  bool has_mouse_focus() const;
  bool has_keyboard_focus() const;
  bool minimized() const;

private:
  SDL_Window *m_window;
  SDL_Renderer *m_renderer;

  int m_width;
  int m_height;

  bool m_mouse_focus;
  bool m_keyboard_focus;
  bool m_fullscreen;
  bool m_minimized;

  void free();
};
