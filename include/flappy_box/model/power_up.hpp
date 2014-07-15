#ifndef FLAPPY_BOX__MODEL__POWER_UP_HPP
#define FLAPPY_BOX__MODEL__POWER_UP_HPP

#include <string>

#include "math.hpp"

#include "model/game_object.hpp"


namespace flappy_box
{
  namespace model
  {
    class PowerUp: public ::model::GameObject {
      public:
        enum Type {
          LIFE,
          STICKY_WALLS,
          SLOW_GAME,
          EVAPORATE_BOXES,

          PADDLE_SMALLER,
          QUICKEN_GAME,
          DECREASE_FAN,
        };

        enum TypeLimits {
          GOOD_TYPE_START = LIFE,
          GOOD_TYPE_END   = EVAPORATE_BOXES,

          BAD_TYPE_START = PADDLE_SMALLER,
          BAD_TYPE_END   = DECREASE_FAN
        };

        PowerUp(Type t, const vec3_type &pos, const vec3_type &max_pos, const std::string &name = "Explosion");

        Type type(void) const { return t; }

        const vec3_type &position(void) const { return pos; }
        vec3_type &position(void) { return pos; }

        const vec3_type &max_position(void) const { return max_pos; }
        vec3_type &max_position(void) { return max_pos; }

        scalar_type angle(void) const { return a; }
        scalar_type &angle(void) { return a; }


      private:
        Type t;
        vec3_type pos, max_pos;
        scalar_type a = 0.f;
    };
  }
}

#endif
