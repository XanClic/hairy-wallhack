#include <dake/gl/shader.hpp>
#include <dake/gl/vertex_attrib.hpp>
#include <dake/math/matrix.hpp>

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <stdexcept>

#include "resource_finder.hpp"

#include "flappy_box/view/explosion_gl_drawable.hpp"


using namespace dake;
using namespace dake::math;
using namespace view;
using namespace flappy_box::view;
using flappy_box::model::Explosion;


static bool resources_valid;

static gl::program *particle_prg;


ExplosionGlDrawable::ExplosionGlDrawable(const std::shared_ptr<const Explosion> &x):
  _model(x.get())
{
  particles_va.set_elements(_model->particle_count());

  particles_va.attrib(0)->format(3);
  particles_va.attrib(1)->format(3);

  gl::vertex_attrib *vac = particles_va.attrib(2);
  vac->format(3);
  vac->data(_model->particle_colors().data());

  particles_va.attrib(3)->format(1);


  if (resources_valid) {
    return;
  }


  gl::shader vsh(gl::shader::VERTEX), gsh(gl::shader::GEOMETRY), fsh(gl::shader::FRAGMENT);

  vsh.load(find_resource_file("particle_vert.glsl").c_str());
  gsh.load(find_resource_file("particle_geom.glsl").c_str());
  fsh.load(find_resource_file("particle_frag.glsl").c_str());

  if (!vsh.compile() || !gsh.compile() || !fsh.compile()) {
    throw std::runtime_error("Could not compile particle shaders");
  }

  particle_prg = new gl::program;

  *particle_prg << vsh;
  *particle_prg << gsh;
  *particle_prg << fsh;

  particle_prg->bind_attrib("in_position",     0);
  particle_prg->bind_attrib("in_velocity",     1);
  particle_prg->bind_attrib("in_color",        2);
  particle_prg->bind_attrib("in_max_lifetime", 3);

  particle_prg->bind_frag("out_mi", 0);
  particle_prg->bind_frag("out_hi", 1);

  if (!particle_prg->link()) {
    throw std::runtime_error("Could not link particle program");
  }


  resources_valid = true;
}


ExplosionGlDrawable::~ExplosionGlDrawable(void)
{}


void ExplosionGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  if (newfag) {
    newfag = false;

    if (_model->info_text()) {
      r.log_add(_model->info_text());
    }
  }


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDepthMask(GL_FALSE);

  // whatever works
  glPointSize(3.f);
  glLineWidth(3.f);


  particles_va.attrib(0)->data(_model->particle_positions()    .data(), static_cast<size_t>(-1), GL_DYNAMIC_DRAW);
  particles_va.attrib(1)->data(_model->particle_velocities()   .data(), static_cast<size_t>(-1), GL_DYNAMIC_DRAW);
  particles_va.attrib(3)->data(_model->particle_max_lifetimes().data(), static_cast<size_t>(-1), GL_DYNAMIC_DRAW);

  particle_prg->use();
  particle_prg->uniform<mat4>("proj") = r.projection() * r.camera();
  particle_prg->uniform<float>("lifetime") = _model->lifetime();

  particles_va.draw(GL_POINTS);


  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
}
