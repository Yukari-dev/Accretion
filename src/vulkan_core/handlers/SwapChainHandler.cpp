#include "vulkan_core/handlers/SwapChainHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/SDLHandler.hpp"
#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <stdexcept>

namespace Accretion{

SwapChainHandler::SwapChainHandler() : VulkanHandler("SwapChainHandler", [&]{
  for(auto& image_view : m_image_views)
    vkDestroyImageView(get_dependency<DeviceHandler>().get_device(), image_view, nullptr);
  vkDestroySwapchainKHR(get_dependency<DeviceHandler>().get_device(), m_swap_chain, nullptr);
}){
  request_dependency("SDLHandler");
  request_dependency("SurfaceHandler");
  request_dependency("DeviceHandler");
}

void SwapChainHandler::create(){
  create_swap_chain();
  // create_image_views();
}

void SwapChainHandler::create_swap_chain(){
  VkPhysicalDevice& physical_device = get_dependency<DeviceHandler>().get_physical_device();
  VkDevice& device = get_dependency<DeviceHandler>().get_device();
  VkSurfaceKHR& surface = get_dependency<SurfaceHandler>().get_surface();

  SwapChainSupportProperties properties = query_swap_chain_support_properties(physical_device, surface);
  QueueFamilyIndices indices = find_queue_families(physical_device, surface);
  uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

  VkSurfaceFormatKHR surface_format = choose_format(properties.formats);
  VkPresentModeKHR present_mode = choose_present_mode(properties.present_modes);
  VkExtent2D extent = choose_extent(properties.capabilities);

  m_format = surface_format.format;
  m_extent = extent;

  uint32_t image_count = properties.capabilities.minImageCount + 1;
  if(properties.capabilities.maxImageCount > 0 && image_count > properties.capabilities.maxImageCount)
    image_count = properties.capabilities.maxImageCount;   

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = get_dependency<SurfaceHandler>().get_surface();
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if(indices.graphics_family != indices.present_family){
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  create_info.preTransform = properties.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;

  VkResult result = vkCreateSwapchainKHR(device, &create_info, nullptr, &m_swap_chain);
  if(result != VK_SUCCESS)
    throw std::runtime_error("FATAL ERROR: Failed to create SwapChain.");

  vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, nullptr);
  m_images.resize(image_count);
  vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, m_images.data());
}

void SwapChainHandler::create_image_views(){
  m_image_views.resize(m_images.size());

  for(size_t i = 0; i < m_images.size(); i++){
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = m_images[i];
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = m_format;
    info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;
    VkResult result = vkCreateImageView(get_dependency<DeviceHandler>().get_device(), &info, nullptr, &m_image_views[i]);
    if(result != VK_SUCCESS)
      throw std::runtime_error("FATAL ERROR: Failed to create Image View.");
  }
}

VkSurfaceFormatKHR SwapChainHandler::choose_format(const std::vector<VkSurfaceFormatKHR>& available_Formats){
  for(const auto& format : available_Formats){
    if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return format;
  }
  return available_Formats[0];
}

VkPresentModeKHR SwapChainHandler::choose_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes){
  for(const auto& mode : available_present_modes){
    if(mode == VK_PRESENT_MODE_FIFO_KHR)
      return mode;
  }
  return VK_PRESENT_MODE_MAILBOX_KHR;
}

VkExtent2D SwapChainHandler::choose_extent(const VkSurfaceCapabilitiesKHR& cap){
  if(cap.currentExtent.width != UINT32_MAX)
    return cap.currentExtent;
  
  int width, height;
  SDL_GetWindowSize(get_dependency<SDLHandler>().get_window(), &width, &height);
  VkExtent2D extent{};
  extent.width = width;
  extent.height = height;

  extent.width = std::clamp(extent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
  extent.height = std::clamp(extent.height, cap.minImageExtent.height, cap.maxImageExtent.height);

  return extent;
}

}
