#include "vulkan_core/Context.hpp"
#include "vulkan_core/handlers/DeviceHandler.hpp"
#include <SDL3/SDL_events.h>

namespace Accretion{

Context::Context(){
    m_handle_manager = std::make_unique<HandleManager>();
    m_handle_manager->initialize(800, 600, "Accretion");
    m_renderer = std::make_unique<Renderer>(*m_handle_manager, m_camera);
}

Context::~Context(){
    m_handle_manager->flush_deferred();
}

void Context::run(){
    bool running{true};
    SDL_Event e{};
    while(running){
        m_input.begin_frame();
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_EVENT_QUIT) running = false;
            m_input.process_event(e);
        }
        if(!running) break;
        m_time.update();
        m_camera.update(m_input.get_state(), m_time.get_delta_time());
        m_renderer->draw_frame();
    }
    vkDeviceWaitIdle(m_handle_manager->find_handler_by_name<DeviceHandler>("DeviceHandler").get_device());
}

}
