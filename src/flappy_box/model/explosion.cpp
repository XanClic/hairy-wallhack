#include <random>
#include <stdexcept>
#include <string>

#include "math.hpp"

#include "flappy_box/model/box.hpp"
#include "flappy_box/model/explosion.hpp"
#include "flappy_box/model/power_up.hpp"


using namespace model;
using namespace flappy_box::model;


Explosion::Explosion(Type tp, const Box &source, const std::string &name):
  GameObject(true, name), t(tp), ini_pos(source.position())
{
  max_pos = vec3_type(source.maxPosition().x() + source.size() / 2.f, source.maxPosition().y() + source.size() / 2.f, HUGE_VALF);
  part_cnt = lrintf(2.f * powf(source.size(), 3.f));

  vec3_type base_color;
  switch (t) {
    case BOX_BOX_COLLISION: base_color = vec3_type(.2f, 1.f, .2f); mlt = .5f; break;
    case BOX_FLOOR_CRASH:   base_color = vec3_type(1.f, .1f, .1f); mlt = 2.f; break;
    case BOX_EVAPORATE:     base_color = vec3_type(.5f, .7f, 1.f); mlt = 3.f; break;
    default: throw std::runtime_error("Invalid type for Box-sourced explosion");
  }

  generate(source.velocity(), vec3_type(source.size(), source.size(), source.size()), base_color);
}


Explosion::Explosion(Type tp, const PowerUp &source, const std::string &name):
  GameObject(true, name), t(tp), ini_pos(source.position()), max_pos(source.max_position())
{
  part_cnt = 500.f;
  mlt = 1.f;

  vec3_type base_color;
  switch (source.type()) {
    case PowerUp::LIFE:            base_color = vec3_type(.4f, 1.f, .2f); info = "1up"; break;
    case PowerUp::STICKY_WALLS:    base_color = vec3_type(.5f, .7f, 1.f); info = "the walls are jelly"; break;
    case PowerUp::SLOW_GAME:       base_color = vec3_type(HUGE_VALF, HUGE_VALF, HUGE_VALF); info = "who homu here"; break;
    case PowerUp::EVAPORATE_BOXES: base_color = vec3_type(1.f, 1.f, 1.f); info = "are you a wizard"; break;
    case PowerUp::PADDLE_SMALLER:  base_color = vec3_type(1.f, .3f, .2f); info = "Honey, I Shrunk the Paddle"; break;
    case PowerUp::QUICKEN_GAME:    base_color = vec3_type(HUGE_VALF, HUGE_VALF, HUGE_VALF); info = "2fast4u"; break;
    case PowerUp::DECREASE_FAN:    base_color = vec3_type(1.f, .8f, .2f); info = "Money was tight again this month"; break;
  }

  // FIXME: Speed
  generate(vec3_type(0.f, 50.f, 100.f), vec3_type(10.f, 10.f, 10.f), base_color);
}


void Explosion::generate(const vec3_type &velocity, const vec3_type &size, const vec3_type &base_color)
{
  part_pos.resize(part_cnt);
  part_vel.resize(part_cnt);
  part_col.resize(part_cnt);
  part_mlt.resize(part_cnt);


  // FIXME
  // (If a box crashes down and bounces back before the collision is detected,
  // the particles will fly in the wrong direction; this is to prevent that
  // case, but it looks like an ugly hack)
  vec3_type vel = velocity;
  if ((t == BOX_FLOOR_CRASH) && (vel.y() > 0.f)) {
    vel.y() *= -1.f;
  }


  std::uniform_real_distribution<float> pos_x(ini_pos.x() - size.x() / 2.f, ini_pos.x() + size.x() / 2.f);
  std::uniform_real_distribution<float> pos_y(ini_pos.y() - size.y() / 2.f, ini_pos.y() + size.y() / 2.f);
  std::uniform_real_distribution<float> pos_z(ini_pos.z() - size.z() / 2.f, ini_pos.z() + size.z() / 2.f);

  std::uniform_real_distribution<float> vel_x(vel.x() * .8f + copysignf(5.f, -vel.x()), vel.x() * 1.25f + copysignf(5.f, vel.x()));
  std::uniform_real_distribution<float> vel_y(vel.y() * .8f + copysignf(5.f, -vel.y()), vel.y() * 1.25f + copysignf(5.f, vel.y()));
  std::uniform_real_distribution<float> vel_z(vel.z() * .8f + copysignf(5.f, -vel.z()), vel.z() * 1.25f + copysignf(5.f, vel.z()));

  std::uniform_real_distribution<float> col_r, col_g, col_b;
  if (base_color.length() <= 10.f) {
    col_r = std::uniform_real_distribution<float>(base_color.r() * .8f, base_color.r() * 1.25f);
    col_g = std::uniform_real_distribution<float>(base_color.g() * .8f, base_color.g() * 1.25f);
    col_b = std::uniform_real_distribution<float>(base_color.b() * .8f, base_color.b() * 1.25f);
  } else {
    col_r = std::uniform_real_distribution<float>(0.f, 1.f);
    col_g = std::uniform_real_distribution<float>(0.f, 1.f);
    col_b = std::uniform_real_distribution<float>(0.f, 1.f);
  }

  std::uniform_real_distribution<float> mlt_v(mlt * .5f, mlt);


  for (vec3_type &p: part_pos) {
    p.x() = pos_x(rng);
    p.y() = pos_y(rng);
    p.z() = pos_z(rng);
  }

  for (vec3_type &v: part_vel) {
    v.x() = vel_x(rng);
    v.y() = vel_y(rng);
    v.z() = vel_z(rng);
  }

  for (vec3_type &c: part_col) {
    c.r() = col_r(rng);
    c.g() = col_g(rng);
    c.b() = col_b(rng);
  }

  for (scalar_type &m: part_mlt) {
    m = mlt_v(rng);
  }
}
