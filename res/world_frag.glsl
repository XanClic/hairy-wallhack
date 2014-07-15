#version 150 core

in vec3 vf_position;
in vec3 vf_normal;

out vec4 out_mi, out_hi;

uniform vec3 light_pos;
uniform float time_step;


vec3 get_color(void)
{
  return max(mix(vec3(0.0, 0.3, 0.6), vec3(1.0, 0.0, 0.2), (1.0 + sin(vf_position.y / 40.0)) / 2.0),
             vec3(0.0,
                  max(0.0, sin((vf_position.z - time_step) / 10.0)),
                  max(0.0, cos((vf_position.z - time_step) / 8.0))));
}


void main(void)
{
  vec3 inv_light_dir = light_pos - vf_position;
  float diff_co = 8.0 * max(0.0, dot(normalize(vf_normal), inv_light_dir)) / pow(length(inv_light_dir), 1.6);
  vec3 real_col = get_color() * diff_co;

  out_mi = vec4(mix(real_col, vec3(0.8, 0.8, 0.8), smoothstep(0.996, 0.999, gl_FragCoord.z))      , 1.0);
  out_hi = vec4(mix(real_col, vec3(0.0, 0.0, 0.0), smoothstep(0.996, 0.999, gl_FragCoord.z)) / 2.5, 1.0);
}
