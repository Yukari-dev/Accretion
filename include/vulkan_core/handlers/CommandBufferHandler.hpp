#pragma once
#include <array>
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"
#include "vulkan_core/utils/VulkanConfigs.hpp"

namespace Accretion{

class CommandBufferHandler : public VulkanHandler{
public:
  CommandBufferHandler();

  void create() override;

  VkCommandBuffer& get_command_buffer(uint32_t index) { return m_command_buffers[index]; }
private:
  std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_command_buffers{};
};

}
