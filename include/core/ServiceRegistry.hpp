#pragma once
#include "core/services/IService.hpp"
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace Accretion{

class ServiceRegistry{
public:
  ~ServiceRegistry() = default;

  void initialize();

  template <typename T, typename = typename std::enable_if_t<std::is_base_of_v<IService, T>>,
  typename... Args>
  void add(Args&&... args){
    m_services[std::type_index(typeid(T))] = std::make_unique<T>(std::forward<Args>(args)...);
  }

  template <typename T> 
  T& get(){
    auto it = m_services.find(std::type_index(typeid(T)));
    if(it == m_services.end())
      throw std::runtime_error("ERROR: Service Not found.");
    return *static_cast<T*>(it->second.get());
  }
private:
  std::unordered_map<std::type_index, std::unique_ptr<IService>> m_services{};
};

}
