#include "vulkan_core/HandleManager.hpp"
#include "vulkan_core/handlers/CommandBufferHandler.hpp"
#include "vulkan_core/handlers/CommandPoolHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/InstanceHandler.hpp"
#include "vulkan_core/handlers/SDLHandler.hpp"
#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include "vulkan_core/handlers/SwapChainHandler.hpp"
#include "vulkan_core/handlers/SyncHandler.hpp"
#include "vulkan_core/utils/VulkanConfigs.hpp"

namespace Accretion{

void HandleManager::initialize(uint32_t width, uint32_t height, const std::string& title){
  add_handler<SDLHandler>(width, height, title);
  add_handler<InstanceHandler>();
  add_handler<SurfaceHandler>();
  add_handler<DeviceHandler>();
  add_handler<SwapChainHandler>();
  add_handler<CommandPoolHandler>();
  add_handler<CommandBufferHandler>();
  add_handler<SyncHandler>();

  fulfill_dependencies_requests();

  initialize_handlers();
  /* 

  bool running{true};

  uint32_t current_frame{};

  auto& sync = find_handler_by_name<SyncHandler>("SyncHandler");
  auto& dev = find_handler_by_name<DeviceHandler>("DeviceHandler");
  auto& swap_chain = find_handler_by_name<SwapChainHandler>("SwapChainHandler");
  auto& command_buffer = find_handler_by_name<CommandBufferHandler>("CommandBufferHandler");
  VkDevice device = dev.get_device();

  uint32_t image_index{};
  SDL_Event e{};
  while(running){
    while(SDL_PollEvent(&e)){
      if(e.type == SDL_EVENT_QUIT)
        running = false;
    }
    if(!running) break;
    VkFence current_fence = sync.get_fence(current_frame);
    vkWaitForFences(device, 1, &current_fence, VK_TRUE, UINT64_MAX);

    VkResult acquire = vkAcquireNextImageKHR(
      device, swap_chain.get_swap_chain(), UINT64_MAX, sync.get_available_image(current_frame),
      VK_NULL_HANDLE, &image_index
    );

    vkResetFences(device, 1, &current_fence);

    if(acquire == VK_ERROR_OUT_OF_DATE_KHR || acquire == VK_SUBOPTIMAL_KHR){
      find_handler_by_name<SDLHandler>("SDLHandler").update_size();
      swap_chain.create();
      continue;
    }

    VkCommandBuffer& cmd = command_buffer.get_command_buffer(current_frame);
    vkResetCommandBuffer(cmd, 0);

    VkRenderingInfo rendering_info{};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea = {{0, 0}, {width, height}};
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = 1;

    VkRenderingAttachmentInfo attach_info{};
    attach_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attach_info.imageView = swap_chain.get_image_views()[image_index];
    attach_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attach_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attach_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attach_info.clearValue = {{1.0f, 1.0f, 1.0f, 1.0f}};

    rendering_info.pColorAttachments = &attach_info;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = swap_chain.get_images()[image_index];
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(
      cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      0, 0, nullptr, 0, nullptr, 1, &barrier
    );

    vkCmdBeginRendering(cmd, &rendering_info);

    vkCmdEndRendering(cmd);

    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(
      cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
      0, 0, nullptr, 0, nullptr, 1, &barrier
    );

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore wait_sems[] = {sync.get_available_image(current_frame)};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_sems;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd;
    VkSemaphore signal_sems[] = {sync.get_render_finished(current_frame)};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_sems;

    vkQueueSubmit(dev.get_graphics_queue(), 1, &submit_info, current_fence);

    VkPresentInfoKHR present{};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = signal_sems;
    present.swapchainCount = 1;
    VkSwapchainKHR swapchains[] = {swap_chain.get_swap_chain()};
    present.pSwapchains = swapchains;
    present.pImageIndices = &image_index;

    VkResult present_result = vkQueuePresentKHR(dev.get_graphics_queue(), &present);
    if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR)
      swap_chain.create();

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  vkDeviceWaitIdle(device);
  */
}

void HandleManager::initialize_handlers(){
  for(auto& [_, handler] : m_handlers)
  handler->create();
}

void HandleManager::fulfill_dependencies_requests(){
  for(auto& [name, handle] : m_handlers){
    auto requests = handle->get_requests();
    if(requests.empty()) continue;
    for(const auto& request : requests){
      VulkanHandler& dep = find_handler_by_name<VulkanHandler>(request);
      handle->add_dependency(request, &dep);
    }
    handle->clear_requests();
  }
}

void HandleManager::defer(DeletionFn fn){
  m_deletion_queue.emplace_back(fn);
}

void HandleManager::flush_deferred(){
  for(auto it = m_deletion_queue.rbegin(); it != m_deletion_queue.rend(); ++it)
    (*it)();
  m_deletion_queue.clear();
}

}
