# include "view/glut_window.hpp"


#include <GL/glew.h>
# include <GL/freeglut.h>

# include <iostream>

using namespace ::view;

GlutWindow::GlutWindow( const std::string& name, size_t width, size_t height, const std::shared_ptr< GlRenderer >& r, const std::shared_ptr< controller::InputEventHandler >& h )
: _name( name )
, _width( width )
, _height( height )
, _renderer( r )
, _input_event_handler( h )
{
  // GLUT GlutWindow Initialization:
  glutInitWindowSize( width, height );
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

  _glut_win_id = glutCreateWindow( name.c_str() );
  if( is_closed() ) 
    throw std::logic_error( "::view::GlutWindow::GlutWindow: Could not create GLUT window." );

  glewInit();

  glutSetWindowData( this );

  // setup callbacks
  glutDisplayFunc( glutDisplay );
  glutReshapeFunc( glutReshape );
  glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
  glutKeyboardFunc(glutKeyboard);
  glutKeyboardUpFunc(glutKeyboardUp);
  glutSpecialFunc(glutSpecial);
  glutSpecialUpFunc(glutSpecialUp);
  glutCloseFunc(glutClose);
  
  // ATTENTION: NEW CODE!
  //
}

GlutWindow::~GlutWindow()
{
  close();
}

void GlutWindow::invalidate()
{
  if( is_closed() )
  {
    std::clog << "view::GlutWindow::ensureCurrent: Window was already closed." << std::endl;
    return;
  }
  ensureCurrent();
  glutPostRedisplay();
}

void GlutWindow::ensureCurrent() const
{
  if( is_closed() )
  {
    std::clog << "view::GlutWindow::ensureCurrent: Window was already closed." << std::endl;
    return;
  }
  if( _glut_win_id != glutGetWindow() ) 
  {
    std::clog << "view::GlutWindow::ensureCurrent: Switching to window " << _name << " with id " << _glut_win_id << "…" << std::endl;
    glutSetWindow( _glut_win_id );
  }
}

unsigned int GlutWindow::width() const
{
  return _width;
}

unsigned int GlutWindow::height() const
{
  return _height;
}

std::shared_ptr< GlRenderer const > GlutWindow::renderer() const
{ 
  return _renderer; 
}

std::shared_ptr< ::controller::InputEventHandler > GlutWindow::input_event_handler()
{
  return _input_event_handler;
}

std::shared_ptr< ::controller::InputEventHandler const > GlutWindow::input_event_handler() const
{
  return _input_event_handler;
}

bool GlutWindow::is_closed() const
{
  return _glut_win_id <= 0;
}

void GlutWindow::close()
{
  if( is_closed() )
  {
    std::clog << "view::GlutWindow::close: Window was already closed." << std::endl;
    return;
  }
  glutDestroyWindow( _glut_win_id );
  _glut_win_id = -1;
}

void GlutWindow::keyboardEvent(unsigned char key, int special_key, int mouse_x, int mouse_y, bool up)
{
    if (!this->input_event_handler()) {
        std::clog << "view::GlutWindow::glutKeyboard: no InputEventHandler attached (which could handle the event)." << std::endl;
        return;
    }

    controller::InputEventHandler::keyboard_event ev;
    ev.key = static_cast<controller::InputEventHandler::keyboard_event::key_type>(key);
    ev.special_key = static_cast<controller::InputEventHandler::keyboard_event::special_key_type>(special_key);
    // ATTENTION: NEW CODE!
    ev.key_state = up
        ? controller::InputEventHandler::keyboard_event::KEY_UP
        : controller::InputEventHandler::keyboard_event::KEY_DOWN;
    //
    ev.mouse_pos[0] = mouse_x;
    ev.mouse_pos[1] = mouse_y;
    int glut_modifiers = glutGetModifiers();
    controller::InputEventHandler::keyboard_event::modifier_mask_type modifier_mask = 0;
    if (glut_modifiers & GLUT_ACTIVE_SHIFT) modifier_mask |= controller::InputEventHandler::keyboard_event::SHIFT_ACTIVE;
    if (glut_modifiers & GLUT_ACTIVE_CTRL) modifier_mask |= controller::InputEventHandler::keyboard_event::CTRL_ACTIVE;
    if (glut_modifiers & GLUT_ACTIVE_ALT) modifier_mask |= controller::InputEventHandler::keyboard_event::ALT_ACTIVE;
    ev.modifier_mask = modifier_mask;
    //TODO: fill event with more keys & infos

    this->input_event_handler()->handle(ev);
}

void GlutWindow::glutDisplay() 
{
  GlutWindow* win = reinterpret_cast< GlutWindow* >( glutGetWindowData() );
  if( ! win )
    throw std::out_of_range( "view::GlutWindow::glutDraw: Could not get pointer to GlutWindow." );

  win->renderer()->visualize_model( *win );
}

void GlutWindow::glutReshape( int width, int height )
{
  GlutWindow* win = reinterpret_cast< GlutWindow* >( glutGetWindowData() );
  if( ! win ) 
    throw std::out_of_range( "view::GlutWindow::glutReshape: Could not get pointer to GlutWindow." );

  win->_width = width;
  win->_height= height;
  win->renderer()->resize( *win ); 
}

void GlutWindow::glutKeyboard( unsigned char glut_key, int mouse_x, int mouse_y )
{
    GlutWindow* win = reinterpret_cast< GlutWindow* >(glutGetWindowData());
    if (!win)
        throw std::out_of_range("view::GlutWindow::glutKeyboard: Could not get pointer to GlutWindow.");

    win->keyboardEvent(glut_key, 0, mouse_x, mouse_y, false);
}

// ATTENTION: NEW CODE!
void GlutWindow::glutKeyboardUp( unsigned char glut_key, int mouse_x, int mouse_y )
{
    GlutWindow* win = reinterpret_cast< GlutWindow* >(glutGetWindowData());
    if (!win)
        throw std::out_of_range("view::GlutWindow::glutKeyboard: Could not get pointer to GlutWindow.");

    win->keyboardEvent(glut_key, 0, mouse_x, mouse_y, true);
}

void GlutWindow::glutSpecial(int glut_special_key, int mouse_x, int mouse_y) {
    GlutWindow* win = reinterpret_cast< GlutWindow* >(glutGetWindowData());
    if (!win)
        throw std::out_of_range("view::GlutWindow::glutKeyboard: Could not get pointer to GlutWindow.");

    win->keyboardEvent(0, glut_special_key, mouse_x, mouse_y, false);
}

void GlutWindow::glutSpecialUp(int glut_special_key, int mouse_x, int mouse_y) {
    GlutWindow* win = reinterpret_cast< GlutWindow* >(glutGetWindowData());
    if (!win)
        throw std::out_of_range("view::GlutWindow::glutKeyboard: Could not get pointer to GlutWindow.");

    win->keyboardEvent(0, glut_special_key, mouse_x, mouse_y, true);
}
//

void GlutWindow::glutClose() 
{ 
  GlutWindow* win = reinterpret_cast< GlutWindow* >( glutGetWindowData() );
  if( ! win )
    throw std::out_of_range( "view::GlutWindow::glutClose: Could not get pointer to GlutWindow." );

  if( win->is_closed() )
  {
    std::clog << "view::GlutWindow::glutClose: Window was already closed." << std::endl;
    return;
  }

  win->_glut_win_id = -1;
}

std::shared_ptr< GlRenderer > GlutWindow::renderer()
{
  return _renderer;
}

void GlutWindow::setRenderer( const std::shared_ptr< GlRenderer >& r )
{
  if( ! r ) throw std::logic_error( "::view::GlutWindow::setRenderer: Invalid renderer." );
  _renderer = r;
}