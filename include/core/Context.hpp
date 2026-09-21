#pragma once
#include <functional>
#include <vulkan/vulkan.hpp>
#include "vma/vk_mem_alloc.h"

namespace Accretion{

using DeletionFn = std::function<void()>;

class Context{
public:
    Context();
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    void defer(DeletionFn fn);
    void flush_deferred();
private:
    void create_instance();

    std::vector<const char *> GetRequiredGLFWExtensions();
private:
    VmaAllocator m_allocator{nullptr};
    VkInstance m_instance{VK_NULL_HANDLE};
    VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};

    std::vector<DeletionFn> m_deletion_queue{};
};

}
