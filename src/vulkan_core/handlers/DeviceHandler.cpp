#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/InstanceHandler.hpp"
#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include "vulkan_core/utils/VulkanUtils.hpp"
#include <map>
#include <set>

namespace Accretion{

DeviceHandler::DeviceHandler() : VulkanHandler("DeviceHandler", [&]{
  if(m_device)
    vkDestroyDevice(m_device, nullptr);
}){
  request_dependency("InstanceHandler");
  request_dependency("SurfaceHandler");
}

void DeviceHandler::create(){
  create_physical_device();
}

void DeviceHandler::create_physical_device(){
  uint32_t count{0};
  VkInstance& instance = get_dependency<InstanceHandler>().get_vk_instance();
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  if(count == 0)
    throw std::runtime_error("Failed to find GPUs with Vulkan Support");
  std::vector<VkPhysicalDevice> available_devices(count);
  vkEnumeratePhysicalDevices(instance, &count, available_devices.data());

  std::multimap<int, VkPhysicalDevice> candidates;
  for(const auto& device : available_devices)
    candidates.insert(std::make_pair(rate_device_suitability(device), device));
  if(candidates.rbegin()->first > 0)
    m_physical_device = candidates.rbegin()->second;
  else
    throw std::runtime_error("Failed to find a suitable GPU");
}

int DeviceHandler::rate_device_suitability(VkPhysicalDevice device){
  if(!is_physical_device_suitable(device)) return 0;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
  int score{0};
  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 1000;

  score += deviceProperties.limits.maxImageDimension2D;

  if(!deviceFeatures.geometryShader)
    return 0;

  return score;
}

bool DeviceHandler::is_physical_device_suitable(VkPhysicalDevice device){
  QueueFamilyIndices indices = FindQueueFamilies(device, get_dependency<SurfaceHandler>().get_surface());
  bool extensionSupport = check_physical_device_extension_support(device);
  //TODO: swapchain format and presnetModes support.
  if(extensionSupport){
    //NOTE: here goes the SwapChainSupprotDetails to get the beeetttterrrr GPU (even tho I only have 1 (integrated btw))
  }
  return indices.IsComplete() && extensionSupport;
}

bool DeviceHandler::check_physical_device_extension_support(VkPhysicalDevice device){
  uint32_t count{0};
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, available_extensions.data());
  std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());
  for(const auto& extension : available_extensions)
    required_extensions.insert(extension.extensionName);
  return !required_extensions.empty();
}

}
