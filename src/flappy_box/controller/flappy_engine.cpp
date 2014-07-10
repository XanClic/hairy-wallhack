#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "flappy_box/controller/flappy_engine.hpp"

#include "flappy_box/model/box.hpp"
#include "flappy_box/model/world.hpp"
#include "flappy_box/controller/box_object_logic.hpp"
#include "flappy_box/controller/paddle_logic.hpp"
#include "flappy_box/controller/world_logic.hpp"
#include "flappy_box/view/box_al_audible.hpp"
#include "flappy_box/view/box_gl_drawable.hpp"
#include "flappy_box/view/paddle_gl_drawable.hpp"
#include "flappy_box/view/world_gl_drawable.hpp"
#include "view/glut_window.hpp"


#include <AL/alut.h>
#include <GL/freeglut.h>



using namespace flappy_box;
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


  // TODO: Remove arguments
  long passes = 5;
  bool bloom_lq = false;

  for (int i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "--help")) {
          printf("Parameters:\n");
          printf("  --blur-passes=n: Sets the number of bloom blur passes (default: 5);\n");
          printf("                   use 0 to disable bloom\n");
          printf("  --bloom-lq:      Reduces the bloom quality by using an integer FBO\n");

          exit(0);
      } else if (!strncmp(argv[i], "--blur-passes=", strlen("--blur-passes="))) {
          char *end = argv[i] + strlen("--blur-passes=");
          if (!*end) {
              fprintf(stderr, "Missing argument for --blur-passes=.\n");
              exit(0);
          }

          errno = 0;
          passes = strtol(end, &end, 0);
          if (*end || errno || (passes < 0)) {
              fprintf(stderr, "Invalid argument for --blur-passes=\n");
              exit(0);
          }
      } else if (!strcmp(argv[i], "--bloom-lq")) {
          bloom_lq = true;
      } else {
          fprintf(stderr, "Unknown parameter %s. Try --help.\n", argv[i]);
          exit(1);
      }
  }


  gl_renderer()->parameters(passes, bloom_lq);


  // register the delegate classes fo Box 
  game_logic() ->   logic_factory().register_module<model::Box>([](const std::shared_ptr<model::Box> &b ) { return std::make_shared<BoxObjectLogic>     (b); });
  al_renderer()-> audible_factory().register_module<model::Box>([](const std::shared_ptr<model::Box> &b ) { return std::make_shared<view::BoxAlAudible> (b); });
  gl_renderer()->drawable_factory().register_module<model::Box>([](const std::shared_ptr<model::Box> &b ) { return std::make_shared<view::BoxGlDrawable>(b); });

  game_logic() ->   logic_factory().register_module<model::Paddle>([](const std::shared_ptr<model::Paddle> &p) { return std::make_shared<PaddleLogic>           (p); });
  gl_renderer()->drawable_factory().register_module<model::Paddle>([](const std::shared_ptr<model::Paddle> &p) { return std::make_shared<view::PaddleGlDrawable>(p); });

  game_logic() ->   logic_factory().register_module<model::World>([](const std::shared_ptr<model::World> &w) { return std::make_shared<WorldLogic>           (w); });
  gl_renderer()->drawable_factory().register_module<model::World>([](const std::shared_ptr<model::World> &w) { return std::make_shared<view::WorldGlDrawable>(w); });


  game_model()->addGameObject(std::make_shared<model::World>(u8"ザ　ワルダ"));
}


void FlappyEngine::run()
{
  // Create a window and connect it with a view::GlRenderer and an InputEventHandler.
  // NOTE: The default resolution was copulating with hazels (1500x1000? wtf?)
  // Also, auto window = std::make_shared<>() should be burnt (and all of this code anyway, for that matter)
  std::shared_ptr<::view::GlutWindow> window(new ::view::GlutWindow("flappy_box", 1280, 720, gl_renderer(), shared_from_this()));

  gl_renderer()->init_with_context();

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

