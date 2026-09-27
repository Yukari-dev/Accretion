#pragma once
#include <SDL3/SDL.h>
#include <array>
#include <glm/glm.hpp>

namespace Accretion{

struct InputState{
  std::array<bool, SDL_SCANCODE_COUNT> keys{};
  glm::vec2 mouse_position{};
  bool lmb_held{};
  bool rmb_held{};

  glm::vec2 mouse_delta{0.0f};
  bool lmb_clicked{};
  bool rmb_clicked{};

  bool key(SDL_Scancode s) const { return keys[s]; }
};

class InputSystem{
public:
  void begin_frame();
  void process_event(const SDL_Event& event);

  InputState& get_state() { return m_state; }
private:
  InputState m_state{};
};

}
