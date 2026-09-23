#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/InstanceHandler.hpp"
#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include <map>
#include <set>
#include <stdexcept>

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
  create_logical_device();
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

void DeviceHandler::create_logical_device(){
  m_indices = FindQueueFamilies(m_physical_device, get_dependency<SurfaceHandler>().get_surface());

  std::set<uint32_t> unique_families{m_indices.graphics_family.value(), m_indices.present_family.value()};
  std::vector<VkDeviceQueueCreateInfo> queues_info{};
  float priority{1.0f};
  for(const auto& family : unique_families){
    VkDeviceQueueCreateInfo device_queue{};
    device_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; 
    device_queue.pQueuePriorities = &priority;
    device_queue.queueFamilyIndex = family;
    device_queue.queueCount = 1;
    queues_info.push_back(device_queue);
  }

  VkPhysicalDeviceFeatures device_features{};
  VkDeviceCreateInfo device_info{};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.queueCreateInfoCount = static_cast<uint32_t>(queues_info.size());
  device_info.pQueueCreateInfos = queues_info.data();
  device_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
  device_info.ppEnabledExtensionNames = m_device_extensions.data();
  device_info.pEnabledFeatures = &device_features;

  VkResult result = vkCreateDevice(m_physical_device, &device_info, nullptr, &m_device);
  if(result != VK_SUCCESS)
    throw std::runtime_error("FATAL ERROR: Failed to create logical device.");
  vkGetDeviceQueue(m_device, m_indices.graphics_family.value(), 0, &m_graphics_queue);
  vkGetDeviceQueue(m_device, m_indices.present_family.value(), 0, &m_present_queue);
}

int DeviceHandler::rate_device_suitability(VkPhysicalDevice device){
  if(!is_physical_device_suitable(device)) return 0;
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceProperties(device, &device_properties);
  vkGetPhysicalDeviceFeatures(device, &device_features);
  int score{0};
  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 1000;

  score += device_properties.limits.maxImageDimension2D;

  if(!device_features.geometryShader)
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
