# pragma once

# include "view/gl_renderer.hpp"

# include "flappy_box/model/box.hpp"

#include "math.hpp"

namespace flappy_box
{
  namespace view
  {
    class BoxGlDrawable : public ::view::GlRenderer::Drawable
    {
      public:

        BoxGlDrawable( const std::shared_ptr< model::Box >& );
        virtual ~BoxGlDrawable();
        virtual void visualize( ::view::GlRenderer&, ::view::GlutWindow& ) override;

      private:
        const model::Box *_model;
        scalar_type lifetime = 0.f;

    }; // BoxGlDrawable
  }
}
