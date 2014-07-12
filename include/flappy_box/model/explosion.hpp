#ifndef FLAPPY_BOX__MODEL__EXPLOSION_HPP
#define FLAPPY_BOX__MODEL__EXPLOSION_HPP

#include <cstddef>
#include <random>
#include <string>
#include <vector>

#include "model/game_object.hpp"
#include "flappy_box/model/box.hpp"


namespace flappy_box
{
  namespace model
  {
    class Explosion: public ::model::GameObject {
      public:
        enum Type {
          BOX_BOX_COLLISION,
          BOX_FLOOR_CRASH,
        };

        Explosion(Type t, const Box &source, const vec3_type &base_color, size_t particles, scalar_type max_lifetime, const std::string &name = "Explosion");

        const std::vector<vec3_type> &particle_positions(void) const { return part_pos; }
        std::vector<vec3_type> &particle_positions(void) { return part_pos; }

        const std::vector<vec3_type> &particle_velocities(void) const { return part_vel; }
        std::vector<vec3_type> &particle_velocities(void) { return part_vel; }

        const std::vector<vec3_type> &particle_colors(void) const { return part_col; }
        std::vector<vec3_type> &particle_colors(void) { return part_col; }

        const std::vector<scalar_type> &particle_max_lifetimes(void) const { return part_mlt; }
        std::vector<scalar_type> &particle_max_lifetimes(void) { return part_mlt; }

        size_t particle_count(void) const { return part_cnt; }

        scalar_type lifetime(void) const { return lt; }
        scalar_type &lifetime(void) { return lt; }

        scalar_type max_lifetime(void) const { return mlt; }

        Type type(void) const { return t; }

        const vec3_type &max_position(void) const { return max_pos; }
        const vec3_type &initial_position(void) const { return ini_pos; }


      private:
        std::vector<vec3_type> part_pos, part_vel, part_col;
        std::vector<scalar_type> part_mlt;

        scalar_type lt = 0.f, mlt;
        size_t part_cnt;

        Type t;
        vec3_type ini_pos, max_pos;

        std::default_random_engine rng;
    };
  }
}

#endif
