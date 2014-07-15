#include <dake/gl/gl.hpp>
#include <dake/gl/obj.hpp>
#include <dake/gl/shader.hpp>
#include <dake/math/matrix.hpp>

#include <cstdio>
#include <stdexcept>

#include "resource_finder.hpp"

#include "flappy_box/view/world_gl_drawable.hpp"


using namespace dake;
using namespace dake::math;
using namespace view;
using namespace flappy_box::view;


static gl::program *world_prg;


WorldGlDrawable::WorldGlDrawable(const std::shared_ptr<const flappy_box::model::World> &w):
  _model(w)
{
  gl::obj world = gl::load_obj(find_resource_file("world.obj").c_str());

  if (world.sections.size() != 1) {
    throw std::runtime_error("Could not load world.obj: Mesh does not have exactly one section");
  }

  va = world.sections.front().make_vertex_array(0, -1, 1);


  if (world_prg) {
    return;
  }


  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT);

  vsh.load(find_resource_file("world_vert.glsl").c_str());
  fsh.load(find_resource_file("world_frag.glsl").c_str());

  if (!vsh.compile() || !fsh.compile()) {
    throw std::runtime_error("Could not compile world shaders");
  }

  world_prg = new gl::program;

  *world_prg << vsh;
  *world_prg << fsh;

  world_prg->bind_attrib("in_position", 0);
  world_prg->bind_attrib("in_normal",   1);

  world_prg->bind_frag("out_mi", 0);
  world_prg->bind_frag("out_hi", 1);

  if (!world_prg->link()) {
    throw std::runtime_error("Could not link world program");
  }
}


WorldGlDrawable::~WorldGlDrawable(void)
{
  delete va;
}


void WorldGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  mat4 mv = mat4::identity();
  mv.scale(vec3(_model->getWorldHalfWidth() * 2.f, _model->getWorldHalfHeight() * 2.f, _model->getWorldHalfWidth() * 2.f));

  world_prg->use();
  world_prg->uniform<mat4>("mv") = mv;
  world_prg->uniform<mat4>("proj") = r.projection() * r.camera();
  world_prg->uniform<mat3>("norm_mat") = mat3(mv).transposed_inverse();

  world_prg->uniform<vec3>("light_pos") = r.light_position();

  if (!_model->remainingLives()) {
    was_dead = true;
  } else if (was_dead) {
    was_dead = false;
    time_step = 0.f;
  }

  // FIXME (periodic reset)
  time_step += r.game_model()->timestep().count() * r.game_model()->gameSpeed() * 150.f;
  world_prg->uniform<float>("time_step") = time_step;

  va->draw(GL_TRIANGLES);


  if (_model->remainingLives()) {
    char info[32];

    snprintf(info, sizeof(info), "%i %s", _model->playerPoints(), _model->playerPoints() == 1 ? "point" : "points");
    r.render_line(vec2(-1.f, 1.f), info);

    snprintf(info, sizeof(info), "%i %s", _model->remainingLives(), _model->remainingLives() == 1 ? "life" : "lives");
    r.render_line(vec2(-1.f, 1.f - r.character_size().y()), info);
  }
}
