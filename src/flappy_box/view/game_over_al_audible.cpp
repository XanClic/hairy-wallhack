#include <memory>

#include "flappy_box/view/game_over_al_audible.hpp"

#include "sound_provider.hpp"

#include <AL/al.h>


using namespace view;
using namespace flappy_box::view;
using flappy_box::model::GameOver;


GameOverAlAudible::GameOverAlAudible(const std::shared_ptr<const GameOver> &go):
  _model(go)
{
  bgm = SoundProvider::getInstance()->playSound("game-over.wav", 0.f, .3f, 1.f, true);
}


GameOverAlAudible::~GameOverAlAudible(void)
{
  alDeleteSources(1, &bgm);
}


void GameOverAlAudible::auralize(AlRenderer &)
{
  // (╯°□°）╯︵ ┻━┻
  if (!_model) {
    return;
  }

  if (!_model->alive()) {
    alSourceStop(bgm);
    _model = nullptr;
    return;
  }
}
