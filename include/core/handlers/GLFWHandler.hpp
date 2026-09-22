#pragma once
#include "core/VulkanHandler.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>

namespace Accretion{

class GLFWHandler : public VulkanHandler{
public:
  GLFWHandler(uint32_t width, uint32_t height, const std::string& title);

  void create() override;
  void close_window();
  bool window_should_close() const;

  GLFWwindow *get_window() { return m_window; }
private:
  GLFWwindow *m_window{};
  uint32_t m_width{}, m_height{};
  std::string m_title{};
};

}
