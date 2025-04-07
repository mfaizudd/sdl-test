#pragma once
#include "CircleCollider.h"
#include "Texture.h"
#include "Transform.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>
class Dot {
public:
  static const int DOT_WIDTH = 20;
  static const int DOT_HEIGHT = 20;
  static const int DOT_VEL = 200;
  Dot(Texture *texture);
  void handle_event(const SDL_Event *e);
  void set_position(float x, float y);
  void update(float dt);

private:
  Texture *texture = nullptr;
  std::shared_ptr<Transform> transform = nullptr;
  std::shared_ptr<CircleCollider> collider = nullptr;
  glm::vec2 velocity{0, 0};
};
