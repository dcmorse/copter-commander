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

const coco_bool coco_draw_background_before_everything_else = COCO_FALSE;

void
coco_init_background( void )
{
}

#define COCO_GRAPH_PAPER_RESOLUTION 23

/* draw the background starscape.  Most efficient for x==y==0. */
void
coco_draw_background( coco_gnum xoffset, coco_gnum yoffset )
{
  static GLuint display_list = 0;
  xoffset %= COCO_GRAPH_PAPER_RESOLUTION<<2;
  if (xoffset < 0) xoffset += COCO_GRAPH_PAPER_RESOLUTION<<2;
  yoffset %= COCO_GRAPH_PAPER_RESOLUTION<<2;
  if (yoffset < 0) yoffset += COCO_GRAPH_PAPER_RESOLUTION<<2;

  if (0==display_list)
    {
      int i;
      display_list = glGenLists( 1 );
      if (0==display_list)
        coco_fatal_printf( COCO_ERROR_DISPLAY_LISTS,
                           "Couldn't allocate display list for stars.\n" );
      glNewList( display_list, GL_COMPILE );
      coco_draw_color( COCO_COLOR_LIGHT_BLUE );
      for (i=-(COCO_GRAPH_PAPER_RESOLUTION<<2);
           i<COCO_OFFICIAL_WINDOW_W;
           i+=COCO_GRAPH_PAPER_RESOLUTION)
        coco_draw_line( i, -(COCO_GRAPH_PAPER_RESOLUTION<<2), i, 
                        COCO_OFFICIAL_WINDOW_H );
      for (i=-(COCO_GRAPH_PAPER_RESOLUTION<<2);
           i<COCO_OFFICIAL_WINDOW_H;
           i+=COCO_GRAPH_PAPER_RESOLUTION)
        coco_draw_line( -(COCO_GRAPH_PAPER_RESOLUTION<<2), i, 
                        COCO_OFFICIAL_WINDOW_W, 
                        i );
      glEndList();
    }

  if (xoffset || yoffset)
    {
      glPushMatrix();
      glTranslatef( xoffset, yoffset, 0 );
      glCallList( display_list );
      glPopMatrix();
    }
  else glCallList( display_list );
}
