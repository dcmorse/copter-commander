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

#include <GL/gl.h>
#include "background.h"
#include <stdlib.h>             /* for rand() */
#include <time.h>               /* for localtime() */

#define COCO_STAR_COUNT ((COCO_OFFICIAL_WINDOW_W*COCO_OFFICIAL_WINDOW_H)>>11)

const coco_bool coco_draw_background_before_everything_else = COCO_TRUE;

static enum coco_sky_colors coco_sky_color;

void
coco_set_sky_color( void )
{
  int hour;
  if ((coco_sky_option >= 0) && (coco_sky_option < COCO_SKY_LIMIT))
    coco_sky_color = coco_sky_option;
  else
    {
      time_t tt;
      struct tm *lt;
      time( &tt );
      lt = localtime( &tt );
      hour = lt->tm_hour;
      switch (hour) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
        coco_sky_color = COCO_NIGHT;
        break;
      case 6:
      case 18:
        coco_sky_color = COCO_TWILIGHT;
        break;
      case 7:
      case 8:
      case 9:
      case 10:
      case 14:
      case 15:
      case 16:
      case 17:
        coco_sky_color = COCO_DAY;
        break;
      case 11:
      case 12:
      case 13:
        coco_sky_color = COCO_NOON;
        break;
      }
    }
}

static struct {
  coco_gnum x;
  coco_gnum y;
  coco_color color;
} coco_stars[COCO_STAR_COUNT];


void
coco_init_background( void )
{
  int i;

  coco_set_sky_color();
  switch (coco_sky_color) {
  case COCO_NOON:
  case COCO_DAY:
    COCO_COLOR_FONT = COCO_COLOR_BLACK;
    break;
  case COCO_NIGHT:
  case COCO_TWILIGHT:
    COCO_COLOR_FONT = COCO_COLOR_LIGHT_BLUE;
    break;
  default: coco_warn_printf( "Didn't init font color for unknown --sky=%d.\n",
                             coco_sky_color );
    break;
  }

  /* Intro still at night no matter what.
     if ((coco_sky_color == COCO_TWILIGHT) || (coco_sky_color == COCO_NIGHT))
   */
    {
      /* automatically change the starscape every version */
      srand( (COCO_MAJOR_VERSION << 4) + COCO_MINOR_VERSION );
      for (i = COCO_STAR_COUNT; i>0; i--)
        {
          int intens = (rand()&255);
          int intens2 = rand()&255; 
          int r = (rand()&31) ? intens : intens2;
          int b = (rand()&31) ? intens : intens2;
          int g = COCO_MIN( r, b );
          coco_stars[i].x = rand() % COCO_OFFICIAL_WINDOW_W;
          coco_stars[i].y = rand() % COCO_OFFICIAL_WINDOW_H;
          coco_stars[i].color = COCO_MAKE_COLOR( r, g, b, 255 );
        }
    }
}

/* internal to coco_draw_starscape */
void
coco_draw_starscape_points( coco_gnum xoffset,
                            coco_gnum yoffset, 
                            coco_bool fullscreen_p )
{
  int i;
  coco_gnum x, y;
  coco_begin_point_drawing();
  for (i = COCO_STAR_COUNT; i>0; i--)
    {
      y = (coco_stars[i].y+yoffset) % coco_real_window_h;
      if (y<0) y = y+coco_real_window_h;
      if (fullscreen_p || (y < COCO_VIEWABLE_H))
        {
          coco_draw_color( coco_stars[i].color );
          if ((xoffset!=0) || (yoffset!=0))
            {
              x = (coco_stars[i].x+xoffset) % coco_real_window_w;
              if (x<0) x = x+coco_real_window_w;
              coco_point_drawing_vertex( x, y );
            }
          else
            coco_point_drawing_vertex( coco_stars[i].x, 
                                       coco_stars[i].y );
        }
    }
  coco_end_point_drawing();
}


void
coco_draw_shaded_sky2( coco_color bottom, coco_color top, coco_bool grain )
{
  static coco_color last_bottom      = COCO_COLOR_BLACK;
  static coco_color last_top         = COCO_COLOR_BLACK;
  static coco_color toppish  = COCO_COLOR_BLACK;
  static coco_color bottomish = COCO_COLOR_BLACK;
  coco_gnum y1 = grain ? COCO_GROUND_Y : COCO_VIEWABLE_H;
  coco_gnum y2 = grain ? COCO_VIEWABLE_H : COCO_GROUND_Y;
  if ((top != last_top) || (bottom != last_bottom))
    { /* cache invalid: interpolate again */
      toppish 
        = COCO_MAKE_COLOR( (3*COCO_RED( top )   + COCO_RED( bottom ))  >>2,
                           (3*COCO_GREEN( top ) + COCO_GREEN( bottom ))>>2,
                           (3*COCO_BLUE( top )  + COCO_BLUE( bottom )) >>2,
                           (3*COCO_ALPHA( top ) + COCO_ALPHA( bottom ))>>2);
      bottomish 
        = COCO_MAKE_COLOR( (COCO_RED( top )   + 3*COCO_RED( bottom ))  >>2,
                           (COCO_GREEN( top ) + 3*COCO_GREEN( bottom ))>>2,
                           (COCO_BLUE( top )  + 3*COCO_BLUE( bottom )) >>2,
                           (COCO_ALPHA( top ) + 3*COCO_ALPHA( bottom ))>>2);
      last_bottom = bottom;
      last_top    = top;
    }
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glBegin( GL_QUADS );
  coco_draw_color( top );
  glVertex2i( COCO_OFFICIAL_WINDOW_W, y1 );
  coco_draw_color( bottomish );
  glVertex2i( COCO_OFFICIAL_WINDOW_W, y2 );
  coco_draw_color( bottom );
  glVertex2i( 0, y2 );
  coco_draw_color( toppish );
  glVertex2i( 0, y1 );
  glEnd();
}

void
coco_draw_shaded_sky3( coco_color bottom, coco_color top, 
                       coco_gnum y1, coco_gnum y2 )
{
  static coco_color last_bottom      = COCO_COLOR_BLACK;
  static coco_color last_top         = COCO_COLOR_BLACK;
  static coco_color bottomish = COCO_COLOR_BLACK;
  if ((top != last_top) || (bottom != last_bottom))
    { /* cache invalid: interpolate again */
      bottomish 
        = COCO_MAKE_COLOR( (COCO_RED( top )   + 3*COCO_RED( bottom ))  >>2,
                           (COCO_GREEN( top ) + 3*COCO_GREEN( bottom ))>>2,
                           (COCO_BLUE( top )  + 3*COCO_BLUE( bottom )) >>2,
                           (COCO_ALPHA( top ) + 3*COCO_ALPHA( bottom ))>>2);
      last_bottom = bottom;
      last_top    = top;
    }
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glBegin( GL_QUADS );
  coco_draw_color( bottomish );
  glVertex2i( COCO_OFFICIAL_WINDOW_W, COCO_GROUND_Y );
  coco_draw_color( top );
  glVertex2i( COCO_OFFICIAL_WINDOW_W, y2 );
  glVertex2i( 0, y1 );
  coco_draw_color( bottom );
  glVertex2i( 0, COCO_GROUND_Y );
  glEnd();
}


/* draw the background starscape.  Most efficient for x==y==0. */
void
coco_draw_background( coco_gnum xoffset, coco_gnum yoffset, coco_bool fullscreen_p )
{
  static GLuint display_list = 0;
  if (fullscreen_p || (0!=xoffset) || (0!=yoffset))
    coco_draw_starscape_points( xoffset, yoffset, fullscreen_p );
  else switch (coco_sky_color) {
  case COCO_DAY:
    coco_draw_color( COCO_COLOR_BACKGROUND );
    coco_draw_rectangle( 0, COCO_VIEWABLE_H, 
                         COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H );
    coco_draw_shaded_sky2( COCO_COLOR_SUN, COCO_COLOR_SKY, COCO_FALSE );
    break;
  case COCO_NOON:
    coco_draw_color( COCO_COLOR_BACKGROUND );
    coco_draw_rectangle( 0, COCO_VIEWABLE_H, 
                         COCO_OFFICIAL_WINDOW_W, COCO_OFFICIAL_WINDOW_H );
    coco_draw_shaded_sky2( COCO_COLOR_SUN, COCO_COLOR_SKY, COCO_TRUE );
    break;
  case COCO_NIGHT:
  case COCO_TWILIGHT:
  default:
    coco_clear_window_color( COCO_COLOR_NIGHT );
    coco_clear_window();
    if (coco_sky_color == COCO_TWILIGHT)
      coco_draw_shaded_sky3( COCO_COLOR_TWIGHLIGHT_SKY, 
                             COCO_COLOR_NIGHT, 350, 230 );
    if (0==display_list)
      {
        display_list = glGenLists( 1 );
        if (0==display_list)
          coco_fatal_printf( COCO_ERROR_DISPLAY_LISTS,
                             "Couldn't allocate display list for stars.\n" );
        glNewList( display_list, GL_COMPILE_AND_EXECUTE );
        coco_draw_starscape_points( 0, 0, fullscreen_p );
        glEndList();
      }
    else glCallList( display_list );
    break;
  }
}
