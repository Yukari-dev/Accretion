#pragma once
#include "vma/vk_mem_alloc.h"
#include "vulkan_core/HandleManager.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Accretion {

class Context {
public:
  Context();
  ~Context();

  Context(const Context &) = delete;
  Context &operator=(const Context &) = delete;
  Context(const Context &&) = delete;
  Context &operator=(const Context &&) = delete;

private:
  std::unique_ptr<HandleManager> m_handle_manager{nullptr};
};

}
