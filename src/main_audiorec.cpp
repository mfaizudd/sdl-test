#include "Globals.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_haptic.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
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
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <sstream>

bool init();
void load_inputs();
bool load_media();
void close();
SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;
TTF_Font *g_font = nullptr;
Texture *prompt_texture = nullptr;
Texture *device_textures[MAX_RECORDING_DEVICES];
Texture *font_texture = nullptr;

// Audio recording stuffs
int g_recording_devices_count = 0;
SDL_AudioDeviceID *g_audio_device_ids = nullptr;

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

  SDL_Event e;
  bool quit = false;
  RecordingState current_state = RecordingState::SelectingDevice;
  SDL_AudioStream *recording_stream = nullptr;
  SDL_AudioStream *playback_stream = nullptr;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }

      switch (current_state) {
      case RecordingState::SelectingDevice:
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.key >= SDLK_0 &&
            e.key.key <= SDLK_9) {
          int index = e.key.key - SDLK_0;
          if (index >= g_recording_devices_count) {
            break;
          }

          recording_stream = SDL_OpenAudioDeviceStream(
              g_audio_device_ids[index], NULL, NULL, NULL);
          if (!recording_stream) {
            SDL_Log("Failed to open recording device! SDL Error: %s\n",
                    SDL_GetError());
            prompt_texture->load_from_rendered_text(
                "Failed to open recording device", SDL_Color{0xFF, 0, 0, 0},
                g_font);
            current_state = RecordingState::Error;
            break;
          }

          SDL_AudioSpec spec;
          SDL_GetAudioStreamFormat(recording_stream, &spec, NULL);
          playback_stream = SDL_OpenAudioDeviceStream(
              SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
          if (!playback_stream) {
            SDL_Log("Failed to open playback device! SDL Error: %s\n",
                    SDL_GetError());
            prompt_texture->load_from_rendered_text(
                "Failed to open playback device!", SDL_Color{0xFF, 0, 0, 0xFF},
                g_font);
            current_state = RecordingState::Error;
            break;
          }

          prompt_texture->load_from_rendered_text(
              "Press 1 to record for 5 seconds", SDL_Color{}, g_font);
          current_state = RecordingState::Stopped;
        }
        break;
      case RecordingState::Stopped:
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_1) {
          // start recording
          SDL_ResumeAudioStreamDevice(recording_stream);
          prompt_texture->load_from_rendered_text("Recording...", SDL_Color{},
                                                  g_font);
          current_state = RecordingState::Recording;
        }
        break;
      case RecordingState::Recorded:
        if (e.type == SDL_EVENT_KEY_DOWN) {
          if (e.key.key == SDLK_1) {
            SDL_AudioSpec spec;
            SDL_GetAudioStreamFormat(recording_stream, &spec, NULL);
            const int want_size =
                (SDL_AUDIO_FRAMESIZE(spec) * spec.freq) * MAX_RECORDING_SECONDS;
            uint8_t *buffer = new uint8_t[want_size];
            const int bytes_read =
                SDL_GetAudioStreamData(recording_stream, buffer, want_size);
            SDL_PutAudioStreamData(playback_stream, buffer, bytes_read);
            SDL_ClearAudioStream(recording_stream);
            delete[] buffer;
            SDL_FlushAudioStream(playback_stream);
            SDL_ResumeAudioStreamDevice(playback_stream);
            prompt_texture->load_from_rendered_text("Playing...", SDL_Color{},
                                                    g_font);
            current_state = RecordingState::Playback;
          } else if (e.key.key == SDLK_2) {
            SDL_ClearAudioStream(recording_stream);
            SDL_ResumeAudioStreamDevice(recording_stream);
            prompt_texture->load_from_rendered_text("Recording...", SDL_Color{},
                                                    g_font);
            current_state = RecordingState::Recording;
          }
        }
        break;
      }
    }
    if (current_state == RecordingState::Recording) {
      SDL_AudioSpec spec;
      SDL_GetAudioStreamFormat(recording_stream, &spec, NULL);
      const int waiting_for_size =
          (SDL_AUDIO_FRAMESIZE(spec) * spec.freq) * MAX_RECORDING_SECONDS;
      const int byte_count = SDL_GetAudioStreamAvailable(recording_stream);

      // Finished recording
      if (byte_count >= waiting_for_size) {
        SDL_PauseAudioStreamDevice(recording_stream);
        prompt_texture->load_from_rendered_text(
            "Press 1 to play back. Press 2 to record again", SDL_Color{},
            g_font);
        current_state = RecordingState::Recorded;
      }
    }

    // update playback
    else if (current_state == RecordingState::Playback &&
             SDL_GetAudioStreamAvailable(playback_stream) == 0) {
      SDL_PauseAudioStreamDevice(playback_stream);
      prompt_texture->load_from_rendered_text(
          "Press 1 to play back. Press 2 to record again", SDL_Color{}, g_font);
      current_state = RecordingState::Recorded;
    }
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    // do something
    prompt_texture->render((SCREEN_WIDTH - prompt_texture->width()) / 2.0, 0);
    if (current_state == RecordingState::SelectingDevice) {
      for (int i = 0; i < g_recording_devices_count; i++) {
        device_textures[i]->render(
            (SCREEN_WIDTH - device_textures[i]->width()) / 2.0,
            i * device_textures[i]->height() + prompt_texture->height());
      }
    }

    SDL_RenderPresent(g_renderer);
  }

  close();

  return 0;
}

bool init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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
  prompt_texture = new Texture(g_renderer);
  font_texture = new Texture(g_renderer);
  for (int i = 0; i < MAX_RECORDING_DEVICES; i++) {
    device_textures[i] = new Texture(g_renderer);
  }
  return true;
}

bool load_media() {
  // if (!prompt_texture->load_from_file("assets/background.png")) {
  //   return false;
  // }
  g_font = TTF_OpenFont("assets/lazy.ttf", 28);
  if (!prompt_texture->load_from_rendered_text(
          "Select recording device:", SDL_Color{0, 0, 0}, g_font)) {
    return false;
  }
  g_audio_device_ids = SDL_GetAudioRecordingDevices(&g_recording_devices_count);
  if (g_audio_device_ids == nullptr) {
    SDL_Log("Failed to get audio recording devices: %s\n", SDL_GetError());
    return false;
  } else if (g_recording_devices_count < 1) {
    SDL_Log("No audio capture devices available!");
    return false;
  }
  if (g_recording_devices_count > MAX_RECORDING_DEVICES) {
    g_recording_devices_count = MAX_RECORDING_DEVICES;
  }

  std::stringstream prompt_text;
  SDL_Color text_color{0, 0, 0, 0};
  for (int i = 0; i < g_recording_devices_count; i++) {
    prompt_text.str("");
    prompt_text << i << ": " << SDL_GetAudioDeviceName(g_audio_device_ids[i]);
    device_textures[i]->load_from_rendered_text(prompt_text.str().c_str(),
                                                text_color, g_font);
  }
  return true;
}

void close() {
  delete prompt_texture;
  prompt_texture = nullptr;
  for (int i = 0; i < MAX_RECORDING_DEVICES; i++) {
    delete device_textures[i];
    device_textures[i] = nullptr;
  }
  SDL_free(g_audio_device_ids);
  g_audio_device_ids = nullptr;

  delete font_texture;
  font_texture = nullptr;
  TTF_CloseFont(g_font);
  g_font = nullptr;
  SDL_DestroyRenderer(g_renderer);
  g_renderer = nullptr;
  SDL_DestroyWindow(g_window);
  g_window = nullptr;
  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
}
