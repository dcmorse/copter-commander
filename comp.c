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

/* comp.c - a rudimentary copter ai */

#include <math.h> /* for COCO_SIN */
#include <stdlib.h> /* for rand */
#include "coco.h"



#define COCO_GUN_DY_THRESHOLD 40

/* perform side effects on self->kpstate, (and self->cruise_altitude) */
void
coco_comp_think( struct coco_comp *self, 
                 struct coco_cstate_serial_header *cstate,
                 coco_cobject *cobjs,
                 struct coco_cblip *cblips )
{
  struct coco_keyboard_pointer_state *state_return = &(self->kpstate);
  int ci, minci;
  coco_gnum x, y, min;
  coco_cobject *cself = cobjs + cstate->cobject_camera_index;
  coco_u32 date = cstate->time + self->index;

  coco_clear_bit_vec( state_return->key_state, COCO_KEYLIMIT, 0 );
  state_return->client_time = 0; /* not senseful for comp */
  state_return->pointer_x = 0;
  state_return->pointer_y = self->cruise_altitude;

  if (!coco_sobject_subtype_p( COCO_COBJECT_CL( cself ), COCO_COPTER_CLASS ))
    return;

  /* reset cruise_altitude and overgun periodically */
  if (0==(date & 3))
    {
      self->overgun = (rand() % 11) - 2;
      if (0==(date & 127))
        {
          coco_gnum mask = (COCO_VIEWABLE_H - COCO_AI_FLOOR)
            - COCO_COBJECT_CL( cself )->canonical_h;
          coco_gnum r1, r2;
          r1 = rand() % mask;
          r2 = rand() % mask;
          self->cruise_altitude = COCO_AI_FLOOR + COCO_MAX( r1, r2 );
        }
    }


  x = COCO_COBJECT_X( cself );
  y = COCO_COBJECT_Y( cself );

  /* scan cobjects for enemy players */
  min = COCO_GNUM_MAX;
  minci = -1;
  for (ci=0; ci<cstate->cobject_count; ci++)
    {
      coco_gnum cur, dx, dy;
      dx = COCO_COBJECT_X( cobjs+ci ) - x;
      dy = COCO_COBJECT_Y( cobjs+ci ) - y;
      cur = dx*dx + dy*dy;
      if ((min > cur)
          && coco_sobject_subtype_p( COCO_COBJECT_CL( cobjs+ci ), 
                                     COCO_COPTER_CLASS )
          && (self->nationality
              != COCO_COBJECT_NATIONALITY_GUESS( cobjs+ci )))
        {
          min = cur;
          minci = ci;
        }
    }
  if (minci>=0)
    {
      coco_gnum xgo;
      coco_gnum tx = COCO_COBJECT_X( cobjs+minci );
      coco_gnum ty = COCO_COBJECT_Y( cobjs+minci );
      coco_gnum dx = tx - x;
      coco_gnum dy = ty - y;
      coco_gnum adx = abs( dx );
      /* flip to face enemy */
      if ((dx<0) == (COCO_COBJECT_RASTER_INDEX( cself )&1))
        coco_comp_key_press( state_return, self, COCO_LPOINTER );

      /* figure out angle to fly at (could use tangent, but don't want
         robotic precision) */
      if (dy>COCO_GUN_DY_THRESHOLD)
        xgo = -COCO_OFFICIAL_WINDOW_W; /* run away! */
      else if (dy<-COCO_GUN_DY_THRESHOLD)
        xgo = COCO_OFFICIAL_WINDOW_W;  /* get em */
      else
        {
          xgo = -dy
             * (COCO_OFFICIAL_WINDOW_W/(COCO_GUN_DY_THRESHOLD)); /* fence */
          coco_cookie( "xgo=%d\n", xgo );
        }
      /* move towards it */
      state_return->pointer_x 
        = xgo * ((dx<0) ? -1 : 1);
      /* If we loose track later, go to top by default */
      if (dy>0) self->cruise_altitude = COCO_VIEWABLE_H;
      /* match altitude to line up guns */
      state_return->pointer_y = ty 
        + adx*COCO_SIN( COCO_COPTER_MAX_ANGLE_OF_ATTACK )
        + self->overgun;
      /* fire guns */
      if ((adx+100) > abs( dy ))
      coco_comp_key_press( state_return, self, COCO_KEY_SHIFT );
      /* drop bomb if belowish and closeish */
      if ((adx<400) && (dy<(50-(adx>>1))))
        coco_comp_key_press( state_return, self, COCO_CHAR_TO_KEY( 'z' ) );
    }
}
