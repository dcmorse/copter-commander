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


#include <sys/time.h>
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

#include "coco.h"


char *
coco_errno_string( int error_number )
{
  return strerror( error_number );
}


void 
coco_fatal_printf( int exit_code, const char *fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( stderr, fmt, ap );
  va_end( ap );
  /* *((char *)NULL) = 'X'; /+ exit w/core +/ */
  exit( exit_code );
}

void 
coco_warn_printf( const char *fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( stderr, fmt, ap );
  va_end( ap );
}

void 
coco_log_printf( const char *fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf ( coco_server_log, fmt, ap );
  va_end( ap );
}

/* return non-void so we can be used inside expressions */
int 
coco_cookie( const char *fmt, ... )
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
coco_strdup( const char *src )
{
  char *buf = coco_malloc( strlen( src ) + 1 );
  strcpy( buf, src );
  return buf;
}

char *
coco_strndup( const char *src, size_t size )
{
  char *buf = coco_malloc( size + 1 );
  strncpy( buf, src, size );
  buf[size] = '\0';
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

/* 

  partial test suite for some and every, below
  COCO_BIT_VEC_VAR( a, 11 );
  COCO_BIT_VEC_VAR( b, 11 );
  COCO_BIT_VEC_VAR( c, 11 );
  COCO_BIT_VEC_VAR( d, 11 );
  coco_clear_bit_vec( a, 11, 0 );
  coco_clear_bit_vec( b, 11, 0 );
  coco_clear_bit_vec( c, 11, 1 );
  coco_clear_bit_vec( d, 11, 1 );
  coco_set_bit( a, X, 1 ); /+ for X = 0, 7, 8, 10 +/
  coco_set_bit( c, X, 0 );
  coco_cookie( "some( a ) = %d %s\n",
               coco_bit_vec_some( a, 11 ),
               coco_bit_vec_some( a, 11 ) ? "pass" : "fail" );
  coco_cookie( "some( b ) = %d %s\n",
               coco_bit_vec_some( b, 11 ),
               coco_bit_vec_some( b, 11 ) ? "fail" : "pass" );
  coco_cookie( "every( c ) = %d %s\n",
               coco_bit_vec_every( c, 11 ),
               coco_bit_vec_every( c, 11 ) ? "fail" : "pass" );
  coco_cookie( "every( d ) = %d %s\n",
               coco_bit_vec_every( d, 11 ),
               coco_bit_vec_every( d, 11 ) ? "pass" : "fail" );
  return 0;

 */
coco_bool 
coco_bit_vec_some( unsigned char *bit_vector, int size )
{
  int i, limit;
  limit = size >> 3;
  for (i=0; i<limit; i++)
    if (bit_vector[i])
      return COCO_TRUE;
  return (size&7) && (0 < (bit_vector[limit] & ((1<<(size&7))-1)));
}

/* tested but not currently needed.
coco_bool 
coco_bit_vec_every( unsigned char *bit_vector, int size )
{
  int i, limit, mask;
  limit = size >> 3;
  for (i=0; i<limit; i++)
    if (bit_vector[i] != 255)
      return COCO_FALSE;
  mask = ((1<<(size&7))-1);
  return (size&7) && ((bit_vector[limit] & mask) == mask);
}
*/

void 
coco_bit_vec_and( unsigned char *v1, unsigned char *v2, int size,
                  unsigned char *vreturn )
{
  int i, limit;
  limit = (size+7) >> 3;
  for (i=0; i<limit; i++)
    vreturn[i] = v1[i] & v2[i];
}

void
coco_bit_vec_not( unsigned char *v1, int size,
                  unsigned char *vreturn )
{
  int i, limit;
  limit = (size+7) >> 3;
  for (i=0; i<limit; i++)
    vreturn[i] = ~(v1[i]);
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
  int nega = udiff < 0;
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
      closedir( dir );
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


/* function to roll NUMBER of dice with SIDES sides.  I.e. for typical
   monopoly turn roll 2 dice with 6 sides: coco_dice( 2, 6 ).
   sides must be <= 255.  This is to call rand() as fewer times than
   otherwise would be the case.  */
int
coco_dice( int number, unsigned char sides )
{
  int randomness;
  int bytes_left = 0; /* bytes of randomness in sides */
  int acc = number; /* every die starts with 1, not 0 */
  if (0==sides) return 1; /* nonsense! */
  for (; number > 0; number--)
    {
      if (0==bytes_left)
        {
          randomness = rand();
          bytes_left = sizeof( int );
        }
      acc += randomness % sides;
      randomness >>= 8;
      bytes_left--;
    }
  return acc;
}

void
coco_time_pad_start( struct timeval *private )
{
  if (0 != gettimeofday( private, NULL ))
    {
      private->tv_sec = private->tv_usec = 0;
      coco_warn_printf( "Gettimeofday failed with errno=%d: %s\n",
                        errno, coco_errno_string( errno ) );
    }
}


void
coco_time_pad_end( struct timeval *private, struct timeval *how_long )
{
  struct timeval diff, now;
  fd_set fdset;
  FD_ZERO( &fdset );
  if (0 != gettimeofday( &now, NULL ))
    {
      coco_warn_printf( "Gettimeofday failed with errno=%d: %s\n",
                        errno, coco_errno_string( errno ) );
      return;
    }
  coco_subtract_timevals( &diff, &now, private );
  coco_subtract_timevals( &diff, how_long, &diff );
  if ((diff.tv_sec < 0)
      || ((diff.tv_usec <= 0) && (diff.tv_sec == 0)))
    {
      /* coco_warn_printf( "Slow renederer\n" ); */
      return;
    }
  else if (-1 == select( 1, &fdset, &fdset, &fdset, &diff ))
    coco_warn_printf( "Sleeping select failed with errno=%d: %s\n",
                      errno, coco_errno_string( errno ) );
}

/* returns CURRENT, modified towards GOAL by as much as BY.  If BY is
   negative, returns CURRENT away from GOAL by as much as BY. */

int
coco_tend_towards( int current, int goal, int by )
{
  int delta = goal - current;
  if (by <= 0)
    {
      if (delta >= 0)
        return current + by;
      else 
        return current - by;
    }
  else
    {
      if (delta >= 0)
        {
          if (delta > by)
            return current + by;
          else return goal;
        }
      else 
        {
          if (-delta > by)
            return current - by;
          else return goal;
        }
    }
  /* tested with this function:
  void
  coco_test_tend_towards( int current, int goal, int by, int answer )
  {
    int x = coco_tend_towards( current,goal,by );
    coco_cookie( "coco_tend_towards( %d %d %d ) => %d %s\n", current, goal, by,
                 x, (x==answer) ? "" : "WRONG" );
  }

  coco_test_tend_towards( 0, 10, 4, 4 );
  coco_test_tend_towards( 0, 10, -4, -4 );
  coco_test_tend_towards( 10, 0, 4, 6 );
  coco_test_tend_towards( 10, 0, -4, 14 );
  coco_test_tend_towards( 0, 10, 14, 10 );
  coco_test_tend_towards( 0, 10, -14, -14 );
  coco_test_tend_towards( 10, 0, 14, 0 );
  coco_test_tend_towards( 10, 0, -14, 24 );
  coco_test_tend_towards( 0, -10, 4, -4 );
  coco_test_tend_towards( 0, -10, -4, 4 );
  coco_test_tend_towards( -10, 0, 4, -6 );
  coco_test_tend_towards( -10, 0, -4, -14 );
  coco_test_tend_towards( 0, -10, 14, -10 );
  coco_test_tend_towards( 0, -10, -14, 14 );
  coco_test_tend_towards( -10, 0, 14, 0 );
  coco_test_tend_towards( -10, 0, -14, -24 );
 */

}

