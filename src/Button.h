#pragma once
#include "Texture.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

enum ButtonState {
  Out,
  Over,
  Down,
  Up,
};

class Button {
public:
  Button(Texture *texture);
  void set_position(float x, float y);
  void handle_event(SDL_Event *e);
  void render();

private:
  SDL_FPoint position;
  ButtonState state;
  SDL_FRect clips[4];
  Texture *texture;
};
