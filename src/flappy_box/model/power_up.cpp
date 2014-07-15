#include <string>

#include "math.hpp"

#include "flappy_box/model/power_up.hpp"


using namespace model;
using namespace flappy_box::model;


PowerUp::PowerUp(PowerUp::Type tp, const vec3_type &p, const vec3_type &mp, const std::string &name):
  GameObject(true, name), t(tp), pos(p), max_pos(mp)
{}
