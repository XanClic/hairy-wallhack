# pragma once

# include "view/al_renderer.hpp"
# include "flappy_box/model/box.hpp"

namespace flappy_box
{
  namespace view
  {
    class BoxAlAudible : public ::view::AlRenderer::Audible
    {
      public:
        BoxAlAudible( std::shared_ptr< model::Box > const& );

        virtual void auralize( ::view::AlRenderer& ) override;

      private:
        std::shared_ptr< model::Box const > _model;
        
    }; // BoxAlAudible
  }
}
