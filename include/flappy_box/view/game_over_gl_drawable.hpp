#ifndef FLAPPY_BOX__VIEW__GAME_OVER_GL_DRAWABLE_HPP
#define FLAPPY_BOX__VIEW__GAME_OVER_GL_DRAWABLE_HPP

#include <dake/gl/vertex_array.hpp>
#include <dake/math/matrix.hpp>

#include "view/gl_renderer.hpp"
#include "flappy_box/model/game_over.hpp"

namespace flappy_box
{
  namespace view
  {
    class GameOverGlDrawable: public ::view::GlRenderer::Drawable {
      public:
        GameOverGlDrawable(const std::shared_ptr<const model::GameOver> &);
        virtual ~GameOverGlDrawable(void);
        virtual void visualize(::view::GlRenderer &, ::view::GlutWindow &) override;

      private:
        std::shared_ptr<const model::GameOver> _model;

        float centered_x(::view::GlRenderer &, const char *);
        void render_centered_string(::view::GlRenderer &r, float yofs, const char *s, dake::math::vec3 color = dake::math::vec3(1.f, 1.f, 1.f));
    };
  }
}

#endif
