#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"

namespace Accretion{

class PipelineHandler : public VulkanHandler{
public:
  PipelineHandler();

  void create();

  VkPipeline& get_pipeline() { return m_pipeline; }
  VkPipelineLayout& get_pipeline_layout() { return m_pipeline_layout; }
private:
  VkPipeline m_pipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};
};

}
