#include "Window.h"
#include "Dot.h"
#include "Globals.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstdio>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <sstream>

Window::Window() {
  m_window = nullptr;
  m_renderer = nullptr;
  m_window_id = 0;

  m_width = 0;
  m_height = 0;

  m_mouse_focus = false;
  m_keyboard_focus = false;
  m_fullscreen = false;
  m_minimized = false;
  m_shown = false;
}

Window::~Window() { free(); }

bool Window::init() {
  auto success =
      SDL_CreateWindowAndRenderer("Window title", SCREEN_WIDTH, SCREEN_HEIGHT,
                                  SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);
  if (!success) {
    SDL_Log("Could create window. Error: %s\n", SDL_GetError());
    return false;
  }

  m_window_id = SDL_GetWindowID(m_window);
  m_display_id = SDL_GetDisplayForWindow(m_window);
  m_shown = true;
  m_width = SCREEN_WIDTH;
  m_height = SCREEN_HEIGHT;

  if (!g_load_media(m_renderer)) {
    SDL_Log("Failed to load media: %s\n", SDL_GetError());
    return false;
  }
  m_bg = std::make_shared<Bg>(m_renderer);
  if (!m_bg->init()) {
    this->free();
    return false;
  }
  m_dot = std::make_shared<Dot>(m_renderer);
  if (!m_dot->init()) {
    this->free();
    return false;
  }
  m_camera = std::make_shared<Camera>();
  if (!g_set_tiles(m_renderer)) {
    this->free();
    return false;
  }
  SDL_SetRenderVSync(m_renderer, 1);
  SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(m_renderer);
  return true;
}

void Window::handle_event(const SDL_Event *e) {
  // bool switch_display = false;
  // if (e->type == SDL_EVENT_KEY_DOWN) {
  //   if (e->key.key == SDLK_UP) {
  //     m_display_id++;
  //     switch_display = true;
  //   } else if (e->key.key == SDLK_DOWN) {
  //     m_display_id--;
  //     switch_display = true;
  //   }
  // }
  m_dot->handle_event(e);
  auto update_title = false;
  // if (switch_display) {
  //   SDL_SetWindowPosition(
  //       m_window,
  //       display_bounds[m_display_id - 1].x +
  //           (display_bounds[m_display_id - 1].w - m_width) / 2.0,
  //       display_bounds[m_display_id - 1].y +
  //           (display_bounds[m_display_id - 1].h - m_height) / 2.0);
  //   update_title = true;
  // }
  if (e->type < SDL_EVENT_WINDOW_FIRST || e->type > SDL_EVENT_WINDOW_LAST) {
    return;
  }
  if (e->window.windowID != m_window_id) {
    return;
  }
  switch (e->type) {
  case SDL_EVENT_WINDOW_MOVED:
    m_display_id = SDL_GetDisplayForWindow(m_window);
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    m_width = e->window.data1;
    m_height = e->window.data2;
    SDL_RenderPresent(m_renderer);
    printf("window resized to %d %d\n", m_width, m_height);
    break;
  case SDL_EVENT_WINDOW_HIDDEN:
    m_shown = false;
    break;
  case SDL_EVENT_WINDOW_EXPOSED:
    SDL_RenderPresent(m_renderer);
    m_shown = true;
    printf("window exposed\n");
    break;
  case SDL_EVENT_WINDOW_MOUSE_ENTER:
    m_mouse_focus = true;
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    m_mouse_focus = false;
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_FOCUS_GAINED:
    m_keyboard_focus = true;
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    m_keyboard_focus = false;
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_MINIMIZED:
    m_minimized = true;
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_MAXIMIZED:
    m_minimized = false;
    update_title = true;
    break;
  case SDL_EVENT_WINDOW_RESTORED:
    m_minimized = false;
    update_title = true;
    break;
  case SDL_EVENT_KEY_DOWN:
    if (e->key.key == SDLK_RETURN) {
      m_fullscreen = !m_fullscreen;
      SDL_SetWindowFullscreen(m_window, m_fullscreen);
      m_minimized = !m_fullscreen;
      printf("set window fullscreen: %b\n", m_fullscreen);
    }
    break;
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    SDL_HideWindow(m_window);
    break;
  }

  if (update_title) {
    std::stringstream title;
    title << "SDL Window - Mouse focus: " << (m_mouse_focus ? "yes" : "no")
          << " Focused: " << (m_keyboard_focus ? "yes" : "no");
    SDL_SetWindowTitle(m_window, title.str().c_str());
    printf("window id %d %s\n", m_window_id, title.str().c_str());
  }
}

void Window::focus() {
  if (!m_shown) {
    SDL_ShowWindow(m_window);
  }

  SDL_RaiseWindow(m_window);
}

void Window::render() {
  if (!m_minimized) {
    // Clear screen
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(m_renderer);

    for (int i = 0; i < TOTAL_TILES; i++) {
      g_tiles(i)->render(m_camera);
    }
    m_dot->render(m_camera);
    // Update screen
    SDL_RenderPresent(m_renderer);
  }
}

void Window::update(float dt) {
  m_dot->update(dt);
  auto pos = m_dot->position();
  auto cam_pos = glm::vec2{pos.x + m_dot->width() / 2.0 - SCREEN_WIDTH / 2.0,
                           pos.y + m_dot->height() / 2.0 - SCREEN_HEIGHT / 2.0};
  if (cam_pos.x < 0) {
    cam_pos.x = 0;
  }
  if (cam_pos.y < 0) {
    cam_pos.y = 0;
  }
  if (cam_pos.x + SCREEN_WIDTH > LEVEL_WIDTH) {
    cam_pos.x = LEVEL_WIDTH - SCREEN_WIDTH;
  }
  if (cam_pos.y + SCREEN_HEIGHT > LEVEL_HEIGHT) {
    cam_pos.y = LEVEL_HEIGHT - SCREEN_HEIGHT;
  }
  m_camera->position(cam_pos);
}

int Window::width() const { return m_width; }
int Window::height() const { return m_height; }

bool Window::has_mouse_focus() const { return m_mouse_focus; }
bool Window::has_keyboard_focus() const { return m_keyboard_focus; }
bool Window::minimized() const { return m_minimized; }
bool Window::shown() const { return m_shown; }

void Window::free() {
  SDL_DestroyRenderer(m_renderer);
  m_renderer = nullptr;

  SDL_DestroyWindow(m_window);
  m_window = nullptr;
}
