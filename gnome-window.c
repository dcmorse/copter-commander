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

/*
From: Dave Morse <dm-gtk@bomberlan.net>
Subject: how to get backtrace on warnings?
To: gtk-app-devel-list@gnome.org
Date: Sat, 19 May 2001 18:23:55 -0700
  
  
I'm trying to figure out which line of code, of roughly 97 million, is
responsible for things like this:
  
  Gdk-ERROR **: BadValue (integer parameter out of range for operation)
    serial 5170 error_code 2 request_code 130 minor_code 9
  
I'd like to get a backtrace or something.  I tried running the app with
"--sync --g-fatal-warnings", then getting gdb to break on the call of the
function "exit", but gdb refuses to break on exit.  I tried breaking
gtk_exit, but its not called.  Ideas?

From: Iain <iain@ximian.com>
Subject: Re: how to get backtrace on warnings?
To: jrb@redhat.com
Cc: Dave Morse <dm-gtk@bomberlan.net>, gtk-app-devel-list@gnome.org
Date: 20 May 2001 01:21:24 -0400

On 19 May 2001 22:26:26 -0400, jrb@redhat.com wrote:
> Break on g_log.

Or if it's an X error like that one looks, try breaking on gdk_x_error

From: Havoc Pennington <hp@redhat.com>
Subject: Re: how to get backtrace on warnings?
To: Dave Morse <dm-gtk@bomberlan.net>
Cc: gtk-app-devel-list@gnome.org
Date: 20 May 2001 01:57:51 -0400
  
Dave Morse <dm-gtk@bomberlan.net> writes:
> I'd like to get a backtrace or something.  I tried running the app with
> "--sync --g-fatal-warnings", then getting gdb to break on the call of the
> function "exit", but gdb refuses to break on exit.  I tried breaking
> gtk_exit, but its not called.  Ideas?
> 
  
You shouldn't need to break on anything, --g-fatal-warnings will call
abort() which should stop you in the debugger.

Failing that, jrb suggested g_log, this only works on compilers with
varargs macros, g_logv should always work though.

From: Havoc Pennington <hp@redhat.com>
Subject: Re: how to get backtrace on warnings?
To: Iain <iain@ximian.com>
Cc: jrb@redhat.com, Dave Morse <dm-gtk@bomberlan.net>,
        gtk-app-devel-list@gnome.org
Date: 20 May 2001 03:47:02 -0400
  
  
Iain <iain@ximian.com> writes:
> Or if it's an X error like that one looks, try breaking on gdk_x_error
> 
  
Hmm, good point - g_warning()/g_log()/g_logv() aren't involved at
all. ;-)
  

From: Havoc Pennington <hp@redhat.com>
Subject: Re: how to get backtrace on warnings?
To: Dave Morse <dm-gtk@bomberlan.net>
Cc: gtk-app-devel-list@gnome.org
Date: 20 May 2001 23:17:18 -0400
  
Dave Morse <dm-gtk@bomberlan.net> writes:
> Hmmmmmm... Still no joy.  Here's a transcript.  Its puzzling that it
> prints out "Breakpoint 1 at 0x813fc38" during the running of the program,
> but doesn't actually wake up the interactive debugging prompt.  Perhaps
> gdb can't find the source for gdk, and thus automatically continues?
  
My best explanation is "gdb on crack"
  
Soemetimes it's a bit confused by breakpoints in shared libs prior to
running the app - you might try something like:
  
(gdb) break main
(gdb) run --sync
[breaks at main]
(gdb) break gdk_x_error
  

 */


#include <gnome.h>
#include <gtkgl/gtkglarea.h> /* sold seperatly from gtk */
#include <gdk/gdk.h>   /* for GdkFont */
#include <gdk/gdkx.h>  /* evil hacking on my part -- have to get x-display */
#include <X11/Xlib.h> /* for ConnectionNumber macro -- nothing to link! */
#include <GL/gl.h>    /* for glFlush, glGenlists, etc. */
#include <ctype.h>    /* for islower */
#include <errno.h>
#include <assert.h>
#include "coco.h"
#include "gnome-kennel.h"

#define COCO_JOIN_DIALOG_Y_PAD 5
#define COCO_JOIN_DIALOG_X_PAD 5
#define COCO_PORT_EDITOR_INTERNAL_PAD 5

#define COCO_BAD_GUYS_PRETTY_NAME "Left"
#define COCO_GOOD_GUYS_PRETTY_NAME "Right"

#define COCO_UI_PAD_HORIZONTAL 5
#define COCO_UI_PAD_VERTICAL 5

/* Set by tutorial gui */
coco_bool coco_start_game_asap = COCO_FALSE;


/* Fwd defs________________________________________________ */

void coco_make_server_process( coco_bool tutorial, int left_comps, 
                               int right_comps, char *full_level_file_name,
                               int iport );
coco_bool coco_join_game_interactively( const char *server, int port, int ntry,
                                        GtkWindow *parent_window, 
                                        coco_bool join_asap );

void coco_load_font( struct coco_font *font );
GtkWidget * coco_menubar_new( GtkWidget *window1 );



static GtkWidget *coco_root_window = NULL;
static GtkWidget *coco_gl_window = NULL;

char *coco_gnu_info_string = "\n"
"Consult the `Help' menu for more\n"
"information.";
char *coco_low_tech_menu = NULL;
coco_bool coco_use_keyboard_for_gui_p = COCO_FALSE;

coco_bool coco_use_join_dialog( GtkWidget *parent_window );

coco_bool coco_use_new_game_dialog( GtkWidget *parent_window );

void coco_use_tutorial_dialog( GtkWidget *parent_window );

void coco_on_game_activate( GtkMenuItem     *menuitem,
                            gpointer         user_data);

void coco_use_string_viewer_dialog( const char *string, const char *title, 
                                    coco_bool fixed_font_p );

const char *coco_gui_description_string = "Gnome";

GtkWidget *coco_gnome_dialog_get_button (GnomeDialog *dialog, gint button );


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


int 
coco_get_x_file_descriptor( void )
{
  static int xfd = -1;
  if ((-1==xfd) && coco_root_window)
    {
      GdkWindow *gdk_win = GTK_WIDGET( coco_root_window )->window;
      Display *xdisplay = GDK_WINDOW_XDISPLAY( gdk_win );
      assert( gdk_win && xdisplay );
      coco_cookie( "XFD = %d\n", ConnectionNumber( xdisplay ) );
      xfd = ConnectionNumber( xdisplay );
      return xfd;
    }
  return xfd;
}

/* called when the user closes the window */
static gint
coco_root_delete_callback( GtkWidget *widget, GdkEvent *event, gpointer data )
{
  coco_cookie( "root_delete_callback here-and-ok\n" );
  /* return FALSE to continue closing the window */
  return FALSE;
}

static gint
coco_root_destroy_callback( GtkWidget *widget, gpointer data )
{
  coco_cookie( "root_destroy_callback here\n" );
  if (widget == coco_root_window)
    {
      gtk_object_unref( GTK_OBJECT( coco_gl_window ) );
      coco_gl_window = NULL;
      gtk_object_unref( GTK_OBJECT( coco_root_window ) );
      coco_root_window = NULL;
    }
  coco_cookie( "root_destroy_callback ok\n" );
  return 0; /* not sure what I should return.  Looks like it makes no diff. */
}

/* When widget is exposed it's contents are redrawn. */
static gint 
coco_draw_callback( GtkWidget *widget, GdkEventExpose *event )
{
  /* Draw only last expose. */
  if (event->count <= 0)
    coco_refresh_window();
  return TRUE;
}

static coco_bool coco_main_window_current = COCO_FALSE;

static gint 
coco_reshape_callback( GtkWidget *widget, GdkEventConfigure *event )
{
  if (coco_main_window_current)
    coco_setup_matricies( widget->allocation.width,
                          widget->allocation.height );
  return TRUE;
}

static gint 
coco_realize_callback( GtkWidget *widget )
{
  if (gtk_gl_area_make_current( GTK_GL_AREA( widget ) ))
    {
      coco_main_window_current = COCO_TRUE;
#ifdef COCO_DEBUG
      if ((COCO_OFFICIAL_WINDOW_W != widget->allocation.width)
          ||(COCO_OFFICIAL_WINDOW_H != widget->allocation.height))
        coco_warn_printf( "Desired size %dx%d not equal to real size %dx%d\n",
                          widget->allocation.width,
                          widget->allocation.height,
                          COCO_OFFICIAL_WINDOW_W,
                          COCO_OFFICIAL_WINDOW_H );
#endif /* COCO_DEBUG */
      coco_setup_matricies( widget->allocation.width,
                            widget->allocation.height );
      coco_setup_misc_graphics();
      coco_load_font( &coco_default_font );
      coco_load_font( &coco_huge_font );
      coco_load_font( &coco_fixed_font );
    }
  else coco_warn_printf( "Failed to make_current during realize_callback\n" );
  return TRUE;
}

/* change buckies 'n' buttons in keystate.
   Suspiciously similar to coco_update_bnb_from_xstate in glx-window.c */
static void
coco_update_bnb_from_gstate( coco_key_state_ref keystate, unsigned int xstate,
                             coco_bool do_mouse_buttons_too )
{
  unsigned int xmask, key;
  if (do_mouse_buttons_too)
    for (xmask=GDK_BUTTON1_MASK, key=COCO_LPOINTER;
         xmask<=GDK_BUTTON5_MASK; 
         xmask<<=1, key++)
      coco_set_bit( keystate, key, xmask & xstate );
  for (xmask=GDK_CONTROL_MASK, key=COCO_KEY_CONTROL;
       xmask<=GDK_MOD5_MASK; 
       xmask<<=1, key++)
    coco_set_bit( keystate, key, xmask & xstate );
  coco_set_bit( keystate, COCO_KEY_SHIFT, GDK_SHIFT_MASK & xstate );
}

/* -1 is returned if something goes wrong, (it WILL go wrong) */
static int
coco_gdk_keysym_to_coco_key( guint keysym )
{
  /* translate the punta with extreme prejudice, not trusting XLookupString */
  if (((GDK_space<=keysym) && (keysym<=GDK_at))
      || ((GDK_bracketleft<=keysym) && (keysym<=GDK_asciitilde)))
    return keysym - GDK_space + COCO_KEY_SPACE;
  if ((GDK_BackSpace==keysym)||(GDK_Delete==keysym)||(GDK_KP_Delete==keysym))
    return COCO_KEY_DELETE;
  if ((GDK_Tab==keysym)||(GDK_KP_Tab==keysym)||(GDK_ISO_Left_Tab==keysym))
    return COCO_KEY_TAB;
  if ((GDK_Return==keysym)||(GDK_KP_Enter==keysym)||(GDK_ISO_Enter==keysym))
    return COCO_KEY_RETURN;
  if ((GDK_Escape==keysym)||(GDK_Cancel==keysym))
    return COCO_KEY_ESCAPE;
  if ((GDK_A<=keysym)&&(keysym<=GDK_Z)) /* dammned caps roaches */
    return keysym - GDK_A + COCO_CHAR_TO_KEY( 'a' );
  if ((GDK_Shift_L==keysym)||(GDK_Shift_R==keysym))
    return COCO_KEY_SHIFT;
  if ((GDK_Control_L==keysym)||(GDK_Control_R==keysym))
    return COCO_KEY_CONTROL;
  if ((GDK_Meta_L==keysym)||(GDK_Meta_R==keysym))
    return COCO_KEY_META;
  if ((GDK_Alt_L==keysym)||(GDK_Alt_R==keysym))
    return COCO_KEY_ALT;
  if ((GDK_Super_L==keysym)||(GDK_Super_R==keysym))
    return COCO_KEY_SUPER;
  if ((GDK_Hyper_L==keysym)||(GDK_Hyper_R==keysym))
    return COCO_KEY_HYPER;
  if ((GDK_KP_Space==keysym))
    return COCO_KEY_SPACE;
  if ((GDK_leftarrow==keysym)||(GDK_Left==keysym))
    return COCO_KEY_LEFT_ARROW;
  if ((GDK_rightarrow==keysym)||(GDK_Right==keysym))
    return COCO_KEY_RIGHT_ARROW;
  if ((GDK_uparrow==keysym)||(GDK_Up==keysym))
    return COCO_KEY_UP_ARROW;
  if ((GDK_downarrow==keysym)||(GDK_Down==keysym))
    return COCO_KEY_DOWN_ARROW;
  return -1;
}


static coco_bool coco_mouse_in_window_p_private = COCO_FALSE;

coco_bool 
coco_mouse_in_window_p()
{
  return coco_mouse_in_window_p_private;
}


static gint
coco_input_event_callback( GtkWidget *area, GdkEvent *event )
{
  gint return_value = FALSE;
  GdkEventType type = event->any.type;
  coco_bool wrote_raw_p = COCO_FALSE;
  coco_gnum xraw = 0;
  coco_gnum yraw = 0;
  coco_key_state_ref keystate = coco_input_state.key_state;

  coco_input_state.client_time
    = COCO_U32_ADD_MODULAR( coco_input_state.client_time );

  if (GDK_MOTION_NOTIFY == type)
    {
      wrote_raw_p = COCO_TRUE;
      xraw = event->motion.x;
      yraw = event->motion.y;
      coco_update_bnb_from_gstate( keystate, event->motion.state, COCO_TRUE );
    }
  else if ((GDK_ENTER_NOTIFY == type) || (GDK_LEAVE_NOTIFY == type))
    {
      coco_mouse_in_window_p_private = (GDK_ENTER_NOTIFY == type);
      wrote_raw_p = COCO_TRUE;
      xraw = event->crossing.x;
      yraw = event->crossing.y;
      coco_update_bnb_from_gstate( keystate, event->crossing.state, COCO_TRUE );
    }
  else if ((GDK_BUTTON_PRESS == type) || (GDK_BUTTON_RELEASE == type))
    {
      coco_bool ispress = GDK_BUTTON_PRESS == type;
      coco_key key = COCO_LPOINTER + event->button.button - 1;
      wrote_raw_p = COCO_TRUE;
      xraw = event->button.x;
      yraw = event->button.y;
      coco_update_bnb_from_gstate( keystate, event->button.state, COCO_TRUE );
      coco_set_bit( keystate, key, ispress );
      if (ispress)
        { 
          coco_key_press( key, NULL, keystate );
          gtk_widget_grab_focus( area );
        }
      else coco_key_release( key, keystate );
    }
  else if ((GDK_KEY_PRESS == type) || (GDK_KEY_RELEASE == type))
    {
      coco_bool ispress = GDK_KEY_PRESS == type;
      int key;
      /* struct _GdkEventKey
         {
         GdkEventType type;
         GdkWindow *window;
         gint8 send_event;
         guint32 time;
         guint state;
         guint keyval;
         gint length;
         gchar *string;
         }; */
      /*
      coco_cookie( "%s  keyval = %d, length = %d, string = %s, send = %d, time = %d\n",
                   ispress ? "press" : "release",
                   event->key.keyval, event->key.length, event->key.string,
                   event->key.send_event, event->key.time ); 
      */
      coco_update_bnb_from_gstate( keystate, event->key.state, COCO_TRUE );
      key = coco_gdk_keysym_to_coco_key( event->key.keyval );
      if (key != -1)
        {
          coco_bool wasset = coco_get_bit( keystate, (coco_key) key );
          coco_set_bit( keystate, (coco_key) key, ispress );
          if (ispress) 
            { 
              if (!wasset) /* don't send multiple for typeomatic */
                coco_key_press( key, 
                                ((event->key.string && event->key.string[0])
                                 ? event->key.string : NULL),
                                keystate );
            }
          else coco_key_release( key, keystate );
        }
      gtk_signal_emit_stop_by_name( GTK_OBJECT( area ),
                                    ispress 
                                    ? "key_press_event"
                                    : "key_release_event" );
      /* this required or else tab "spaces" the keyboard focus */
      return_value = TRUE;
    }

  if (wrote_raw_p)
    {
      coco_gnum delta = COCO_OFFICIAL_WINDOW_H - yraw;
      coco_input_state.pointer_y 
        = COCO_CLAMP( 0, delta, COCO_OFFICIAL_WINDOW_H );
      coco_input_state.pointer_x = xraw - (COCO_OFFICIAL_WINDOW_W>>1);
    }
  return return_value; /* not sure what to return */
}


void 
coco_kill_window( void )
{
  if (coco_root_window)
    gtk_widget_destroy( coco_root_window );
}

void
coco_handle_ui_events( void )
{
  if (NULL == coco_root_window)
    {
      /* this must go after the root_window is NULL, or else it will call
         destroy on itself again.  */
      coco_client_shutdown( COCO_TRUE );
    }
  while (g_main_pending())
    g_main_iteration( COCO_FALSE );
  return;
}

void 
coco_swap_buffer( void )
{
  if (coco_gl_window) /* sometimes we're in the process of shutting down. */
    {
      if (coco_double_buffer)
        gtk_gl_area_swapbuffers( GTK_GL_AREA( coco_gl_window ) );
      else glFlush();
    }
  return;
}

static coco_bool coco_pointer_grabbed_flag = COCO_FALSE;

coco_bool coco_get_pointer_grabbed_flag()
{
  return coco_pointer_grabbed_flag;
}

coco_bool 
coco_grab_pointer( void )
{
  GdkWindow *gdkwindow;
  if (!coco_gl_window) return COCO_FALSE;
  gdkwindow = coco_gl_window->window;
  coco_pointer_grabbed_flag |= GrabSuccess == 
    gdk_pointer_grab( gdkwindow, True,
                      GDK_BUTTON_PRESS_MASK
                      | GDK_BUTTON_RELEASE_MASK
                      | GDK_POINTER_MOTION_MASK
                      | GDK_ENTER_NOTIFY_MASK
                      | GDK_LEAVE_NOTIFY_MASK,
                      gdkwindow, NULL, GDK_CURRENT_TIME);
  return coco_pointer_grabbed_flag;
}

void 
coco_ungrab_pointer( void )
{
  gdk_pointer_ungrab( GDK_CURRENT_TIME );
  coco_pointer_grabbed_flag = COCO_FALSE;
  return;
}


/* the implementation_datum is the gdk font. */
void 
coco_load_font( struct coco_font *font )
{
  GdkFont *gdkfont;
  gdkfont = gdk_font_load( font->name );
  if (NULL == gdkfont)
    {
      coco_warn_printf( "Couldn't load font \"%s\", using \"9x15\" instead.",
                        font->name );
      gdkfont = gdk_font_load( "9x15" );
      if (NULL == gdkfont)
        coco_fatal_printf( COCO_ERROR_NO_FONT, 
                           "Couldn't even find \"9x15\"." );
    }
  gdk_font_ref( gdkfont );
  font->implementation_datum = gdkfont;
  font->list_base = glGenLists( 256 );
  if (0 == font->list_base)
    coco_fatal_printf( COCO_ERROR_DISPLAY_LISTS,
                  "Problems allocating OpenGL display lists for fonts." );

  gdk_gl_use_gdk_font( gdkfont, 0, 256, font->list_base );

  return;
}

int 
coco_font_height( struct coco_font *font )
{
  GdkFont *gdkfont = font->implementation_datum;
  return gdkfont->ascent + gdkfont->descent;
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
  GdkFont *gdkfont = font->implementation_datum;
  int i;
  int cursor = 0;
  int line_height = gdkfont->ascent + gdkfont->descent;
  int hacc = line_height;
  int wmax = 0;
  int chars_counted = 0;
  char ch;
  if (w||h)
    while (1)
      {
        int line_width;
        i = cursor;
        while (1) /* advance i looking for line terminators */
          { 
            ch=string[i];
            if ((ch=='\0') || (ch=='\n')
                || ((nchars_or_minus_one!=-1)
                    && (chars_counted >= nchars_or_minus_one))) 
              break;
            i++;
            chars_counted++;
          };
        line_width = gdk_text_width( gdkfont, string+cursor, i-cursor );

        /* Be nice if XTextWidth didn't return 0 in XFree... */
        /* ww = XTextWidth( xfs, string+cursor, cursor-i ); */
        if (wmax < line_width) wmax = line_width;
        if ((ch=='\0')
            || ((nchars_or_minus_one!=-1)
                && (chars_counted >= nchars_or_minus_one)))
          break;
        if (string[i+1] != '\0') hacc += line_height;
        cursor = i + 1;
      }
  if (w) *w = wmax;
  if (h) *h = hacc;
  if (ascent) *ascent = gdkfont->ascent;
}




void
coco_on_new_game1_activate( GtkMenuItem *menuitem,
                            gpointer user_data )
{
  if (coco_root_window)
    coco_use_new_game_dialog( coco_root_window );
}


void
coco_on_tutorial1_activate( GtkMenuItem     *menuitem,
                             gpointer         user_data)
{
  if (coco_root_window)
    coco_use_tutorial_dialog( coco_root_window );
}


void
coco_on_join_game1_activate( GtkMenuItem     *menuitem,
                             gpointer         user_data)
{
  if (coco_root_window)
    coco_use_join_dialog( coco_root_window );
}

void
coco_on_replay_movie1_activate( GtkMenuItem     *menuitem,
                                gpointer         user_data)
{
  coco_replay_intro_movie();
}

void
coco_on_disconnect1_activate( GtkMenuItem     *menuitem,
                              gpointer         user_data)
{
  coco_client_disconnect();
}

void
coco_on_exit1_activate( GtkMenuItem     *menuitem,
                        gpointer         user_data)
{
  coco_cookie( "exit1_activate here\n" );
  coco_kill_window();
  coco_cookie( "exit1_activate done\n" );
}

/* coco_on_game_activate: moved to after menu def   */

void
coco_on_key_bindings1_activate( GtkMenuItem     *menuitem,
                               gpointer         user_data)
{
  coco_use_string_viewer_dialog( coco_keybindings, 
                                 "Key & Mouse Bindings", COCO_TRUE );
}

void
coco_on_manual1_activate( GtkMenuItem     *menuitem,
                               gpointer         user_data)
{
  coco_use_string_viewer_dialog( coco_mini_manual, 
                                 COCO_PRODUCT_NAME " Manual", COCO_TRUE );
}

void
coco_on_copying_conditions1_activate( GtkMenuItem     *menuitem,
                                      gpointer         user_data)
{
  coco_use_string_viewer_dialog( coco_gnu_public_license, 
                                 "Copying Conditions", COCO_FALSE );
}


void
coco_on__non_warranty1_activate( GtkMenuItem     *menuitem,
                                 gpointer         user_data )
{
  coco_use_string_viewer_dialog( COCO_GNU_NON_WARRANTY, 
                                 "(Non)Warranty", COCO_FALSE );
}


void
coco_on_about1_activate( GtkMenuItem     *menuitem,
                         gpointer         user_data)
{
  GtkWidget *about = gnome_about_new( coco_product_name,
                                      coco_version_string,
                                      coco_product_copyright,
                                      coco_product_authors,
                                      coco_product_description_windbag,
                                      NULL );
  if (coco_root_window)
    gnome_dialog_set_parent( GNOME_DIALOG( about ), 
                             GTK_WINDOW( coco_root_window ));
  gtk_widget_show( about );
}


#define COCO_TUTORIAL_WIDGET (coco_game1_menu_uiinfo[0].widget)
#define COCO_NEW_GAME_WIDGET (coco_game1_menu_uiinfo[1].widget)
#define COCO_JOIN_GAME_WIDGET (coco_game1_menu_uiinfo[2].widget)
#define COCO_PLAY_INTRO_WIDGET (coco_game1_menu_uiinfo[3].widget)
#define COCO_DISCONNECT_WIDGET (coco_game1_menu_uiinfo[4].widget)

/* glade output */
static GnomeUIInfo coco_game1_menu_uiinfo[] =
{
  {  /* see COCO_TUTORIAL_WIDGET */
    GNOME_APP_UI_ITEM, N_("_Tutorial"),
    NULL,
    (void *) coco_on_tutorial1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  /* see COCO_NEW_GAME_WIDGET */
  GNOMEUIINFO_MENU_NEW_GAME_ITEM (coco_on_new_game1_activate, NULL),
  {  /* see COCO_JOIN_GAME_WIDGET */
    GNOME_APP_UI_ITEM, N_("_Join game"),
    NULL,
    (void *) coco_on_join_game1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  { /* see COCO_PLAY_INTRO_WIDGET */
    GNOME_APP_UI_ITEM, N_("_Play Intro"),
    NULL,
    (void *) coco_on_replay_movie1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  { /* see COCO_DISCONNECT_WIDGET */
    GNOME_APP_UI_ITEM, N_("_Disconnect"),
    NULL,
    (void *) coco_on_disconnect1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  GNOMEUIINFO_MENU_EXIT_ITEM (coco_on_exit1_activate, NULL),
  GNOMEUIINFO_END
};

/* glade output */
static GnomeUIInfo coco_help1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Key Bindings"),
    NULL,
    (void *) coco_on_key_bindings1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Manual"),
    NULL,
    (void *) coco_on_manual1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("Copying Conditions"),
    NULL,
    (void *) coco_on_copying_conditions1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("(Non)Warranty"),
    NULL,
    (void *) coco_on__non_warranty1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, 0, NULL
  },
  GNOMEUIINFO_MENU_ABOUT_ITEM (coco_on_about1_activate, NULL),
  GNOMEUIINFO_END
};

#define COCO_GAME_MENU_WIDGET (coco_menubar1_uiinfo[0].widget)

/* glade output */
static GnomeUIInfo coco_menubar1_uiinfo[] =
{
  GNOMEUIINFO_MENU_GAME_TREE (coco_game1_menu_uiinfo), /* see also COCO_GAME_MENU_WIDGET */
  GNOMEUIINFO_MENU_HELP_TREE (coco_help1_menu_uiinfo),
  GNOMEUIINFO_END
};

void
coco_on_game_activate( GtkMenuItem     *menuitem,
                       gpointer         user_data)
{
  coco_bool play_intro_enable = COCO_FALSE;
  coco_bool disconnect_enable = COCO_FALSE;
  coco_clientstate state = coco_get_client_state();
  switch (state) {
  case COCO_CLIENT_DEMO: 
    play_intro_enable = COCO_TRUE;
    break;
  case COCO_CLIENT_PLAY:
  case COCO_CLIENT_CHATROOM_DIALOG:
    disconnect_enable = COCO_TRUE;
    break;
  default: break;
  }
  if (play_intro_enable 
      == !(GTK_WIDGET_IS_SENSITIVE( GTK_WIDGET( COCO_PLAY_INTRO_WIDGET ) )))
    gtk_widget_set_sensitive( GTK_WIDGET( COCO_PLAY_INTRO_WIDGET ), 
                              play_intro_enable );
  if (disconnect_enable 
      == !(GTK_WIDGET_IS_SENSITIVE( GTK_WIDGET( COCO_DISCONNECT_WIDGET ) )))
    {
      gtk_widget_set_sensitive( GTK_WIDGET( COCO_DISCONNECT_WIDGET ), 
                                disconnect_enable );
      gtk_widget_set_sensitive( GTK_WIDGET( COCO_NEW_GAME_WIDGET ), 
                                !disconnect_enable );
      gtk_widget_set_sensitive( GTK_WIDGET( COCO_JOIN_GAME_WIDGET ), 
                                !disconnect_enable );
    }
}


void
coco_gdk_string_bounding_box( GdkFont *font, const char *string, 
                              int *width_return, int *height_return )
{
  const char *cursor = string;
  const char *line = string;
  int line_count = 1;
  int width_max = 0;
  while (1)
    {
      char ch = *cursor;
      if (ch == '\0')
        {
          int width_current = gdk_text_width( font, line, cursor - line );
          *width_return = COCO_MAX( width_max, width_current );
          *height_return = (font->ascent + font->descent) * line_count;
          return;
        }
      else if (ch == '\n')
        {
          int width_current = gdk_text_width( font, line, cursor - line );
          line_count++;
          /* coco_cookie( "line %d width %d\n", line_count, width_current ); */
          width_max = COCO_MAX( width_max, width_current );
          line = cursor + 1;
        }
      cursor++;
    }
}

/* Unfortunately I have no choice but to access internals of gnome less. */
GdkFont *coco_gnome_less_get_font( GnomeLess *less )
{
  GdkFont *slot = less->font;
  if (slot)
    {
      coco_cookie( "Heh, got font slot\n" );
      return slot;
    }
  coco_cookie( "Falling back to widget font\n" );
  return GTK_WIDGET( less )->style->font;
}


void
coco_use_string_viewer_dialog( const char *string, const char *title, 
                               coco_bool fixed_font_p )
{
  GtkWidget *dialog;
  GtkWidget *less;

  dialog = gnome_dialog_new( title, GNOME_STOCK_BUTTON_CLOSE, NULL );
  less = gnome_less_new();

  /* enable user resizing */
  gtk_window_set_policy( GTK_WINDOW( dialog ), FALSE, TRUE, FALSE );

  gtk_box_pack_start( GTK_BOX( GNOME_DIALOG( dialog )->vbox ), 
                      less, TRUE, TRUE, 0 );
  gnome_dialog_button_connect_object( GNOME_DIALOG( dialog ),
                                      0, gtk_widget_destroy,
                                      GTK_OBJECT( dialog ) );
  if (coco_root_window)
    gnome_dialog_set_parent( GNOME_DIALOG( dialog ), 
                             GTK_WINDOW( coco_root_window ));

  gtk_widget_realize( less ); /* try to force it to have a font */

  if (fixed_font_p)
    gnome_less_set_fixed_font( GNOME_LESS( less ), TRUE );

  gnome_less_show_string( GNOME_LESS( less ), string );

  {
    int w, h;
    GdkFont *font = coco_gnome_less_get_font( GNOME_LESS( less ) );
    coco_gdk_string_bounding_box( font, string, &w, &h );
    if ((w<800)&&(h<600))
      { /* small enough to constrain it */
        /* 100, 10 = fudge for less overhead */
        gtk_widget_set_usize( GTK_WIDGET( less ), w + 100, h + 10 );
      }
    else
      { /* its big, better just hint at what we want */
        GdkFont *dialog_font = GTK_WIDGET( dialog )->style->font;
        int ww, hh;
        /* fudges for dialog overhead: */
        ww = w + 100;
        hh = h + 150 + dialog_font->ascent + dialog_font->descent;

        gtk_window_set_default_size( GTK_WINDOW( dialog ), 
                                     COCO_MIN( 800, ww ), 
                                     COCO_MIN( 600, hh ) );
      }
  }

  gtk_widget_show_all( dialog );
}

#define COCO_PLAYER_TABLE_COLUMNS 2

void
coco_player_table_update( void *clist_raw, union coco_message *message )
{
  int i;
  GtkWidget *clistw = (GtkWidget *)clist_raw;
  GtkCList *clist = GTK_CLIST( clistw );

  gtk_clist_freeze( clist );
  gtk_clist_clear( clist );
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    {
      if (coco_players[i].in_use)
        {
          char *text[COCO_PLAYER_TABLE_COLUMNS];
          text[0] = coco_players[i].name;
          text[1] = (COCO_GOOD_GUYS == coco_players[i].nationality)
            ? COCO_GOOD_GUYS_PRETTY_NAME : COCO_BAD_GUYS_PRETTY_NAME;
          gtk_clist_append( clist, text );
        }
      else
        { /* pad with dummy rows to keep the widget big enough. */
          char *text[COCO_PLAYER_TABLE_COLUMNS] = {"", ""};
          gtk_clist_append( clist, text );
        };
      gtk_clist_set_selectable( clist, i, FALSE );
    }
  gtk_clist_thaw( clist );
}

void
coco_player_table_clear( GtkWidget *clistw )
{
  int i;
  GtkCList *clist = GTK_CLIST( clistw );

  gtk_clist_freeze( clist );
  gtk_clist_clear( clist );
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    { /* pad with dummy rows to keep the widget big enough. */
      char *text[COCO_PLAYER_TABLE_COLUMNS] = {"", ""};
      gtk_clist_append( clist, text );
      gtk_clist_set_selectable( clist, i, FALSE );
    }
  gtk_clist_thaw( clist );
}

void
coco_widget_delete_game_message_notification( gpointer data )
{
  coco_delete_message_notification( COCO_MESSAGE_GAME,
                                    (coco_notification_id)
                                    (((int) data) - 1) );
}

void
coco_widget_delete_talk_message_notification( gpointer data )
{
  coco_delete_message_notification( COCO_MESSAGE_TALK,
                                    (coco_notification_id)
                                    (((int) data) - 1) );
}


void
coco_connection_label_update( void *label_raw, union coco_message *message )
{
  char label_buff[200];
  snprintf( label_buff, 200, "%s:%d  game %s",
            coco_server, coco_port, coco_game_status_string );
  gtk_label_set_text( GTK_LABEL( label_raw ), label_buff );
}

void
coco_team_radio_update( void *radio_raw, union coco_message *message )
{
  GtkObject *radio_obj = GTK_OBJECT( radio_raw );
  coco_nationality nationality = (coco_nationality) (int)
    gtk_object_get_data( radio_obj, "coco nationality" );
  assert( (nationality == COCO_GOOD_GUYS) || (nationality == COCO_BAD_GUYS) );
  /* remove ourselves from the notification list.  */
  gtk_object_remove_data( radio_obj, "coco game_msg_id" );
  if (nationality == coco_get_player_nationality( coco_cl_nationality ))
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( radio_obj ),
                                  TRUE );
  coco_cookie( "Toggle: My nat is %d, and players is %d\n",
               nationality, coco_get_player_nationality( coco_cl_nationality ));
}

void
coco_choose_team( GtkButton *button, gpointer *nationality_raw )
{
  coco_nationality nationality = (coco_nationality) (int) nationality_raw;
  if (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( button ) ))
    {
      coco_send_join_message( nationality );
    }
}



GtkWidget *
coco_player_table_new( void )
{
  coco_notification_id game_msg_id;
  GtkWidget *clistw,  *team_vbox, *team_hbox,
    *badguy_radio, *goodguy_radio;
  GtkCList *clist;
  static gchar *titles[] = { "Name     ", "Team" };

  clistw = gtk_clist_new_with_titles( COCO_PLAYER_TABLE_COLUMNS, titles );
  clist = GTK_CLIST( clistw );

  /* Make nothing selectable. */
  gtk_clist_set_selection_mode( clist, GTK_SELECTION_EXTENDED );
  
  game_msg_id = 
    coco_add_message_notification( COCO_MESSAGE_GAME,
                                   coco_player_table_update, 
                                   (void *) clistw );

  if (coco_have_game_message)
    coco_player_table_update( (void *) clistw, NULL );
  else coco_player_table_clear( clistw );

  gtk_object_set_data_full( GTK_OBJECT( clist ), 
                            "coco game_msg_id", (void *) (game_msg_id + 1),
                            coco_widget_delete_game_message_notification );

  gtk_clist_column_titles_passive( clist );

  team_vbox = gtk_vbox_new( FALSE, COCO_UI_PAD_VERTICAL );
  team_hbox = gtk_hbox_new( FALSE, COCO_UI_PAD_HORIZONTAL );
  badguy_radio
    = gtk_radio_button_new_with_label_from_widget
    ( NULL, COCO_BAD_GUYS_PRETTY_NAME );
  goodguy_radio
    = gtk_radio_button_new_with_label_from_widget
    ( GTK_RADIO_BUTTON( badguy_radio ), COCO_GOOD_GUYS_PRETTY_NAME );
  {
    int i;
    GtkWidget *radio;
    coco_nationality nat = coco_get_player_nationality( coco_cl_nationality );
    if ((COCO_NEUTRAL == nat))
      for (i=0, radio = goodguy_radio; i<2; i++, radio = badguy_radio)
        { /* the hard way, probably have game message, but sometimes not */
          int id = 
            coco_add_message_notification( COCO_MESSAGE_GAME,
                                           coco_team_radio_update, 
                                           (void *) radio );
          gtk_object_set_data_full
            ( GTK_OBJECT( radio ), "coco game_msg_id", (void *) (id + 1),
              coco_widget_delete_game_message_notification );
          gtk_object_set_data( GTK_OBJECT( radio ), "coco nationality", 
                               (void *) ((int) i+COCO_GOOD_GUYS) );
        coco_cookie( "\n\nTHE HARD WAY gg=%d\n\n", (COCO_GOOD_GUYS == nat) );
        }
    else 
      { /* the easy way. */
        coco_cookie( "\n\nTHE EASY WAY gg=%d\n\n", (COCO_GOOD_GUYS == nat) );
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( (COCO_GOOD_GUYS == nat)
                                                         ? goodguy_radio
                                                         : badguy_radio ),
                                      TRUE );
      }
  }

  gtk_signal_connect( GTK_OBJECT( badguy_radio ), "toggled",
                      GTK_SIGNAL_FUNC( coco_choose_team ), 
                      (gpointer) COCO_BAD_GUYS );
  gtk_signal_connect( GTK_OBJECT( goodguy_radio ), "toggled",
                      GTK_SIGNAL_FUNC( coco_choose_team ), 
                      (gpointer) COCO_GOOD_GUYS );
  gtk_box_pack_start( GTK_BOX( team_hbox ), gtk_label_new( "Your team:" ), 
                      FALSE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( team_hbox ), badguy_radio, FALSE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( team_hbox ), goodguy_radio, FALSE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( team_vbox ), 
                      gtk_label_new( "Teams" ), FALSE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( team_vbox ), clistw, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( team_vbox ), team_hbox, FALSE, TRUE, 0 );

  return team_vbox;
}


GtkWidget *
coco_connection_label_new( void )
{
  coco_notification_id game_msg_id;
  GtkWidget *label
    = gtk_label_new( "Waiting for game information from server" );
  game_msg_id = 
    coco_add_message_notification( COCO_MESSAGE_GAME,
                                   coco_connection_label_update, 
                                   (void *) label );
  if (coco_have_game_message)
    coco_connection_label_update( (void *) label, NULL );
  gtk_object_set_data_full( GTK_OBJECT( label ), 
                            "coco game_msg_id", (void *) (game_msg_id + 1),
                            coco_widget_delete_game_message_notification );
  return label;
}



void
coco_recieve_widget_update( void *text_raw, union coco_message *message )
{
  coco_u32 plr;
  GtkWidget *textw = GTK_WIDGET( text_raw );
  GtkText *text = GTK_TEXT( textw );
  guint point = gtk_text_get_length( text );
  int slen = strlen( message->talk.message );
  GtkAdjustment *vadj = text->vadj;
  coco_bool scroll_at_bottom = vadj->value +vadj->page_size >= vadj->upper;

  gtk_text_freeze( text );
  gtk_text_set_point( text, point );

  /* coco_players[message->talk.from_player].name */

  plr = COCO_U32_NTOH( message->talk.from_player );
  if (plr!=COCO_U32_MAX)
    {
      gtk_text_insert( text, NULL, NULL, NULL, 
                       coco_players[plr].name, -1 );
      gtk_text_insert( text, NULL, NULL, NULL, ": ", -1 );
    }
  gtk_text_insert( text, NULL, NULL, NULL, message->talk.message,
                   COCO_MIN( slen, COCO_CHAT_MAX ) );
  gtk_text_insert( text, NULL, NULL, NULL, "\n", 1 );

  gtk_text_set_point( text, gtk_text_get_length( text ) );

  gtk_text_thaw( text );
  /* sadly this must go after the thaw, or else it doesn't get the new
     values for vadj->upper that come from adding the new text. */
  if (scroll_at_bottom)
    gtk_adjustment_set_value( vadj, vadj->upper );
}


GtkWidget *
coco_recieve_widget_new( void )
{
  GtkWidget *hbox, *text, *scroller;
  text = gtk_text_new( NULL, NULL );
  scroller = gtk_vscrollbar_new( GTK_TEXT( text )->vadj );
  hbox = gtk_hbox_new( FALSE, 0 );
  gtk_text_set_editable( GTK_TEXT( text ), FALSE );
  gtk_box_pack_start( GTK_BOX( hbox ), text, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), scroller, FALSE, FALSE, 0 );
  gtk_widget_show_all( hbox );

  {
    coco_notification_id msg_id;
    msg_id = 
      coco_add_message_notification( COCO_MESSAGE_TALK,
                                     coco_recieve_widget_update, 
                                     (void *) text );
    gtk_object_set_data_full( GTK_OBJECT( text ), 
                              "coco talk_msg_id", (void *) (msg_id + 1),
                              coco_widget_delete_talk_message_notification );
  }

  
  return hbox;
}


static gint
coco_send_widget_kbd_maybe_send( GtkWidget *entry, GdkEvent *event )
{
  int key = coco_gdk_keysym_to_coco_key( event->key.keyval );
  gchar *text = gtk_entry_get_text( GTK_ENTRY( entry ) );
  if ((COCO_KEY_RETURN == key) && !((NULL==text) || (text[0] == '\0')))
    {
      /* coco_chat copies internally, and gtk_entry_get_text returns an
         internal buffer.  Radness. */
      coco_chat( text );
      gtk_entry_set_text( GTK_ENTRY( entry ), "" );
    }
  return 0; /* not sure what to return */
}

static void
coco_send_widget_button_send( GtkWidget *button, gpointer entry_raw )
{
  GtkWidget *entry = entry_raw;
  gchar *text = gtk_entry_get_text( GTK_ENTRY( entry ) );
  if (!((NULL==text) || (text[0] == '\0')))
    {
      coco_chat( text );
      gtk_entry_set_text( GTK_ENTRY( entry ), "" );
    }
}

GtkWidget *
coco_send_widget_new( void )
{
  GtkWidget *hbox, *button, *entry;
  hbox = gtk_hbox_new( FALSE, COCO_UI_PAD_HORIZONTAL );
  button = gtk_button_new_with_label( "Send" );
  entry = gtk_entry_new_with_max_length( COCO_CHAT_MAX-1 );
  gtk_box_pack_start( GTK_BOX( hbox ), entry, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), button, FALSE, FALSE, 0 );
  gtk_widget_show_all( button );
  gtk_widget_show_all( entry );
  gtk_signal_connect( GTK_OBJECT( entry ), "key_press_event",
                      GTK_SIGNAL_FUNC( coco_send_widget_kbd_maybe_send ), 
                      NULL );
  gtk_signal_connect (GTK_OBJECT( button ), "clicked",
                      GTK_SIGNAL_FUNC( coco_send_widget_button_send ), 
                      entry );
  return hbox;
}


GtkWidget *
coco_chat_widget_new( char *label_or_null )
{
  GtkWidget *snd, *rcv, *vbox, *lab;
  lab = label_or_null ? gtk_label_new( label_or_null ) : NULL;
  snd = coco_send_widget_new();
  rcv = coco_recieve_widget_new();
  vbox = gtk_vbox_new( FALSE, COCO_UI_PAD_VERTICAL );
  gtk_widget_show( GTK_WIDGET( snd ) );
  gtk_widget_show( GTK_WIDGET( rcv ) );
  if (lab)
    gtk_box_pack_start( GTK_BOX( vbox ), lab, FALSE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( vbox ), rcv, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( vbox ), snd, FALSE, TRUE, 0 );
  return vbox;
}


static void
coco_chatroom_ok_clicked( GtkWidget *button, gpointer data )
{
  gtk_widget_destroy( GTK_WIDGET( data ) );
  coco_send_play_message_maybe();
}

static void
coco_chatroom_cancel_clicked( GtkWidget *button, gpointer dialog )
{
  coco_client_disconnect();
  gtk_widget_destroy( GTK_WIDGET( dialog ) );
  coco_refresh_window_eventually();
}

static gint
coco_remove_play_notification_callback( GtkWidget *widget, 
                                         gpointer data )
{
  coco_notification_id id = (coco_notification_id) data;
  coco_cookie( "deleting play notification %d\n", id );
  coco_delete_message_notification( COCO_MESSAGE_PLAY, id );
  return 0;
}

static void
coco_gtk_widget_destroy_adapter( void *data, union coco_message *message )
{
  coco_cookie( "destroy_adapter here\n" );
  gtk_widget_destroy( GTK_WIDGET( data ) );
}


GtkWidget *
coco_chatroom_dialog_new( GtkWindow *window_parent )
{
  GtkWidget *dialog, *outter_vbox, *paned;
  dialog = gnome_dialog_new( COCO_PRODUCT_NAME " Chat Room",
                             GNOME_STOCK_BUTTON_OK,
                             GNOME_STOCK_BUTTON_CANCEL,
                             NULL );
  outter_vbox = GNOME_DIALOG( dialog )->vbox;
  paned = gtk_vpaned_new();
  gtk_paned_pack1( GTK_PANED( paned ), coco_player_table_new(), TRUE, TRUE );
  gtk_paned_pack2( GTK_PANED( paned ), coco_chat_widget_new( "Chat" ),
                   TRUE, TRUE );
  gtk_box_pack_start( GTK_BOX( outter_vbox ), 
                      coco_connection_label_new(), FALSE, FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( outter_vbox ), 
                      gtk_hseparator_new(), FALSE, FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( outter_vbox ), 
                      paned,
                      TRUE, TRUE, 0 );
  { /* set chatroom dialog minimum size to be pretty big -dm 05-07-2001 */
    gint w, h;
    gdk_window_get_size( GTK_WIDGET( window_parent )->window, &w, &h);
    gtk_widget_set_usize( dialog, w, h );
  }
  /* enable user resizing */
  gtk_window_set_policy( GTK_WINDOW( dialog ), FALSE, TRUE, FALSE );

  gnome_dialog_set_parent( GNOME_DIALOG( dialog ), window_parent );

  gnome_dialog_button_connect
    ( GNOME_DIALOG( dialog ), 0, 
      GTK_SIGNAL_FUNC( coco_chatroom_ok_clicked ), dialog );
  gnome_dialog_button_connect
    ( GNOME_DIALOG( dialog ), 1, 
      GTK_SIGNAL_FUNC( coco_chatroom_cancel_clicked ), 
      dialog );

  { /* make the window close itself when play message recieved */
    coco_notification_id id;
    id = coco_add_message_notification( COCO_MESSAGE_PLAY,
                                        coco_gtk_widget_destroy_adapter,
                                        dialog );
    coco_cookie( "Added play notification %d\n", id );
    gtk_signal_connect( GTK_OBJECT( dialog ), "destroy",
                        GTK_SIGNAL_FUNC
                        ( coco_remove_play_notification_callback ),
                        (gpointer) id );
  }

  gtk_widget_show_all( outter_vbox );

  return dialog;
}


/* lightly edited from glade output */
GtkWidget *
coco_menubar_new( GtkWidget *window1 )
{
  GtkWidget *menubar1;

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_ref (menubar1);
  /* hm, glade put this here.
  gtk_object_set_data_full (GTK_OBJECT (window1), "menubar1", menubar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  */
  gtk_widget_show (menubar1);
  gnome_app_fill_menu (GTK_MENU_SHELL (menubar1), coco_menubar1_uiinfo,
                       NULL, FALSE, 0);

  gtk_widget_ref (coco_menubar1_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "game1",
                            coco_menubar1_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_game1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "new_game1",
                            coco_game1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_game1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "join_game1",
                            coco_game1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_game1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "play_movie1",
                            coco_game1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_game1_menu_uiinfo[3].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "disconnect1",
                            coco_game1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_game1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "exit1",
                            coco_game1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_menubar1_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "help1",
                            coco_menubar1_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_help1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "key_bindings1",
                            coco_help1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_help1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "manual1",
                            coco_help1_menu_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_help1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "copying_conditions1",
                            coco_help1_menu_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_help1_menu_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "_non_warranty1",
                            coco_help1_menu_uiinfo[3].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (coco_help1_menu_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (window1), "about1",
                            coco_help1_menu_uiinfo[4].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_signal_connect( GTK_OBJECT( COCO_GAME_MENU_WIDGET ), "activate",
                      GTK_SIGNAL_FUNC( coco_on_game_activate ), NULL );


  return menubar1;
}



#ifdef COCO_COMPOUND_PORT_EDITOR
static void
coco_port_button_default_clicked(GtkWidget *button, gpointer data)
{
  gtk_adjustment_set_value( GTK_ADJUSTMENT( data ), coco_port );
}
#endif

/* Records the last user-selected port or COCO_U32_MAX for 
   "initialize-me-from-the-command-line-argument".   */
static coco_u32 coco_port_ui_initial_value = COCO_U32_MAX;

/* create a widget for editing the port. */
GtkWidget *
coco_port_editor_new( void )
{
  GtkObject *adjustment;
  GtkWidget *spin;
#ifdef COCO_COMPOUND_PORT_EDITOR
  GtkWidget *hbox, *button;
#endif
  if (COCO_U32_MAX == coco_port_ui_initial_value)
    coco_port_ui_initial_value = coco_port;
  adjustment = gtk_adjustment_new( coco_port, 0, 65535, 1, 1024, 0 );
  spin = gtk_spin_button_new( GTK_ADJUSTMENT( adjustment ), 1.0, 0 );
  gtk_spin_button_set_numeric( GTK_SPIN_BUTTON( spin ), TRUE );
  gtk_spin_button_set_wrap( GTK_SPIN_BUTTON( spin ), FALSE );
  gtk_spin_button_set_snap_to_ticks( GTK_SPIN_BUTTON( spin ), TRUE );
  gtk_spin_button_set_update_policy( GTK_SPIN_BUTTON( spin ),
                                     GTK_UPDATE_IF_VALID );
#ifdef COCO_COMPOUND_PORT_EDITOR
  button = gtk_button_new_with_label( "Default" );
  gtk_signal_connect (GTK_OBJECT( button ), "clicked",
                      GTK_SIGNAL_FUNC( coco_port_button_default_clicked ), 
                      adjustment );
  hbox = gtk_hbox_new( FALSE, COCO_PORT_EDITOR_INTERNAL_PAD );
  gtk_box_pack_start( GTK_BOX( hbox ), spin, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), button, FALSE, TRUE, 0 );
  gtk_object_set_data( GTK_OBJECT( hbox ), "coco_port_spin", spin );
  return hbox;
#else
  return spin;
#endif
}

int
coco_port_editor_get_value( GtkWidget *port_editor )
{
#ifdef COCO_COMPOUND_PORT_EDITOR
  return
    gtk_spin_button_get_value_as_int
    ( gtk_object_get_data( GTK_OBJECT( port_editor ), "coco_port_spin" ) );
#else
  return gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( port_editor ) );
#endif
}



/* Use a gui join dialog to get hostname and port information about a
   server.  On success, return true, set the hostname in a freshly
   coco_malloc'ed *hostname_return, and set the port in *port_return.  
   On failure, return NULL. */
coco_bool
coco_use_join_dialog( GtkWidget *parent_window )
{
  gint button_index;
  GtkWidget *dialog, *vbox, *table, *host_input, *port_input;
  GtkWidget *host_label, *port_label;

  dialog = gnome_dialog_new( "Join a Server",
                             GNOME_STOCK_BUTTON_OK,
                             GNOME_STOCK_BUTTON_CANCEL,
                             NULL );
  vbox = GNOME_DIALOG( dialog )->vbox;
  table = gtk_table_new( 2, 2, FALSE );
  host_label = gtk_label_new( "Host:" );
  port_label = gtk_label_new( "Port:" );
  host_input = gnome_entry_new( "coco remote hosts" );
  port_input = coco_port_editor_new();

  gtk_label_set_justify( GTK_LABEL( host_label ), GTK_JUSTIFY_LEFT );
  gtk_label_set_justify( GTK_LABEL( port_label ), GTK_JUSTIFY_LEFT );

  if (coco_server_specified_p)
    {
      GtkWidget *gtk_entry;
      gtk_entry = gnome_entry_gtk_entry( GNOME_ENTRY( host_input ) );
      gtk_entry_set_text( GTK_ENTRY( gtk_entry ), coco_server );
    }

  gnome_dialog_set_parent(GNOME_DIALOG( dialog ), GTK_WINDOW( parent_window ));

  gtk_table_attach( GTK_TABLE( table ), host_label,
                    0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 
                    COCO_JOIN_DIALOG_X_PAD, COCO_JOIN_DIALOG_Y_PAD );
  gtk_table_attach( GTK_TABLE( table ), port_label,
                    0, 1, 1, 2, GTK_EXPAND, GTK_EXPAND, 
                    COCO_JOIN_DIALOG_X_PAD, COCO_JOIN_DIALOG_Y_PAD );
  gtk_table_attach( GTK_TABLE( table ), host_input,
                    1, 2, 0, 1, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 
                    COCO_JOIN_DIALOG_X_PAD, COCO_JOIN_DIALOG_Y_PAD );
  gtk_table_attach( GTK_TABLE( table ), port_input,
                    1, 2, 1, 2, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 
                    COCO_JOIN_DIALOG_X_PAD, COCO_JOIN_DIALOG_Y_PAD );

  gtk_box_pack_start( GTK_BOX( vbox ), table, TRUE, TRUE, 0 );

  gtk_widget_show_all( table );

 RETRY:
  button_index = gnome_dialog_run( GNOME_DIALOG( dialog ) );
  if (0==button_index)
    {
      GtkWidget *host_input_gtk 
        = gnome_entry_gtk_entry( GNOME_ENTRY( host_input ) );
      char *text = gtk_entry_get_text( GTK_ENTRY( host_input_gtk ) );
      char *hostname;

      if (coco_server && coco_server_malloced_p)
        coco_free( coco_server );

      if ((NULL==text) || (text[0] == '\0'))
        {
          gnome_dialog_run_and_close
            (GNOME_DIALOG
             ( gnome_error_dialog_parented
               ( "You must specify the hostname\n"
                 "and port of a computer where\n"
                 "another player has created a\n"
                 "game.  For example:\n"
                 "copter-commander.net\n"
                 "7500", 
                 GTK_WINDOW( dialog ) )));
          goto RETRY;
        }


      /* otherwise static storage: */
      hostname = coco_malloc( strlen( text ) + 1 );
      strcpy( hostname, text );


      coco_server = hostname;
      coco_server_malloced_p = COCO_TRUE;

      coco_port = coco_port_editor_get_value( port_input );

      if (coco_join_game_interactively( coco_server, coco_port, 1, 
                                        GTK_WINDOW( dialog ),
                                        COCO_FALSE ))
        {
          gnome_dialog_close( GNOME_DIALOG( dialog ) );
          gtk_widget_show( coco_chatroom_dialog_new( GTK_WINDOW( coco_root_window ) ) );
          return COCO_TRUE;
        }
      else goto RETRY;
    }
  else /* its 1 (CANCEL) or -1 (KILL), or something suspicious */
    {
      if (GNOME_IS_DIALOG( dialog )) /* maybe its already dead */
        gnome_dialog_close( GNOME_DIALOG( dialog ) );
      return COCO_FALSE;
    }
}

/* make a lisp-esque string human-pretty (captalize words, replace _ and -
   with space) */
static void
coco_beautify_string( char *string )
{
  coco_bool cap = COCO_TRUE;
  char ch;
  if (string)
    while (1)
      {
        coco_bool newcap = COCO_FALSE;
        ch = *string;
        if (ch == '\0') return;
        else if ((ch == '-') || (ch == '_'))
          {
            *string = ' ';
            newcap = COCO_TRUE;
          }
        else if (ch == ' ')
          newcap = COCO_TRUE;
        else if (islower( ch ) && cap)
          *string = ch + 'A' - 'a';
        cap = newcap;
        string++;
      }

}


struct coco_built_in_level_menu_closure {
  GtkWidget *menu;
  int selection_size; /* including null char */
  char *selection;
  GtkWidget *item_default;
};


void
coco_on_built_in_level_menuitem_activate( GtkMenuItem *menuitem,
                                          gpointer raw_closure )
{
  struct coco_built_in_level_menu_closure *closure = raw_closure;
  char *label = gtk_object_get_data( GTK_OBJECT( menuitem ),
                                     "coco level filename" );
  int label_size;
  assert( label );
  label_size = strlen( label ) + 1;
  if ((label_size > closure->selection_size))
    {
      closure->selection_size = label_size + 50;
      closure->selection
        = coco_realloc( closure->selection, closure->selection_size );
    }
  strcpy( closure->selection, label );
}


static coco_bool 
coco_add_menuitem_for_ccl_strings( char *filename, void *raw_closure )
{
  struct coco_built_in_level_menu_closure *closure = raw_closure;
  size_t len = strlen( filename );
  if (coco_level_filename_p( filename, COCO_FALSE ))
    {
      int i;
      char ch;
      char *after_slash;
      after_slash = filename;
      for (i=0; (ch = filename[i]) != '\0'; i++)
        if (ch == COCO_DIRECTORY_SEPARATOR_CHAR)
          after_slash = filename + i + 1;
      {
        GtkWidget *menuitem;
        int slen
          = len - (after_slash - filename) /* length, minus the directories */
          - (COCO_LEVEL_SUFFIX_LENGTH+1) /* minus the suffix, incl dot */
          + 1; /* room for null */
        int strsize = slen * sizeof( char );
        char *label = alloca( strsize );

        if (NULL==label) label = coco_malloc( strsize );
        strncpy( label, after_slash, slen-1 );
        label[slen-1] = '\0';
        coco_beautify_string( label );
        menuitem = gtk_menu_item_new_with_label( label );
        gtk_object_set_data_full( GTK_OBJECT( menuitem ), 
                                  "coco level filename", 
                                  coco_string_malloc_copy( filename ),
                                  coco_free );
        gtk_signal_connect( GTK_OBJECT( menuitem ), "activate",
                            GTK_SIGNAL_FUNC
                            ( coco_on_built_in_level_menuitem_activate ), 
                            raw_closure );
        gtk_menu_append( GTK_MENU( closure->menu ), menuitem );
        if (NULL==closure->item_default) 
          closure->item_default = menuitem;
      }
    }
  return COCO_FALSE;
}


static void
coco_level_built_in_callback( GtkButton *button, 
                              gpointer *widget_to_sensitize )
{
  int active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( button ) );
  gtk_widget_set_sensitive( GTK_WIDGET( widget_to_sensitize ), active );
}


void
coco_gtk_set_wait_cursor( GtkWidget *window, coco_bool wait )
{
  static GdkCursor *cursor = NULL;
  if (wait && ! cursor)
    cursor = gdk_cursor_new( GDK_WATCH );
  gdk_window_set_cursor
    ( gtk_widget_get_toplevel( window )->window, wait ? cursor : NULL ) ;
}


GtkWidget *
coco_tutorial_dialog_new( GtkWidget *parent_window )
{
  int i;
  GtkWidget *dialog = gnome_dialog_new( "Select Tutorial",
                                        GNOME_STOCK_BUTTON_OK,
                                        GNOME_STOCK_BUTTON_CANCEL,
                                        NULL );
  GtkBox *vbox = GTK_BOX( GNOME_DIALOG( dialog )->vbox );
  GtkWidget *radio = NULL;
  gnome_dialog_set_parent( GNOME_DIALOG( dialog ), 
                           GTK_WINDOW( parent_window ) );
  for (i=0; i<COCO_TUTORIAL_INFO_SIZE; i++)
    {
      GtkRadioButton *prev = radio ? GTK_RADIO_BUTTON( radio ) : NULL;
      char *ugly = coco_tutorial_infos[i].name;
      char *pretty = strdup( ugly );
      coco_beautify_string( pretty );
      radio = gtk_radio_button_new_with_label_from_widget( prev, pretty );
      gtk_box_pack_start( vbox, radio, TRUE, TRUE, 0 );
    }
  gtk_widget_show_all( GNOME_DIALOG( dialog )->vbox );
  return dialog;
}

struct coco_tutorial_dialog_selected_closure {
  int i;
  int answer;
};

void
coco_tutorial_dialog_selected_helper( GtkWidget *radio, void *data )
{
  struct coco_tutorial_dialog_selected_closure *closure = data;
  if (GTK_IS_TOGGLE_BUTTON( radio )
      && gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( radio ) ))
    closure->answer = closure->i;
  (closure->i)++;
}

int
coco_tutorial_dialog_get_selected( GtkWidget *tutorial_dialog )
{
  GtkContainer *container 
    = GTK_CONTAINER( GNOME_DIALOG( tutorial_dialog )->vbox );
  struct coco_tutorial_dialog_selected_closure closure;
  closure.i = 0;
  closure.answer = -1;
  gtk_container_foreach( container, coco_tutorial_dialog_selected_helper, &closure );
  if (closure.answer > -1)
    return closure.answer;
  else
    {
      coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, 
                         "No tutorial radio button is selected." );
      return -1; /* quiet compiler */
    }
}

void
coco_use_tutorial_dialog( GtkWidget *parent_window )
{
  GtkWidget *dialog = coco_tutorial_dialog_new( parent_window );
  int button_idx;
  button_idx = gnome_dialog_run( GNOME_DIALOG( dialog ) );
  coco_cookie( "button_idx = %d\n", button_idx );
  if (0==button_idx) /* OK button pressed */
    {
      int radio_idx = coco_tutorial_dialog_get_selected( dialog );
      char *pathname, filename[200];
      assert( radio_idx >= 0 );
      assert( radio_idx < COCO_TUTORIAL_INFO_SIZE );
      g_snprintf( filename, 200, "%s.%s", coco_tutorial_infos[radio_idx].name, 
                  COCO_TUTORIAL_SUFFIX );
      pathname = g_concat_dir_and_file( coco_level_directory, filename );
      coco_make_server_process( COCO_TRUE, 
                                coco_tutorial_infos[radio_idx].left_comps,
                                coco_tutorial_infos[radio_idx].right_comps,
                                pathname, coco_port );
      g_free( pathname );
      coco_join_game_interactively( coco_local_server, coco_port, 
                                    COCO_LOCALHOST_JOIN_TRIES,
                                    GTK_WINDOW( dialog ), COCO_TRUE );
    }
  if (GNOME_IS_DIALOG( dialog )) /* maybe its already dead */
    gnome_dialog_close( GNOME_DIALOG( dialog ) );
}



#ifdef COCO_IGNORE

/*

On 10 May 2001 08:47:00 -0700, Dave Morse wrote:
> 
> 
> > ----------
> > From:       Dave Morse[SMTP:DM-GTK@BOMBERLAN.NET]
> > Sent:       Thursday, May 10, 2001 9:47:00 AM
> > To:         gtk-app-devel-list@gnome.org
> > Subject:    dialog with timeout
> > Auto forwarded by a Rule
> > 
> _________________________________________________
> |Connecting to 12.34.56.78:9012                 |
> |_______________________________________________|
> |                                               |
> |                                               |
> | [==============            (try 2/10)      ]  |  <-- progress bar
> |                                               |
> |                                               |
> |                                       CANCEL  |
> |_______________________________________________|
> 
> 
> I'm trying to create a modal dialog with a timeout.  The interface is
> connecting to a remote service.  This may fail up to ten times, each try
> should be seperated by 1 second of "idleness", where idleness means
> checking to see if the user has clicked the Cancel button, etc.  
> 
> How to do it?


From: Lance Capser <lmcapser@bigfoot.com>
Subject: Re: dialog with timeout
To: dm-gtk@bomberlan.net
Cc: gtk-app-devel-list@gnome.org
Date: 10 May 2001 13:04:39 -0600
X-Mailer: Evolution/0.10 (Preview Release)
In-Reply-To: <15098.47092.929007.314994@ork.bomberlan.net>

I'll take a crack at this one :)

First, create a standard dialog box with the cancel button set to close
the dialog.  Make the dialog modal and call gtk_window_set_transients to
force the dialog to stay on top of the parent window.  For the retries
and the progress bar, call gtk_timeout_add() and set it to call a
"retry" function every second.  In the retry func, try to connect to the
remote service and update the progress bar (you will need to do
while(gtk_events_pending()) {gtk_main_iteration()} to update the
progress bar on the screen while still in the timeout func).  If you
successfully connect, close the dialog and return TRUE to disable the
timout.  Otherwise, leave the dialog on screen and return FALSE - this
will leave the timeout in place to call you back again in another
second.

Simple :)


*/

/* parent_window may be NULL */
GtkWidget *
coco_join_progress_dialog_new( GtkWindow *parent_window, char *host, int port,
                               GtkWidget **progress_return )
{
  int i;
  char str[100];
  GtkBox *vbox;
  GtkWidget *dialog;
  GtkWidget *progress;

  g_snprintf( str, sizeof( str ), "connecting to %s:%d", host, port );
  str[sizeof( str )-1] = '\0';
  dialog = gnome_dialog_new( str, GNOME_STOCK_BUTTON_CANCEL, NULL );
  vbox = GTK_BOX( GNOME_DIALOG( dialog )->vbox );
  if (parent_window)
    gnome_dialog_set_parent( GNOME_DIALOG( dialog ), parent_window );
  progress = gtk_progress_bar_new();
  gtk_progress_bar_set_orientation( GTK_PROGRESS_BAR( progress ),
                                    GTK_PROGRESS_LEFT_TO_RIGHT );
  gtk_box_pack_start( vbox, progress, TRUE, TRUE, 0 );
  gtk_widget_show( progress );
  if (progress_return) *progress_return = progress;
  return dialog;
}
#endif /* COCO_IGNORE */

/* returns success */
coco_bool
coco_join_game_interactively( const char *server, int port, int ntry,
                              GtkWindow *parent_window /* NON NULL */,
                              coco_bool join_asap)
{
  enum coco_network_error cerror;
  int uerror = 0;
  coco_bool have_error = COCO_TRUE;
  int i;
  coco_start_game_asap = join_asap;
  for (i=0; i<ntry; i++)
    {
      if (coco_client_connect_to_server( &cerror, &uerror ))
        {
          have_error = COCO_FALSE;
          break;
        }
      else if (!((COCO_NETERROR_CONNECT_TCP == cerror)
                 && (ECONNREFUSED == uerror)))
        {
          have_error = COCO_TRUE;
          break;
        }
      else /* retryable error */ 
        {
          
          have_error = COCO_TRUE;
          /*
          should post progress dialog with cancel button
          if ((0==i) && (ntry > 1))
            dialog = coco_join_progress_dialog_new( parent_window, 
                                                    server, port, &progress );
            ...                                        
          */
          sleep( 1 );
        } 
    }
  if (have_error)
    {
      char *errorstr 
        = coco_user_level_network_error_message( cerror, uerror,
                                                 COCO_TRUE );
      gnome_dialog_run_and_close
        ( GNOME_DIALOG
          ( gnome_error_dialog_parented( errorstr, parent_window) ) );
      coco_free( errorstr );
      return COCO_FALSE;
    }
  else return COCO_TRUE;

}


void
coco_make_server_process( coco_bool tutorial, int left_comps, int right_comps,
                          char *full_level_file_name, int iport )
{
  pid_t pid;

  pid = fork();

  if (0==pid)
    {
      char port[10], lcomps[10], rcomps[10];
      sprintf( port, "%d", iport );
      sprintf( lcomps, "%d", left_comps );
      sprintf( rcomps, "%d", right_comps );
      if (tutorial)
        execlp( coco_run_name, coco_run_name, "server", "--quiet",
                "--level", full_level_file_name, "--port", port,
                "--left-comps", lcomps, 
                "--right-comps", rcomps , "--tutorial", NULL );
      else
        execlp( coco_run_name, coco_run_name, "server", "--quiet",
                "--level", full_level_file_name, "--port", port,
                "--left-comps", lcomps, 
                "--right-comps", rcomps , NULL );

    }
  else if (-1==pid)
    coco_fatal_printf( COCO_ERROR_OUT_OF_MEMORY,
                       "fork() failed: %s\n",
                       coco_errno_string( errno ) );
  /* else we're still the client, and everything is good. */
}


#define COCO_NEW_GAME_FRAME_COUNT 3
#define COCO_NEW_GAME_NETWORK_FRAME_IDX 0
#define COCO_NEW_GAME_LEVEL_FRAME_IDX 1
#define COCO_NEW_GAME_COMP_FRAME_IDX 2
coco_bool
coco_use_new_game_dialog( GtkWidget *parent_window )
{
  int i;
  gint button_index;
  GtkWidget *dialog, *vbox;
  GtkWidget *frames[COCO_NEW_GAME_FRAME_COUNT];
  GtkWidget *port_ed;
  GtkWidget *radio_bi, *radio_fi, *level_bi_options, *level_fi_entry;
  GtkObject *left_comps_adj, *right_comps_adj;
  struct coco_built_in_level_menu_closure closure;
  static char *frame_names[COCO_NEW_GAME_FRAME_COUNT]
    = { "Network", "Level", "Computer Players" };

  coco_GtkKennel *kennel 
    = coco_gtk_kennel_new( COCO_GTK_KENNEL_TO_MAXIMUM, 
                           COCO_GTK_KENNEL_TO_MAXIMUM ); 

  coco_GtkKennel *label_kennel 
    = coco_gtk_kennel_new( COCO_GTK_KENNEL_TO_MAXIMUM, 
                           COCO_GTK_KENNEL_TO_MAXIMUM ); 

  dialog = gnome_dialog_new( "Start a Server",
                             GNOME_STOCK_BUTTON_OK,
                             GNOME_STOCK_BUTTON_CANCEL,
                             NULL );
  vbox = GNOME_DIALOG( dialog )->vbox;

  gnome_dialog_set_parent(GNOME_DIALOG( dialog ), GTK_WINDOW( parent_window ));

  gtk_object_set_data_full( GTK_OBJECT( dialog ), 
                            "coco kennel", kennel,
                            (GtkDestroyNotify) coco_gtk_kennel_unref );
  coco_gtk_kennel_ref( kennel );

  gtk_object_set_data_full( GTK_OBJECT( dialog ), 
                            "coco label kennel", label_kennel,
                            (GtkDestroyNotify) coco_gtk_kennel_unref );
  coco_gtk_kennel_ref( label_kennel );


  for (i=0; i<COCO_NEW_GAME_FRAME_COUNT; i++)
    {
      GtkWidget *frame = gtk_frame_new( frame_names[i] );
      frames[i] = frame;
      gtk_widget_show( frame );
      gtk_box_pack_start( GTK_BOX( vbox ), frame, TRUE, TRUE, 0 );
    }
  
  { /* build network frame */
    GtkWidget *label = gtk_label_new( "Port:" );
    GtkWidget *hbox = gtk_hbox_new( FALSE, COCO_UI_PAD_HORIZONTAL );
    port_ed = coco_port_editor_new();
    gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
    gtk_box_pack_start( GTK_BOX( hbox ), label, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( hbox ), port_ed, FALSE, TRUE, 0 );
    gtk_container_set_border_width( GTK_CONTAINER( hbox ), 
                                    COCO_UI_PAD_VERTICAL );
    coco_gtk_kennel_add( kennel, port_ed );
    coco_gtk_kennel_add( label_kennel, label );
    gtk_widget_show_all( hbox );
    gtk_container_add( GTK_CONTAINER( frames
                                      [COCO_NEW_GAME_NETWORK_FRAME_IDX] ),
                       hbox );
  }

  { /* build level frame */
    coco_bool built_in_found_p;
    GtkWidget *table, *menu;
    table = gtk_table_new( 2, 2, FALSE );

    radio_bi
      = gtk_radio_button_new_with_label_from_widget( NULL, "Built In:" );
    radio_fi
      = gtk_radio_button_new_with_label_from_widget
      ( GTK_RADIO_BUTTON( radio_bi ), "File:" );

    coco_gtk_kennel_add( label_kennel, radio_bi );
    coco_gtk_kennel_add( label_kennel, radio_fi );

    level_bi_options = gtk_option_menu_new();
    menu = gtk_menu_new();
    coco_cookie( "Menu set at %p\n", menu );

    { /* add menu items */
      const char *leveldir = coco_level_directory;
      int rezult;
      closure.menu = menu;
      closure.selection_size = 0;
      closure.selection = NULL;
      closure.item_default = NULL;
      coco_cookie( "Leveldir = %s\n", leveldir );
      rezult = coco_map_directory( leveldir, 
                                   coco_add_menuitem_for_ccl_strings,
                                   &closure );
      if (rezult != 0)
        {
          built_in_found_p = COCO_FALSE;
          coco_warn_printf( "Couldn't read built in level "
                            "directory \"%s\":%s.\n",
                            leveldir, coco_errno_string( rezult ) );
        }
      else built_in_found_p = COCO_TRUE;

      if (closure.item_default)
        gtk_menu_item_activate( GTK_MENU_ITEM( closure.item_default ) );
    }
    gtk_option_menu_set_menu( GTK_OPTION_MENU( level_bi_options ), menu );

    level_fi_entry = gnome_file_entry_new( "coco level", "File:" );
    gnome_file_entry_set_modal( GNOME_FILE_ENTRY( level_fi_entry ), TRUE );

    gtk_table_attach( GTK_TABLE( table ), radio_bi,
                      0, 1, 0, 1, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_table_attach( GTK_TABLE( table ), radio_fi,
                      0, 1, 1, 2, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_table_attach( GTK_TABLE( table ), level_bi_options,
                      1, 2, 0, 1, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_table_attach( GTK_TABLE( table ), level_fi_entry,
                      1, 2, 1, 2, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_container_set_border_width( GTK_CONTAINER( table ), 
                                    COCO_UI_PAD_VERTICAL );
    coco_gtk_kennel_add( kennel, level_bi_options );
    coco_gtk_kennel_add( kennel, level_fi_entry );

    gtk_widget_show_all( table );

    gtk_signal_connect( GTK_OBJECT( radio_bi ), "toggled",
                        GTK_SIGNAL_FUNC( coco_level_built_in_callback ), 
                        (gpointer) level_bi_options );
    gtk_signal_connect( GTK_OBJECT( radio_fi ), "toggled",
                        GTK_SIGNAL_FUNC( coco_level_built_in_callback ), 
                        (gpointer) level_fi_entry );

    if (!built_in_found_p)
      gtk_widget_set_sensitive( radio_bi, FALSE );

    {
      /* select twice, just to make sure everyone gets the signal to grey
         out their crud. */
      gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( radio_fi ), TRUE );
      if (built_in_found_p)
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( radio_bi ), TRUE );
    }

    gtk_container_add( GTK_CONTAINER( frames
                                      [COCO_NEW_GAME_LEVEL_FRAME_IDX] ),
                       table );
  }

  { /* build computer player frame */
    GtkWidget *table, *label_left, *label_right, *comps_left, *comps_right;
    int half_comps = COCO_MAX_COMPS>>1;
    left_comps_adj = gtk_adjustment_new( 0, 0, half_comps, 1, 1, 0 );
    right_comps_adj = gtk_adjustment_new( 0, 0, half_comps, 1, 1, 0 );

    table = gtk_table_new( 2, 2, FALSE );

    label_left = gtk_label_new( "Left:" );
    label_right = gtk_label_new( "Right:" );

    gtk_label_set_justify( GTK_LABEL( label_left ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( label_left ), 0.0, 0.5 );
    gtk_label_set_justify( GTK_LABEL( label_right ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( label_right ), 0.0, 0.5 );

    comps_left = gtk_spin_button_new( GTK_ADJUSTMENT( left_comps_adj ),
                                      1.0, 0 );

    comps_right = gtk_spin_button_new( GTK_ADJUSTMENT( right_comps_adj ), 
                                       1.0, 0 );

    gtk_table_attach( GTK_TABLE( table ), label_left,
                      0, 1, 0, 1, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_table_attach( GTK_TABLE( table ), label_right,
                      0, 1, 1, 2, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_table_attach( GTK_TABLE( table ), comps_left,
                      1, 2, 0, 1, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_table_attach( GTK_TABLE( table ), comps_right,
                      1, 2, 1, 2, GTK_EXPAND|GTK_FILL, GTK_EXPAND, 0, 0 );
    gtk_container_set_border_width( GTK_CONTAINER( table ), 
                                    COCO_UI_PAD_VERTICAL );

    coco_gtk_kennel_add( label_kennel, label_left );
    coco_gtk_kennel_add( label_kennel, label_right );

    coco_gtk_kennel_add( kennel, comps_left );
    coco_gtk_kennel_add( kennel, comps_right );
    gtk_widget_show_all( table );

    gtk_container_add( GTK_CONTAINER( frames
                                      [COCO_NEW_GAME_COMP_FRAME_IDX] ),
                       table );


  }

  /* coco_gtk_kennel_resize( kennel, 5, 5 ); */

 RETRY:
  button_index = gnome_dialog_run( GNOME_DIALOG( dialog ) );
  if (0==button_index)
    {
      char *level_file, *level_file_text, *level_load_error = NULL;
      coco_gtk_set_wait_cursor( dialog, COCO_TRUE );
      if (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( radio_bi ) ))
        {
          assert( closure.selection );
          coco_cookie( "dir = %s, file = %s", coco_level_directory,
                       closure.selection );
          /* g_concat_dir_and_file always returns freshly malloced string */
          level_file 
            = g_concat_dir_and_file( coco_level_directory, closure.selection );
        }
      else if (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( radio_fi ) ))
        {
          /* gnome_file_entry_get_full_path returns a freshly malloced
             string */
          level_file = gnome_file_entry_get_full_path
            ( GNOME_FILE_ENTRY( level_fi_entry ), FALSE );
        }
      else
        {
          coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                             "Neither level radio button is selected." );
          return 0;
        }
      level_file_text = coco_map_level_text( level_file, &level_load_error );
      if (level_file_text)
        level_load_error = coco_load_level_definition( level_file_text, NULL );
      if (level_load_error) 
        {
          static const char *level_load_error_preamble1
            = "Error loading ";
          static const char *level_load_error_preamble2
            = ":\n  ";
          static const char *level_load_error_preamble3
            = "\nPlease select another level.  ";
          
          char *really_long_level_load_error
            = coco_malloc( 1 + strlen( level_load_error )
                           + strlen( level_load_error_preamble1 )
                           + strlen( level_load_error_preamble2 )
                           + strlen( level_load_error_preamble3 )
                           + strlen( level_file ) );
          sprintf( really_long_level_load_error,
                   "%s%s%s%s%s", level_load_error_preamble1,
                   level_file, level_load_error_preamble2,
                   level_load_error, level_load_error_preamble3 );
          gnome_dialog_run_and_close
            ( GNOME_DIALOG
              ( gnome_error_dialog_parented( really_long_level_load_error, 
                                             GTK_WINDOW( dialog ) ) ) );
          coco_free( level_load_error );
          coco_free( really_long_level_load_error );
          g_free( level_file );
          coco_gtk_set_wait_cursor( dialog, COCO_FALSE );
          goto RETRY;
        }
      else 
        {
          coco_bool joined;
          int rcompsi = (int) (GTK_ADJUSTMENT( right_comps_adj )->value);
          int lcompsi = (int) (GTK_ADJUSTMENT( left_comps_adj )->value);

          /* assign global value */
          coco_port = coco_port_editor_get_value( port_ed );

          coco_make_server_process( COCO_FALSE, lcompsi, rcompsi, level_file,
                                    coco_port );
          joined = coco_join_game_interactively( coco_local_server, coco_port, 
                                                 COCO_LOCALHOST_JOIN_TRIES,
                                                 GTK_WINDOW( dialog ),
                                                 COCO_FALSE );
          g_free( level_file );
          if (joined)
            {
              gnome_dialog_close( GNOME_DIALOG( dialog ) );
              coco_free( closure.selection );
              gtk_widget_show( coco_chatroom_dialog_new
                               ( GTK_WINDOW( coco_root_window ) ) );
              return COCO_TRUE;
            }
          else
            {
              coco_gtk_set_wait_cursor( dialog, COCO_FALSE );
              goto RETRY;
            }
        }
    }
  else /* its 1 (CANCEL) or -1 (KILL), or something suspicious */
    {
      if (GNOME_IS_DIALOG( dialog )) /* maybe its already dead */
        gnome_dialog_close( GNOME_DIALOG( dialog ) );
      return COCO_FALSE;
    }
}

/* Instead of refreshing immediately, queue and espose for future
   refreshing. */
void
coco_refresh_window_eventually( void )
{
#ifdef COCO_DEBUG
  coco_warn_printf( "coco_refresh_window_eventually unimplemented\n" );
#endif
  return;
}

void
coco_notify_version_incompatible( int major, int minor )
{
  char buf[300];
  snprintf( buf, 300, 
            "You have been disconnected because the server\n"
            "is running %s version %d.%d, which is incompatible\n"
            "with your version %d.%d.\n",
            coco_product_name, major, minor, 
            COCO_MAJOR_VERSION, COCO_MINOR_VERSION );
  gnome_error_dialog_parented( buf, GTK_WINDOW( coco_root_window ) );
}


void 
coco_make_window( void )
{
  GtkWidget *vbox;
  static int db_attribs[] = { GDK_GL_RGBA, GDK_GL_DOUBLEBUFFER, None };
  static int sb_attribs[] = { GDK_GL_RGBA, None };

  coco_root_window = gnome_app_new( coco_product_name, coco_product_name );
  gtk_object_ref( GTK_OBJECT( coco_root_window ) ); /* for global variable */

  gtk_signal_connect( GTK_OBJECT( coco_root_window ), "delete_event",
                      GTK_SIGNAL_FUNC( coco_root_delete_callback ), NULL );
  gtk_signal_connect( GTK_OBJECT( coco_root_window ), "destroy",
                      GTK_SIGNAL_FUNC( coco_root_destroy_callback ), NULL );

  if (!coco_command_line_inhibit_double_buffer)
    coco_gl_window = gtk_gl_area_new( db_attribs );
  if (coco_gl_window)
    coco_double_buffer = COCO_TRUE;
  else 
    {
      coco_double_buffer = COCO_FALSE;
      coco_gl_window = gtk_gl_area_new( sb_attribs );
      if (NULL==coco_gl_window)
        coco_fatal_printf( COCO_ERROR_NO_VISUAL,
                           "gtk_gl_area_new() returned NULL, either there "
                           "is an GtkGLArea installation problem or the X"
                           " display is SERIOUSLY low tech." );
    }
  gtk_object_ref( GTK_OBJECT( coco_gl_window ) ); /* for global variable */

  gtk_widget_set_events ( coco_gl_window,
                          GDK_POINTER_MOTION_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_EXPOSURE_MASK
                          | GDK_KEY_PRESS_MASK
                          | GDK_KEY_RELEASE_MASK
                          | GDK_ENTER_NOTIFY_MASK
                          | GDK_LEAVE_NOTIFY_MASK
                          );

  GTK_WIDGET_SET_FLAGS( coco_gl_window, GTK_CAN_FOCUS );

  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "expose_event",
                      GTK_SIGNAL_FUNC( coco_draw_callback ), NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "configure_event",
                      GTK_SIGNAL_FUNC( coco_reshape_callback ), NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "realize",
                      GTK_SIGNAL_FUNC( coco_realize_callback ), NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "key_press_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "key_release_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "button_press_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "button_release_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "motion_notify_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "enter_notify_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );
  gtk_signal_connect( GTK_OBJECT( coco_gl_window ), "leave_notify_event",
                      GTK_SIGNAL_FUNC( coco_input_event_callback ), 
                      NULL );

  gtk_drawing_area_size( GTK_DRAWING_AREA( coco_gl_window ), 
                         COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H );

  vbox = gtk_vbox_new( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( vbox ), coco_menubar_new( coco_root_window ),
                      FALSE, FALSE, 0 );
  gtk_box_pack_start ( GTK_BOX( vbox ), coco_gl_window, TRUE, TRUE, 0 );
  gnome_app_set_contents( GNOME_APP( coco_root_window ), vbox );  

  /* disable user resizing */
  gtk_window_set_policy( GTK_WINDOW( coco_root_window ), FALSE, FALSE, TRUE );
  gtk_widget_show_all( coco_root_window );
  gtk_widget_grab_focus( coco_gl_window );

  return;
}

