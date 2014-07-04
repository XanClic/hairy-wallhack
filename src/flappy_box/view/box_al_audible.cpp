# include "flappy_box/view/box_al_audible.hpp"

# include <AL/alut.h>
# include <thread>
# include <functional>

# include "math.hpp"

# include "sound_provider.hpp"

using namespace ::flappy_box::view;
 

BoxAlAudible::BoxAlAudible( std::shared_ptr< ::flappy_box::model::Box > const& b )
: _model( b )
{
  // normalized x-coordinate of box
  double x_pos = b->position()[0] / b->maxPosition()[0];
  
  // play the sound
  SoundProvider::getInstance()->playSound("birth.wav", x_pos, 0.2, 1.0, false);
}


void BoxAlAudible::auralize( ::view::AlRenderer& renderer )
{
  if (  !_model->isAlive() )
  {
    // normalized x-coordinate of box
    double x_pos = _model->position()[0] / _model->maxPosition()[0];
    SoundProvider::getInstance()->playSound("crash.wav", x_pos, 0.5, 1.0, false);
  }
  
}
