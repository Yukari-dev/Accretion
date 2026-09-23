#include "vulkan_core/utils/VulkanUtils.hpp"

namespace Accretion{


QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface){
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

}
