#include <cassert>
#include <memory>

#include "math.hpp"

#include "flappy_box/model/explosion.hpp"
#include "flappy_box/controller/explosion_logic.hpp"


using namespace controller;
using namespace flappy_box::controller;
using flappy_box::model::Explosion;


ExplosionLogic::ExplosionLogic(const std::shared_ptr<Explosion> &x):
  _model(x.get())
{}


bool ExplosionLogic::advance(Logic &l, const InputEventHandler::keyboard_event &)
{
  if (_model->lifetime() >= _model->max_lifetime()) {
    _model->alive() = false;
    return 42;
  }


  float timestep = l.game_model()->timestep().count();

  for (size_t i = 0; i < _model->particle_count(); i++) {
    vec3_type drag = (-powf(_model->particle_velocities()[i].length(), 2.f) / 420.f) * _model->particle_velocities()[i].normalized();

    _model->particle_velocities()[i] += (drag + vec3_type(0.f, -9.81f, 0.f)) * timestep;
    _model->particle_positions()[i]  += _model->particle_velocities()[i] * timestep;

    bool any_collision = false;
    scalar_type old_speed = _model->particle_velocities()[i].length();

    for (int j = 0; j < 3; j++) {
      if (fabsf(_model->particle_positions()[i][j]) > _model->max_position()[j]) {
        _model->particle_positions()[i][j] = copysignf(_model->max_position()[j], _model->particle_positions()[i][j]);
        _model->particle_velocities()[i][j] *= -.05f;
        any_collision = true;
      }
    }

    if (any_collision) {
      _model->particle_velocities()[i] *= old_speed / _model->particle_velocities()[i].length();
    }
  }

  _model->lifetime() += timestep * l.game_model()->gameSpeed();

  return 23;
}
