#include "core/VulkanHandler.hpp"
#include <iostream>

namespace Accretion{

VulkanHandler::VulkanHandler(const std::string& name, DeletionFn deletionFn) : 
  m_name(name), m_deletionFn(deletionFn) {}

void VulkanHandler::add_dependency(const std::string& name, VulkanHandler handle){
  auto [_, inserted] = m_dependecies.emplace(name, handle);
  if(!inserted)
    std::cerr << "WARNING: Duplicate dependency: " << name << '\n';
}

}

