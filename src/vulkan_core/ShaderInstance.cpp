#include "vulkan_core/ShaderInstance.hpp"
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace Accretion{

ShaderInstance::ShaderInstance(Shader& shader, VkDevice& device, VmaAllocator& allocator, VkDescriptorSetLayout& layout) 
: m_shader(shader), m_device(device), m_allocator(allocator), m_descriptor_layout(layout){
  create_buffer();
  cache_shader_variables();
}

ShaderInstance::~ShaderInstance(){
  if(m_buffer)
    vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}

void ShaderInstance::create_buffer(){
  m_variables = m_shader.get_variables();

  const ShaderVariable* ubo{nullptr};
  for(auto& pair : m_variables){
    ubo = &pair.second;
    break;
  }
  if (!ubo)
    throw std::runtime_error("ShaderInstance: no UBO in shader");

  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = ubo->size;
  info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

  if(vmaCreateBuffer(m_allocator, &info, &alloc_info, &m_buffer, &m_allocation, &m_allocation_info) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create buffer using VMA.");

  VkDescriptorPoolSize pool_size{};
  pool_size.descriptorCount = 1;
  pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = &pool_size;
  pool_info.maxSets = 1;

  if(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create Descriptor Pool.");

  VkDescriptorSetAllocateInfo set_info{};
  set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  set_info.descriptorPool = m_descriptor_pool;
  set_info.descriptorSetCount = 1;
  set_info.pSetLayouts = &m_descriptor_layout;

  if(vkAllocateDescriptorSets(m_device, &set_info, &m_descriptor_set) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create Descriptor Set.");

  VkDescriptorBufferInfo descriptor_buffer{};
  descriptor_buffer.buffer = m_buffer;
  descriptor_buffer.offset = 0;
  descriptor_buffer.range = ubo->size;

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = m_descriptor_set;
  write.dstBinding = ubo->binding;
  write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write.pBufferInfo = &descriptor_buffer;

  vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}

void ShaderInstance::bind(VkCommandBuffer cmd, VkPipelineLayout layout){
  vkCmdBindDescriptorSets(
    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, 
    layout,
    0, 1, &m_descriptor_set, 0, nullptr
  );
}

void ShaderInstance::cache_shader_variables(){
  for(const auto& [ubo_name, ubo_var] : m_variables){
    for(const auto& [member_name, member] : ubo_var.members){
      m_flat_member_lookup[ubo_name + "." + member_name] = member;
      m_flat_member_lookup[member_name] = member;
    }
  }
}

UniformLocation ShaderInstance::get_location(const std::string& name){
  auto member_it = m_flat_member_lookup.find(name);
  if(member_it != m_flat_member_lookup.end())
    return {member_it->second.offset, true};
  return {0, false};
}

void ShaderInstance::set(UniformLocation loc, float v){
  set<float>(loc, v);
}

void ShaderInstance::set(UniformLocation loc, glm::vec2 v){
  set<glm::vec2>(loc, v);
}

void ShaderInstance::set(UniformLocation loc, glm::vec3 v){
  set<glm::vec3>(loc, v);
}

void ShaderInstance::set(UniformLocation loc, glm::mat4 v){
  set<glm::mat4>(loc, v);
}

template <typename T>
void ShaderInstance::set(UniformLocation loc, const T& v){
  if(!loc.valid) return;
  std::memcpy(static_cast<char*>(m_allocation_info.pMappedData) + loc.offset, &v, sizeof(T));
}

}
