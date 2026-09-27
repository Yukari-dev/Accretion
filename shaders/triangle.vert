#version 450 

layout(set = 0, binding = 0) uniform Camera{
  mat4 u_view_proj;
  vec3 u_pos;
} camera;

vec2 positions[3] = vec2[](
  vec2( 0.0,-0.5),
  vec2( 0.5, 0.5),
  vec2(-0.5, 0.5)
);

void main(){
  gl_Position = camera.u_view_proj * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
