#include <memory>

#include "flappy_box/view/game_over_al_audible.hpp"

#include "sound_provider.hpp"

#include <AL/al.h>


using namespace view;
using namespace flappy_box::view;
using flappy_box::model::GameOver;


GameOverAlAudible::GameOverAlAudible(const std::shared_ptr<const GameOver> &go):
  _model(go.get())
{}


GameOverAlAudible::~GameOverAlAudible(void)
{
  if (bgm) {
    alDeleteSources(1, &bgm);
  }
}


void GameOverAlAudible::auralize(AlRenderer &r)
{
  if (!bgm) {
    bgm = SoundProvider::getInstance()->playSound(r, "game-over.wav", r.camera_position(), .3f, 1.f, true);
  }

  if (!_model->alive()) {
     alSourceStop(bgm);
  }
}
