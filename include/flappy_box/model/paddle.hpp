#ifndef FLAPPY_BOX__MODEL__PADDLE_HPP
#define FLAPPY_BOX__MODEL__PADDLE_HPP

#include <string>

#include "math.hpp"
#include "model/game_object.hpp"


namespace flappy_box
{
  namespace model
  {
    class Paddle: public ::model::GameObject {
      public:
        Paddle(const std::string &name);

        vec3_type &position(void) { return _position; }
        const vec3_type &position(void) const { return _position; }

        vec3_type &maxPosition(void) { return _maxPosition; }
        const vec3_type &maxPosition(void) const { return _maxPosition; }

        vec3_type &velocity(void) { return _velocity; }
        const vec3_type &velocity(void) const { return _velocity; }

        vec3_type &acceleration(void) { return _acceleration; }
        const vec3_type &acceleration(void) const { return _acceleration; }

        vec3_type &size(void) { return _size; }
        const vec3_type &size(void) const { return _size; }

        vec3_type &playerControl(void) { return _playerControl; }
        const vec3_type &playerControl(void) const { return _playerControl; }


      private:
        vec3_type _position      = vec3_type(0.f, 0.f, 0.f);
        vec3_type _maxPosition   = vec3_type(1.f, HUGE_VALF, 0.f);
        vec3_type _velocity      = vec3_type(0.f, 0.f, 0.f);
        vec3_type _acceleration  = vec3_type(0.f, 0.f, 0.f);
        vec3_type _size          = vec3_type(1.f, 1.f, 1.f);
        vec3_type _playerControl = vec3_type(0.f, 0.f, 0.f);
    };
  }
}

#endif
