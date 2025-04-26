#pragma once
#include "BoxCollider.h"
#include "Transform.h"
#include <SDL3/SDL_events.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

class Camera {
public:
  Camera();
  glm::vec2 position() const;
  void position(glm::vec2 position);
  std::shared_ptr<BoxCollider> collider() const;
  float width() const;
  float height() const;
private:
  std::shared_ptr<Transform> m_transform = nullptr;
  std::shared_ptr<BoxCollider> m_collider = nullptr;
};
