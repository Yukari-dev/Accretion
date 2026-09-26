#version 450 

layout(set = 0, binding = 0) uniform Camera{
  float u_offset_x;
} camera; 

vec2 positions[3] = vec2[](
  vec2( 0.0,-0.5),
  vec2( 0.5, 0.5),
  vec2(-0.5, 0.5)
);

void main(){
  vec2 offset = vec2(camera.u_offset_x, 0);
  gl_Position = vec4(positions[gl_VertexIndex] + offset, 0.0, 1.0);
}
