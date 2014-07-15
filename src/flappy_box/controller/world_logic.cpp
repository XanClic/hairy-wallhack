#include <algorithm>
#include <chrono>
#include <memory>
#include <random>

#include "math.hpp"
#include "model/game_object.hpp"
#include "flappy_box/model/box.hpp"
#include "flappy_box/model/explosion.hpp"
#include "flappy_box/model/game_over.hpp"
#include "flappy_box/model/paddle.hpp"
#include "flappy_box/model/power_up.hpp"
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
  _model(world_ptr), rng(std::chrono::system_clock::now().time_since_epoch().count())
{}


// I love this function because it shows how bad this whole engine model is
// (this class basically does all the physics calculations, so we could have
// put everything in a single big loop anyway)
bool WorldLogic::advance(Logic &l, const InputEventHandler::keyboard_event &evt)
{
  _model->gameRestarted() = false;

  if (evt.key == 'r') {
    _shallRestartTheGame = true;
  }

  if (_shallRestartTheGame) {
    restartGame(l);
  }


  // Oh my beloved Madoka, you can't be serious
  const auto &objs = l.game_model()->objects();
  auto paddle_obj_it = std::find_if(objs.begin(), objs.end(),
          [](const std::shared_ptr<GameObject> &obj) { return typeid(*obj) == typeid(Paddle); });

  std::shared_ptr<Paddle> paddle(nullptr);

  if (paddle_obj_it != objs.end()) {
    paddle = std::dynamic_pointer_cast<Paddle>(*paddle_obj_it);

    if (!paddle->alive()) {
      paddle = nullptr;
    }
  }


  // paddle == nullptr means Game Over


  scalar_type timestep_sec = l.game_model()->timestep().count();

  static scalar_type add_box_interval_timer;
  add_box_interval_timer += timestep_sec;

  float true_add_box_interval = add_box_interval * powf(l.game_model()->gameSpeed(), -8.f);
  if (true_add_box_interval > add_box_interval) {
    true_add_box_interval = add_box_interval;
  }

  while (add_box_interval_timer >= true_add_box_interval) {
    if (paddle || (box_count < 20)) {
      addBoxToGame(l);
    }
    add_box_interval_timer -= true_add_box_interval;
  }


  if (isnan(next_powerup_timer) || (next_powerup_timer <= 0.f)) {
    if (paddle && !isnan(next_powerup_timer)) {
      bool generate_bad = std::uniform_int_distribution<int>(0, 2)(rng) != 0;
      std::uniform_int_distribution<int> t_dist;
      if (generate_bad) {
        t_dist = std::uniform_int_distribution<int>(PowerUp::BAD_TYPE_START, PowerUp::BAD_TYPE_END);
      } else {
        t_dist = std::uniform_int_distribution<int>(PowerUp::GOOD_TYPE_START, PowerUp::GOOD_TYPE_END);
      }

      PowerUp::Type t = static_cast<PowerUp::Type>(t_dist(rng));

      std::uniform_real_distribution<float> x_dist(-_model->getWorldHalfWidth() + 5.f, _model->getWorldHalfWidth() - 5.f);

      l.game_model()->addGameObject(std::make_shared<PowerUp>(t,
                                                              vec3_type(x_dist(rng), paddle->position().y(), -400.f),
                                                              vec3_type(_model->getWorldHalfWidth(), _model->getWorldHalfHeight(), HUGE_VALF)));
    }

    std::uniform_real_distribution<scalar_type> nput_dist(7.f, 10.f);
    next_powerup_timer = powf(l.game_model()->gameSpeed(), -7.f) * nput_dist(rng);
  }

  if (paddle) {
    next_powerup_timer -= timestep_sec;
  }


  if (l.game_model()->stickyWallTimer() > 0.f) {
    l.game_model()->stickyWallTimer() -= timestep_sec;

    if (l.game_model()->stickyWallTimer() < 0.f) {
      l.game_model()->stickyWallTimer() = 0.f;
    }
  }


  // The task says to do this after incrementing the point count, but I really
  // don't see why
  if ((_model->remainingLives() <= 0) && paddle) {
    paddle->alive() = false;
    for (const std::shared_ptr<GameObject> &obj: objs) {
      if (typeid(*obj) == typeid(PowerUp)) {
        obj->alive() = false;
      }
    }

    l.game_model()->addGameObject(std::make_shared<GameOver>("Game Over", _model->playerPoints()));
  }


  float old_player_points = _model->playerPoints();

  // Keep the points exact
  static scalar_type player_points_inc;

  if (paddle) {
    player_points_inc += timestep_sec;

    int inc_now = static_cast<int>(player_points_inc * 10.f);
    _model->playerPoints() += inc_now;
    player_points_inc -= static_cast<scalar_type>(inc_now) / 10.f;
  }

  for (std::shared_ptr<GameObject> obj: objs) {
    if (!obj->alive()) {
      continue;
    }

    std::shared_ptr<Box> box = std::dynamic_pointer_cast<Box>(obj);
    if (!box) {
      // haha this nesting is so ugly (but it's not my fault)
      std::shared_ptr<PowerUp> pu = std::dynamic_pointer_cast<PowerUp>(obj);
      if (pu && paddle) {
        if ((pu->position() - paddle->position()).length() <= (paddle->size().x() + 5.f)) {
          Explosion::Type xt;

          switch (pu->type()) {
            case PowerUp::LIFE:
              ++_model->remainingLives();
              xt = Explosion::GOOD_POWER_UP_COLLECTED;
              break;

            case PowerUp::STICKY_WALLS:
              l.game_model()->stickyWallTimer() = 10.f;
              xt = Explosion::GOOD_POWER_UP_COLLECTED;
              break;

            case PowerUp::SLOW_GAME:
              l.game_model()->gameSpeed() /= 1.05f;
              xt = Explosion::GOOD_POWER_UP_COLLECTED;
              break;

            case PowerUp::EVAPORATE_BOXES:
              // FIXME: Somehow I don't like this... But it will probably work.
              for (std::shared_ptr<GameObject> pobj: objs) {
                if (!pobj->alive()) {
                  continue;
                }

                std::shared_ptr<Box> puff_the_magic_box = std::dynamic_pointer_cast<Box>(pobj);
                if (puff_the_magic_box) {
                  l.game_model()->addGameObject(std::make_shared<Explosion>(Explosion::BOX_EVAPORATE, *puff_the_magic_box));

                  puff_the_magic_box->alive() = false;
                  box_count--;
                }
              }
              xt = Explosion::GOOD_POWER_UP_COLLECTED;
              break;

            case PowerUp::PADDLE_SMALLER:
              // I'd love to directly influence the model, but Catalyst apparently
              // really dislikes frequent STATIC_DRAW updates (the libGL part
              // crashes)
              paddle->scale() *= .3f;
              xt = Explosion::BAD_POWER_UP_COLLECTED;
              break;

            case PowerUp::QUICKEN_GAME:
              l.game_model()->gameSpeed() *= 1.1f;
              xt = Explosion::BAD_POWER_UP_COLLECTED;
              break;

            case PowerUp::DECREASE_FAN:
              paddle->relativeFanPower() *= .2f;
              xt = Explosion::BAD_POWER_UP_COLLECTED;
              break;
          }

          l.game_model()->addGameObject(std::make_shared<Explosion>(xt, *pu));

          pu->alive() = false;
        }
      }
      continue;
    }

    setForce(box, paddle);

    if (!paddle) {
      continue;
    }

    if (box->position().y() - box->size() / 2.f < paddle->position().y()) {
      l.game_model()->addGameObject(std::make_shared<Explosion>(Explosion::BOX_FLOOR_CRASH, *box));

      box->alive() = false;
      box_count--;

      if (_model->remainingLives() > 0) {
        --_model->remainingLives();
      }
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
        l.game_model()->addGameObject(std::make_shared<Explosion>(Explosion::BOX_BOX_COLLISION, *box));
        l.game_model()->addGameObject(std::make_shared<Explosion>(Explosion::BOX_BOX_COLLISION, *ibox));

        box->alive() = false;
        ibox->alive() = false;
        box_count -= 2;

        // EXTRA MONSTER POINTS
        _model->playerPoints() += (box->velocity() - ibox->velocity()).length() / 4.f;
      }
    }
  }

  if (paddle) {
    if (l.game_model()->gameSpeed() < 1.f) {
      l.game_model()->gameSpeed() = 1.f;
    }

    l.game_model()->gameSpeed() += (_model->playerPoints() - old_player_points) / 10000.f;

    if (l.game_model()->gameSpeed() > 1.25f) {
      l.game_model()->gameSpeed() = 1.25f;
    }
  } else {
    l.game_model()->gameSpeed() = .3f;
  }

  return true;
}


void WorldLogic::addBoxToGame(Logic &l)
{
  std::uniform_real_distribution<scalar_type> size_exp_dist(1.7f, 3.4f);
  scalar_type size = exp2(size_exp_dist(rng));

  vec3_type max_pos(_model->getWorldHalfWidth() - size / 2.f, _model->getWorldHalfHeight() - size / 2.f, HUGE_VALF);

  std::uniform_real_distribution<scalar_type> x_dist(-max_pos.x(), max_pos.x());
  scalar_type x = x_dist(rng);

  std::shared_ptr<Box> box(new Box("Box", !_model->remainingLives()));
  box->size() = size;
  box->position() = vec3_type(x, 0.f, 0.f);
  box->maxPosition() = max_pos;

  l.game_model()->addGameObject(box);
  box_count++;
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
  if (!paddle) {
    std::uniform_real_distribution<scalar_type> x_dist(-20.f, 20.f), y_dist(-5.f, 25.f);
    box->externalForce() = vec3_type(x_dist(rng), y_dist(rng), 0.f);

    return;
  }

  // I like how this function completely disregards the size of the box in
  // regard to how box and paddle overlap

  vec3_type pll = paddle->position() - paddle->size() / 2.f; // lower left
  vec3_type pur = paddle->position() + paddle->size() / 2.f; // upper right
  pll.y() = -HUGE_VALF;
  pur.y() =  HUGE_VALF;

  vec3_type force;

  if (in_bounding_box(box->position(), pll, pur)) {
    force = vec3_type(0.f, 1.f, 0.f);
  } else if (box->position().y() - box->size() / 2.f > paddle->position().y()) { // "oberhalb", not "oberhalb oder auf gleicher Höhe"
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

    // If a box is bigger than the paddle, it might happen that the box is
    // off-center, but its closest edge ie on the other side of the paddle
    // center than most of the box; this will lead to the box being pushed in
    // the wrong direction. Fix this here.
    if (((box->position().x() > paddle->position().x()) && (min_vector.x() < 0.f)) ||
        ((box->position().x() < paddle->position().x()) && (min_vector.x() > 0.f)))
    {
      min_vector.x() = 0.f;
    }

    force = off_force_mult * powf(vec3_type(0.f, 1.f, 0.f).dot(min_vector), off_force_exp) * min_vector;
  }

  box->externalForce() = paddle->relativeFanPower() * force * 10.f * box->size() * box->size();
}


void WorldLogic::restartGame(Logic &l)
{
  for (const std::shared_ptr<GameObject> &obj: l.game_model()->objects()) {
    if (typeid(obj) != typeid(model::Explosion)) {
      obj->alive() = false;
    }
  }

  add_box_interval_timer = 0.f;
  next_powerup_timer = NAN; // batman
  box_count = 0;

  _model->alive() = true;
  _model->playerPoints() = 0;
  _model->remainingLives() = 5;
  l.game_model()->gameSpeed() = 1.f;
  l.game_model()->stickyWallTimer() = 0.f;

  std::shared_ptr<Paddle> user_paddle(new Paddle("PlayerPaddle"));
  user_paddle->size() = vec3_type(10.f, 2.5f, 10.f);
  user_paddle->position() = vec3_type(0.f, -_model->getWorldHalfHeight() + user_paddle->size().y() * 2.f, 0.f);
  user_paddle->maxPosition() = vec3_type(_model->getWorldHalfWidth() - user_paddle->size().x() / 2.f, _model->getWorldHalfHeight(), HUGE_VALF);

  l.game_model()->addGameObject(user_paddle);

  _shallRestartTheGame = false;

  _model->gameRestarted() = true;
}
