#pragma once
#include "vulkan_core/VulkanHandler.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Accretion{

class SwapChainHandler : public VulkanHandler{
public:
  SwapChainHandler();

  void create() override;

  VkSwapchainKHR& get_swap_chain() { return m_swap_chain; }
  const std::vector<VkImage>& get_images() const { return m_images; }
  const std::vector<VkImageView>& get_image_views() const { return m_image_views; }
  const VkExtent2D& get_extent() { return m_extent; }
  const VkFormat& get_format() { return m_format; }
private:
  void create_swap_chain();
  void create_image_views();
  VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR>& available_Formats);
  VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
  VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& cap);
private:
  VkSwapchainKHR m_swap_chain{VK_NULL_HANDLE};
  std::vector<VkImage> m_images{};
  std::vector<VkImageView> m_image_views{};

  VkFormat m_format{};
  VkExtent2D m_extent{};
};

}
