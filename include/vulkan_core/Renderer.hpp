#pragma once
#include "vulkan_core/HandleManager.hpp"
#include "vulkan_core/handlers/CommandBufferHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/PipelineHandler.hpp"
#include "vulkan_core/handlers/SDLHandler.hpp"
#include "vulkan_core/handlers/SwapChainHandler.hpp"
#include "vulkan_core/handlers/SyncHandler.hpp"

namespace Accretion{

class Renderer{
public:
  Renderer(HandleManager& handler_manager);

  void draw_frame();
private:
  bool begin_frame();
  void update_frame();
  void record_frame();
  void submit_frame();
  void present_frame();
  void end_frame();
private:
  uint32_t m_current_frame{}, m_image_index{};

  HandleManager& m_handler_manager;

  SDLHandler* m_sdl{nullptr};
  DeviceHandler* m_device{nullptr};
  SwapChainHandler* m_swap_chain{nullptr};
  CommandBufferHandler* m_command_buffer{nullptr};
  SyncHandler* m_sync{nullptr};
  PipelineHandler* m_pipeline{nullptr};
};

}
