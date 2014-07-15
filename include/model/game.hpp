#ifndef MODEL__GAME_HPP
#define MODEL__GAME_HPP

#include <chrono>
#include <vector>
#include <memory>

#include "math.hpp"

#include "model/game_object.hpp"


namespace model
{
  class Game {
    public:
      Game(void);

      void addGameObject(const std::shared_ptr<GameObject> &o);
      void processAddGameObjects(void);

      void removeInvalidGameObjects(void);

      const std::vector<std::shared_ptr<GameObject>> &objects(void) const;

      void setTimestamp(const std::chrono::steady_clock::time_point &t);

      const std::chrono::steady_clock::time_point &timestamp(void) const;
      const std::chrono::duration<double>         &timestep(void) const;

      scalar_type gameSpeed(void) const { return game_speed; }
      scalar_type &gameSpeed(void) { return game_speed; }

      scalar_type stickyWallTimer(void) const { return sticky_wall_timer; }
      scalar_type &stickyWallTimer(void) { return sticky_wall_timer; }

  private:
    std::vector<std::shared_ptr<GameObject>>        _objects;
    std::vector<std::shared_ptr<GameObject>> _waitingObjects;
    std::chrono::steady_clock::time_point         _timestamp;
    std::chrono::duration<double>                  _timestep;

    scalar_type game_speed = 1.f;
    scalar_type sticky_wall_timer = 0.f;
  }; // Game
} // model::

#endif
