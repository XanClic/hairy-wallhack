#include "flappy_box/model/box.hpp"

using namespace flappy_box::model;

Box::Box(const std::string &n, bool m):
  GameObject(true, n), is_mute(m)
{}
