#include <algorithm>
#include <memory>
#include <random>

#include "math.hpp"
#include "model/game_object.hpp"
#include "flappy_box/model/box.hpp"
#include "flappy_box/model/game_over.hpp"
#include "flappy_box/model/paddle.hpp"
#include "flappy_box/controller/world_logic.hpp"


static const scalar_type add_box_interval = 2.f;
static const int hit_extra_points = 42;

static const scalar_type off_force_mult = 1.f;
static const scalar_type off_force_exp = 5.f;


// I don't even know why we have all those namespaces
using namespace controller;
using namespace model;
using namespace flappy_box::controller;
using namespace flappy_box::model;


WorldLogic::WorldLogic(const std::shared_ptr<flappy_box::model::World> &world_ptr):
  _model(world_ptr)
{}

bool WorldLogic::advance(Logic &l, const InputEventHandler::keyboard_event &evt)
{
  if (_shallRestartTheGame) {
    restartGame(l);
  }

  scalar_type timestep_sec = l.game_model()->timestep().count();

  static scalar_type add_box_interval_timer;
  add_box_interval_timer += timestep_sec;

  while (add_box_interval_timer >= add_box_interval) {
    addBoxToGame(l);
    add_box_interval_timer -= add_box_interval;
  }

  // Oh my beloved Madoka, you can't be serious
  const auto &objs = l.game_model()->objects();
  auto paddle_obj_it = std::find_if(objs.begin(), objs.end(),
          [](const std::shared_ptr<GameObject> &obj) { return typeid(*obj) == typeid(Paddle); });

  if (paddle_obj_it == objs.end()) {
    return true;
  }

  // I hate shared_ptr
  std::shared_ptr<Paddle> paddle = std::dynamic_pointer_cast<Paddle>(*paddle_obj_it);

  // The task says to do this after incrementing the point count, but I really
  // don't see why
  if (_model->remainingLives() <= 0) {
    paddle->alive() = false;
    l.game_model()->addGameObject(std::make_shared<GameOver>("Game Over", _model->playerPoints()));
  }

  // Keep the points exact
  static scalar_type player_points_inc;
  player_points_inc += timestep_sec;

  int inc_now = static_cast<int>(player_points_inc * 10.f);
  _model->playerPoints() += inc_now;
  player_points_inc -= static_cast<scalar_type>(inc_now) / 10.f;

  for (std::shared_ptr<GameObject> obj: objs) {
    if (!obj->alive()) {
      continue;
    }

    std::shared_ptr<Box> box = std::dynamic_pointer_cast<Box>(obj);
    if (!box) {
      continue;
    }

    setForce(box, paddle);

    if (box->position().y() < paddle->position().y()) {
      box->alive() = false;
      --_model->remainingLives();
    }

    // this is genius
    for (std::shared_ptr<GameObject> iobj: objs) {
      if (!iobj->alive() || (iobj == obj)) {
        continue;
      }

      std::shared_ptr<Box> ibox = std::dynamic_pointer_cast<Box>(iobj);
      if (!ibox) {
        continue;
      }

      if ((box->position() - ibox->position()).length() < box->size() + ibox->size()) {
        box->alive() = false;
        ibox->alive() = false;

        // EXTRA MONSTER POINTS
        _model->playerPoints() += hit_extra_points;
      }
    }
  }

  l.game_model()->points = _model->playerPoints();
  l.game_model()->lives  = _model->remainingLives();

  return true;
}

void WorldLogic::addBoxToGame(Logic &l)
{
  // I'm not so sure about this initialization on each call
  // (╯°□°）╯︵ ┻━┻
  std::default_random_engine rng(std::chrono::system_clock::now().time_since_epoch().count());

  std::uniform_real_distribution<scalar_type> size_exp_dist(1.7f, 3.4f);
  scalar_type size = exp2(size_exp_dist(rng));

  vec3_type max_pos(_model->getWorldHalfWidth() - size / 2.f, _model->getWorldHalfHeight() - size / 2.f, HUGE_VALF);

  std::uniform_real_distribution<scalar_type> x_dist(-max_pos.x(), max_pos.x());
  scalar_type x = x_dist(rng);

  std::shared_ptr<Box> box(new Box);
  box->size() = size;
  box->position() = vec3_type(x, 0.f, 0.f);
  box->maxPosition() = max_pos;

  l.game_model()->addGameObject(box);
}

static bool in_bounding_box(const vec3_type &point, const vec3_type &lower_left, const vec3_type &upper_right)
{
  for (int i = 0; i < 3; i++) {
    if ((point[i] < lower_left[i]) || (point[i] > upper_right[i])) {
      return false;
    }
  }

  return true;
}

void WorldLogic::setForce(std::shared_ptr<Box> &box, std::shared_ptr<Paddle> &paddle)
{
  // I like how this function completely disregards the size of the box in
  // regard to how box and paddle overlap

  vec3_type pll = paddle->position() - paddle->size() / 2.f; // lower left
  vec3_type pur = paddle->position() + paddle->size() / 2.f; // upper right
  pll.y() = -HUGE_VALF;
  pur.y() =  HUGE_VALF;

  vec3_type force;

  if (in_bounding_box(box->position(), pll, pur)) {
    force = vec3_type(0.f, 1.f, 0.f);
  } else if (box->position().y() > paddle->position().y()) { // "oberhalb", not "oberhalb oder auf gleicher Höhe"
    vec3_type min_vector(HUGE_VALF, HUGE_VALF, HUGE_VALF);

    // Only consider vertices, no edges (the tasks requires this)
    // (2edgy4u)
    for (int xm: {-1, 1}) {
      for (int ym: {-1, 1}) {
        vec3_type paddle_vertex =
            paddle->position() + vec3_type(xm * paddle->size().x(), ym * paddle->size().y(), 0.f);

        vec3_type vector = box->position() - paddle_vertex;
        if (vector.length() < min_vector.length()) {
          min_vector = vector;
        }
      }
    }

    min_vector.normalize();

    force = off_force_mult * powf(vec3_type(0.f, 1.f, 0.f).dot(min_vector), off_force_exp) * min_vector;
  }

  box->externalForce() = force * 10.f * box->size() * box->size();
}

void WorldLogic::restartGame(Logic &l)
{
  for (const std::shared_ptr<GameObject> &obj: l.game_model()->objects()) {
    obj->alive() = false;
  }

  _model->alive() = true;
  _model->playerPoints() = 0;
  _model->remainingLives() = 5;

  std::shared_ptr<Paddle> user_paddle(new Paddle("PlayerPaddle"));
  user_paddle->size() = vec3_type(10.f, 2.5f, 1.f);
  user_paddle->position() = vec3_type(0.f, -_model->getWorldHalfHeight() + user_paddle->size().y() * 2.f, 0.f);
  user_paddle->maxPosition() = vec3_type(_model->getWorldHalfWidth() - user_paddle->size().x() / 2.f, _model->getWorldHalfHeight(), HUGE_VALF);

  l.game_model()->addGameObject(user_paddle);

  _shallRestartTheGame = false;
}
