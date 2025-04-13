#pragma once
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>
class Transform {
public:
  Transform();
  Transform(std::shared_ptr<Transform> parent);
  void set_position(float x, float y);
  glm::vec2 get_position() const;
  std::optional<std::shared_ptr<Transform>> get_parent() const;

private:
  std::optional<std::shared_ptr<Transform>> parent;
  glm::vec2 position{0, 0};
};
