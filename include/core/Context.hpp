#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan.hpp>
#include "core/handlers/GLFWHandler.hpp"
#include "vma/vk_mem_alloc.h"
#include "core/VulkanHandler.hpp"

namespace Accretion{

using DeletionFn = std::function<void()>;

class Context{
public:
    Context();
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(const Context&&) = delete;
    Context& operator=(const Context&&) = delete;

    void defer(DeletionFn fn);
    void flush_deferred();
private:
    void create_instance();
    void create_debug_messenger();
    
    template <typename T, typename = typename
    std::enable_if<std::is_base_of<VulkanHandler, T>::value>::type
    , typename... Args>
    void add_handler(Args&&... args){
        auto handler = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = handler.get();
        auto [it, inserted] = m_handlers.emplace(handler->get_name(), std::move(handler));
        if(!inserted){
            std::cerr << "WARNING: Duplicate handlers: " << handler->get_name() << '\n';
            return;
        }
        ptr->create();
    }

    std::vector<const char *> get_required_glfw_extensions();
private:
    VmaAllocator m_allocator{nullptr};
    VkInstance m_instance{VK_NULL_HANDLE};
    VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    std::unordered_map<std::string, std::unique_ptr<VulkanHandler>> m_handlers{};

    std::vector<DeletionFn> m_deletion_queue{};

    VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};
    std::vector<const char*> m_validation_layers{"VK_LAYER_KHRONOS_validation"};
    #ifndef NDEBUG
        const bool m_enable_validation_layers{false};
    #else
        const bool m_enable_validation_layers{true};
    #endif
};

}
