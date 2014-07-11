#version 150

in vec4 gf_color;

out vec4 out_mi, out_hi;


void main(void)
{
  out_mi = vec4(gf_color.rgb, 0.2 * gf_color.a);
  out_hi = gf_color;
}
