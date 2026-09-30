#include "vulkan_core/Context.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include <SDL3/SDL_events.h>
#include <vulkan/vulkan_core.h>

namespace Accretion{

Context::Context(){
    m_handle_manager = std::make_unique<HandleManager>();
    m_handle_manager->initialize(800, 600, "Accretion");
    m_services_registry.initialize();
    m_renderer = std::make_unique<Renderer>(*m_handle_manager, m_services_registry.get<Camera>());
}

Context::~Context(){
    auto& device_handler = m_handle_manager->find_handler_by_name<DeviceHandler>("DeviceHandler");
    if (device_handler.get_device() != VK_NULL_HANDLE)
        vkDeviceWaitIdle(device_handler.get_device());

    m_renderer.reset();
    m_handle_manager->flush_deferred();
}

void Context::run(){
    bool running{true};
    SDL_Event e{};

    FrameContext frame_ctx{
        .time = m_time, 
        .input = m_input, 
        .service_registry = m_services_registry, 
    };

    Camera& camera = m_services_registry.get<Camera>();
    while(running){
        m_input.begin_frame();
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_EVENT_QUIT) running = false;
            m_input.process_event(e);
        }
        if(!running) break;
        m_time.update();
        camera.update(m_input.get_state(), m_time.get_delta_time());
        m_renderer->draw_frame();
    }
    vkDeviceWaitIdle(m_handle_manager->find_handler_by_name<DeviceHandler>("DeviceHandler").get_device());
}

}
