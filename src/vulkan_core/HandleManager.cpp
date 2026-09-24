#include "vulkan_core/HandleManager.hpp"
#include "vulkan_core/handlers/CommandBufferHandler.hpp"
#include "vulkan_core/handlers/CommandPoolHandler.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include "vulkan_core/handlers/InstanceHandler.hpp"
#include "vulkan_core/handlers/SDLHandler.hpp"
#include "vulkan_core/handlers/SurfaceHandler.hpp"
#include "vulkan_core/handlers/SwapChainHandler.hpp"

namespace Accretion{

void HandleManager::initialize(uint32_t width, uint32_t height, const std::string& title){
  add_handler<SDLHandler>(width, height, title);
  add_handler<InstanceHandler>();
  add_handler<SurfaceHandler>();
  add_handler<DeviceHandler>();
  add_handler<SwapChainHandler>();
  add_handler<CommandPoolHandler>();
  add_handler<CommandBufferHandler>();

  fulfill_dependencies_requests();

  initialize_handlers();
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
