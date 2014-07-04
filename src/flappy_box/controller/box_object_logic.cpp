# include "flappy_box/controller/box_object_logic.hpp"
# include <AL/alut.h>
# include <thread>


# include <GL/freeglut.h>

using namespace ::flappy_box::controller;

BoxObjectLogic::BoxObjectLogic(const std::shared_ptr< flappy_box::model::Box >& b )
: ::controller::Logic::ObjectLogic()
, _model( b )
{}

bool BoxObjectLogic::advance( ::controller::Logic& l, ::controller::InputEventHandler::keyboard_event const& ev )
{
  double timestep_sec = l.game_model()->timestep().count();


  return false;
}