#ifndef FLAPPY_BOX__CONTROLLER__WORLD_LOGIC_HPP
#define FLAPPY_BOX__CONTROLLER__WORLD_LOGIC_HPP

#include <memory>

#include "controller/logic.hpp"
#include "flappy_box/model/box.hpp"
#include "flappy_box/model/paddle.hpp"
#include "flappy_box/model/world.hpp"

namespace flappy_box
{
  namespace controller
  {
    class WorldLogic: public ::controller::Logic::ObjectLogic {
      public:
        WorldLogic(const std::shared_ptr<model::World> &);

        virtual bool advance(::controller::Logic &, const ::controller::InputEventHandler::keyboard_event &) override;

      private:
        std::shared_ptr<model::World> _model;
        bool _shallRestartTheGame = true;

        void addBoxToGame(::controller::Logic &l);
        void setForce(std::shared_ptr<model::Box> &box, std::shared_ptr<model::Paddle> &paddle);
        void restartGame(::controller::Logic &l);
    };
  }
}

#endif
