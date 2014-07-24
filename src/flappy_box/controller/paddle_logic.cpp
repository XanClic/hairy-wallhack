#include "math.hpp"
#include "flappy_box/controller/paddle_logic.hpp"

#include <GL/glut.h>


static const scalar_type player_accel_scale = 1000.f;
static const scalar_type accel_damping = .0f;
static const scalar_type vlcty_damping = .01f;


using namespace controller;
using namespace flappy_box::controller;


PaddleLogic::PaddleLogic(const std::shared_ptr<flappy_box::model::Paddle> &paddle_ptr):
  _model(paddle_ptr.get())
{}


bool PaddleLogic::advance(Logic &l, const InputEventHandler::keyboard_event &evt)
{
  scalar_type timestep_sec = l.game_model()->timestep().count();

  if (evt.special_key == GLUT_KEY_RIGHT) {
    right_down = evt.key_state == InputEventHandler::keyboard_event::KEY_DOWN;
  } else if (evt.special_key == GLUT_KEY_LEFT) {
    left_down = evt.key_state == InputEventHandler::keyboard_event::KEY_DOWN;
  }

  if (left_down == right_down) {
    _model->playerControl().x() =  0.f;
  } else if (left_down) {
    _model->playerControl().x() = -1.f;
  } else if (right_down) {
    _model->playerControl().x() =  1.f;
  }

  _model->acceleration() = _model->acceleration() * powf(accel_damping, timestep_sec) + _model->playerControl() * player_accel_scale;

  _model->velocity() = _model->velocity() * powf(vlcty_damping, timestep_sec) + _model->acceleration() * timestep_sec;
  if (_model->velocity().length() > _model->maxVelocity()) {
    _model->velocity() *= _model->maxVelocity() / _model->velocity().length();
  }

  _model->position() += _model->velocity() * timestep_sec;
  for (int i = 0; i < 3; i++) {
    if (fabs(_model->position()[i]) > _model->maxPosition()[i]) {
      _model->position()[i] = copysign(_model->maxPosition()[i], _model->position()[i]);
      _model->velocity()[i] *= -1.f;
    }
  }

  for (scalar_type *scale: {&_model->scale(), &_model->relativeFanPower()}) {
    if (*scale < 1.f) {
      *scale *= exp(timestep_sec / 10.f);

      if (*scale > 1.f) {
        *scale = 1.f;
      }
    }
  }

  return false;
}
