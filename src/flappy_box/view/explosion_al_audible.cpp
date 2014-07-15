#include "math.hpp"
#include "sound_provider.hpp"

#include "flappy_box/view/explosion_al_audible.hpp"


using namespace view;
using namespace flappy_box::view;
using flappy_box::model::Explosion;


ExplosionAlAudible::ExplosionAlAudible(const std::shared_ptr<const Explosion> &x):
  _model(x.get())
{}


void ExplosionAlAudible::auralize(AlRenderer &r)
{
  if (newfag) {
    newfag = false;

    const char *file;
    float pitch, gain;

    switch (_model->type()) {
      case Explosion::BOX_BOX_COLLISION:
        file  = "crash.wav";
        pitch = 1.7f;
        gain  = 40.f;
        break;

      case Explosion::BOX_FLOOR_CRASH:
        file  = "crash.wav";
        pitch = .8f;
        gain  = 150.f;
        break;

      case Explosion::BOX_EVAPORATE:
        return;

      case Explosion::GOOD_POWER_UP_COLLECTED:
        file  = "bling_high.wav";
        pitch = 1.f;
        gain  = 200.f;
        break;

      case Explosion::BAD_POWER_UP_COLLECTED:
        file  = "bling_low.wav";
        pitch = 1.f;
        gain  = 200.f;
        break;
    }

    SoundProvider::getInstance()->playSound(r, file, r.spherical_projection(_model->initial_position(), _model->max_position()), gain, pitch, false);
  }
}
