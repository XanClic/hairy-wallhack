#ifndef FLAPPY_BOX__VIEW__POWER_UP_GL_DRAWABLE_HPP
#define FLAPPY_BOX__VIEW__POWER_UP_GL_DRAWABLE_HPP

#include <memory>

#include "math.hpp"

#include "view/gl_renderer.hpp"
#include "view/glut_window.hpp"
#include "flappy_box/model/power_up.hpp"


namespace flappy_box
{
  namespace view
  {
    class PowerUpGlDrawable: public ::view::GlRenderer::Drawable {
      public:
        PowerUpGlDrawable(const std::shared_ptr<const model::PowerUp> &);

        virtual void visualize(::view::GlRenderer &, ::view::GlutWindow &) override;


      private:
        const model::PowerUp *_model;
    };
  }
}


#endif
