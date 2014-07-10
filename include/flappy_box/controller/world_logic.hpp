#ifndef FLAPPY_BOX__CONTROLLER__WORLD_LOGIC_HPP
#define FLAPPY_BOX__CONTROLLER__WORLD_LOGIC_HPP

#include <memory>
#include <random>

#include "math.hpp"

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
        std::default_random_engine rng;

        scalar_type add_box_interval_timer = 0.f;
        int box_count = 0;

        void addBoxToGame(::controller::Logic &l);
        void setForce(std::shared_ptr<model::Box> &box, std::shared_ptr<model::Paddle> &paddle);
        void restartGame(::controller::Logic &l);
    };
  }
}

#endif
