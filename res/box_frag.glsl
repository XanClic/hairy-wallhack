#version 150 core

in vec3 vf_position;
in vec2 vf_texcoord;
in vec3 vf_normal;

out vec4 out_mi, out_hi;

uniform vec3 light_pos, ambient, diffuse_base;
uniform float enlightenment;
uniform sampler2D tex;


void main(void)
{
  vec3 inv_light_dir = light_pos - vf_position;
  float diff_co = 42.0 * max(0.0, dot(normalize(vf_normal), inv_light_dir)) / pow(length(inv_light_dir), 2.0);
  vec3 col = enlightenment * (ambient + diff_co * diffuse_base) * texture(tex, vf_texcoord).rgb;

  out_mi = vec4(col      , 1.0);
  out_hi = vec4(col / 5.0, 1.0);
}
