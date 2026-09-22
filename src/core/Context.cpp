#include "core/Context.hpp"
#include "core/handlers/GLFWHandler.hpp"
#include "core/handlers/InstanceHandler.hpp"
#include <GLFW/glfw3.h>

namespace Accretion{

Context::Context(){
    add_handler<GLFWHandler>(800, 600, "title");
    add_handler<InstanceHandler>();
}

Context::~Context(){
    flush_deferred();
}

void Context::defer(DeletionFn fn){
    m_deletion_queue.push_back(std::move(fn));
}

void Context::flush_deferred(){
    for(auto it = m_deletion_queue.rbegin(); it != m_deletion_queue.rend(); ++it)
        (*it)();
    m_deletion_queue.clear();
}

}
