#pragma once
#include "Texture.h"
#include "Transform.h"
#include <glm/ext/vector_float2.hpp>
const int TOTAL_PARTICLES = 20;

class Particle {
public:
  Particle(int x, int y);
  void render(glm::vec2 cam_pos);
  bool is_dead();

private:
  Transform m_transform;
  float m_width;
  float m_height;
  int m_frame;
  Texture *m_texture;
  Texture *m_texture_red;
  Texture *m_texture_green;
  Texture *m_texture_blue;
  Texture *m_texture_shimmer;
};
