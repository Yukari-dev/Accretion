#include "core/Time.hpp"

namespace Accretion{

void Time::update(){
  auto now = clock::now();

  m_delta_time = std::chrono::duration<float>(now - m_last).count();
  if (m_delta_time > 0.1f) m_delta_time = 0.1f;
  m_last = now;
  m_elapsed = std::chrono::duration<float>(now - m_start).count();
  m_frame++;

  if(m_delta_time > 0.f){
    float instant_fps = 1.0f / m_delta_time;
    m_fps = m_fps == 0.0f ? instant_fps : m_fps * 0.95 + instant_fps * 0.05f;
  }
}

}
