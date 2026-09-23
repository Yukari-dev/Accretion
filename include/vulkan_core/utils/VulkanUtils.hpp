#pragma once
#include <cstdint>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace Accretion{

struct QueueFamilyIndices{
  std::optional<uint32_t> graphics_family{};
  std::optional<uint32_t> present_family{};

  bool IsComplete() const {
    return graphics_family.has_value() && present_family.has_value();
  }
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

}
