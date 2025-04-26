#pragma once
#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

class BoxCollider {
public:
  BoxCollider(float width, float height);
  BoxCollider(float x, float y, float width, float height);
  BoxCollider(std::shared_ptr<Transform> parent, float width, float height);
  void set_dimension(float width, float height);
  void set_position(float x, float y);
  float width() const;
  float height() const;
  glm::vec2 position() const;
  glm::vec2 local_position() const;

private:
  glm::vec2 m_position{0, 0};
  float m_width = 0;
  float m_height = 0;
  std::optional<std::shared_ptr<Transform>> m_parent = std::nullopt;
};
