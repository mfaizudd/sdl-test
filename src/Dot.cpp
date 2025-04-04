#include "Dot.h"
#include "Globals.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

Dot::Dot(Texture *texture) : texture(texture) {
  this->position.x = SCREEN_WIDTH / 2.0f;
  this->position.y = SCREEN_HEIGHT / 2.0f;
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
  this->position.x += this->velocity.x * dt;
  this->position.y += this->velocity.y * dt;
  if (this->position.x < 0 || this->position.x + DOT_WIDTH > SCREEN_WIDTH) {
    this->position.x -= this->velocity.x;
  }
  if (this->position.y < 0 || this->position.y + DOT_HEIGHT > SCREEN_HEIGHT) {
    this->position.y -= this->velocity.y;
  }
  this->texture->render(this->position.x, this->position.y);
}
