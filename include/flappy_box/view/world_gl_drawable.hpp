#ifndef FLAPPY_BOX__VIEW__WORLD_GL_DRAWABLE_HPP
#define FLAPPY_BOX__VIEW__WORLD_GL_DRAWABLE_HPP

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
    };
  }
}

#endif
