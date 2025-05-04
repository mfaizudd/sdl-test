#pragma once

#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Texture.h"
#include "Tile.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <memory>

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const float SCREEN_FPS = 60;
const float TICKS_PER_FRAME = 1000 / SCREEN_FPS;
const int TOTAL_FRAMES = 4;
const int TOTAL_DATA = 10;
const int JOYSTICK_DEADZONE = 8000;
const int MAX_RECORDING_DEVICES = 10;
const int MAX_RECORDING_SECONDS = 5;
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;
const int TOTAL_WINDOWS = 3;

// Tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

enum RecordingState {
  SelectingDevice,
  Stopped,
  Recording,
  Recorded,
  Playback,
  Error
};

std::shared_ptr<BoxCollider> g_wall();

bool check_collision(std::shared_ptr<BoxCollider> a,
                     std::shared_ptr<BoxCollider> b);

bool check_collision(std::shared_ptr<CircleCollider> a,
                     std::shared_ptr<BoxCollider> b);

bool touches_wall(std::shared_ptr<CircleCollider> box);

Texture *g_texture_red();
Texture *g_texture_green();
Texture *g_texture_blue();
Texture *g_texture_shimmer();
Texture *g_texture_tiles();
std::shared_ptr<Tile> g_tiles(int index);
SDL_FRect tile_clip(TileType tile_type);

bool g_init();
bool g_load_media(SDL_Renderer *renderer);
bool g_set_tiles(SDL_Renderer *renderer);
void g_close();
