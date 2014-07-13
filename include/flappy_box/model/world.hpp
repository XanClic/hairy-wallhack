#ifndef FLAPPY_BOX__MODEL__WORLD_HPP
#define FLAPPY_BOX__MODEL__WORLD_HPP

#include <string>

#include "math.hpp"
#include "model/game_object.hpp"


namespace flappy_box
{
  namespace model
  {
    class World: public ::model::GameObject {
      public:
        World(const std::string &name);

        int &playerPoints(void) { return _playerPoints; }
        int playerPoints(void) const { return _playerPoints; }

        int &remainingLives(void) { return _remainingLives; }
        int remainingLives(void) const { return _remainingLives; }

        distance_type getWorldHalfHeight(void) const { return 50.f; }
        distance_type getWorldHalfWidth(void) const { return 80.f; }

        bool gameRestarted(void) const { return game_restarted; }
        bool &gameRestarted(void) { return game_restarted; }


      private:
        int _playerPoints   = 0;
        int _remainingLives = 5;
        bool game_restarted = false;
    };
  }
}

#endif
