#version 150 core

in vec2 vf_pos;

out vec4 out_mi, out_hi;

uniform sampler2D fb_mi, fb_hi, ssao, depth_tex;


void main(void)
{
  float ssao_val = pow(min(texture(ssao, vf_pos).r, 0.35) / 0.35, 1.5);
  // somehow get fog into this thing
  ssao_val = mix(ssao_val, 1.0, smoothstep(0.9975, 0.999, texture(depth_tex, vf_pos).r));

  out_mi = vec4(texture(fb_mi, vf_pos).rgb * ssao_val, 1.0);
  out_hi = vec4(texture(fb_hi, vf_pos).rgb * ssao_val, 1.0);
}
