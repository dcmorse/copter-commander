/*
 * Copter Commander - A 2d network helicopter game
 * Copyright (C) 20001  David Morse
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

#define COCO_STAR_COUNT ((COCO_OFFICIAL_WINDOW_W*COCO_OFFICIAL_WINDOW_H)>>11)

const coco_bool coco_draw_background_before_everything_else = COCO_TRUE;

static struct {
  coco_gnum x;
  coco_gnum y;
  coco_color color;
} coco_stars[COCO_STAR_COUNT];


void
coco_init_background( void )
{
  int i;
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

/* draw the background starscape.  Most efficient for x==y==0. */
void
coco_draw_background( coco_gnum xoffset, coco_gnum yoffset, coco_bool fullscreen_p )
{
  static GLuint display_list = 0;
  if (fullscreen_p || (0!=xoffset) || (0!=yoffset))
    coco_draw_starscape_points( xoffset, yoffset, fullscreen_p );
  else if (0==display_list)
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
}
