# include "flappy_box/view/box_al_audible.hpp"

# include <AL/alut.h>
# include <thread>
# include <functional>

# include "math.hpp"

# include "sound_provider.hpp"

using namespace ::flappy_box::view;
 

BoxAlAudible::BoxAlAudible(const std::shared_ptr<::flappy_box::model::Box> &b):
  _model(b.get())
{}


void BoxAlAudible::auralize(::view::AlRenderer &r)
{
  if (newfag) {
    newfag = false;

    if (!_model->mute()) {
      SoundProvider::getInstance()->playSound(r, "birth.wav", r.spherical_projection(_model->position(), _model->maxPosition()), 30.f, 1.f, false);
    }
  }
}
