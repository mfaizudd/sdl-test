#include "Tile.h"
#include "BoxCollider.h"
#include "Globals.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

Tile::Tile(float x, float y, TileType type, SDL_Renderer *renderer) {
  m_renderer = renderer;
  m_transform = std::make_shared<Transform>(x, y);
  m_collider = std::make_shared<BoxCollider>(m_transform);
  m_type = type;
  m_texture = std::make_shared<Texture>();
}

bool Tile::init() {
  // TODO: Load tile texture by tile type
  return true;
}

void Tile::handle_event(const SDL_Event *e) {}
void Tile::render(std::shared_ptr<Camera> camera) {
  if (check_collision(m_collider, camera->collider())) {
    auto cam_pos = camera->position();
    auto pos = position() - cam_pos;
    m_texture->render(pos.x, pos.y);
  }
}
void Tile::update(float dt) {}
TileType Tile::type() { return m_type; }
std::shared_ptr<BoxCollider> Tile::collider() { return m_collider; }
