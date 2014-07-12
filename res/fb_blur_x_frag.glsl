#version 150 core

in vec2 vf_pos;

out vec4 out_color;

uniform sampler2D input_tex;
uniform float epsilon;


vec4 access(float diff)
{
  return texture(input_tex, vec2(clamp(vf_pos.x + epsilon * diff, epsilon, 1.0 - epsilon), vf_pos.y));
}

void main(void)
{
  out_color = access( 0.0) * 0.4
            + access( 1.0) * 0.2
            + access(-1.0) * 0.2
            + access( 2.0) * 0.1
            + access(-2.0) * 0.1;
}
