#include "vulkan_core/utils/VulkanUtils.hpp"

namespace Accretion{


QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface){
  QueueFamilyIndices indices{};
  uint32_t count{};
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());
  int i{};
  for(const auto& family : families){
    VkBool32 presentSupport{false};
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphics_family = i;
    if(presentSupport)
      indices.present_family = i;
    if(indices.IsComplete())
      break;
    i++;
  }

  return indices;
}

SwapChainSupportProperties query_swap_chain_support_properties(VkPhysicalDevice device, VkSurfaceKHR surface){
  SwapChainSupportProperties properties{};
  
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &properties.capabilities);
  
  uint32_t format_count{0};
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
  if(format_count != 0){
    properties.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, properties.formats.data());
  }

  uint32_t presents_count{0};
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presents_count, nullptr);
  if(presents_count != 0){
    properties.present_modes.resize(presents_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presents_count, properties.present_modes.data());
  }

  return properties;
}

}
