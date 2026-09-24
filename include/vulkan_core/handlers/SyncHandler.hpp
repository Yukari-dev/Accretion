#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"
#include "vulkan_core/utils/VulkanConfigs.hpp"

namespace Accretion{

class SyncHandler : public VulkanHandler{
public:
  SyncHandler();

  void create() override;

  VkSemaphore get_available_image(uint32_t index) { return m_image_available_semaphores[index]; }
  VkSemaphore get_render_finished(uint32_t index) { return m_render_finished_semaphores[index]; }
  VkFence& get_fence(uint32_t index) { return m_in_flight_fences[index]; }
private:
  std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_image_available_semaphores{};
  std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_render_finished_semaphores{};
  std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_in_flight_fences{};
};

}
