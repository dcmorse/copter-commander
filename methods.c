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

#include <stdlib.h>      /* for rand() */
#include <math.h>        /* for COCO_COS */
#include <assert.h>
#include "coco.h"
#include "rasters.h"

static int
coco_find_closest_enemy_player( coco_sobject *sobject, coco_sstate *sstate );


/* returns info about how to shoot at target, and also does some physics
   as well. */
static void
coco_ai_bullet_coords( coco_sobject *gun, 
                       coco_sobject *target, coco_bool speed_compensate,
                       float jitter, /* a +/- of the muzzle velo, usually 0.4 */
                       coco_sobject_cl *bullet_cl,
                       coco_gnum *dx_return, coco_gnum *dy_return,
                       coco_gnum *x_return, coco_gnum *y_return,
                       coco_angle *angle_return )
{
  assert( target );
  {
    float speed_over_hypotenuse, dyrf;
    coco_u32 time_to_target;
    coco_gnum gx, gy, tx, ty, dx, dy;

    coco_sobject_weapon_hardpoint_coords( gun, &gx, &gy );
    coco_center_of_mass_obsolete( target, COCO_SOBJECT_CL( target ),
                         COCO_SOBJECT_RASTER_INDEX( target ), &tx, &ty );
    tx += COCO_SOBJECT_X( target );
    ty += COCO_SOBJECT_Y( target );
    dx = tx - gx;
    dy = ty - gy;
    if (speed_compensate)
      {
        time_to_target = sqrt( dx*dx + dy*dy )/bullet_cl->muzzle_velocity;
        /* make estimate of where he'll be.  Its inexcat because he can
           accellerate, and becuase the new time_to_target might not be
           equal to the old time_to_target */
        tx += COCO_SOBJECT_DX( target ) * time_to_target;
        ty += COCO_SOBJECT_DY( target ) * time_to_target;
        dx = tx - gx;
        dy = ty - gy;
      }
    {
      int r = (rand() & 0xFFFF) - 0x7FFF;
    /*speed_over_hypotenuse = (bullet_cl->muzzle_velocity * jitter) / sqrt( dx*dx + dy*dy );*/
      speed_over_hypotenuse = (bullet_cl->muzzle_velocity
                               * (1.0 + ((jitter * r) / 0xFFFF))
                               / sqrt( dx*dx + dy*dy ));
    }
    *dx_return = dx * speed_over_hypotenuse;
    *dy_return = dyrf = dy * speed_over_hypotenuse;
    *x_return = gx;
    *y_return = gy;
    *angle_return = coco_atan2( dy, dx );
  }
}

static void 
coco_slow_heal( coco_sobject *sobject, coco_sstate *sstate )
{
  if ((0 == (sstate->time & 63))
      && (COCO_SOBJECT_HEALTH( sobject ) < COCO_SOBJECT_HEALTH_MAX( sobject )))
    {
      COCO_SOBJECT_HEALTH( sobject )++;
    }
}

coco_bool
coco_auto_die_update_state( coco_sobject *sobject,
                            coco_sobject_cl *cl,
                            coco_sstate *sstate )
{
  return COCO_FALSE;
}

coco_bool
coco_cargo_copter_update_state( coco_sobject *sobject,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate )
{
  if ((!COCO_CARGO_COPTER_SUMMON_ENABLE( sobject ))
      && (sstate->time 
          == COCO_CARGO_COPTER_SUMMON_DISABLE_UNTIL( sobject )))
    COCO_CARGO_COPTER_SUMMON_ENABLE( sobject ) = COCO_TRUE;
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


coco_bool 
coco_ground_unit_update_state( coco_sobject *sobject, 
                               coco_sobject_cl *cl, 
                               coco_sstate *sstate )
{
  coco_gnum board_w = sstate->board_w;
  coco_gnum x = COCO_SOBJECT_X( sobject );
  if ((x < -COCO_GROUND_UNIT_WARP_OUT_X) 
      || (x > board_w+COCO_GROUND_UNIT_WARP_OUT_X))
    {
      COCO_SOBJECT_X( sobject ) = (x<0)
        ? (board_w+COCO_GROUND_UNIT_WARP_OUT_X)
        : -COCO_GROUND_UNIT_WARP_OUT_X;
      COCO_SOBJECT_HEALTH( sobject ) = COCO_SOBJECT_HEALTH_MAX( sobject );
    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}



coco_bool 
coco_every_sobject_update_state( coco_sobject *sobject, 
                                 coco_sobject_cl *cl, 
                                 coco_sstate *sstate )
{
  if (COCO_SOBJECT_EMITS_SMOKE_WHEN_DAMAGED( sobject ))
    coco_emit_smoke( sobject, COCO_SOBJECT_CL( sobject ), sstate );
  return COCO_TRUE;
}

coco_bool
coco_hut_update_state( coco_sobject *sobject, coco_sobject_cl *cl, coco_sstate *sstate )
{
  coco_slow_heal( sobject, sstate );
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


coco_bool
coco_explosion_update_state( coco_sobject *sobject,
                             coco_sobject_cl *cl,
                             coco_sstate *sstate )
{
  int ri = COCO_SOBJECT_RASTER_INDEX( sobject );
  int base, count, limit;
  if (COCO_BETWEENI( COCO_EXPLOSION_NAPALM_BASE, ri, 
                     (COCO_EXPLOSION_NAPALM_BASE 
                      + COCO_EXPLOSION_NAPALM_COUNT - 1) ))
    {
      base = COCO_EXPLOSION_NAPALM_BASE;
      count = COCO_EXPLOSION_NAPALM_COUNT;
    }
  else if (COCO_BETWEENI( COCO_EXPLOSION_BOMB_BASE, ri, 
                          (COCO_EXPLOSION_BOMB_BASE 
                           + COCO_EXPLOSION_BOMB_COUNT - 1) ))
    {
      base = COCO_EXPLOSION_BOMB_BASE;
      count = COCO_EXPLOSION_BOMB_COUNT;
    }
  else /* COCO_EXPLOSION_AIRBURST */
    {
      base = COCO_EXPLOSION_AIRBURST_BASE;
      count = COCO_EXPLOSION_AIRBURST_COUNT;
    }
  limit = base + count;

  if (!((base==COCO_EXPLOSION_NAPALM_BASE) && (sstate->time&3)))
    {
      ri++;
      if (ri >= limit)
        return COCO_FALSE; /* self destruct */
      else
        {
          coco_sobject_set_raster_index( sobject, ri );
          /* the above will center us on x and y.  If we're ground-based, we don't want y to move: */
          if ((base==COCO_EXPLOSION_BOMB_BASE)
              || (base==COCO_EXPLOSION_NAPALM_BASE))
          COCO_SOBJECT_Y( sobject ) = COCO_GROUND_Y;
          COCO_SOBJECT_DX( sobject ) = COCO_SOBJECT_DX( sobject )>>1;
        }
    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_balloon_line_update_state( coco_sobject *line,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate )
{
  int balloon_index = COCO_BALLOON_LINE_BALLOON_INDEX( line );
  if ((balloon_index<0)) 
    {
      int hut_index = COCO_BALLOON_LINE_HUT_INDEX( line );
      if (hut_index>=0)
        --COCO_SOBJECT_H( line );
      else 
        {
          COCO_SOBJECT_DY( line ) = -1;
          COCO_SOBJECT_DX( line ) = 0;
          if (0==COCO_SOBJECT_Y( line ))
            {
              COCO_SOBJECT_Y( line ) = 1;
              --COCO_SOBJECT_H( line );
            }
        }
    }
  if (COCO_SOBJECT_H( line )<=0)
    return COCO_FALSE;
  /* call-next-method */
  else return coco_sobject_update_state( line, cl->super, sstate );
}

coco_bool
coco_balloon_update_state( coco_sobject *balloon,
                           coco_sobject_cl *cl,
                           coco_sstate *sstate )
{
  int line_index, hut_index;
  coco_sobject *line, *hut;
  coco_u32 time = sstate->time;
  coco_u32 phase = COCO_BALLOON_BALLOON_TIME_PHASE( balloon );
  
  line_index = COCO_BALLOON_LINE_INDEX( balloon );
  line = (line_index >=0)
    ? COCO_SSTATE_OBJECT_REF( sstate, line_index )
    : NULL;
  hut_index = line ? COCO_BALLOON_LINE_HUT_INDEX( line ) : -1;
  hut = (hut_index >= 0)
    ? COCO_SSTATE_OBJECT_REF( sstate, hut_index )
    : NULL;
  /*
  if (hut||line)
    coco_cookie( "hut_index = %d\n"
                 "hut %d's line idx = %d\n"
                 "line %d's hut idx = %d, balloon_idx = %d\n"
                 "balloon %d's line idx = %d\n",
                 hut_index,
                 hut ? COCO_SOBJECT_INDEX( hut ) : -1,
                 hut ? COCO_HUT_LINE_INDEX( hut ) : -1,
                 line ? COCO_SOBJECT_INDEX( line ) : -1,
                 line ? COCO_BALLOON_LINE_HUT_INDEX( line ) : -1,
                 line ? COCO_BALLOON_LINE_BALLOON_INDEX( line ) : -1,
                 COCO_SOBJECT_INDEX( balloon ),
                 COCO_BALLOON_LINE_INDEX( balloon ) );
  */
  if ((COCO_U32_ADD_N_MODULAR( time, phase ) & 15)==0)
    {
      int num = rand(); /* jus call rand once, reuse different bits */
      switch (COCO_SOBJECT_DY( balloon )) {
      case 1:
        if (COCO_BALLOON_FACTORY_FRESH( balloon ))
          {
            if (COCO_SOBJECT_DY( balloon ) < 100)
              break;
            else COCO_BALLOON_FACTORY_FRESH( balloon ) = COCO_FALSE;
          }
        /* no break is intentional */
      case -1: 
        if (num&1) COCO_SOBJECT_DY( balloon ) = 0; break;
      case 0: 
        if (!(num&3))
          COCO_SOBJECT_DY( balloon ) = (num&4) ? -1 : 1;
        break;
      default:
        /* never get here */
        COCO_SOBJECT_DY( balloon ) >>= 1;
      }
      if (NULL == hut)
        switch (COCO_SOBJECT_DX( balloon )) {
        case 1:
        case -1: 
          if (num&8) COCO_SOBJECT_DX( balloon ) = 0; break;
        case 0: 
          if (!(num&12))
            COCO_SOBJECT_DX( balloon ) = (num&16) ? -1 : 1;
          /* DX used to be this, but why?
             Maybe it was supposed to be DY?
              = (COCO_SOBJECT_Y( balloon ) >= COCO_BALLOON_LINE_CEILING)
              ? -1
              : ((COCO_SOBJECT_Y( balloon ) <= COCO_BALLOON_FLOOR)
                 ? 1
                 : ((num&16)
                    ? -1
                    : 1));
          */
          break;
        default:
          /* never get here */
          COCO_SOBJECT_DX( balloon ) >>= 1;
        }
    }
  { /* clip the velocity so we don't wander off screen */
    coco_gnum ymax = COCO_SOBJECT_Y( balloon );
    coco_gnum ymin = ymax - (line ? COCO_SOBJECT_H( line ) : 0);
    if ((ymax>=COCO_BALLOON_LINE_CEILING) && hut)
      COCO_SOBJECT_DY( balloon ) = COCO_MIN( COCO_SOBJECT_DY( balloon ), 0 );
    else if (ymin<=COCO_BALLOON_FLOOR)
      COCO_SOBJECT_DY( balloon ) = COCO_MAX( COCO_SOBJECT_DY( balloon ), 0 );
  }
  /* update the line too. */
  if (hut)
    COCO_SOBJECT_H( line ) += COCO_SOBJECT_DY( balloon );
  else if (line)
    {
      COCO_SOBJECT_DY( line ) = COCO_SOBJECT_DY( balloon );
      COCO_SOBJECT_DX( line ) = COCO_SOBJECT_DX( balloon );
    }

  /* call-next-method */
  return coco_sobject_update_state( balloon, cl->super, sstate );
}

coco_bool
coco_fake_balloon_update_state( coco_sobject *balloon,
                           coco_sobject_cl *cl,
                           coco_sstate *sstate )
{
  if (COCO_SOBJECT_Y( balloon ) > COCO_BALLOON_FLOOR)
    /* DANGER WILL ROBINSON: CHANGING CLASS! */
    COCO_SOBJECT_CL( balloon ) = COCO_BALLOON_CLASS;
  /* call-next-method */
  return coco_sobject_update_state( balloon, cl->super, sstate );
}

coco_bool
coco_tank_update_state( coco_sobject *sobject,
                        coco_sobject_cl *cl,
                        coco_sstate *sstate )
{
  coco_sobject *lock_on = NULL;
  coco_angle lock_on_angle = 0;
  coco_gnum min_range = COCO_GNUM_MAX;
  coco_nationality nationality = COCO_SOBJECT_NATIONALITY( sobject );
  coco_facing facing = COCO_SOBJECT_FACING( sobject );
  coco_gnum front, dummy;
  coco_sobject_weapon_hardpoint_coords( sobject, &front, &dummy );
  { /* had to manually macroexpand COCO_DO_SSTATE_OBJECTS, because
       it wasn't working for some reason.  */
    coco_sobject *  target  = (  sstate )->sobjects;
    coco_sobject *coco__sobject_end__=   target  + (  sstate )->sobject_size;
    for (;   target  < coco__sobject_end__;   target ++) 
      if (((    target   )->every.in_use) )   
        {
          coco_gnum target_x, range;
          coco_center_of_mass_obsolete( target, COCO_SOBJECT_CL( target ),
                                        COCO_SOBJECT_RASTER_INDEX( target ),
                                        &target_x, &dummy );
          target_x += COCO_SOBJECT_X( target ); 
          /* now target-x is the center of mass in world coordinates */
          range = abs( target_x - front );

          if (COCO_ENEMY_P( nationality, COCO_SOBJECT_NATIONALITY( target ) )
              && (COCO_SOLID == COCO_SOBJECT_PHASE( target ))
              /* 4==fudge for some un-understood bug where the tank
                 doesn't advance enough to get in range of the infantry,
                 and for some reason the infantry isn't advancing, perhaps
                 its fixing a gun. */
              && (range < (COCO_TANK_GUN_RANGE - 4))
              && (range < min_range)
              && (coco_ground_trooper_target_p( target, 
                                                COCO_SOBJECT_CL( target ),
                                                sobject ))
              && ((facing>0) ? (target_x > front) : (target_x < front)))
            {
              coco_angle angle;
              coco_ai_bullet_coords( sobject, target, COCO_FALSE, 0.0,
                                     COCO_SOBJECT_BULLET_CLASS( sobject ),
                                     &dummy, &dummy,
                                     &dummy, &dummy, &angle );
              if ((facing>0)
                  ?((angle<=COCO_TANK_GUN_MAX_ELEVATION) ||(angle>=224))
                  :(((angle>=(124-COCO_TANK_GUN_MAX_ELEVATION))
                     && (angle<=160))))
                {
                  lock_on = target;
                  min_range = range;
                  lock_on_angle = angle;
                }
            }
        } 
  }
  if (lock_on)
    {
      if (0==(sstate->time & 7))
        coco_fire_bullet( sobject, COCO_SOBJECT_CL( sobject ), 
                          sstate, lock_on );
      COCO_SOBJECT_DX( sobject ) = 0;
      COCO_SOBJECT_ANGLE( sobject ) = lock_on_angle;
    }
  else
    {
      COCO_SOBJECT_DX( sobject ) = facing * COCO_SOBJECT_DX_MAX( sobject );
      /* depress gun if we're damaged :) */
      COCO_SOBJECT_ANGLE( sobject )
        = COCO_TANK_HEALTH_TO_ANGLE( COCO_SOBJECT_HEALTH( sobject ),  
                                     COCO_SOBJECT_HEALTH_MAX( sobject ),
                                     facing );
    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_firetruck_update_state( coco_sobject *sobject,
                             coco_sobject_cl *cl,
                             coco_sstate *sstate )
{
  unsigned int ri = COCO_SOBJECT_RASTER_INDEX( sobject );
  unsigned char smoke_countdown;
  if ((2==ri) || (0==ri))
    {
      coco_bool facing_forward_p = COCO_SOBJECT_FACING( sobject ) > 0;
      coco_nationality nat = COCO_SOBJECT_NATIONALITY( sobject );
      coco_gnum x = COCO_SOBJECT_X( sobject );
      coco_gnum xmin = facing_forward_p ? x : (x - COCO_FIRETRUCK_RANGE);
      coco_gnum xmax = facing_forward_p ? (x + COCO_FIRETRUCK_RANGE) : x;
      COCO_DO_SSTATE_OBJECTS( target, sstate, {
        coco_gnum ex = COCO_SOBJECT_X( target );
        if ((ex <= xmax) && ((ex + COCO_SOBJECT_W( target )) >= xmin)
            && COCO_ENEMY_P( nat, COCO_SOBJECT_NATIONALITY( target ) )
            && (coco_sobject_subtype_p( COCO_SOBJECT_CL( target ),
                                        COCO_COPTER_CLASS )
                ||coco_sobject_subtype_p( COCO_SOBJECT_CL( target ),
                                          COCO_MECH_CLASS )))
          coco_launch_missile( sobject, COCO_SOBJECT_CL( sobject ), 
                               sstate, target );
      });
    }
  else if ((1==ri)||(3==ri))
    {
      coco_gnum x = COCO_SOBJECT_X( sobject );
      if ((x<-COCO_FIRETRUCK_RELOAD_X)
          ||(x>sstate->board_w+COCO_FIRETRUCK_RELOAD_X))
        {
          coco_sobject_set_raster_index( sobject, ri-1 );
          COCO_SOBJECT_FACING( sobject ) = -1 * COCO_SOBJECT_FACING( sobject );
          COCO_SOBJECT_DX( sobject ) = -1 * COCO_SOBJECT_DX( sobject );
        }
    }
  smoke_countdown = COCO_FIRETRUCK_SMOKE_COUNTDOWN( sobject );
  if (smoke_countdown > 0)
    {
      coco_sobject *smoke = coco_sstate_allocate_sobject( sstate );
      COCO_FIRETRUCK_SMOKE_COUNTDOWN( sobject ) = smoke_countdown - 1;
      if (smoke)
        coco_init_smoke( smoke, COCO_SMOKE_CLASS, 
                         COCO_SOBJECT_X( sobject ) + (rand()&63) + 5,
                         COCO_GROUND_Y + (rand()&15), 
                         COCO_SMOKE_RASTER_COUNT-1 );
    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_headquarters_update_state( coco_sobject *sobject,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate )
{
  unsigned char smoke_countdown;
  {
    int pillbox_index = COCO_HEADQUARTERS_PILLBOX_INDEX( sobject );
    coco_sobject *pillbox = (pillbox_index>=0)
      ? COCO_SSTATE_OBJECT_REF( sstate, pillbox_index )
      : NULL;
    coco_nationality nat = COCO_SOBJECT_NATIONALITY( sobject );
    if (pillbox 
        && (COCO_BUNKER_DEFENDERS( pillbox ) > 0)
        && (COCO_SOBJECT_NATIONALITY( pillbox )==nat))
      {
        coco_gnum x = COCO_SOBJECT_X( sobject );
        coco_gnum xmin = x - COCO_HEADQUARTERS_MISSILE_RANGE;
        coco_gnum xmax = x + COCO_HEADQUARTERS_MISSILE_RANGE;
        COCO_DO_SSTATE_OBJECTS( target, sstate, {
          coco_gnum ex = COCO_SOBJECT_X( target );
          if ((ex <= xmax) && ((ex + COCO_SOBJECT_W( target )) >= xmin)
              && COCO_ENEMY_P( nat, COCO_SOBJECT_NATIONALITY( target ) )
              && (coco_sobject_subtype_p( COCO_SOBJECT_CL( target ),
                                         COCO_COPTER_CLASS )
                  || coco_sobject_subtype_p( COCO_SOBJECT_CL( target ),
                                             COCO_MECH_CLASS )))
            coco_launch_missile( sobject, COCO_SOBJECT_CL( sobject ), 
                                 sstate, target );
        });
      }
  }
  smoke_countdown = COCO_HEADQUARTERS_SMOKE_COUNTDOWN( sobject );
  if (smoke_countdown > 0)
    {
      coco_sobject *smoke = coco_sstate_allocate_sobject( sstate );
      COCO_HEADQUARTERS_SMOKE_COUNTDOWN( sobject ) = smoke_countdown - 1;
      if (smoke)
        coco_init_smoke( smoke, COCO_SMOKE_CLASS, 
                         COCO_SOBJECT_X( sobject ) + (rand()&63)
                         + ((COCO_SOBJECT_W( sobject )-63)>>1),
                         COCO_GROUND_Y + (rand()&31), 
                         COCO_SMOKE_RASTER_COUNT-1 );
    }
  /* headquarters moves at 2x frames per second. */
  if (0==(sstate->time % (COCO_IDEAL_FRAMES_PER_SECOND>>1)))
    COCO_SOBJECT_DX( sobject ) = COCO_SOBJECT_FACING( sobject );
  else COCO_SOBJECT_DX( sobject ) = 0;
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_missile_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate )
{
  int target_index = COCO_MISSILE_TARGET_INDEX( sobject );
  unsigned fuel = COCO_MISSILE_FUEL( sobject );
  if ((target_index<0) || (fuel<=0))
    coco_free_fall_adjust( sobject, COCO_GRAVITY_ACCELL, -1 );
  else
    {
      int i;
      coco_gnum mx, my, tx, ty, dx, dy, vx, vy;
      float mvx, mvy, cos1, sin1;
      coco_angle goal_angle, new_angle;
      int current_angle = COCO_SOBJECT_ANGLE( sobject );
      coco_sobject *target = COCO_SSTATE_OBJECT_REF( sstate, target_index );
      coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ),
                           COCO_SOBJECT_RASTER_INDEX( sobject ), &mx, &my);
      mx += COCO_SOBJECT_X( sobject ) + COCO_SOBJECT_DX( sobject );
      my += COCO_SOBJECT_Y( sobject ) + COCO_SOBJECT_DY( sobject );
      coco_center_of_mass_obsolete( target, COCO_SOBJECT_CL( target ),
                           COCO_SOBJECT_RASTER_INDEX( target ), &tx, &ty);
      tx += COCO_SOBJECT_X( target ) + COCO_SOBJECT_DX_PUBLIC( target );
      ty += COCO_SOBJECT_Y( target ) + COCO_SOBJECT_DY_PUBLIC( target );
      dx = tx - mx;
      dy = ty - my;
      goal_angle = coco_atan2( dy, dx );

      /*
      coco_cookie( "current_angle %3d      goal_angle %3d      (%3d, %3d)\n",
                   current_angle, goal_angle, dx, dy );
      */


      sin1 = COCO_SIN( current_angle );
      cos1 = COCO_COS( current_angle );

      /* turn to face target (I know its ugly) */
      if (-sin1*dx + cos1*dy > 0)
        for (i=0, new_angle = current_angle;
             i <= COCO_MISSILE_MAX_ANGULAR_SPEED;
             i++, new_angle++)
          {
            if ((new_angle & 255) == goal_angle) break;
          }
      else
        for (i=0, new_angle = current_angle;
             i <= COCO_MISSILE_MAX_ANGULAR_SPEED;
             i++, new_angle--)
          {
            if ((new_angle & 255) == goal_angle) break;
          }
      new_angle &= 255;

      
      mvx = COCO_MISSILE_ACCELL * cos1;
      mvy = COCO_MISSILE_ACCELL * sin1;
      vx = COCO_SOBJECT_DX( sobject );
      vy = COCO_SOBJECT_DY( sobject );
      if (mvx*dx + mvy*dy > 0)
        { /* burn thruster */
          COCO_MISSILE_FUEL( sobject ) = --fuel;
          vx += mvx;
          vy += mvy;
          if (0>=fuel)
            COCO_SOBJECT_NATIONALITY( sobject ) = COCO_NEUTRAL;
        }
      /* friction */
      vx = vx - (vx>>4);
      vy = vy - (vy>>4);
      COCO_SOBJECT_DX( sobject ) = vx;
      COCO_SOBJECT_DY( sobject ) = vy;
      COCO_SOBJECT_ANGLE( sobject ) = new_angle;
    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_rocket_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate )
{
  coco_facing facing = COCO_SOBJECT_FACING( sobject );
  /* the missile is fired depressed 10 degrees.  At that angle the x
     componet is about 1.0, and the y component about .125. */
  coco_gnum dx, dy;
  coco_gnum dxmax = COCO_SOBJECT_DX_MAX( sobject );
  dx = COCO_SOBJECT_DX( sobject ) 
    + (facing>0 ? COCO_MISSILE_ACCELL : -COCO_MISSILE_ACCELL);

  dx = COCO_MAX( COCO_MIN( dx, dxmax ), -dxmax );

  dy = - abs( dx ) >> 2;

  COCO_SOBJECT_DX( sobject ) = dx;
  COCO_SOBJECT_DY( sobject ) = dy;
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_shrapnel_update_state( coco_sobject *sobject,
                            coco_sobject_cl *cl,
                            coco_sstate *sstate )
{
  coco_free_fall_adjust( sobject, COCO_SOBJECT_Y_FORCE( sobject ), -1 );
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

void
coco_hut_stormin( coco_sobject *hut, coco_sstate *sstate,
                  coco_nationality stormin_nat )
{
  int line_idx, loon_idx, refund;
  coco_sobject *line, *loon;
  coco_nationality hut_nat, final_nat;

  refund = COCO_SOBJECT_REFUND( hut );
  if (refund > 0)
    coco_add_funds( coco_find_closest_enemy_player( hut, sstate ),
                    COCO_FALSE, refund );

  hut_nat = COCO_SOBJECT_NATIONALITY( hut );
  final_nat
    = (COCO_ENEMY_P( hut_nat, stormin_nat )) ? COCO_NEUTRAL : stormin_nat;
  COCO_SOBJECT_NATIONALITY( hut ) = final_nat;
  coco_sobject_set_raster_index( hut, (unsigned char) final_nat );
  COCO_HUT_POINTS_TILL_REINCARNATION( hut ) 
    = COCO_HUT_BALLOON_REINCARNATION_FRAMES;
  line_idx = COCO_HUT_LINE_INDEX( hut );
  if (line_idx<0) return;
  line = COCO_SSTATE_OBJECT_REF( sstate, line_idx );
  COCO_SOBJECT_NATIONALITY( line ) = final_nat;
  loon_idx = COCO_BALLOON_LINE_BALLOON_INDEX( line );
  if (loon_idx<0) return;
  loon = COCO_SSTATE_OBJECT_REF( sstate, loon_idx );
  COCO_SOBJECT_NATIONALITY( loon ) = final_nat;
  coco_sobject_set_raster_index( loon, (unsigned char) final_nat );
}

/* returns true if the infant was munched up. */
coco_bool
coco_feisty_infantry_enter_hut( coco_sobject *infantry, coco_sobject_cl *cla,
                                coco_sobject *hut, coco_sstate *sstate,
                                coco_bool *stand_still_p )
{
  *stand_still_p = COCO_FALSE;
  if (COCO_SOBJECT_NATIONALITY( infantry ) != COCO_SOBJECT_NATIONALITY( hut ))
    {
      coco_hut_stormin( hut, sstate, COCO_SOBJECT_NATIONALITY( infantry ) );
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}

coco_bool
coco_engineer_enter_hut( coco_sobject *engie, coco_sobject_cl *cla,
                         coco_sobject *hut, coco_sstate *sstate,
                         coco_bool *stand_still_p )
{
  if ((COCO_SOBJECT_NATIONALITY( engie ) == COCO_SOBJECT_NATIONALITY( hut ))
      && (COCO_HUT_LINE_INDEX( hut ) < 0))
    {
      if ((--COCO_HUT_POINTS_TILL_REINCARNATION( hut ))<=0)
        coco_hut_deploy_balloon( hut, sstate );
      *stand_still_p = COCO_TRUE;
    }
  else *stand_still_p = COCO_FALSE;
  return COCO_FALSE;
}


coco_bool
coco_infantry_update_state( coco_sobject *sobject,
                            coco_sobject_cl *cl,
                            coco_sstate *sstate )
{
  coco_bool hut_stop_and_scratch_p = COCO_FALSE;
  /* using signed int here to shut up gcc about comparing with >= 0: */
  int ri = COCO_SOBJECT_RASTER_INDEX( sobject );
  if (((COCO_INFANTRY_WALK_EAST_BASE <= ri)
       && (ri < (COCO_INFANTRY_WALK_EAST_BASE 
                 + COCO_INFANTRY_WALK_COUNT)))
      || ((COCO_INFANTRY_WALK_WEST_BASE <= ri) /* yes, its always true */
          && (ri < (COCO_INFANTRY_WALK_WEST_BASE
                    + COCO_INFANTRY_WALK_COUNT))))
    {
      ri++;
      if (!((ri == (COCO_INFANTRY_WALK_EAST_BASE + COCO_INFANTRY_WALK_COUNT))
            || (ri == (COCO_INFANTRY_WALK_WEST_BASE + COCO_INFANTRY_WALK_COUNT))))
        coco_sobject_set_raster_index( sobject, ri );
      else
        {
          coco_bool infantry_overlap_p = COCO_FALSE;
          coco_nationality nationality = COCO_SOBJECT_NATIONALITY( sobject );
          coco_gnum x, y;
          coco_sobject *lock_on_target = NULL;
          coco_gnum lock_on_range_squared = COCO_GNUM_MAX;

          coco_sobject_set_raster_index( sobject, 
                                         ri - COCO_INFANTRY_WALK_COUNT );

          coco_sobject_weapon_hardpoint_coords( sobject, &x, &y );
          { /* Couldn't use COCO_DO_SSTATE_OBJECTS because of bugs. */
            coco_sobject *targ  = (sstate)->sobjects;
            coco_sobject *coco__sobject_end__=targ  + (sstate)->sobject_size;
            for (;   targ  < coco__sobject_end__;   targ ++) 
              if (((targ)->every.in_use) )   
                {
                  if ((COCO_ENEMY_P( COCO_SOBJECT_NATIONALITY( targ ),
                                     nationality ))
                      && (COCO_SOBJECT_PHASE( targ ) == COCO_SOLID)
                      && (abs( COCO_SOBJECT_Y( targ ) - y )
                          < COCO_INFANTRY_GUN_RANGE)
                      && (coco_ground_trooper_target_p
                          ( targ, COCO_SOBJECT_CL( targ ), sobject )))
                    {
                      coco_gnum tx, ty, dx, dy, range_sqr;
                      coco_center_of_mass_obsolete( targ, COCO_SOBJECT_CL( targ ),
                                           COCO_SOBJECT_RASTER_INDEX( targ ),
                                           &tx, &ty );
                      tx += COCO_SOBJECT_X( targ );
                      ty += COCO_SOBJECT_Y( targ );
                      dx = x - tx;
                      dy = y - ty;
                      range_sqr = dx*dx + dy*dy;
                      if ((range_sqr - 2 < /* 2 = fudge to make it work */
                           (COCO_INFANTRY_GUN_RANGE *COCO_INFANTRY_GUN_RANGE))
                          &&(range_sqr < lock_on_range_squared))
                        {
                          lock_on_range_squared = range_sqr;
                          lock_on_target = targ;
                        }
                    }
                  else if ((nationality == COCO_SOBJECT_NATIONALITY( targ ))
                           && (COCO_SOBJECT_INDEX( targ ) 
                               < COCO_SOBJECT_INDEX( sobject ))
                           && (COCO_SOBJECT_Y( targ ) == COCO_GROUND_Y)
                           && (x >= COCO_SOBJECT_X( targ ) - 12)
                           && (x <= COCO_SOBJECT_X( targ ) + 12)
                           && (COCO_SOBJECT_FACING( sobject )
                               == COCO_SOBJECT_FACING( targ ))
                           && (COCO_SOBJECT_DX( sobject ) != 0) /* bunch for fight */
                           && coco_sobject_subtype_p( COCO_SOBJECT_CL( targ ), 
                                                      COCO_INFANTRY_CLASS ))
                    infantry_overlap_p = COCO_TRUE;
                  else if ((COCO_SOBJECT_X_CENTER( targ )
                            >= x-(COCO_INFANTRY_WALK_COUNT>>1))
                           && (COCO_SOBJECT_X_CENTER( targ )
                               <= x+(COCO_INFANTRY_WALK_COUNT>>1))
                           && (coco_sobject_subtype_p( COCO_SOBJECT_CL( targ ),
                                                       COCO_HUT_CLASS )))
                    {
                      coco_bool stop_p;
                      if (coco_infantry_enter_hut( sobject, 
                                                   COCO_SOBJECT_CL( sobject ),
                                                   targ, sstate, &stop_p ))
                        return COCO_FALSE;
                      hut_stop_and_scratch_p |= stop_p;
                    }
                } 
          } ;

          if (lock_on_target)
            coco_fire_bullet( sobject, COCO_SOBJECT_CL( sobject ),
                              sstate, lock_on_target );
          COCO_SOBJECT_DX( sobject )
            = ((lock_on_target||infantry_overlap_p||hut_stop_and_scratch_p)
               ? 0
               : ((COCO_SOBJECT_FACING( sobject ) > 0) ? 1 : -1));
        }
    }
  else if ((COCO_INFANTRY_FREE_FALL_BASE <= ri)
           && (ri < (COCO_INFANTRY_FREE_FALL_COUNT
                     + COCO_INFANTRY_FREE_FALL_BASE)))
    {
      coco_free_fall_adjust( sobject, COCO_SOBJECT_Y_FORCE( sobject ), -1 );
      ri++;
      if (ri == (COCO_INFANTRY_FREE_FALL_BASE + COCO_INFANTRY_FREE_FALL_COUNT))
        coco_sobject_set_raster_index( sobject, COCO_INFANTRY_FREE_FALL_BASE );
      else coco_sobject_set_raster_index( sobject, ri );
      if ((COCO_INFANTRY_PARACHUTES( sobject )>0)
          && (COCO_SOBJECT_Y( sobject )
              <=COCO_INFANTRY_PARACHUTE_HEIGHT( sobject )))
        {
          COCO_INFANTRY_PARACHUTES( sobject )--;
          if (0!=(rand()&3)) /* chute failure */
            {
              coco_gnum old_y = COCO_SOBJECT_Y( sobject );
              coco_sobject_set_raster_index
                ( sobject, 
                  (COCO_GOOD_GUYS == COCO_SOBJECT_NATIONALITY( sobject ))
                  ? COCO_INFANTRY_GOOD_GUY_CHUTE 
                  : COCO_INFANTRY_BAD_GUY_CHUTE );
              COCO_SOBJECT_Y( sobject ) = old_y;
            }
        }
    }
  else if ((ri == COCO_INFANTRY_GOOD_GUY_CHUTE)
           || (ri == COCO_INFANTRY_BAD_GUY_CHUTE))
    {
      coco_gnum ground_dist = COCO_GROUND_Y - COCO_SOBJECT_Y( sobject );
      if (ground_dist == 0)
        {
          coco_gnum facing;
          COCO_SOBJECT_FACING( sobject ) = facing
            = (COCO_SOBJECT_NATIONALITY( sobject ) == COCO_GOOD_GUYS) ? -1 : 1;
          coco_sobject_set_raster_index( sobject,
                                         ((facing>0)
                                          ? COCO_INFANTRY_WALK_EAST_BASE
                                          : COCO_INFANTRY_WALK_WEST_BASE )
                                         + (rand()%COCO_INFANTRY_WALK_COUNT));
          COCO_SOBJECT_Y( sobject ) = COCO_GROUND_Y;
          COCO_SOBJECT_DY( sobject ) = COCO_SOBJECT_DX( sobject ) = 0;
        }
      else 
        {
          COCO_SOBJECT_DY( sobject ) = COCO_MAX( -3, ground_dist);
          COCO_SOBJECT_DX( sobject ) = COCO_SOBJECT_DX( sobject )>>1;
        }
    }
  else coco_warn_printf( "Mysterious raster index %d with infantry.\n", ri );

  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_engineer_update_state( coco_sobject *sobject,
                            coco_sobject_cl *cl,
                            coco_sstate *sstate )
{
  coco_gnum x = COCO_SOBJECT_X( sobject );
  COCO_DO_SSTATE_OBJECTS( target, sstate, {
    if ((COCO_SOBJECT_X( target ) == x) &&
        (coco_sobject_subtype_p( COCO_SOBJECT_CL( target ), 
                                 COCO_GUN_RUBBLE_CLASS )))
      {
        coco_bool return_result;
        COCO_GUN_RUBBLE_POINTS_TILL_REINCARNATION( target )--;
        COCO_GUN_RUBBLE_REINCARNATION_NATIONALITY( target ) 
          = COCO_SOBJECT_NATIONALITY( sobject );
        /* call-next-method */
        return_result
          = coco_sobject_update_state( sobject, cl->super, sstate );
        if (return_result)
          COCO_SOBJECT_DX( sobject ) = 0;
        return return_result;
      }
  });
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool
coco_gun_rubble_update_state( coco_sobject *sobject,
                              coco_sobject_cl *cl,
                              coco_sstate *sstate )
{
  if (COCO_GUN_RUBBLE_POINTS_TILL_REINCARNATION( sobject )<=0)
    {
      /* overwrite our sobject with the sobject of the gun, thus avoiding
         a possibly failed allocation.  */
      coco_sobject_cl *klas = coco_sobject_classes 
        + COCO_GUN_RUBBLE_REINCARNATION_CLASS_INDEX( sobject );
      coco_init_gun( sobject, klas, COCO_SOBJECT_X( sobject ),
                     COCO_GUN_RUBBLE_REINCARNATION_NATIONALITY( sobject ) );
      return COCO_TRUE;
    }
  else /* call-next-method */
    return coco_sobject_update_state( sobject, cl->super, sstate );
}


coco_bool
coco_metal_shrapnel_update_state( coco_sobject *sobject,
                                  coco_sobject_cl *cl,
                                  coco_sstate *sstate )
{
  if (0==(rand()&7))
    coco_randomize_metal_shrapnel( sobject );
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}



coco_bool 
coco_inviso_camera_update_state( coco_sobject *sobject, 
                                 coco_sobject_cl *cl, 
                                 coco_sstate *sstate )
{
  const static coco_u32 snap_frames
    = COCO_RESPAWN_BASE_SNAP_SECONDS * COCO_IDEAL_FRAMES_PER_SECOND;
  signed char camera_idx = COCO_SOBJECT_CAMERA_INDEX( sobject );
  if ((-1 != camera_idx)
      && sstate->respawn[camera_idx]
      && (sstate->respawn_times[camera_idx]
          == COCO_U32_ADD_N_MODULAR( snap_frames, sstate->time )))
    {
      coco_gnum x, y;
      coco_copter_start_coords( sstate, COCO_SOBJECT_NATIONALITY( sobject ),
                                camera_idx, &x, &y );
      COCO_SOBJECT_X( sobject ) = x;
      coco_make_snd( (coco_players[camera_idx].copters > 0)
                     && coco_sobject_subtype_p( sstate->favorite_copter_class
                                                [camera_idx],
                                                COCO_MECH_CLASS )
                     ? COCO_SOUND_MECH_TAKE_OFF
                     : COCO_SOUND_COPTER_TAKE_OFF,
                     sstate, x, y );
    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


coco_bool 
coco_smoke_update_state( coco_sobject *sobject, 
                         coco_sobject_cl *cl, 
                         coco_sstate *sstate )
{
  int ri = COCO_SOBJECT_RASTER_INDEX( sobject ) - 1;
  if (ri < 0)
    return COCO_FALSE;
  else
    {
      coco_sobject_set_raster_index( sobject, ri );
      /* call-next-method */
      return coco_sobject_update_state( sobject, cl->super, sstate );
    }
}


coco_bool 
coco_cloud_update_state( coco_sobject *sobject, 
                         coco_sobject_cl *cl, 
                         coco_sstate *sstate )
{
  coco_u32 time = sstate->time;
  coco_u32 phase = COCO_CLOUD_CLOUD_TIME_PHASE( sobject );
  
  if ((COCO_U32_ADD_N_MODULAR( time, phase ) & 63)==0)
    {
      int num = rand(); /* jus call rand once, reuse different bits */
      switch (COCO_SOBJECT_DY( sobject )) {
      case 1:
      case -1: 
        if (num&3) COCO_SOBJECT_DY( sobject ) = 0; break;
      case 0: 
        if (!(num&7))
          COCO_SOBJECT_DY( sobject ) = (num&8) ? -1 : 1;
        break;
      default:
        /* never get here */
        COCO_SOBJECT_DY( sobject ) >>= 1;
      }

        
        
      switch (COCO_SOBJECT_DX( sobject )) {
      case 1:
      case -1: 
        if (num&16) COCO_SOBJECT_DX( sobject ) = 0; break;
      case 0: 
        if (!(num&56))
          COCO_SOBJECT_DX( sobject ) = (num&64) ? -1 : 1;
        break;
      default:
        /* never get here */
        COCO_SOBJECT_DX( sobject ) >>= 1;
      }
    }
  /* check to make sure we don't drift too low.  Don't have to worry
     about top, left, or right, because clip_to_board is on.  */
  if ((COCO_SOBJECT_Y( sobject ) <= COCO_CLOUD_FLOOR)
      && (COCO_SOBJECT_DY( sobject ) < 0))
    COCO_SOBJECT_DY( sobject ) = 0;
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


/* 
Moved to server.c because it depends on static server data.

coco_bool 
coco_copter_update_state( coco_sobject *sobject, 
                          coco_sobject_cl *cl, 
                          coco_sstate *sstate )
*/

coco_bool 
coco_bullet_update_state( coco_sobject *sobject, 
                          coco_sobject_cl *cl, 
                          coco_sstate *sstate )
{
  if (COCO_BULLET_EXPIRATION_DATE( sobject ) == sstate->time)
    return COCO_FALSE;
  /* call-next-method */
  else if (!coco_sobject_update_state( sobject, cl->super, sstate ))
    return COCO_FALSE;
  else
    {
      coco_gnum dx = COCO_SOBJECT_DX( sobject );
      coco_gnum dy = COCO_SOBJECT_DY( sobject );
      COCO_BULLET_DISTANCE_LEFT( sobject ) -= sqrt( dx*dx + dy*dy );
      if (COCO_BULLET_DISTANCE_LEFT( sobject ) > 0)
        { /* drag */
          coco_gnum mv = COCO_SOBJECT_MUZZLE_VELOCITY( sobject );
          if (dx > mv)       dx--;
          else if (dx < -mv) dx++;
          if (dy > mv)       dy--;
          else if (dy < -mv) dy++;
          COCO_SOBJECT_DX( sobject ) = dx;
          COCO_SOBJECT_DY( sobject ) = dy;
          return COCO_TRUE;
        }
      else return COCO_FALSE;
    }
}

coco_bool 
coco_bomb_update_state( coco_sobject *sobject, 
                        coco_sobject_cl *cl, 
                        coco_sstate *sstate )
{
  unsigned char ri = COCO_SOBJECT_RASTER_INDEX( sobject );
  unsigned char rinew=ri;
  coco_gnum dy = COCO_SOBJECT_DY( sobject );
  switch (ri) {
  case 0: if (dy<-5) rinew=1; break;
  case 4: if (dy<-5) rinew=3; break;
  case 1: 
  case 3: if (dy<-14) rinew=2; break;
  default: break;
  }
  if (rinew!=ri)
    coco_sobject_set_raster_index( sobject, rinew );
  coco_free_fall_adjust( sobject, COCO_SOBJECT_Y_FORCE( sobject ), 5 );
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

coco_bool 
coco_gun_update_state( coco_sobject *sobject, 
                       coco_sobject_cl *cl, 
                       coco_sstate *sstate )
{
  coco_nationality my_nat = COCO_SOBJECT_NATIONALITY( sobject );
  coco_gnum my_x, my_y;
  coco_gnum best_target_range_sqr = COCO_GNUM_MAX;
  coco_sobject *best_target = NULL;

  coco_sobject_weapon_hardpoint_coords( sobject, &my_x, &my_y );

  /* only fire 1 frame in 4.  Fire if (time + object-number)%4 == 0.  The
     object number is done to keep two guns from likely firing at exactly
     the same time, meaning 2 sounds at exactly the same time, leading to
     a wierd sound, and jitter. */
  if (0 == ((sstate->time + COCO_SOBJECT_INDEX( sobject )) & 3))
    {
      int i;
      int cl_count = COCO_SOBJECT_TARGETABLE_CLASSES_COUNT( sobject );
      coco_sobject_cl **classes = COCO_SOBJECT_TARGETABLE_CLASSES( sobject );
      { /* couldn't use COCO_DO_SSTATE_OBJECTS because of bugs */ 
        coco_sobject *target = sstate->sobjects;
        coco_sobject *sobject_end = target + sstate->sobject_size;
        for (; target < sobject_end; target++)
          if ((COCO_SOBJECT_IN_USE( target )) )
            {
              if (COCO_ENEMY_P( my_nat, COCO_SOBJECT_NATIONALITY( target )))
                {
                  coco_gnum this_range_sqr;
                  coco_gnum dx, dy;
                  dx = my_x - COCO_SOBJECT_X_CENTER( target );
                  dy = my_y - COCO_SOBJECT_Y_CENTER( target );
                  this_range_sqr = dx*dx + dy*dy;
                  if ((this_range_sqr < best_target_range_sqr)
                      && (this_range_sqr < (COCO_BULLET_RANGE * COCO_BULLET_RANGE)))
                    for (i=0; i < cl_count; i++)
                      if (coco_sobject_subtype_p( COCO_SOBJECT_CL( target ), 
                                                  classes[i] ))
                        { 
                          best_target_range_sqr = this_range_sqr;
                          best_target = target;
                          break;
                        }
                }
            }
      }
      if (best_target)
        coco_fire_bullet( sobject, COCO_SOBJECT_CL( sobject ), sstate, 
                          best_target );
    }
  coco_slow_heal( sobject, sstate );
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


void
coco_every_sobject_kaboom( coco_sobject *sobject,
                           coco_sobject_cl *cl,
                           coco_sstate *sstate )
{
  int refund = COCO_SOBJECT_REFUND( sobject );
  if (refund > 0)
    coco_add_funds( coco_find_closest_enemy_player( sobject, sstate ), 
                    COCO_FALSE, refund );
  coco_make_shrapnel( COCO_SOBJECT_SHRAPNEL_COUNT( sobject ),
                      sstate, COCO_SOBJECT_X_CENTER( sobject ),
                      COCO_SOBJECT_Y_CENTER( sobject ),
                      COCO_SOBJECT_DX( sobject ),
                      COCO_SOBJECT_DY( sobject ) );
  coco_sobject_delete( sobject, COCO_SOBJECT_CL( sobject ), sstate );
}

void
coco_airbursting_kaboom( coco_sobject *sobject,
                         coco_sobject_cl *cl,
                         coco_sstate *sstate )
{
  coco_sobject *explosion = coco_sstate_allocate_sobject( sstate );

  if (explosion)
    {
      coco_gnum x, y;
      unsigned char ri = COCO_EXPLOSION_AIRBURST_BASE;
      struct coco_raster *raster 
        = COCO_EXPLOSION_CLASS->rasters[ri];
      coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ),
                           COCO_SOBJECT_RASTER_INDEX( sobject ),
                           &x, &y );
      x += COCO_SOBJECT_X( sobject );
      y += COCO_SOBJECT_Y( sobject );
      coco_init_explosion( explosion, COCO_EXPLOSION_CLASS, 0, 
                           x - ((coco_gnum) raster->w>>1),
                           y - ((coco_gnum) raster->h>>1), 0, 0, ri );
      coco_make_snd( COCO_SOBJECT_AIRBURST_SOUND_TYPE( sobject ), 
                     sstate, x, y );
    }
  /* call-next-method */
  coco_kaboom( sobject, cl->super, sstate );
}

void
coco_copter_kaboom( coco_sobject *sobject,
                    coco_sobject_cl *cl,
                    coco_sstate *sstate )
{
  coco_sobject *trooper;
  COCO_COPTER_PARATROOPERS( sobject ) = 1;
  trooper 
    = coco_drop_paratrooper( sobject, COCO_SOBJECT_CL( sobject ), sstate );

  /* call-next-method [SORT OF]: also want airburst, so just call that
     method which will call-next-method itself */
  coco_airbursting_kaboom( sobject, cl, sstate );
}

void
coco_bullet_kaboom( coco_sobject *sobject,
                    coco_sobject_cl *cl,
                    coco_sstate *sstate )
{
  int i;
  for (i=COCO_SOBJECT_SPARK_COUNT( sobject ); i>0; i--)
    {
      int rando;
      coco_gnum x, y;
      signed char dx, dy;
      coco_sobject *spark = coco_sstate_allocate_sobject( sstate );
      if (NULL == spark) return;
      rando = rand();
      x = COCO_SOBJECT_X( sobject );
      y = COCO_SOBJECT_Y( sobject );
      dx = (signed char)
        (COCO_SOBJECT_DX( sobject )
         + ((signed char) (rando & 30) - 15));
      dy = (signed char)
        (COCO_SOBJECT_DY( sobject ) 
         + ((signed char) ((rando>>8) & 30) - 15));
      if (y < COCO_GROUND_Y)
        {
          dy = abs( dy );
          y = COCO_GROUND_Y;
        }
      coco_init_spark( spark, COCO_SPARK_CLASS, x, y, dx, dy );
      {
        /* only make sound sometimes to reduce load on system */
        if (rando&64)
          {
            coco_make_snd( (rando&128)?COCO_SOUND_SPARK1:COCO_SOUND_SPARK2,
                           sstate, x, y );
            coco_cookie( "playing spark %d\n", 
                         (rando&128)?COCO_SOUND_SPARK1:COCO_SOUND_SPARK2 );
          }
      }
    }
  /* call-next-method */
  coco_kaboom( sobject, cl->super, sstate );
}

void
coco_gun_kaboom( coco_sobject *sobject,
                 coco_sobject_cl *cl,
                 coco_sstate *sstate )
{
  /* Should the gun-rubble be on the kaboom, or the delete?  Feels like
     kaboom to me right now. */
  coco_sobject *rubble = coco_sstate_allocate_sobject( sstate );
  if (rubble)
    {
      int rci = COCO_SOBJECT_CL_INDEX( sobject );
      coco_init_gun_rubble( rubble, COCO_GUN_RUBBLE_CLASS,
                            COCO_SOBJECT_X( sobject ), rci );
    }
  else coco_warn_printf( "Allocation of gun_rubble at x=%d "
                         "t=%dfailed.\n",
                         COCO_SOBJECT_X( sobject ), sstate->time );
  /* call-next-method */
  coco_kaboom( sobject, cl->super, sstate );
}

void
coco_bomb_kaboom( coco_sobject *sobject,
                  coco_sobject_cl *cl,
                  coco_sstate *sstate )
{
  if (COCO_SOBJECT_Y( sobject ) < COCO_BALLOON_FLOOR)
    {
      coco_sobject *explosion = coco_sstate_allocate_sobject( sstate );
      struct coco_raster **rasters = COCO_EXPLOSION_CLASS->rasters;
      if (explosion)
        {
          coco_gnum x = COCO_SOBJECT_X( sobject ) 
            + ((COCO_SOBJECT_W( sobject )
                - ((coco_gnum) rasters[COCO_EXPLOSION_BOMB_BASE]->w))
               >>1);
          coco_init_explosion( explosion, COCO_EXPLOSION_CLASS, 0, x,
                               COCO_GROUND_Y, 0, 0, COCO_EXPLOSION_BOMB_BASE );
          coco_make_snd( COCO_SOUND_BOMB_HIT, sstate, x, COCO_GROUND_Y );
        }
    }

  /* call-next-method */
  coco_kaboom( sobject, cl->super, sstate );
}

void
coco_rocket_kaboom( coco_sobject *sobject,
                    coco_sobject_cl *cl,
                    coco_sstate *sstate )
{
  if (COCO_SOBJECT_Y( sobject ) < COCO_BALLOON_FLOOR)
    {
      
      coco_facing facing = COCO_SOBJECT_FACING( sobject );
      struct coco_raster **rasters = COCO_EXPLOSION_CLASS->rasters;
      coco_gnum raster_w = rasters[COCO_EXPLOSION_NAPALM_BASE]->w;
      coco_gnum x = COCO_SOBJECT_X( sobject ) 
        + (COCO_SOBJECT_W( sobject )>>1)
        - ((facing>0) ? 0 : raster_w);
      int i;
      for (i=0; i<COCO_EXPLOSION_NAPALM_COUNT; i++)
        {
          coco_sobject *explosion = coco_sstate_allocate_sobject( sstate );
          if (explosion)
            coco_init_explosion( explosion, COCO_EXPLOSION_CLASS, 0,
                                 x, COCO_GROUND_Y,
                                 (COCO_SOBJECT_DX_PUBLIC( sobject )>>1)
                                 *(COCO_EXPLOSION_NAPALM_COUNT-i), 
                                 0, 
                                 COCO_EXPLOSION_NAPALM_BASE + i );
        }
      coco_make_snd( COCO_SOUND_ROCKET_HIT, sstate, x, COCO_GROUND_Y );
    }
  /* call-next-method */
  coco_kaboom( sobject, cl->super, sstate );
}


void
coco_infantry_kaboom( coco_sobject *sobject,
                      coco_sobject_cl *cl,
                      coco_sstate *sstate )
{
  /* 3 == number of COCO_SOUND_SCREAM* */
  coco_make_snd( COCO_SOUND_SCREAM1 + (rand()%3), sstate, 
                 COCO_SOBJECT_X( sobject ), COCO_SOBJECT_Y( sobject ) );
  /* call-next-method */
  coco_kaboom( sobject, cl->super, sstate );
}


/* -1 on fail */
static int
coco_find_closest_enemy_player( coco_sobject *sobject, coco_sstate *sstate )
{
  coco_gnum sx = COCO_SOBJECT_X( sobject );
  coco_gnum min_distance = COCO_GNUM_MAX;
  int player_index = -1;
  coco_nationality nat = COCO_SOBJECT_NATIONALITY( sobject );
  COCO_DO_SSTATE_OBJECTS( cam, sstate,
  {
    if ((abs( COCO_SOBJECT_X( cam ) - sx ) < min_distance)
        && (COCO_SOBJECT_CAMERA_INDEX( cam ) >= 0)
        && COCO_ENEMY_P( COCO_SOBJECT_NATIONALITY( cam ), nat )
        && coco_sobject_subtype_p( COCO_SOBJECT_CL( cam ), COCO_CAMERA_CLASS ))
      {
        min_distance = abs( COCO_SOBJECT_X( cam ) - sx );
        player_index = COCO_SOBJECT_CAMERA_INDEX( cam );
      }
  });
  return player_index;
}


void
coco_every_sobject_delete( coco_sobject *sobject,
                           coco_sobject_cl *cl,
                           coco_sstate *sstate )
{
  int idx;
  idx = COCO_SOBJECT_INDEX( sobject );
  COCO_DO_SSTATE_OBJECTS( so, sstate, {
    coco_sobject_unlink( so, COCO_SOBJECT_CL( so ), idx, sstate );
  });
  coco_sstate_delete_sobject( sstate, sobject );
}

void
coco_copter_mech_delete( coco_sobject *sobject,
                         coco_sobject_cl *cl,
                         coco_sstate *sstate )
{
  coco_sobject *camera;
  coco_gnum xmid, ymid;
  coco_gnum x = COCO_SOBJECT_X( sobject );
  int cam_idx = COCO_SOBJECT_CAMERA_INDEX( sobject );
  coco_nationality nat = COCO_SOBJECT_NATIONALITY( sobject );
  coco_gnum velo_offset
    = coco_camera_velocity_offset( sobject, COCO_SOBJECT_CL( sobject ),
                                   sstate );
  coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ),
                       COCO_SOBJECT_RASTER_INDEX( sobject ),
                       &xmid, &ymid );
  coco_sobject_delete( sobject, cl->super, sstate ); /* call-next-method */
  sobject = NULL; /* because we can't legally access it after here */
  if (cam_idx<0) return; /* comp don't need an inviso-camera */
  camera = coco_sstate_allocate_sobject( sstate );
  if (!camera) return;
  coco_init_inviso_camera( camera, COCO_INVISO_CAMERA_CLASS, cam_idx,
                           /* duplicate the X */
                           x + xmid + velo_offset,
                           nat );
  sstate->camera_indicies[cam_idx] = COCO_SOBJECT_INDEX( camera );
}


unsigned char
coco_every_sobject_raster_flip_index( coco_sobject *sobject,
                                      coco_sobject_cl *cl,
                                      unsigned char index )
{
  switch (COCO_SOBJECT_RASTER_COUNT( sobject )) {
  case 0: return 0;
  case 1: return 1;
  case 2: return ~index;
  default:
    coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                       "Need to implement a raster_flip_index method for"
                       " class number %d\n", COCO_SOBJECT_CL_INDEX( sobject ));
    return 0; /* shut up compiler */
  }
}

unsigned char
coco_copter_raster_flip_index( coco_sobject *copter,
                               coco_sobject_cl *cl,
                               unsigned char index )
{
  return index ^ 1;
}


void 
coco_every_sobject_toggle_facing( coco_sobject *sobject, coco_sobject_cl *cl )
{
  if (COCO_SOBJECT_RASTER_COUNT( sobject ) > 0)
    {
      unsigned char flip_index;
      unsigned char oldri = COCO_SOBJECT_RASTER_INDEX( sobject );
      flip_index
        = coco_raster_flip_index( sobject, COCO_SOBJECT_CL( sobject ), oldri );
      coco_sobject_set_raster_index( sobject, flip_index );
    }
  COCO_SOBJECT_FACING( sobject ) = -1 * COCO_SOBJECT_FACING( sobject );
}

void 
coco_copter_toggle_facing( coco_sobject *sobject, coco_sobject_cl *cl )
{
  /* call-next-method */
  coco_toggle_facing( sobject, cl->super );
  COCO_SOBJECT_ANGLE( sobject ) 
    = (COCO_SOBJECT_ANGLE( sobject ) + (int)128) & 255;
}

#define COCO_SIGN_SQRT( x ) ((((x)<0)?-1:1) * sqrt( abs( x ) ))

void
coco_copter_fire_bullet_coords( coco_sobject *copter, coco_sobject_cl *cl,
                                coco_sobject *target_ignore, 
                                coco_gnum *dx_return, coco_gnum *dy_return,
                                coco_gnum *x_return, coco_gnum *y_return )
{
  coco_sobject_cl *bcl = COCO_SOBJECT_BULLET_CLASS( copter );
  coco_angle aofa = COCO_SOBJECT_ANGLE( copter );
  coco_gnum dxp = COCO_SOBJECT_DX_PUBLIC( copter );
  *dx_return = COCO_COS( aofa ) * bcl->muzzle_velocity + dxp;
  *dy_return = COCO_SIN( aofa ) * bcl->muzzle_velocity; /* +dyp too unstable */
  coco_sobject_weapon_hardpoint_coords( copter, x_return, y_return );
}


void
coco_gun_fire_bullet_coords( coco_sobject *gun, coco_sobject_cl *cl,
                             coco_sobject *target, 
                             coco_gnum *dx_return, coco_gnum *dy_return,
                             coco_gnum *x_return, coco_gnum *y_return )
{
  coco_angle angle_return;
  coco_ai_bullet_coords( gun, target, 
                         /* motion-comp used to be TRUE, but this makes
                            life harder on mechs: */
                         coco_sobject_subtype_p( COCO_SOBJECT_CL( gun ),
                                                 COCO_AA_GUN_CLASS ), 
                         0.4,
                         COCO_SOBJECT_BULLET_CLASS( gun ),
                         dx_return, dy_return, x_return, y_return,
                         &angle_return );

  /* stop firing into the ground -dm 05-30-2001 */
  *dy_return = abs( *dy_return ); 

  /* evil side effect: swivel gun barrel to track target */
  /* ps don't have to with tank because its update-state does it for us. */
  COCO_SOBJECT_ANGLE( gun ) = angle_return;
}

void
coco_tank_infantry_fire_bullet_coords( coco_sobject *gun, coco_sobject_cl *cl,
                             coco_sobject *target, 
                             coco_gnum *dx_return, coco_gnum *dy_return,
                             coco_gnum *x_return, coco_gnum *y_return )
{
  coco_angle angle_return;
  coco_ai_bullet_coords( gun, target, COCO_FALSE, 0.4,
                         COCO_SOBJECT_BULLET_CLASS( gun ),
                         dx_return, dy_return, x_return, y_return,
                         &angle_return );
}

void
coco_bunker_fire_bullet_coords( coco_sobject *gun, coco_sobject_cl *cl,
                                coco_sobject *target, 
                                coco_gnum *dx_return, coco_gnum *dy_return,
                                coco_gnum *x_return, coco_gnum *y_return )
{
  coco_angle angle_return;
  coco_ai_bullet_coords( gun, target, COCO_TRUE, 0.4,
                         COCO_SOBJECT_BULLET_CLASS( gun ),
                         dx_return, dy_return, x_return, y_return,
                         &angle_return );
}



coco_sobject *
coco_every_sobject_fire_bullet( coco_sobject *sobject, coco_sobject_cl *cl,
                                coco_sstate *sstate,
                                coco_sobject *target_or_null )
{
  coco_sobject_cl *bcl;
  coco_facing facing = COCO_SOBJECT_FACING( sobject );
  coco_gnum dx, dy, x, y;
  coco_sobject *bullet;
  coco_fire_bullet_coords( sobject, COCO_SOBJECT_CL( sobject ),
                           target_or_null, &dx, &dy, &x, &y );
  bullet = coco_sstate_allocate_sobject( sstate );
  if (NULL==bullet) return NULL;
  bcl = COCO_SOBJECT_BULLET_CLASS( sobject );
  coco_init_bullet( bullet, bcl,
                    facing ? facing : (dx>0?1:-1), /* doesn't matter tho */
                    x, y, dx, dy, COCO_SOBJECT_NATIONALITY( sobject ),
                    COCO_U32_ADD_N_MODULAR( sstate->time, bcl->lifespan ));
  /* coco_cookie( "Class %s bullet sound type %d\n", COCO_SOBJECT_CL( sobject )->name, COCO_SOBJECT_FIRE_BULLET_SOUND_TYPE( sobject ) ); */
  coco_make_snd( COCO_SOBJECT_FIRE_BULLET_SOUND_TYPE( sobject ), sstate, 
                 x, y );
  return bullet;
}

coco_sobject *
coco_copter_fire_bullet( coco_sobject *copter, coco_sobject_cl *cl,
                         coco_sstate *sstate, coco_sobject *target_or_null )
{
  unsigned short bullets = COCO_COPTER_BULLETS( copter );
  if ((bullets > 0) && !COCO_COPTER_GUN_COOLDOWN( copter ))
    {
      /* call-next-method */
      coco_sobject *bul
        = coco_fire_bullet( copter, cl->super, sstate, target_or_null );
      if (bul)
        {
          COCO_COPTER_BULLETS( copter ) = bullets - 1;
          COCO_COPTER_GUN_COOLDOWN( copter ) = COCO_TRUE;
        }
      return bul;
    }
  else return NULL;
}

coco_sobject *
coco_every_sobject_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                                  coco_sstate *sstate, coco_sobject *target )
{
  coco_sobject *missile = coco_sstate_allocate_sobject( sstate );
  coco_gnum hpx, hpy, tx, ty;
  if (!missile) return NULL;
  coco_center_of_mass_obsolete( target, COCO_SOBJECT_CL( target ),
                                COCO_SOBJECT_RASTER_INDEX( target ),
                                &tx, &ty );
  tx += COCO_SOBJECT_X( target ) + COCO_SOBJECT_DX_PUBLIC( target );
  ty += COCO_SOBJECT_Y( target ) + COCO_SOBJECT_DY_PUBLIC( target );
  coco_sobject_weapon_hardpoint_coords( sobject, &hpx, &hpy );
  coco_init_missile( missile, COCO_MISSILE_CLASS, 0, hpx, hpy, 
                     COCO_SOBJECT_DX( sobject ), 
                     /* dy was COCO_SOBJECT_DY( sobject ) and not 0,
                        but feels wrong for copters */
                     0, 
                     COCO_SOBJECT_NATIONALITY( sobject ),
                     coco_atan2( (ty - hpy), (tx - hpx) ),
                     COCO_SOBJECT_INDEX( target ) );
  coco_make_snd( COCO_SOUND_MISSILE_LAUNCH, sstate, hpx, hpy );
  return missile;
}

coco_sobject *
coco_copter_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                           coco_sstate *sstate, coco_sobject *target )
{
  unsigned char missiles = COCO_COPTER_MISSILES( sobject );
  coco_sobject *missile = NULL;
  if ((missiles > 0) && !COCO_COPTER_MISSILE_COOLDOWN( sobject ))
    {
      /* call-next-method */
      missile = coco_launch_missile( sobject, cla->super, sstate, target );
      if (missile)
        {
          COCO_COPTER_MISSILES( sobject ) = missiles - 1;
          COCO_COPTER_MISSILE_COOLDOWN( sobject ) = COCO_TRUE;
          COCO_COPTER_MISSILE_COOLDOWN_TIME( sobject )
            = COCO_U32_ADD_N_MODULAR( sstate->time, 
                                      COCO_IDEAL_FRAMES_PER_SECOND>>1 );
        }
    }
  return missile;
}

coco_sobject *
coco_firetruck_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                              coco_sstate *sstate, coco_sobject *target )
{
  unsigned char ri = COCO_SOBJECT_RASTER_INDEX( sobject );
  coco_sobject *missile = NULL;
  coco_u32 time;
  if (((ri == 0) || (ri == 2)) &&
      (COCO_SOBJECT_LOCKED_ON_TIME( target ) 
       != (time = (sstate->time / (COCO_IDEAL_FRAMES_PER_SECOND<<1)))))
    {
      /* call-next-method */
      missile = coco_launch_missile( sobject, cla->super, sstate, target );
      if (missile)
        {
          int i;
          COCO_SOBJECT_LOCKED_ON_TIME( target ) = time;
          COCO_SOBJECT_DX( sobject ) *= -1;
          COCO_FIRETRUCK_SMOKE_COUNTDOWN( sobject ) = 5;
          coco_sobject_set_raster_index( sobject, ri + 1 );
          COCO_SOBJECT_FACING( sobject ) *= -1;
          for (i=0; i<COCO_SMOKE_RASTER_COUNT; i++)
            {
              coco_sobject *smoke = coco_sstate_allocate_sobject( sstate );
              if (!smoke) break;
              coco_init_smoke( smoke, COCO_SMOKE_CLASS, 
                               COCO_SOBJECT_X( sobject ) + (rand()&63) + 5,
                               COCO_GROUND_Y + (rand()&15), i );
            }
        }
    }
  return missile;
}

coco_sobject *
coco_headquarters_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                                 coco_sstate *sstate, coco_sobject *target )
{
  coco_sobject *missile = NULL;
  coco_u32 time = sstate->time / (COCO_IDEAL_FRAMES_PER_SECOND<<1);
  if (COCO_SOBJECT_LOCKED_ON_TIME( target ) != time)
    {
      /* call-next-method */
      missile = coco_launch_missile( sobject, cla->super, sstate, target );
      if (missile)
        {
          int i;
          COCO_SOBJECT_LOCKED_ON_TIME( target ) = time;
          COCO_HEADQUARTERS_SMOKE_COUNTDOWN( sobject ) = 5;
          for (i=0; i<COCO_SMOKE_RASTER_COUNT; i++)
            {
              coco_sobject *smoke = coco_sstate_allocate_sobject( sstate );
              if (NULL==smoke) break;
              coco_init_smoke( smoke, COCO_SMOKE_CLASS, 
                               COCO_SOBJECT_X( sobject ) + (rand()&63)
                               + ((COCO_SOBJECT_W( sobject )-63)>>1),
                               COCO_GROUND_Y + (rand()&31), i );
            }
        }
    }
  return missile;
}


coco_sobject *
coco_drop_bomb( coco_sobject *copter, coco_sobject_cl *cl,
                coco_sstate *sstate )
{
  unsigned char bombs = COCO_COPTER_BOMBS( copter );
  if ((bombs > 0)
      && (COCO_U32_ADD_MODULAR( COCO_COPTER_LAST_BOMB_DROP_TIME( copter ) )
          != sstate->time))
    {
      coco_sobject *bomb;
      coco_facing copter_facing = COCO_SOBJECT_FACING( copter );
      bomb = coco_sstate_allocate_sobject( sstate );
      if (NULL == bomb) return NULL;
      else
        {
          unsigned char ri = COCO_SOBJECT_RASTER_INDEX( copter );
          coco_mini_point hp
            = COCO_SOBJECT_WEAPON_HARDPOINTS( copter )[ri];
          COCO_COPTER_BOMBS( copter ) = bombs - 1;
          COCO_COPTER_LAST_BOMB_DROP_TIME( copter ) = sstate->time;
          coco_init_bomb( bomb, COCO_BOMB_CLASS, copter_facing, 
                          COCO_SOBJECT_X( copter ) + COCO_MINI_POINT_X( hp )
                          - (COCO_BOMB_EAST_RASTER_W>>1),
                          COCO_SOBJECT_Y( copter ) + COCO_MINI_POINT_Y( hp )
                          - COCO_BOMB_EAST_RASTER_H,
                          COCO_SOBJECT_DX( copter ),
                          COCO_SOBJECT_DY( copter ),
                          COCO_SOBJECT_NATIONALITY( copter ) );
          return bomb;
        }
    }
  else return NULL;
}

coco_sobject *
coco_launch_rocket( coco_sobject *sobject, coco_sobject_cl *cl,
                    coco_sstate *sstate )
{
  unsigned char rockets = COCO_COPTER_ROCKETS( sobject );
  coco_u32 last_time = COCO_COPTER_LAST_ROCKET_LAUNCH_TIME( sobject );
  if ((rockets > 0)
      /* any closer together timewise than 3 ticks, and the shrapnel lites
         back up the fuse of a full spread, and can even climb to your
         copter. */
      && (COCO_U32_ADD_N_MODULAR( last_time, 3 ) < sstate->time))
    {
      coco_facing facing = COCO_SOBJECT_FACING( sobject );
      coco_sobject *rocket = coco_sstate_allocate_sobject( sstate );
      if (rocket)
        {
          coco_gnum x, y, sodx;
          COCO_COPTER_ROCKETS( sobject ) = rockets - 1;
          COCO_COPTER_LAST_ROCKET_LAUNCH_TIME( sobject ) = sstate->time;
          coco_sobject_weapon_hardpoint_coords( sobject, &x, &y );
          sodx = COCO_SOBJECT_DX( sobject );
          if ((sodx<0)!=(facing<0))
            sodx=0;
          coco_init_rocket( rocket, COCO_ROCKET_CLASS, facing, x, y,
                            sodx, 
                            -abs( sodx )>>2,
                            COCO_SOBJECT_NATIONALITY( sobject ),
                            (facing>0)
                            ? COCO_ROCKET_RIGHT_ANGLE
                            : COCO_ROCKET_LEFT_ANGLE );
          coco_make_snd( COCO_SOUND_ROCKET_LAUNCH, sstate, x, y );
        }
      else coco_cookie( "FAILED TO ALLOC ROCKET!!!\n" );
      return rocket;
    }
  else return NULL;
}

void 
coco_copter_pad_update( coco_sobject *copter, coco_sobject_cl *cl,
                        coco_sstate *sstate )
{ /* reload and refuel copter */
  coco_u32 time = sstate->time;
  int bullets = COCO_COPTER_BULLETS( copter );
  coco_health health = COCO_SOBJECT_HEALTH( copter );
  int fuel;
  if (bullets < COCO_SOBJECT_MAX_BULLETS( copter ))
    COCO_COPTER_BULLETS( copter ) = 1 + bullets;
  fuel = COCO_COPTER_FUEL( copter );
  if (fuel < COCO_SOBJECT_MAX_FUEL( copter ))
    COCO_COPTER_FUEL( copter ) = fuel + COCO_IDEAL_FRAMES_PER_SECOND;
  if (0==(time&3))
    {
      int rockets;
      int bombs = COCO_COPTER_BOMBS( copter );

      if (bombs < COCO_SOBJECT_MAX_BOMBS( copter ))
        COCO_COPTER_BOMBS( copter ) = 1 + bombs;

      rockets = COCO_COPTER_ROCKETS( copter );
      if ((rockets < COCO_SOBJECT_MAX_ROCKETS( copter ))
          && (0==(time&7)))
        COCO_COPTER_ROCKETS( copter ) = 1 + rockets;
    }
  if ((health < COCO_SOBJECT_HEALTH_MAX( copter ))
      && (0==(time&COCO_PAD_HEAL_MASK)))
    COCO_SOBJECT_HEALTH( copter ) = 1 + health;
  {
    int missiles = COCO_COPTER_MISSILES( copter );
    if ((missiles < COCO_SOBJECT_MAX_MISSILES( copter ))
        && (0==(time%(COCO_IDEAL_FRAMES_PER_SECOND<<1))))
      {
        if (COCO_COPTER_NEXT_MISSILEBEAT_ON_PAD( copter ) == time)
          COCO_COPTER_MISSILES( copter ) = missiles + 1;
        else COCO_COPTER_NEXT_MISSILEBEAT_ON_PAD( copter )
               = COCO_U32_ADD_N_MODULAR( time,
                                         COCO_IDEAL_FRAMES_PER_SECOND<<1 );
      }

  }

}


coco_gnum 
coco_every_camera_velocity_offset( coco_sobject *camera, coco_sobject_cl *cla,
                                   coco_sstate *sstate )
{
  return 0;
}

coco_gnum 
coco_copter_velocity_offset( coco_sobject *camera, 
                             coco_sobject_cl *cla,
                             coco_sstate *sstate )
{
  coco_gnum old, raw, new;
  coco_nationality nat = COCO_SOBJECT_NATIONALITY( camera );
  coco_gnum x = COCO_SOBJECT_X( camera )
    + COCO_MINI_POINT_X( coco_sobject_com( camera ) );
  old = COCO_CAMERA_OLD_VELOCITY_OFFSET( camera );
  raw = coco_copter_velocity_offset_old_skool( camera, 
                                               COCO_SOBJECT_CL( camera ),
                                               sstate );
  if (abs( old ) < abs( raw ))
    {
      coco_bool sticky = COCO_FALSE;
      COCO_DO_SSTATE_OBJECTS( target, sstate,
      {
        if (COCO_ENEMY_P( nat, COCO_SOBJECT_NATIONALITY( target ) )
            && coco_sobject_subtype_p( COCO_SOBJECT_CL( target ), 
                                       COCO_CAMERA_CLASS )
            && !coco_sobject_subtype_p( COCO_SOBJECT_CL( target ), 
                                        COCO_INVISO_CAMERA_CLASS ))
          {
            coco_gnum delta = x
              - COCO_MINI_POINT_X( coco_sobject_com( target ))
              - COCO_SOBJECT_X( target );
            if ((abs( delta ) <= (3*COCO_OFFICIAL_WINDOW_W/4)))
              {
                if ((delta<0) == (raw<0))
                  sticky = COCO_TRUE;
                else goto be_raw;
              }
          }
      });
      if (sticky)
        return old;
    }
 be_raw:
  new = coco_tend_towards( old, raw, 12 ); /* don't snap camera too hard */
  COCO_CAMERA_OLD_VELOCITY_OFFSET( camera ) = new;
  return new;
}


coco_gnum 
coco_mech_velocity_offset( coco_sobject *camera, coco_sobject_cl *cla,
                           coco_sstate *sstate )
{
  switch (COCO_SOBJECT_NATIONALITY( camera )) {
  case COCO_GOOD_GUYS: return -COCO_OFFICIAL_WINDOW_W>>3;
  case COCO_BAD_GUYS:  return COCO_OFFICIAL_WINDOW_W>>3;
  case COCO_NEUTRAL:
  default: 
    return 0;
  }
}


void
sobject_ignore_neutral_buiding( coco_sobject *unit, coco_sobject_cl *cla,
                                coco_sobject *building )
{
  ; /* do nothing */
}

coco_sobject *
coco_copter_drop_paratrooper( coco_sobject *sobject,
                              coco_sobject_cl *cla,
                              coco_sstate *sstate )
{
  unsigned char ntroopers = COCO_COPTER_PARATROOPERS( sobject );
  unsigned char nengineers = COCO_COPTER_ENGITROOPERS( sobject );
  if ((ntroopers > 0) || (nengineers > 0))
    {
      coco_sobject *troop 
        = coco_drop_paratrooper( sobject, cla->super, sstate );
      if (troop)
        {
          if (ntroopers > 0)
            COCO_COPTER_PARATROOPERS( sobject ) = ntroopers - 1;
          else 
            COCO_COPTER_ENGITROOPERS( sobject ) = nengineers - 1;
          return troop;
        }
    }
  return NULL;
}


coco_sobject *
coco_every_sobject_drop_paratrooper( coco_sobject *sobject,
                                     coco_sobject_cl *cla,
                                     coco_sstate *sstate )
{
  coco_sobject *troop;
  coco_gnum x, y;
  troop = coco_sstate_allocate_sobject( sstate );
  if (NULL == troop) return NULL;
  coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ), 
                       COCO_SOBJECT_RASTER_INDEX( sobject ), &x, &y );
  coco_init_infantry( troop, COCO_INFANTRY_CLASS, 
                      COCO_SOBJECT_X( sobject )+x,
                      COCO_SOBJECT_Y( sobject )-1,
                      0, 0,
                      COCO_INFANTRY_FREE_FALL_BASE
                      + (rand()%COCO_INFANTRY_FREE_FALL_COUNT),
                      COCO_SOBJECT_NATIONALITY( sobject ) );
  return troop;
}

coco_sobject *
coco_inviso_camera_drop_paratrooper( coco_sobject *sobject,
                                     coco_sobject_cl *cla,
                                     coco_sstate *sstate )
{
  return NULL;
}

coco_health
coco_unarmored_effective_damage( coco_sobject *damagee,
                                 coco_sobject_cl *damagee_cl,
                                 coco_sobject *damager )
{
  return COCO_SOBJECT_DAMAGE( damager );
}

coco_health
coco_flesh_damageable_effective_damage( coco_sobject *damagee,
                                       coco_sobject_cl *damagee_cl,
                                       coco_sobject *damager )
{
  /* rifle bullets can only kill one dude at a time */
  return coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                                 COCO_INFANTRY_CLASS )
    ? 1
    : COCO_SOBJECT_DAMAGE( damager );
}

coco_health
coco_infantry_effective_damage( coco_sobject *damagee,
                                coco_sobject_cl *damagee_cl,
                                coco_sobject *damager )
{
  int ri = COCO_SOBJECT_RASTER_INDEX( damager );
  return ((ri >= COCO_EXPLOSION_ANTI_PERSONELL_BASE)
          && (ri < (COCO_EXPLOSION_ANTI_PERSONELL_COUNT
                    + COCO_EXPLOSION_ANTI_PERSONELL_BASE))
          && coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                                     COCO_EXPLOSION_CLASS ))
    ? 1
    : ((COCO_SOBJECT_Y( damagee )>0)
       && coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                                  COCO_SHRAPNEL_CLASS )
       && (rand()&15)
       ? 0
       : COCO_SOBJECT_DAMAGE( damager ));
}



/* 50% effective against rifle bullets, double damage from bombs, 2/3
   damage from rockets. */
coco_health
coco_gun_effective_damage( coco_sobject *damagee,
                           coco_sobject_cl *damagee_cl,
                           coco_sobject *damager )
{
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                              COCO_BOMB_CLASS ))
    return COCO_SOBJECT_DAMAGE( damager ) << 1;
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                                   COCO_ROCKET_CLASS ))
    return (COCO_SOBJECT_DAMAGE( damager ) << 1) / 3;
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
				   COCO_RIFLE_BULLET_CLASS ))
    return (rand()%2) ? 0 : COCO_SOBJECT_DAMAGE( damager );
  else return COCO_SOBJECT_DAMAGE( damager );
}


/* 100% effective against rifle bullets and shrapnel. */
coco_health
coco_heavy_armor_effective_damage( coco_sobject *damagee,
                                 coco_sobject_cl *damagee_cl,
                                 coco_sobject *damager )
{
  if ((coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                               COCO_RIFLE_BULLET_CLASS )
       || coco_sobject_subtype_p( COCO_SOBJECT_CL( damager ),
                                  COCO_SHRAPNEL_CLASS )))
    {
      return 0;
    }
  else
    { 
      return COCO_SOBJECT_DAMAGE( damager );
    }
}


void
coco_ignore_ground_collide( coco_sobject *sobject, coco_sobject_cl *cla,
                            coco_sstate *sstate )
{
  ; /* do nothing */
}

void
coco_fixup_ground_collide( coco_sobject *sobject, coco_sobject_cl *cla,
                            coco_sstate *sstate )
{
  COCO_SOBJECT_Y( sobject ) = COCO_GROUND_Y;
}


static coco_bool
coco_safe_landing_angle_p( coco_sobject *sobject, coco_sobject_cl *cla )
{
  coco_angle angle = COCO_SOBJECT_ANGLE( sobject );
  coco_bool result
    = (((angle <= (128 + COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK))
        && (angle >= (128 - COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK)))
       || (angle <= COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK)
       || (angle >= (((int) 256)
                     -COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK)));
  return result;
}


void
coco_copter_ground_collide( coco_sobject *sobject, coco_sobject_cl *cla,
                            coco_sstate *sstate )
{
  
  if (coco_safe_landing_angle_p( sobject, COCO_SOBJECT_CL( sobject ) )
      && (COCO_COPTER_FUEL( sobject )>0))
    {
      COCO_SOBJECT_Y( sobject ) = COCO_GROUND_Y;
      COCO_SOBJECT_DY( sobject ) = 0;
      COCO_SOBJECT_DX( sobject ) = 0;
      COCO_COPTER_LANDED_P( sobject ) = COCO_TRUE;
    }
  else
    {
      /* call-next-method */
      coco_ground_collide( sobject, cla->super, sstate ); 
    }
}

void
coco_every_sobject_ground_collide( coco_sobject *sobject,
                                   coco_sobject_cl *cla,
                                   coco_sstate *sstate )
{
  /*
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( sobject ),
                              COCO_BOMB_CLASS ))
    coco_cookie( "Packed right into the surface\n" ); */
  coco_kaboom( sobject, COCO_SOBJECT_CL( sobject ), sstate );
}


void
coco_building_hostile_collision( coco_sobject *building,
                                 coco_sobject_cl *building_cl,
                                 coco_sobject *obj2, 
                                 coco_sstate *sstate,
                                 coco_bool *stat1, coco_bool *stat2 )
{
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ), COCO_BUILDING_CLASS ))
    *stat1 = *stat2 = COCO_FALSE;
  /* call-next-method */
  else coco_hostile_collision( building, building_cl->super, obj2, sstate,
                               stat1, stat2 );
}

#define COCO_PLASMA_DISINTEGRATE( plasma ) \
  { COCO_PLASMA_COHERENT( plasma ) = COCO_FALSE; \
    COCO_SOBJECT_RASTER_INDEX( plasma ) = 0; }


void
coco_plasma_hostile_collision( coco_sobject *plasma,
                               coco_sobject_cl *plasma_cl,
                               coco_sobject *obj2, 
                               coco_sstate *sstate,
                               coco_bool *stat1, coco_bool *stat2 )
{
  *stat1 = *stat2 = COCO_FALSE;

  /* call-next-method */
  coco_hostile_collision( plasma, plasma_cl->super, obj2, sstate,
                          stat1, stat2 );
  if (!*stat1)
    {
      COCO_PLASMA_DISINTEGRATE( plasma );
      COCO_SOBJECT_DX( plasma ) >>= 1;
      COCO_SOBJECT_DY( plasma ) >>= 1;
    }
}

void
coco_plasma_friendly_collision( coco_sobject *plasma,
                                coco_sobject_cl *plasma_cl,
                                coco_sobject *obj2, 
                                coco_sstate *sstate,
                                coco_bool *stat1, coco_bool *stat2 )
{
  if (COCO_PLASMA_COHERENT( plasma )
      || !coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                 COCO_INFANTRY_CLASS ))
    /* call-next-method */
    coco_friendly_collision( plasma, plasma_cl->super, obj2, 
                             sstate, stat1, stat2 );
  else
    /* call-EVIL-method! */
    coco_hostile_collision( plasma, COCO_SOBJECT_CL( plasma ), obj2, sstate,
                            stat1, stat2 );
}


void
coco_plasma_ground_collide( coco_sobject *sobject, coco_sobject_cl *cla,
                            coco_sstate *sstate )
{
  coco_gnum y = COCO_SOBJECT_Y( sobject );
  coco_gnum cen = COCO_GROUND_Y - COCO_PLASMA_RADIUS( sobject );
  if (cen>=y)
    {
      COCO_SOBJECT_Y( sobject ) = cen;
      COCO_SOBJECT_DY( sobject ) = COCO_MAX( 0, COCO_SOBJECT_DY( sobject ) );
    }
}


void
coco_bunker_hostile_collision( coco_sobject *bunker,
                               coco_sobject_cl *bunker_cl,
                               coco_sobject *obj2, 
                               coco_sstate *sstate,
                               coco_bool *stat1, coco_bool *stat2 )
{
  coco_gnum roof_tab = COCO_SOBJECT_ROOF_TAB( bunker );
  unsigned char defenders = COCO_BUNKER_DEFENDERS( bunker );
  *stat1 = *stat2 = COCO_FALSE;
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                              COCO_TANK_BULLET_CLASS ))
    {
      if (defenders > 0)
        {
          COCO_BUNKER_DEFENDERS( bunker ) = --defenders;
          coco_kaboom( obj2, COCO_SOBJECT_CL( obj2 ), sstate );
          *stat2 = COCO_TRUE;
        }
    }
  else if ((COCO_GROUND_Y == COCO_SOBJECT_Y( obj2 ))
           && coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                      COCO_INFANTRY_CLASS )
           && ((COCO_SOBJECT_X( bunker ) + roof_tab)
               < COCO_SOBJECT_X_CENTER( obj2 ))
           && ((COCO_SOBJECT_X1( bunker ) - roof_tab)
               > COCO_SOBJECT_X_CENTER( obj2 )))
    {
      coco_sobject_delete( obj2, COCO_SOBJECT_CL( obj2 ), sstate );
      *stat2 = COCO_TRUE;
      if (0==defenders)
        {
          int refund;
          unsigned char ri = COCO_SOBJECT_RASTER_INDEX( bunker );
          unsigned char ricount = COCO_SOBJECT_RASTER_COUNT( bunker );
          unsigned char new_ri;
          refund = COCO_SOBJECT_REFUND( bunker );
          if (refund > 0)
            coco_add_funds( coco_find_closest_enemy_player( bunker, sstate ),
                            COCO_FALSE, refund );
          switch (ricount) {
          case 1: new_ri = ri; break;
          case 2: new_ri = ri^1; break;
          default: 
            new_ri = ri;
            coco_warn_printf( "Bunker or subtype has strange "
                              "number of rasters: %d.", ri );
          }
          coco_sobject_set_raster_index( bunker, new_ri );
          COCO_SOBJECT_NATIONALITY( bunker )
            = COCO_SOBJECT_NATIONALITY( obj2 );
          COCO_BUNKER_DEFENDERS( bunker ) = 1;
        }
      else COCO_BUNKER_DEFENDERS( bunker ) = --defenders;
    } 
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                   COCO_PLASMA_CLASS ))
    /* tricky calling into another method */
    coco_plasma_hostile_collision( obj2, COCO_SOBJECT_CL( obj2 ),
                                   bunker, sstate, stat1, stat2 );
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                   COCO_GROUND_UNIT_CLASS )
           || coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                      COCO_EXPLOSION_CLASS )
           || coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                      COCO_BUILDING_CLASS )
           || coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                      COCO_MECH_CLASS ))
    {
      /* no op -- they can blow/drive through. */
    }
  else coco_kaboom( obj2, COCO_SOBJECT_CL( obj2 ), sstate );
}

coco_bool
coco_bunker_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate )
{
  coco_gnum roof_tab = COCO_SOBJECT_ROOF_TAB( sobject );
  unsigned char rifles = COCO_BUNKER_DEFENDERS( sobject );
  coco_nationality sonat = COCO_SOBJECT_NATIONALITY( sobject );
  coco_bool can_shoot = (rifles > 0);
  coco_gnum bx0 = COCO_SOBJECT_X( sobject );
  coco_gnum bx1 = COCO_SOBJECT_W( sobject ) + bx0;
  coco_gnum bxr0 = bx0 - COCO_INFANTRY_GUN_RANGE;
  coco_gnum bxr1 = bx1 + COCO_INFANTRY_GUN_RANGE;
  bx0 += roof_tab;
  bx1 -= roof_tab;
  COCO_DO_SSTATE_OBJECTS( targ, sstate, {
    coco_gnum tx0 = COCO_SOBJECT_X( targ );
    coco_gnum tx1 = COCO_SOBJECT_W( targ ) + tx0;
    coco_gnum ty  = COCO_SOBJECT_Y( targ );
    coco_nationality tnat = COCO_SOBJECT_NATIONALITY( targ );
    if (can_shoot
        && (0 == (sstate->time 
                  & ((1<<COCO_MAX( 7 - rifles, 0 )) - 1)))
        && COCO_ENEMY_P( tnat, sonat )
        && (ty < COCO_GROUND_Y + 5)
        && (COCO_BETWEENI( bxr0, tx0, bx0 )
            || COCO_BETWEENI( bxr0, tx1, bx0 )
            || COCO_BETWEENI( bx1, tx0, bxr1 )
            || COCO_BETWEENI( bx1, tx1, bxr1 ))
        && coco_ground_trooper_target_p( targ, COCO_SOBJECT_CL( targ ), 
                                         sobject )
        && coco_fire_bullet( sobject, COCO_SOBJECT_CL( sobject ),
                             sstate, targ ))
      can_shoot = COCO_FALSE;
    if ((sonat == tnat)
        && (COCO_GROUND_Y == ty)
        && COCO_BETWEENI( bx0, (tx0+tx1)>>1, bx1 ))
      coco_enter_friendly_bunker( targ, COCO_SOBJECT_CL( targ ), 
                                  sobject, sstate );

  });
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


coco_bool
coco_plasma_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate )
{
  coco_gnum h;
  int dradius, radius;

  if (COCO_PLASMA_COHERENT( sobject ))
    {
      coco_gnum dx = COCO_SOBJECT_DX( sobject );
      coco_gnum dy = COCO_SOBJECT_DY( sobject );
      coco_gnum v = sqrt( dx*dx + dy*dy );
      coco_gnum vmax = COCO_PLASMA_SPEED;
      char t = sstate->time & 1;
      if (v>vmax) dradius = 2 + t;
      else if ((v<<1)>vmax) dradius = 2;
      else dradius = 1 + t;
    }
  else dradius = 5;
  radius = COCO_PLASMA_RADIUS( sobject ) + dradius;
  if (radius > COCO_PLASMA_RADIUS_MAX) return COCO_FALSE;
  COCO_PLASMA_RADIUS( sobject ) = radius;
  COCO_SOBJECT_X( sobject ) -= dradius;
  COCO_SOBJECT_Y( sobject ) -= dradius;
  h = COCO_SOBJECT_H( sobject ) + (dradius<<1);
  COCO_SOBJECT_W( sobject ) = h;
  COCO_SOBJECT_H( sobject ) = h;
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}



coco_bool
coco_balloon_factory_update_state( coco_sobject *sobject,
                                   coco_sobject_cl *cl,
                                   coco_sstate *sstate )
{
  double seconds = ((double) sstate->time) / COCO_IDEAL_FRAMES_PER_SECOND;
  double seconds_ = floor( seconds );
  if (seconds == seconds_)
    {
      
      int smokesize;
      coco_u32 div = COCO_BALLOON_FACTORY_BALLOON_RATE( sobject );
      switch (((int) seconds_) % div) {
      case 1:
        {
          coco_sobject *b = coco_sstate_allocate_sobject( sstate );
          if (b)
            {
              coco_sobject_cl *bc = COCO_FAKE_BALLOON_CLASS;
              coco_gnum x = COCO_SOBJECT_X( sobject )
                + COCO_BALLOON_FACTORY_BALLOON_TAB;
              coco_gnum y = 1 + COCO_SOBJECT_H( sobject )
                + COCO_SOBJECT_Y( sobject ) - bc->canonical_h;
              coco_init_fake_balloon( b, bc, x, y, 
                                      (rand()&1) + 1, COCO_NEUTRAL );
              COCO_SOBJECT_DY( b ) = 1;
              COCO_BALLOON_FACTORY_FRESH( b ) = COCO_TRUE;
            }
          smokesize = 2;
        }
        break;
      case 0: smokesize = 2; break;
      default: smokesize = rand()%3-1; break;
      }
      if (smokesize>=0)
        {
          coco_sobject *s = coco_sstate_allocate_sobject( sstate );
          if (s)
            {
              coco_sobject_cl *sc = COCO_SMOKE_CLASS;
              coco_gnum x = COCO_SOBJECT_X( sobject )
                + COCO_BALLOON_FACTORY_STACK_TAB 
                + ((COCO_BALLOON_FACTORY_STACK_RASTER_W - sc->canonical_w)>>1);
              coco_gnum y = COCO_SOBJECT_Y( sobject )
                + COCO_SOBJECT_H( sobject )
                + COCO_BALLOON_FACTORY_STACK_RASTER_H
                + COCO_SMOKE_BIG_RASTER_H;
              coco_init_smoke( s, sc, x, y, smokesize );
            }
        }

    }
  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}


void
coco_health_subtracting_hostile_collision( coco_sobject *obj1,
                                           coco_sobject_cl *obj1cl,
                                           coco_sobject *obj2, 
                                           coco_sstate *sstate,
                                           coco_bool *stat1, coco_bool *stat2 )
{ 
  coco_health hea1 = COCO_SOBJECT_HEALTH( obj1 );
  coco_health hea2 = COCO_SOBJECT_HEALTH( obj2 );
  /* dmg2: damage done by 2 to 1 */
  coco_health dmg2 = coco_effective_damage( obj1, COCO_SOBJECT_CL( obj1 ),
                                            obj2 );
  /* dmg1: damage done by 1 to 2 */
  coco_health dmg1 = coco_effective_damage( obj2, COCO_SOBJECT_CL( obj2 ),
                                            obj1 );
  /*
  coco_cookie( "smak dmg1 = %d, hea2 = %d, dmg2 = %d, hea1 = %d\n",
  dmg1, hea2, dmg2, hea1 ); */
    
  *stat1 = *stat2 = COCO_FALSE;

  /* COLLISION OVERHAUL
     if (coco_execute_building_collision_maybe( sstate, obj1, obj2, &rv )
     || coco_execute_building_collision_maybe( sstate, obj2, obj1, &dummy ))
     return rv;
  */
  if (hea1 != COCO_NIGH_INVULNERABLE)
    {
      if (hea1 <= dmg2)
        {
          coco_kaboom( obj1, COCO_SOBJECT_CL( obj1 ), sstate );
          *stat1 = COCO_TRUE;
        }
      else COCO_SOBJECT_HEALTH( obj1 ) = hea1 - dmg2;
    }
  if (hea2 != COCO_NIGH_INVULNERABLE)
    {
      if (hea2 <= dmg1)
        {
          coco_kaboom( obj2, COCO_SOBJECT_CL( obj2 ), sstate );
          *stat2 = COCO_TRUE;
        }
      else COCO_SOBJECT_HEALTH( obj2 ) = hea2 - dmg1;
    }
}

void
coco_every_sobject_hostile_collision( coco_sobject *obj1,
                                      coco_sobject_cl *obj1cl,
                                      coco_sobject *obj2, 
                                      coco_sstate *sstate,
                                      coco_bool *stat1, coco_bool *stat2 )
{
  coco_health_subtracting_hostile_collision( obj1, obj1cl, obj2, sstate,
                                             stat1, stat2 );
}


void
coco_munition_hostile_collision( coco_sobject *obj1,
                                 coco_sobject_cl *obj1cl,
                                 coco_sobject *obj2, 
                                 coco_sstate *sstate,
                                 coco_bool *stat1, coco_bool *stat2 )
{
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ), 
                              COCO_BALLOON_LINE_CLASS ))
    {
      *stat1 = *stat2 = COCO_FALSE;
    }
  else coco_health_subtracting_hostile_collision( obj1, obj1cl, obj2, sstate,
                                                  stat1, stat2 );
}

void
coco_bullet_hostile_collision( coco_sobject *obj1,
                               coco_sobject_cl *obj1cl,
                               coco_sobject *obj2, 
                               coco_sstate *sstate,
                               coco_bool *stat1, coco_bool *stat2 )
{
  unsigned char ri = COCO_SOBJECT_RASTER_INDEX( obj2 );
  if (((ri == COCO_INFANTRY_GOOD_GUY_CHUTE)
       || (ri == COCO_INFANTRY_BAD_GUY_CHUTE))
      && (COCO_SOBJECT_Y( obj1 )
          >= COCO_SOBJECT_Y( obj2 ) + COCO_INFANTRY_CHUTE_Y)
      && coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ), 
                                 COCO_INFANTRY_CLASS ))
    {
      *stat1 = COCO_TRUE;
      *stat2 = COCO_FALSE;
      coco_kaboom( obj1, COCO_SOBJECT_CL( obj1 ), sstate );
      COCO_SOBJECT_RASTER_INDEX( obj2 ) = COCO_INFANTRY_FREE_FALL_BASE;
      COCO_INFANTRY_PARACHUTES( obj2 ) = 0;
      return;
    }
  /*
  else if (COCO_SOBJECT_HEALTH( obj2 ) == COCO_NIGH_INVULNERABLE)
    {
      /+ tired of seeing tanks unable to shoot through balloon factories,
       infantry unable to shoot through huts, etc. -dm 05-19-2001 +/
      /+ On second thought not -dm +/
      *stat1 = *stat2 = COCO_FALSE;
      return;
    }
  */
  /* call-next-method */
  else coco_hostile_collision( obj1, obj1cl->super, obj2, sstate, 
                               stat1, stat2 );
}



void
coco_balloon_part_hostile_collision( coco_sobject *obj1,
                                     coco_sobject_cl *obj1cl,
                                     coco_sobject *obj2, 
                                     coco_sstate *sstate,
                                     coco_bool *stat1,
                                     coco_bool *stat2 )
{
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ), 
                              COCO_BALLOON_PART_CLASS )
      || coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ), 
                                 COCO_BUILDING_CLASS ))
    *stat1 = *stat2 = COCO_FALSE;
  /* call-next-method */
  else coco_hostile_collision( obj1, obj1cl->super, obj2, sstate,
                               stat1, stat2 );
}


void
coco_ground_unit_hostile_collision( coco_sobject *obj1,
                               coco_sobject_cl *obj1cl,
                               coco_sobject *obj2, 
                               coco_sstate *sstate,
                               coco_bool *stat1, coco_bool *stat2 )
{
  assert( !coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                   COCO_MUNITION_CLASS) );
  /* do nothing, especially not call-next-method */
  *stat1 = *stat2 = COCO_FALSE;
}

void
coco_demo_van_hostile_collision( coco_sobject *obj1,
                                 coco_sobject_cl *obj1cl,
                                 coco_sobject *obj2, 
                                 coco_sstate *sstate,
                                 coco_bool *stat1, coco_bool *stat2 )
{
  if ((COCO_NEUTRAL == sstate->game_over_nationality)
      && coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                 COCO_HEADQUARTERS_CLASS ))
    {
      sstate->game_over_nationality = COCO_SOBJECT_NATIONALITY( obj1 );
      sstate->game_over_time
        = COCO_U32_ADD_N_MODULAR( sstate->time, 
                                  COCO_IDEAL_FRAMES_PER_SECOND<<4 );
      /* make van invulnerable */
      COCO_SOBJECT_HEALTH( obj1 ) = COCO_NIGH_INVULNERABLE;
      COCO_SOBJECT_DX( obj1 ) = 0;
      coco_kaboom( obj2, COCO_SOBJECT_CL( obj2 ), sstate );
      *stat1 = COCO_FALSE;
      *stat2 = COCO_TRUE;
    }
  /* call-next-method */
  else coco_hostile_collision( obj1, obj1cl->super, obj2, sstate,
                               stat1, stat2 );
}

void
coco_copter_hostile_collision( coco_sobject *obj1,
                               coco_sobject_cl *obj1cl,
                               coco_sobject *obj2, 
                               coco_sstate *sstate,
                               coco_bool *stat1, coco_bool *stat2 )
{
  *stat1 = *stat2 = COCO_FALSE;
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                              COCO_LANDING_PAD_CLASS ))
    {
      /* Note for copters with landing gear (ie collision recti bottom not
         0), the pad will jostle a bit as you hit it, because the bottom
         of the copter has to touch the pad before it gets kicked back up
         to the bottom of the raster. This because the collision code
         doesn't really know about the bottom of the copter raster, only
         the bottom of the collision recti. -dm 05-30-2001*/
      if (coco_safe_landing_angle_p( obj1, COCO_SOBJECT_CL( obj1 ) ))
        {
          coco_gnum x0, x1, y, px0, px1;
          coco_get_collision_recti( obj1, &x0, &x1, &y, &y );
          coco_get_collision_recti( obj2, &px0, &px1, &y, &y );
          if ((px0<=x0) && (x1<=px1))
            {
              COCO_COPTER_LANDED_P( obj1 ) = COCO_TRUE;
              COCO_SOBJECT_DX( obj1 ) = COCO_SOBJECT_DY( obj1 ) = 0;
              COCO_SOBJECT_Y( obj1 ) = COCO_SOBJECT_Y1( obj2 ) ;
              COCO_COPTER_LANDED_ON_PAD_P( obj1 ) = COCO_TRUE;
            }
        }
      /* else don't explode because we're collided with pad! */
    }
  /* call-next-method */
  else coco_hostile_collision( obj1, obj1cl->super, obj2, sstate,
                               stat1, stat2 );
}



void
coco_mech_hostile_collision( coco_sobject *obj1,
                             coco_sobject_cl *obj1cl,
                             coco_sobject *obj2, 
                             coco_sstate *sstate,
                             coco_bool *stat1, coco_bool *stat2 )
{
  *stat1 = *stat2 = COCO_FALSE;
  assert( !coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                   COCO_MUNITION_CLASS) );
  if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                              COCO_LANDING_PAD_CLASS ))
    {
      coco_gnum x0, x1, y, px0, px1;
      coco_get_collision_recti( obj1, &x0, &x1, &y, &y );
      coco_get_collision_recti( obj2, &px0, &px1, &y, &y );
      if ((px0<=x0) && (x1<=px1))
        {
          COCO_MECH_LANDED_ON_PAD_P( obj1 ) = COCO_TRUE;
          COCO_SOBJECT_DY( obj1 ) = 0;
          COCO_SOBJECT_Y( obj1 ) = COCO_SOBJECT_Y1( obj2 ) ;
        }
    }
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                   COCO_BUILDING_CLASS ))
    *stat1 = *stat2 = COCO_FALSE; /* eg hut, pad, hq */
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                   COCO_BALLOON_LINE_CLASS )
           && (COCO_SOBJECT_Y( obj1 ) <= (COCO_GROUND_Y+1))
           && (COCO_BALLOON_LINE_HUT_INDEX( obj2 ) >= 0))
    *stat1 = *stat2 = COCO_FALSE; /* walking mechs don't explode when they hit wires. */
  /* call-next-method */
  else coco_hostile_collision( obj1, obj1cl->super, obj2, sstate,
                               stat1, stat2 );
}

void
coco_every_sobject_friendly_collision( coco_sobject *obj1,
                                       coco_sobject_cl *obj1cl,
                                       coco_sobject *obj2, 
                                       coco_sstate *sstate,
                                       coco_bool *stat1, coco_bool *stat2 )
{
  *stat1 = *stat2 = COCO_FALSE;
}


void
coco_copter_friendly_collision( coco_sobject *obj1,
                                coco_sobject_cl *obj1cl,
                                coco_sobject *obj2, 
                                coco_sstate *sstate,
                                coco_bool *stat1, coco_bool *stat2 )
{
  unsigned char paratroopers = COCO_COPTER_PARATROOPERS( obj1 );
  *stat1 = *stat2 = COCO_FALSE;
  if (COCO_COPTER_LANDED_P( obj1 )
      && (!COCO_COPTER_LANDED_ON_PAD_P( obj1 ))
      && (COCO_COPTER_ENGITROOPERS( obj1 ) < 1))
    {
      coco_bool delete2 = COCO_FALSE;
      coco_bool is_engineer = coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                                      COCO_ENGINEER_CLASS );
      if ((0 == paratroopers) && is_engineer)
        {
          delete2 = COCO_TRUE;
          COCO_COPTER_ENGITROOPERS( obj1 )++;
        }
      else if ((paratroopers < COCO_SOBJECT_MAX_INTERNAL_INFANTRY( obj1 ))
               && (!is_engineer)
               && coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                                          COCO_INFANTRY_CLASS ))
        {
          delete2 = COCO_TRUE;
          COCO_COPTER_PARATROOPERS( obj1 ) = paratroopers + 1;
        }

      if (delete2)
        {
          *stat2 = COCO_TRUE;
          coco_sobject_delete( obj2, COCO_SOBJECT_CL( obj2 ), sstate );
          return;
        }
    }
  coco_friendly_collision( obj1, obj1cl->super, obj2, sstate, stat1, stat2 );
}


void
coco_every_sobject_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                           int unlink_idx, coco_sstate *sstate )
{
  ; /* do nothing */
}

void
coco_missile_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                    int unlink_idx, coco_sstate *sstate )
{
  if (unlink_idx == COCO_MISSILE_TARGET_INDEX( sobject ))
    {
      COCO_MISSILE_TARGET_INDEX( sobject ) = -1;
    }
  /* call-next-method */
  coco_sobject_unlink( sobject, cla->super, unlink_idx, sstate );
}


void
coco_balloon_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                     int unlink_idx, coco_sstate *sstate )
{
  if (unlink_idx == COCO_BALLOON_LINE_INDEX( sobject ))
    {
      COCO_BALLOON_LINE_INDEX( sobject ) = -1;
      COCO_SOBJECT_NATIONALITY( sobject ) = COCO_NEUTRAL;
    }
  /* call-next-method */
  coco_sobject_unlink( sobject, cla->super, unlink_idx, sstate );
}

void
coco_balloon_line_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                          int unlink_idx, coco_sstate *sstate )
{
  if (unlink_idx == COCO_BALLOON_LINE_HUT_INDEX( sobject ))
    {
      int balloon_idx = COCO_BALLOON_LINE_BALLOON_INDEX( sobject );
      COCO_BALLOON_LINE_HUT_INDEX( sobject ) = -1;
      COCO_SOBJECT_NATIONALITY( sobject ) = COCO_NEUTRAL;
      if (balloon_idx>=0)
        COCO_SOBJECT_NATIONALITY( COCO_SSTATE_OBJECT_REF( sstate,
                                                          balloon_idx ) )
          = COCO_NEUTRAL;
    }
  else if (unlink_idx == COCO_BALLOON_LINE_BALLOON_INDEX( sobject ))
    {
      COCO_BALLOON_LINE_BALLOON_INDEX( sobject ) = -1;
    }
  /* call-next-method */
  coco_sobject_unlink( sobject, cla->super, unlink_idx, sstate );
}

void
coco_hut_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                 int unlink_idx, coco_sstate *sstate )
{
  if (unlink_idx == COCO_HUT_LINE_INDEX( sobject ))
    {
      COCO_HUT_LINE_INDEX( sobject ) = -1;
      COCO_HUT_POINTS_TILL_REINCARNATION( sobject )
        = COCO_HUT_BALLOON_REINCARNATION_FRAMES;
    }
  /* call-next-method */
  coco_sobject_unlink( sobject, cla->super, unlink_idx, sstate );
}

void
coco_headquarters_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                 int unlink_idx, coco_sstate *sstate )
{
  if (unlink_idx == COCO_HEADQUARTERS_PILLBOX_INDEX( sobject ))
    {
      COCO_HEADQUARTERS_PILLBOX_INDEX( sobject ) = -1;
    }
  /* call-next-method */
  coco_sobject_unlink( sobject, cla->super, unlink_idx, sstate );
}

void
coco_pillbox_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                 int unlink_idx, coco_sstate *sstate )
{
  if (unlink_idx == COCO_PILLBOX_HEADQUARTERS_INDEX( sobject ))
    {
      COCO_PILLBOX_HEADQUARTERS_INDEX( sobject ) = -1;
    }
  /* call-next-method */
  coco_sobject_unlink( sobject, cla->super, unlink_idx, sstate );
}


coco_bool coco_trooper_always_target_me( coco_sobject *sobject, 
                                         coco_sobject_cl *cla,
                                         coco_sobject *targeter )
{
  return COCO_TRUE;
}


coco_bool coco_trooper_never_target_me( coco_sobject *sobject, 
                                         coco_sobject_cl *cla,
                                         coco_sobject *targeter )
{
  return COCO_FALSE;
}


coco_bool coco_trooper_bunker_target_p( coco_sobject *sobject, 
                                        coco_sobject_cl *cla,
                                        coco_sobject *targeter )
{
  /* its only a target if that would do some good. */
  return (COCO_BUNKER_DEFENDERS( sobject ) > 0)
    && coco_sobject_subtype_p( COCO_SOBJECT_BULLET_CLASS( targeter ), 
                               COCO_TANK_BULLET_CLASS );
}

void
coco_ignore_bunker( coco_sobject *sobject, coco_sobject_cl *cl,
                    coco_sobject *bunker, coco_sstate *sstate )
{
  ; /* do nothing */
}


void
coco_infantry_enter_friendly_bunker( coco_sobject *sobject,
                                     coco_sobject_cl *cl,
                                     coco_sobject *bunker,
                                     coco_sstate *sstate )
{
  unsigned char defenders = COCO_BUNKER_DEFENDERS( bunker );
  if (defenders < COCO_SOBJECT_MAX_INTERNAL_INFANTRY( bunker ))
    {
      COCO_BUNKER_DEFENDERS( bunker ) = defenders+1;
      coco_sobject_delete( sobject, COCO_SOBJECT_CL( sobject ), sstate );
    }
  /* call-next-method */
  else coco_enter_friendly_bunker( sobject, cl->super, bunker, sstate );
}


coco_blip_index
coco_every_sobject_blip_color_index( coco_sobject *sobject, 
                                     coco_sobject_cl *cl,
                                     int p_or_c_index,
                                     coco_bool is_comp,
                                     coco_sstate *sstate )
{
  coco_nationality nat = is_comp
    ? coco_comps[p_or_c_index].nationality
    : coco_players[p_or_c_index].nationality;
  coco_nationality onat = COCO_SOBJECT_NATIONALITY( sobject );
  coco_blip_triplet citrip 
    = COCO_SOBJECT_DEFAULT_BLIP_COLOR_INDEX( sobject );
  return ((onat == nat)
          ? COCO_BLIP_TRIPLET_FRY( citrip )
          : ((onat == COCO_NEUTRAL)
            ? COCO_BLIP_TRIPLET_NTL( citrip )
            : COCO_BLIP_TRIPLET_EMY( citrip )));
}


coco_blip_index
coco_camera_blip_color_index( coco_sobject *sobject, 
                              coco_sobject_cl *cl,
                              int p_or_c_index,
                              coco_bool is_comp,
                              coco_sstate *sstate )
{
  if ((is_comp || (!(sstate->time & 12)))
      && ((is_comp 
           ? COCO_SOBJECT_COMP_INDEX( sobject )
           : COCO_SOBJECT_CAMERA_INDEX( sobject ))
          == p_or_c_index))
    {
      return COCO_BLIP_CAMERA_BLINK;
    }
  /* call next method */
  else return coco_sobject_blip_color_index( sobject, cl->super, p_or_c_index, is_comp, sstate );
}

coco_blip_index
coco_copter_mech_blip_color_index( coco_sobject *sobject, 
                                   coco_sobject_cl *cl,
                                   int p_or_c_index,
                                   coco_bool is_comp,
                                   coco_sstate *sstate )
{
  if (coco_jamming)
    {
      coco_nationality nationality = COCO_SOBJECT_NATIONALITY( sobject );
      coco_gnum x = COCO_SOBJECT_X( sobject );
      coco_gnum y = COCO_SOBJECT_Y( sobject );
      if ((nationality == (is_comp 
                           ? coco_comps[p_or_c_index].nationality
                           : coco_players[p_or_c_index].nationality)))
        /* call-next-method */
        return coco_sobject_blip_color_index( sobject, cl->super, 
                                              p_or_c_index, is_comp, sstate );
      else
        COCO_DO_SSTATE_OBJECTS( sensor, sstate, 
        {
          coco_nationality sens_nat = COCO_SOBJECT_NATIONALITY( sensor );
          coco_sobject_cl *sens_cl = COCO_SOBJECT_CL( sensor );
          coco_gnum sens_y = COCO_SOBJECT_Y( sensor );
          if ((sens_nat != nationality)
              && (sens_nat != COCO_NEUTRAL)
              && ((abs(COCO_SOBJECT_X( sensor ) - x) 
                   + COCO_MAX( 0, y-sens_y )) /* works better downhill */
                  <= ((COCO_OFFICIAL_WINDOW_W>>1) + 100))
              && (coco_sobject_subtype_p( sens_cl, COCO_GROUND_UNIT_CLASS )
                  || coco_sobject_subtype_p( sens_cl, COCO_CAMERA_CLASS ) 
                  || coco_sobject_subtype_p( sens_cl, COCO_BUILDING_CLASS )))
            {
              /* call-next-method */
              return coco_sobject_blip_color_index( sobject, cl->super, 
                                                    p_or_c_index, is_comp,
                                                    sstate );
            }
        });
      return COCO_BLIP_TRANSPARENT;
    }
  else
    /* call-next-method */
    return coco_sobject_blip_color_index( sobject, cl->super, 
                                          p_or_c_index,
                                          is_comp, sstate );
}


void 
coco_cargo_copter_summon_places( coco_sobject *sobject, coco_sobject_cl *cl,
                                 coco_sstate *sstate, 
                                 coco_bool **summon_enable,
                                 coco_u32 **summon_disable_until )
{
  if (COCO_COPTER_LANDED_P( sobject )
      && !(COCO_COPTER_LANDED_ON_PAD_P( sobject )))
    {
      *summon_enable
        = &COCO_CARGO_COPTER_SUMMON_ENABLE( sobject );
      *summon_disable_until
        = &COCO_CARGO_COPTER_SUMMON_DISABLE_UNTIL( sobject );
    }
  else
    /* call-next-method */
    coco_sobject_summon_places( sobject, cl->super, sstate, summon_enable,
                                summon_disable_until );
}

void 
coco_every_sobject_summon_places( coco_sobject *sobject, coco_sobject_cl *cl,
                                 coco_sstate *sstate, 
                                 coco_bool **summon_enable,
                                 coco_u32 **summon_disable_until )
{
  struct coco_team *t = sstate->teams 
    + COCO_SOBJECT_NATIONALITY( sobject ) - COCO_GOOD_GUYS;
  *summon_enable = &(t->summon_enable);
  *summon_disable_until = &(t->summon_disable_until);
}

coco_bool
coco_every_sobject_is_visible( coco_sobject *sobject,
                               coco_sobject_cl *cl,
                               coco_sstate *sstate, 
                               coco_gnum cam_left,
                               coco_gnum cam_right,
                               coco_sobject *cam_or_null )
{
  coco_gnum obj_x = COCO_SOBJECT_X( sobject );
  coco_gnum obj_w = COCO_SOBJECT_W( sobject );
  return (cam_left <= (obj_x+obj_w)) && (obj_x < cam_right);
}

coco_bool
coco_sndobject_is_visible( coco_sobject *sobject,
                           coco_sobject_cl *cl,
                           coco_sstate *sstate, 
                           coco_gnum cam_left,
                           coco_gnum cam_right,
                           coco_sobject *cam_or_null )
{
  coco_gnum obj_x = COCO_SOBJECT_X( sobject );
  int audible = COCO_SNDOBJECT_AUDIBLE_TO_PLAYER( sobject );
  if (cam_or_null && (audible >= 0))
    return audible == COCO_SOBJECT_CAMERA_INDEX( cam_or_null );
  else if (cam_or_null)
    {
      coco_gnum r = coco_game_fx_radius[COCO_SNDOBJECT_SOUND( sobject )];
      coco_gnum d = abs( obj_x - COCO_SOBJECT_X( cam_or_null ) );
      return r >= d;
    }
  else return (cam_left <= obj_x) && (obj_x <= cam_right);
}

coco_bool
coco_flying_player_name_is_visible( coco_sobject *sobject,
                                    coco_sobject_cl *cl,
                                    coco_sstate *sstate, 
                                    coco_gnum cam_left,
                                    coco_gnum cam_right,
                                    coco_sobject *cam_or_null )
{
  coco_bool is_comp, is_comp_same, p_or_c_index_same, nat_same;
  int p_or_c_index;
  /* call-next-method */
  if (!cam_or_null 
      || !coco_sobject_is_visible( sobject, cl->super, sstate, cam_left, 
                                   cam_right, cam_or_null ))
    return COCO_FALSE;
  is_comp = (p_or_c_index = COCO_SOBJECT_COMP_INDEX( cam_or_null )) >= 0;
  if (!is_comp) p_or_c_index = COCO_SOBJECT_CAMERA_INDEX( cam_or_null );
  is_comp_same 
    = COCO_FLYING_PLAYER_NAME_IS_COMP( sobject ) == is_comp;
  p_or_c_index_same
    = COCO_FLYING_PLAYER_NAME_P_OR_C_INDEX( sobject ) == p_or_c_index;
  nat_same
    = (COCO_SOBJECT_NATIONALITY( sobject )
       == COCO_SOBJECT_NATIONALITY( cam_or_null ));
  return (!(is_comp_same && p_or_c_index_same))
            && (nat_same
                ||coco_sobject_is_obscured( cam_or_null, sstate ) < 2 );
}


coco_mini_point
coco_sobject_weapon_hardpoint_by_raster_index( coco_sobject_cl *cla, 
                                               unsigned char ri )

{
  coco_mini_point *hps = cla->weapon_hardpoints;
  if (hps)
    return hps[ri];
  else
    {
      struct coco_raster *r = cla->rasters[ri];
      return COCO_MAKE_MINI_POINT( r->w>>1, r->h>>1 );
    }
}



coco_sobject *
coco_every_sobject_emit_smoke( coco_sobject *sobject, coco_sobject_cl *cla,
                               coco_sstate *sstate )
{
  coco_mini_point mp = coco_sobject_com( sobject );
  return coco_emit_smoke_helper( sobject, sstate, 
                                 COCO_MINI_POINT_X( mp ) 
                                 + COCO_SOBJECT_X( sobject ),
                                 COCO_MINI_POINT_Y( mp ) 
                                 + COCO_SOBJECT_Y( sobject ));
}


coco_sobject *
coco_copter_emit_smoke( coco_sobject *copter, coco_sobject_cl *cla,
                        coco_sstate *sstate )
{
  coco_gnum x, y;
  coco_mini_rectangle hp, rp;
  unsigned char ri;
  ri = COCO_SOBJECT_RASTER_INDEX( copter );
  hp = COCO_SOBJECT_WEAPON_HARDPOINTS( copter )[ri];
  rp = COCO_SOBJECT_ROTOR_POINTS( copter )[ri];
  /* emit smoke between hardpoint and rotorpoint. */
  x = COCO_SOBJECT_X( copter )
    + ((COCO_MINI_POINT_X( hp ) + COCO_MINI_POINT_X( rp ))>>1) ;
  y = COCO_SOBJECT_Y( copter ) 
    + ((COCO_MINI_POINT_Y( hp ) + COCO_MINI_POINT_Y( rp ))>>1) ;
  return coco_emit_smoke_helper( copter, sstate, x, y );
}



/* ____ CENTER OF MASS _______________________________________ */

coco_mini_point
coco_sobject_com( coco_sobject *sobject )
{
  unsigned char ri = COCO_SOBJECT_RASTER_INDEX( sobject );
  coco_mini_point *coms = COCO_SOBJECT_CENTER_OF_MASS( sobject );
  if (ri >= COCO_SOBJECT_RASTER_COUNT( sobject ))
    {
      if (ri!=0) coco_warn_printf( "Raster index out of bounds\n" );
      return COCO_MAKE_MINI_POINT( COCO_SOBJECT_CANONICAL_W( sobject )>>1,
                                   COCO_SOBJECT_CANONICAL_H( sobject )>>1 );
    }
  else if (coms)
    return coms[ri];
  else
    {
      struct coco_raster *r = COCO_SOBJECT_RASTER( sobject, ri );
      return COCO_MAKE_MINI_POINT( r->w>>1, r->h>>1 );
    }
}

coco_mini_point
coco_sobject_com_by_raster_index( coco_sobject_cl *cla, unsigned char ri )
{
  coco_mini_point *coms = cla->center_of_mass;
  if (ri >= cla->raster_count)
    {
      if (ri!=0) coco_warn_printf( "Raster index out of bounds\n" );
      return COCO_MAKE_MINI_POINT( cla->canonical_w>>1, cla->canonical_h>>1 );
    }
  else if (coms)
    return coms[ri];
  else
    {
      struct coco_raster *r = cla->rasters[ri];
      return COCO_MAKE_MINI_POINT( r->w>>1, r->h>>1 );
    }
}

coco_mini_point
coco_cobject_com( coco_cobject *cobject )
{
  unsigned char ri = COCO_COBJECT_RASTER_INDEX( cobject );
  coco_sobject_cl *cla = COCO_COBJECT_CL( cobject );
  coco_mini_point *coms = cla->center_of_mass;
  if (ri >= COCO_COBJECT_CL( cobject )->raster_count)
    {
      if (ri!=0) coco_warn_printf( "Raster index out of bounds\n" );
      return COCO_MAKE_MINI_POINT( cla->canonical_w>>1, cla->canonical_h>>1 );
    }
  else if (coms)
    return coms[COCO_COBJECT_RASTER_INDEX( cobject )];
  else
    {
      struct coco_raster *r = COCO_COBJECT_RASTER( cobject );
      return COCO_MAKE_MINI_POINT( r->w>>1, r->h>>1 );
    }
}

/* syntactic sugar for the new functions */
void
coco_center_of_mass_obsolete( coco_sobject *sobject, 
                              coco_sobject_cl *cl,
                              unsigned char raster_index,
                              coco_gnum *x_return,
                              coco_gnum *y_return )
{
  coco_mini_point pt
    = coco_sobject_com_by_raster_index( COCO_SOBJECT_CL( sobject ),
                                        raster_index );
  *x_return = COCO_MINI_POINT_X( pt );
  *y_return = COCO_MINI_POINT_Y( pt );
}


/* __END CENTER OF MASS _______________________________________ */
