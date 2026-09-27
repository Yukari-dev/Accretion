#include "vulkan_core/Renderer.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Accretion{

Renderer::Renderer(HandleManager& handler_manager, Camera& camera) : m_handler_manager(handler_manager), m_camera(camera){
  m_sdl = &m_handler_manager.find_handler_by_name<SDLHandler>("SDLHandler");
  m_device = &m_handler_manager.find_handler_by_name<DeviceHandler>("DeviceHandler");
  m_swap_chain = &m_handler_manager.find_handler_by_name<SwapChainHandler>("SwapChainHandler");
  m_command_buffer = &m_handler_manager.find_handler_by_name<CommandBufferHandler>("CommandBufferHandler");
  m_sync = &m_handler_manager.find_handler_by_name<SyncHandler>("SyncHandler");
  m_pipeline = &m_handler_manager.find_handler_by_name<PipelineHandler>("PipelineHandler");
}

void Renderer::draw_frame(){
  if(!begin_frame()) return;
  update_frame();
  record_frame();
  submit_frame();
  present_frame();
  end_frame();
}

bool Renderer::begin_frame(){
  VkFence current_fence = m_sync->get_fence(m_current_frame);
  vkWaitForFences(m_device->get_device(), 1, &current_fence, VK_TRUE, UINT64_MAX);

  VkResult acquire = vkAcquireNextImageKHR(
    m_device->get_device(), m_swap_chain->get_swap_chain(), UINT64_MAX, 
    m_sync->get_available_image(m_current_frame), VK_NULL_HANDLE, &m_image_index
  );

  if(acquire == VK_ERROR_OUT_OF_DATE_KHR || acquire == VK_SUBOPTIMAL_KHR){
    m_swap_chain->create();
    return false;
  }
  return true;
}

void Renderer::update_frame(){
  VkCommandBuffer& cmd = m_command_buffer->get_command_buffer(m_current_frame);
  vkResetCommandBuffer(cmd, 0);

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(cmd, &begin_info);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = m_swap_chain->get_images()[m_image_index];
  barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  barrier.srcAccessMask = 0;
  barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  vkCmdPipelineBarrier(
    cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    0, 0, nullptr, 0, nullptr, 1, &barrier
  );

  VkRenderingAttachmentInfo color_info{};
  color_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  color_info.imageView = m_swap_chain->get_image_views()[m_image_index];
  color_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_info.clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}};

  VkRenderingInfo rendering_info{};
  rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  rendering_info.renderArea = {{0, 0}, {m_sdl->get_width(), m_sdl->get_height()}};
  rendering_info.layerCount = 1;
  rendering_info.colorAttachmentCount = 1;
  rendering_info.pColorAttachments = &color_info;

  vkCmdBeginRendering(cmd, &rendering_info);

  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get_pipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(m_sdl->get_width());
  viewport.height = static_cast<float>(m_sdl->get_height());
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(cmd, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {m_sdl->get_width(), m_sdl->get_height()};
  vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void Renderer::record_frame(){
  VkCommandBuffer& cmd = m_command_buffer->get_command_buffer(m_current_frame);
  ShaderInstance& instance = m_pipeline->get_shader_instance();

  instance.set(instance.get_location("camera.u_view_proj"), m_camera.get_view_proj());
  instance.bind(cmd, m_pipeline->get_pipeline_layout());
  vkCmdDraw(cmd, 3, 1, 0, 0);
}

void Renderer::submit_frame(){
  VkCommandBuffer& cmd = m_command_buffer->get_command_buffer(m_current_frame);
  vkCmdEndRendering(cmd);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = m_swap_chain->get_images()[m_image_index];
  barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.dstAccessMask = 0;
  barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

  vkCmdPipelineBarrier(
    cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    0, 0, nullptr, 0, nullptr, 1, &barrier
  );

  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_sems[] = {m_sync->get_available_image(m_current_frame)};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_sems;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd;
  VkSemaphore signal_sems[] = {m_sync->get_render_finished(m_current_frame)};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_sems;

  VkFence current_fence = m_sync->get_fence(m_current_frame);
  vkResetFences(m_device->get_device(), 1, &current_fence);
  vkQueueSubmit(m_device->get_graphics_queue(), 1, &submit_info, current_fence);
}

void Renderer::present_frame(){
  VkSemaphore signal_sems[] = {m_sync->get_render_finished(m_current_frame)};
  VkPresentInfoKHR present{};
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = signal_sems;
  present.swapchainCount = 1;
  VkSwapchainKHR swapchains[] = {m_swap_chain->get_swap_chain()};
  present.pSwapchains = swapchains;
  present.pImageIndices = &m_image_index;

  VkResult present_result = vkQueuePresentKHR(m_device->get_graphics_queue(), &present);
  if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR){
    vkDeviceWaitIdle(m_device->get_device());
    m_swap_chain->create();
  }
}

void Renderer::end_frame(){
  m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

}
