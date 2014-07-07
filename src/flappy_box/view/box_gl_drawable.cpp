#include <dake/gl/gl.hpp>
#include <dake/gl/obj.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/vertex_array.hpp>

#include "flappy_box/view/box_gl_drawable.hpp"

#include <algorithm>
#include <vector>


using namespace dake;
using namespace dake::math;
using namespace view;
using namespace flappy_box::view;
using flappy_box::model::Box;


static bool resources_valid;

static std::vector<gl::obj_section> box_sections;
static std::vector<gl::vertex_array *> box_vas;
static gl::program *box_prg;


BoxGlDrawable::BoxGlDrawable(const std::shared_ptr<Box> &b):
  _model(b)
{
  if (resources_valid) {
    return;
  }


  gl::obj box = gl::load_obj("res/box.obj");

  // Fit into 1x1x1 box
  float scale = HUGE_VALF;
  for (int i = 0; i < 3; i++) {
    float s = 1.f / (box.upper_right[i] - box.lower_left[i]);
    if (s < scale) {
      scale = s;
    }
  }

  vec3 translation = -(box.upper_right + box.lower_left) / 2;

  for (gl::obj_section &s: box.sections) {
    for (vec3 &p: s.positions) {
      p = scale * (p + translation);
    }

    box_vas.push_back(s.make_vertex_array(0, 1, 2));
  }

  box_sections = std::move(box.sections);


  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT);

  vsh.load("res/box_vsh.glsl");
  fsh.load("res/box_fsh.glsl");

  if (!vsh.compile() || !fsh.compile()) {
    throw std::runtime_error("Could not compile box shaders");
  }

  box_prg = new gl::program;

  *box_prg << vsh;
  *box_prg << fsh;

  box_prg->bind_attrib("in_position",  0);
  box_prg->bind_attrib("in_tex_coord", 1);
  box_prg->bind_attrib("in_normal",    2);

  box_prg->bind_frag("out_mi", 0);
  box_prg->bind_frag("out_hi", 1);

  if (!box_prg->link()) {
    delete box_prg;

    throw std::runtime_error("Could not link box program");
  }


  resources_valid = true;
}

BoxGlDrawable::~BoxGlDrawable()
{}

void BoxGlDrawable::visualize(GlRenderer &r, GlutWindow &w)
{
  mat4 mvp = r.projection() * r.camera();
  mvp.translate(_model->position());
  mvp.rotate(_model->angle(), vec3(0.f, 0.f, 1.f));
  mvp.scale(vec3(_model->size(), _model->size(), _model->size()));

  box_prg->use();
  box_prg->uniform<mat4>("mvp") = mvp;
  box_prg->uniform<mat3>("norm_mat") = mat3(mvp).transposed_inverse();

  box_prg->uniform<vec3>("light_dir") = vec3(-1.f, -3.f, 1.f);
  box_prg->uniform<vec3>("ambient") = vec3(.1f, .1f, .1f);

  for (bool lines: {false, true}) {
    if (lines) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(5.f);
    }

    box_prg->uniform<float>("enlightenment") = lines ? 2.f : 1.f;

    for (size_t i = 0; i < box_sections.size(); i++) {
      box_prg->uniform<gl::texture>("tex") = *box_sections[i].material.tex;
      box_sections[i].material.tex->bind();

      box_vas[i]->draw(GL_TRIANGLES);
    }
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
