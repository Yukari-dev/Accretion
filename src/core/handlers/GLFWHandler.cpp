#include "core/handlers/GLFWHandler.hpp"
#include <iostream>
#include <stdexcept>

namespace Accretion{

GLFWHandler::GLFWHandler(uint32_t width, uint32_t height, const std::string& title):
  VulkanHandler("GLFWHandler", [&]{
    glfwDestroyWindow(m_window);
    glfwTerminate();
  }), m_width(width), m_height(height), m_title(title) {
  if (!glfwInit())
    throw std::runtime_error("FATAL ERROR: Failed to initialize GLFW.");
}

void GLFWHandler::create(){
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
  if (!m_window)
    throw std::runtime_error("FATAL ERROR: Failed to create GLFW Window.");
}

void GLFWHandler::close_window(){
  glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

bool GLFWHandler::window_should_close() const{
  return glfwWindowShouldClose(m_window);
}

}
