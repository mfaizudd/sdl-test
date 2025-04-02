#include "Button.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_haptic.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TOTAL_FRAMES = 4;
const int JOYSTICK_DEADZONE = 8000;
bool init();
void load_inputs();
SDL_Gamepad *load_gamepad(SDL_JoystickID *joysticks);
SDL_Joystick *load_joystick(SDL_JoystickID *joysticks, SDL_Haptic **haptic);
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
SDL_Gamepad *g_game_pad = nullptr;
SDL_Joystick *g_joystick = nullptr;
SDL_Haptic *g_haptic = nullptr;

// Mixer stuffs
Mix_Music *g_music = nullptr;

Mix_Chunk *g_scratch = nullptr;
Mix_Chunk *g_high = nullptr;
Mix_Chunk *g_medium = nullptr;
Mix_Chunk *g_low = nullptr;

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
  uint64_t start_time = 0;
  std::stringstream time_text;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      } else if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.key) {
        case SDLK_1:
          Mix_PlayChannel(-1, g_high, 0);
          break;
        case SDLK_2:
          Mix_PlayChannel(-1, g_medium, 0);
          break;
        case SDLK_3:
          Mix_PlayChannel(-1, g_low, 0);
          break;
        case SDLK_4:
          Mix_PlayChannel(-1, g_scratch, 0);
          break;
        case SDLK_9:
          if (!Mix_PlayingMusic()) {
            Mix_PlayMusic(g_music, 0);
          } else if (!Mix_PausedMusic()) {
            Mix_PauseMusic();
          } else {
            Mix_ResumeMusic();
          }
          break;
        case SDLK_0:
          Mix_HaltMusic();
          break;
        case SDLK_RETURN:
          start_time = SDL_GetTicks();
          break;
        }
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
      } else if (e.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN) {
        if (g_game_pad != nullptr) {
          if (!SDL_RumbleGamepad(g_game_pad, 0xFFFF * .75, 0xFFFF * .75, 500)) {
            SDL_Log("Warning: unable to start gamepad rumble. SDL Error: %s\n",
                    SDL_GetError());
          }
        } else if (g_haptic != nullptr) {
          if (!SDL_PlayHapticRumble(g_haptic, .75, 500)) {
            SDL_Log("Warning: unable to start haptic rumble. SDL Error: %s\n",
                    SDL_GetError());
          }
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
    time_text.str("");
    time_text << "Milliseconds since start " << SDL_GetTicks() - start_time;
    if (!font_texture->load_from_rendered_text(
            time_text.str(), SDL_Color{0, 0, 0, 0xFF}, g_font)) {
      SDL_Log("Unable to render time texture");
    }
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
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
                SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
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
  SDL_AudioSpec audio_spec{};
  audio_spec.format = MIX_DEFAULT_FORMAT;
  audio_spec.channels = 2;
  audio_spec.freq = 44100;
  if (!Mix_OpenAudio(0, &audio_spec)) {
    SDL_Log("Could not initialize SDL Mixer. SDL Error: %s\n", SDL_GetError());
    return false;
  }
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
  load_inputs();
  return true;
}

void load_inputs() {
  SDL_JoystickID *joysticks = SDL_GetJoysticks(nullptr);
  if (!joysticks) {
    SDL_Log("Warning: no joysticks connected");
    return;
  }

  g_game_pad = load_gamepad(joysticks);
  if (g_game_pad == nullptr) {
    g_joystick = load_joystick(joysticks, &g_haptic);
  }

  SDL_free(joysticks);
}

SDL_Gamepad *load_gamepad(SDL_JoystickID *joysticks) {
  SDL_Gamepad *gamepad = nullptr;
  if (!SDL_IsGamepad(joysticks[0])) {
    SDL_Log("Warning: joystick is not a gamepad interface. SDL Error: %s\n",
            SDL_GetError());
    return gamepad;
  }

  gamepad = SDL_OpenGamepad(joysticks[0]);
  if (gamepad == nullptr) {
    SDL_Log("Warning: unable to open game controller. SDL Error: %s\n",
            SDL_GetError());
    return gamepad;
  }

  if (!SDL_GetBooleanProperty(SDL_GetGamepadProperties(gamepad),
                              SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false)) {
    SDL_Log("Warning: gamepad doesn't have rumble. SDL Error: %s\n",
            SDL_GetError());
  }
  return gamepad;
}

SDL_Joystick *load_joystick(SDL_JoystickID *joysticks, SDL_Haptic **haptic) {
  SDL_Joystick *joystick = SDL_OpenJoystick(joysticks[0]);
  if (joystick == nullptr) {
    SDL_Log("Warning: unable to open joystick. SDL Error: %s\n",
            SDL_GetError());
    return nullptr;
  }

  if (!SDL_IsJoystickHaptic(joystick)) {
    SDL_Log("Warning: joystick doens't support haptic");
    return joystick;
  }

  *haptic = SDL_OpenHapticFromJoystick(joystick);
  if (*haptic == nullptr) {
    SDL_Log("Warning: could not open haptic from joystic. SDL Error: %s\n",
            SDL_GetError());
    return joystick;
  }

  if (!SDL_InitHapticRumble(*haptic)) {
    SDL_Log("Warning: could not initialize haptic rumble. SDL Error: %s\n",
            SDL_GetError());
    return joystick;
  }
  return joystick;
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
  g_music = Mix_LoadMUS("assets/sounds/beat.wav");
  if (g_music == nullptr) {
    SDL_Log("Failed to load beat music. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  g_scratch = Mix_LoadWAV("assets/sounds/scratch.wav");
  if (g_scratch == nullptr) {
    SDL_Log("Failed to load scratch sound effect. SDL Error: %s\n",
            SDL_GetError());
    return false;
  }

  g_high = Mix_LoadWAV("assets/sounds/high.wav");
  if (g_high == nullptr) {
    SDL_Log("Failed to load high sound effect. SDL Error: %s\n",
            SDL_GetError());
    return false;
  }

  g_medium = Mix_LoadWAV("assets/sounds/medium.wav");
  if (g_medium == nullptr) {
    SDL_Log("Failed to load medium sound effect. SDL Error: %s\n",
            SDL_GetError());
    return false;
  }

  g_low = Mix_LoadWAV("assets/sounds/low.wav");
  if (g_low == nullptr) {
    SDL_Log("Failed to load low sound effect. SDL Error: %s\n", SDL_GetError());
    return false;
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

  Mix_FreeChunk(g_low);
  Mix_FreeChunk(g_medium);
  Mix_FreeChunk(g_high);
  Mix_FreeChunk(g_scratch);
  Mix_FreeMusic(g_music);
  g_low = nullptr;
  g_medium = nullptr;
  g_high = nullptr;
  g_scratch = nullptr;
  g_music = nullptr;

  SDL_CloseGamepad(g_game_pad);
  SDL_CloseJoystick(g_joystick);
  SDL_CloseHaptic(g_haptic);
  g_game_pad = nullptr;
  g_joystick = nullptr;
  g_haptic = nullptr;
#ifdef SDL_TTF_MAJOR_VERSION
  delete font_texture;
  font_texture = nullptr;
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
  Mix_Quit();
  SDL_Quit();
}
