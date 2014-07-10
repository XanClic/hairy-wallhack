#include <memory>

#include "flappy_box/view/world_al_audible.hpp"

#include "sound_provider.hpp"

#include <AL/al.h>


using namespace view;
using namespace flappy_box::view;
using flappy_box::model::World;


WorldAlAudible::WorldAlAudible(const std::shared_ptr<const World> &w):
  _model(w)
{
  bgm = SoundProvider::getInstance()->playSound("bg.wav", 0.f, .3f, 1.f, true);
}


WorldAlAudible::~WorldAlAudible(void)
{
  alDeleteSources(1, &bgm);
}


void WorldAlAudible::auralize(AlRenderer &)
{
  if (!_model->alive()) {
    alSourceStop(bgm);
    return;
  }

  if (!_model->remainingLives()) {
    if (!was_dead) {
      was_dead = true;
      alSourceRewind(bgm);
    }
  } else if (was_dead) {
    was_dead = false;
    alSourcePlay(bgm);
  }

  alSourcef(bgm, AL_PITCH, _model->gameSpeed());
}
