/*
 * Copter Commander - A 2d network helicopter game
 * Copyright (C) 2001  David Morse
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <GL/glx.h> /* includes the necessary X11 headers */
#include <assert.h>
#include <stdlib.h> /* for exit */
#include "coco.h"

char *coco_gnu_info_string = "For\n"
"more information about these matters\n"
"press [L] to print the licence to the\n"
"standard output.";
char *coco_low_tech_menu = "Press [P] to play.\nPress [ESC] to quit.";

coco_bool coco_use_keyboard_for_gui_p = COCO_TRUE;

const char *coco_gui_description_string = "GLX";

/* keys and keystates _____________________________________ */

static struct coco_keyboard_pointer_state coco_input_state = { 0 };

/* Get the (statically allocated) current key state.
   May be altered from call to call of handle-events. */
coco_key_state_ref coco_get_key_state( void )
{
  return coco_input_state.key_state;
}

/* get the (statically allocated) current input state.
   May be altered from call to call of handle-events. */
struct coco_keyboard_pointer_state *coco_get_keyboard_pointer_state()
{
  return &coco_input_state;
}



/* ________________________________________________________ */

#define COCO_X_EVENT_MASK ExposureMask | ButtonPressMask | \
      StructureNotifyMask | KeyPressMask | \
      KeyReleaseMask | ButtonPressMask | \
      ButtonReleaseMask | PointerMotionMask | \
      EnterWindowMask | LeaveWindowMask

static Display *xdisplay = NULL;
static Window xwindow = 0;
static int xscreennum;
static Atom x_wm_protocols_atom = None;
static Atom x_wm_delete_atom = None;

void coco_load_font( struct coco_font *font );

/* returns -1 if X isn't running yet, otherwise the display's fd */
int 
coco_get_x_file_descriptor( void )
{
  if (xdisplay)
    return ConnectionNumber( xdisplay );
  else return -1;
}

void 
coco_make_window( void )
{
  Window rootwindow = 0;
  GLXContext context;
  XVisualInfo *visual = NULL;
  { /* X-display */
    int ignore1, ignore2;
    xdisplay = XOpenDisplay( NULL );
    if (NULL == xdisplay)
      coco_fatal_printf( COCO_ERROR_NO_X_DISPLAY,
                         "Couldn't open XDisplay.\n" );
    xscreennum = DefaultScreen( xdisplay );
    if (!glXQueryExtension( xdisplay, &ignore1, &ignore2 ))
      coco_fatal_printf( COCO_ERROR_NO_GL,
                    "X Server doesn't support OpenGL.  "
                    "Maybe Mesa is not installed?\n" );
    rootwindow = RootWindow( xdisplay, xscreennum );
  }
  { /* Visual selection ... */
    int db_attribs[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
    int sb_attribs[] = { GLX_RGBA, None };
    if (!coco_command_line_inhibit_double_buffer)
      visual = glXChooseVisual( xdisplay, xscreennum, db_attribs );
    if (NULL == visual)
      {
        coco_double_buffer = COCO_FALSE;
        visual = glXChooseVisual( xdisplay, xscreennum, sb_attribs );
        if (NULL == visual)
          coco_fatal_printf( COCO_ERROR_NO_VISUAL, 
                        "Coudn't find rgba visual.  "
                        "Maybe try setting screen bits-per-pixel higher "
                        "in /etc/X11/XF86Config.\n" );
      }
  }
  { /* glX context */
    context = glXCreateContext( xdisplay, visual, NULL, True );
    if (NULL == context)
      coco_fatal_printf( COCO_ERROR_GLX_CONTEXT_PROBLEMS,
                    "Couldn't create a glX rendering context.  "
                    "Your OpenGL is not healthy.\n" );
  }
  { /* Set Window Attributes */
    XSetWindowAttributes swa;
    XSizeHints sh = { PSize | PMinSize | PMaxSize, 0, 0,
                      COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H,
                      COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H,
                      COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H };
    swa.colormap = XCreateColormap( xdisplay, rootwindow, visual->visual,
                                    AllocNone );
    swa.border_pixel = 0;
    swa.event_mask = COCO_X_EVENT_MASK;
    xwindow = XCreateWindow( xdisplay, rootwindow, 0, 0, 
                             COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H,
                             0, visual->depth, InputOutput, visual->visual, 
                             CWBorderPixel | CWColormap | CWEventMask, &swa );
    /* This returns an int, but I can't find any doc on what it means. */
    coco_cookie( "xwindow=%d\n", xwindow );
    XSetStandardProperties( xdisplay, xwindow, coco_product_name, 
                            coco_product_name, None, NULL, 0, &sh );
    if (!glXMakeCurrent( xdisplay, xwindow, context ))
      coco_fatal_printf( COCO_ERROR_GLX_CONTEXT_PROBLEMS, 
                         "Couldn't make glXContext current.  "
                         "Check your OpenGL/X/video-card installation.\n");

    /* for some reason this only disables the close button,
     * without sending me ClientMessage events.  Crud.
     *
     * x_wm_protocols_atom = XInternAtom( xdisplay, "WM_PROTOCOLS", True);
     * x_wm_delete_atom = XInternAtom( xdisplay, "WM_DELETE_WINDOW", True );
     * if (None==x_wm_protocols_atom)
     *   coco_cookie( "argh, None" );
     * if (None==x_wm_delete_atom)
     *   coco_cookie( "argh, None" );
     * if (0==XSetWMProtocols( xdisplay, xwindow, &x_wm_delete_atom, 1 ))
     *   coco_warn_printf( "XSetWMProtocols had problems interning atoms.\n" );
     */
    XMapWindow( xdisplay, xwindow );
    XFlush( xdisplay );
    coco_setup_matricies( COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H );
    coco_setup_misc_graphics();

    coco_load_font( &coco_default_font );
    coco_load_font( &coco_huge_font );
    coco_load_font( &coco_fixed_font );
    /* not yet, wait for more initialization: */
    /* coco_handle_ui_events( NULL ); */
  }
}

void 
coco_kill_window( void )
{
  if (xwindow)
    XDestroyWindow( xdisplay, xwindow );
}


/* -1 is returned if something goes wrong, (it WILL go wrong) */
static int
coco_keysym_to_coco_key( KeySym keysym )
{
  /* translate the punta with extreme prejudice, not trusting XLookupString */
  if (((XK_space<=keysym) && (keysym<=XK_at))
      || ((XK_bracketleft<=keysym) && (keysym<=XK_asciitilde)))
    return keysym - XK_space + COCO_KEY_SPACE;
  if ((XK_BackSpace==keysym)||(XK_Delete==keysym)||(XK_KP_Delete==keysym))
    return COCO_KEY_DELETE;
  if ((XK_Tab==keysym)||(XK_KP_Tab==keysym)||(XK_ISO_Left_Tab==keysym))
    return COCO_KEY_TAB;
  if ((XK_Return==keysym)||(XK_KP_Enter==keysym)||(XK_ISO_Enter==keysym))
    return COCO_KEY_RETURN;
  if ((XK_Escape==keysym)||(XK_Cancel==keysym))
    return COCO_KEY_ESCAPE;
  if ((XK_A<=keysym)&&(keysym<=XK_Z)) /* dammned caps roaches */
    return keysym - XK_A + COCO_CHAR_TO_KEY( 'a' );
  if ((XK_Shift_L==keysym)||(XK_Shift_R==keysym))
    return COCO_KEY_SHIFT;
  if ((XK_Control_L==keysym)||(XK_Control_R==keysym))
    return COCO_KEY_CONTROL;
  if ((XK_Meta_L==keysym)||(XK_Meta_R==keysym))
    return COCO_KEY_META;
  if ((XK_Alt_L==keysym)||(XK_Alt_R==keysym))
    return COCO_KEY_ALT;
  if ((XK_Super_L==keysym)||(XK_Super_R==keysym))
    return COCO_KEY_SUPER;
  if ((XK_Hyper_L==keysym)||(XK_Hyper_R==keysym))
    return COCO_KEY_HYPER;
  if ((XK_KP_Space==keysym))
    return COCO_KEY_SPACE;
  if (XK_Left==keysym)
    return COCO_KEY_LEFT_ARROW;
  if (XK_Right==keysym)
    return COCO_KEY_RIGHT_ARROW;
  if (XK_Up==keysym)
    return COCO_KEY_UP_ARROW;
  if (XK_Down==keysym)
    return COCO_KEY_DOWN_ARROW;
  return -1;
}



/* because of the type of game we're playing, coordinates are relative to
   (COCO_OFFICIAL_WINDOW_W/2, 0), where y=0 implies about where the ground
   is.  */
/* NOT USED
void 
coco_get_pointer_coordinates( int *x, int *y )
{
  if (x) *x = coco_input_state.pointer_x;
  if (y) *y = coco_input_state.pointer_y;
}
*/

/* change buckies 'n' buttons in keystate */
static void
coco_update_bnb_from_xstate( coco_key_state_ref keystate, unsigned int xstate,
                             coco_bool do_mouse_buttons_too )
{
  unsigned int xmask, key;
  if (do_mouse_buttons_too)
    for (xmask=Button1Mask, key=COCO_LPOINTER;
         xmask<=Button5Mask; 
         xmask<<=1, key++)
      coco_set_bit( keystate, key, xmask & xstate );
  for (xmask=ControlMask, key=COCO_KEY_CONTROL;
       xmask<=Mod5Mask; 
       xmask<<=1, key++)
    coco_set_bit( keystate, key, xmask & xstate );
  coco_set_bit( keystate, COCO_KEY_SHIFT, ShiftMask & xstate );
}

void
coco_poll_pointer( void )
{
  Window rw, cw;
  int rx, ry, wx, wy;
  unsigned int xstate;
  if (XQueryPointer( xdisplay, xwindow, &rw, &cw, &rx, 
                     &ry, &wx, &wy, &xstate ))
    {
      coco_input_state.client_time
        = COCO_U32_ADD_MODULAR( coco_input_state.client_time );
      coco_input_state.pointer_x = wx - (COCO_OFFICIAL_WINDOW_W>>1);
      coco_input_state.pointer_y = COCO_OFFICIAL_WINDOW_H - wy;
      coco_update_bnb_from_xstate( coco_input_state.key_state, xstate, 
                                   COCO_TRUE );
    }
}

static coco_bool coco_mouse_in_window_p_private = COCO_FALSE;

coco_bool coco_mouse_in_window_p()
{
  return coco_mouse_in_window_p_private;
}

void
coco_handle_ui_events( void )
{
  int need_redraw = 0;
  XEvent xevent;
  coco_key_state_ref keystate = coco_input_state.key_state;
  /* coco_cookie( "About to XCheckWindowEvent\n" ); */
  /* coco_time_cookie( "events" ); */
  while (True == XCheckMaskEvent( xdisplay, -1L, &xevent ))
    {
      /* coco_cookie( "XCheckWindowEvent found event\n" ); */
      switch (xevent.type) {
      case ButtonRelease: /* all these events are exactly the same */
      case ButtonPress:   /* except KeyPress/Release have a */
      case KeyRelease:    /* keycode field. */
      case KeyPress:
      case MotionNotify:
      case EnterNotify:
      case LeaveNotify:
        {
          coco_bool iskeyevent
            = (xevent.type == KeyPress) || (xevent.type == KeyRelease);
          /* because of the type of game we're playing, coordinates are
             relative to (COCO_OFFICIAL_WINDOW_W/2, 0), where y=0 implies
             about where the ground is.  */
          coco_input_state.client_time
            = COCO_U32_ADD_MODULAR( coco_input_state.client_time );
          coco_input_state.pointer_x
            = xevent.xbutton.x - (COCO_OFFICIAL_WINDOW_W>>1);
          coco_input_state.pointer_y
            = COCO_OFFICIAL_WINDOW_H - xevent.xbutton.y;
          coco_update_bnb_from_xstate( keystate, xevent.xbutton.state, 
                                       COCO_TRUE );
          if (iskeyevent)
            {
              KeySym keysym;
              int key;
              int ispress = xevent.type == KeyPress;
              keysym = XLookupKeysym( &(xevent.xkey), 0 );
              key = coco_keysym_to_coco_key( keysym );
              if (key != -1)
                {
                  coco_bool wasset = coco_get_bit( keystate, (coco_key) key );
                  coco_set_bit( keystate, key, ispress );
                  if (ispress)
                    { 
                      /* str is cheesey hack.  Should use xlookupstring or
                         something. */
                      char str[2];
                      str[0] = COCO_KEY_TO_CHAR( key );
                      str[1] = '\0';
                      if (!wasset) 
                        coco_key_press( key, str, keystate );
                    }
                  else coco_key_release( key, keystate );
                }
            }
          else if ((xevent.type == ButtonPress) 
                   || (xevent.type == ButtonRelease))
            {
              int ispress = xevent.type == ButtonPress;
              coco_key key = COCO_LPOINTER + xevent.xbutton.button - Button1;
              assert( (key >= COCO_LPOINTER) && (key <= COCO_5POINTER) );
              coco_set_bit( keystate, key, ispress );
              if (ispress) coco_key_press( key, NULL, keystate );
              else coco_key_release( key, keystate );
            }
          else if (xevent.type == EnterNotify)
            coco_mouse_in_window_p_private = COCO_TRUE;
          else if (xevent.type == LeaveNotify)
            coco_mouse_in_window_p_private = COCO_FALSE;
        }
        break;
      case Expose:
        need_redraw = 1;
        break;
      case ConfigureNotify:
        /* coco_cookie( "Got ConfigureNotify event\n" ); */
        coco_real_window_w = xevent.xconfigure.width;
        coco_real_window_h = xevent.xconfigure.height;
        coco_setup_matricies( xevent.xconfigure.width, 
                              xevent.xconfigure.height );
        break;
      case ClientMessage:
        if ((None != x_wm_protocols_atom)
            && (None != x_wm_delete_atom)
            && (x_wm_protocols_atom == xevent.xclient.message_type)
            && (x_wm_delete_atom == xevent.xclient.data.l[0]))
          exit( 0 );
          break;
      case DestroyNotify:
        coco_client_shutdown( COCO_TRUE );
        break;
      default:
        break;
      }
    }
  /* coco_poll_pointer(); */
  if (need_redraw && !coco_get_inhibit_refresh())
    coco_refresh_window();
}

void 
coco_swap_buffer( void )
{
  if (coco_double_buffer)
    glXSwapBuffers( xdisplay, xwindow );
  else glFlush();
}

static coco_bool coco_pointer_grabbed_flag = COCO_FALSE;

coco_bool coco_get_pointer_grabbed_flag()
{
  return coco_pointer_grabbed_flag;
}

coco_bool 
coco_grab_pointer( void )
{
  coco_pointer_grabbed_flag |= GrabSuccess == 
    XGrabPointer( xdisplay, xwindow, True,
                  ButtonPressMask | ButtonReleaseMask | PointerMotionMask
                  | EnterWindowMask | LeaveWindowMask,
                  GrabModeAsync, GrabModeAsync, xwindow, None, CurrentTime);
  return coco_pointer_grabbed_flag;
}

void 
coco_ungrab_pointer( void )
{
  XUngrabPointer( xdisplay, CurrentTime );
  XFlush( xdisplay );
  coco_pointer_grabbed_flag = COCO_FALSE;
}

/* here, we use the implementation_datum slot directly as the xfontstruct. */
void 
coco_load_font( struct coco_font *font )
{
  font->implementation_datum = XLoadQueryFont( xdisplay, font->name );
  if (NULL == font->implementation_datum)
    {
      coco_warn_printf( "Couldn't load font \"%s\", using \"9x15\" instead.",
                   font->name );
      font->implementation_datum = XLoadQueryFont( xdisplay, "9x15" );
      if (NULL == font->implementation_datum)
        coco_fatal_printf( COCO_ERROR_NO_FONT, "Couldn't even find \"9x15\"." );
    }
  font->list_base = glGenLists( 256 );
  if (0 == font->list_base)
    coco_fatal_printf( COCO_ERROR_DISPLAY_LISTS,
                  "Problems allocating OpenGL display lists for fonts." );
  glXUseXFont( ((XFontStruct *) font->implementation_datum)->fid, 
               0, 256, font->list_base );
}

int 
coco_font_height( struct coco_font *font )
{
  XFontStruct *xfs = font->implementation_datum;
  return xfs->ascent + xfs->descent;
}

/* sets w and h to be the bounding box for the specified string, taking
   into account multi-line issues.  Ascent is not multi-line aware, (what
   would that mean?), it simply is set to the ascent of the font as a
   whole.  */
void 
coco_font_string_dimensions( const char *string, int nchars_or_minus_one,
                             struct coco_font *font,
                             int *w, int *h, int *ascent )
{
  XFontStruct *xfs = font->implementation_datum;
  int i;
  int cursor = 0;
  int line_height = xfs->ascent + xfs->descent;
  int hacc = line_height;
  int wmax = 0;
  int chars_counted = 0;
  char ch;
  if (w||h)
    while (1)
      {
        int ww = 0;
        XCharStruct *per_char = xfs->per_char;
        int char_min = xfs->min_char_or_byte2;
        int char_max = xfs->max_char_or_byte2;
        i = cursor;
        while (1)
          { 
            XCharStruct *xcs;
            ch=string[i];
            if ((ch=='\0') || (ch=='\n')
                || ((nchars_or_minus_one!=-1)
                    && (chars_counted >= nchars_or_minus_one))) 
              break;
            i++;
            xcs = per_char
                   ? (((char_min <= ch) && (ch <= char_max))
                      ? (per_char + ch - char_min)
                      : (per_char + xfs->default_char - char_min))
                   : &(xfs->max_bounds);
            /*coco_cookie( "xcs->width=%d\n", xcs->width );*/
            ww += xcs->width;
            chars_counted++;
          };
        /* Be nice if XTextWidth didn't return 0 in XFree... */
        /* ww = XTextWidth( xfs, string+cursor, cursor-i ); */
        if (wmax < ww) wmax = ww;
        if (ch=='\0') break;
        /* hm, looks to me like should also break if
        ((nchars_or_minus_one!=-1) && (chars_counted >=
        nchars_or_minus_one)), but it should just overscann the string.
        Anyway I put it in the gnome code. */
        if (string[i+1] != '\0') hacc += line_height;
        cursor = i + 1;
      }
  if (w) *w = wmax;
  if (h) *h = hacc;
  if (ascent) *ascent = xfs->ascent;
}


/*
int 
coco_send_garbage_key( void )
{
  XEvent event;
  event.type = KeyRelease;
  event.xkey.serial = 0;
  event.xkey.send_event = True;
  event.xkey.display = xdisplay;
  event.xkey.window = xwindow;
  event.xkey.root = RootWindow( xdisplay, DefaultScreen( xdisplay ) ) ;
  event.xkey.subwindow = xwindow;
  event.xkey.time = 0;
  event.xkey.x = 0;
  event.xkey.y = 0;
  event.xkey.x_root = 0;
  event.xkey.y_root = 0;
  event.xkey.state = 0;
  event.xkey.keycode = 55555;
  event.xkey.same_screen = True;
  if (!XSendEvent( xdisplay, xwindow, False, 0, &event ))
    coco_warn_printf( "XSendEvent failed: couldn't send ourselves "
                      "a key.\n" );
  return 0;
}
*/

/* Instead of refreshing immediately, queue and espose for future
   refreshing. */
void
coco_refresh_window_eventually( void )
{
  if (xdisplay && xwindow)
    {
      XEvent event;
      event.type = KeyRelease;
      event.xexpose.serial = 0;
      event.xexpose.send_event = True;
      event.xexpose.display = xdisplay;
      event.xexpose.window = xwindow;
      event.xexpose.x = 0;
      event.xexpose.y = 0;
      event.xexpose.width = coco_real_window_w;
      event.xexpose.height = coco_real_window_h;
      event.xexpose.count = 0;
      if (!XSendEvent( xdisplay, xwindow, False, 0, &event ))
        coco_warn_printf( "XSendEvent failed: couldn't send ourselves "
                          "an expose.\n" );
    }    
}

void
coco_notify_version_incompatible( int major, int minor )
{
  coco_fatal_printf( COCO_ERROR_SERVER_VERSION_INCOMPATIBLE,
                     "Server is running incompatible version %d.%d\n",
                     major, minor );
}
