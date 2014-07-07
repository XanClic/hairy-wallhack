#version 150 core

in vec2 vf_texcoord;
in vec3 vf_normal;

out vec4 out_mi, out_hi;

uniform vec3 light_dir, ambient;
uniform float enlightenment;
uniform sampler2D tex;


void main(void)
{
  float diff_co = dot(normalize(vf_normal), normalize(-light_dir));
  vec3 col = enlightenment * (ambient + diff_co * texture(tex, vf_texcoord).rgb);

  out_mi = vec4(col, 1.0);
  out_hi = vec4(col / 10.0, 1.0);
}
