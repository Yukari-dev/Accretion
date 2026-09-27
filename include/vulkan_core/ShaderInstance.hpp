#pragma once
#include "vk_mem_alloc.h"
#include "vulkan_core/Shader.hpp"
#include <glm/glm.hpp>

namespace Accretion{

struct UniformLocation{
  uint32_t offset{};
  bool valid{};
};

class ShaderInstance{
public:
  ShaderInstance(Shader& shader, VkDevice& device, VmaAllocator& allocator, VkDescriptorSetLayout& layout);
  ~ShaderInstance();

  void bind(VkCommandBuffer cmd, VkPipelineLayout layout);

  UniformLocation get_location(const std::string& name);

  void set(UniformLocation loc, float v);
  void set(UniformLocation loc, glm::vec2 v);
  void set(UniformLocation loc, glm::vec3 v);
  void set(UniformLocation loc, glm::mat4 v);

  ShaderInstance(ShaderInstance&&) noexcept;
  ShaderInstance& operator=(ShaderInstance&&) noexcept;
  ShaderInstance(const ShaderInstance&) = delete;
  ShaderInstance& operator=(const ShaderInstance&) = delete;
private:
  void cache_shader_variables();
  void create_buffer();

  template<typename T>
  void set(UniformLocation loc, const T& v);
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

  std::unordered_map<std::string, ShaderVariable> m_variables{};
  std::unordered_map<std::string, ShaderVariable::Member> m_flat_member_lookup;
};

}
