#pragma once

#include <chrono>
namespace Accretion{

class Time{
public:
  void update();

  float get_delta_time() const { return m_delta_time; }
  float get_elapsed() const { return m_elapsed; }
  float get_fps() const { return m_fps; }
  uint64_t get_frame() const { return m_frame; }
private:
  using clock = std::chrono::steady_clock;
  clock::time_point m_last{clock::now()};
  clock::time_point m_start{clock::now()};

  float m_delta_time{};
  float m_elapsed{};
  float m_fps{};
  uint64_t m_frame{};
};

}
