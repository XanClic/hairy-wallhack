#include "flappy_box/view/paddle_al_audible.hpp"

#include <AL/alut.h>

#include "math.hpp"
#include "sound_provider.hpp"


using namespace view;
using namespace flappy_box::view;
using flappy_box::model::Paddle;


PaddleAlAudible::PaddleAlAudible(const std::shared_ptr<const Paddle> &p):
  _model(p)
{
  buf = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 150.f, 0.f, 1.f);

  alGenSources(1, &src);
  alSourcei(src, AL_BUFFER, buf);
  alSourcei(src, AL_LOOPING, true);
  alSourcef(src, AL_GAIN, 0.f);

  alSourcePlay(src);
}


PaddleAlAudible::~PaddleAlAudible(void)
{
  alDeleteSources(1, &src);
  alDeleteBuffers(1, &buf);
}


void PaddleAlAudible::auralize(AlRenderer &)
{
  if (!_model->alive()) {
    alSourceStop(src);
    return;
  }

  // Normalizing to [(-1, 0, 0), (1, 0, 0)] is wrong and dumb; the reason why
  // OpenAL exists is so you can give coordinates in 3D space. Just giving the
  // X axis results in having either left or right speaker on full output.

  // FIXME: camera position is unknown
  alSource3f(src, AL_POSITION, _model->position().x(), _model->position().y(), 100.f);

  float normalized_speed = fabsf(_model->velocity().x()) / _model->maxVelocity();
  alSourcef(src, AL_GAIN, normalized_speed * 50.f);
  alSourcef(src, AL_PITCH, normalized_speed + .5f);
}
