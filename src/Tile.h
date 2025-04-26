#pragma once
#include "BoxCollider.h"
#include "Globals.h"
#include "Transform.h"
#include <SDL3/SDL_rect.h>
#include <memory>

class Tile {
public:
  Tile(float x, float y, TileType type);
  void render(const SDL_FRect &cam_rect);
  TileType type();
  std::shared_ptr<BoxCollider> collider();
private:
  std::shared_ptr<Transform> m_transform;
  std::shared_ptr<BoxCollider> m_collider;
  TileType m_type;
};
