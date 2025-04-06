#include "CircleCollider.h"
#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>

CircleCollider::CircleCollider(float x, float y, float radius) {
  this->set_position(x, y);
  this->radius = radius;
}

CircleCollider::CircleCollider(std::shared_ptr<Transform> parent,
                               float radius) {
  this->parent = parent;
  this->radius = radius;
}

void CircleCollider::set_position(float x, float y) {
  this->position.x = x;
  this->position.y = y;
}

float CircleCollider::get_radius() const { return this->radius; }

glm::vec2 CircleCollider::get_position() const {
  if (!this->parent.has_value()) {
    return this->position;
  }
  auto parent = this->parent.value();
  return parent->get_position() + this->position;
}
