#include "DataStream.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <sstream>

DataStream::DataStream() {
  m_images[0] = nullptr;
  m_images[1] = nullptr;
  m_images[2] = nullptr;
  m_images[3] = nullptr;

  m_current_image = 0;
  m_delay_frames = 4;
}

DataStream::~DataStream() {
  this->free();
}

bool DataStream::load_media() {
  for (int i = 0; i < 4; i++) {
    std::stringstream path;
    path << "assets/foo_walk_" << i << ".png";
    SDL_Surface *loaded_surface = IMG_Load(path.str().c_str());
    if (loaded_surface == nullptr) {
      SDL_Log("Unable to load image %s. SDL_image error: %s\n",
              path.str().c_str(), SDL_GetError());
      return false;
    }

    m_images[i] = SDL_ConvertSurface(loaded_surface, SDL_PIXELFORMAT_RGBA8888);
    SDL_DestroySurface(loaded_surface);
  }
  return true;
}

void DataStream::free() {
  for(int i = 0; i < 4; i++) {
    SDL_DestroySurface(m_images[i]);
    m_images[i] = nullptr;
  }
}

void *DataStream::get_buffer() {
  m_delay_frames--;
  if (m_delay_frames <= 0) {
    m_current_image++;
    m_delay_frames = 4;
  }

  if (m_current_image >= 4) {
    m_current_image = 0;
  }
  
  return m_images[m_current_image]->pixels;
}
