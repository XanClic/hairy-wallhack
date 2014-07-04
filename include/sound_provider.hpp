# pragma once

# include <AL/alut.h>
# include <map>

class SoundProvider
{
  public:
  
    // returns singleton
    static SoundProvider * getInstance();
    // convinience method, will generate buffer from filename, creates source and immediately starts to play
    // also sets the source properties gain, x-position, pitch
    // if looping flag is not set it destroys the source after sound is played
    // returns handle to sound source
    ALuint playSound( const char * filename, double x_position, double gain, double pitch, bool loop );
  
  private:    
    
    SoundProvider();
    
    // returns OpenAl buffer that corresponds to filename ( will create buffer if called for the first time)
    void getAlBuffer(const char * filename, ALuint * buffer);
    
    std::map<const char*, ALuint > _buffers;
    static SoundProvider * _theInstance; 
};