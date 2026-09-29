#pragma once
#include "core/services/IService.hpp"
#include "core/InputSystem.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Accretion{

class Camera : public IService{
public:
  void update(const InputState& state, float dt);

  glm::mat4 get_view_proj() const;
  glm::vec3 get_position() const { return m_position; }

private:
  glm::vec3 m_position{0.f, 0.f, 3.f};
  glm::vec3 m_forward{};
  glm::vec3 m_up{0.0f, 1.0f, 0.0f};

  float m_fov{glm::radians(60.0f)};
  float m_near{0.1f};
  float m_far{100.f};
  float m_aspect{16.0 / 9.0f};
  
  float m_yaw{-90.f};
  float m_pitch{};

  float m_speed{3.0f};
  float m_sensitivity{0.15f};
};

}
