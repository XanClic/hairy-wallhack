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

        scalar_type blades_ang = 0.f;

        static const int outer_segments = 40, inner_segments = 15;
        dake::gl::vertex_array paddle_va;

        static const int blade_cnt = 9;
        dake::gl::vertex_array blade_va;

        static const int vortex_cnt = 3, vortex_line_len = 20;
        static const constexpr scalar_type vortex_speed = 400.f, vortex_band_width = 10.f;
        dake::gl::vertex_array vortex_vas[vortex_cnt];
        vec3_type vortex_dat[vortex_cnt][vortex_line_len][2];

        void updateVBOs(void);
    };
  }
}


#endif
