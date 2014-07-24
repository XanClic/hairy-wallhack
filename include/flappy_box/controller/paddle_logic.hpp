#ifndef FLAPPY_BOX__CONTROLLER__PADDLE_LOGIC_HPP
#define FLAPPY_BOX__CONTROLLER__PADDLE_LOGIC_HPP

#include <memory>

#include "controller/logic.hpp"
#include "flappy_box/model/paddle.hpp"

namespace flappy_box
{
  namespace controller
  {
    class PaddleLogic: public ::controller::Logic::ObjectLogic {
      public:
        PaddleLogic(const std::shared_ptr<model::Paddle> &);

        virtual bool advance(::controller::Logic &, const ::controller::InputEventHandler::keyboard_event &) override;

      private:
        model::Paddle *_model;
        bool left_down = false, right_down = false;
    };
  }
}

#endif
