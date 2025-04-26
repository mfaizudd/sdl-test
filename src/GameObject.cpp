#include "GameObject.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <memory>

GameObject::GameObject() {
  m_transform = std::make_shared<Transform>();
}

void GameObject::position(float x, float y) { m_transform->position(x, y); }
glm::vec2 GameObject::position() { return m_transform->position(); }
float GameObject::width() { return m_width; }
float GameObject::height() { return m_height; }

