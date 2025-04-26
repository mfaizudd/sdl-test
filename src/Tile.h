#pragma once
#include "BoxCollider.h"
#include "GameObject.h"
#include "Globals.h"
#include "Texture.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

class Tile : public GameObject {
public:
  Tile(float x, float y, TileType type, SDL_Renderer *renderer);
  bool init() override;
  void handle_event(const SDL_Event *e) override;
  void update(float dt) override;
  void render(std::shared_ptr<Camera> camera) override;
  TileType type();
  std::shared_ptr<BoxCollider> collider();
private:
  SDL_Renderer *m_renderer = nullptr;
  std::shared_ptr<Texture> m_texture = nullptr;
  std::shared_ptr<BoxCollider> m_collider;
  TileType m_type;
};
