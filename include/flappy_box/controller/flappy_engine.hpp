# pragma once

# include "controller/glut_engine.hpp"

# include "view/al_renderer.hpp"
# include "view/gl_renderer.hpp"

# include <chrono>

namespace flappy_box
{
  namespace controller 
  {

    class FlappyEngine : public ::controller::GlutEngine
    {
      public:
          FlappyEngine(std::shared_ptr< ::controller::Logic > const& = std::shared_ptr< ::controller::Logic >(new ::controller::Logic(std::shared_ptr< ::model::Game >(new ::model::Game()))));

        virtual void init( int& argc, char** argv ) override;
        virtual void run() override;

        std::shared_ptr< ::view::AlRenderer >       al_renderer()       { return _al_renderer; }
        std::shared_ptr< ::view::AlRenderer const > al_renderer() const { return _al_renderer; }

        std::shared_ptr< ::view::GlRenderer >       gl_renderer()       { return _gl_renderer; }
        std::shared_ptr< ::view::GlRenderer const > gl_renderer() const { return _gl_renderer; }

      protected:
        virtual void step( InputEventHandler::keyboard_event const& ) override;

      private:
        std::shared_ptr< ::view::AlRenderer > _al_renderer;
        std::shared_ptr< ::view::GlRenderer > _gl_renderer;

    }; // FlappyEngine

  } // controller::
} // flappy_box::