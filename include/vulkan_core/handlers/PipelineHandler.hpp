#pragma once
#include "vk_mem_alloc.h"
#include <optional>
#include <vulkan/vulkan.hpp>
#include "vulkan_core/ShaderInstance.hpp"
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion{

class PipelineHandler : public VulkanHandler{
public:
  PipelineHandler();

  void create();

  VkPipeline& get_pipeline() { return m_pipeline; }
  VkPipelineLayout& get_pipeline_layout() { return m_pipeline_layout; }
  VkDescriptorSet& get_descriptor_set() { return m_descriptor_set; }

  ShaderInstance& get_shader_instance() { return m_shader_instance.value(); }
private:
  VkPipeline m_pipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};
  VkDescriptorSet m_descriptor_set{VK_NULL_HANDLE};

  std::optional<ShaderInstance> m_shader_instance;

  VkBuffer m_ubo_buffer{};
  VmaAllocation m_ubo_alloc{};
};

}
