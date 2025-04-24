#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

Transform::Transform() : m_parent(std::nullopt) {}

Transform::Transform(std::shared_ptr<Transform> parent) : m_parent(parent) {}

void Transform::position(float x, float y) {
  m_position.x = x;
  m_position.y = y;
}

glm::vec2 Transform::position() const { return m_position; }

std::optional<std::shared_ptr<Transform>> Transform::parent() const {
  return m_parent;
}
