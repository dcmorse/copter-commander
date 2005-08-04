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


#include <math.h> /* sin and cos */
#include <GL/gl.h>
#include <string.h> 		/* memcpy and strcmp */
#include <assert.h>
#include "coco.h"
#include "rasters.h"


/* Dashboards with < than this many items will be padded in the middle. */
#define COCO_DASHBOARD_ITEM_MIN 8



#define DEFINE_AMMO_MAX_READER( name, slot ) \
 static int name( coco_sobject_cl *cl ) { return cl->slot; }


#define DEFINE_AMMO_READER( name, slot ) \
  static int name( struct coco_cstate *cstate ) \
    { return cstate->dashinfo.slot; }

DEFINE_AMMO_MAX_READER( coco_max_bullets,  max_bullets  )
DEFINE_AMMO_MAX_READER( coco_max_bombs,    max_bombs    )
DEFINE_AMMO_MAX_READER( coco_max_missiles, max_missiles )
DEFINE_AMMO_MAX_READER( coco_max_rockets,  max_rockets  )
DEFINE_AMMO_MAX_READER( coco_max_plasma,   max_plasma   )
DEFINE_AMMO_MAX_READER( coco_max_fuel,     max_fuel     )
DEFINE_AMMO_MAX_READER( coco_max_kits,     max_kits     )
/* DEFINE_AMMO_MAX_READER( coco_max_energy,   max_energy   ) not used */
DEFINE_AMMO_MAX_READER( coco_max_internal_infantry,     
                        max_internal_infantry           )

DEFINE_AMMO_READER( coco_bullets,    bullets  )
DEFINE_AMMO_READER( coco_bombs,      bombs    )
DEFINE_AMMO_READER( coco_missiles,   missiles )
DEFINE_AMMO_READER( coco_rockets,    rockets  )
/* DEFINE_AMMO_READER( coco_fuel,       fuel     ) not used */
DEFINE_AMMO_READER( coco_plasma,     plasma   )
/* DEFINE_AMMO_READER( coco_energy,     fuel     ) not used */
DEFINE_AMMO_READER( coco_funds,      funds    )
/* DEFINE_AMMO_READER( coco_copters,    copters  ) */
DEFINE_AMMO_READER( coco_kits,       kits     )
/* DEFINE_AMMO_READER( coco_kills,      kills    ) */


#define COCO_DASHBOARD_ICON_TAB 44
#define COCO_DASHBOARD_ICON_INNER_TAB 36

void 
coco_draw_dashboard_string( int x, char *str )
{
  coco_draw_string( x + COCO_DASHBOARD_ICON_TAB, COCO_DASHBOARD_BASELINE,
                    str, -1, &coco_dashboard_font, COCO_BASELINE );
}

void 
coco_draw_dashboard_numeric( int x, struct coco_cstate *cstate,
                             void *handy1, void *handy2 )
{
  char buf[16];
  int num;
  coco_color color;
  int (* numfn)( struct coco_cstate * ) 
    = (int (*)( struct coco_cstate * )) handy1;
  int (* maxfn)( coco_sobject_cl * ) 
    = (int (*)( coco_sobject_cl * )) handy2;
  coco_i32 camera_class_index = cstate->dashinfo.camera_class_index;
  coco_sobject_cl *cl = (camera_class_index>=0)
    ? (coco_sobject_classes + camera_class_index) : NULL;
  assert( numfn );
  num = numfn( cstate );
  color = ((!maxfn) || (num < maxfn( cl )))
    ? COCO_COLOR_DASHBOARD_FG 
    : COCO_COLOR_DASHBOARD_FULL;
  sprintf( buf, "%d", (int) num );
  coco_draw_color( color );
  coco_draw_dashboard_string( x, buf );
}

void
coco_draw_dashboard_raster( int x, void *handy )
{
  struct coco_raster *r = handy;
#ifdef COCO_DEBUG
  assert( handy );
  if (COCO_DASHBOARD_ICON_INNER_TAB < r->w)
    coco_warn_printf( "Raster (%dx%d) is wider than the max "
                      "for standard dashboard icons\n", r->w, r->h );
#else
  if (!handy) return;
#endif /* COCO_DEBUG */
  coco_draw_raster( x + COCO_DASHBOARD_ICON_INNER_TAB - r->w,
                    COCO_DASHBOARD_BASELINE, r );
}

void 
coco_draw_dashboard_copters( int x, struct coco_cstate *cstate,
                             void *ignore1, void *ignore2 )
{
  char buf[50];
  coco_sobject_cl *cl = coco_sobject_classes
    + cstate->dashinfo.fancy_class_index;
  assert( cl->dashboard_icon );
  coco_draw_dashboard_raster( x, cl->dashboard_icon );
  coco_draw_color( (cstate->dashinfo.copters > 0)
		   ? COCO_COLOR_DASHBOARD_FULL
		   : COCO_COLOR_DASHBOARD_FG );
  if (cstate->dashinfo.kills)
  sprintf( buf, "%d (%d/%d)", 
	   (int) cstate->dashinfo.copters, 
	   (int) cstate->dashinfo.kills,
	   COCO_NKILLS_FOR_FANCY_COPTER);
  else
  sprintf( buf, "%d", 
	   (int) cstate->dashinfo.copters );
  coco_draw_dashboard_string( x, buf );

}


void 
coco_draw_dashboard_fuel( int x, struct coco_cstate *cstate,
                          void *ignore1, void *ignore2 )
{
  double angle;
  float length;
  coco_gnum x0, y0;
  char buf[16];
  coco_u32 fuel = cstate->dashinfo.fuel;
  coco_i32 cla_idx = cstate->dashinfo.camera_class_index;
  coco_sobject_cl *cla = (cla_idx < 0) 
    ? NULL : (coco_sobject_classes + cla_idx);
  int seconds;

  /* figure out draw color */
  if (cla
      && coco_sobject_subtype_p( cla, COCO_COPTER_CLASS )
      && ((cstate->board_w<<1) >= (cla->dx_max * cstate->dashinfo.fuel)))
    coco_draw_color( ((cstate->board_w >= (cla->dx_max * fuel))
                      && ((coco_draw_frame_number&6)>1))
                     ? COCO_COLOR_RED
                     : COCO_COLOR_YELLOW );
  else
    coco_draw_color( (cla && (fuel >= cla->max_fuel)) 
                     ? COCO_COLOR_DASHBOARD_FULL
                     : COCO_COLOR_DASHBOARD_FG );
  /* draw needle onto fuel guage */
  angle = (cla && (cla->max_fuel>0))
    ? COCO_PI * (1.0 - (((double) cstate->dashinfo.fuel) 
                        / ((double) cla->max_fuel)))
    : COCO_PI;
  length = COCO_FUEL_ICON_RASTER_H - 2;
  /* 1=fudge to make it look right */
  x0 = x + COCO_DASHBOARD_ICON_INNER_TAB - (COCO_FUEL_ICON_RASTER_W>>1) - 1;
  /* 4=height of dot above bottom of raster: */
  y0 = COCO_DASHBOARD_BASELINE + 4;
  coco_draw_line( x0,
                  y0, 
                  x0 + length*cos( angle ),
                  y0 + length*sin( angle ) );


  /* draw boring old string */
  seconds = fuel / cstate->dashinfo.server_fps;
  sprintf( buf, "%d:%02d", seconds / 60, seconds % 60 );
  coco_draw_dashboard_string( x, buf );
}

void
coco_draw_dashboard_energy( int x, struct coco_cstate *cstate,
                            void *ignore1, void *ignore2 )
{
  /* we've got this raster of a battery, and are going to fill it up with
     power, relative to total charge.  The battery is a raster.  It has
     some wierd geometry about it.  I could encapsulate that with a bunch
     of deines, but that doesn't seem worth the time. */
  coco_i32 cla_idx = cstate->dashinfo.camera_class_index;
  coco_sobject_cl *cla = (coco_sobject_classes + cla_idx);
  coco_u16 energy = cstate->dashinfo.fuel;
  coco_u16 emax = cla->max_energy;
  int x0, x1;
  int fract = energy * 255 / emax;
  int dif = 255-fract;
  coco_color color 
    = COCO_MAKE_COLOR( (int)(sqrt( dif )*16), (int)(sqrt( fract )*16),
                       0, 255 );
  char buf[16];
  sprintf( buf, "%d", energy*100/emax );
  coco_draw_color( color );
  coco_draw_dashboard_string( x, buf );
  x0 = x + COCO_DASHBOARD_ICON_INNER_TAB - COCO_ENERGY_ICON_RASTER_W + 1;
  x1 = x0+((COCO_ENERGY_ICON_RASTER_W-4)*energy/emax);
  if (x1>x0)
    {
      coco_draw_rectangle( x0, COCO_DASHBOARD_BASELINE+1, x1,
                           COCO_DASHBOARD_BASELINE
                           +COCO_ENERGY_ICON_RASTER_H-1 );
      if (emax==energy) /* fill the nipple */
        coco_draw_rectangle( x0+COCO_ENERGY_ICON_RASTER_W-4, 
                             COCO_DASHBOARD_BASELINE+4,
                             x0+COCO_ENERGY_ICON_RASTER_W-1,
                             COCO_DASHBOARD_BASELINE+8 );
    }
  coco_draw_dashboard_raster( x, &coco_energy_icon_raster );
}


void 
coco_draw_dashboard_men( int x, struct coco_cstate *cstate,
                          void *ignore1, void *ignore2 )
{
  char str[16];
  int engitroopers = cstate->dashinfo.engitroopers;
  int paratroopers = cstate->dashinfo.paratroopers;
  coco_i32 cci = cstate->dashinfo.camera_class_index;
  coco_color color;
  if (engitroopers)
    sprintf( str, "%d+%d", paratroopers, engitroopers );
  else sprintf( str, "%d", paratroopers );
  if (engitroopers
      ||((cci>=0)
         && (coco_sobject_classes[cci].max_internal_infantry == paratroopers)))
    color = COCO_COLOR_DASHBOARD_FULL;
  else color = COCO_COLOR_DASHBOARD_FG;
  coco_draw_color( color );
  coco_draw_dashboard_string( x, str );
}


static struct coco_dashboard_item 
coco_funds_dashboard_item = { 
    "funds", COCO_DASHBOARD_RIGHT,
    coco_draw_dashboard_raster, (void *) &coco_money_icon_raster,
    coco_draw_dashboard_numeric, (void *) coco_funds, NULL
};

static struct coco_dashboard_item 
coco_copters_dashboard_item = {   
    "copters", COCO_DASHBOARD_RIGHT,
    NULL, NULL,
    coco_draw_dashboard_copters, NULL, NULL
};

/* (left side) empty item, used to consume space */
static struct coco_dashboard_item 
coco_left_spacer_dashboard_item = { 
  "left-spacer", COCO_DASHBOARD_LEFT,
  NULL, NULL, NULL, NULL, NULL
};

/* (right side) empty item, used to consume space  -- not used
static struct coco_dashboard_item 
coco_right_spacer_dashboard_item = { 
  "right-spacer", COCO_DASHBOARD_RIGHT,
  NULL, NULL, NULL, NULL, NULL
};
*/

#define COCO_DASHBOARD_STATIC_COUNT 2

static struct coco_dashboard_item *
coco_static_dashboard_items[COCO_DASHBOARD_STATIC_COUNT] = {
  &coco_copters_dashboard_item,
  &coco_funds_dashboard_item
};

 
int
coco_dashboard_items_internal( coco_sobject_cl *cl_or_null, 
                               struct coco_dashboard_item **di_return )
{
  if (cl_or_null)
    {
      int n = 0;
      int (* fn)( coco_sobject_cl *, struct coco_dashboard_item ** ) = NULL;
      for (; cl_or_null; cl_or_null = cl_or_null->super)
        {
          int (* newfn)( coco_sobject_cl *, struct coco_dashboard_item ** );
          newfn = cl_or_null->gen_dashboard_items;
          if (newfn != fn)
            {
              fn = newfn;
              if (newfn) n += newfn( cl_or_null,
                                     di_return ? (di_return + n) : NULL );
            }
        }
      return n 
        + coco_dashboard_items_internal( NULL, 
                                         di_return ? (di_return + n) : NULL );
    }
  else 
    {
      if (di_return)
        memcpy( di_return, coco_static_dashboard_items,
                COCO_DASHBOARD_STATIC_COUNT
                * sizeof( struct coco_dashboard_item * )  );
      return COCO_DASHBOARD_STATIC_COUNT;
    }

}

int
coco_dashboard_items( coco_sobject_cl *cl_or_null, 
                      struct coco_dashboard_item **di_return )
{
  int result = coco_dashboard_items_internal( cl_or_null, di_return );
  int i;
  
  if (di_return)
    for (i=0; i<result>>1; i++)
      {
        int j = result-i-1;
        struct coco_dashboard_item *tmp;      
        tmp = di_return[i];
        di_return[i] = di_return[j];
        di_return[j] = tmp;
      }
  return result;
}





#define COCO_DASHBOARD_COPTER_COUNT 7

int
coco_copter_dashboard_items( coco_sobject_cl *cl,
                             struct coco_dashboard_item **di_return )
{
  static struct coco_dashboard_item items[COCO_DASHBOARD_COPTER_COUNT] = {
    { "men", COCO_DASHBOARD_RIGHT,
      coco_draw_dashboard_raster, (void *) &coco_man_icon_raster,
      coco_draw_dashboard_men, NULL, (void *) coco_max_internal_infantry
    },
    { "fuel", COCO_DASHBOARD_RIGHT,
      coco_draw_dashboard_raster, (void *) &coco_fuel_icon_raster,
      coco_draw_dashboard_fuel, NULL, (void *) coco_max_fuel
    },
    { "rockets", COCO_DASHBOARD_LEFT,
      coco_draw_dashboard_raster, (void *) &coco_rocket_icon_raster,
      coco_draw_dashboard_numeric,
      (void *) coco_rockets,
      (void *) coco_max_rockets
    },
    { "missiles", COCO_DASHBOARD_LEFT,
      coco_draw_dashboard_raster, (void *) &coco_missile_icon_raster,
      coco_draw_dashboard_numeric,
      (void *) coco_missiles,
      (void *) coco_max_missiles
    },
    { "kits", COCO_DASHBOARD_LEFT,
      coco_draw_dashboard_raster, (void *) &coco_kit_icon_raster,
      coco_draw_dashboard_numeric,
      (void *) coco_kits,
      (void *) coco_max_kits
    },
    { "bombs", COCO_DASHBOARD_LEFT,
      coco_draw_dashboard_raster, (void *) &coco_bomb_icon_raster,
      coco_draw_dashboard_numeric, 
      (void *) coco_bombs,
      (void *) coco_max_bombs
    },
    { "guns", COCO_DASHBOARD_LEFT,
      coco_draw_dashboard_raster, (void *) &coco_gun_icon_raster,
      coco_draw_dashboard_numeric, 
      (void *) coco_bullets,
      (void *) coco_max_bullets
    }
  };
  int i=0, j=0;
  for (; i<COCO_DASHBOARD_COPTER_COUNT; i++)
    {
      int (* maxfn)( coco_sobject_cl * ) 
        = (int (*)( coco_sobject_cl * )) items[i].dynamic_handy2;
      assert( maxfn );
      if (maxfn( cl ) != 0)
        {
          if (di_return)
            di_return[j] = (items + i);
          j++;
        }
      else if (!(0==strcmp( items[i].name, "missiles" )
                 || (0==strcmp( items[i].name, "kits" ))))
        { 
          coco_cookie( "not skipping item %s\n", items[i].name );
          if (di_return)
            di_return[j] = (&coco_left_spacer_dashboard_item);
          j++;
        }
      else coco_cookie( "skipping item %s\n", items[i].name );

    }
  return j;
}

#define COCO_DASHBOARD_MECH_COUNT 2

int
coco_mech_dashboard_items( coco_sobject_cl *cl,
                           struct coco_dashboard_item **di_return )
{
  static struct coco_dashboard_item items[COCO_DASHBOARD_COPTER_COUNT] = {
    { "energy", COCO_DASHBOARD_RIGHT,
      NULL, NULL,
      coco_draw_dashboard_energy, NULL, NULL
    },
    { "plasma", COCO_DASHBOARD_LEFT,
      coco_draw_dashboard_raster, (void *) &coco_plasma_icon_raster,
      coco_draw_dashboard_numeric, 
      (void *) coco_plasma,
      (void *) coco_max_plasma
    },
  };
  int i;
  if (di_return)
    for (i=0; i<COCO_DASHBOARD_MECH_COUNT; i++)
      di_return[i] = items + i;
  return COCO_DASHBOARD_MECH_COUNT;
}


void
coco_draw_dashboard( struct coco_cstate *cstate )
{
  static unsigned int null_dashboard_list = 0;
  int i, l, r, xinc, mitems;
  coco_i32 camera_class_index = cstate->dashinfo.camera_class_index;
  coco_sobject_cl *cl = (camera_class_index>=0)
    ? (coco_sobject_classes + camera_class_index) : NULL;
  unsigned int *list_addr = cl ? &(cl->dashboard_list) : &null_dashboard_list;
  unsigned int list = *list_addr;
  struct coco_dashboard_item **items;
  int nitems = cl ? cl->dashboard_nitems : -1;
  if (nitems<0)
    {
      int other_nitems;
      nitems = coco_dashboard_items( cl, NULL );
      items = (struct coco_dashboard_item **)
        coco_malloc( nitems * sizeof( struct coco_dashboard_item * ) );
      if (cl) 
        {
          cl->dashboard_nitems = nitems;
          cl->dashboard_items = items;
        }
      other_nitems = coco_dashboard_items( cl, items );
      assert( other_nitems = nitems );
    }
  else items = cl->dashboard_items;

  mitems = COCO_MAX( nitems, COCO_DASHBOARD_ITEM_MIN );
  xinc = COCO_OFFICIAL_WINDOW_W / mitems;

  /* draw static */
  if (list)
    glCallList( list );
  else
    {
      static coco_bool listless = COCO_FALSE;
      if (!listless)
        *list_addr = list = glGenLists( 1 );
      if (!list && !listless)
        {
          listless = COCO_TRUE;
          coco_warn_printf( "Unable to glGenList.  Dealing with it.\n" );
        }
      else if (list)
        glNewList( list, GL_COMPILE_AND_EXECUTE );
      l = 0;
      r = 0;
      for (i=0; i<nitems; i++)
        {
          void (* draw_static)( int x, void *handy ) = items[i]->draw_static;
          int notch = (items[i]->pack == COCO_DASHBOARD_LEFT)
            ? l++ : (mitems - ++r);
          assert( notch >= 0 );
          assert( notch < mitems );
          if (NULL != draw_static)
            draw_static( xinc*notch, items[i]->static_handy );
        }
      if (list)
        glEndList();
    }

  /* draw dynamic */
  l = 0;
  r = 0;
  for (i=0; i<nitems; i++)
    {
      int notch = (items[i]->pack == COCO_DASHBOARD_LEFT)
        ? l++ : (mitems - ++r);
      assert( notch >= 0 );
      assert( notch < mitems );
      if (items[i]->draw_dynamic)
        items[i]->draw_dynamic( xinc*notch, cstate,
                                items[i]->dynamic_handy1, 
                                items[i]->dynamic_handy2 );
    }
}

