#include <dake/gl/obj.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>
#include <dake/math/matrix.hpp>

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <stdexcept>

#include "flappy_box/view/power_up_gl_drawable.hpp"


using namespace dake;
using namespace dake::math;
using namespace view;
using namespace flappy_box::view;
using flappy_box::model::PowerUp;


static bool resources_valid;

static gl::program *power_up_prg;
static gl::vertex_array *power_up_va;


PowerUpGlDrawable::PowerUpGlDrawable(const std::shared_ptr<const PowerUp> &pu):
  _model(pu.get())
{
  if (resources_valid) {
    return;
  }


  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT);

  vsh.load("res/power_up_vert.glsl");
  fsh.load("res/power_up_frag.glsl");

  if (!vsh.compile() || !fsh.compile()) {
    throw std::runtime_error("Could not compile power-up shaders");
  }

  power_up_prg = new gl::program;

  *power_up_prg << vsh;
  *power_up_prg << fsh;

  power_up_prg->bind_attrib("in_position", 0);
  power_up_prg->bind_attrib("in_normal",   1);

  power_up_prg->bind_frag("out_mi", 0);
  power_up_prg->bind_frag("out_hi", 1);

  if (!power_up_prg->link()) {
    throw std::runtime_error("Could not link power-up program");
  }


  gl::obj obj = gl::load_obj("res/power_up.obj");

  if (obj.sections.size() != 1) {
    throw std::runtime_error("Could not load power-up mesh: Has more than one material");
  }

  // Fit into 1x1x1 obj
  float scale = HUGE_VALF;
  for (int i = 0; i < 3; i++) {
    float s = 1.f / (obj.upper_right[i] - obj.lower_left[i]);
    if (s < scale) {
      scale = s;
    }
  }

  vec3 translation = -(obj.upper_right + obj.lower_left) / 2;

  for (vec3 &p: obj.sections.front().positions) {
    p = scale * (p + translation);
  }

  power_up_va = obj.sections.front().make_vertex_array(0, -1, 1);
}


void PowerUpGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  mat4 mv = mat4::identity();
  mv.translate(_model->position());
  mv.rotate(_model->angle(), vec3(0.f, 1.f, 0.f));
  mv.scale(vec3(20.f, 20.f, 20.f));

  power_up_prg->use();
  power_up_prg->uniform<mat4>("mv") = mv;
  power_up_prg->uniform<mat4>("proj") = r.projection() * r.camera();
  power_up_prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  power_up_prg->uniform<vec3>("light_pos") = r.light_position();
  power_up_prg->uniform<vec3>("cam_pos") = r.camera_position();

  switch (_model->type()) {
    case PowerUp::LIFE:
      power_up_prg->uniform<vec3>("diffuse") = vec3(1.f, 2.5f, .5f);
      power_up_prg->uniform<vec3>("specular") = vec3(4.f, 10.f, 2.f);
      break;

    case PowerUp::STICKY_WALLS:
      power_up_prg->uniform<vec3>("diffuse") = vec3(5.f, 7.f, 10.f);
      power_up_prg->uniform<vec3>("specular") = vec3(0.f, 0.f, 0.f);
      break;

    case PowerUp::SLOW_GAME:
      power_up_prg->uniform<vec3>("diffuse") = vec3(0.f, 0.f, 0.f);
      power_up_prg->uniform<vec3>("specular") = vec3(18.f, 5.f, 20.f);
      break;

    case PowerUp::EVAPORATE_BOXES:
      power_up_prg->uniform<vec3>("diffuse") = vec3(10.f, 10.f, 10.f);
      power_up_prg->uniform<vec3>("specular") = vec3(0.f, 0.f, 0.f);
      break;

    case PowerUp::PADDLE_SMALLER:
      power_up_prg->uniform<vec3>("diffuse") = vec3(2.5f, .75f, .5f);
      power_up_prg->uniform<vec3>("specular") = vec3(10.f, 3.f, 2.f);
      break;

    case PowerUp::QUICKEN_GAME:
      power_up_prg->uniform<vec3>("diffuse") = vec3(2.f, 1.4f, 1.5f);
      power_up_prg->uniform<vec3>("specular") = vec3(20.f, 14.f, 15.f);
      break;

    case PowerUp::DECREASE_FAN:
      power_up_prg->uniform<vec3>("diffuse") = vec3(2.f, 1.6f, .4f);
      power_up_prg->uniform<vec3>("specular") = vec3(10.f, 8.f, 2.f);
      break;
  }

  power_up_va->draw(GL_TRIANGLES);
}
