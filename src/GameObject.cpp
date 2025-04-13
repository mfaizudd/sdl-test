#include "GameObject.h"
#include "CircleCollider.h"
#include "Globals.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

GameObject::GameObject(Texture *texture) : texture(texture) {
  this->transform = std::make_shared<Transform>();
  this->collider = std::make_shared<CircleCollider>(this->transform, 10);
  this->collider->set_position(DOT_WIDTH / 2.0f, DOT_HEIGHT / 2.0f);
}

// TODO: extract this
void GameObject::handle_event(const SDL_Event *e) {
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

void GameObject::set_position(float x, float y) {
  this->transform->set_position(x, y);
}

glm::vec2 GameObject::get_position() { return this->transform->get_position(); }

void GameObject::update(float dt) {
  auto position = this->transform->get_position();
  auto moved = position + this->velocity * dt;
  this->transform->set_position(moved.x, moved.y);
  if (moved.x < 0 || moved.x + DOT_WIDTH > LEVEL_WIDTH ||
      check_collision(this->collider, g_wall)) {
    moved.x -= this->velocity.x * dt;
  }
  if (moved.y < 0 || moved.y + DOT_HEIGHT > LEVEL_HEIGHT ||
      check_collision(this->collider, g_wall)) {
    moved.y -= this->velocity.y * dt;
  }
  this->transform->set_position(moved.x, moved.y);
}

void GameObject::render(glm::vec2 camPos) {
  auto screenPos = this->get_position() - camPos;
  // if (screenPos.x + DOT_WIDTH < 0 || screenPos.x > SCREEN_WIDTH ||
  //     screenPos.y + DOT_HEIGHT < 0 || screenPos.y > SCREEN_HEIGHT) {
  //   return;
  // }

  this->texture->render(screenPos.x, screenPos.y);
}
