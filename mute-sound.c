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


#include "coco.h"

/* mute-sound.c -- Provides dummy functions for users that don't want sound. */

void 
coco_init_sound_system( void )
{
}

void 
coco_exit_sound_system( void )
{
}

coco_bool 
coco_load_sound( char *name, coco_sound_type type, coco_sound *sound_return )
{
  static coco_sound id = 0;
  *sound_return = id++;
  return COCO_TRUE;
}

void 
coco_unload_sound( coco_sound sound )
{
}

void 
coco_play_sound( coco_sound sound, coco_sound_type type, 
                 coco_i32 x, coco_i32 y )
{ 
}

void 
coco_flush_pending_plays( void )
{
}

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

