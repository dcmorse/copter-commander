#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <assert.h>
#include "coco.h"


static coco_bool set_video_mode = COCO_FALSE;
static int old_video_mode_w = -1;
static int old_video_mode_h = -1;


/* this technique of using the extension more or less ripped off from
   SDL, since I couldn't find any actual DOCs. */
coco_bool coco_set_video_mode_to_dimensions( Display *display, int w, int h )
{
  coco_bool success = COCO_FALSE;
#ifdef COCO_USE_XF86VIDMODE
  int screen = DefaultScreen( display );
  int ignore1;
  int ignore2;
  int ignore3;
  XF86VidModeModeLine mode;
  XF86VidModeModeInfo **modes;
  int nmodes;

  if (!XF86VidModeQueryExtension( display, &ignore1, &ignore2 ))
    return COCO_FALSE;

  coco_cookie ("Sniffing for video modes, goal resolution is %dx%d\n", w, h );
  if ( XF86VidModeGetModeLine( display, screen, &ignore3, &mode)
       && XF86VidModeGetAllModeLines( display, screen, &nmodes, &modes))
    {
      int i;
      for (i=0; i<nmodes; i++)
	{
	  /* Do I want to optimize dotclock?  What the heck is dotclock anyway? */
	  coco_cookie ("found mode %dx%d dotclock=%d\n", modes[i]->hdisplay, modes[i]->vdisplay, modes[i]->dotclock );
	  if ((modes[i]->hdisplay == w)
	      && (modes[i]->vdisplay == h))
	    {
	      if ((modes[i]->hdisplay != mode.hdisplay)
		  || (modes[i]->vdisplay != mode.vdisplay) )
		{
		  old_video_mode_w = mode.hdisplay;
		  old_video_mode_h = mode.vdisplay;
		  coco_cookie ("CHANGING VIDEO MODE from %dx%d!!!!\n", old_video_mode_w, old_video_mode_h );
		  if (XF86VidModeSwitchToMode( display, screen, modes[i] ))
		    success = COCO_TRUE;
		  break; 
		}
	    }
	}
      XFree( modes );
    }
#endif /* COCO_USE_XF86VIDMODE */
  return success;
}


coco_bool 
coco_set_video_mode_from_xdisplay( Display *display )
{
  assert( display );
  if (!set_video_mode)
    {
      coco_gnum x;
      coco_gnum y;
      int w = COCO_OFFICIAL_WINDOW_W;
      int h = COCO_OFFICIAL_WINDOW_H;
      coco_window_xy( &x, &y );
      set_video_mode = coco_set_video_mode_to_dimensions( display, w, h );
#ifdef COCO_USE_XF86VIDMODE
      XF86VidModeSetViewPort( display, DefaultScreen( display ), x, y );
#endif
    }
  return set_video_mode;
}


void 
coco_restore_video_mode_from_xdisplay( Display *display )
{
  int w = old_video_mode_w;
  int h = old_video_mode_h;
  assert( display );
  coco_cookie( "coco_restore_video_mode_from_xdisplay() here!  Planning to do %s\n", set_video_mode ? "something" : "nothing" );
  if (set_video_mode)
    set_video_mode = !coco_set_video_mode_to_dimensions( display, w, h );
}
