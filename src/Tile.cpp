#include "Tile.h"
#include "BoxCollider.h"
#include <SDL3/SDL_rect.h>
#include <memory>

Tile::Tile(float x, float y, TileType type) {
  m_transform = std::make_shared<Transform>(x, y);
  m_collider = std::make_shared<BoxCollider>(m_transform);
  m_type = type;
}
