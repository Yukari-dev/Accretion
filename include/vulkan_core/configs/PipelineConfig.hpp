#pragma once
#include <string>
#include <vulkan/vulkan.hpp>
#include "vulkan_core/Shader.hpp"

namespace Accretion{

struct Pipeline{
  VkPipeline handle{};
  VkPipelineLayout handle_layout{};
  std::optional<Shader> vertex_shader{};
  std::optional<Shader> fragment_shader{};
};

struct ShaderConfig {
  std::string vert_path;
  std::string frag_path;
};

struct PipelineConfig{
  ShaderConfig shader_config{};
};

}
