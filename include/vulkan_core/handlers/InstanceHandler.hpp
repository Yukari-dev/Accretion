#pragma once
#include "vulkan_core/VulkanHandler.hpp"
#include <vulkan/vulkan.hpp>

namespace Accretion{

class InstanceHandler : public VulkanHandler{
public:
  InstanceHandler();

  void create() override;
  VkInstance& get_vk_instance() { return m_instance; }
private:
  void create_instance();
  void setup_debug_messenger();

  std::vector<const char *> get_required_glfw_extensions();
  std::vector<const char *> get_required_sdl_extensions();
private:
  VkInstance m_instance{VK_NULL_HANDLE};

  VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};
  std::vector<const char*> m_validation_layers{"VK_LAYER_KHRONOS_validation"};

  #ifndef NDEBUG
    const bool m_enable_validation_layers{false};
  #else
    const bool m_enable_validation_layers{true};
  #endif
};

}
