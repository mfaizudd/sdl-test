#pragma once

#include "CircleCollider.h"
#include "GameObject.h"
#include "Particle.h"
#include <SDL3/SDL_events.h>
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <memory>

class Dot : public GameObject {
public:
  Dot(SDL_Renderer *renderer);
  bool init() override;
  void handle_event(const SDL_Event *e) override;
  void update(float dt) override;
  void render(std::shared_ptr<Camera> camera) override;

private:
  SDL_Renderer *m_renderer = nullptr;
  static const int DOT_VEL = 200;
  std::shared_ptr<Texture> m_texture = nullptr;
  std::shared_ptr<Particle> m_particles[TOTAL_PARTICLES];
  std::shared_ptr<CircleCollider> m_collider = nullptr;
  void render_particles(glm::vec2 cam_pos);
};
