# include "flappy_box/view/box_gl_drawable.hpp"

#include <GL/freeglut.h>

# include <algorithm>

using namespace ::flappy_box::view;


BoxGlDrawable::BoxGlDrawable(const std::shared_ptr< ::flappy_box::model::Box >& b )
: _model( b )
{
  
}

BoxGlDrawable::~BoxGlDrawable()
{

}

void BoxGlDrawable::visualize( ::view::GlRenderer& r, ::view::GlutWindow& w )
{

    // TODO: Replace old rendering code with new and improved rendering - Aufgabe 5.3

  glPushMatrix();
  {
    double angle = _model->angle();
    const vec3_type& pos = _model->position();
    const double size = _model->size();
    glTranslated( pos[0], pos[1], pos[2] );   
    glRotated( angle, 0., 1., 0. );
    glScaled( size, size, size );

    glColor3d( 0.25, 0.25, 0.25 );

    ::glBegin(GL_LINES);

    ::glVertex3d(-0.5, -0.5, -0.5);
    ::glVertex3d(-0.5, -0.5,  0.5);
    ::glVertex3d(-0.5,  0.5, -0.5);
    ::glVertex3d(-0.5,  0.5,  0.5);
    ::glVertex3d( 0.5, -0.5, -0.5);
    ::glVertex3d( 0.5, -0.5,  0.5);
    ::glVertex3d( 0.5,  0.5, -0.5);
    ::glVertex3d( 0.5,  0.5,  0.5);

    ::glVertex3d(-0.5, -0.5, -0.5);
    ::glVertex3d(-0.5,  0.5, -0.5);
    ::glVertex3d( 0.5, -0.5, -0.5);
    ::glVertex3d( 0.5,  0.5, -0.5);
    ::glVertex3d(-0.5, -0.5,  0.5);
    ::glVertex3d(-0.5,  0.5,  0.5);
    ::glVertex3d( 0.5, -0.5,  0.5);
    ::glVertex3d( 0.5,  0.5,  0.5);

    ::glVertex3d(-0.5, -0.5, -0.5);
    ::glVertex3d( 0.5, -0.5, -0.5);
    ::glVertex3d(-0.5, -0.5,  0.5);
    ::glVertex3d( 0.5, -0.5,  0.5);
    ::glVertex3d(-0.5,  0.5, -0.5);
    ::glVertex3d( 0.5,  0.5, -0.5);
    ::glVertex3d(-0.5,  0.5,  0.5);
    ::glVertex3d( 0.5,  0.5,  0.5);

    ::glEnd();

  }
  glPopMatrix();
}
