#include <dake/gl/gl.hpp>
#include <dake/math/matrix.hpp>

#include "math.hpp"

# include "view/gl_renderer.hpp"

# include "view/glut_window.hpp"

# include "GL/freeglut.h"

using namespace dake;
using namespace dake::math;
using namespace ::view;

GlRenderer::GlRenderer( std::shared_ptr< model::Game const > const& g )
: _game_model( g )
{}

std::shared_ptr< ::model::Game const > const& GlRenderer::game_model() const
{
  return _game_model;
}

GlRenderer::delegate_factory_type& GlRenderer::drawable_factory()
{
  return _drawable_factory;
}

GlRenderer::delegate_factory_type const& GlRenderer::drawable_factory() const
{
  return _drawable_factory;
}

void GlRenderer::init_with_context(void)
{
  gl::glext_init();

  glClearColor(1.f, 1.f, 1.f, 1.f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void GlRenderer::visualize_model( GlutWindow& w )
{
#ifndef DEBUG_VISUALIZATION 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif


  cam = mat4::identity().translated(vec3(0.f, 0.f, -100.f));

  // render routines for game objects
  for( auto o : game_model()->objects() )
  {
    auto drawable = o->getData< Drawable >();
    if( ! drawable )
    {
      //std::clog << "::view::GlRenderer::visualize_model: Adding new Drawable for \"" << o->name() << "\"." << std::endl;
      drawable = _drawable_factory.create_for( o );
      o->registerData( drawable );
    }

    if( drawable ) drawable->visualize( *this, w );
  }

  glutSwapBuffers(); 
}

void GlRenderer::resize(GlutWindow &win)
{
  width = win.width();
  height = win.height();

  glViewport(0, 0, width, height);

  proj = mat4::projection(static_cast<float>(M_PI) / 2.f, static_cast<float>(width) / height, 1.f, 1000.f);
}
