#version 150

layout(points) in;
layout(line_strip, max_vertices=2) out;

in vec3 vg_velocity[];
in vec4 vg_color[];

out vec4 gf_color;

uniform mat4 proj;


void main(void)
{
  gf_color = vg_color[0];
  gl_Position = proj * gl_in[0].gl_Position;
  EmitVertex();

  gf_color = vg_color[0];
  gl_Position = proj * (gl_in[0].gl_Position + vec4(vg_velocity[0] / 20.0, 0.0));
  EmitVertex();

  EndPrimitive();
}
