#include "BoxCollider.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>

BoxCollider::BoxCollider(float width, float height)
    : m_width(width), m_height(height) {}

BoxCollider::BoxCollider(float x, float y, float width, float height)
    : BoxCollider(width, height) {
  set_position(x, y);
}

BoxCollider::BoxCollider(std::shared_ptr<Transform> parent, float width,
                         float height)
    : BoxCollider(width, height) {
  m_parent = parent;
}

void BoxCollider::set_dimension(float width, float height) {
  m_width = width;
  m_height = height;
}

void BoxCollider::set_position(float x, float y) {
  m_position.x = x;
  m_position.y = y;
}

float BoxCollider::width() const { return m_width; }
float BoxCollider::height() const { return m_height; }

glm::vec2 BoxCollider::position() const {
  if (!m_parent.has_value()) {
    return m_position;
  }
  auto parent = m_parent.value();
  return parent->position() + m_position;
}
