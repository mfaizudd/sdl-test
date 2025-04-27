#pragma once
#include "BoxCollider.h"
#include "GameObject.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

// Tile sprites
enum TileType : int {
  Red = 0,
  Green,
  Blue,
  Center,
  Top,
  TopRight,
  Right,
  BottomRight,
  Bottom,
  BottomLeft,
  Left,
  TopLeft
};

class Tile : public GameObject {
public:
  Tile(float x, float y, TileType type);
  bool init() override;
  void handle_event(const SDL_Event *e) override;
  void update(float dt) override;
  void render(std::shared_ptr<Camera> camera) override;
  TileType type();
  std::shared_ptr<BoxCollider> collider();
private:
  std::shared_ptr<BoxCollider> m_collider;
  TileType m_type;
};
