#include <memory>

#include "flappy_box/view/world_al_audible.hpp"

#include "sound_provider.hpp"

#include <AL/al.h>


using namespace view;
using namespace flappy_box::view;
using flappy_box::model::World;


WorldAlAudible::WorldAlAudible(const std::shared_ptr<const World> &w):
  _model(w)
{}


WorldAlAudible::~WorldAlAudible(void)
{
  if (bgm) {
    alDeleteSources(1, &bgm);
  }
}


void WorldAlAudible::auralize(AlRenderer &r)
{
  if (!bgm) {
    bgm = SoundProvider::getInstance()->playSound(r, "bg.wav", r.camera_position(), .3f, 1.f, true);
  }

  if (!_model->alive()) {
    alSourceStop(bgm);
    return;
  }

  if (!_model->remainingLives()) {
    if (!was_dead) {
      was_dead = true;
      alSourceRewind(bgm);
    }
  } else if (_model->gameRestarted()) {
    was_dead = false;
    alSourcePlay(bgm);
  }

  alSourcef(bgm, AL_PITCH, r.game_model()->gameSpeed());
}
