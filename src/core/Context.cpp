#include "core/Context.hpp"
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace Accretion{

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT *data,
  void *pUserData
){
    const char* sev = "?";
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) sev = "ERROR";
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) sev = "WARN";
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) sev = "INFO";
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) sev = "VERBOSE";

    std::cerr << "[" << sev << "] " << data->pMessage << '\n';
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
  const VkAllocationCallbacks *pAllocator,
  VkDebugUtilsMessengerEXT *pDebugMessenger
){
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if(func != nullptr)
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  else
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(
  VkInstance instance, 
  VkDebugUtilsMessengerEXT debugMessnger,
  const VkAllocationCallbacks *pAllocator
){
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr)
    func(instance, debugMessnger, pAllocator);
}

void populate_debug_messenger_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
}

bool check_validation_layers(const std::vector<const char*> validation_layers){
    uint32_t layerCount{0};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties>  layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for(const char *layerName : validation_layers){
        bool found{false};
        for(const auto& layer : layers){
            if(strcmp(layerName, layer.layerName) == 0){
                found = true;
                break;
            }
        }
        if(!found)
            return false;
    }

    return true;
}

Context::Context(){
    create_instance();
    create_debug_messenger();
}

Context::~Context(){
    flush_deferred();
    if (m_allocator) vmaDestroyAllocator(m_allocator);
    if (m_device) vkDestroyDevice(m_device, nullptr);
    if(m_debug_messenger)
        DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    if (m_instance) vkDestroyInstance(m_instance, nullptr);
    glfwTerminate();
}

void Context::create_instance(){
    if (!glfwInit())
        throw std::runtime_error("FATAL ERROR: Failed to initialize GLFW.");
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Accretion";
    appInfo.applicationVersion = VK_VERSION_1_0;
    appInfo.pEngineName = "Accretion engine";
    appInfo.engineVersion = VK_VERSION_1_0;
    appInfo.apiVersion = VK_API_VERSION_1_3;;

    auto extensions = get_required_glfw_extensions();

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();

    if(m_enable_validation_layers && !check_validation_layers(m_validation_layers))
        throw std::runtime_error("ERROR: Validation Layers is needed, but not available.");

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(m_enable_validation_layers){
        info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
        info.ppEnabledLayerNames = m_validation_layers.data();
        populate_debug_messenger_debug_create_info(debugCreateInfo);
        info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }

    VkResult result = vkCreateInstance(&info, nullptr, &m_instance);
    if(result != VK_SUCCESS)
        throw std::runtime_error("FATAL ERROR: Failed to create VkInstance.");
}

void Context::create_debug_messenger(){
    if(!m_enable_validation_layers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populate_debug_messenger_debug_create_info(createInfo);
    VkResult result = CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debug_messenger);
    if(result != VK_SUCCESS)
        throw std::runtime_error("FATAL ERROR: Failed to Setup Debug Messenger.");
}

std::vector<const char*> Context::get_required_glfw_extensions(){
    uint32_t count{};
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
    if(m_enable_validation_layers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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
