#include "vulkan_core/handlers/CommandBufferHandler.hpp"
#include "vulkan_core/handlers/CommandPoolHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include <stdexcept>

namespace Accretion{

CommandBufferHandler::CommandBufferHandler() : VulkanHandler("CommandBufferHandler", [&]{}){
  request_dependency("DeviceHandler");
  request_dependency("CommandPoolHandler");
}

void CommandBufferHandler::create(){
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandPool = get_dependency<CommandPoolHandler>().get_command_pool();
  info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  VkResult result = vkAllocateCommandBuffers(get_dependency<DeviceHandler>().get_device(), &info, m_command_buffers.data());
  if(result != VK_SUCCESS)
    throw std::runtime_error("FATAL ERROR: Failed to allocate CommandBuffer.");
}

}
