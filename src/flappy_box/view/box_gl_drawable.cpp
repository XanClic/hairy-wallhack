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
static gl::program *box_prg, *box_fog_prg;


BoxGlDrawable::BoxGlDrawable(const std::shared_ptr<Box> &b):
  _model(b.get())
{
  if (resources_valid) {
    return;
  }


  gl::obj box = gl::load_obj(drop ? "res/drop.obj" : "res/box.obj");

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


  gl::shader vsh(gl::shader::VERTEX, "res/box_vert.glsl");

  box_prg     = new gl::program {gl::shader::frag(drop ? "res/drop_frag.glsl"     : "res/box_frag.glsl")};
  box_fog_prg = new gl::program {gl::shader::frag(drop ? "res/drop_fog_frag.glsl" : "res/box_fog_frag.glsl")};

  *box_prg     << vsh;
  *box_fog_prg << vsh;

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


  resources_valid = true;
}

BoxGlDrawable::~BoxGlDrawable()
{}

void BoxGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  mat4 mv = mat4::identity();
  mv.translate(_model->position() + vec3(0.f, 0.f, -10.f / lifetime));
  if (drop) {
    vec3 vlcty = _model->velocity() / 200.f;
    //                        3^{1/2} (length of (1, 1, 1))
    mv.scale(_model->size() * 1.73205f * (vec3(1.f, 1.f, 1.f) + vec3(fabsf(vlcty.x()), fabsf(vlcty.y()), fabsf(vlcty.z()))).normalized());
  } else {
    mv.scale(_model->size() * vec3(1.f, 1.f, 1.f));
  }
  mv.rotate(_model->angle(), vec3(0.f, 0.f, 1.f));

  gl::program *prg = lifetime > 1.f ? box_prg : box_fog_prg;

  prg->use();
  prg->uniform<mat4>("mv") = mv;
  prg->uniform<mat4>("proj") = r.projection() * r.camera();
  prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  prg->uniform<vec3>("light_pos") = r.light_position();

  if (drop) {
    prg->uniform<vec3>("cam_pos") = r.camera_position();
  }

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
      prg->uniform<vec3>("ambient") = lines || drop ? vec3(box_sections[i].material.ambient) : vec3(0.f, 0.f, 0.f);

      box_vas[i]->draw(GL_TRIANGLES);
    }

    if (drop) {
      break;
    }
  }

  if (!drop) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  lifetime += r.game_model()->timestep().count();
}
