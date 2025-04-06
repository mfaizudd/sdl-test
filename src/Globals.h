#pragma once

#include "BoxCollider.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <memory>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float SCREEN_FPS = 60;
const float TICKS_PER_FRAME = 1000 / SCREEN_FPS;
const int TOTAL_FRAMES = 4;
const int JOYSTICK_DEADZONE = 8000;

static std::shared_ptr<BoxCollider> g_wall =
    std::make_shared<BoxCollider>(300, 120, 128, 64);
inline bool check_collision(std::shared_ptr<BoxCollider> a,
                            std::shared_ptr<BoxCollider> b) {
  int left_a, left_b;
  int right_a, right_b;
  int top_a, top_b;
  int bottom_a, bottom_b;

  auto a_pos = a->get_position();
  auto a_width = a->get_width();
  auto a_height = a->get_height();
  left_a = a_pos.x;
  right_a = a_pos.x + a_width;
  top_a = a_pos.y;
  bottom_a = a_pos.y + a_height;

  auto b_pos = b->get_position();
  auto b_width = b->get_width();
  auto b_height = b->get_height();
  left_b = b_pos.x;
  right_b = b_pos.x + b_width;
  top_b = b_pos.y;
  bottom_b = b_pos.y + b_height;

  // given that y = -y
  if (bottom_a <= top_b) {
    return false;
  }

  if (top_a >= bottom_b) {
    return false;
  }

  if (right_a <= left_b) {
    return false;
  }

  if (left_a >= right_b) {
    return false;
  }

  return true;
}
