#ifndef FLAPPY_BOX__MODEL__EXPLOSION_HPP
#define FLAPPY_BOX__MODEL__EXPLOSION_HPP

#include <cstddef>
#include <random>
#include <string>
#include <vector>

#include "model/game_object.hpp"
#include "flappy_box/model/box.hpp"
#include "flappy_box/model/power_up.hpp"


namespace flappy_box
{
  namespace model
  {
    class Explosion: public ::model::GameObject {
      public:
        enum Type {
          BOX_BOX_COLLISION,
          BOX_FLOOR_CRASH,
          BOX_EVAPORATE,

          GOOD_POWER_UP_COLLECTED,
          BAD_POWER_UP_COLLECTED,
        };

        Explosion(Type t, const Box &source, const std::string &name = "Explosion");
        Explosion(Type t, const PowerUp &source, const std::string &name = "Explosion");

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

        const char *info_text(void) const { return info; }


      private:
        std::vector<vec3_type> part_pos, part_vel, part_col;
        std::vector<scalar_type> part_mlt;

        scalar_type lt = 0.f, mlt;
        size_t part_cnt;

        Type t;
        vec3_type ini_pos, max_pos;
        const char *info = nullptr;

        std::default_random_engine rng;

        void generate(const vec3_type &velocity, const vec3_type &size, const vec3_type &base_color);
    };
  }
}

#endif
