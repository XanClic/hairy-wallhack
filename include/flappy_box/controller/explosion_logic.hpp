#ifndef FLAPPY_BOX__CONTROLLER__EXPLOSION_LOGIC_HPP
#define FLAPPY_BOX__CONTROLLER__EXPLOSION_LOGIC_HPP

#include <memory>

#include "controller/logic.hpp"
#include "flappy_box/model/explosion.hpp"

namespace flappy_box
{
  namespace controller
  {
    class ExplosionLogic: public ::controller::Logic::ObjectLogic {
      public:
        ExplosionLogic(const std::shared_ptr<model::Explosion> &);

        virtual bool advance(::controller::Logic &, const ::controller::InputEventHandler::keyboard_event &) override;

      private:
        model::Explosion *_model;
    };
  }
}

#endif
