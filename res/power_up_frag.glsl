#version 150 core

in vec3 vf_position;
in vec3 vf_normal;

out vec4 out_mi, out_hi;

uniform vec3 light_pos, cam_pos;
uniform vec3 diffuse, specular;


void main(void)
{
  vec3 inv_light = light_pos - vf_position, inv_light_dir = normalize(inv_light);
  vec3 normal = normalize(vf_normal);
  vec3 to_viewer = normalize(cam_pos - vf_position);

  float light_strength = 42.0 / length(inv_light);

  float diff_co = max(0.0, dot(normal, inv_light_dir));
  float spec_co = pow(max(0.0, dot(reflect(normalize(inv_light), normal), to_viewer)), 20.0) ;

  vec3 col = light_strength * (diff_co * diffuse + spec_co * specular);

  // make them appear faster than normal
  out_mi = vec4(mix(col, vec3(0.7, 0.7, 0.7), smoothstep(0.995, 0.999, gl_FragCoord.z))      , 1.0);
  out_hi = vec4(mix(col, vec3(0.0, 0.0, 0.0), smoothstep(0.995, 0.999, gl_FragCoord.z)) / 5.0, 1.0);
}
