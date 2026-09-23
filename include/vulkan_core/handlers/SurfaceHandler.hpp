#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion{

class SurfaceHandler : public VulkanHandler{
public:
  SurfaceHandler();

  void create() override;

  VkSurfaceKHR& get_surface() { return m_surface; }
private:
  VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};

}

