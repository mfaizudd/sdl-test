#pragma once

#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>
class CircleCollider {
public:
  CircleCollider(float x, float y, float radius);
  CircleCollider(std::shared_ptr<Transform> parent, float radius);
  glm::vec2 position() const;
  void position(float x, float y);
  float radius() const;

private:
  glm::vec2 m_position{0,0};
  float m_radius = 0;
  std::optional<std::shared_ptr<Transform>> m_parent = std::nullopt;
};
