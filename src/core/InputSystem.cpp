#include "core/InputSystem.hpp"

namespace Accretion{

void InputSystem::begin_frame(){
  m_state.mouse_delta = {0.0f, 0.0f};
  m_state.lmb_clicked = false;
  m_state.rmb_clicked = false;
}

void InputSystem::process_event(const SDL_Event& e){
  switch(e.type){
    case SDL_EVENT_KEY_DOWN:
      m_state.keys[e.key.scancode] = true;
      break;
    case SDL_EVENT_KEY_UP:
      m_state.keys[e.key.scancode] = false;
      break;
    case SDL_EVENT_MOUSE_MOTION:
      m_state.mouse_position = {e.motion.x, e.motion.y};
      m_state.mouse_delta += glm::vec2{e.motion.xrel, e.motion.yrel};
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (e.button.button == SDL_BUTTON_LEFT){ 
        m_state.lmb_held = true; 
        m_state.lmb_clicked = true;
      }
      if (e.button.button == SDL_BUTTON_RIGHT){
        m_state.rmb_held = true;
        m_state.rmb_clicked = true;
      }
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      if (e.button.button == SDL_BUTTON_LEFT)
        m_state.lmb_held = false;
      if (e.button.button == SDL_BUTTON_RIGHT)
        m_state.rmb_held = false;
      break;
    default:
      break;
  }
}

}
