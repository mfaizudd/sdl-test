#include "Button.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <cstdint>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TOTAL_FRAMES = 4;
const int JOYSTICK_DEADZONE = 8000;
bool init();
bool load_media();
void close();
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
TTF_Font *g_font = nullptr;
#endif // SDL_TTF_MAJOR_VERSION
Texture *texture = nullptr;
Texture *background = nullptr;
Texture *sheet = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
Texture *font_texture = nullptr;
#endif // SDL_TTF_MAJOR_VERSION
Texture *button_texture = nullptr;
SDL_FRect sprite_clips[TOTAL_FRAMES];
Button *buttons[3];
SDL_Joystick *game_pad = nullptr;

int main(int argc, char *args[]) {
  if (!init()) {
    SDL_Log("Failed to initialize\n");
    close();
    return 0;
  }

  if (!load_media()) {
    SDL_Log("Failed to load media\n");
    close();
    return 0;
  }

  SDL_UpdateWindowSurface(g_window);

  uint8_t r = 0x80;
  uint8_t g = 0x80;
  uint8_t b = 0x80;
  uint8_t a = 0x00;
  double degrees = 0;
  SDL_FlipMode flip = SDL_FLIP_NONE;

  SDL_Event e;
  bool quit = false;
  int frame_index = 0;
  int joyX = 0;
  int joyY = 0;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      } else if (e.type == SDL_EVENT_JOYSTICK_AXIS_MOTION) {
        if (e.jaxis.axis == 0 && e.jaxis.value < -JOYSTICK_DEADZONE) {
          joyX = -1;
        } else if (e.jaxis.axis == 0 && e.jaxis.value > JOYSTICK_DEADZONE) {
          joyX = 1;
        } else if (e.jaxis.axis == 0) {
          joyX = 0;
        }
        if (e.jaxis.axis == 1 && e.jaxis.value < -JOYSTICK_DEADZONE) {
          joyY = -1;
        } else if (e.jaxis.axis == 1 && e.jaxis.value > JOYSTICK_DEADZONE) {
          joyY = 1;
        } else if (e.jaxis.axis == 1) {
          joyY = 0;
        }
      }
      for (int i = 0; i < 3; i++) {
        buttons[i]->handle_event(&e);
      }
      const bool *current_key_states = SDL_GetKeyboardState(nullptr);
      if (current_key_states[SDL_SCANCODE_Q]) {
        r += 0x20;
      }
      if (current_key_states[SDL_SCANCODE_W]) {
        g += 0x20;
      }
      if (current_key_states[SDL_SCANCODE_E]) {
        b += 0x20;
      }
      if (current_key_states[SDL_SCANCODE_A]) {
        r -= 0x20;
      }
      if (current_key_states[SDL_SCANCODE_S]) {
        g -= 0x20;
      }
      if (current_key_states[SDL_SCANCODE_D]) {
        b -= 0x20;
      }
      if (current_key_states[SDL_SCANCODE_R]) {
        flip = SDL_FLIP_NONE;
      }
      if (current_key_states[SDL_SCANCODE_F]) {
        flip = (flip & SDL_FLIP_HORIZONTAL) == 0
                   ? (SDL_FlipMode)(flip | SDL_FLIP_HORIZONTAL)
                   : (SDL_FlipMode)(flip & ~SDL_FLIP_HORIZONTAL);
      }
      if (current_key_states[SDL_SCANCODE_V]) {
        flip = (flip & SDL_FLIP_VERTICAL) == 0
                   ? (SDL_FlipMode)(flip | SDL_FLIP_VERTICAL)
                   : (SDL_FlipMode)(flip & ~SDL_FLIP_VERTICAL);
      }
    }
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    degrees = std::atan2((double)joyY, (double)joyX) * 180 / M_PI;
    if (joyX == 0 && joyY == 0) {
      degrees = 0;
    }

    background->set_color(r, g, b);
    background->render(0, 0);
    SDL_FRect *frame_clip = &sprite_clips[frame_index / 4];
    SDL_FPoint center{frame_clip->w / 2, frame_clip->h / 2};
    texture->render(240, 190, frame_clip, degrees, &center, flip);
    SDL_FRect clip1{0, 0, 100, 100};
    sheet->set_blend_mode(SDL_BLENDMODE_ADD);
    sheet->set_alpha(a);
    sheet->render(0, 0, &clip1);
    SDL_FRect clip2{100, 0, 100, 100};
    sheet->render(SCREEN_WIDTH - 100, 0, &clip2);
    SDL_FRect clip3{0, 100, 100, 100};
    sheet->render(0, SCREEN_HEIGHT - 100, &clip3);
    SDL_FRect clip4{100, 100, 100, 100};
    sheet->render(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, &clip4);
    for (auto button : buttons) {
      button->render();
    }
#ifdef SDL_TTF_MAJOR_VERSION
    font_texture->render((SCREEN_WIDTH - font_texture->get_width()) / 2,
                         (SCREEN_HEIGHT - font_texture->get_height()) / 2);
#endif // SDL_TTF_MAJOR_VERSION
    SDL_RenderPresent(g_renderer);
    frame_index = (frame_index + 1) % (TOTAL_FRAMES * 4);
  }

  close();

  return 0;
}

bool init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
    SDL_Log("Could not initialize SDL. Error: %s\n", SDL_GetError());
    return false;
  }

  // Create window and renderer
  if (!SDL_CreateWindowAndRenderer("SDL Test", SCREEN_WIDTH, SCREEN_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &g_window,
                                   &g_renderer)) {
    SDL_Log("Could not create window and renderer. Error: %s\n",
            SDL_GetError());
    return false;
  }

  SDL_SetRenderVSync(g_renderer, 1);
  SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
#ifdef SDL_TTF_MAJOR_VERSION
  if (!TTF_Init()) {
    SDL_Log("Could not initialize ttf. Error: %s\n", SDL_GetError());
    return false;
  }
#endif // SDL_TTF_MAJOR_VERSION
       //
  texture = new Texture(g_renderer);
  background = new Texture(g_renderer);
  sheet = new Texture(g_renderer);
#ifdef SDL_TTF_MAJOR_VERSION
  font_texture = new Texture(g_renderer);
#endif // SDL_TTF_MAJOR_VERSION
  button_texture = new Texture(g_renderer);
  for (int i = 0; i < 3; i++) {
    buttons[i] = new Button(button_texture);
  }
  buttons[0]->set_position(100, 50);
  buttons[1]->set_position(300, 200);
  buttons[2]->set_position(300, 120);
  SDL_JoystickID *joysticks = SDL_GetJoysticks(nullptr);
  if (!joysticks) {
    return true;
  }

  if (!joysticks[0]) {
    SDL_Log("Warning: no joysticks connected");
    SDL_free(joysticks);
    return true;
  }

  game_pad = SDL_OpenJoystick(joysticks[0]);
  if (game_pad == nullptr) {
    SDL_Log("Warning: unable to open game controller. SDL Error: %s\n",
            SDL_GetError());
    SDL_free(joysticks);
    return true;
  }

  SDL_free(joysticks);
  return true;
}

bool load_media() {
  if (!background->load_from_file("assets/background.png")) {
    return false;
  }
  if (!texture->load_from_file("assets/foo.png")) {
    return false;
  }
  if (!button_texture->load_from_file("assets/buttons.png")) {
    return false;
  }
  for (int i = 0; i < TOTAL_FRAMES; i++) {
    sprite_clips[i].x = i * 64;
    sprite_clips[i].w = 64;
    sprite_clips[i].h = 205;
  }
#ifdef SDL_TTF_MAJOR_VERSION
  g_font = TTF_OpenFont("assets/lazy.ttf", 28);
  if (!font_texture->load_from_rendered_text(
          "The quick brown fox jumps over the lazy dog.", SDL_Color{0, 0, 0},
          g_font)) {
    return false;
  }
#endif // SDL_TTF_MAJOR_VERSION
  return sheet->load_from_file("assets/sprites.png");
}

void close() {
  delete texture;
  texture = nullptr;
  delete background;
  background = nullptr;
  delete sheet;
  sheet = nullptr;
  for (int i = 0; i < 3; i++) {
    delete buttons[i];
    buttons[i] = nullptr;
  }
  delete button_texture;
  button_texture = nullptr;
  SDL_CloseJoystick(game_pad);
#ifdef SDL_TTF_MAJOR_VERSION
  delete font_texture;
  font_texture - = nullptr;
  TTF_CloseFont(g_font);
  g_font = nullptr;
#endif // SDL_TTF_MAJOR_VERSION
  SDL_DestroyRenderer(g_renderer);
  g_renderer = nullptr;
  SDL_DestroyWindow(g_window);
  g_window = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
  TTF_Quit();
#endif // SDL_TTF_MAJOR_VERSION
  SDL_Quit();
}
