#include "Bg.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

Bg::Bg(SDL_Renderer *renderer) {
  m_renderer = renderer;
  m_texture = std::make_unique<Texture>(renderer);
}

bool Bg::init() {
  if (!m_texture->load_from_file("assets/background.png")) {
    return false;
  }
  m_width = m_texture->width();
  m_height = m_texture->height();
  return true;
}

void Bg::handle_event(const SDL_Event *e) {}

void Bg::update(float dt) {}

void Bg::render(std::shared_ptr<Camera> camera) {
  auto screen_pos = position() - camera->position();
  m_texture->render(screen_pos.x, screen_pos.y);
}
