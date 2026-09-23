#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion{

class DeviceHandler : public VulkanHandler{
public:
  DeviceHandler();
  
  void create() override;
private:
  void create_physical_device();

  int rate_device_suitability(VkPhysicalDevice device);
  bool is_physical_device_suitable(VkPhysicalDevice device);
  bool check_physical_device_extension_support(VkPhysicalDevice device);
private:
  VkDevice m_device{VK_NULL_HANDLE};
  VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
  const std::vector<const char*> m_device_extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}
