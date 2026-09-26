#pragma once
#include "vk_mem_alloc.h"
#include "vulkan_core/Shader.hpp"

namespace Accretion{

class ShaderInstance{
public:
  ShaderInstance(Shader& shader, VkDevice& device, VmaAllocator& allocator, VkDescriptorSetLayout& layout);
  ~ShaderInstance();

  void bind(VkCommandBuffer cmd, VkPipelineLayout layout);

  void set(const std::string& name, float v);

  ShaderInstance(ShaderInstance&&) noexcept;
  ShaderInstance& operator=(ShaderInstance&&) noexcept;
  ShaderInstance(const ShaderInstance&) = delete;
  ShaderInstance& operator=(const ShaderInstance&) = delete;
private:
  void create_buffer();
private:
  Shader& m_shader;
  VkDevice& m_device;

  VkBuffer m_buffer{};

  VkDescriptorSetLayout m_descriptor_layout{};
  VkDescriptorPool m_descriptor_pool{};
  VkDescriptorSet m_descriptor_set{};

  VmaAllocator& m_allocator;
  VmaAllocation m_allocation{};
  VmaAllocationInfo m_allocation_info{};

  std::vector<ShaderVariable> m_variables{};
};

}
