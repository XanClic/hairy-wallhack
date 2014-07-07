#include "flappy_box/view/world_gl_drawable.hpp"


using namespace view;
using namespace flappy_box::view;


WorldGlDrawable::WorldGlDrawable(const std::shared_ptr<const flappy_box::model::World> &w):
  _model(w)
{}

WorldGlDrawable::~WorldGlDrawable(void)
{}

void WorldGlDrawable::visualize(GlRenderer &, GlutWindow &)
{
}
