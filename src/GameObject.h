#pragma once
#include "CircleCollider.h"
#include "Particle.h"
#include "Texture.h"
#include "Transform.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

class GameObject {
public:
  static const int DOT_VEL = 200;
  GameObject(Texture *texture);
  ~GameObject();
  void handle_event(const SDL_Event *e);
  void position(float x, float y);
  glm::vec2 position();
  void update(float dt);
  void render(glm::vec2 cam_pos);
  float width();
  float height();

private:
  Particle *m_particles[TOTAL_PARTICLES];
  Texture *m_texture = nullptr;
  float m_width;
  float m_height;
  std::shared_ptr<Transform> m_transform = nullptr;
  std::shared_ptr<CircleCollider> m_collider = nullptr;
  glm::vec2 m_velocity{0, 0};

  void render_particles(glm::vec2 cam_pos);
  void free();
};
