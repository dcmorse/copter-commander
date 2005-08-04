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

/* comp.c - a rudimentary copter ai */

#include <math.h> /* for COCO_SIN */
#include <stdlib.h> /* for rand */
#include <assert.h> 
#include "coco.h"



#define COCO_GUN_DY_THRESHOLD 40

#define COCO_OFFENSIVE_FACING( nat ) (((nat)==COCO_GOOD_GUYS) ? -1 : 1)
#define COCO_DEFENSIVE_FACING( nat ) (((nat)==COCO_GOOD_GUYS) ? 1 : -1)

#define COCO_COPTER_FACING_RIGHT( cobj ) (COCO_COBJECT_RASTER_INDEX( cobj )&1)


void
coco_comp_ensure_facing( coco_facing facing, 
                         struct coco_keyboard_pointer_state *state_return,
                         struct coco_comp *self,
                         coco_cobject *cself )
{
  if ((facing>0) != COCO_COPTER_FACING_RIGHT( cself ))
    coco_comp_key_press( state_return, self, COCO_LPOINTER );
}

coco_cobject *
coco_comp_find_cobject( coco_cobject *cobjs, int ncobjs,
                        coco_sobject_cl *class,
                        int nationality_maybe,
                        coco_gnum x )
{
  int i;
  int it = -1;
  coco_gnum dx = COCO_GNUM_MAX;
  for (i=0; i<ncobjs; i++)
    {
      coco_gnum new_dx = abs( x - COCO_COBJECT_X( cobjs + i ) );
      if ((new_dx < dx)
          && ((-1==nationality_maybe)
              || (COCO_COBJECT_NATIONALITY_GUESS( cobjs+i )
                  == nationality_maybe))
          && coco_sobject_subtype_p( COCO_COBJECT_CL( cobjs+i ), class ))
        {
          it = i;
          dx = new_dx;
        }
    }
  if (it>=0)
    return cobjs + it;
  else 
    return NULL;
}



/* computes the number of bombs to be dropped.  For most targets, one bomb
   is enough, so this just roots through cobjs to find an already in
   flight bomb.  If it finds one, it returns 0, otherwise it returns 1.
   For tanks and other tougher targets, it guesses the number needed,
   subtracts the in-flights, and returns that. */
int
coco_comp_compute_needed_bombs( coco_cobject *targ, coco_cobject *self, 
                                coco_cobject *cobjs, int ncobjs )
{
  coco_cobject *cobj;
  coco_health hmax;
  int needed;
  int i;
  coco_gnum sx, sy;
  coco_mini_point shp;
  coco_sobject_cl *cla, *tcla;
  unsigned char ri;

  tcla = COCO_COBJECT_CL( targ );
  hmax = tcla->health_max;
  if (hmax == COCO_NIGH_INVULNERABLE)
    return 0; /* don't try dropping bombs on it */
  else if (coco_sobject_subtype_p( tcla, COCO_TANK_CLASS ))
    {
      needed = ceil( COCO_TANK_ANGLE_TO_HEALTH( COCO_COBJECT_ANGLE( targ ),
                                                hmax )
                     / (double) COCO_BOMB_DAMAGE );
    }
  else 
    needed = ceil( hmax / (double) COCO_BOMB_DAMAGE );

  cla = COCO_COBJECT_CL( self );
  ri = COCO_COBJECT_RASTER_INDEX( self );
  shp = coco_sobject_weapon_hardpoint_by_raster_index( cla, ri );
  sx = COCO_COBJECT_X( self ) + COCO_MINI_POINT_X( shp );
  sy = COCO_COBJECT_Y( self ) + COCO_MINI_POINT_Y( shp );
  for (i=0, cobj=cobjs; i<ncobjs; i++, cobj++)
    if (coco_sobject_subtype_p( COCO_COBJECT_CL( cobj ), COCO_BOMB_CLASS )
        && (COCO_COBJECT_Y( cobj ) < sy))
      {
        coco_gnum lhs = COCO_SOBJECT_X( cobj ) - 40;
        coco_gnum rhs = COCO_COBJECT_X( cobj ) + 40;
        if (COCO_BETWEENI( lhs, sx, rhs ))
          {
            needed--;
            if (needed<=0) 
              return 0;
          }
      }
  return needed;
}


/* other copters are frothy if they're more than this number from horizontal */
#define COCO_COMP_FROTHY_ANGLE 4


/* If changing, please sync COCO_COMP_BUY_INDEX_MAX to equal the
   number of entries!: */
static coco_sobject_cl *buy_schedule[COCO_COMP_BUY_INDEX_MAX]
 = { COCO_TANK_CLASS,
     COCO_FIRETRUCK_CLASS,
     COCO_DEMO_VAN_CLASS,
     COCO_ENGINEER_CLASS,
     COCO_DEMO_VAN_CLASS,
     COCO_FIRETRUCK_CLASS,
     COCO_INFANTRY_CLASS,
     COCO_INFANTRY_CLASS,
     COCO_FIRETRUCK_CLASS };

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
  int blip_max = cstate->cblip_count;
  struct coco_cblip *bself;
  coco_u32 date = cstate->time + self->index;

  /* init to some very basic values */
  coco_clear_bit_vec( state_return->key_state, COCO_KEYLIMIT, 0 );
  state_return->client_time = 0; /* not senseful for comp */
  state_return->pointer_x = 0;
  state_return->pointer_y = self->cruise_altitude;
 
  { /* spend money (if available) */
    coco_u32 funds = cstate->dashinfo.funds;
    coco_bool summon_lockout
      = cstate->dashinfo.bits & (1<<COCO_DASHINFO_BIT_SUMMON_DISABLE);
    coco_sobject_cl *buy_cl = buy_schedule[self->buy_index];
    coco_u32 threshold_funds = buy_cl->cost;
    if ((!summon_lockout) && (funds >= threshold_funds))
      {
	int i;
	for (i=0; i<COCO_UNIT_ORDER_KEYMAP_LENGTH; i++)
	  if (buy_cl == coco_unit_order_keymap[i].cl)
	    break;
	if (i<COCO_UNIT_ORDER_KEYMAP_LENGTH)
	  {
	    coco_comp_key_press( state_return, self, 
				 coco_unit_order_keymap[i].key );
	    /* assume the order goes through ... not 100% safe assumption, since teammate may be mashing keys too */
	    self->buy_index++;
	  }
	if (self->buy_index >= COCO_COMP_BUY_INDEX_MAX)
	  self->buy_index = 0;
      }
  }

  { /* find bself on radar */
    int i;
    bself = NULL;
    for (i=0; i<blip_max; i++)
      if (cblips[i].color_index == COCO_BLIP_CAMERA_BLINK)
        {
          bself = cblips + i;
          break;
        }
    if (bself == NULL)
      return;
  }

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
          && (coco_sobject_subtype_p( COCO_COBJECT_CL( cobjs+ci ), 
                                     COCO_COPTER_CLASS )
              || coco_sobject_subtype_p( COCO_COBJECT_CL( cobjs+ci ), 
                                         COCO_MECH_CLASS ))
          && (self->nationality
              != COCO_COBJECT_NATIONALITY_GUESS( cobjs+ci )))
        {
          min = cur;
          minci = ci;
        }
    }
#ifndef COCO_NONLETHAL_AI
  if (minci>=0) /* if enemy copter around, focus on that */
    {
      coco_gnum xgo;
      coco_gnum tx = COCO_COBJECT_X( cobjs+minci );
      coco_gnum ty = COCO_COBJECT_Y( cobjs+minci );
      coco_gnum dx = tx - x;
      coco_gnum dy = ty - y;
      coco_gnum adx = abs( dx );
      /* flip to face enemy */
      coco_comp_ensure_facing( ((dx<0) ? -1 : 1), state_return, self, cself );

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
      return;
    }
#endif /* COCO_NONLETHAL_AI */

  if ((cstate->dashinfo.bullets < 12) /* low ammo */
      || (cstate->board_w * 1.5 >=          /* low fuel */
          (COCO_COBJECT_CL( cself )->dx_max * cstate->dashinfo.fuel))
      /* || (cstate->dashinfo.fuel < (14 * 155)) TO TEST */
      || self->refuelling)
    {
      coco_cobject *pad =
        coco_comp_find_cobject( cobjs, cstate->cobject_count,
                                COCO_LANDING_PAD_CLASS, -1, x );
      if (pad != NULL)
        {
          /* coco_mini_point pt = coco_cobject_com( cself ); */
          coco_sobject_cl *cla = COCO_COBJECT_CL( cself );
          state_return->pointer_x
            = COCO_COBJECT_X( pad ) - x
            /*+ ((COCO_OFFICIAL_WINDOW_W
                + COCO_MINI_POINT_X( pt )
                - COCO_LANDING_PAD_CLASS->canonical_w)>>1) */;
          state_return->pointer_y 
            = ((COCO_GROUND_Y==y) ? COCO_VIEWABLE_H : COCO_GROUND_Y+2);
          self->refuelling =
            !((cstate->dashinfo.bullets >= cla->max_bullets)
              && (cstate->dashinfo.bombs >= cla->max_bombs)
              && (cstate->dashinfo.missiles >= cla->max_missiles)
              && (cstate->dashinfo.rockets >= cla->max_rockets)
              && (cstate->dashinfo.fuel >= cla->max_fuel));
          return;
        }
      else
        { /* not in sight yet, home in using radar */
          int i;
          coco_rxnum my_x = bself->x;
          coco_rxnum closest_x = COCO_RXNUM_MAX;
          int closest_i = -1;
          for (i=0; i<blip_max; i++)
            if ((cblips[i].color_index == COCO_BLIP_LANDING_PAD))
              {
                coco_rxnum diff = cblips[i].x - my_x;
                if ((self->nationality == COCO_GOOD_GUYS)
                    ? (diff < 0)
                    : (diff > 0))
                  diff = diff<<2;
                diff = abs( diff );
                if (diff < closest_x)
                  {
                    closest_x = diff;
                    closest_i = i;
                  }
              }
          if (closest_i == -1)
            coco_warn_printf( "AI can't find a landing pad on this level" );
          else
            {
              coco_facing face = (cblips[closest_i].x < my_x) ? -1 : 1;
              coco_comp_ensure_facing( face, state_return, self, cself );
              state_return->pointer_x = COCO_OFFICIAL_WINDOW_W * face;
              return;
            }

        }

    }
  /* Look for good things to bomb */
  {
    coco_cobject *target;
    coco_sobject_cl *tcl;

    target = coco_comp_find_cobject( cobjs, cstate->cobject_count,
                                     COCO_GROUND_UNIT_CLASS,
                                     (self->nationality == COCO_GOOD_GUYS)
                                     ? COCO_BAD_GUYS : COCO_GOOD_GUYS, x );
    tcl = target ? COCO_COBJECT_CL( target ) : NULL;

    if (target)
      self->cruise_altitude = COCO_CLAMP( COCO_INFANTRY_GUN_RANGE, 
                                          self->cruise_altitude,
                                          (COCO_VIEWABLE_H*3/4) );
    /*
    if (target)
      coco_cookie( "have target %s\n", COCO_COBJECT_CL( target )->name );
    */

    if (target && (cstate->dashinfo.bombs>0) && 
        (coco_comp_compute_needed_bombs( target, cself, cobjs, 
                                         cstate->cobject_count )>0))
      {
        coco_mini_point mmp, tmp;
        int mri, tri;
        coco_gnum delta, probable_target_dx;
        coco_sobject_cl *mcl;
        mcl = COCO_COBJECT_CL( cself );
        tcl = COCO_COBJECT_CL( target );
        mri = COCO_COBJECT_RASTER_INDEX( cself );
        tri = COCO_COBJECT_RASTER_INDEX( target );
        mmp = coco_sobject_weapon_hardpoint_by_raster_index( mcl, mri );
        tmp = coco_sobject_com_by_raster_index( tcl, tri );
        delta = COCO_COBJECT_X( target ) + COCO_MINI_POINT_X( tmp )
          - x - COCO_MINI_POINT_X( mmp );

        state_return->pointer_x = (delta>30)
          ? 240
          : ((delta<-30) ? -240 : (delta<<3));
        probable_target_dx = ((self->nationality == COCO_GOOD_GUYS) ? -1 : 1);
        if ((abs( delta ) < (coco_sobject_subtype_p( tcl, 
                                                     COCO_DEMO_VAN_CLASS )
                             ? 6 : 10))
            && (abs( cstate->dx + probable_target_dx ) < 2))
          coco_comp_key_press( state_return, self, COCO_CHAR_TO_KEY( 'z' ) );
        else
          self->cruise_altitude -= 1;
        return;
      }
    else if (target && (cstate->dashinfo.bombs==0))
      {
        coco_cookie( "getting more bombs\n" );
        self->refuelling = COCO_TRUE; /* go get some bombs to deal w/ these turkeys */
        return;
      }
  }


  { /* don't clump with other friendly copters */
    int myci = cstate->cobject_camera_index;
    for (ci=0; ci<myci; ci++)
      if ((abs(COCO_COBJECT_X( cobjs+ci ) - COCO_COBJECT_X( cself )) < 300)
          && coco_sobject_subtype_p( COCO_COBJECT_CL( cobjs+ci ), 
                                     COCO_COPTER_CLASS )
          && (self->nationality
              == COCO_COBJECT_NATIONALITY_GUESS( cobjs+ci )))
        { /* only chill if they're doing something */
          coco_angle angle = COCO_COBJECT_ANGLE( cobjs+ci );
          if (((angle > COCO_COMP_FROTHY_ANGLE) 
               && (angle < (128 - COCO_COMP_FROTHY_ANGLE)))
              || ((angle > (128 + COCO_COMP_FROTHY_ANGLE))
                  && (angle < (256 - COCO_COMP_FROTHY_ANGLE))))
            {
              /* chill out and wait for them to move further away */
              state_return->pointer_x = 0;
              return;
            }
        }
  }


  { /* adjust goto_area, and go there */
    coco_gnum board_w = cstate->board_w;
    coco_gnum go = self->goto_area;
    coco_gnum delta = COCO_RXNUM_TO_GNUM( bself->x, board_w ) - go;
    coco_facing face = (delta>0)?-1:1;
    if (abs(delta) < 100)
      {
        if (go != board_w>>1)
          go = board_w>>1;
        else if (self->nationality == COCO_GOOD_GUYS)
          go = 0;
        else go = board_w-1;
        self->goto_area = go;
      }
    
    /* charge like maniac towards goto_area */
    state_return->pointer_x = face * COCO_OFFICIAL_WINDOW_W;
    coco_comp_ensure_facing( face, state_return, self, cself );
  }
}
