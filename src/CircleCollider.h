#pragma once

#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>
class CircleCollider {
public:
  CircleCollider(float x, float y, float radius);
  CircleCollider(std::shared_ptr<Transform> parent, float radius);
  void set_position(float x, float y);
  float get_radius() const;
  glm::vec2 get_position() const;

private:
  glm::vec2 position{0,0};
  float radius = 0;
  std::optional<std::shared_ptr<Transform>> parent = std::nullopt;
};
