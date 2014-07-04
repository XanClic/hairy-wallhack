#ifndef FLAPPY_BOX__MODEL__GAME_OVER_HPP
#define FLAPPY_BOX__MODEL__GAME_OVER_HPP

#include <string>

#include "model/game_object.hpp"


namespace flappy_box
{
  namespace model
  {
    class GameOver: public ::model::GameObject {
      public:
        GameOver(const std::string &name, int init_points);

        int playerPoints(void) const { return _playerPoints; }


      private:
        const int _playerPoints;
    };
  }
}

#endif
