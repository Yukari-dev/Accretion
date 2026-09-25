#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Accretion{

struct QueueFamilyIndices{
  std::optional<uint32_t> graphics_family{};
  std::optional<uint32_t> present_family{};

  bool IsComplete() const {
    return graphics_family.has_value() && present_family.has_value();
  }
};

QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

struct SwapChainSupportProperties{
  VkSurfaceCapabilitiesKHR capabilities{}; 
  std::vector<VkSurfaceFormatKHR> formats{};
  std::vector<VkPresentModeKHR> present_modes{};
};

SwapChainSupportProperties query_swap_chain_support_properties(VkPhysicalDevice device, VkSurfaceKHR surface);

std::vector<uint32_t> ReadFileUint32(const std::string& filename);

}
