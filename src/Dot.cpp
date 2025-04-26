#include "Dot.h"
#include "Globals.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

Dot::Dot(SDL_Renderer *renderer) {
  m_renderer = renderer;
  m_texture = std::make_unique<Texture>(m_renderer);
  m_collider = std::make_shared<CircleCollider>(m_transform, 10);
  m_collider->position(m_width / 2.0f, m_height / 2.0f);
  auto pos = m_transform->position();
  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    m_particles[i] = std::make_shared<Particle>(pos.x, pos.y);
  }
}

bool Dot::init() {
  if (!m_texture->load_from_file("assets/dot.png")) {
    return false;
  }
  m_width = m_texture->width();
  m_height = m_texture->height();
  return true;
}

void Dot::handle_event(const SDL_Event *e) {
  if (e->type == SDL_EVENT_KEY_DOWN && !e->key.repeat) {
    switch (e->key.key) {
    case SDLK_UP:
      m_velocity.y -= DOT_VEL;
      break;
    case SDLK_DOWN:
      m_velocity.y += DOT_VEL;
      break;
    case SDLK_LEFT:
      m_velocity.x -= DOT_VEL;
      break;
    case SDLK_RIGHT:
      m_velocity.x += DOT_VEL;
      break;
    }
  } else if (e->type == SDL_EVENT_KEY_UP && !e->key.repeat) {
    switch (e->key.key) {
    case SDLK_UP:
      m_velocity.y += DOT_VEL;
      break;
    case SDLK_DOWN:
      m_velocity.y -= DOT_VEL;
      break;
    case SDLK_LEFT:
      m_velocity.x += DOT_VEL;
      break;
    case SDLK_RIGHT:
      m_velocity.x -= DOT_VEL;
      break;
    }
  }
}

void Dot::update(float dt) {
  auto position = m_transform->position();
  auto moved = position + m_velocity * dt;
  m_transform->position(moved.x, moved.y);
  if (moved.x < 0 || moved.x + m_width > LEVEL_WIDTH ||
      check_collision(m_collider, g_wall)) {
    moved.x -= m_velocity.x * dt;
  }
  if (moved.y < 0 || moved.y + m_height > LEVEL_HEIGHT ||
      check_collision(m_collider, g_wall)) {
    moved.y -= m_velocity.y * dt;
  }
  m_transform->position(moved.x, moved.y);
}

void Dot::render(std::shared_ptr<Camera> camera) {
  auto cam_pos = camera->position();
  render_particles(cam_pos);
  auto screen_pos = position() - cam_pos;
  if (screen_pos.x + m_width < 0 || screen_pos.x > SCREEN_WIDTH ||
      screen_pos.y + m_height < 0 || screen_pos.y > SCREEN_HEIGHT) {
    return;
  }

  m_texture->render(screen_pos.x, screen_pos.y);
}

void Dot::render_particles(glm::vec2 cam_pos) {
  auto pos = position();

  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    if (m_particles[i]->is_dead()) {
      m_particles[i] = std::make_shared<Particle>(pos.x, pos.y);
    }
  }

  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    m_particles[i]->render(cam_pos);
  }
}
