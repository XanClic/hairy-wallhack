#version 150 core

in vec3 in_position;

out vec3 vf_position;

uniform mat4 proj;


void main(void)
{
  vf_position = in_position;
  gl_Position = proj * vec4(in_position, 1.0);
}
