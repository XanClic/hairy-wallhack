# include "flappy_box/controller/flappy_engine.hpp"

# include "flappy_box/model/box.hpp"


# include "flappy_box/controller/box_object_logic.hpp"
# include "flappy_box/view/box_gl_drawable.hpp"
# include "flappy_box/view/box_al_audible.hpp"
# include "view/glut_window.hpp"


# include <AL/alut.h>
# include <GL/freeglut.h>



using namespace ::flappy_box::controller;

static std::function< void () > __current_glut_advance_func = [](){ std::cerr << "Warning: Default function called in __current_glut_advance_func." << std::endl; };

FlappyEngine::FlappyEngine( const std::shared_ptr< ::controller::Logic >& l )
: ::controller::GlutEngine( l )
, _al_renderer( std::make_shared< ::view::AlRenderer >( game_model() ) )
, _gl_renderer( std::make_shared< ::view::GlRenderer >( game_model() ) )
{}

void FlappyEngine::init( int& argc, char** argv )
{
  GlutEngine::init( argc, argv );

  alutInit( &argc, argv );

  // register the delegate classes fo Box 
  game_logic() ->   logic_factory().register_module< flappy_box::model::Box >( []( std::shared_ptr< flappy_box::model::Box > const& b ) { return std::make_shared< BoxObjectLogic >     ( b ); } );
  al_renderer()-> audible_factory().register_module< flappy_box::model::Box >( []( std::shared_ptr< flappy_box::model::Box > const& b ) { return std::make_shared< view::BoxAlAudible > ( b ); } );
  gl_renderer()->drawable_factory().register_module< flappy_box::model::Box >( []( std::shared_ptr< flappy_box::model::Box > const& b ) { return std::make_shared< view::BoxGlDrawable >( b ); } );
  
  // TODO: Register all the other delegate classes
  //.
  //.
  //.
  
  
  // create one single cube (to be deleted later...)
  std::shared_ptr< flappy_box::model::Box > box = std::make_shared< flappy_box::model::Box >("Box"); 
  box->setSize(20.0);
  box->setAngle(22.5);
  game_model()->addGameObject( box );
  
  // TODO: Aufgabe 3.4 create and add a world object here
  //...
  
}


void FlappyEngine::run()
{
  // Create a window and connect it with a view::GlRenderer and an InputEventHandler.
  auto window = std::make_shared< ::view::GlutWindow >( "flappy_box", 1500, 1000, gl_renderer() , shared_from_this() );

  // run game
  GlutEngine::run();

  alutExit();
}

void FlappyEngine::step( ::controller::InputEventHandler::keyboard_event const& ev )
{
  ::controller::GlutEngine::step( ev );
  al_renderer()->auralize_model();
  glutPostRedisplay();
}

