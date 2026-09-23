#include "vulkan_core/VulkanHandler.hpp"
#include <iostream>

namespace Accretion{

VulkanHandler::VulkanHandler(const std::string& name, DeletionFn deletionFn) : 
  m_name(name), m_deletionFn(deletionFn) {}

void VulkanHandler::add_dependency(const std::string& name, VulkanHandler* handle){
  if (!handle) {
    throw std::invalid_argument("ERROR: Cannot add null dependency for: " + name);
  }
  for(const auto& [existing_name, handle] : m_dependecies){
    if(existing_name == name){
      std::cerr << "WARNING: Duplicate dependency: " << name << '\n';
      return;
    }
  }
  m_dependecies.push_back({name, handle});
}

void VulkanHandler::request_dependency(const std::string& name){
  for(const auto& [existing_name, handle] : m_dependecies){
    if(existing_name == name){
      std::cerr << "WARNING: Duplicate request dependency: " << name << '\n';
      return;
    }
  }
  m_requested_dependencies.emplace_back(name);
}

void VulkanHandler::clear_requests(){
  m_requested_dependencies.clear();
}

}
