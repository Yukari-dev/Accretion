#version 450 

vec2 local_triangle[3] = vec2[](
  vec2( 0.0,-0.5),
  vec2(-0.5, 0.5),
  vec2( 0.5, 0.5)
);

layout(location = 0) in vec3 aPos;

void main(){
  gl_Position = vec4(aPos, 1.0);
}
