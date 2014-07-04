# include "controller/logic.hpp"

# include "math.hpp"







using namespace ::controller;


Logic::Logic( const std::shared_ptr< ::model::Game >& g ) : _model( g ) 
{
  
}




bool Logic::advance_model( const ::controller::InputEventHandler::keyboard_event& ev )
{
  game_model()->setTimestamp( std::chrono::steady_clock::now() );
  //std::clog << "::controller::Logic::advance: timestep " << std::chrono::duration_cast< std::chrono::milliseconds >( game_model()->timestep() ).count() << '.' << std::endl;

  // add waiting game objects
  game_model()->processAddGameObjects();
  
  // Tidy Up! Remove invalid game objects...
  game_model()->removeInvalidGameObjects();
  
  // iterate through all game objects
  for( auto o : game_model()->objects() )
  {
    if( o->isDynamic() )
    {
      // set the object's logic when there wasn't any
      auto obj_logic = o->getData< ObjectLogic >();
      if( ! obj_logic )
      {
        //std::clog << "::controller::Logic::advance: Adding new ObjectLogic for \"" << o->name() << "\"." << std::endl;
        obj_logic = _obj_logic_factory.create_for( o );
        o->registerData( obj_logic );
      }
  
      // advance the objects logic
      //if( obj_logic && obj_logic->advance( *this, ev ) ); // return true; // <- WHY SHALL WE RETURN IN THE MIDDLE OF THE ADVANCE ROUTINE
      if( obj_logic )
        obj_logic->advance( *this, ev );
    }
  }

  return false;
}
