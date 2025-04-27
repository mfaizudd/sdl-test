#include "Camera.h"
#include "BoxCollider.h"
#include "Globals.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>

Camera::Camera() {
  m_transform = std::make_shared<Transform>();
  m_collider =
      std::make_shared<BoxCollider>(m_transform, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Camera::position(glm::vec2 position) {
  m_transform->position(position.x, position.y);
}
glm::vec2 Camera::position() const { return m_transform->position(); }
std::shared_ptr<BoxCollider> Camera::collider() const { return m_collider; }
float Camera::width() const { return m_collider->width(); }
float Camera::height() const { return m_collider->height(); }
