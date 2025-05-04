#include <SDL3/SDL_surface.h>
class DataStream {
public:
  DataStream();
  ~DataStream();
  bool load_media();
  void free();
  void *get_buffer();

private:
  SDL_Surface *m_images[4];
  int m_current_image;
  int m_delay_frames;
};
