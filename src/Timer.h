#pragma once
#include <cstdint>
class Timer {
public:
  Timer();
  void start();
  void stop();
  void pause();
  void resume();
  uint64_t get_ticks() const;
  bool is_started();
  bool is_paused();
private:
  uint64_t start_ticks;
  uint64_t paused_ticks;
  bool paused;
  bool started;
};
