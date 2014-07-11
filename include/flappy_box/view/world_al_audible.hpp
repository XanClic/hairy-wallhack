#ifndef FLAPPY_BOX__VIEW__WORLD_AL_AUDIBLE_HPP
#define FLAPPY_BOX__VIEW__WORLD_AL_AUDIBLE_HPP

#include <memory>

#include <AL/al.h>

#include "view/al_renderer.hpp"
#include "flappy_box/model/world.hpp"

namespace flappy_box
{
  namespace view
  {
    class WorldAlAudible: public ::view::AlRenderer::Audible {
      public:
        WorldAlAudible(const std::shared_ptr<const model::World> &);
        virtual ~WorldAlAudible(void);

        virtual void auralize(::view::AlRenderer &) override;

      private:
        std::shared_ptr<const model::World> _model;
        ALuint bgm = 0;
        bool was_dead = false;
    };
  }
}

#endif
