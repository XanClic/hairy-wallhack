#include <dake/gl/gl.hpp>
#include <dake/gl/framebuffer.hpp>
#include <dake/gl/shader.hpp>
#include <dake/gl/texture.hpp>
#include <dake/gl/vertex_array.hpp>
#include <dake/gl/vertex_attrib.hpp>
#include <dake/math/matrix.hpp>

#include <memory>
#include <cstdio>

#include "math.hpp"

#include "view/gl_renderer.hpp"
#include "view/glut_window.hpp"
#include "flappy_box/view/explosion_gl_drawable.hpp"
#include "flappy_box/view/paddle_gl_drawable.hpp"

#include "GL/freeglut.h"


using namespace dake;
using namespace dake::math;
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


void GlRenderer::parameters(long passes, bool bloom_lq)
{
    bloom_use_lq = bloom_lq;
    bloom_blur_passes = passes;
}


void GlRenderer::init_with_context(void)
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);


  fb = std::make_shared<gl::framebuffer>(2);

  // TMU 0 will be used by the blur FBO textures later; they "replace" the
  // second texture of this FBO
  (*fb)[0].set_tmu(1);
  (*fb)[1].set_tmu(0);


  blur_fbs[0] = std::make_shared<gl::framebuffer>(1);
  blur_fbs[1] = std::make_shared<gl::framebuffer>(1);

  (*blur_fbs[0])[0].filter(GL_LINEAR);
  (*blur_fbs[1])[0].filter(GL_LINEAR);


  if (!bloom_use_lq) {
    fb->color_format(1, GL_R11F_G11F_B10F);
    blur_fbs[0]->color_format(0, GL_R11F_G11F_B10F);
    blur_fbs[1]->color_format(0, GL_R11F_G11F_B10F);
  }


  gl::shader vsh(gl::shader::VERTEX), fsh(gl::shader::FRAGMENT);

  vsh.load("res/fb_vert.glsl");
  fsh.load("res/fb_combine_frag.glsl");

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

  blur_x.load("res/fb_blur_x_frag.glsl");
  blur_y.load("res/fb_blur_y_frag.glsl");

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


  gl::shader char_vsh(gl::shader::VERTEX), char_fsh(gl::shader::FRAGMENT);

  char_vsh.load("res/char_vsh.glsl");
  char_fsh.load("res/char_fsh.glsl");

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


  bitmap_font = std::make_shared<gl::texture>("res/font.png");


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
  if (bloom_blur_passes) {
    // clear normal color texture with white

    fb->mask(1);
    fb->bind();

    glClearColor(.7f, .7f, .7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // clear bright color texture with black

    fb->unmask(1);
    fb->mask(0);
    fb->bind();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    fb->unmask(0);
    fb->bind();
  } else {
    gl::framebuffer::unbind();

    glClearColor(.7f, .7f, .7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


  if (bloom_blur_passes) {
    glViewport(0, 0, width / 2, height / 2);

    const gl::texture *input_tex = &(*fb)[1];
    for (int i = 0, cur_fb = 0; i < bloom_blur_passes * 2; i++, cur_fb ^= 1) {
      blur_prg[cur_fb]->use();
      blur_fbs[cur_fb]->bind();

      input_tex->bind();
      blur_prg[cur_fb]->uniform<gl::texture>("input_tex") = *input_tex;
      blur_prg[cur_fb]->uniform<float>("epsilon") = exp2((8 - i / 2) / 2.f) / (cur_fb ? height : width);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      fb_vertices->draw(GL_TRIANGLE_STRIP);

      input_tex = & (*blur_fbs[cur_fb])[0];
    }


    gl::framebuffer::unbind();
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fb_prg->use();

    (*fb)[0].bind();
    input_tex->bind();

    fb_prg->uniform<gl::texture>("fb_mi") = (*fb)[0];
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


void GlRenderer::resize(GlutWindow &win)
{
  width = win.width();
  height = win.height();

  fb->resize(width, height);
  blur_fbs[0]->resize(width / 2, height / 2);
  blur_fbs[1]->resize(width / 2, height / 2);

  proj = mat4::projection(static_cast<float>(M_PI) / 3.f, static_cast<float>(width) / height, 1.f, 1000.f);

  // FIXME: This depends on the character aspect
  char_size = vec2(.025f, .05f * width / height);
}
