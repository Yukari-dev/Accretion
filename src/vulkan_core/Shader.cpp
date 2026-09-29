#include "vulkan_core/Shader.hpp"
#include "spirv_reflect.h"
#include "vulkan_core/utils/VulkanUtils.hpp"
#include <stdexcept>

namespace Accretion{

Shader::Shader(const std::string& path, VkDevice& device) : m_device(device){
  create(path);
  reflect(path);
}

Shader::Shader(Shader&& other) noexcept 
  : m_device(other.m_device),
  m_shader_module(other.m_shader_module),
  m_stage(other.m_stage)
{
  other.m_shader_module = VK_NULL_HANDLE;  
}

Shader& Shader::operator=(Shader&& other) noexcept{
  if(this != &other){
    if(m_shader_module != VK_NULL_HANDLE)
      vkDestroyShaderModule(m_device, m_shader_module, nullptr);
    m_device = other.m_device;
    m_shader_module = other.m_shader_module;
    other.m_shader_module = VK_NULL_HANDLE;
  }
  return *this;
}

Shader::~Shader(){
  if(m_shader_module)
    vkDestroyShaderModule(m_device, m_shader_module, nullptr);
}

void Shader::create(const std::string& path){
  auto source = ReadFileUint32(path);
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = source.size() * sizeof(uint32_t);
  info.pCode = source.data();

  if(vkCreateShaderModule(m_device, &info, nullptr, &m_shader_module) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create Shader Module.");
}

void Shader::reflect(const std::string& path){
  auto source = ReadFileUint32(path);
  SpvReflectShaderModule module{};
  SpvReflectResult result = spvReflectCreateShaderModule(source.size() * sizeof(uint32_t), source.data(), &module);
  if(result != SPV_REFLECT_RESULT_SUCCESS)
    throw std::runtime_error("ERROR: Failed to SPRIV-REFLECT on the shader.");

  uint32_t count{};
  spvReflectEnumerateDescriptorBindings(&module, &count, nullptr);
  std::vector<SpvReflectDescriptorBinding*> bindings(count);
  spvReflectEnumerateDescriptorBindings(&module, &count, bindings.data());
  
  for(const auto* binding : bindings){
    ShaderVariable variable{};
    variable.name = binding->name;
    variable.binding = binding->binding;
    variable.set = binding->set;
    variable.offset = binding->block.offset;
    variable.type = static_cast<VkDescriptorType>(binding->descriptor_type);
    variable.usage = get_buffer_usage(variable.type);

    uint32_t total_size{};
    for(size_t i{}; i < binding->block.member_count; ++i){
      const auto& spv_member = binding->block.members[i];
      ShaderVariable::Member member{};
      member.name = binding->block.members[i].name;
      member.size = (spv_member.padded_size > 0) ? spv_member.padded_size : spv_member.size;
      member.offset = binding->block.members[i].offset;
      variable.members.emplace(member.name, member);

      uint32_t member_end = member.offset + member.size;
      if(member_end > total_size)
        total_size = member_end;
    }
  
    if(binding->block.padded_size > 0) variable.size = binding->block.padded_size;
    else if (binding->block.size > 0) variable.size = binding->block.size;
    else variable.size = total_size;

    m_variables.emplace(variable.name, variable);
  }

  spvReflectDestroyShaderModule(&module);
}

VkBufferUsageFlagBits Shader::get_buffer_usage(VkDescriptorType type){
  switch(type){
    case VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    case VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    default: return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  }
}

}
