#version 150 core

in vec2 vf_pos;

out vec4 out_color;

uniform sampler2D fb_mi, fb_hi;


void main(void)
{
  vec4 bloom = clamp(texture(fb_hi, vf_pos) * 5.0 - vec4(0.1, 0.1, 0.1, 0.0), 0.0, 1.0);
  out_color = texture(fb_mi, vf_pos) + bloom;
}
