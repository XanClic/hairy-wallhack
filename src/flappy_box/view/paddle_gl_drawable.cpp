#include <dake/gl/elements_array.hpp>
#include <dake/gl/gl.hpp>
#include <dake/gl/obj.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>
#include <dake/math/matrix.hpp>

#include "flappy_box/model/paddle.hpp"
#include "flappy_box/view/paddle_gl_drawable.hpp"


using namespace dake;
using namespace dake::math;
using namespace view;
using namespace flappy_box::view;
using flappy_box::model::Paddle;


static gl::program *paddle_prg;


PaddleGlDrawable::PaddleGlDrawable(const std::shared_ptr<const Paddle> &p):
  _model(p)
{
  updateVBOs();


  if (paddle_prg) {
    return;
  }

  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT);

  vsh.load("res/paddle_vsh.glsl");
  fsh.load("res/paddle_fsh.glsl");

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
    delete paddle_prg;
    paddle_prg = nullptr;

    throw std::runtime_error("Could not link paddle program");
  }


  // set constant uniforms right away

  paddle_prg->uniform<vec3>("diffuse_base") = vec3(.6f, .9f, .7f);
  paddle_prg->uniform<vec3>("ambient") = vec3(0.f, 0.f, 0.f);
}


PaddleGlDrawable::~PaddleGlDrawable(void)
{}


void PaddleGlDrawable::updateVBOs(void)
{
  r0 = (_model->size().x() > _model->size().z() ? _model->size().x() : _model->size().z()) / 2.f;
  // Tasks says "r1 should be at least r0", but that's BS, it should be
  // "at most r0".
  r1 = _model->size().y() / 2.f > r0 ? r0 : _model->size().y() / 2.f;

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
}


void PaddleGlDrawable::visualize(GlRenderer &r, GlutWindow &w)
{
  if (_model->size() != size_for_r) {
    updateVBOs();
  }

  // FIXME (shouldn't be set here)
  r.light_position() = _model->position() + vec3(0.f, 3.f * r1, 1.5f * r0);

  mat4 mv = mat4::identity();
  mv.translate(_model->position());

  paddle_prg->use();
  paddle_prg->uniform<mat4>("mv") = mv;
  paddle_prg->uniform<mat4>("proj") = r.projection() * r.camera();
  paddle_prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  paddle_prg->uniform<vec3>("light_pos") = r.light_position();
  paddle_prg->uniform<float>("enlightenment") = 1.f;

  paddle_va.draw(GL_TRIANGLES);
}
