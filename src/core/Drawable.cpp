#include "core/Drawable.hpp"

namespace Accretion{

void Drawable::draw(VkCommandBuffer cmd) const{
  vertex_buffer->bind(cmd);

  vkCmdDraw(cmd, 3, instance_count, 0, 0);
}

}
