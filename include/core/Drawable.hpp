#pragma once
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "vk_mem_alloc.h"
#include "vulkan_core/VertexBuffer.hpp"

namespace Accretion{

struct Drawable{
  std::shared_ptr<VertexBuffer> vertex_buffer{nullptr};

  uint32_t vertex_count{};
  uint32_t instance_count{1};

  void draw(VkCommandBuffer cmd) const;
};

}
