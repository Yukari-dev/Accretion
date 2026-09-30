#pragma once
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>
#include "vulkan_core/VulkanHandler.hpp"
#include "vulkan_core/configs/PipelineConfig.hpp"
#include <memory>

namespace Accretion{

class PipelineHandler : public VulkanHandler{
public:
  PipelineHandler();

  static PipelineConfig get_default_config();

  Pipeline& create_pipeline(const std::string& name, const PipelineConfig& config);
  Pipeline* get(const std::string& name);
  bool has(const std::string& name) const;
  void remove(const std::string& name);

  VkDescriptorSet& get_descriptor_set() { return m_descriptor_set; }
private:
  void build(const PipelineConfig& config, Pipeline& out_pipeline);
  void create() override;
private:
  std::unordered_map<std::string, std::unique_ptr<Pipeline>> m_pipelines{};
  PipelineConfig m_config{};
  VkDescriptorSet m_descriptor_set{VK_NULL_HANDLE};
};

}
