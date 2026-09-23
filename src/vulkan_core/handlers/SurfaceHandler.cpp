#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include "vulkan_core/handlers/GLFWHandler.hpp"
#include "vulkan_core/handlers/InstanceHandler.hpp"
#include <GLFW/glfw3.h>

namespace Accretion{


SurfaceHandler::SurfaceHandler() : VulkanHandler("SurfaceHandler", [&]{
  vkDestroySurfaceKHR(get_dependency<InstanceHandler>().get_vk_instance(), m_surface, nullptr);
}) {
  request_dependency("GLFWHandler");
  request_dependency("InstanceHandler");
}

void SurfaceHandler::create(){
  VkInstance& instance = get_dependency<InstanceHandler>().get_vk_instance();
  GLFWwindow *window = get_dependency<GLFWHandler>().get_window();
  glfwCreateWindowSurface(instance, window, nullptr, &m_surface);
}

}
