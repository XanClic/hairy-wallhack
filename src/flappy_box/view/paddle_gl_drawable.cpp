#include <dake/gl/elements_array.hpp>
#include <dake/gl/gl.hpp>
#include <dake/gl/obj.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>
#include <dake/helper/function.hpp>
#include <dake/math/matrix.hpp>

#include "flappy_box/model/paddle.hpp"
#include "flappy_box/view/paddle_gl_drawable.hpp"


using namespace dake;
using namespace dake::helper;
using namespace dake::math;
using namespace view;
using namespace flappy_box::view;
using flappy_box::model::Paddle;


static bool shaders_valid;
static gl::program *paddle_prg, *vortex_prg;


PaddleGlDrawable::PaddleGlDrawable(const std::shared_ptr<const Paddle> &p):
  _model(p.get())
{
  updateVBOs();


  for (gl::vertex_array &va: vortex_vas) {
    va.set_elements(vortex_line_len * 2);

    gl::vertex_attrib *vap = va.attrib(0);
    vap->format(3);
    vap->data(nullptr, static_cast<size_t>(-1), GL_DYNAMIC_DRAW);
  }

  for (int v = 0; v < vortex_cnt; v++) {
    for (int s = 0; s < vortex_line_len; s++) {
      vortex_dat[v][s][0] = _model->position();
      vortex_dat[v][s][1] = _model->position() + vec3(0.f, 1.5f * r1, 0.f);
    }
  }


  if (shaders_valid) {
    return;
  }


  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT);

  vsh.load("res/paddle_vert.glsl");
  fsh.load("res/paddle_frag.glsl");

  if (!vsh.compile() || !fsh.compile()) {
    throw std::runtime_error("Could not compile paddle shaders");
  }

  paddle_prg = new gl::program;

  *paddle_prg << vsh;
  *paddle_prg << fsh;

  paddle_prg->bind_attrib("in_position", 0);
  paddle_prg->bind_attrib("in_normal",   1);

  paddle_prg->bind_frag("out_mi", 0);
  paddle_prg->bind_frag("out_hi", 1);

  if (!paddle_prg->link()) {
    throw std::runtime_error("Could not link paddle program");
  }

  // set constant uniform right away
  paddle_prg->uniform<vec3>("ambient") = vec3(0.f, 0.f, 0.f);


  gl::shader vvsh(gl::shader::VERTEX), vfsh(gl::shader::FRAGMENT);

  vvsh.load("res/vortex_vert.glsl");
  vfsh.load("res/vortex_frag.glsl");

  if (!vvsh.compile() || !vfsh.compile()) {
    throw std::runtime_error("Could not compile vortex shaders");
  }

  vortex_prg = new gl::program;

  *vortex_prg << vvsh;
  *vortex_prg << vfsh;

  vortex_prg->bind_attrib("in_position", 0);

  vortex_prg->bind_frag("out_mi", 0);
  vortex_prg->bind_frag("out_hi", 1);

  if (!vortex_prg->link()) {
    throw std::runtime_error("Could not link vortex program");
  }

  vortex_prg->uniform<vec3>("ambient") = vec3(1.f, .6f, .3f);


  shaders_valid = true;
}


PaddleGlDrawable::~PaddleGlDrawable(void)
{}


void PaddleGlDrawable::updateVBOs(void)
{
  r0 = maximum(_model->size().x(), _model->size().z()) / 2.f;
  // Tasks says "r1 should be at least r0", but that's BS, it should be
  // "at most r0".
  r1 = minimum(_model->size().y() / 2.f, r0);

  size_for_r = _model->size();


  // welp, creating a nice triangle strip would have been boring, I guess

  vec3 *vertex_positions = new vec3[inner_segments * outer_segments];
  vec3 *vertex_normals   = new vec3[inner_segments * outer_segments];

  for (int outer = 0; outer < outer_segments; outer++) {
    float a = 2.f * static_cast<float>(M_PI) * outer / outer_segments;

    for (int inner = 0; inner < inner_segments; inner++) {
      float b = 2.f * static_cast<float>(M_PI) * inner / inner_segments;

      int index = outer * inner_segments + inner;
      vertex_normals  [index] = cosf(b) * vec3(cosf(a), 0.f, sinf(a)) + vec3(0.f, sinf(b), 0.f);
      vertex_positions[index] =    r0   * vec3(cosf(a), 0.f, sinf(a)) + r1 * vertex_normals[index];
    }
  }


  GLuint *indices = new GLuint[inner_segments * outer_segments * 3 * 2];

  int ii = 0;
  for (int to = 0; to < outer_segments; to++) {
    int no = (to + 1) % outer_segments;

    for (int ti = 0; ti < inner_segments; ti++) {
      int ni = (ti + 1) % inner_segments;

      // I have no idea why this is the other way around than in the task (the
      // task seems correct)
      indices[ii++] = to * inner_segments + ti;
      indices[ii++] = to * inner_segments + ni;
      indices[ii++] = no * inner_segments + ti;

      indices[ii++] = no * inner_segments + ti;
      indices[ii++] = to * inner_segments + ni;
      indices[ii++] = no * inner_segments + ni;
    }
  }


  paddle_va.set_elements(inner_segments * outer_segments * 3 * 2);

  gl::vertex_attrib *vap = paddle_va.attrib(0);
  vap->format(3);
  vap->data(vertex_positions, inner_segments * outer_segments * sizeof(vec3));
  delete[] vertex_positions;

  gl::vertex_attrib *van = paddle_va.attrib(1);
  van->format(3);
  van->data(vertex_normals, inner_segments * outer_segments * sizeof(vec3));
  delete[] vertex_normals;

  gl::elements_array *ela = paddle_va.indices();
  ela->format(3);
  ela->data(indices);
  delete[] indices;


  // rotor

  // creating an index buffer is dumb as hell
  //
  // Without an index buffer, you'll need to transfer the following amount of data:
  // 2 * (blade_cnt * 3) * sizeof(GLfloat)   -- positions and normals
  // = 2 * 9 * 3 * 4 = 216
  //
  // With an index buffer, you'll need to transfer the following amount of data:
  // 2 * (blade_cnt * 2 + 1) * sizeof(GLfloat) + blade_cnt * 3 * sizeof(GLint)
  // = 2 * (9 * 2 + 1) * 4 + 9 * 3 * 4
  // = 152 + 108 = 260
  //
  // Additionally, the normal is wrong.
  //
  // I just noticed that I want two triangles per blade (because I like backface
  // culling), but using an index buffer wouldn't improve the situation there
  // either, because the normal is the other way around. Therefore, it's nearly
  // the same only with double the numbers.

  vertex_positions = new vec3[blade_cnt * 3 * 2];
  vertex_normals   = new vec3[blade_cnt * 3 * 2];

  for (int blade = 0; blade < blade_cnt; blade++) {
    // task cannot into usual arithmetic conversions
    float a1 = 2.f * static_cast<float>(M_PI) * (blade + .3f) / blade_cnt;
    float a2 = 2.f * static_cast<float>(M_PI) * (blade - .3f) / blade_cnt;

    vertex_positions[blade * 6 + 0] = vec3(0.f, 0.f, 0.f);
    vertex_positions[blade * 6 + 1] = vec3((r0 - r1) * cosf(a1), -r1, (r0 - r1) * sinf(a1));
    vertex_positions[blade * 6 + 2] = vec3((r0 - r1) * cosf(a2),  r1, (r0 - r1) * sinf(a2));

    // As the task implies compatibility mode, it would probably have been too
    // easy to auto-calculate the normal
    vertex_normals[blade * 6 + 0] = vertex_normals[blade * 6 + 1] = vertex_normals[blade * 6 + 2] =
        vec3(vertex_positions[blade * 6 + 1].cross(vertex_positions[blade * 6 + 2]));

    vertex_positions[blade * 6 + 3] = vertex_positions[blade * 6 + 0];
    vertex_positions[blade * 6 + 4] = vertex_positions[blade * 6 + 2];
    vertex_positions[blade * 6 + 5] = vertex_positions[blade * 6 + 1];

    vertex_normals[blade * 6 + 3] = vertex_normals[blade * 6 + 4] = vertex_normals[blade * 6 + 5] =
        -vertex_normals[blade * 6 + 0];
  }


  blade_va.set_elements(blade_cnt * 3 * 2);

  vap = blade_va.attrib(0);
  vap->format(3);
  vap->data(vertex_positions);
  delete[] vertex_positions;

  van = blade_va.attrib(1);
  van->format(3);
  van->data(vertex_normals);
  delete[] vertex_normals;
}


void PaddleGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  if (_model->size() != size_for_r) {
    updateVBOs();
  }

  // FIXME (shouldn't be set here)
  r.light_position() = _model->position() + vec3(0.f, 3.f * r1, 1.5f * r0);


  mat4 mv = mat4::identity();
  mv.translate(_model->position());
  mv.scale(vec3(_model->scale(), _model->scale(), _model->scale()));

  paddle_prg->use();
  paddle_prg->uniform<mat4>("mv") = mv;
  paddle_prg->uniform<mat4>("proj") = r.projection() * r.camera();
  paddle_prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  paddle_prg->uniform<vec3>("light_pos") = r.light_position();
  paddle_prg->uniform<float>("enlightenment") = 1.f;
  paddle_prg->uniform<vec3>("diffuse_base") = vec3(.6f, .9f, .7f);

  paddle_va.draw(GL_TRIANGLES);


  mv.rotate(blades_ang, vec3(0.f, 1.f, 0.f));

  float timestep_sec = r.game_model()->timestep().count();

  blades_ang += timestep_sec * 4.f * static_cast<float>(M_PI);
  while (blades_ang >= 2.f * static_cast<float>(M_PI)) {
    blades_ang -= 2.f * static_cast<float>(M_PI);
  }

  paddle_prg->uniform<mat4>("mv") = mv;
  paddle_prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  paddle_prg->uniform<vec3>("diffuse_base") = vec3(.4f, 1.f, .2f);

  blade_va.draw(GL_TRIANGLES);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDepthMask(GL_FALSE);

  vortex_prg->use();
  vortex_prg->uniform<mat4>("proj") = r.projection() * r.camera();
  vortex_prg->uniform<float>("base_alpha") = r.has_bloom() ? .05f : .4f;

  for (int v = 0; v < vortex_cnt; v++) {
    for (int s = vortex_line_len - 1; s > 0; s--) {
      vortex_dat[v][s][0] = vortex_dat[v][s - 1][0] + vec3_type(0.f, _model->relativeFanPower() * timestep_sec * vortex_speed, 0.f);
      vortex_dat[v][s][1] = vortex_dat[v][s][0] + (vortex_dat[v][s - 1][1] - vortex_dat[v][s - 1][0]) * (1.f + .075f * powf(_model->relativeFanPower(), .5f));
    }

    float a = 2.f * static_cast<float>(M_PI) * v / vortex_cnt - blades_ang;

    vortex_dat[v][0][0] = _model->position();
    vortex_dat[v][0][1] = _model->position() + _model->scale() * vec3((r0 - r1) * cosf(a), 1.5f * r1, (r0 - r1) * sinf(a));

    vec3 *vd = static_cast<vec3 *>(vortex_vas[v].attrib(0)->map());
    for (int s = 0; s < vortex_line_len; s++) {
      *(vd++) = vortex_dat[v][s][1] - vec3(.5f * vortex_band_width, 0.f, 0.f);
      *(vd++) = vortex_dat[v][s][1] + vec3(.5f * vortex_band_width, 0.f, 0.f);
    }
    vortex_vas[v].attrib(0)->unmap();

    vortex_prg->uniform<float>("strip_min_y") = vortex_dat[v][0][1].y();
    vortex_prg->uniform<float>("strip_height") = vortex_dat[v][vortex_line_len - 1][1].y() - vortex_dat[v][0][1].y();

    vortex_vas[v].draw(GL_TRIANGLE_STRIP);
  }

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
}
