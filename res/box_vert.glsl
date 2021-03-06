#version 150 core

in vec3 in_position;
in vec2 in_tex_coord;
in vec3 in_normal;

out vec2 vf_texcoord;
out vec3 vf_normal;
out vec3 vf_position;

uniform mat4 mv, proj;
uniform mat3 norm_mat;


void main(void)
{
  vec4 global_pos = mv * vec4(in_position, 1.0);
  vf_position = global_pos.xyz;
  gl_Position = proj * global_pos;

  vf_texcoord = in_tex_coord;
  vf_normal = normalize(norm_mat * in_normal);
}
