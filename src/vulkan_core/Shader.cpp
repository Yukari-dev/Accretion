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
  // std::string correct_path = path.ends_with(".spv") ? path : path + ".spv";
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
  
  count = 0;
  spvReflectEnumerateInputVariables(&module, &count, nullptr);
  std::vector<SpvReflectInterfaceVariable*> inputs(count);
  spvReflectEnumerateInputVariables(&module, &count, inputs.data());

  m_input_attributes.clear();
  for (const auto* input : inputs) {
    // 1. Ignore built-in variables like gl_VertexIndex, gl_InstanceIndex, etc.
    if (input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
      continue;
    }

    VertexInputAttribute attr{};
    attr.name = input->name ? input->name : "";
    attr.location = input->location;
    attr.format = static_cast<VkFormat>(input->format);

    // Calculate byte size based on scalar type and component count
    uint32_t component_count = input->numeric.vector.component_count;
    if (component_count == 0) component_count = 1;

    uint32_t byte_size = (input->numeric.scalar.width / 8) * component_count;
    attr.size = byte_size;

    m_input_attributes.push_back(attr);
  }

  std::sort(
    m_input_attributes.begin(), m_input_attributes.end(),
    [](const VertexInputAttribute& a, const VertexInputAttribute& b) {
      return a.location < b.location;
    }
  );

  spvReflectDestroyShaderModule(&module);
}

VkPipelineVertexInputStateCreateInfo Shader::get_vertex_input_state(
  std::vector<VkVertexInputBindingDescription>& binding_descs,
  std::vector<VkVertexInputAttributeDescription>& attribute_descs,
  uint32_t binding_slot
) const {
  binding_descs.clear();
  attribute_descs.clear();

  if (m_input_attributes.empty()) {
    VkPipelineVertexInputStateCreateInfo empty_info{};
    empty_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    return empty_info;
  }

  uint32_t current_offset = 0;
  for (const auto& attr : m_input_attributes) {
    VkVertexInputAttributeDescription desc{};
    desc.location = attr.location;
    desc.binding = binding_slot;
    desc.format = attr.format;
    desc.offset = current_offset;

    attribute_descs.push_back(desc);
    current_offset += attr.size;
  }

  VkVertexInputBindingDescription binding{};
  binding.binding = binding_slot;
  binding.stride = current_offset;
  binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  binding_descs.push_back(binding);

  VkPipelineVertexInputStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descs.size());
  info.pVertexBindingDescriptions = binding_descs.data();
  info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descs.size());
  info.pVertexAttributeDescriptions = attribute_descs.data();

  return info;
}

VkBufferUsageFlagBits Shader::get_buffer_usage(VkDescriptorType type){
  switch(type){
    case VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    case VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    default: return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  }
}

}
