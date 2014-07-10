#include "math.hpp"
#include "flappy_box/controller/paddle_logic.hpp"

#include <GL/glut.h>


static const scalar_type player_accel_scale = 1000.f;
static const scalar_type accel_damping = .0f;
static const scalar_type vlcty_damping = .9f;


using namespace controller;
using namespace flappy_box::controller;


PaddleLogic::PaddleLogic(const std::shared_ptr<flappy_box::model::Paddle> &paddle_ptr):
  _model(paddle_ptr)
{}

bool PaddleLogic::advance(Logic &l, const InputEventHandler::keyboard_event &evt)
{
  scalar_type timestep_sec = l.game_model()->timestep().count();

  if (evt.special_key == GLUT_KEY_RIGHT) {
    _model->playerControl().x() = evt.key_state == InputEventHandler::keyboard_event::KEY_DOWN ?  1.f : 0.f;
  } else if (evt.special_key == GLUT_KEY_LEFT) {
    _model->playerControl().x() = evt.key_state == InputEventHandler::keyboard_event::KEY_DOWN ? -1.f : 0.f;
  }

  _model->acceleration() = _model->acceleration() * accel_damping + _model->playerControl() * player_accel_scale;

  _model->velocity() = _model->velocity() * vlcty_damping + _model->acceleration() * timestep_sec;
  if (_model->velocity().length() > _model->maxVelocity()) {
    _model->velocity() *= _model->maxVelocity() / _model->velocity().length();
  }

  _model->position() += _model->velocity() * timestep_sec;
  for (int i = 0; i < 3; i++) {
    if (fabs(_model->position()[i]) > _model->maxPosition()[i]) {
      _model->position()[i] = copysign(_model->maxPosition()[i], _model->position()[i]);
      _model->velocity()[i] *= -vlcty_damping; // could've been just -1.f here, but the task says "do it just like 5.2.1"
    }
  }

  return false;
}
