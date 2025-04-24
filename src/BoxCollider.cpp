#include "BoxCollider.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>

BoxCollider::BoxCollider(float width, float height)
    : width(width), height(height) {}

BoxCollider::BoxCollider(float x, float y, float width, float height)
    : BoxCollider(width, height) {
  this->set_position(x, y);
}

BoxCollider::BoxCollider(std::shared_ptr<Transform> parent, float width,
                         float height)
    : BoxCollider(width, height) {
  this->parent = parent;
}

void BoxCollider::set_dimension(float width, float height) {
  this->width = width;
  this->height = height;
}

void BoxCollider::set_position(float x, float y) {
  this->position.x = x;
  this->position.y = y;
}

float BoxCollider::get_width() const { return this->width; }
float BoxCollider::get_height() const { return this->height; }

glm::vec2 BoxCollider::get_position() const {
  if (!this->parent.has_value()) {
    return this->position;
  }
  auto parent = this->parent.value();
  return parent->position() + this->position;
}
