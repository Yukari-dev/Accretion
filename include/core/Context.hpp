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
    template <typename T, typename = typename std::enable_if<std::is_base_of<VulkanHandler, T>::value>::type
    , typename... Args>
    void add_handler(Args&&... args){
        auto handler = std::make_unique<T>(std::forward<Args>(args)...);
        std::string name = handler->get_name();
        for(const auto& [extisting_name, ptr] : m_handlers){
            if(extisting_name == name){
                std::cerr << "WARNING: Duplicate handlers: " << ptr->get_name() << '\n';
                return;
            }
        }
        T* ptr = handler.get();
        m_handlers.emplace_back(name, std::move(handler));
        m_deletion_queue.emplace_back(ptr->get_deletion_fn());
        ptr->create();
    }

private:
    VmaAllocator m_allocator{nullptr};
    VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    std::vector<std::pair<std::string, std::unique_ptr<VulkanHandler>>> m_handlers{};

    std::vector<DeletionFn> m_deletion_queue{};
};

}
