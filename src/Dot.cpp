#include "Dot.h"
#include "BoxCollider.h"
#include "Globals.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <memory>

Dot::Dot(Texture *texture) : texture(texture) {
  this->transform = std::make_shared<Transform>();
  this->collider = std::make_shared<BoxCollider>(this->transform, 20, 20);
}

void Dot::handle_event(const SDL_Event *e) {
  if (e->type == SDL_EVENT_KEY_DOWN && !e->key.repeat) {
    switch (e->key.key) {
    case SDLK_UP:
      this->velocity.y -= DOT_VEL;
      break;
    case SDLK_DOWN:
      this->velocity.y += DOT_VEL;
      break;
    case SDLK_LEFT:
      this->velocity.x -= DOT_VEL;
      break;
    case SDLK_RIGHT:
      this->velocity.x += DOT_VEL;
      break;
    }
  } else if (e->type == SDL_EVENT_KEY_UP && !e->key.repeat) {
    switch (e->key.key) {
    case SDLK_UP:
      this->velocity.y += DOT_VEL;
      break;
    case SDLK_DOWN:
      this->velocity.y -= DOT_VEL;
      break;
    case SDLK_LEFT:
      this->velocity.x += DOT_VEL;
      break;
    case SDLK_RIGHT:
      this->velocity.x -= DOT_VEL;
      break;
    }
  }
}

void Dot::update(float dt) {
  auto position = this->transform->get_position();
  auto moved = position + this->velocity * dt;
  this->transform->set_position(moved.x, moved.y);
  if (moved.x < 0 || moved.x + DOT_WIDTH > SCREEN_WIDTH ||
      check_collision(this->collider, g_wall)) {
    moved.x -= this->velocity.x * dt;
  }
  if (moved.y < 0 || moved.y + DOT_HEIGHT > SCREEN_HEIGHT ||
      check_collision(this->collider, g_wall)) {
    moved.y -= this->velocity.y * dt;
  }
  this->transform->set_position(moved.x, moved.y);
  position = this->transform->get_position();
  this->texture->render(position.x, position.y);
}
