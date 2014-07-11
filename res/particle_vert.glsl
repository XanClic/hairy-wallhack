#version 150

in vec3 in_position, in_velocity, in_color;
in float in_max_lifetime;

out vec3 vg_velocity;
out vec4 vg_color;

uniform float lifetime;


void main(void)
{
  gl_Position = vec4(in_position, 1.0);
  vg_velocity = in_velocity;
  vg_color    = vec4(in_color, max(0.0, 1.0 - lifetime / in_max_lifetime));
}
