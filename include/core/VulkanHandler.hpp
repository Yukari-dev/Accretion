#pragma once
#include <string>
#include <unordered_map>
#include <functional>

namespace Accretion{

using DeletionFn = std::function<void()>;

class VulkanHandler{
public:
  VulkanHandler(const std::string& name, DeletionFn deletionFn);

  virtual void create() {}

  void add_dependency(const std::string& name, VulkanHandler handle);

  const std::string& get_name() const { return m_name; }
protected:
  std::string m_name{};
  std::unordered_map<std::string, VulkanHandler> m_dependecies{};
  DeletionFn m_deletionFn{};
};

}
