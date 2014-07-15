#include <dake/gl/gl.hpp>
#include <dake/gl/obj.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/vertex_array.hpp>

#include "resource_finder.hpp"

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
static gl::program *box_prg, *box_fog_prg;


BoxGlDrawable::BoxGlDrawable(const std::shared_ptr<Box> &b):
  _model(b.get())
{
  if (resources_valid) {
    return;
  }


  gl::obj box = gl::load_obj(find_resource_file("box.obj").c_str());

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


  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT), fog_fsh(gl::shader::FRAGMENT);

  vsh.load(find_resource_file("box_vert.glsl").c_str());
  fsh.load(find_resource_file("box_frag.glsl").c_str());
  fog_fsh.load(find_resource_file("box_fog_frag.glsl").c_str());

  if (!vsh.compile() || !fsh.compile() || !fog_fsh.compile()) {
    throw std::runtime_error("Could not compile box shaders");
  }

  box_prg = new gl::program;
  box_fog_prg = new gl::program;

  *box_prg << vsh;
  *box_prg << fsh;

  *box_fog_prg << vsh;
  *box_fog_prg << fog_fsh;

  box_prg->bind_attrib("in_position",  0);
  box_prg->bind_attrib("in_tex_coord", 1);
  box_prg->bind_attrib("in_normal",    2);

  box_prg->bind_frag("out_mi", 0);
  box_prg->bind_frag("out_hi", 1);

  box_fog_prg->bind_attrib("in_position",  0);
  box_fog_prg->bind_attrib("in_tex_coord", 1);
  box_fog_prg->bind_attrib("in_normal",    2);

  box_fog_prg->bind_frag("out_mi", 0);
  box_fog_prg->bind_frag("out_hi", 1);

  if (!box_prg->link() || !box_fog_prg->link()) {
    throw std::runtime_error("Could not link box programs");
  }


  resources_valid = true;
}

BoxGlDrawable::~BoxGlDrawable()
{}

void BoxGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  mat4 mv = mat4::identity();
  mv.translate(_model->position() + vec3(0.f, 0.f, -10.f / lifetime));
  mv.rotate(_model->angle(), vec3(0.f, 0.f, 1.f));
  mv.scale(vec3(_model->size(), _model->size(), _model->size()));

  gl::program *prg = lifetime > 1.f ? box_prg : box_fog_prg;

  prg->use();
  prg->uniform<mat4>("mv") = mv;
  prg->uniform<mat4>("proj") = r.projection() * r.camera();
  prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  prg->uniform<vec3>("light_pos") = r.light_position();

  for (bool lines: {false, true}) {
    if (lines) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(5.f);
    }

    prg->uniform<float>("enlightenment") = (_model->maxPosition().y() - _model->position().y()) / 10.f + 1.f;

    for (size_t i = 0; i < box_sections.size(); i++) {
      if (box_sections[i].material.tex) {
        box_sections[i].material.tex->bind();
        prg->uniform<gl::texture>("tex") = *box_sections[i].material.tex;
      }
      prg->uniform<vec3>("diffuse_base") = box_sections[i].material.diffuse;
      prg->uniform<vec3>("ambient") = lines ? vec3(box_sections[i].material.ambient) : vec3(0.f, 0.f, 0.f);

      box_vas[i]->draw(GL_TRIANGLES);
    }
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  lifetime += r.game_model()->timestep().count();
}
