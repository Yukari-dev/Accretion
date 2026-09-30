#include "vulkan_core/VertexBuffer.hpp"
#include <stdexcept>

namespace Accretion {

VertexBuffer::~VertexBuffer(){
  if(m_buffer)
    vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}

void VertexBuffer::create(const void *data, size_t size){
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = size;
  info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

  if(vmaCreateBuffer(m_allocator, &info, &alloc_info, &m_buffer, &m_allocation, &m_allocation_info))
    throw std::runtime_error("ERROR: Failed to create a Vertex Buffer using VMA.");

  if (data && size > 0)
    std::memcpy(m_allocation_info.pMappedData, data, size);
}

void VertexBuffer::bind(VkCommandBuffer cmd, uint32_t binding){
  VkDeviceSize offsets[]{0};
  vkCmdBindVertexBuffers(cmd, binding, 1, &m_buffer, offsets);
}

}

