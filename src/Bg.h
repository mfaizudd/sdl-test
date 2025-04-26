#pragma once

#include "GameObject.h"
#include "Texture.h"
#include <SDL3/SDL_render.h>
#include <memory>

class Bg : public GameObject {
public:
  Bg(SDL_Renderer *renderer);
  bool init() override;
  void handle_event(const SDL_Event *e) override;
  void update(float dt) override;
  void render(glm::vec2 cam_pos) override;

private:
  std::shared_ptr<Texture> m_texture = nullptr;
};
