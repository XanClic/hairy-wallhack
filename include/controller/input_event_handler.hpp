# pragma once

namespace controller 
{
  class InputEventHandler
  {
    public:

      struct keyboard_event
      {
        typedef char key_type;
        typedef int special_key_type;

        // only one of the keys can be different from zero
        key_type key;
        special_key_type special_key;

        enum modifier_type 
        {   SHIFT_ACTIVE = 1
          ,  CTRL_ACTIVE = 2
          ,   ALT_ACTIVE = 4
        };
        typedef int modifier_mask_type;

        modifier_mask_type modifier_mask;
// ATTENTION: NEW CODE!
        enum key_state_type
        {
          NONE = 0,
          KEY_UP,
          KEY_DOWN
        };
        
        key_state_type key_state;
//        
        double mouse_pos[2];
      };

      // Return true if event was handled.
      virtual bool handle( const keyboard_event& ) = 0;

  }; // InputEventHandler

} // controller::
