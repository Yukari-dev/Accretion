#version 450 

layout(set = 0, binding = 0) uniform Camera{
  mat4 u_view_proj;
  vec3 u_pos;
} camera;

layout(set = 0, binding = 1) uniform Data{
  vec3 u_color;
} data;

layout(std430, set = 0, binding = 2) buffer Particles{
  vec3 positions[];
} particles;

vec2 local_triangle[3] = vec2[](
  vec2( 0.0,-0.5),
  vec2(-0.5, 0.5),
  vec2( 0.5, 0.5)
);

layout(location = 0) out vec3 color;

void main(){
  vec3 particle_center = particles.positions[gl_InstanceIndex];
  vec3 world_pos = particle_center + vec3(local_triangle[gl_VertexIndex], 0.0);
  gl_Position = camera.u_view_proj * vec4(world_pos, 1.0);
  color = data.u_color;
}
