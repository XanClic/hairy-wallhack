#include <dake/math/matrix.hpp>

#include <memory>
#include <cstring>

#include "flappy_box/view/game_over_gl_drawable.hpp"


using namespace dake::math;
using namespace view;
using namespace flappy_box::view;
using flappy_box::model::GameOver;


GameOverGlDrawable::GameOverGlDrawable(const std::shared_ptr<const GameOver> &go):
  _model(go.get())
{}


GameOverGlDrawable::~GameOverGlDrawable(void)
{}


float GameOverGlDrawable::centered_x(GlRenderer &r, const char *s)
{
  return -.5f * r.line_size(s).x();
}


void GameOverGlDrawable::render_centered_string(GlRenderer &r, float yofs, const char *s, vec3 color)
{
  r.render_line(vec2(centered_x(r, s), yofs * r.character_size().y()), s, color);
}


void GameOverGlDrawable::visualize(GlRenderer &r, GlutWindow &)
{
  // i'm so genius
  r.light_position() = vec3(0.f, 0.f, 5.f);

  render_centered_string(r, 2.f, "GAME OVER!", vec3(1.f, 0.f, 0.f));

  char points[64];
  snprintf(points, 64, "You've reached %i points.", _model->playerPoints());
  render_centered_string(r, 1.f, points);

  render_centered_string(r, -1.f, "Press R to restart");
}
