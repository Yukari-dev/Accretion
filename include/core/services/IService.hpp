#pragma once
#include <string>

namespace Accretion{

class IService{
public:
  virtual ~IService() = default;

  virtual void initialize() {}
  virtual void shutdown() {}

  virtual const std::string& get_name() const { return m_name;}
private:
  std::string m_name{};
};

}
