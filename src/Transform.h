#pragma once
#include <glm/ext/vector_float2.hpp>
class Transform {
public:
  Transform();
  void set_position(float x, float y);
  const glm::vec2 get_position() const;

private:
  glm::vec2 position{0, 0};
};
