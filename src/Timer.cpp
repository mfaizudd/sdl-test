#include "Timer.h"
#include <SDL3/SDL_timer.h>
#include <cstdint>

Timer::Timer() {
  this->start_ticks = 0;
  this->paused_ticks = 0;
  this->paused = false;
  this->started = false;
}

void Timer::start() {
  this->started = true;
  this->start_ticks = SDL_GetTicks();
}

void Timer::stop() {
  this->started = false;
  this->paused = false;
  this->start_ticks = 0;
  this->paused_ticks = 0;
}

void Timer::pause() {
  if (!this->started || this->paused) {
    return;
  }
  this->paused = true;
  this->paused_ticks = SDL_GetTicks() - this->start_ticks;
}

void Timer::resume() {
  if (!this->paused) {
    return;
  }
  this->start_ticks = SDL_GetTicks() - this->paused_ticks;
  this->paused = false;
  this->paused_ticks = 0;
}

uint64_t Timer::get_ticks() const {
  if (!this->started) {
    return 0;
  }
  if (this->paused) {
    return this->paused_ticks;
  }
  return SDL_GetTicks() - this->start_ticks;
}

bool Timer::is_started() {
  return this->started;
}

bool Timer::is_paused() {
  return this->started && this->paused;
}
