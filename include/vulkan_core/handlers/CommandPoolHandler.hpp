#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion {

class CommandPoolHandler : public VulkanHandler{
public:
  CommandPoolHandler();

  void create() override;

  VkCommandPool& get_command_pool() { return m_command_pool; }
private:
  VkCommandPool m_command_pool{};
};

}
