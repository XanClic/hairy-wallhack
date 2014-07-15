#version 150 core

in vec2 vf_pos;

out vec4 out_color;

uniform sampler2D input_tex, depth_tex;
uniform float epsilon;


vec4 access(float diff)
{
  return texture(input_tex, vec2(clamp(vf_pos.x + epsilon * diff, epsilon, 1.0 - epsilon), vf_pos.y));
}

float factor(float diff, float this_depth)
{
  float ratio = this_depth / texture(depth_tex, vec2(clamp(vf_pos.x + epsilon * diff, epsilon, 1.0 - epsilon), vf_pos.y)).r;

  if (ratio > 1.0) {
    ratio = 1.0 / ratio;
  }

  return max(0.0, 1.0 - 100.0 * (ratio - 1.0));
}

void main(void)
{
  float this_depth = texture(depth_tex, vf_pos).r;

  float m_l2 = 1.0 * factor(-2.0, this_depth);
  float m_l1 = 2.0 * factor(-1.0, this_depth);
  float m_r1 = 2.0 * factor( 1.0, this_depth);
  float m_r2 = 1.0 * factor( 2.0, this_depth);

  float adjust = 0.6 / (m_l2 + m_l1 + m_r1 + m_r2);

  out_color =  access( 0.0) * 0.4
            + (access(-2.0) * m_l2
            +  access(-1.0) * m_l1
            +  access( 1.0) * m_r1
            +  access( 2.0) * m_r2) * adjust;
}
