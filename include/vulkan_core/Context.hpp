#pragma once
#include "core/InputSystem.hpp"
#include "core/Time.hpp"
#include "core/Camera.hpp"
#include "vk_mem_alloc.h"
#include "vulkan_core/HandleManager.hpp"
#include "vulkan_core/Renderer.hpp"
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

  void run();

private:
  std::unique_ptr<HandleManager> m_handle_manager{nullptr};
  std::unique_ptr<Renderer> m_renderer{nullptr};
  Time m_time{};
  InputSystem m_input{};
  Camera m_camera{};
};

}
