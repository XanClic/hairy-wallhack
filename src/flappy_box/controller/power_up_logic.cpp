#include "math.hpp"

#include "flappy_box/controller/power_up_logic.hpp"


using namespace controller;
using namespace flappy_box::controller;
using flappy_box::model::PowerUp;


PowerUpLogic::PowerUpLogic(const std::shared_ptr<PowerUp> &pu):
  _model(pu.get())
{}


bool PowerUpLogic::advance(Logic &l, const InputEventHandler::keyboard_event &)
{
  scalar_type timestep_sec = l.game_model()->timestep().count();

  _model->angle() += static_cast<float>(M_PI) * timestep_sec;
  while (_model->angle() >= 2 * static_cast<float>(M_PI)) {
    _model->angle() -= 2 * static_cast<float>(M_PI);
  }

  _model->position().z() += powf(l.game_model()->gameSpeed(), 3.f) * timestep_sec * 150.f;

  // FIXME (depends on camera)
  if (_model->position().z() > 50.f) {
    _model->alive() = false;
  }

  return 27;
}
