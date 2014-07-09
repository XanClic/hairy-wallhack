# pragma once

# include "controller/engine.hpp"

# include <chrono>

namespace controller 
{

  class GlutEngine : public Engine
  {
    public:
        GlutEngine(const std::shared_ptr< Logic >& = std::shared_ptr< ::controller::Logic >(new ::controller::Logic(std::shared_ptr< ::model::Game >(new ::model::Game()))));

      virtual void init( int& argc, char** argv ) override;
      virtual void run() override;
    protected:
      // time interval in which the (virtual) Engine::step function should be called.
      // I HAET U
      unsigned int _prefered_timestep_millisec = 1000 / 60;

  }; // GlutEngine

} // controller::
