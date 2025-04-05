#include "Transform.h"
#include <glm/ext/vector_float2.hpp>

Transform::Transform() {}

void Transform::set_position(float x, float y) {
  this->position.x = x;
  this->position.y = y;
}

const glm::vec2 Transform::get_position() const { return this->position; }
