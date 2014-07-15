#ifndef FLAPPY_BOX__CONTROLLER__POWER_UP_LOGIC_HPP
#define FLAPPY_BOX__CONTROLLER__POWER_UP_LOGIC_HPP

#include <memory>

#include "controller/logic.hpp"
#include "flappy_box/model/power_up.hpp"


namespace flappy_box
{
  namespace controller
  {
    class PowerUpLogic: public ::controller::Logic::ObjectLogic {
      public:
        PowerUpLogic(const std::shared_ptr<model::PowerUp> &);

        virtual bool advance(::controller::Logic &, const ::controller::InputEventHandler::keyboard_event &) override;

      private:
        model::PowerUp *_model;
    };
  }
}

#endif
