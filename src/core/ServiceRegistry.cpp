#include "core/ServiceRegistry.hpp"
#include "core/services/Camera.hpp"

namespace Accretion{

void ServiceRegistry::initialize(){
  add<Camera>();
}

}
