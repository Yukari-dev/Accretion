#pragma once
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <string>

namespace Accretion{

struct ShaderVariable{
  std::string name{};
  uint32_t location{};
  uint32_t binding{};
  uint32_t set{};
  VkFormat format{};
  VkDescriptorType type{};
  VkBufferUsageFlagBits usage{};
  
  uint32_t offset{};
  uint32_t size{};

  struct Member{
    std::string name{};
    uint32_t offset{};
    uint32_t size{};
  };
  std::unordered_map<std::string, Member> members{};
};

struct VertexInputAttribute {
  std::string name;
  uint32_t location;
  VkFormat format;
  uint32_t size;
};

class Shader{
public:
  Shader(const std::string& path, VkDevice& device);
  ~Shader();

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  void create(const std::string& path);

  VkShaderModule get_module() const { return m_shader_module; }
  VkShaderStageFlagBits get_stage() const { return m_stage; }
  const std::unordered_map<std::string, ShaderVariable>& get_variables() const { return m_variables; }
  const std::vector<VertexInputAttribute>& get_input_attributes() const { return m_input_attributes; } 
  VkPipelineVertexInputStateCreateInfo get_vertex_input_state(
    std::vector<VkVertexInputBindingDescription>& binding_descs, 
    std::vector<VkVertexInputAttributeDescription>& attribute_descs, 
    uint32_t binding_slot = 0
  ) const;
private:
  void reflect(const std::string& path);

  VkBufferUsageFlagBits get_buffer_usage(VkDescriptorType type);
private:
  VkDevice& m_device;
  VkShaderModule m_shader_module{};
  VkShaderStageFlagBits m_stage{VK_SHADER_STAGE_VERTEX_BIT};

  std::unordered_map<std::string, ShaderVariable> m_variables{};
  std::vector<VertexInputAttribute> m_input_attributes;
};

}
