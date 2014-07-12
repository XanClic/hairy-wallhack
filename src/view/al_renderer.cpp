#include <dake/helper/function.hpp>

#include <cassert>

#include "math.hpp"

#include "view/al_renderer.hpp"


using namespace dake::helper;
using namespace view;


AlRenderer::AlRenderer(const std::shared_ptr<const model::Game> &g):
  _game_model(g)
{}


const std::shared_ptr<const model::Game> &AlRenderer::game_model() const
{
  return _game_model;
}


AlRenderer::delegate_factory_type &AlRenderer::audible_factory()
{
  return _audible_factory;
}


const AlRenderer::delegate_factory_type &AlRenderer::audible_factory() const
{
  return _audible_factory;
}


void AlRenderer::auralize_model()
{
  for (auto o: game_model()->objects()) {
    std::shared_ptr<Audible> audible = o->getData<Audible>();
    if (!audible) {
      //std::clog << "::view::AlRenderer::auralize_model: Adding new Audible for \"" << o->name() << "\"." << std::endl;
      try {
        audible = _audible_factory.create_for(o);
        o->registerData( audible );
      } catch (std::out_of_range oor) {
        // some error andling here 
      }
    }

    if (audible) {
      audible->auralize(*this);
    }
  } 
}


// Projects the position on a half-sphere around the camera based on
// max_position (maximum x will be left of the camera; maximum y will be on top
// of the camera; x = y = 0 will be in front of the camera; the distance is
// equal to the distance of camera and object (if everything went well))
vec3_type AlRenderer::spherical_projection(const vec3_type &position, const vec3_type &max_position)
{
  // To do this properly with any camera position, one would have to project the
  // position of the object onto the camera plane; and the max_position as well,
  // which really is the hard part to get right. I don't want to (and I don't
  // need to), so skip it.
  assert(!cam_pos.x() && !cam_pos.y());
  scalar_type x = position.x() / max_position.x();
  scalar_type y = position.y() / max_position.y();

  scalar_type xy_angle = atan2f(y, x);
  scalar_type xy_rel_length = maximum(fabsf(x), fabsf(y));

  scalar_type supposed_distance = (position - cam_pos).length();

  // 1 = sqrt((cos(a) * n)² + (sin(a) * n)² + x)
  //   = sqrt((sin²(a) + cos²(a)) * n² + x)
  //   = sqrt(n² + x)
  // 1 = n² + x
  // x = 1 - n²

  return vec3_type(cosf(xy_angle) * xy_rel_length,
                   sinf(xy_angle) * xy_rel_length,
                   1.f - xy_rel_length * xy_rel_length) * supposed_distance;
}
