#pragma once
#include "vk_mem_alloc.h"
#include "vulkan_core/Shader.hpp"
#include <glm/glm.hpp>

namespace Accretion{

struct UniformLocation{
  std::string name;
  uint32_t binding{};
  uint32_t offset{};
  bool valid{};
};

struct MemberRef{
  uint32_t buffer_binding{};
  uint32_t offset{};
  uint32_t size{};
};

struct BufferData{
  VkBuffer buffer{};
  VmaAllocation alloc{};
  VmaAllocationInfo alloc_info{};
  VkDescriptorType descriptor_type{};
  uint32_t binding{};
  uint32_t offset{};
  uint32_t size{};
  uint32_t capacity{};
};

class ShaderInstance{
public:
  ShaderInstance(Shader& shader, VkDevice& device, VmaAllocator& allocator, VkDescriptorSetLayout& layout);
  ~ShaderInstance();

  void bind_storage_buffer(const std::string& name, const void* data, uint32_t size);
  void bind(VkCommandBuffer cmd, VkPipelineLayout layout);

  UniformLocation get_location(const std::string& name);

  const std::unordered_map<std::string, ShaderVariable> get_variables() const { return m_variables; }

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
  void update();

  template<typename T>
  void set(UniformLocation loc, const T& v);
private:
  Shader& m_shader;
  VkDevice& m_device;

  bool m_dirty{true};

  std::unordered_map<uint32_t, BufferData> m_buffers{};
  std::unordered_map<std::string, MemberRef> m_member_look_up{};

  VkDescriptorSetLayout m_descriptor_layout{};
  VkDescriptorPool m_descriptor_pool{};
  VkDescriptorSet m_descriptor_set{};

  VmaAllocator& m_allocator;

  std::unordered_map<std::string, ShaderVariable> m_variables{};
  std::unordered_map<std::string, ShaderVariable::Member> m_flat_member_lookup;
};

}
