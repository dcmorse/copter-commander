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

/* openal-sound.c -- OpenAL implementation of api in sound.h. */

#include <gnome.h>
#include <esd.h>
#include <stdlib.h> /* Just for NULL, of all things */
#include <unistd.h> /* temp - for sleep() */
#include <assert.h>
#include "coco.h"

#define COCO_GNOME_SOUND_INITIALIZED() (gnome_sound_connection >= 0) 

static coco_bool mute = COCO_FALSE;

void 
coco_set_mute( coco_bool on )
{
  mute = on;
}

coco_bool 
coco_get_mute( void )
{
  return mute;
}


void 
coco_exit_sound_system( void )
{
}

coco_bool 
coco_load_sound( char *name, coco_sound *sound_return )
{
  char *file_name;
  int sampleid;

  if (!COCO_GNOME_SOUND_INITIALIZED()) return COCO_FALSE;
  
  file_name = coco_sound_name_to_file_name( name );

  sampleid = gnome_sound_sample_load( name, file_name );
  if (sampleid < 0) 
    return COCO_FALSE;
  else
    {
      *sound_return = sampleid; /* cast int to unsigned int, cross fingers */
      return COCO_TRUE;
    }
}

void 
coco_unload_sound( coco_sound sound )
{
  esd_sample_free( gnome_sound_connection, sound );
}

void
coco_play_sound( coco_sound sound, coco_i32 x, coco_i32 y )
{
  if (mute) return;
  esd_sample_play( gnome_sound_connection, sound );
  fsync( gnome_sound_connection ); /* magic thanks to gnome-sound.c */
}

void 
coco_init_sound_system( void )
{
  if (!coco_gnome)
    {
      coco_warn_printf( "esd sound without gnome isn't really supported,\n"
                        "because there has to be an outer gnome loop.\n"
                        "So that's why you get the next warning:\n");
    }
  if (!COCO_GNOME_SOUND_INITIALIZED())
    {
      coco_warn_printf( "Initializing sounds failed: "
                        "gnome_sound_connection = %d.\n", 
                        gnome_sound_connection );
    }
}

void
coco_flush_pending_plays( void )
{
}


