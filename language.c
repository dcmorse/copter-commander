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

#include "coco.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h> /* for atan2, floor */
#include <sys/times.h> /* for times() */
#include <sys/types.h> /* for opendir */
#include <dirent.h>    /* for readdir */
#include <assert.h>
#include <errno.h>
#include <alloca.h>

char *coco_errno_string( int error_number )
{
  return strerror( error_number );
}


void coco_fatal_printf( int exit_code, const char *fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( stderr, fmt, ap );
  va_end( ap );
  /* *((char *)NULL) = 'X'; /+ exit w/core +/ */
  exit( exit_code );
}

void coco_warn_printf( const char *fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( stderr, fmt, ap );
  va_end( ap );
}

void coco_log_printf( const char *fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( coco_server_log, fmt, ap );
  va_end( ap );
}

/* return non-void so we can be used inside expressions */
int coco_cookie( const char *fmt, ... )
{
#ifdef COCO_DEBUG
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( stderr, fmt, ap );
  va_end( ap );
  fflush( stderr );
#endif
  return 0;
}

void *
coco_malloc( size_t size )
{
  void *ptr = malloc( size );
  if (NULL==ptr)
    coco_fatal_printf( COCO_ERROR_OUT_OF_MEMORY,
                       "Memory exhausted, malloc returned NULL.\n" );
  /* coco_cookie( "coco_malloc(%d)=>%p\n", (int) size, ptr ); */
  return ptr;
}

void *
coco_realloc( void *ptr, size_t size )
{
  ptr = realloc( ptr, size );
  if (NULL==ptr)
    coco_fatal_printf( COCO_ERROR_OUT_OF_MEMORY,
                       "Memory exhausted, malloc returned NULL.\n" );
  /* coco_cookie( "coco_realloc(%p, %d)=>%p\n", optr, (int) size, ptr ); */
  return ptr;
}


void 
coco_free( void *ptr )
{
  /* coco_cookie( "coco_free(%p)\n", ptr ); */
  free( ptr );
}

char *
coco_string_malloc_copy( char *src )
{
  char *buf = coco_malloc( strlen( src ) + 1 );
  strcpy( buf, src );
  return buf;
}

/* _Time___________________________________________________________ */

#define COCO_U32_1_4 (UINT_MAX>>2)
#define COCO_U32_3_4 (COCO_U32_1_4 + COCO_U32_1_4 + COCO_U32_1_4)

/* return true if t1 < t2 in a modulo 2^32 number system.  
   An inexact result. */
coco_bool 
time_less_than( coco_u32 t1, coco_u32 t2 )
{
  if (t1 < t2) return !((t1<COCO_U32_1_4) && (COCO_U32_3_4<t2));
  else return (t2<COCO_U32_1_4) && (COCO_U32_3_4<t1);
}

/* _BIT_VECTORS____________________________________________________ */

int 
coco_get_bit( unsigned char *bit_vector, int i )
{
  unsigned char byte = bit_vector[i>>3];
  return byte & (1 << (i&7));
}

void 
coco_set_bit( unsigned char *bit_vector, int i, int bit )
{
  int i_8 = i>>3;
  unsigned char byte = bit_vector[i_8];
  unsigned char mask = 1 << (i&7);
  bit_vector[i_8] = bit ? byte|mask : byte&~mask;
}

void
coco_clear_bit_vec( unsigned char *bit_vector, int size, int value )
{
  value = value ? 0xFF : 0;
  size = (size+7) >> 3;
  while (size)
    {
      size--;
      bit_vector[size] = value;
    }
}

void 
coco_copy_bit_vec( unsigned char *dest, unsigned char *src, int size )
{
  memcpy( dest, src, ((size+7) >> 3) * sizeof( unsigned char ) );
}


/* _Misc______________________________________________________ */

void
coco_format_ipaddr( FILE *stream, coco_u32 addr, unsigned short port, 
                    coco_bool use_port )
{
  fprintf( stream, "%d.%d.%d.%d", addr>>24, (addr>>16) & 255, 
           (addr>>8) & 255, addr &255 );
  if (use_port) fprintf( stream, ":%d", port );
}

/* this structure appears in gettimeofday etc. */
void 
coco_add_timevals( struct timeval *tv_return, 
                   struct timeval *tv1, 
                   struct timeval *tv2 )
{
  long u1 = tv1->tv_usec;
  long u2 = tv2->tv_usec;
  long usum = u1 + u2;
  long udiff = usum - 1000000;
  int greater = udiff>=0;
  tv_return->tv_sec = tv1->tv_sec + tv2->tv_sec + greater;
  tv_return->tv_usec = greater ? udiff : usum;
}


void 
coco_subtract_timevals( struct timeval *tv_return, 
                        struct timeval *tv1, 
                        struct timeval *tv2 )
{
  long u1 = tv1->tv_usec;
  long u2 = tv2->tv_usec;
  long udiff = u1 - u2;
  int nega = (udiff<0) ? 1 : 0;
  tv_return->tv_sec = tv1->tv_sec - tv2->tv_sec - nega;
  tv_return->tv_usec = udiff + (nega ? 1000000 : 0);
}

#define COCO_MAX_TIME_COOKIES 10
#define COCO_MAX_TIME_COOKIE_LABEL 30

static enum coco_time_cookie_mode coco_tc_mode = COCO_REAL;

static void
coco_real_time_cookie( char *label )
{
  static char labels[COCO_MAX_TIME_COOKIE_LABEL][COCO_MAX_TIME_COOKIES];
  static struct timeval times[COCO_MAX_TIME_COOKIES];
  static int index = 0;
  if (NULL == label)
    {
      int i;
      struct timeval total;
      coco_subtract_timevals( &total, times+index-1, times );
      for (i = index-1; i>=1; i--)
        coco_subtract_timevals( times+i, times+i, times+i-1 );
      for (i=1; i<index; i++)
        coco_cookie( "%s-%s %6d  ", labels[i-1], labels[i], times[i].tv_usec );
      coco_cookie( "TOTAL %d.%6d\n", total.tv_sec, total.tv_usec );
      index = 0;
    }
  else if (index < COCO_MAX_TIME_COOKIES)
    {
      int string_size;
      gettimeofday( times + index, NULL);
      string_size = strlen( label )+1;
      memcpy( labels[index], label, 
              COCO_MIN( COCO_MAX_TIME_COOKIE_LABEL-1, string_size ) );
      labels[index][COCO_MAX_TIME_COOKIE_LABEL-1] = '\0';
      index++;
    }
}


static void
coco_sys_user_time_cookie( char *label )
{
  static char labels[COCO_MAX_TIME_COOKIE_LABEL][COCO_MAX_TIME_COOKIES];
  static clock_t timez[COCO_MAX_TIME_COOKIES];
  static int index = 0;
  if (NULL == label)
    {
      struct tms tms;
      int i;
      clock_t total;
      times( &tms );
      total = timez[index-1] - timez[0];
      for (i = index-1; i>=1; i--)
        timez[i] = timez[i] - timez[i-1];
      for (i=1; i<index; i++)
        coco_cookie( "%s-%s %6d  ", labels[i-1], labels[i], (int) timez[i] );
      coco_cookie( "TOTAL %6d current %d\n",
                   (int) total,
                   (int) ((coco_tc_mode==COCO_SYS) 
                          ? tms.tms_stime : tms.tms_utime));
      index = 0;
    }
  else if (index < COCO_MAX_TIME_COOKIES)
    {
      int string_size;
      struct tms tms;
      times( &tms );
      timez[index] = (coco_tc_mode==COCO_SYS) ? tms.tms_stime : tms.tms_utime;
      string_size = strlen( label )+1;
      memcpy( labels[index], label, 
              COCO_MIN( COCO_MAX_TIME_COOKIE_LABEL-1, string_size ) );
      labels[index][COCO_MAX_TIME_COOKIE_LABEL-1] = '\0';
      index++;
    }
}

/* COCO_SYS, COCO_USER, COCO_REAL, or COCO_NONE (=do nothing). */
void 
coco_set_time_cookie_mode( enum coco_time_cookie_mode mode )
{
  coco_tc_mode = mode;
}


/* Record time info.  With a string argument, time is taken and stored.
   With NULL argument prints out the times between all labeled calls. */
void
coco_time_cookie( char *label )
{
  switch (coco_tc_mode) {
  case COCO_REAL: 
    coco_real_time_cookie( label ); 
    break;
  case COCO_SYS:
  case COCO_USER:
    coco_sys_user_time_cookie( label ); 
    break;
  case COCO_NONE:
    break;
  default:
    coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                       "Feel through time_cookie switch.\n" );
  }
}

coco_angle
coco_atan2( coco_gnum y, coco_gnum x )
{
  int raw_angle = floor( (atan2( y, x ) * 128 / COCO_PI) + 0.5 );
  return (raw_angle < 0.0) ? (raw_angle + 256) : raw_angle;
}

/* returns 0 on success, or a positive number on failure (the errno) */
int
coco_map_directory( const char *directory, 
                    coco_bool (* fn)( char *, void * ),
                    void *data )
{
  DIR *dir;
  struct dirent *entry;
  dir = opendir( directory );
  if (NULL == dir)
    return errno;
  else 
    {
      while (1)
        {
          entry = readdir( dir );
          if (NULL == entry)
            break;
          if (fn( entry->d_name, data ))
            break;
        }
      return 0;
    }
}

/* returns true if filename exists and is readable. */
coco_bool
coco_probe_file( char *filename )
{
  int old_errno = errno;
  FILE *f = fopen( filename, "r" );
  if (f)
    {
      if (0!=fclose( f ))
        coco_warn_printf( "Fclose of %s during coco_probe_file failed "
                          "with error %s.\n", filename, 
                          coco_errno_string( errno ) );
      return COCO_TRUE;
    }
  else 
    {
      errno = old_errno;
      return COCO_FALSE;
    }
}

/* returns a freshly coco_malloced file name */
char *
coco_sound_name_to_file_name( char *sound_name )
{
  char *buf = (char *) coco_malloc( (strlen( coco_sound_directory ) 
                                     + strlen( sound_name ) + 6)
                                    * sizeof( char ) );
  sprintf( buf, "%s/%s.wav", coco_sound_directory, sound_name );
  coco_cookie( "Looking for \"%s\"\n", buf );
  return buf;
}

#if 1234 == COCO_BYTE_ORDER

coco_u32 
COCO_U32_HTON( u32 )
{
  return (u32<<24)
    | ((u32>>24)&0xFF)
    | ((u32>>8)&0xFF00)
    | ((u32&0xFF00)<<8);
}

coco_u16
COCO_U16_HTON( u16 )
{
  return ((u16>>8)&0xFF) | (u16<<8);
}

#endif /* 1234 == COCO_BYTE_ORDER */

int
coco_check_hton( void )
{
  coco_u32 uu = 0x01020304;
  coco_i32 ii = (coco_i32) uu;
  coco_u16 u = 0x0102;
  coco_i16 i = (coco_i16) u;
  int test = 1;
  if (uu != COCO_U32_HTON( COCO_U32_NTOH( uu ) )) return test; test++;
  if (uu != COCO_U32_NTOH( COCO_U32_HTON( uu ) )) return test; test++;
  if (ii != COCO_I32_HTON( COCO_I32_NTOH( ii ) )) return test; test++;
  if (ii != COCO_I32_NTOH( COCO_I32_HTON( ii ) )) return test; test++;
  if (u != COCO_U16_HTON( COCO_U16_NTOH( u ) )) return test; test++;
  if (u != COCO_U16_NTOH( COCO_U16_HTON( u ) )) return test; test++;
  if (i != COCO_I16_HTON( COCO_I16_NTOH( i ) )) return test; test++;
  if (i != COCO_I16_NTOH( COCO_I16_HTON( i ) )) return test; test++;
  return 0;
}

