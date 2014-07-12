#ifndef VIEW__AL_RENDERER_HPP
#define VIEW__AL_RENDERER_HPP

#include "model/game.hpp"

#include "factory_map.hpp"
#include "math.hpp"

#include <AL/al.h>

namespace view 
{
  class AlRenderer {
    public:
      struct Audible: public ::model::GameObject::Data {
        virtual void auralize(AlRenderer &) = 0;
      };

      typedef factory_map<model::GameObject, Audible> delegate_factory_type;

      AlRenderer(void) = delete;
      AlRenderer(const std::shared_ptr<const model::Game> &);

      const std::shared_ptr<const model::Game> &game_model() const;

      /// Return factory creating Audible delegates.
      delegate_factory_type &audible_factory(void);
      const delegate_factory_type &audible_factory(void) const;

      vec3_type &camera_position(void) { return cam_pos; }
      const vec3_type &camera_position(void) const { return cam_pos; }

      vec3_type spherical_projection(const vec3_type &position, const vec3_type &max_position);

      void auralize_model(void);


    private:
      std::string _name;
      std::shared_ptr<const model::Game> _game_model;
      delegate_factory_type _audible_factory;

      vec3_type cam_pos = vec3_type(0.f, 0.f, 0.f);
  }; // AlRenderer

} // view::

#endif
