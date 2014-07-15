#version 150 core

in vec2 vf_pos;

out vec4 out_color;

uniform sampler2D depth_tex, noise_tex;
uniform vec2 epsilon;
uniform mat4 proj, inv_proj;


void main(void)
{
  float shadow = 1.0;
  float this_depth = texture(depth_tex, vf_pos).r;
  vec2 noise_base = vf_pos / epsilon;

  noise_base.s = mod(noise_base.s, 4.0) / 4.0;
  noise_base.t = mod(noise_base.t, 4.0) / 4.0;

  // lol what the fuck this actually works
  vec4 gp = (inv_proj * vec4(2.0 * vf_pos - vec2(1.0), this_depth, 1.0));
  vec3 global_pos = gp.xyz / gp.w;

  float avg_spl_d = 0.0;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      vec3 sample_rel_vector = 20.0 * (texture(noise_tex, noise_base + vec2(x / 16.0, y / 16.0) + vec2(0.03125, 0.03125)).xyz - vec3(0.5));
      vec4 projected_sample = proj * vec4(global_pos + sample_rel_vector, 1.0);
      float sample_depth = projected_sample.z / projected_sample.w;
      float actual_depth = texture(depth_tex, 0.5 * projected_sample.xy / projected_sample.w + vec2(0.5)).r;

      float depth_ratio = sample_depth / actual_depth;

      if (depth_ratio > 1.0) {
        shadow -= 0.6 / (length(sample_rel_vector) + pow(1000.0 * (depth_ratio - 1.0), 5.0));
      }
    }
  }

  out_color = vec4(vec3(shadow), 1.0);
}
