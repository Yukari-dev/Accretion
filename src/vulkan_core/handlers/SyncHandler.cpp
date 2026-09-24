#include "vulkan_core/handlers/SyncHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"

namespace Accretion{

SyncHandler::SyncHandler() : VulkanHandler("SyncHandler", [&]{
  VkDevice& device = get_dependency<DeviceHandler>().get_device();
  for(auto& sem : m_image_available_semaphores)
    vkDestroySemaphore(device, sem, nullptr);
  for(auto& sem : m_render_finished_semaphores)
    vkDestroySemaphore(device, sem, nullptr);
  for(auto& f : m_in_flight_fences)
    vkDestroyFence(device, f, nullptr);
}){
  request_dependency("DeviceHandler");
}

void SyncHandler::create(){
  VkSemaphoreCreateInfo sem_info{};
  sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VkDevice& device = get_dependency<DeviceHandler>().get_device();

  for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
    if(vkCreateSemaphore(device, &sem_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS)
      throw std::runtime_error("FATAL ERROR: Failed to create per frame Semaphore.");
    if(vkCreateFence(device, &fence_info, nullptr, &m_in_flight_fences[i]))
      throw std::runtime_error("FATAL ERROR: Failed to create per frame Fence.");
  }

  for(auto& sem : m_render_finished_semaphores){
    if(vkCreateSemaphore(device, &sem_info, nullptr, &sem) != VK_SUCCESS)
      throw std::runtime_error("FATAL ERROR: Failed to create Render Finished Semaphore");
  }
}

}
