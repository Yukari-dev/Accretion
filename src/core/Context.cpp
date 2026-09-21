#include "core/Context.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

namespace Accretion{

Context::Context(){
    create_instance();
}

Context::~Context(){
    flush_deferred();
    if (m_allocator) vmaDestroyAllocator(m_allocator);
    if (m_device) vkDestroyDevice(m_device, nullptr);
    if (m_instance) vkDestroyInstance(m_instance, nullptr);
}

void Context::create_instance(){
    glfwInit();
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Accretion";
    appInfo.applicationVersion = VK_VERSION_1_0;
    appInfo.pEngineName = "Accretion engine";
    appInfo.engineVersion = VK_VERSION_1_0;
    appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

    auto extensions = GetRequiredGLFWExtensions();

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();

    VkResult result = vkCreateInstance(&info, nullptr, &m_instance);
    if(result != VK_SUCCESS)
        throw std::runtime_error("FATAL ERROR: Failed to create VkInstance.");
}

std::vector<const char*> Context::GetRequiredGLFWExtensions(){
    uint32_t count{};
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
    return extensions;
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
