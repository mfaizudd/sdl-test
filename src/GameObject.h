#pragma once
#include "Camera.h"
#include "Transform.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

class GameObject {
public:
  GameObject();
  void position(float x, float y);
  glm::vec2 position();
  float width();
  float height();

  virtual bool init() = 0;
  virtual void handle_event(const SDL_Event *e) = 0;
  virtual void update(float dt) = 0;
  virtual void render(std::shared_ptr<Camera> camera) = 0;

protected:
  float m_width;
  float m_height;
  std::shared_ptr<Transform> m_transform = nullptr;
  glm::vec2 m_velocity{0, 0};
};
