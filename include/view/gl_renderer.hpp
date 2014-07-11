#ifndef VIEW__GL_RENDERER_HPP
#define VIEW__GL_RENDERER_HPP

#include <dake/gl/framebuffer.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/math/matrix.hpp>

#include <memory>

#include "model/game.hpp"

#include "factory_map.hpp"

namespace view 
{
  class GlutWindow;

  class GlRenderer
  {
    public:
      struct Drawable : public ::model::GameObject::Data
      {
        virtual void visualize( GlRenderer&, GlutWindow& ) = 0;
      };

      typedef factory_map< model::GameObject, Drawable > delegate_factory_type;

      GlRenderer() = delete;
      GlRenderer( std::shared_ptr< model::Game const > const& );

      void init_with_context(void);

      std::shared_ptr< model::Game const > const& game_model() const;

      /// Return factory creating Drawable delegates.
      delegate_factory_type&       drawable_factory();
      delegate_factory_type const& drawable_factory() const;

      dake::math::mat4 &camera(void) { return cam; }
      const dake::math::mat4 &camera(void) const { return cam; }

      const dake::math::mat4 &projection(void) const { return proj; }

      dake::math::vec3 &light_position(void) { return light_pos; }
      const dake::math::vec3 &light_position(void) const { return light_pos; }

      void render_character(dake::math::vec2 pos, unsigned char c, dake::math::vec3 color = dake::math::vec3(1.f, 1.f, 1.f));
      void render_line(dake::math::vec2 pos, const char *string, dake::math::vec3 color = dake::math::vec3(1.f, 1.f, 1.f));
      const dake::math::vec2 &character_size(void) const { return char_size; }

      void parameters(long passes, bool bloom_lq);
      bool has_bloom(void) const { return bloom_blur_passes; }

      virtual void visualize_model( GlutWindow& );
      virtual void resize( GlutWindow& );

    private:
      std::shared_ptr< model::Game const > _game_model;
      delegate_factory_type _drawable_factory;

      dake::math::mat4 cam, proj;
      std::shared_ptr<dake::gl::framebuffer> fb, blur_fbs[2];
      std::shared_ptr<dake::gl::program> fb_prg, blur_prg[2], char_prg;
      std::shared_ptr<dake::gl::vertex_array> fb_vertices;
      std::shared_ptr<dake::gl::texture> bitmap_font;

      dake::math::vec3 light_pos = dake::math::vec3(0.f, 0.f, 0.f);

      dake::math::vec2 char_size;
      unsigned width, height;

      bool bloom_use_lq = false;
      long bloom_blur_passes = 6;

  }; // GlRenderer

} // view::

#endif
