#ifndef FLAPPY_BOX__VIEW__EXPLOSION_GL_DRAWABLE_HPP
#define FLAPPY_BOX__VIEW__EXPLOSION_GL_DRAWABLE_HPP

#include <memory>

#include <dake/gl/vertex_array.hpp>

#include "math.hpp"

#include "view/gl_renderer.hpp"
#include "view/glut_window.hpp"
#include "flappy_box/model/explosion.hpp"


namespace flappy_box
{
  namespace view
  {
    class ExplosionGlDrawable: public ::view::GlRenderer::Drawable {
      public:
        ExplosionGlDrawable(const std::shared_ptr<const model::Explosion> &);
        virtual ~ExplosionGlDrawable(void);

        virtual void visualize(::view::GlRenderer &, ::view::GlutWindow &) override;


      private:
        const model::Explosion *_model;

        dake::gl::vertex_array particles_va;
        bool newfag = true;
    };
  }
}


#endif
