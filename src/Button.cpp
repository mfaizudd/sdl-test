#include "Button.h"
#include "Texture.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>

Button::Button(Texture *texture) : texture(texture) {
  this->position = {0, 0};
  this->state = ButtonState::Out;
  auto column = 2;
  auto row = 2;
  auto width = 128;
  auto height = 64;
  for (auto i = 0; i < row; i++) {
    for (auto j = 0; j < column; j++) {
      auto index = (i * row) + j;
      this->clips[index].x = j * width;
      this->clips[index].y = i * height;
      this->clips[index].w = width;
      this->clips[index].h = height;
    }
  }
}

void Button::set_position(float x, float y) {
  this->position.x = x;
  this->position.y = y;
}

void Button::handle_event(SDL_Event *e) {
  if (e->type != SDL_EVENT_MOUSE_MOTION &&
      e->type != SDL_EVENT_MOUSE_BUTTON_UP &&
      e->type != SDL_EVENT_MOUSE_BUTTON_DOWN) {
    return;
  }
  float x, y;
  SDL_GetMouseState(&x, &y);

  bool inside = !(x < this->position.x ||
                  x > this->position.x + this->clips[this->state].w ||
                  y < this->position.y ||
                  y > this->position.y + this->clips[this->state].h);
  if (!inside) {
    this->state = ButtonState::Out;
    return;
  }

  switch (e->type) {
  case SDL_EVENT_MOUSE_MOTION:
    this->state = ButtonState::Over;
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    this->state = ButtonState::Down;
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    this->state = ButtonState::Up;
    break;
  }
}

void Button::render() {
  this->texture->render(this->position.x, this->position.y,
                        &this->clips[this->state]);
}
