#include "core/services/Camera.hpp"

namespace Accretion{

void Camera::update(const InputState& state, float dt){
  if(state.rmb_held){
    m_yaw += state.mouse_delta.x * m_sensitivity;
    m_pitch += state.mouse_delta.y * m_sensitivity;
    m_pitch = glm::clamp(m_pitch, -90.f, 90.f);
  }

  glm::vec3 forward{};
  forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  forward.y = sin(glm::radians(m_pitch));
  forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  forward = glm::normalize(forward);

  glm::vec3 right = glm::normalize(glm::cross(forward, m_up));
  glm::vec3 relative_up = glm::cross(right, forward);
  float velocity = m_speed * dt;
  if (state.key(SDL_SCANCODE_W)) m_position += forward * velocity;
  if (state.key(SDL_SCANCODE_S)) m_position -= forward * velocity;
  if (state.key(SDL_SCANCODE_D)) m_position += right * velocity;
  if (state.key(SDL_SCANCODE_A)) m_position -= right * velocity;
  if (state.key(SDL_SCANCODE_SPACE)) m_position += relative_up * velocity;
  if (state.key(SDL_SCANCODE_LSHIFT)) m_position -= relative_up * velocity;
  
  m_forward = m_position + forward;
}

glm::mat4 Camera::get_view_proj() const{
  glm::mat4 view = glm::lookAt(m_position, m_forward, m_up);
  glm::mat4 proj = glm::perspective(m_fov, m_aspect, m_near, m_far);
  return proj * view;
}

}
