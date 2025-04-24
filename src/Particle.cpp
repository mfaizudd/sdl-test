#include "Globals.h"
#include "Particle.h"
#include "Texture.h"
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>

Particle::Particle(int x, int y) {
  m_transform = Transform();
  m_transform.position(x - 5 + (rand() % 25), y - 5 + (rand() % 25));
  m_frame = rand() % 5;
  switch (rand() % 3) {
  case 0:
    m_texture = g_texture_red();
    break;
  case 1:
    m_texture = g_texture_green();
    break;
  case 2:
    m_texture = g_texture_blue();
    break;
  }
  m_width = m_texture->width();
  m_height = m_texture->height();
}

void Particle::render(glm::vec2 cam_pos) {
  auto pos = m_transform.position();
  auto screen_pos = pos - cam_pos;
  if (screen_pos.x + m_width < 0 || screen_pos.x > SCREEN_WIDTH ||
      screen_pos.y + m_height < 0 || screen_pos.y > SCREEN_HEIGHT) {
    return;
  }

  m_texture->render(screen_pos.x, screen_pos.y);
  if (m_frame % 2 == 0) {
    g_texture_shimmer()->render(screen_pos.x, screen_pos.y);
  }
  m_frame++;
}

bool Particle::is_dead() { return m_frame > 10; }
