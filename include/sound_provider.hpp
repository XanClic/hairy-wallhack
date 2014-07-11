#ifndef SOUND_PROVIDER_HPP
#define SOUND_PROVIDER_HPP

#include <AL/alut.h>
#include <map>

#include "math.hpp"

#include "view/al_renderer.hpp"

class SoundProvider
{
  public:
    // returns singleton
    static SoundProvider *getInstance(void);
    // convinience method, will generate buffer from filename, creates source and immediately starts to play
    // also sets the source properties gain, position, pitch
    // if looping flag is not set it destroys the source after sound is played
    // returns handle to sound source
    ALuint playSound(view::AlRenderer &, const char *filename, const vec3_type &pos, scalar_type gain, scalar_type pitch, bool loop);

  private:
    SoundProvider(void);

    // returns OpenAl buffer that corresponds to filename (will create buffer if called for the first time)
    void getAlBuffer(const char *filename, ALuint *buffer);

    std::map<const char *, ALuint> _buffers;
    static SoundProvider *inst;
};

#endif
