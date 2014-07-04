# pragma once

# include "model/game_object.hpp"
# include "math.hpp"

// ATTENTION: NEW CODE!

namespace flappy_box
{

  namespace model
  {
    class Box : public ::model::GameObject
    {
      public:

        Box( const std::string& = "Box" );

        const double& angle() const { return _angle; }
        void setAngle( double a ) { _angle = a; }

        const vec3_type& position() const { return _position; }
        void setPosition( const vec3_type& p ) { _position = p; }
        
        const vec3_type& maxPosition() const { return _maxPosition; }
        void setMaxPosition( const vec3_type& p ) { _maxPosition = p; }
        
        const double& size() const { return _size; }
        void setSize( double s ) { _size = s; }

        const vec3_type& velocity() const { return _velocity; }
        void setVelocity( const vec3_type& v ) { _velocity = v; }

        const vec3_type& acceleration() const { return _acceleration; }
        void setAcceleration( const vec3_type& a ) { _acceleration = a; }
        
        const vec3_type& externalForce() const { return _externalForce; }
        void setExternalForce( const vec3_type& f ) { _externalForce = f; }
        
        const double& rotAcceleration() const { return _rotAcceleration; }
        void setRotAcceleration( double ra ) { _rotAcceleration = ra; }
        
        const double& rotVelocity() const { return _rotVelocity; }
        void setRotVelocity( double rv ) { _rotVelocity = rv; }

    private:

        double               _angle;
        double                _size; // same size in every direction
        vec3_type         _position;
        vec3_type      _maxPosition;
        vec3_type         _velocity;
        vec3_type     _acceleration;
        double     _rotAcceleration; // only in y-direction
        double         _rotVelocity; // only in y-direction
        vec3_type    _externalForce; // vector containing the external force (direction is normalized, length is force)

    }; // Box

  } // model::
} // flappy_box::