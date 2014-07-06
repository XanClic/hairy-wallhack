#include <memory>

#include "flappy_box/controller/flappy_engine.hpp"

#include "flappy_box/model/box.hpp"
#include "flappy_box/model/world.hpp"
#include "flappy_box/controller/box_object_logic.hpp"
#include "flappy_box/controller/paddle_logic.hpp"
#include "flappy_box/controller/world_logic.hpp"
#include "flappy_box/view/box_gl_drawable.hpp"
#include "flappy_box/view/box_al_audible.hpp"
#include "view/glut_window.hpp"


#include <AL/alut.h>
#include <GL/freeglut.h>



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

  game_logic()->logic_factory().register_module<flappy_box::model::Paddle>([](const std::shared_ptr<flappy_box::model::Paddle> &p) { return std::make_shared<PaddleLogic>(p); });

  game_logic()->logic_factory().register_module<flappy_box::model::World>([](const std::shared_ptr<flappy_box::model::World> &w) { return std::make_shared<WorldLogic>(w); });

  // create one single cube (to be deleted later...)
  std::shared_ptr< flappy_box::model::Box > box = std::make_shared< flappy_box::model::Box >("Box"); 
  box->size() = 20.f;
  box->angle() = 22.5f;
  game_model()->addGameObject( box );

  game_model()->addGameObject(std::make_shared<flappy_box::model::World>(u8"ザ　ワルダ"));
}


void FlappyEngine::run()
{
  // Create a window and connect it with a view::GlRenderer and an InputEventHandler.
  // NOTE: The default resolution was copulating with hazels (1500x1000? wtf?)
  // Also, auto window = std::make_shared<>() should be burnt (and all of this code anyway, for that matter)
  std::shared_ptr<::view::GlutWindow> window(new ::view::GlutWindow("flappy_box", 1280, 720, gl_renderer(), shared_from_this()));

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

