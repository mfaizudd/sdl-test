#pragma once
#include "Texture.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
class Dot {
public:
  static const int DOT_WIDTH = 20;
  static const int DOT_HEIGHT = 20;
  static const int DOT_VEL = 200;
  Dot(Texture *texture);
  void handle_event(const SDL_Event *e);
  void update(float dt);

private:
  Texture *texture = nullptr;
  SDL_FPoint position{0, 0};
  SDL_FPoint velocity{0, 0};
};
