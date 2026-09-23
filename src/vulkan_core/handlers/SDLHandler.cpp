#include "vulkan_core/handlers/SDLHandler.hpp"

namespace Accretion{

SDLHandler::SDLHandler(uint32_t width, uint32_t height, const std::string& title) 
  : VulkanHandler("SDLHandler", [&]{
  if(m_window)
    SDL_DestroyWindow(m_window);
  SDL_Quit();
}), m_width(width), m_height(height), m_title(title){
  if(!SDL_Init(SDL_INIT_VIDEO))
    throw std::runtime_error("FATAL ERROR: Failed to initialize SDL.");
}

void SDLHandler::create(){
  m_window = SDL_CreateWindow(m_title.c_str(), m_width, m_height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
  if (!m_window)
    throw std::runtime_error("FATAL ERROR: Failed to create SDL Window.");
  SDL_ShowWindow(m_window);
}

}
