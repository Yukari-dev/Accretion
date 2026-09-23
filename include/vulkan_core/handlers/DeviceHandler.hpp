#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"
#include "vulkan_core/utils/VulkanUtils.hpp"

namespace Accretion{

class DeviceHandler : public VulkanHandler{
public:
  DeviceHandler();
  
  void create() override;

  VkDevice& get_device() { return m_device; }
  VkPhysicalDevice& get_physical_device() { return m_physical_device; }
  uint32_t get_graphics_queue_family() const { return m_indices.graphics_family.value(); }
private:
  void create_physical_device();
  void create_logical_device();

  int rate_device_suitability(VkPhysicalDevice device);
  bool is_physical_device_suitable(VkPhysicalDevice device);
  bool check_physical_device_extension_support(VkPhysicalDevice device);
private:
  VkDevice m_device{VK_NULL_HANDLE};
  VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
  QueueFamilyIndices m_indices{};
  VkQueue m_graphics_queue{};
  VkQueue m_present_queue{};
  const std::vector<const char*> m_device_extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}
