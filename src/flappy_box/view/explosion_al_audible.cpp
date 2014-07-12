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
    float pitch = 1.7f, gain = 40.f;

    if (_model->type() == Explosion::BOX_FLOOR_CRASH) {
      pitch = .8f;
      gain  = 150.f;
    }

    SoundProvider::getInstance()->playSound(r, "crash.wav", r.spherical_projection(_model->initial_position(), _model->max_position()), gain, pitch, false);
    newfag = false;
  }
}
