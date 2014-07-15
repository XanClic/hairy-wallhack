#include "math.hpp"

#include "flappy_box/controller/box_object_logic.hpp"

#include <cstdio>


static const scalar_type accel_damping = .25f;
static const scalar_type vlcty_damping = .8f;
static const scalar_type rot_vlcty_damping = .05f;
static const scalar_type max_rot_vlcty = 20.f;
static const vec3_type gravity = vec3_type(0.f, -9.81f, 0.f);


using namespace controller;
using namespace flappy_box::controller;
using flappy_box::model::Box;


BoxObjectLogic::BoxObjectLogic(const std::shared_ptr<Box> &b):
  Logic::ObjectLogic(), _model(b.get())
{}


bool BoxObjectLogic::advance(Logic &l, const InputEventHandler::keyboard_event &)
{
  scalar_type timestep_sec = l.game_model()->timestep().count();

  _model->rotAcceleration() = _model->rotAcceleration() * powf(accel_damping, timestep_sec * 100.f) - _model->externalForce().x() / 10.f;
  _model->rotVelocity()     = _model->rotVelocity() * powf(rot_vlcty_damping, timestep_sec) + _model->rotAcceleration() * timestep_sec;
  if (fabsf(_model->rotVelocity()) > max_rot_vlcty) {
    _model->rotVelocity() = copysignf(max_rot_vlcty, _model->rotVelocity());
  }
  _model->angle()          += _model->rotVelocity()     * timestep_sec;

  _model->angle() = fmodf(_model->angle(), 2.f * static_cast<float>(M_PI));

  _model->acceleration() = _model->acceleration() * powf(accel_damping, timestep_sec * 100.f) + _model->externalForce() + gravity;
  _model->velocity()    += _model->acceleration() * timestep_sec;
  _model->position()    += _model->velocity()     * timestep_sec;

  for (int i = 0; i < 3; i++) {
    if (fabs(_model->position()[i]) > _model->maxPosition()[i]) {
      _model->position()[i] = copysignf(_model->maxPosition()[i], _model->position()[i]);
      _model->velocity()[i] *= -exp(-l.game_model()->stickyWallTimer()) * vlcty_damping;
    }
  }

  return false;
}
