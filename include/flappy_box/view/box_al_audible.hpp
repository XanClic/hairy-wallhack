# pragma once

# include "view/al_renderer.hpp"
# include "flappy_box/model/box.hpp"

namespace flappy_box
{
  namespace view
  {
    class BoxAlAudible: public ::view::AlRenderer::Audible
    {
      public:
        BoxAlAudible(const std::shared_ptr<model::Box> &);

        virtual void auralize(::view::AlRenderer &) override;

      private:
        std::shared_ptr<const model::Box> _model;
    }; // BoxAlAudible
  }
}
