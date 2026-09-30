#pragma once
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"

namespace Accretion{

class VertexBuffer{
public:
  VertexBuffer(VkDevice& device, VmaAllocator& allocator) : m_device(device), m_allocator(allocator) {}
  ~VertexBuffer();

  VertexBuffer(VertexBuffer&&) noexcept;
  VertexBuffer& operator=(VertexBuffer&&) noexcept;
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer& operator=(const VertexBuffer&) = delete;

  void create(const void *data, size_t size);
  void bind(VkCommandBuffer cmd, uint32_t binding = 0);

  VkBuffer get_handle() const { return m_buffer; };
private:
  VkDevice& m_device;
  VmaAllocator& m_allocator;
  VkBuffer m_buffer{};
  VmaAllocation m_allocation{};
  VmaAllocationInfo m_allocation_info{};
};

}
