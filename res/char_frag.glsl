#version 150 core

in vec2 vf_pos;

out vec4 out_mi, out_hi;

uniform sampler2D font;
uniform vec3 color;


void main(void)
{
  if (texture(font, vf_pos).r > 0.5) {
    discard;
  } else {
    out_mi = vec4(color, 1.0);
    out_hi = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
