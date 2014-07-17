#include <dake/gl/gl.hpp>
#include <dake/gl/framebuffer.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>
#include <dake/helper/function.hpp>
#include <dake/math/matrix.hpp>

#include <memory>
#include <cstdio>

#include "math.hpp"
#include "resource_finder.hpp"

#include "view/gl_renderer.hpp"
#include "view/glut_window.hpp"
#include "flappy_box/view/explosion_gl_drawable.hpp"
#include "flappy_box/view/paddle_gl_drawable.hpp"

#include "GL/freeglut.h"


using namespace dake;
using namespace dake::math;
using namespace dake::helper;
using namespace ::view;


GlRenderer::GlRenderer(const std::shared_ptr<const model::Game> &g):
  _game_model(g)
{}

const std::shared_ptr<const ::model::Game> &GlRenderer::game_model(void) const
{
  return _game_model;
}

GlRenderer::delegate_factory_type &GlRenderer::drawable_factory(void)
{
  return _drawable_factory;
}

const GlRenderer::delegate_factory_type &GlRenderer::drawable_factory(void) const
{
  return _drawable_factory;
}


void GlRenderer::parameters(long passes, bool bloom_lq, SSAOQuality do_ssao)
{
  if (passes < 0) {
    passes = 0;
  }
  if (!passes && (do_ssao != NO_SSAO)) {
    throw std::invalid_argument("Bloom is required for SSAO");
  }

  bloom_use_lq = bloom_lq;
  bloom_blur_passes = passes;
  ssao = do_ssao;
}


void GlRenderer::init_with_context(void)
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);


  if (has_bloom()) {
    fb = std::make_shared<gl::framebuffer>(2);

    // TMU 0 will be used by the blur FBO textures later; they "replace" the
    // second texture of this FBO
    (*fb)[0].set_tmu(1);
    (*fb)[1].set_tmu(0);

    // May not be 0 or 1, as it is bound together with the FB textures
    fb->depth().set_tmu(2);


    blur_fbs[0] = std::make_shared<gl::framebuffer>(1);
    blur_fbs[1] = std::make_shared<gl::framebuffer>(1);

    (*blur_fbs[0])[0].filter(GL_LINEAR);
    (*blur_fbs[1])[0].filter(GL_LINEAR);


    if (!bloom_use_lq) {
      fb->color_format(1, GL_R11F_G11F_B10F);
      blur_fbs[0]->color_format(0, GL_R11F_G11F_B10F);
      blur_fbs[1]->color_format(0, GL_R11F_G11F_B10F);
    }
  } else {
    fb = nullptr;
    blur_fbs[0] = nullptr;
    blur_fbs[1] = nullptr;
  }


  if (ssao != NO_SSAO) {
    ssao_fb = std::make_shared<gl::framebuffer>(1);

    (*ssao_fb)[0].set_tmu(3);


    ssao_blur_fbs[0] = std::make_shared<gl::framebuffer>(1);
    ssao_blur_fbs[1] = std::make_shared<gl::framebuffer>(1);

    // may not be 0 or 1, as they are bound together with the FB textures
    // may not be 2, as they are bound together with the FB depth
    (*ssao_blur_fbs[0])[0].set_tmu(3);
    (*ssao_blur_fbs[1])[0].set_tmu(4);


    if (ssao == LQ_SSAO) {
      (*ssao_fb)[0].filter(GL_LINEAR);
      (*ssao_blur_fbs[0])[0].filter(GL_LINEAR);
      (*ssao_blur_fbs[1])[0].filter(GL_LINEAR);
    }


    ssao_output_fb = std::make_shared<gl::framebuffer>(2);

    (*ssao_output_fb)[0].set_tmu(1);
    (*ssao_output_fb)[1].set_tmu(0);


    if (!bloom_use_lq) {
      ssao_output_fb->color_format(1, GL_R11F_G11F_B10F);
    }
  } else {
    ssao_fb = nullptr;
    ssao_blur_fbs[0] = nullptr;
    ssao_blur_fbs[1] = nullptr;
    ssao_output_fb = nullptr;
  }


  gl::shader vsh(gl::shader::VERTEX);

  if (has_bloom()) {
    gl::shader fsh(gl::shader::FRAGMENT);

    vsh.load(find_resource_file("fb_vert.glsl").c_str());
    fsh.load(find_resource_file("fb_combine_frag.glsl").c_str());

    if (!vsh.compile() || !fsh.compile()) {
      throw std::runtime_error("Could not compile FB display shaders");
    }

    fb_prg = std::make_shared<gl::program>();

    *fb_prg << vsh;
    *fb_prg << fsh;

    fb_prg->bind_attrib("in_pos", 0);
    fb_prg->bind_frag("out_color", 0);

    if (!fb_prg->link()) {
      throw std::runtime_error("Could not link FB display program");
    }


    gl::shader blur_x(gl::shader::FRAGMENT), blur_y(gl::shader::FRAGMENT);

    blur_x.load(find_resource_file("fb_blur_x_frag.glsl").c_str());
    blur_y.load(find_resource_file("fb_blur_y_frag.glsl").c_str());

    if (!blur_x.compile() || !blur_y.compile()) {
      throw std::runtime_error("Could not compile blur shaders");
    }

    for (int i: {0, 1}) {
      blur_prg[i] = std::make_shared<gl::program>();

      *blur_prg[i] << vsh;
      *blur_prg[i] << (i ? blur_y : blur_x);

      blur_prg[i]->bind_attrib("in_pos", 0);
      blur_prg[i]->bind_frag("out_color", 0);

      if (!blur_prg[i]->link()) {
        throw std::runtime_error("Could not link blur program");
      }
    }
  } else {
    fb_prg = nullptr;
    blur_prg[0] = nullptr;
    blur_prg[1] = nullptr;
  }


  if (ssao != NO_SSAO) {
    gl::shader ssao_fsh(gl::shader::FRAGMENT), ssao_blur_x(gl::shader::FRAGMENT), ssao_blur_y(gl::shader::FRAGMENT), ssao_apply_fsh(gl::shader::FRAGMENT);

    ssao_fsh.load(find_resource_file(ssao == HQ_SSAO ? "ssao_frag.glsl" : "ssao_lq_frag.glsl").c_str());
    ssao_blur_x.load(find_resource_file("ssao_blur_x_frag.glsl").c_str());
    ssao_blur_y.load(find_resource_file("ssao_blur_y_frag.glsl").c_str());
    ssao_apply_fsh.load(find_resource_file("ssao_apply_frag.glsl").c_str());

    if (!ssao_fsh.compile() || !ssao_blur_x.compile() || !ssao_blur_y.compile() || !ssao_apply_fsh.compile()) {
      throw std::runtime_error("Could not compile SSAO shaders");
    }

    for (std::shared_ptr<gl::program> *prg: {&ssao_prg, &ssao_blur_prg[0], &ssao_blur_prg[1], &ssao_apply_prg}) {
      *prg = std::make_shared<gl::program>();

      **prg << vsh;
      **prg << (prg == &ssao_prg         ? ssao_fsh :
                prg == &ssao_apply_prg   ? ssao_apply_fsh :
                prg == &ssao_blur_prg[0] ? ssao_blur_x :
                                           ssao_blur_y);

      (*prg)->bind_attrib("in_pos", 0);

      if (prg == &ssao_apply_prg) {
        (*prg)->bind_frag("out_mi", 0);
        (*prg)->bind_frag("out_hi", 1);
      } else {
        (*prg)->bind_frag("out_color", 0);
      }

      if (!(*prg)->link()) {
        throw std::runtime_error("Could not link SSAO program");
      }
    }
  } else {
    ssao_prg = nullptr;
    ssao_blur_prg[0] = nullptr;
    ssao_blur_prg[1] = nullptr;
    ssao_apply_prg = nullptr;
  }


  gl::shader char_vsh(gl::shader::VERTEX), char_fsh(gl::shader::FRAGMENT);

  char_vsh.load(find_resource_file("char_vert.glsl").c_str());
  char_fsh.load(find_resource_file("char_frag.glsl").c_str());

  if (!char_vsh.compile() || !char_fsh.compile()) {
    throw std::runtime_error("Could not compile character rendering shaders");
  }

  char_prg = std::make_shared<gl::program>();

  *char_prg << char_vsh;
  *char_prg << char_fsh;

  char_prg->bind_attrib("in_pos", 0);

  char_prg->bind_frag("out_mi", 0);
  char_prg->bind_frag("out_hi", 1);

  if (!char_prg->link()) {
    throw std::runtime_error("Could not link character rendering program");
  }


  bitmap_font = std::make_shared<gl::texture>(find_resource_file("font.png").c_str());

  if (ssao != NO_SSAO) {
    ssao_noise = std::make_shared<gl::texture>(find_resource_file("ssao-noise.png").c_str());
  } else {
    ssao_noise = nullptr;
  }


  fb_vertices = std::make_shared<gl::vertex_array>();
  fb_vertices->set_elements(4);

  vec2 fb_vertex_positions[] = {
    vec2(-1.f, 1.f), vec2(-1.f, -1.f), vec2(1.f, 1.f), vec2(1.f, -1.f)
  };

  gl::vertex_attrib *va_p = fb_vertices->attrib(0);
  va_p->format(2);
  va_p->data(fb_vertex_positions);
  va_p->load();
}

void GlRenderer::visualize_model( GlutWindow& w )
{
  const gl::texture *color_mi, *color_hi;

  if (has_bloom()) {
    // clear normal color texture with white

    fb->mask(1);
    fb->bind();

    glClearColor(.8f, .8f, .8f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // clear bright color texture with black

    fb->unmask(1);
    fb->mask(0);
    fb->bind();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    fb->unmask(0);
    fb->bind();

    color_mi = &(*fb)[0];
    color_hi = &(*fb)[1];
  } else {
    gl::framebuffer::unbind();

    glClearColor(.8f, .8f, .8f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    color_mi = nullptr;
    color_hi = nullptr;
  }

  glViewport(0, 0, width, height);


  cam = mat4::identity().translated(cam_pos);

  // render routines for game objects
  flappy_box::view::PaddleGlDrawable *paddle = nullptr;

  for (auto o: game_model()->objects()) {
    auto drawable = o->getData<Drawable>();
    if (!drawable) {
      //std::clog << "::view::GlRenderer::visualize_model: Adding new Drawable for \"" << o->name() << "\"." << std::endl;
      drawable = _drawable_factory.create_for(o);
      o->registerData(drawable);
    }

    if (drawable) {
      if (typeid(*drawable) == typeid(flappy_box::view::PaddleGlDrawable)) {
        // the paddle (or rather the vortices) should be drawn last
        paddle = dynamic_cast<flappy_box::view::PaddleGlDrawable *>(drawable.get());
      } else if (typeid(*drawable) != typeid(flappy_box::view::ExplosionGlDrawable)) {
        // explosions should be drawn as late as possible as well
        drawable->visualize(*this, w);
      }
    }
  }


  // SSAO calculation needs to be done before the vortex. But this engine design
  // is dumb as hell (as everyone knew from the start, but hey), so we can't do
  // that without removing the paddle from SSAO calculations (because vortex and
  // paddle arent' seperable. Great.
  // Lucky for us, the paddle basically is the light source, so not having an
  // SSAO shadow there won't hurt too much.
  if (ssao != NO_SSAO) {
    if (ssao == LQ_SSAO) {
      glViewport(0, 0, width / 2, height / 2);
    }

    ssao_prg->use();
    ssao_fb->bind();

    fb->depth().bind();
    ssao_noise->bind();

    ssao_prg->uniform<gl::texture>("depth_tex") = fb->depth();
    ssao_prg->uniform<gl::texture>("noise_tex") = *ssao_noise;
    ssao_prg->uniform<vec2>("epsilon") = vec2(1.f / width, 1.f / height);
    ssao_prg->uniform<mat4>("proj") = proj;
    ssao_prg->uniform<mat4>("inv_proj") = proj.inverse();

    glClear(GL_DEPTH_BUFFER_BIT);

    fb_vertices->draw(GL_TRIANGLE_STRIP);


    const gl::texture *input_tex = &(*ssao_fb)[0];
    for (int i = 0, cur_fb = 0; i < (ssao == HQ_SSAO ? 4 : 2); i++, cur_fb ^= 1) {
      ssao_blur_prg[cur_fb]->use();
      ssao_blur_fbs[cur_fb]->bind();

      input_tex->bind();
      fb->depth().bind();
      ssao_blur_prg[cur_fb]->uniform<gl::texture>("input_tex") = *input_tex;
      ssao_blur_prg[cur_fb]->uniform<gl::texture>("depth_tex") = fb->depth();
      ssao_blur_prg[cur_fb]->uniform<float>("epsilon") = (2.f - i / 2) / (cur_fb ? height : width);

      glClear(GL_DEPTH_BUFFER_BIT);

      fb_vertices->draw(GL_TRIANGLE_STRIP);

      input_tex = &(*ssao_blur_fbs[cur_fb])[0];
    }


    glViewport(0, 0, width, height);

    ssao_apply_prg->use();
    ssao_output_fb->bind();

    input_tex->bind();
    (*fb)[0].bind();
    (*fb)[1].bind();
    fb->depth().bind();

    ssao_apply_prg->uniform<gl::texture>("ssao") = *input_tex;
    ssao_apply_prg->uniform<gl::texture>("fb_mi") = (*fb)[0];
    ssao_apply_prg->uniform<gl::texture>("fb_hi") = (*fb)[1];
    ssao_apply_prg->uniform<gl::texture>("depth_tex") = fb->depth();

    glClear(GL_DEPTH_BUFFER_BIT);

    fb_vertices->draw(GL_TRIANGLE_STRIP);

    glClear(GL_DEPTH_BUFFER_BIT);


    color_mi = &(*ssao_output_fb)[0];
    color_hi = &(*ssao_output_fb)[1];
  }


  // Draw immaterial stuff which is irrelevant for SSAO (and the paddle)


  // since the paddle has a solid part, draw it first (and hope for the best)
  if (paddle) {
    paddle->visualize(*this, w);
  }

  for (const std::shared_ptr<model::GameObject> &o: game_model()->objects()) {
    std::shared_ptr<Drawable> drawable = o->getData<Drawable>();

    if (drawable && (typeid(*drawable) == typeid(flappy_box::view::ExplosionGlDrawable))) {
      drawable->visualize(*this, w);
    }
  }


  if (!log.empty()) {
    // Remove old entries
    float timestep_sec = _game_model->timestep().count();
    for (auto it = log.begin(); it != log.end();) {
      it->lifetime -= timestep_sec;
      if (it->lifetime <= 0.f) {
        it = log.erase(it);
      } else {
        ++it;
      }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Display other entries
    float line_y = -1.f + log.size() * char_size.y();
    for (const LogEntry &e: log) {
      float col = minimum(1.f, e.lifetime);
      render_line(vec2(-1.f, line_y), e.msg.c_str(), vec3(col, col, col));

      line_y -= char_size.y();
    }

    glDisable(GL_BLEND);
  }


  if (bloom_blur_passes) {
    glViewport(0, 0, width / 2, height / 2);

    const gl::texture *input_tex = color_hi;
    for (int i = 0, cur_fb = 0; i < bloom_blur_passes * 2; i++, cur_fb ^= 1) {
      blur_prg[cur_fb]->use();
      blur_fbs[cur_fb]->bind();

      input_tex->bind();
      blur_prg[cur_fb]->uniform<gl::texture>("input_tex") = *input_tex;
      blur_prg[cur_fb]->uniform<float>("epsilon") = exp2((6 - i / 2) / 2.f) / (cur_fb ? height / 2 : width / 2);

      glClear(GL_DEPTH_BUFFER_BIT);

      fb_vertices->draw(GL_TRIANGLE_STRIP);

      input_tex = &(*blur_fbs[cur_fb])[0];
    }


    gl::framebuffer::unbind();
    glViewport(0, 0, width, height);

    glClear(GL_DEPTH_BUFFER_BIT);

    color_mi->bind();
    input_tex->bind();

    fb_prg->use();

    fb_prg->uniform<gl::texture>("fb_mi") = *color_mi;
    fb_prg->uniform<gl::texture>("fb_hi") = *input_tex;

    fb_vertices->draw(GL_TRIANGLE_STRIP);
  }


  glutSwapBuffers();
}


void GlRenderer::render_character(vec2 pos, unsigned char c, vec3 color)
{
  char_prg->use();
  char_prg->uniform<vec2>("position") = pos + vec2(char_size.x() / 2.f, -char_size.y() / 2.f);
  char_prg->uniform<vec2>("size") = char_size / 2.f;
  char_prg->uniform<vec2>("char_position") = vec2(c % 16, c / 16);
  char_prg->uniform<vec3>("color") = color;

  bitmap_font->bind();
  char_prg->uniform<gl::texture>("font") = *bitmap_font;

  fb_vertices->draw(GL_TRIANGLE_STRIP);
}


void GlRenderer::render_line(vec2 pos, const char *string, vec3 color)
{
  while (*string) {
    int chr = *string;
    if (chr & 0x80) {
      if ((chr & 0xe0) == 0xc0) {
        if ((string[1] & 0xc0) == 0x80) {
          chr = ((chr & ~0xe0) << 6) | (string[1] & ~0xc0);
          string += 2;
        } else {
          chr = '?';
          string++;
        }
      } else {
        chr = '?';
        string++;
      }
    } else {
      string++;
    }

    if (chr > 255) {
      chr = '?';
    }

    render_character(pos, chr, color);
    pos.x() += 6.f / 5.f * char_size.x();
  }
}


vec2 GlRenderer::line_size(const char *s) const
{
  size_t mbslen = 0;

  while (*s) {
    if ((*s & 0x80) && ((*s & 0xe0) == 0xc0) && ((s[1] & 0xc0) == 0x80)) {
      s++;
    }

    mbslen++;
    s++;
  }

  return vec2(mbslen * 6.f / 5.f * char_size.x(), char_size.y());
}


void GlRenderer::log_add(const std::string &msg)
{
  log.emplace_back(msg, 10.f);
}


void GlRenderer::resize(GlutWindow &win)
{
  width = win.width();
  height = win.height();

  if (has_bloom()) {
    fb->resize(width, height);
    blur_fbs[0]->resize(width / 2, height / 2);
    blur_fbs[1]->resize(width / 2, height / 2);
  }

  if (ssao == HQ_SSAO) {
    ssao_fb->resize(width, height);
    ssao_blur_fbs[0]->resize(width, height);
    ssao_blur_fbs[1]->resize(width, height);
    ssao_output_fb->resize(width, height);
  } else if (ssao == LQ_SSAO) {
    ssao_fb->resize(width / 2, height / 2);
    ssao_blur_fbs[0]->resize(width / 2, height / 2);
    ssao_blur_fbs[1]->resize(width / 2, height / 2);
    ssao_output_fb->resize(width, height);
  }

  proj = mat4::projection(static_cast<float>(M_PI) / 3.f, static_cast<float>(width) / height, 1.f, 1000.f);

  // FIXME: This depends on the character aspect
  char_size = vec2(.025f, .05f * width / height);
}
