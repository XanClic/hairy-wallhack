#version 150 core

in vec3 vf_position;

out vec4 out_mi, out_hi;

uniform vec3 ambient;
uniform float strip_min_y, strip_height;


void main(void)
{
  float alpha = sin(3.141 * (vf_position.y - strip_min_y) / strip_height);

  // Now THIS looks hot
  out_mi = vec4(ambient       , 0.05 * alpha);
  out_hi = vec4(ambient * 10.0, 0.3  * alpha);
}
