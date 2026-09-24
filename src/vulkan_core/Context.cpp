#include "vulkan_core/Context.hpp"
#include <SDL3/SDL_events.h>

namespace Accretion{

Context::Context(){
    m_handle_manager = std::make_unique<HandleManager>();
    m_handle_manager->initialize(800, 600, "Accretion");
    m_renderer = std::make_unique<Renderer>(*m_handle_manager);
}

Context::~Context(){
    m_handle_manager->flush_deferred();
}

void Context::run(){
    bool running{true};
    SDL_Event e{};
    while(running){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_EVENT_QUIT)
                running = false;
        }
        if(!running) break;
        m_renderer->draw_frame();
    }
}

}
