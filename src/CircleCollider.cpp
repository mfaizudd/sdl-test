#include "CircleCollider.h"
#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>

CircleCollider::CircleCollider(float x, float y, float radius) {
  position(x, y);
  m_radius = radius;
}

CircleCollider::CircleCollider(std::shared_ptr<Transform> parent,
                               float radius) {
  position(0, 0);
  m_parent = parent;
  m_radius = radius;
}

void CircleCollider::position(float x, float y) {
  m_position.x = x;
  m_position.y = y;
}

float CircleCollider::radius() const { return m_radius; }

glm::vec2 CircleCollider::position() const {
  if (!m_parent.has_value()) {
    return m_position;
  }
  auto parent = m_parent.value();
  return parent->position() + m_position;
}
