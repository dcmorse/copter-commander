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

/* openal-sound.c -- OpenAL implementation of api in sound.h. */

#include <stdlib.h> /* Just for NULL, of all things */
#include <unistd.h> /* temp - for sleep() */
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <assert.h>
#include "coco.h"

#define COCO_SOUND_NSOURCES 8 /* changed from 32 for the hell of it -dm 01-31-2001 */

coco_u32 coco_sound_time = 0;


static ALuint coco_sources[COCO_SOUND_NSOURCES];
/* relies on ALuint 0 == no sound.  IIRC that is correct. */
struct { ALuint sound; coco_sound_type type; coco_u32 last_use; } 
coco_current_source_info[COCO_SOUND_NSOURCES]
 = { { 0, 0, 0 } };



static ALCdevice *device = NULL;
static void *context = NULL; /* ALCcontext * in doc but void * in headers!  */

#define COCO_GNUM_TO_ALFLOAT( x ) ((ALfloat) (((coco_gnum) (x)) * 0.01))

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
  if (context)
    {
      alcDestroyContext( context );
      context = NULL;
    }
  if (device)
    {
      alcCloseDevice( device );
      device = NULL;
    }
}



coco_bool 
coco_load_sound( char *name, coco_sound_type type, coco_sound *sound_return )
{
  char *file_name;
  void *wav;
  ALuint buffid;
  ALsizei size;
  ALsizei bits;
  ALsizei freq;
  ALsizei format;
  ALboolean err;

  /*
    loadit( "/home/dm/coco/boom.wav" );
    return COCO_TRUE; */

  if (!context) return COCO_FALSE;
  
  file_name = coco_sound_name_to_file_name( name );
  alGenBuffers( 1, &buffid );
  err = alutLoadWAV( file_name, &wav, &format, &size, &bits, &freq );
  coco_free( file_name );
  if (AL_FALSE == err) 
    return COCO_FALSE;
  alBufferData( buffid, format, wav, size, freq );
  free( wav ); /* openal makes a local copy of wave data */

  *sound_return = buffid;
  return COCO_TRUE;
}

void 
coco_unload_sound( coco_sound sound )
{
  if (!context) return;
#ifdef COCO_DEBUG
  if (!alIsBuffer( sound ))
    coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                       "coco_sound %d not alIsBuffer().\n", sound );
  else
#endif
  alDeleteBuffers( 1, &sound );
}



/* is there something I'm missing, or are my speakers swapped? */
/* Your speakers were swapped, fool! -dm 03-21-2001
 * #define COCO_AL_SOURCE_X( x ) \
 * ((8.0 * (float) (((COCO_OFFICIAL_WINDOW_W>>1) - (x)) << 1)) \
 * / COCO_OFFICIAL_WINDOW_W) */
#define COCO_AL_SOURCE_X( x ) \
 ((8.0 * (float) (((x) - (COCO_OFFICIAL_WINDOW_W>>1)) << 1)) \
 / COCO_OFFICIAL_WINDOW_W)


#define COCO_AL_SOURCE_Y( y ) \
  ((8.0 * (float) (y))/COCO_VIEWABLE_H)

void
coco_play_sound_from_source_index( int source_index, coco_sound sound,
                                   coco_sound_type type,
                                   coco_i32 x, coco_i32 y )
{
  ALuint source = coco_sources[source_index];
  alSource3f( source, AL_POSITION, 
              COCO_AL_SOURCE_X( x ),
              COCO_AL_SOURCE_Y( y ), 0.0 );
  if (sound != coco_current_source_info[source_index].sound)
    alSourcei( source, AL_BUFFER, sound );
  alSourcePlay( source );

  /* update cache */
  coco_current_source_info[source_index].sound = sound;
  coco_current_source_info[source_index].type = type;
  coco_current_source_info[source_index].last_use = coco_sound_time;
  if (coco_sound_time >= COCO_U32_MAX)
    {
      /* we've wrapped around.  lets still play sound but not worth
         repairing the cache perfectly. */
      int i;
      coco_sound_time = 0;
      /* imperect but all sounds played in past: */
      for (i=0; i<COCO_SOUND_NSOURCES; i++)
        coco_current_source_info[source_index].last_use = 0; 
    }
  else coco_sound_time++;
}

coco_bool
coco_source_is_playing( ALuint source )
{
  ALint enumerant;
  alGetSourceiv( source, AL_SOURCE_STATE, &enumerant );
  return AL_PLAYING == enumerant;
}


/* returns an index into coco_sources or -1 on fail */
int
coco_find_loaded_source( coco_sound sound, coco_sound_type type )
{
  int i;
  for (i=0; i<COCO_SOUND_NSOURCES; i++)
    if ((coco_current_source_info[i].sound == sound)
        && !coco_source_is_playing( coco_sources[i] ))
      return i;
  return -1;
}

/* This implementation finds the oldest idle source and uses that. 
int
coco_find_clobberable_source( coco_sound sound, coco_sound_type type )
{
  int i, oldest;
  oldest = -1;
  for (i=0; i<COCO_SOUND_NSOURCES; i++)
    if ((!coco_source_is_playing( coco_sources[i] ))
        && ((oldest < 0)
            || (coco_current_source_info[i].last_use < 
                coco_current_source_info[oldest].last_use)))
      oldest = i;
  return oldest;
}
*/


/* This implementation tries to preferentially clobber sounds that
   already have multiple sources.  Failing that it find the oldest
   idle source and uses that.  */
int
coco_find_clobberable_source( coco_sound sound, coco_sound_type type )
{
  int i, oldest;
  coco_bool playing[COCO_SOUND_NSOURCES];
  unsigned char nfree[COCO_GAME_SOUNDS_SIZE] = { 0 };
  for (i=0; i<COCO_SOUND_NSOURCES; i++)
    if ((!(playing[i] = coco_source_is_playing( coco_sources[i] )))
        && (coco_current_source_info[i].sound != 0)
        && (coco_current_source_info[i].type == type))
      {
        int n = nfree[type];
        if (n>1)
          return i;
        else 
          nfree[type] = 1;
      }
  oldest = -1;
  for (i=0; i<COCO_SOUND_NSOURCES; i++)
    if ((!playing[i])
        && ((oldest < 0)
            || (coco_current_source_info[i].last_use < 
                coco_current_source_info[oldest].last_use)))
      oldest = i;
  return oldest;
}


void
coco_play_sound( coco_sound sound, coco_sound_type type, 
                 coco_i32 x, coco_i32 y )
{
  int i;
  static coco_bool not_warned = COCO_TRUE;

  if (!context) return;
  if (mute) return;

  i = coco_find_loaded_source( sound, type );
  if (i<0)
    i = coco_find_clobberable_source( sound, type );
  if (i<0)
    {
      if (not_warned)
        {
          not_warned = COCO_FALSE;
          coco_warn_printf( "Couldn't find_clobberable_source for sound %d "
                            "type %d\n.  Message will not be repeated.\n", 
                            sound, type );
        }
      return;
    }
  coco_play_sound_from_source_index( i, sound, type, x, y );
}

void 
coco_init_sound_system( void )
{
  ALCenum current;
  static int attrlist[] = { ALC_FREQUENCY, 22050, 0 };
  assert( NULL == context );
  coco_cookie( "alcOpenDevice( NULL )" );
  device = alcOpenDevice( NULL );
  if (NULL == device)
    {
      coco_warn_printf( "Couldn't detect default OpenAL device.  "
                        "No sound for you.\n" );
      return;
    }
  coco_cookie( "...ok.\n" );
  coco_cookie( "alcCreateContext( %p, [ ALC_FREQUENCY, 22050, 0 ] )",
               device );
  context = alcCreateContext( device, attrlist );
  if (NULL==context)
    {
      coco_warn_printf( "Couldn't create OpenAL context.  "
                        "No sound for you.\n" );
      return;
    }
  coco_cookie( "...ok.\n" );
  current = alcMakeContextCurrent( context );
  if (current != ALC_NO_ERROR)
    {
      coco_warn_printf( "Couldn't make OpenAL context at %p current.  "
                        "No sound for you.\n", context );
      return;
    }
  { /* init listener */
    ALfloat zeroes[] = { 0.0f, 0.0f,  0.0f };
    ALfloat back[]   = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    alListener3f( AL_POSITION, 
                  COCO_GNUM_TO_ALFLOAT( COCO_OFFICIAL_WINDOW_W>>1 ),
                  COCO_GNUM_TO_ALFLOAT( COCO_VIEWABLE_H>>1 ),  
                  COCO_GNUM_TO_ALFLOAT( COCO_OFFICIAL_WINDOW_W>>1 ) );
    coco_warn_printf( "FIXME - OPENAL SOUND SCALING IS IS NOT RIGHT\n" );
    alListener3f( AL_POSITION, 
                  COCO_AL_SOURCE_X( COCO_OFFICIAL_WINDOW_W>>1 ),
                  COCO_AL_SOURCE_Y( COCO_OFFICIAL_WINDOW_W>>1 ),
                  (ALfloat) 3 );
    alListenerfv( AL_POSITION, zeroes );
    alListenerfv(AL_VELOCITY, zeroes );
    alListenerfv(AL_ORIENTATION, back );
  }
  coco_cookie( "OpenAL listener initialization complete.\n" );

  coco_cookie( "Allocating general purpose sources" );
  alGenSources( COCO_SOUND_NSOURCES, coco_sources );
  coco_cookie( "...ok\n" );
  coco_cookie( "OpenAL source allocation complete.\n" );
}

void
coco_flush_pending_plays( void )
{
}

