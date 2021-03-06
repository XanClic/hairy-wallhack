#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "flappy_box/controller/flappy_engine.hpp"

#include "flappy_box/model/box.hpp"
#include "flappy_box/model/explosion.hpp"
#include "flappy_box/model/game_over.hpp"
#include "flappy_box/model/paddle.hpp"
#include "flappy_box/model/power_up.hpp"
#include "flappy_box/model/world.hpp"
#include "flappy_box/controller/box_object_logic.hpp"
#include "flappy_box/controller/explosion_logic.hpp"
#include "flappy_box/controller/paddle_logic.hpp"
#include "flappy_box/controller/power_up_logic.hpp"
#include "flappy_box/controller/world_logic.hpp"
#include "flappy_box/view/box_al_audible.hpp"
#include "flappy_box/view/box_gl_drawable.hpp"
#include "flappy_box/view/explosion_al_audible.hpp"
#include "flappy_box/view/explosion_gl_drawable.hpp"
#include "flappy_box/view/game_over_al_audible.hpp"
#include "flappy_box/view/game_over_gl_drawable.hpp"
#include "flappy_box/view/paddle_al_audible.hpp"
#include "flappy_box/view/paddle_gl_drawable.hpp"
#include "flappy_box/view/power_up_gl_drawable.hpp"
#include "flappy_box/view/world_al_audible.hpp"
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
  ::view::GlRenderer::SSAOQuality ssao = ::view::GlRenderer::HQ_SSAO;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--help")) {
      printf("Parameters:\n");
      printf("  --blur-passes=n: Sets the number of bloom blur passes (default: %li);\n", passes);
      printf("                   use 0 to disable bloom\n");
      printf("  --bloom-lq:      Reduces the bloom quality by using an integer FBO\n");
      printf("  --ssao=which:    Select Screen Space Ambient Occlusion Quality (none, lq, hq (default))\n");

      exit(0);
    } else if (!strncmp(argv[i], "--blur-passes=", strlen("--blur-passes="))) {
      char *end = argv[i] + strlen("--blur-passes=");
      if (!*end) {
        fprintf(stderr, "Missing argument for --blur-passes=.\n");
        exit(1);
      }

      errno = 0;
      passes = strtol(end, &end, 0);
      if (*end || errno || (passes < 0)) {
        fprintf(stderr, "Invalid argument for --blur-passes=.\n");
        exit(1);
      }
    } else if (!strcmp(argv[i], "--bloom-lq")) {
      bloom_lq = true;
    } else if (!strncmp(argv[i], "--ssao=", strlen("--ssao="))) {
      char *end = argv[i] + strlen("--ssao=");
      if (!*end) {
        fprintf(stderr, "Missing argument for --ssao=.\n");
        exit(1);
      } else if (!strcmp(end, "none")) {
        ssao = ::view::GlRenderer::NO_SSAO;
      } else if (!strcmp(end, "lq")) {
        ssao = ::view::GlRenderer::LQ_SSAO;
      } else if (!strcmp(end, "hq")) {
        ssao = ::view::GlRenderer::HQ_SSAO;
      } else {
        fprintf(stderr, "Invalid argument for --ssao=.\n");
        exit(1);
      }
    } else {
      fprintf(stderr, "Unknown parameter %s. Try --help.\n", argv[i]);
      exit(1);
    }
  }


  gl_renderer()->parameters(passes, bloom_lq, ssao);

  al_renderer()->camera_position() = vec3_type(0.f, 0.f, -100.f);
  gl_renderer()->camera_position() = vec3_type(0.f, 0.f, -100.f);


  // register the delegate classes fo Box 
  game_logic() ->   logic_factory().register_module<model::Box>([](const std::shared_ptr<model::Box> &b ) { return std::make_shared<BoxObjectLogic>     (b); });
  al_renderer()-> audible_factory().register_module<model::Box>([](const std::shared_ptr<model::Box> &b ) { return std::make_shared<view::BoxAlAudible> (b); });
  gl_renderer()->drawable_factory().register_module<model::Box>([](const std::shared_ptr<model::Box> &b ) { return std::make_shared<view::BoxGlDrawable>(b); });

  game_logic() ->   logic_factory().register_module<model::Paddle>([](const std::shared_ptr<model::Paddle> &p) { return std::make_shared<PaddleLogic>           (p); });
  al_renderer()-> audible_factory().register_module<model::Paddle>([](const std::shared_ptr<model::Paddle> &p) { return std::make_shared<view::PaddleAlAudible> (p); });
  gl_renderer()->drawable_factory().register_module<model::Paddle>([](const std::shared_ptr<model::Paddle> &p) { return std::make_shared<view::PaddleGlDrawable>(p); });

  game_logic() ->   logic_factory().register_module<model::World>([](const std::shared_ptr<model::World> &w) { return std::make_shared<WorldLogic>           (w); });
  al_renderer()-> audible_factory().register_module<model::World>([](const std::shared_ptr<model::World> &w) { return std::make_shared<view::WorldAlAudible> (w); });
  gl_renderer()->drawable_factory().register_module<model::World>([](const std::shared_ptr<model::World> &w) { return std::make_shared<view::WorldGlDrawable>(w); });

  al_renderer()-> audible_factory().register_module<model::GameOver>([](const std::shared_ptr<model::GameOver> &go) { return std::make_shared<view::GameOverAlAudible> (go); });
  gl_renderer()->drawable_factory().register_module<model::GameOver>([](const std::shared_ptr<model::GameOver> &go) { return std::make_shared<view::GameOverGlDrawable>(go); });

  game_logic() ->   logic_factory().register_module<model::Explosion>([](const std::shared_ptr<model::Explosion> &x) { return std::make_shared<ExplosionLogic>           (x); });
  al_renderer()-> audible_factory().register_module<model::Explosion>([](const std::shared_ptr<model::Explosion> &x) { return std::make_shared<view::ExplosionAlAudible> (x); });
  gl_renderer()->drawable_factory().register_module<model::Explosion>([](const std::shared_ptr<model::Explosion> &x) { return std::make_shared<view::ExplosionGlDrawable>(x); });

  game_logic() ->   logic_factory().register_module<model::PowerUp>([](const std::shared_ptr<model::PowerUp> &pu) { return std::make_shared<PowerUpLogic>           (pu); });
  gl_renderer()->drawable_factory().register_module<model::PowerUp>([](const std::shared_ptr<model::PowerUp> &pu) { return std::make_shared<view::PowerUpGlDrawable>(pu); });


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

