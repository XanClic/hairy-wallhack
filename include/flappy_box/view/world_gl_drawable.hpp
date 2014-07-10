#ifndef FLAPPY_BOX__VIEW__WORLD_GL_DRAWABLE_HPP
#define FLAPPY_BOX__VIEW__WORLD_GL_DRAWABLE_HPP

#include <dake/gl/vertex_array.hpp>

#include <memory>

#include "view/gl_renderer.hpp"
#include "flappy_box/model/world.hpp"

namespace flappy_box
{
  namespace view
  {
    class WorldGlDrawable: public ::view::GlRenderer::Drawable {
      public:
        WorldGlDrawable(const std::shared_ptr<const model::World> &);
        virtual ~WorldGlDrawable(void);
        virtual void visualize(::view::GlRenderer &, ::view::GlutWindow &) override;

      private:
        std::shared_ptr<const model::World> _model;

        dake::gl::vertex_array *va;
        float time_step = 0.f;
        bool was_dead = false;
    };
  }
}

#endif
