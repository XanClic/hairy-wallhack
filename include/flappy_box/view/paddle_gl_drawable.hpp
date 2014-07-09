#ifndef FLAPPY_BOX__VIEW__PADDLE_GL_DRAWABLE_HPP
#define FLAPPY_BOX__VIEW__PADDLE_GL_DRAWABLE_HPP

#include <memory>

#include <dake/gl/vertex_array.hpp>

#include "math.hpp"

#include "view/gl_renderer.hpp"
#include "view/glut_window.hpp"
#include "flappy_box/model/paddle.hpp"


namespace flappy_box
{
  namespace view
  {
    class PaddleGlDrawable: public ::view::GlRenderer::Drawable {
      public:
        PaddleGlDrawable(const std::shared_ptr<const model::Paddle> &);
        virtual ~PaddleGlDrawable(void);

        virtual void visualize(::view::GlRenderer &, ::view::GlutWindow &) override;

      private:
        std::shared_ptr<const model::Paddle> _model;

        scalar_type r0, r1;
        vec3_type size_for_r;

        static const int outer_segments = 40, inner_segments = 15;
        dake::gl::vertex_array paddle_va;

        void updateVBOs(void);
    };
  }
}


#endif
