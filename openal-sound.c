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

#include <stdlib.h> /* Just for NULL, of all things */
#include <unistd.h> /* temp - for sleep() */
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <assert.h>
#include "coco.h"

static void *context = NULL;

#define COCO_GNUM_TO_ALFLOAT( x ) ((ALfloat) (((coco_gnum) (x)) * 0.01))

#define COCO_SOUND_NSOURCES 8 /* changed from 32 for the hell of it -dm 01-31-2001 */

void coco_play_sound_immediately( coco_sound sound, coco_i32 x, coco_i32 y );

static ALuint coco_sources[COCO_SOUND_NSOURCES];
static coco_bool coco_sources_allocated[COCO_SOUND_NSOURCES];
static int coco_next_source = 0;

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
}


coco_bool 
coco_load_sound( char *name, coco_sound *sound_return )
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
#define COCO_AL_SOURCE_X( x ) \
 ((8.0 * (float) (((COCO_OFFICIAL_WINDOW_W>>1) - (x)) << 1)) \
  / COCO_OFFICIAL_WINDOW_W)

#define COCO_AL_SOURCE_Y( y ) \
  ((8.0 * (float) (y))/COCO_VIEWABLE_H)


#define COCO_AL_NQUEUE_MAX 1 /* max number of distinct new sounds per turn */
static int coco_al_nqueue = 0;
struct {
  coco_sound sound;
  coco_i32 x;
  coco_i32 y;
} coco_al_queue[COCO_AL_NQUEUE_MAX];

void
coco_play_sound( coco_sound sound, coco_i32 x, coco_i32 y )
{
  /* the idea is, keep a queue of sounds, then play them in one fell swoop
     when coco_flush_pending_plays gets called.  This may keep the AL in
     cache more, rather than what we used to do, intermingling AL and GL
     calls. */
  if (coco_al_nqueue < (COCO_AL_NQUEUE_MAX-1))
    {
      coco_al_queue[coco_al_nqueue].sound = sound;
      coco_al_queue[coco_al_nqueue].x = x;
      coco_al_queue[coco_al_nqueue].y = y;
      coco_al_nqueue++;
    }
  else coco_play_sound_immediately( sound, x, y ); /* queue full, skip it */
}

void 
coco_init_sound_system( void )
{
  static int attrlist[] = { ALC_FREQUENCY, 22050, 0 };
  int i;
  for (i=0; i<COCO_SOUND_NSOURCES; i++)
    coco_sources_allocated[i] = COCO_FALSE;
  assert( NULL == context );
  context = alcCreateContext( attrlist );
  if (NULL==context)
    {
      coco_warn_printf( "Couldn't create OpenAL context.  No sound for you.\n" );
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
  alGenSources( COCO_SOUND_NSOURCES, coco_sources );
}

void
coco_flush_pending_plays( void )
{
  int i;
  if (!context) return;
  for (i=0; i<coco_al_nqueue; i++)
    coco_play_sound_immediately( coco_al_queue[i].sound, 
                                 coco_al_queue[i].x,
                                 coco_al_queue[i].y );
  coco_al_nqueue = 0;
}

coco_bool
coco_play_sound_from_source_maybe( ALuint source, coco_sound sound,
                                   coco_i32 x, coco_i32 y )
{
  ALint enumerant;
  alGetSourcei( source, AL_SOURCE_STATE, &enumerant );
  if (AL_PLAYING != enumerant)
    {
      alSource3f( source, AL_POSITION, 
                  COCO_AL_SOURCE_X( x ),
                  COCO_AL_SOURCE_Y( y ), 0.0 );
      /* alSourcefv( source, AL_ORIENTATION, back );  */
      alSourcei( source, AL_BUFFER, sound );
      /* alSourcei(  source, AL_SOURCE_LOOPING_LOKI, AL_TRUE); */
      alSourcePlay( source );
      /* talAttenuationScale(2); */
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}

void 
coco_play_sound_immediately( coco_sound sound, coco_i32 x, coco_i32 y )
{
  int i;

  if (!context) return;
  if (mute) return;

  while (1)
    for (i=coco_next_source; i<COCO_SOUND_NSOURCES; i++)
      if (coco_play_sound_from_source_maybe( coco_sources[i], sound, x, y ))
        {
          coco_next_source = ((i<(COCO_SOUND_NSOURCES-1)) ? (i+1) : 0);
          return;
        }
  for (i=0; i<coco_next_source; i++)
    if (coco_play_sound_from_source_maybe( coco_sources[i], sound, x, y ))
      {
        coco_next_source = ((i<(COCO_SOUND_NSOURCES-1)) ? (i+1) : 0);
        return;
      }
}



