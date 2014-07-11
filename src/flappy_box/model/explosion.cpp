#include <random>

#include "math.hpp"

#include "flappy_box/model/box.hpp"
#include "flappy_box/model/explosion.hpp"


using namespace model;
using namespace flappy_box::model;


Explosion::Explosion(const Box &source, const vec3_type &base_color, size_t amount, scalar_type max_lifetime, const std::string &name):
  GameObject(true, name), mlt(max_lifetime), part_cnt(amount), ini_pos(source.position()), max_pos(source.maxPosition())
{
  part_pos.resize(amount);
  part_vel.resize(amount);
  part_col.resize(amount);
  part_mlt.resize(amount);


  std::uniform_real_distribution<float> pos_x(source.position().x() - source.size() / 2.f, source.position().x() + source.size() / 2.f);
  std::uniform_real_distribution<float> pos_y(source.position().y() - source.size() / 2.f, source.position().y() + source.size() / 2.f);
  std::uniform_real_distribution<float> pos_z(source.position().z() - source.size() / 2.f, source.position().z() + source.size() / 2.f);

  std::uniform_real_distribution<float> vel_x(source.velocity().x() * .8f + copysignf(5.f, -source.velocity().x()), source.velocity().x() * 1.25f + copysignf(5.f, source.velocity().x()));
  std::uniform_real_distribution<float> vel_y(source.velocity().y() * .8f + copysignf(5.f, -source.velocity().y()), source.velocity().y() * 1.25f + copysignf(5.f, source.velocity().y()));
  std::uniform_real_distribution<float> vel_z(source.velocity().z() * .8f + copysignf(5.f, -source.velocity().z()), source.velocity().z() * 1.25f + copysignf(5.f, source.velocity().z()));

  std::uniform_real_distribution<float> col_r(base_color.r() * .8f, base_color.r() * 1.25f);
  std::uniform_real_distribution<float> col_g(base_color.g() * .8f, base_color.g() * 1.25f);
  std::uniform_real_distribution<float> col_b(base_color.b() * .8f, base_color.b() * 1.25f);

  std::uniform_real_distribution<float> mlt(max_lifetime * .5f, max_lifetime);


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
    m = mlt(rng);
  }
}
