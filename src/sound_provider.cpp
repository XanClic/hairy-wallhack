#include "sound_provider.hpp"
#include <iostream>

#include <thread>
#include <functional>

#include <string>
#include <cstdio>

#include "view/al_renderer.hpp"


SoundProvider *SoundProvider::inst = nullptr;


// loads wave file, creates buffer and source
// the sound will be played immediately with no repeat
// after playing the resources will be freed
void check_state_thread(ALuint source, ALint state) // <-- vorgeben?
{
  // continously recheck the source state
  for (;;) {
    // be aware of busy waiting, so give control to other threads
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (state == AL_PLAYING) {
      alGetSourcei(source, AL_SOURCE_STATE, &state);
    } else if (state == AL_STOPPED) {
      // free source
      alDeleteSources(1, &source);
      // std::clog<< alutGetErrorString(alutGetError())<<" - DELETED SOURCE"<<std::endl;;
      return;
    }
  }
};


SoundProvider *SoundProvider::getInstance(void)
{
  if (!inst) {
    inst = new SoundProvider;
  }

  return inst;
}


SoundProvider::SoundProvider(void)
{
  // destroy all the buffers
  for (auto b: _buffers) {
    alDeleteBuffers(1, &b.second);
  }
}


void SoundProvider::getAlBuffer(const char *filename, ALuint *buffer)
{
  // try to find buffer in the map
  std::map<const char *, ALuint>::iterator found = _buffers.find(filename);

  if (found != _buffers.end()) {
    // if we found corresponding buffer write it
    *buffer = (*found).second;
  } else {
    // if we did not found it we insert
    ALuint newBuffer;
    alGenBuffers(1, &newBuffer);

    // search in subdirs for folder "res"
    std::string path = "res/";
    for (int i = 0; i < 4; i++) {
      FILE *f = fopen((path + filename).c_str(), "r");
      if (f) {
        fclose(f);
        break;
      }
      path = "../" + path;
    }

    newBuffer = alutCreateBufferFromFile((path + filename).c_str());
    if (newBuffer) {
      std::cout << "Successfully loaded " << filename << std::endl;
      _buffers.insert(std::pair<const char *, ALuint>(filename, newBuffer));
      *buffer = newBuffer;
    } else {
      std::cerr << "Error on loading file " << filename << ": " << alutGetErrorString(alutGetError()) << std::endl;
    }
  }
}


ALuint SoundProvider::playSound(view::AlRenderer &r, const char *filename, const vec3_type &pos, scalar_type gain, scalar_type pitch, bool loop)
{
  ALuint buf;
  ALenum err = alGetError();
  if (err != ALC_NO_ERROR) {
      std::cerr << "Error before receiving sound buffer, error: " << alutGetErrorString(alutGetError()) << std::endl;
      return 0;
  }

  getAlBuffer(filename, &buf);

  err = ::alGetError();
  if (err != ALC_NO_ERROR) {
      std::cerr << "Error before generating sound source, error: " << alutGetErrorString(alutGetError()) << std::endl;
      return 0;
  }

  ALuint source;
  alGenSources(1, &source);
  err = ::alGetError();
  if (err != ALC_NO_ERROR) {
      std::cerr << "Error generating sound source, error: " << alutGetErrorString(alutGetError()) << std::endl;
      return 0;
  }

  alSourcei(source, AL_BUFFER, buf);

  // set source props
  vec3_type p = pos - r.camera_position();
  alSourcefv(source, AL_POSITION, p);
  alSourcef(source, AL_GAIN, gain);
  alSourcef(source, AL_PITCH, pitch);
  alSourcei(source, AL_LOOPING, loop);

  // play the source
  alSourcePlay(source);

  // observe the source state
  // FIXME: haha race condition
  ALint state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  if (state != AL_PLAYING) {
    std::clog << "Error on playing sound, state = " << state << ", error: " << alutGetErrorString(alutGetError()) << std::endl;
  }

  if (!loop) {
    // continously recheck the source state in separate thread
    std::thread(std::bind(check_state_thread, source, state)).detach();
  }

  return source;
}
