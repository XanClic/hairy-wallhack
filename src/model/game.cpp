# include "model/game.hpp"

# include <algorithm>

using namespace model;

Game::Game() 
: _timestamp( std::chrono::steady_clock::now() )
{ }

void Game::addGameObject( std::shared_ptr< GameObject > const& o )
{
  // we do lazy adding in case of inserting an element while iterating on the object vector
  if( ! o ) throw std::logic_error( "model::Game::addObject: Invalid object." );
  _waitingObjects.push_back( o );
}

void Game::processAddGameObjects()
{
    // HAZARD: we need to add object usually at the second place! WTF!
    //         world object, which is created as first object will be the first
    //         object ever. The paddle, added as second object, must be drawn
    //         as last object to get the right effect with the transparency.
  auto second = _objects.begin();
  if (_objects.end() != second) second++;
  _objects.insert( second, _waitingObjects.begin(), _waitingObjects.end() );
  _waitingObjects.clear();
}

// ATTENTION: NEW CODE!
void Game::removeInvalidGameObjects(  )
{
  auto predicate = []( std::shared_ptr< GameObject > const& go )->bool { return ! go->isAlive(); }; 
  _objects.erase(  std::remove_if( _objects.begin(), _objects.end(), predicate ), 
                   _objects.end()
                 );
}
//

const std::vector< std::shared_ptr< GameObject > >&  Game::objects() const 
{
  return _objects;
}

std::chrono::steady_clock::time_point const& Game::timestamp() const 
{
  return _timestamp;
}

std::chrono::duration< double > const& Game::timestep() const 
{
  return _timestep;
}

void Game::setTimestamp( std::chrono::steady_clock::time_point const& t )
{
  auto last_timestamp = timestamp();
  _timestamp = std::chrono::steady_clock::now();
  _timestep = t - last_timestamp;
}
