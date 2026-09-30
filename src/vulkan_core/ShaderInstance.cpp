#include "vulkan_core/ShaderInstance.hpp"
#include <cstring>
#include <stdexcept>

namespace Accretion{

ShaderInstance::ShaderInstance(Shader& shader, VkDevice& device, VmaAllocator& allocator, VkDescriptorSetLayout& layout) 
: m_shader(shader), m_device(device), m_allocator(allocator), m_descriptor_layout(layout){
  create_buffer();
  cache_shader_variables();
}

ShaderInstance::~ShaderInstance(){
  for(auto& data : m_buffers)
    vmaDestroyBuffer(m_allocator, data.second.buffer, data.second.alloc);
  if(m_descriptor_pool)
    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
}

void ShaderInstance::create_buffer(){
  m_variables = m_shader.get_variables();

  for(auto& [block_name, var] : m_variables){
    if (var.type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER && var.type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) continue;

    for(auto& [member_name, member] : var.members){
      m_member_look_up[member_name] = MemberRef{var.binding, member.offset, member.size};
      m_member_look_up[block_name + "." + member_name] = m_member_look_up[member_name];
    }
  }

  for(const auto& [name, var] : m_variables){
    if(var.size == 0) continue;
    if(var.type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER && var.type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) continue;

    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = var.size;
    info.usage = var.usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;

    VkBuffer buffer{};
    VmaAllocation alloc{};
    VmaAllocationInfo allocation_info{};
    VkResult res = vmaCreateBuffer(m_allocator, &info, &alloc_create_info, &buffer, &alloc, &allocation_info);
    if(res != VK_SUCCESS)
      throw std::runtime_error("ERROR: Failed to create buffer using VMA.");

    BufferData data{};
    data.buffer = buffer;
    data.alloc = alloc;
    data.alloc_info = allocation_info;
    data.binding = var.binding;
    data.offset = var.offset;
    data.size = var.size;
    data.descriptor_type = var.type;
    m_buffers.emplace(var.binding, data);
  }
}

void ShaderInstance::update(){
  if(m_dirty){
    std::unordered_map<VkDescriptorType, uint32_t> counts;
    for(auto& [binding, data] : m_buffers){
      counts[data.descriptor_type]++;
    }

    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(counts.size());
    for (const auto& [type, count] : counts) {
      VkDescriptorPoolSize pool_size{};
      pool_size.type = type;
      pool_size.descriptorCount = count;
      pool_sizes.push_back(pool_size);
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
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

    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorBufferInfo> buffer_infos;
    buffer_infos.reserve(m_buffers.size());
    for(const auto& [binding, buffer_data] : m_buffers){
      VkDescriptorBufferInfo descriptor_buffer{};
      descriptor_buffer.buffer = buffer_data.buffer;
      descriptor_buffer.offset = 0;
      descriptor_buffer.range = buffer_data.size;
      buffer_infos.push_back(descriptor_buffer);

      VkWriteDescriptorSet write{};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = m_descriptor_set;
      write.dstBinding = binding;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = buffer_data.descriptor_type;;
      write.pBufferInfo = &buffer_infos.back();
      writes.push_back(write);
    }
    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    m_dirty = false;
  }
}

void ShaderInstance::bind(VkCommandBuffer cmd, VkPipelineLayout layout){
  update();
  vkCmdBindDescriptorSets(
    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, 
    layout,
    0, 1, &m_descriptor_set, 0, nullptr
  );
}

void ShaderInstance::bind_storage_buffer(const std::string& name, const void* data, uint32_t size) {
  auto it = m_variables.find(name);
  if (it == m_variables.end()) {
    throw std::runtime_error("ERROR: Failed to find SSBO variable: " + name);
  }

  uint32_t binding = it->second.binding;
  auto buf_it = m_buffers.find(binding);

  if (buf_it != m_buffers.end() && buf_it->second.capacity >= size) {
    auto& buf = buf_it->second;
    buf.size = size;

    if (data && buf.alloc_info.pMappedData) {
      std::memcpy(buf.alloc_info.pMappedData, data, size);
    }
    return;
  }

  if (buf_it != m_buffers.end()) {
    vmaDestroyBuffer(m_allocator, buf_it->second.buffer, buf_it->second.alloc);
    m_buffers.erase(buf_it);
  }

  uint32_t new_capacity = size;

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = new_capacity;
  buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo alloc_create_info{};
  alloc_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT 
    | VMA_ALLOCATION_CREATE_MAPPED_BIT;
  alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;

  VkBuffer buffer{};
  VmaAllocation alloc{};
  VmaAllocationInfo allocation_info{};
  VkResult res = vmaCreateBuffer(m_allocator, &buffer_info, &alloc_create_info, &buffer, &alloc, &allocation_info);
  if (res != VK_SUCCESS) {
    throw std::runtime_error("ERROR: Failed to create SSBO buffer using VMA.");
  }

  if (data && allocation_info.pMappedData) {
    std::memcpy(allocation_info.pMappedData, data, size);
  }

  BufferData buf_data{};
  buf_data.buffer = buffer;
  buf_data.alloc = alloc;
  buf_data.alloc_info = allocation_info;
  buf_data.binding = binding;
  buf_data.offset = it->second.offset;
  buf_data.size = size;
  buf_data.capacity = new_capacity;
  buf_data.descriptor_type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

  m_buffers[binding] = buf_data;

  m_dirty = true;
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
  auto it = m_member_look_up.find(name);
  if(it != m_member_look_up.end()){
    return {name, it->second.buffer_binding, it->second.offset, true};
  }
  return {"", 0, 0, false};
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
  auto& buf = m_buffers.at(loc.binding);
  std::memcpy(static_cast<char*>(buf.alloc_info.pMappedData) + loc.offset, &v, sizeof(T));
}

}
