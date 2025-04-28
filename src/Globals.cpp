#include "Globals.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Texture.h"
#include "Tile.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdio>
#include <fstream>
#include <memory>

TTF_Font *font = nullptr;
Texture *font_texture = nullptr;
Texture *texture_red = nullptr;
Texture *texture_green = nullptr;
Texture *texture_blue = nullptr;
Texture *texture_shimmer = nullptr;
Texture *texture_tiles = nullptr;
std::shared_ptr<Tile> tiles[TOTAL_TILES] = {};
SDL_FRect tile_clips[TOTAL_TILE_SPRITES];
std::shared_ptr<BoxCollider> wall = nullptr;

bool g_init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
                SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
  }

  if (!TTF_Init()) {
    SDL_Log("Could not initialize ttf. Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_AudioSpec audio_spec{};
  audio_spec.format = MIX_DEFAULT_FORMAT;
  audio_spec.channels = 2;
  audio_spec.freq = 44100;
  if (!Mix_OpenAudio(0, &audio_spec)) {
    SDL_Log("Could not initialize SDL Mixer. SDL Error: %s\n", SDL_GetError());
    return false;
  }
  wall = std::make_shared<BoxCollider>(300, 120, 128, 64);
  return true;
}

bool g_load_media(SDL_Renderer *renderer) {
  font_texture = new Texture(renderer);
  texture_red = new Texture(renderer);
  texture_green = new Texture(renderer);
  texture_blue = new Texture(renderer);
  texture_shimmer = new Texture(renderer);
  texture_tiles = new Texture(renderer);
  font = TTF_OpenFont("assets/lazy.ttf", 28);
  if (!font_texture->load_from_rendered_text(
          "The quick brown fox jumps over the lazy dog.", SDL_Color{0, 0, 0},
          font)) {
    return false;
  }
  if (!texture_red->load_from_file("assets/red.bmp")) {
    return false;
  }
  if (!texture_green->load_from_file("assets/green.bmp")) {
    return false;
  }
  if (!texture_blue->load_from_file("assets/blue.bmp")) {
    return false;
  }
  if (!texture_shimmer->load_from_file("assets/shimmer.bmp")) {
    return false;
  }
  if (!texture_tiles->load_from_file("assets/tiles.png")) {
    return false;
  }
  texture_red->set_alpha(192);
  texture_green->set_alpha(192);
  texture_blue->set_alpha(192);
  texture_shimmer->set_alpha(192);
  return true;
}

bool g_set_tiles(SDL_Renderer *renderer) {
  int x = 0, y = 0;
  std::ifstream map("assets/lazy.map");
  if (map.fail()) {
    printf("Unable to load map file: \n");
    return false;
  }

  for (int i = 0; i < TOTAL_TILES; i++) {
    int tile_type = -1;
    map >> tile_type;
    if (map.fail()) {
      printf("Error loading map: unexpected end of file.\n");
      return false;
    }
    if (tile_type < 0 || tile_type >= TOTAL_TILE_SPRITES) {
      printf("Invalid tile type at %i\n", i);
      return false;
    }

    tiles[i] = std::make_shared<Tile>(x, y, (TileType)tile_type);

    x += TILE_WIDTH;
    if (x >= LEVEL_WIDTH) {
      x = 0;
      y += TILE_HEIGHT;
    }
  }

  tile_clips[TileType::Red].x = 0;
  tile_clips[TileType::Red].y = 0;
  tile_clips[TileType::Red].w = TILE_WIDTH;
  tile_clips[TileType::Red].h = TILE_HEIGHT;

  tile_clips[TileType::Green].x = 0;
  tile_clips[TileType::Green].y = 80;
  tile_clips[TileType::Green].w = TILE_WIDTH;
  tile_clips[TileType::Green].h = TILE_HEIGHT;

  tile_clips[TileType::Blue].x = 0;
  tile_clips[TileType::Blue].y = 160;
  tile_clips[TileType::Blue].w = TILE_WIDTH;
  tile_clips[TileType::Blue].h = TILE_HEIGHT;

  tile_clips[TileType::TopLeft].x = 80;
  tile_clips[TileType::TopLeft].y = 0;
  tile_clips[TileType::TopLeft].w = TILE_WIDTH;
  tile_clips[TileType::TopLeft].h = TILE_HEIGHT;

  tile_clips[TileType::Left].x = 80;
  tile_clips[TileType::Left].y = 80;
  tile_clips[TileType::Left].w = TILE_WIDTH;
  tile_clips[TileType::Left].h = TILE_HEIGHT;

  tile_clips[TileType::BottomLeft].x = 80;
  tile_clips[TileType::BottomLeft].y = 160;
  tile_clips[TileType::BottomLeft].w = TILE_WIDTH;
  tile_clips[TileType::BottomLeft].h = TILE_HEIGHT;

  tile_clips[TileType::Top].x = 160;
  tile_clips[TileType::Top].y = 0;
  tile_clips[TileType::Top].w = TILE_WIDTH;
  tile_clips[TileType::Top].h = TILE_HEIGHT;

  tile_clips[TileType::Center].x = 160;
  tile_clips[TileType::Center].y = 80;
  tile_clips[TileType::Center].w = TILE_WIDTH;
  tile_clips[TileType::Center].h = TILE_HEIGHT;

  tile_clips[TileType::Bottom].x = 160;
  tile_clips[TileType::Bottom].y = 160;
  tile_clips[TileType::Bottom].w = TILE_WIDTH;
  tile_clips[TileType::Bottom].h = TILE_HEIGHT;

  tile_clips[TileType::TopRight].x = 240;
  tile_clips[TileType::TopRight].y = 0;
  tile_clips[TileType::TopRight].w = TILE_WIDTH;
  tile_clips[TileType::TopRight].h = TILE_HEIGHT;

  tile_clips[TileType::Right].x = 240;
  tile_clips[TileType::Right].y = 80;
  tile_clips[TileType::Right].w = TILE_WIDTH;
  tile_clips[TileType::Right].h = TILE_HEIGHT;

  tile_clips[TileType::BottomRight].x = 240;
  tile_clips[TileType::BottomRight].y = 160;
  tile_clips[TileType::BottomRight].w = TILE_WIDTH;
  tile_clips[TileType::BottomRight].h = TILE_HEIGHT;

  map.close();
  return true;
}

std::shared_ptr<BoxCollider> g_wall() { return wall; }

bool check_collision(std::shared_ptr<BoxCollider> a,
                     std::shared_ptr<BoxCollider> b) {
  int left_a, left_b;
  int right_a, right_b;
  int top_a, top_b;
  int bottom_a, bottom_b;

  auto a_pos = a->position();
  auto a_width = a->width();
  auto a_height = a->height();
  left_a = a_pos.x;
  right_a = a_pos.x + a_width;
  top_a = a_pos.y;
  bottom_a = a_pos.y + a_height;

  auto b_pos = b->position();
  auto b_width = b->width();
  auto b_height = b->height();
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

bool check_collision(std::shared_ptr<CircleCollider> a,
                     std::shared_ptr<BoxCollider> b) {
  // x axis closest point
  glm::vec2 c{0, 0};
  auto a_pos = a->position();
  auto b_pos = b->position();
  if (a_pos.x < b_pos.x) {
    c.x = b_pos.x;
  } else if (a_pos.x > b_pos.x + b->width()) {
    c.x = b_pos.x + b->width();
  } else {
    c.x = a_pos.x;
  }

  if (a_pos.y < b_pos.y) {
    c.y = b_pos.y;
  } else if (a_pos.y > b_pos.y + b->height()) {
    c.y = b_pos.y + b->height();
  } else {
    c.y = a_pos.y;
  }

  // SDL_Log("Circle(%f, %f) Closest(%f, %f) Distance(%f) Radius(%f)", a_pos.x,
  //         a_pos.y, c.x, c.y, glm::distance(a_pos, c), a->get_radius());
  return glm::distance(a_pos, c) < a->radius();
}

bool touches_wall(std::shared_ptr<CircleCollider> box) {
  for (int i = 0; i < TOTAL_TILES; i++) {
    if (tiles[i]->type() >= TileType::Center &&
        tiles[i]->type() <= TileType::TopLeft) {
      if (check_collision(box, tiles[i]->collider())) {
        return true;
      }
    }
  }

  return false;
}

Texture *g_texture_red() { return texture_red; }

Texture *g_texture_green() { return texture_green; }

Texture *g_texture_blue() { return texture_blue; }

Texture *g_texture_shimmer() { return texture_shimmer; }

Texture *g_texture_tiles() { return texture_tiles; }

std::shared_ptr<Tile> g_tiles(int index) { return tiles[index]; }
SDL_FRect tile_clip(TileType tile_type) { return tile_clips[tile_type]; }

void g_close() {
  delete texture_red;
  texture_red = nullptr;
  delete texture_green;
  texture_green = nullptr;
  delete texture_blue;
  texture_blue = nullptr;
  delete texture_shimmer;
  texture_shimmer = nullptr;
  delete font_texture;
  font_texture = nullptr;
  delete texture_tiles;
  texture_tiles = nullptr;
  TTF_CloseFont(font);
  font = nullptr;

  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}
