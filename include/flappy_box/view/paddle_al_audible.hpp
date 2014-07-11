#ifndef FLAPPY_BOX__VIEW__PADDLE_AL_AUDIBLE_HPP
#define FLAPPY_BOX__VIEW__PADDLE_AL_AUDIBLE_HPP

#include <memory>

#include <AL/al.h>

#include "view/al_renderer.hpp"
#include "flappy_box/model/paddle.hpp"

namespace flappy_box
{
  namespace view
  {
    class PaddleAlAudible: public ::view::AlRenderer::Audible {
      public:
        PaddleAlAudible(const std::shared_ptr<const model::Paddle> &);
        virtual ~PaddleAlAudible(void);

        virtual void auralize(::view::AlRenderer &) override;

      private:
        const model::Paddle *_model;
        ALuint src, buf;
    };
  }
}

#endif
