#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

Transform::Transform() : parent(std::nullopt) {}

Transform::Transform(std::shared_ptr<Transform> parent) : parent(parent) {}

void Transform::set_position(float x, float y) {
  this->position.x = x;
  this->position.y = y;
}

glm::vec2 Transform::get_position() const { return this->position; }

std::optional<std::shared_ptr<Transform>> Transform::get_parent() const {
  return this->parent;
}
