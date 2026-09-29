#pragma once
#include <glm/glm.hpp>

namespace Accretion{

struct Particle{
  alignas(16) glm::vec3 position{};
};

}
