#include "vulkan_core/Context.hpp"

namespace Accretion{

Context::Context(){
    m_handle_manager = std::make_unique<HandleManager>();
    m_handle_manager->initialize(800, 600, "Accretion");
}

Context::~Context(){
    m_handle_manager->flush_deferred();
}

void Context::run(){

}

}
