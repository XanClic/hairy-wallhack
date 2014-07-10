#ifndef FLAPPY_BOX__VIEW__GAME_OVER_AL_AUDIBLE_HPP
#define FLAPPY_BOX__VIEW__GAME_OVER_AL_AUDIBLE_HPP

#include <memory>

#include <AL/al.h>

#include "view/al_renderer.hpp"
#include "flappy_box/model/game_over.hpp"

namespace flappy_box
{
  namespace view
  {
    class GameOverAlAudible: public ::view::AlRenderer::Audible {
      public:
        GameOverAlAudible(const std::shared_ptr<const model::GameOver> &);
        virtual ~GameOverAlAudible(void);

        virtual void auralize(::view::AlRenderer &) override;

      private:
        std::shared_ptr<const model::GameOver> _model;
        ALuint bgm;
    };
  }
}

#endif
