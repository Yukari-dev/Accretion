#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include "vulkan_core/handlers/InstanceHandler.hpp"
#include "vulkan_core/handlers/SDLHandler.hpp"
#include <GLFW/glfw3.h>

namespace Accretion{


SurfaceHandler::SurfaceHandler() : VulkanHandler("SurfaceHandler", [&]{
  vkDestroySurfaceKHR(get_dependency<InstanceHandler>().get_vk_instance(), m_surface, nullptr);
}) {
  request_dependency("SDLHandler");
  request_dependency("InstanceHandler");
}

void SurfaceHandler::create(){
  VkInstance& instance = get_dependency<InstanceHandler>().get_vk_instance();
  auto *window = get_dependency<SDLHandler>().get_window();
  SDL_Vulkan_CreateSurface(window, instance, nullptr, &m_surface);
}

}
