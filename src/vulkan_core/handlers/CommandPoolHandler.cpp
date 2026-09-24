#include "vulkan_core/handlers/CommandPoolHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"

namespace Accretion{

CommandPoolHandler::CommandPoolHandler() : VulkanHandler("CommandPoolHandler", [&]{
  if(m_command_pool)
    vkDestroyCommandPool(get_dependency<DeviceHandler>().get_device(), m_command_pool, nullptr);
}) {
  request_dependency("DeviceHandler");
}

void CommandPoolHandler::create(){
  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = get_dependency<DeviceHandler>().get_graphics_queue_family();

  VkResult result = vkCreateCommandPool(get_dependency<DeviceHandler>().get_device(), &info, nullptr, &m_command_pool);
  if(result != VK_SUCCESS)
    throw std::runtime_error("FATAL ERROR: Failed to create Command Pool");
}


}
