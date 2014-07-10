#ifndef FLAPPY_BOX__MODEL__BOX_HPP
#define FLAPPY_BOX__MODEL__BOX_HPP

# include "model/game_object.hpp"
# include "math.hpp"

// ATTENTION: NEW CODE!

namespace flappy_box
{
  namespace model
  {
    class Box: public ::model::GameObject {
      public:
        Box(const std::string & = "Box", bool mute = false);

        scalar_type &angle(void) { return _angle; }
        scalar_type angle(void) const { return _angle; }

        vec3_type &position(void) { return _position; }
        const vec3_type &position(void) const { return _position; }

        vec3_type &maxPosition(void) { return _maxPosition; }
        const vec3_type &maxPosition(void) const { return _maxPosition; }

        scalar_type &size(void) { return _size; }
        scalar_type size(void) const { return _size; }

        vec3_type &velocity(void) { return _velocity; }
        const vec3_type &velocity(void) const { return _velocity; }

        vec3_type &acceleration(void) { return _acceleration; }
        const vec3_type &acceleration(void) const { return _acceleration; }

        vec3_type &externalForce(void) { return _externalForce; }
        const vec3_type &externalForce(void) const { return _externalForce; }

        scalar_type &rotAcceleration(void) { return _rotAcceleration; }
        scalar_type rotAcceleration(void) const { return _rotAcceleration; }

        scalar_type &rotVelocity(void) { return _rotVelocity; }
        scalar_type rotVelocity(void) const { return _rotVelocity; }

        bool mute(void) const { return is_mute; }


    private:
        scalar_type           _angle = 0.f;
        scalar_type            _size = 1.f; // same size in every direction
        vec3_type          _position = vec3_type(0.f, 0.f, 0.f);
        vec3_type       _maxPosition = vec3_type(1.f, 1.f, HUGE_VALF);
        vec3_type          _velocity = vec3_type(0.f, 0.f, 0.f);
        vec3_type      _acceleration = vec3_type(0.f, 0.f, 0.f);
        scalar_type _rotAcceleration = 0.f; // only in y-direction
        scalar_type     _rotVelocity = 0.f; // only in y-direction
        vec3_type     _externalForce = vec3_type(0.f, 0.f, 0.f); // vector containing the external force (direction is normalized, length is force)

        bool is_mute;
    }; // Box
  } // model::
} // flappy_box::

#endif
