#include <string>
#include "flappy_box/model/game_over.hpp"


using namespace model;
using namespace flappy_box::model;


GameOver::GameOver(const std::string &name, int init_points):
  GameObject(false, name),
  _playerPoints(init_points)
{}
