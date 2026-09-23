#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion{

class SDLHandler : public VulkanHandler{
public:
  SDLHandler(uint32_t width, uint32_t height, const std::string& title);

  void create() override;
  void run();

  SDL_Window *get_window() { return m_window; }
private:
  SDL_Window *m_window{nullptr};
  uint32_t m_width{}, m_height{};
  std::string m_title{};
};

}
