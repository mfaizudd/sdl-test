#include "GameObject.h"
#include "CircleCollider.h"
#include "Globals.h"
#include "Particle.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

GameObject::GameObject(Texture *texture) {
  m_texture = texture;
  m_width = m_texture->width();
  m_height = m_texture->height();
  m_transform = std::make_shared<Transform>();
  m_collider = std::make_shared<CircleCollider>(m_transform, 10);
  m_collider->set_position(m_width / 2.0f, m_height / 2.0f);
  auto pos = m_transform->position();
  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    m_particles[i] = new Particle(pos.x, pos.y);
  }
}

GameObject::~GameObject() { this->free(); }

// TODO: extract this
void GameObject::handle_event(const SDL_Event *e) {
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

void GameObject::position(float x, float y) { m_transform->position(x, y); }

glm::vec2 GameObject::position() { return m_transform->position(); }

void GameObject::update(float dt) {
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

void GameObject::render(glm::vec2 cam_pos) {
  render_particles(cam_pos);
  auto screen_pos = position() - cam_pos;
  if (screen_pos.x + m_width < 0 || screen_pos.x > SCREEN_WIDTH ||
      screen_pos.y + m_height < 0 || screen_pos.y > SCREEN_HEIGHT) {
    return;
  }

  m_texture->render(screen_pos.x, screen_pos.y);
}

float GameObject::width() { return m_width; }
float GameObject::height() { return m_height; }

void GameObject::render_particles(glm::vec2 cam_pos) {
  auto pos = position();

  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    if (m_particles[i]->is_dead()) {
      delete m_particles[i];
      m_particles[i] = new Particle(pos.x, pos.y);
    }
  }

  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    m_particles[i]->render(cam_pos);
  }
}

void GameObject::free() {
  for (int i = 0; i < TOTAL_PARTICLES; i++) {
    delete m_particles[i];
    m_particles[i] = nullptr;
  }

  delete m_texture;
  m_texture = nullptr;
}
