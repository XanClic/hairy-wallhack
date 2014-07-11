#ifndef FLAPPY_BOX__VIEW__EXPLOSION_AL_AUDIBLE_HPP
#define FLAPPY_BOX__VIEW__EXPLOSION_AL_AUDIBLE_HPP

#include <memory>

#include "view/al_renderer.hpp"
#include "flappy_box/model/explosion.hpp"

namespace flappy_box
{
  namespace view
  {
    class ExplosionAlAudible: public ::view::AlRenderer::Audible {
      public:
        ExplosionAlAudible(const std::shared_ptr<const model::Explosion> &);

        virtual void auralize(::view::AlRenderer &) override;

      private:
        std::shared_ptr<const model::Explosion> _model;
        bool newfag = true;
    };
  }
}

#endif
