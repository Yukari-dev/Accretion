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
    variable.size = binding->block.size;
    variable.offset = binding->block.offset;
    for(size_t i{}; i < binding->block.member_count; ++i){
      ShaderVariable::Member member{};
      member.name = binding->block.members[i].name;
      member.size = binding->block.members[i].size;
      member.offset = binding->block.members[i].offset;
      variable.members.emplace(member.name, member);
    }
    m_variables.emplace(variable.name, variable);
  }

  spvReflectDestroyShaderModule(&module);
}

}
