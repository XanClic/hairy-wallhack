#ifndef VIEW__GL_RENDERER_HPP
#define VIEW__GL_RENDERER_HPP

#include <dake/gl/framebuffer.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/math/matrix.hpp>

#include <memory>
#include <string>
#include <vector>

#include "model/game.hpp"

#include "factory_map.hpp"
#include "math.hpp"

namespace view 
{
  class GlutWindow;

  class GlRenderer {
    public:
      struct Drawable: public ::model::GameObject::Data {
        virtual void visualize(GlRenderer &, GlutWindow &) = 0;
      };

      enum SSAOQuality {
        NO_SSAO,
        LQ_SSAO,
        HQ_SSAO
      };

      typedef factory_map<model::GameObject, Drawable> delegate_factory_type;

      GlRenderer(void) = delete;
      GlRenderer(const std::shared_ptr<const model::Game> &);

      void init_with_context(void);

      const std::shared_ptr<const model::Game> &game_model(void) const;

      /// Return factory creating Drawable delegates.
      delegate_factory_type &drawable_factory(void);
      const delegate_factory_type &drawable_factory() const;

      vec3_type &camera_position(void) { return cam_pos; }
      const vec3_type &camera_position(void) const { return cam_pos; }

      dake::math::mat4 &camera(void) { return cam; }
      const dake::math::mat4 &camera(void) const { return cam; }

      const dake::math::mat4 &projection(void) const { return proj; }

      dake::math::vec3 &light_position(void) { return light_pos; }
      const dake::math::vec3 &light_position(void) const { return light_pos; }

      void render_character(dake::math::vec2 pos, unsigned char c, dake::math::vec3 color = dake::math::vec3(1.f, 1.f, 1.f));
      void render_line(dake::math::vec2 pos, const char *string, dake::math::vec3 color = dake::math::vec3(1.f, 1.f, 1.f));
      const dake::math::vec2 &character_size(void) const { return char_size; }
      dake::math::vec2 line_size(const char *string) const;

      void log_add(const std::string &msg);

      void parameters(long passes, bool bloom_lq, SSAOQuality ssao);
      bool has_bloom(void) const { return bloom_blur_passes; }

      virtual void visualize_model(GlutWindow &);
      virtual void resize(GlutWindow &);


    private:
      struct LogEntry {
        LogEntry(void) {}
        LogEntry(const std::string &m, float lt): msg(m), lifetime(lt) {}

        std::string msg;
        float lifetime;
      };

      std::shared_ptr<const model::Game> _game_model;
      delegate_factory_type _drawable_factory;

      vec3_type cam_pos = vec3_type(0.f, 0.f, 0.f);
      dake::math::mat4 cam, proj;
      std::shared_ptr<dake::gl::framebuffer> fb, blur_fbs[2], ssao_fb, ssao_blur_fbs[2], ssao_output_fb;
      std::shared_ptr<dake::gl::program> fb_prg, blur_prg[2], char_prg, ssao_prg, ssao_blur_prg[2], ssao_apply_prg;
      std::shared_ptr<dake::gl::vertex_array> fb_vertices;
      std::shared_ptr<dake::gl::texture> bitmap_font, ssao_noise;

      dake::math::vec3 light_pos = dake::math::vec3(0.f, 0.f, 0.f);

      dake::math::vec2 char_size;
      unsigned width, height;

      bool bloom_use_lq = false;
      long bloom_blur_passes = 5;
      SSAOQuality ssao = HQ_SSAO;

      std::vector<LogEntry> log;
  }; // GlRenderer

} // view::

#endif
