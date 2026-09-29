#pragma once
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <functional>

namespace Accretion{

using DeletionFn = std::function<void()>;

class VulkanHandler{
public:
  VulkanHandler(const std::string& name, DeletionFn deletionFn);

  virtual void create() {}

  void add_dependency(const std::string& name, VulkanHandler* handle);
  void request_dependency(const std::string& name);
  void clear_requests();

  template <typename T>
  T& get_dependency(){
    T& dep = find_dependency_by_class<T>();
    return dep;
  }

  template <typename T>
  T& find_dependency_by_class(){
    auto it = std::find_if(
      m_dependecies.begin(), m_dependecies.end(),
      [&](const auto& pair){
        return dynamic_cast<T*>(pair.second) != nullptr;
      }
    );
    if(it == m_dependecies.end())
      throw std::runtime_error("ERROR: Dependency not found using search by class: " + std::string(typeid(T).name()));
    T* casted = dynamic_cast<T*>(it->second);
    return *casted;
  }

  template <typename T>
  T& find_dependency_by_name(const std::string& name){
    auto it = std::find_if(
      m_dependecies.begin(), m_dependecies.end(),
      [&](const auto& pair){
        return pair.first  == name;
      }
    );
    if(it == m_dependecies.end())
      throw std::runtime_error("ERROR: Dependency not found using search by name: " + name);
    T* casted = dynamic_cast<T*>(it->second);
    if (!casted) {
      throw std::runtime_error("ERROR: Dependency '" + name + "' is not of requested type");
    }

    return *casted;
  }

  template <typename T>
  T& find_requested_dependency_by_name(const std::string& name){
    auto it = std::find_if(
      m_requested_dependencies.begin(), m_dependecies.end(),
      [&](const auto& other){
        return other == name;
      }
    );
    if(it == m_dependecies.end())
      throw std::runtime_error("ERROR: Dependency not found: " + name);
    return it->second;
  }

  const std::string& get_name() const { return m_name; }
  DeletionFn& get_deletion_fn() { return m_deletionFn; }
  const std::vector<std::string>& get_requests() const { return m_requested_dependencies; }
  const std::vector<std::pair<std::string, VulkanHandler*>>& get_dependencies() const { return m_dependecies; };
protected:
  std::string m_name{};
  std::vector<std::string> m_requested_dependencies;
  std::vector<std::pair<std::string, VulkanHandler*>> m_dependecies{};
  DeletionFn m_deletionFn{};
};

}
