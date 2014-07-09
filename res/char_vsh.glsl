#version 150 core

in vec2 in_pos;

out vec2 vf_pos;

uniform vec2 position, size, char_position;


void main(void)
{
  gl_Position = vec4(in_pos * size + position, 0.0, 1.0);
  vf_pos = (vec2(in_pos.x, -in_pos.y) + vec2(1.0, 1.0) + 2.0 * char_position) / 32.0;
}
