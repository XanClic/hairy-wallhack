#include "math.hpp"
# include "flappy_box/controller/box_object_logic.hpp"

#include <cstdio>


static const scalar_type accel_damping = .1f;
static const scalar_type vlcty_damping = .8f;
static const vec3_type gravity = vec3_type(0.f, -9.81f, 0.f);


using namespace ::flappy_box::controller;

BoxObjectLogic::BoxObjectLogic(const std::shared_ptr< flappy_box::model::Box >& b )
: ::controller::Logic::ObjectLogic()
, _model( b )
{}

bool BoxObjectLogic::advance( ::controller::Logic& l, ::controller::InputEventHandler::keyboard_event const& ev )
{
  scalar_type timestep_sec = l.game_model()->timestep().count();

  _model->rotAcceleration() = _model->rotAcceleration() * accel_damping - _model->externalForce().x() / 10.f;
  _model->rotVelocity()    += _model->rotAcceleration() * timestep_sec;
  _model->angle()          += _model->rotVelocity()     * timestep_sec;

  _model->angle() = fmodf(_model->angle(), 2.f * static_cast<float>(M_PI));

  _model->acceleration() = _model->acceleration() * accel_damping + _model->externalForce() + gravity;
  _model->velocity()    += _model->acceleration() * timestep_sec;
  _model->position()    += _model->velocity()     * timestep_sec;

  for (int i = 0; i < 3; i++) {
    if (fabs(_model->position()[i]) > _model->maxPosition()[i]) {
      _model->position()[i] = copysign(_model->maxPosition()[i], _model->position()[i]);
      _model->velocity()[i] *= -vlcty_damping;
    }
  }

  return false;
}
