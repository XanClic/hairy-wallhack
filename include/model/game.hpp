# pragma once

# include "model/game_object.hpp"

# include <chrono>
# include <vector>
# include <memory>

namespace model
{

  class Game
  {
    public:

      Game();

      void addGameObject( std::shared_ptr< GameObject > const& o );
      void processAddGameObjects();
      
      // ATTENTION: NEW CODE!
      void removeInvalidGameObjects( );
      //
      const std::vector< std::shared_ptr< GameObject > >& objects() const;

      void setTimestamp( std::chrono::steady_clock::time_point const& t );

      std::chrono::steady_clock::time_point const& timestamp() const;
      std::chrono::duration< double > const&        timestep() const;

  private:
    std::vector< std::shared_ptr< GameObject > >        _objects;
    std::vector< std::shared_ptr< GameObject > > _waitingObjects;
    std::chrono::steady_clock::time_point             _timestamp;
    std::chrono::duration< double >                    _timestep;

  }; // Game

} // model::
