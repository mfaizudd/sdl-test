#pragma once
#include "Dimension.h"
#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <optional>

class BoxCollider {
public:
  BoxCollider(float width, float height);
  BoxCollider(float x, float y, float width, float height);
  BoxCollider(std::shared_ptr<Transform> parent, float width, float height);
  void set_dimension(float width, float height);
  void set_position(float x, float y);
  Dimension get_dimension() const;
  glm::vec2 get_position() const;
  glm::vec2 get_local_position() const;

private:
  glm::vec2 position{0, 0};
  Dimension dimension{0, 0};
  std::optional<std::shared_ptr<Transform>> parent = std::nullopt;
};
