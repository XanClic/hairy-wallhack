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
    SoundProvider::getInstance()->playSound(r, "crash.wav", _model->initial_position(), 40.f, 1.f, false);
    newfag = false;
  }
}
