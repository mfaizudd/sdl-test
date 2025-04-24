#pragma once
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>
class Transform {
public:
  Transform();
  Transform(std::shared_ptr<Transform> parent);
  void position(float x, float y);
  glm::vec2 position() const;
  std::optional<std::shared_ptr<Transform>> parent() const;

private:
  std::optional<std::shared_ptr<Transform>> m_parent;
  glm::vec2 m_position{0, 0};
};
