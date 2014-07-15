#version 150 core

in vec2 vf_pos;

out vec4 out_color;

uniform sampler2D depth_tex, noise_tex;
uniform vec2 epsilon;


void main(void)
{
  float shadow = 1.0;
  float this_depth = texture(depth_tex, vf_pos).r;
  vec2 noise_base = vf_pos / epsilon;

  noise_base.s = mod(noise_base.s, 4.0) / 4.0;
  noise_base.t = mod(noise_base.t, 4.0) / 4.0;

  // looks better than 0..3 (I blame the noise texture)
  for (int y = 1; y < 4; y++) {
    for (int x = 1; x < 4; x++) {
      vec2 compare_tex_vec = 42.0 * (texture(noise_tex, noise_base + vec2(x / 16.0, y / 16.0) + vec2(0.03125, 0.03125)).st - vec2(0.5, 0.5));
      float compare_depth = texture(depth_tex, vf_pos + compare_tex_vec * epsilon).r;
      float compare_ratio = this_depth / compare_depth;

      if (compare_ratio > 1.00001) {
        shadow -= 0.12444 / (1.0 + 300.0 * (compare_ratio - 1.0));
      }
    }
  }

  out_color = vec4(vec3(shadow), 1.0);
}
