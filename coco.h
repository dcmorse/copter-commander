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

#ifndef COCO_H
#define COCO_H

#include <sys/time.h>           /* for timeval */
#include <stdio.h>              /* for FILE * */
#include <limits.h>             /* for INT_MAX */
#include <netinet/in.h>         /* for ntohl */

/* These defines control the look+feel of the in-game play.
   RESCUE_RAIDERS is a sloppy attempt to emulate the sir-tech title.
   DOODLE is bay's idea of making the whole game look like something a
   third grader would draw.  Test for which is present in the manner in
   rasters.h.*/
#define COCO_APPEARANCE_RESCUE_RAIDERS 1
#define COCO_APPEARANCE_DOODLE 2

/* The file name extention of level files. */
#define COCO_LEVEL_SUFFIX "ccl"
#define COCO_LEVEL_SUFFIX_LENGTH 3


/* general types */

/* for network traffic, or flags,
   moved to configure-time.h 
   typedef unsigned int coco_u32;
   #define COCO_U32_MAX UINT_MAX */
 
/* Believe it or not, C doesn't specify overflow behavior.  In gcc on
   linux it saturates with -O0, and wraps with -03. */
#define COCO_U32_ADD_MODULAR( number ) \
  (((number)==COCO_U32_MAX) ? 0 : ((number)+1))
#define COCO_U32_ADD_N_MODULAR( a, b ) \
  (((a) > (COCO_U32_MAX - (b)) ) ? ((a) - (COCO_U32_MAX - (b))) : ((a)+(b)))


#include "configure-time.h"


/* for network traffic, moved to configure-time.h,
   typedef signed int coco_i32;
   #define COCO_I32_MAX INT_MAX */

typedef int coco_gnum;           /* pixels from lower left of board, etc. */
#define COCO_GNUM_MAX INT_MAX
#define COCO_GNUM_MIN INT_MIN
typedef long coco_lnum;          /* level design metric, >= coco_gnum. */
typedef signed char coco_znum;   /* depth number [-1 2].  */
#define COCO_ZNUM_MIN -1
#define COCO_ZNUM_MAX 2
typedef coco_i16 coco_cnum;         /* client screen-origin relative */
#define COCO_CNUM_HTON( cnum ) (COCO_I16_HTON( cnum ))
#define COCO_CNUM_NTOH( cnum ) (COCO_I16_NTOH( cnum ))
typedef coco_i16 coco_rxnum; /* x pixels on low-res radar screen. */
#define COCO_RXNUM_HTON( rxnum ) (COCO_I16_HTON( rxnum ))
#define COCO_RXNUM_NTOH( rynum ) (COCO_I16_NTOH( rynum ))
typedef signed char coco_rynum; /* y pixels on low-res radar screen. */
#define COCO_RYNUM_HTON( rynum ) (rynum)
#define COCO_RYNUM_NTOH( rynum ) (rynum)
typedef signed char coco_facing; /* either -1, 0, or 1.  1=facing right. */
typedef unsigned char coco_color_index; /* for radar blips. */
typedef unsigned char coco_angle; /* 0==256==positive-x-axis.  64==positive-y-axis */
#define COCO_ANGLE_TO_FLOAT_DEGREES( coco_angle ) \
  ((float) coco_angle * (360.0 / 256.0))
#define COCO_COS( coco_angle ) (cos( (coco_angle) * COCO_PI/128 ))
#define COCO_SIN( coco_angle ) (sin( (coco_angle) * COCO_PI/128 ))
coco_angle coco_atan2( coco_gnum y, coco_gnum x );

typedef unsigned char coco_bool; 
#define COCO_TRUE 1
#define COCO_FALSE 0

/* All coco values sent over tcp are converted to big endian aka network
   byte order */
#if 1234 == COCO_BYTE_ORDER
/* Little endian */
coco_u32 COCO_U32_HTON( u32 );
#define COCO_U32_NTOH( u32 ) COCO_U32_HTON( u32 )
#define COCO_I32_HTON( i32 ) COCO_U32_HTON( i32 )
#define COCO_I32_NTOH( i32 ) COCO_I32_HTON( i32 )
coco_u16 COCO_U16_HTON( u16 );
#define COCO_U16_NTOH( u16 ) COCO_U16_HTON( u16 )
#define COCO_I16_HTON( i16 ) COCO_U16_HTON( i16 )
#define COCO_I16_NTOH( i16 ) COCO_U16_HTON( i16 )

#elif 4321 == COCO_BYTE_ORDER
/* Big endian */
#define COCO_U32_HTON( u32 ) (u32)
#define COCO_U32_NTOH( u32 ) (u32)
#define COCO_I32_HTON( i32 ) (i32)
#define COCO_I32_NTOH( i32 ) (i32)
#define COCO_U16_HTON( u16 ) (u16)
#define COCO_U16_NTOH( u16 ) (u16)
#define COCO_I16_HTON( i16 ) (i16)
#define COCO_I16_NTOH( i16 ) (i16)

#else
#define COCO_U32_HTON( u32 ) "Wacky byte ordering not supported"
#define COCO_U32_NTOH( u32 ) "Wacky byte ordering not supported"
#define COCO_I32_HTON( i32 ) "Wacky byte ordering not supported"
#define COCO_I32_NTOH( i32 ) "Wacky byte ordering not supported"
#define COCO_U16_HTON( u16 ) "Wacky byte ordering not supported" 
#define COCO_U16_NTOH( u16 ) "Wacky byte ordering not supported" 
#define COCO_I16_HTON( i16 ) "Wacky byte ordering not supported" 
#define COCO_I16_NTOH( i16 ) "Wacky byte ordering not supported" 
#endif /* check byte orders */

/* fatal error codes */
#define COCO_ERROR_NO_X_DISPLAY 1
#define COCO_ERROR_NO_GL 2
#define COCO_ERROR_NO_VISUAL 3
#define COCO_ERROR_GLX_CONTEXT_PROBLEMS 4
#define COCO_ERROR_NO_FONT 5
#define COCO_ERROR_DISPLAY_LISTS 6
#define COCO_ERROR_NETWORK_BROKEN 7
#define COCO_ERROR_BAD_COMMAND_LINE 8
#define COCO_ERROR_NOT_IMPLEMENTED 9
#define COCO_ERROR_XLIB_PROBLEMS 10
#define COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM 11
#define COCO_ERROR_OUT_OF_MEMORY 12
#define COCO_ERROR_BAD_LEVEL_DESIGN 13
#define COCO_ERROR_LEVEL_FILE_NOT_FOUND 14
#define COCO_ERROR_BAD_LEVEL 15
#define COCO_ERROR_SERVER_VERSION_INCOMPATIBLE 16
#define COCO_ERROR_SOUND_FILE_PROBLEMS 17

/* gnome.c or glx.c */

extern coco_bool coco_gnome; /* true if gnome gui present */
coco_bool coco_process_gnome_args( int start, int end, char **argv );

/* language.c */

#define COCO_MAX( a, b ) (((a)>(b)) ? (a) : (b))
#define COCO_MIN( a, b ) (((a)<(b)) ? (a) : (b))
#define COCO_CLAMP( a, b, c ) COCO_MIN( COCO_MAX( a, b ), c )
/* true if middle number is between outer ones (i=inclusive) */
#define COCO_BETWEENI( a, b, c ) (((b)>=(a))&&((b)<=(c)))

#define COCO_PRODUCT_NAME "Copter Commander"
extern const char *coco_product_name;  /* Copter Commander */
extern const char *coco_run_name;  /* argv[0] */

/* VERSIONS

   Versions help keep track of what is compatible with what.  There are two
   main areas where incompatibilities can exist.  The first is in the
   network protocol.  The second is the level file format.

   Each version keeps a list of older versions with which it is backward
   compatible.  Unfortunately, this approach can't work for future versions,
   because we don't know if they're going to be compatible or not.
   Therefore the version is split into major and minor.  If two entites
   share the same major version then the older will assume it can
   communicate with the younger.  If the younger is smart enough to check
   itself (a client or a server, but not a level), it may still decide not
   to talk. 

   The function coco_version_compatible_p tests for compatiblity between
   this version of the software and a specified version. 

   if you change COCO_MAJOR_VERSION or COCO_MINOR_VERSION, you must also
   change coco_version_string in globals.c 
*/

/* Moved to configure-time.h
 * #define COCO_MAJOR_VERSION 1 
 * #define COCO_MINOR_VERSION 0 */

#include "object-time.h"

extern const char *coco_version_string;
enum coco_version_domain {COCO_NETWORK, COCO_LEVEL_FILE_FORMAT};

extern const char *coco_product_authors[];
extern const char *coco_product_copyright;
extern const char *coco_product_description;

extern const char *coco_gnu_public_license;
#define COCO_GNU_NON_WARRANTY (coco_gnu_public_license + 13852)

char *coco_errno_string( int error_number );

void coco_fatal_printf( int exit_code, const char *fmt, ... );
void coco_warn_printf( const char *fmt, ... );
void coco_log_printf( const char *fmt, ... );
int coco_cookie( const char *fmt, ... );

unsigned int coco_random( unsigned int mask );

void *coco_malloc( size_t size );
void *coco_realloc( void *ptr, size_t size );
void coco_free( void *ptr );
char *coco_string_malloc_copy( char *src );

coco_bool time_less_than( coco_u32 t1, coco_u32 t2 );

/* produce a stack variable named VARNAME, capable of holding SIZE bits.
   Also works to produce typedefs named VARNAME.  */
#define COCO_BIT_VEC_VAR( varname, size ) unsigned char varname[((size)+7)>>3]
/* produce a type declaration for a given SIZEd bit-vector. */
#define COCO_BIT_VEC_TYPE( size ) unsigned char[((size)+7)>>3]
/* sometimes when bit vectors must be returned, the compiler wants this
   instead of a BIT_VEC_TYPE. */
typedef unsigned char *coco_bit_vec_ref;

int coco_get_bit( unsigned char *bit_vector, int i );
void coco_set_bit( unsigned char *bit_vector, int i, int bit );
void coco_clear_bit_vec( unsigned char *bit_vector, int size, int value );
void coco_copy_bit_vec( unsigned char *dest, unsigned char *src, int size );
/* have to provide a bytewise method for network serialization. */
#define coco_get_byte( bit_vector, bytenum ) \
  ((unsigned char)((bit_vector)[bytenum]))
#define coco_set_byte( bit_vector, bytenum, byteval ) \
  (((bit_vector)[bytenum]) = (byteval))

/* _Misc_______ */

void coco_format_ipaddr( FILE *stream, coco_u32 addr, unsigned short port, 
                         coco_bool use_port );
void coco_add_timevals( struct timeval *tv_return, 
                        struct timeval *tv1, struct timeval *tv2 );
void coco_subtract_timevals( struct timeval *tv_return, 
                             struct timeval *tv1, struct timeval *tv2 );

enum coco_time_cookie_mode {COCO_REAL, COCO_SYS, COCO_USER, COCO_NONE};

void coco_set_time_cookie_mode( enum coco_time_cookie_mode mode );

void coco_time_cookie( char *label );

#define COCO_PI 3.14159265358979323846

#define COCO_DIRECTORY_SEPERAOTOR_CHAR '/'

int coco_map_directory( const char *directory, 
                        coco_bool (* fn)( char *, void * ),
                        void *data );



coco_bool coco_probe_file( char *filename );

#define COCO_MAKE_MINI_RECTANGLE( x0, y0, x1, y1 ) \
  ((x0)|((y0<<8))|((x1)<<16)|((y1<<24)))
#define COCO_MINI_RECTANGLE_X0( mr ) ((int) ((mr)&255))
#define COCO_MINI_RECTANGLE_Y0( mr ) ((int) (((mr)>>8)&255))
#define COCO_MINI_RECTANGLE_X1( mr ) ((int) (((mr)>>16)&255))
#define COCO_MINI_RECTANGLE_Y1( mr ) ((int) (((mr)>>24)&255))
#define COCO_MINI_RECTANGLE_NULL_P( mr ) ((mr)==4294967295u)
#define COCO_MINI_RECTANGLE_NULL (4294967295u)
typedef coco_u32 coco_mini_rectangle;

#define COCO_MAKE_MINI_POINT( x, y ) (((x)<<8)|(y))
#define COCO_MINI_POINT_X( mp ) ((int) (((mp)>>8)&255))
#define COCO_MINI_POINT_Y( mp ) ((int) ((mp)&255))
#define COCO_MINI_POINT_NULL_P( mp ) ((mp)&65536)
#define COCO_MINI_POINT_NULL 65536
typedef coco_u32 coco_mini_point;

char *coco_sound_name_to_file_name( char *sound_name );

int coco_check_hton( void );

/* command-line-sho.c */

void coco_set_log_file( char *log_file_name );

coco_bool coco_level_filename_p( char *filename );

coco_bool coco_massage_level_file_name( char **level_file_name_in_out, 
                                        coco_bool *
                                        level_file_name_malloced_p_in_out );

void coco_usage_message( coco_bool exit_p );
void coco_help_message();
void coco_version_message( void );
void coco_list_levels_message( void );

/* graphics.c */

typedef unsigned int coco_color;

#define COCO_RED( color )   (255 & ((color)>>24))
#define COCO_GREEN( color ) (255 & ((color)>>16))
#define COCO_BLUE( color )  (255 & ((color)>>8))
#define COCO_ALPHA( color ) (255 & (color))
#define COCO_MAKE_COLOR( r, g, b, a ) (((r)<<24)+((g)<<16)+((b)<<8)+(a))

#define COCO_COLOR_TRANSPARENT 0xFFFFFF00
#define COCO_COLOR_WHITE 0xFFFFFFFF
#define COCO_COLOR_BLACK 0x000000FF
#define COCO_COLOR_RED   0xFF0000FF
#define COCO_COLOR_GREEN 0x00FF00FF
#define COCO_COLOR_BLUE  0x0000FFFF
#define COCO_COLOR_LIGHT_BLUE  0x3A80EEFF /* must match corner rasters */
#define COCO_COLOR_GREY  COCO_COLOR_GREY_8
#define COCO_COLOR_BROWN 0xA06710FF
#define COCO_COLOR_ORANGE COCO_MAKE_COLOR( 255, 157, 0, 255 )
#define COCO_COLOR_YELLOW COCO_MAKE_COLOR( 255, 255, 0, 255 )
#define COCO_COLOR_DARK_YELLOW COCO_MAKE_COLOR( 190, 190, 0, 255 )
#define COCO_COLOR_DARK_RED COCO_MAKE_COLOR( 170, 0, 0, 255 )
#define COCO_COLOR_KHAKI COCO_MAKE_COLOR( 178, 149, 87, 255 )
#define COCO_COLOR_GREEN_GREY COCO_MAKE_COLOR( 127, 190, 127, 255 )
#define COCO_COLOR_GREY_1 0x0F0F0FFF
#define COCO_COLOR_GREY_2 0x1F1F1FFF
#define COCO_COLOR_GREY_3 0x2F2F2FFF
#define COCO_COLOR_GREY_4 0x3F3F3FFF
#define COCO_COLOR_GREY_5 0x4F4F4FFF
#define COCO_COLOR_GREY_6 0x5F5F5FFF
#define COCO_COLOR_GREY_7 0x6F6F6FFF
#define COCO_COLOR_GREY_8 0x7F7F7FFF
#define COCO_COLOR_GREY_9 0x8F8F8FFF
#define COCO_COLOR_GREY_A 0x9F9F9FFF
#define COCO_COLOR_GREY_B 0xAFAFAFFF
#define COCO_COLOR_GREY_C 0xBFBFBFFF
#define COCO_COLOR_GREY_D 0xCFCFCFFF
#define COCO_COLOR_GREY_E 0xDFDFDFFF
#define COCO_COLOR_GREY_F 0xEFEFEFFF

#if (COCO_APPEARANCE == COCO_APPEARANCE_RESCUE_RAIDERS)
#define COCO_COLOR_BACKGROUND COCO_COLOR_BLACK
#define COCO_COLOR_GROUND COCO_COLOR_BROWN
#define COCO_COLOR_FONT COCO_COLOR_LIGHT_BLUE
#define COCO_COLOR_CURSOR COCO_COLOR_RED
#define COCO_COLOR_CHAT COCO_COLOR_FONT
#define COCO_COLOR_CURSOR COCO_COLOR_RED
#define COCO_COLOR_ROTOR COCO_COLOR_GREY
#define COCO_COLOR_BULLET COCO_COLOR_WHITE
#define COCO_COLOR_BARRAGE_BALLON_LINE COCO_COLOR_GREY_E
#define COCO_COLOR_BARREL COCO_COLOR_GREY
#define COCO_COLOR_DASHBOARD_FG COCO_COLOR_GREY
#define COCO_COLOR_DASHBOARD_FULL COCO_COLOR_GREEN_GREY
#elif (COCO_APPEARANCE == COCO_APPEARANCE_DOODLE)
#define COCO_COLOR_BACKGROUND COCO_COLOR_GREY_E
#define COCO_COLOR_GROUND COCO_COLOR_GREY_4
#define COCO_COLOR_FONT COCO_COLOR_GREY_1
#define COCO_COLOR_CURSOR COCO_COLOR_RED
#define COCO_COLOR_CHAT COCO_COLOR_GREY_4
#define COCO_COLOR_CURSOR COCO_COLOR_RED
#define COCO_COLOR_ROTOR COCO_COLOR_GREY_2
#define COCO_COLOR_BULLET COCO_COLOR_BLACK
#define COCO_COLOR_BARRAGE_BALLON_LINE COCO_COLOR_GREY_3
#define COCO_COLOR_BARREL COCO_COLOR_GREY_4
#define COCO_COLOR_DASHBOARD_FG COCO_COLOR_GREY_3
#define COCO_COLOR_DASHBOARD_FULL COCO_COLOR_BLACK
#else
#error COCO_APPEARANCE is not a value known to coco.h
#endif

extern const coco_color coco_palette[256];
#define COCO_INDEX_TRANSPARENT 0
#define COCO_INDEX_BLACK 1
#define COCO_INDEX_RED   2
#define COCO_INDEX_GREEN 3
#define COCO_INDEX_BLUE  4
#define COCO_INDEX_GREY  5
#define COCO_INDEX_BROWN 6
#define COCO_INDEX_LIGHT_BLUE 7
#define COCO_INDEX_ORANGE 8
#define COCO_INDEX_WHITE 9
#define COCO_INDEX_YELLOW 10
#define COCO_INDEX_GREY_C 11
#define COCO_INDEX_DARK_YELLOW 12
#define COCO_INDEX_DARK_RED 13
#define COCO_INDEX_KHAKI 14
#define COCO_COLOR_INDEX( color_index ) (coco_palette[color_index])

typedef coco_u32 coco_color_index_triplet;
#define COCO_CITRIP_FRIENDLY( citriplet ) (255 & ((citriplet)>>16))
#define COCO_CITRIP_HOSTILE( citriplet )  (255 & ((citriplet)>>8))
#define COCO_CITRIP_NEUTRAL( citriplet ) (255 & (citriplet))
#define COCO_MAKE_COLOR_INDEX_TRIPLET( friendly, hostile, neutral ) \
  (((friendly)<<16) + ((hostile)<<8) + (neutral))
#define COCO_COLOR_INDEX_TRIPLET_TRANSPARENT             \
  COCO_MAKE_COLOR_INDEX_TRIPLET( COCO_INDEX_TRANSPARENT, \
                                 COCO_INDEX_TRANSPARENT, \
                                 COCO_INDEX_TRANSPARENT )

void coco_draw_color( coco_color color );
void coco_clear_window_color( coco_color color );
void coco_clear_window( void );
void coco_draw_point( coco_gnum x, coco_gnum y );
void coco_draw_big_point( coco_gnum x, coco_gnum y, int size );
void coco_begin_point_drawing( void );
void coco_point_drawing_vertex( coco_gnum x0, coco_gnum y0 );
void coco_end_point_drawing( void );
void coco_draw_rectangle( coco_gnum x0, coco_gnum y0,
                           coco_gnum x1, coco_gnum y1 );
void coco_draw_line( coco_gnum x0, coco_gnum y0, 
                     coco_gnum x1, coco_gnum y1 );
void coco_begin_line_drawing( void );
void coco_end_line_drawing( void );
void coco_line_drawing_vertex( coco_gnum x0, coco_gnum y0, 
                                coco_gnum x1, coco_gnum y1 );

void coco_setup_matricies( coco_gnum w, coco_gnum h );
void coco_setup_misc_graphics( void );

extern coco_bool coco_double_buffer;

void coco_rasterpos( coco_gnum x, coco_gnum y ); 

#include "raster.h"
void coco_draw_raster( coco_gnum x, coco_gnum y, struct coco_raster *raster );
void
coco_draw_raster_rotate( coco_gnum x, coco_gnum y,
                         coco_gnum w, coco_gnum h,
                         struct coco_raster *raster );
void coco_begin_rotated_view( coco_gnum x, coco_gnum y,
                              coco_angle angle );
void coco_end_rotated_view( void );

void 
coco_reload_texture( struct coco_raster *raster, coco_gnum w, coco_gnum h );

void coco_warn_gl_errors( char *label );


struct coco_font {
  char *name;
  void *implementation_datum;
  long list_base;
};

extern struct coco_font coco_default_font;
extern struct coco_font coco_huge_font;
extern struct coco_font coco_fixed_font;

enum coco_font_origin {COCO_BASELINE, COCO_TOP, COCO_BOTTOM};

void coco_draw_string( coco_gnum x, coco_gnum y, 
                       const char *string, int nchars_or_minus_one,
                       struct coco_font *font, enum coco_font_origin origin );

int coco_font_height( struct coco_font *font );
void coco_font_string_dimensions( char *string, int nchars_or_minus_one,
                                  struct coco_font *font,
                                  int *w, int *h, int *ascent );


coco_bool coco_set_inhibit_refresh( coco_bool inhibit );
coco_bool coco_get_inhibit_refresh( void );



/* window.c -- this is a virtual source file possibly implemented in
   either of gnome-window.c or glx-window.c */

extern const char *coco_gui_description_string; /* GLX or Gnome */
extern coco_bool coco_use_keyboard_for_gui_p;
extern char *coco_gnu_info_string;
extern char *coco_low_tech_menu;

/* returns -1 if X isn't running yet, otherwise the display's fd */
int coco_get_x_file_descriptor( void );

void coco_make_window( void );
void coco_kill_window( void );
void coco_handle_ui_events( void );
void coco_swap_buffer( void );
coco_bool coco_get_pointer_grabbed_flag();
coco_bool coco_grab_pointer( void );
void coco_ungrab_pointer( void );
coco_bool coco_mouse_in_window_p();

/* coco_keys correspond to unsigned ascii as much as possible.  Keys 32
   through 126 are usually represented by character literals.  These are
   key caps and not characters, so the effect of shifting is not taken
   into effect.  Thuts its hard for conventional keyboards to produce '@',
   instead they make SHIFT and '2'.  The capital letters are not used,
   (lowercase is equivalent).  Various specials are packed into the
   capital space, and if need be into the control characters instead. */
typedef unsigned char coco_key;
/* #define COCO_KEY_BACKSPACE  8 Use delete */
#define COCO_KEY_TAB        9
#define COCO_KEY_RETURN    12
#define COCO_KEY_ESCAPE    27
#define COCO_KEY_SPACE     32
#define COCO_LPOINTER     'A' /* These pointers must lie here or  */
#define COCO_MPOINTER     'B' /* coco_censor_keys gets confused. */
#define COCO_RPOINTER     'C'
#define COCO_4POINTER     'D'
#define COCO_5POINTER     'E'
#define COCO_KEY_SHIFT    'F'
#define COCO_KEY_CONTROL  'G'
#define COCO_KEY_META     'H' 
#define COCO_KEY_ALT      'I' 
#define COCO_KEY_SUPER    'J' /* Note how we subtly prepare the way */
#define COCO_KEY_HYPER    'L' /* for the return of the space cadet. */
#define COCO_KEY_HELP     'M' /* Not supported? */
#define COCO_KEY_LEFT_ARROW  'N'
#define COCO_KEY_RIGHT_ARROW 'O'
#define COCO_KEY_UP_ARROW    'P'
#define COCO_KEY_DOWN_ARROW  'Q'
#define COCO_KEY_DELETE   127 /* both delete and backspace */
#define COCO_KEYLIMIT     128 /* one more than the highest key. */

/* these converters assume the key isn't special. */
#define COCO_KEY_TO_CHAR( coco_key ) ((char) (coco_key))
#define COCO_CHAR_TO_KEY( ch ) ((coco_key) (ch))

/* special means a text editor would do something other than
   self-insert-char. */
coco_bool coco_key_special( coco_key key );

/* keystates */
/* coco_key_state records what keys and buttons are down.
   It is effeciently passed by value.   */
typedef COCO_BIT_VEC_VAR( coco_key_state, COCO_KEYLIMIT );
typedef coco_bit_vec_ref  coco_key_state_ref;

/* Get the (statically allocated) current key state.
   May be altered from call to call of handle-events. */
coco_key_state_ref coco_get_key_state( void );


void coco_refresh_window_eventually( void );

/* NOT USED, and prolly not supported under gnome.
void coco_get_pointer_coordinates( int *x, int *y );
*/

struct coco_keyboard_pointer_state {
  coco_u32 client_time; /* this is a uid [mod 32 bits] of the input state */
  coco_key_state key_state;
  coco_i32 pointer_x;
  coco_i32 pointer_y;
};

/* get the (statically allocated) current input state.
   May be altered from call to call of handle-events. */
struct coco_keyboard_pointer_state *coco_get_keyboard_pointer_state();

void coco_notify_version_incompatible( int major, int minor );

/* game intrensic types and defines (.c) */

typedef unsigned char coco_phase;
#define COCO_SOLID 0
#define COCO_EXPLOSIVE 1
#define COCO_VAPOR 2

typedef unsigned char coco_nationality;
#define COCO_NEUTRAL 0
#define COCO_GOOD_GUYS 1       /* start on right */
#define COCO_BAD_GUYS 2        /* start on left */
#define COCO_ENEMY_P( nat1, nat2 ) \
  ((COCO_GOOD_GUYS+COCO_BAD_GUYS)==((nat1)+(nat2)))


typedef unsigned char coco_health;
#define COCO_NIGH_INVULNERABLE 255 /* special meaning wrt health */

union coco_int_or_gnum {
  int integer;
  coco_gnum gnum;
};

struct coco_sstate_struct; /* forward decl wrt struct coco_class */
typedef struct coco_sstate_struct coco_sstate;

/* network.c */

enum coco_network_error {
  COCO_NETERROR_NONE = 0,
  COCO_NETERROR_DNS = 1,
  COCO_NETERROR_CREATE_TCP_SOCKET = 2,
  COCO_NETERROR_CONNECT_TCP = 3,
  COCO_NETERROR_COCO_WRITE_TCP = 4,
  COCO_NETERROR_CREATE_UDP_SOCKET = 5,
  COCO_NETERROR_BIND_SOCKET = 6,
  COCO_NETERROR_GETSOCKNAME = 7
  /* adding new messages must also add to the table in
     coco_network_error_string. */
};

char *coco_network_error_string( enum coco_network_error error );

void coco_assign_network_error( enum coco_network_error err,
                                enum coco_network_error *err_return, 
                                int *err_arg_return_or_null );

char *coco_user_level_network_error_message( enum coco_network_error error,
                                             int unix_errno_or_zero,
                                             coco_bool spawned_server_p );


/* adding members to this requires modification of
   coco_cstate_serial_header_hton and coco_cstate_serial_header_ntoh. */
struct coco_dashboard_info {
  coco_u32 bullets;
  coco_u32 bombs;
  coco_u32 missiles;
  coco_u32 rockets;
  coco_u32 paratroopers;
  coco_u32 engitroopers;
  coco_u32 copters;
  coco_u32 funds;
  coco_u32 fuel;
  unsigned char server_fps;
  coco_i32 camera_class_index; /* so wide a type for -1 */
};

/* adding members to this requires modification of the associated _hton
   and _ntoh functions. */
struct coco_cstate_serial_header {
  coco_u32 time;
  coco_u32 cobject_count;
  coco_u32 cblip_count;
  unsigned char game_over_nationality;
  coco_i32 cam_left;
  coco_i32 board_w;
  struct coco_dashboard_info dashinfo;
};

void
coco_cstate_serial_header_hton( struct coco_cstate_serial_header *str );

void
coco_cstate_serial_header_ntoh( struct coco_cstate_serial_header *str );

#define COCO_MESSAGE_SIZE 248

struct coco_stream {
  int fd;
  unsigned int buffer_in_use; /* in bytes from beginning */
  char buffer[COCO_MESSAGE_SIZE];
};

#define COCO_MAX_TEAM_PLAYERS 2
#define COCO_MAX_PLAYERS 4
#define COCO_MAX_COMPS 16
#define COCO_PLAYER_NAME_MAX 25 /* limit on player names, incl. null */


/* MESSAGE DATA DEFINITIONS______________________________ */

/* either direction */
#define COCO_MESSAGE_QUIT 'Q'
#define COCO_MESSAGE_VERSION 'V' /* describes running version number  */
#define COCO_MESSAGE_PLAY 'P' /* c->s: request game begin,
                                 s->c: go to your fun loop */

/* client to server */
#define COCO_MESSAGE_NAME 'N' /* set human readable player name */
#define COCO_MESSAGE_JOIN 'J' /* join the specified nationality  */
#define COCO_MESSAGE_CHAT 'C' /* chat to certain players */
#define COCO_MESSAGE_NOMINATE 'M' /* voting procedure */

/* server to client */
#define COCO_MESSAGE_TALK 'T' /* rebroadcast of player comments */
#define COCO_MESSAGE_GAME 'G' /* describes game chatroom */
#define COCO_MESSAGE_FULL 'F' /* Server is full.  Prepare to be dissed. */
#define COCO_MESSAGE_PORT 'O' /* # of udp port on sending machine */


#define COCO_CHAT_MAX (COCO_MESSAGE_SIZE - 8) /* incl. null */

struct coco_message_every {
  unsigned char type;
  char pad[COCO_MESSAGE_SIZE-1];
};

struct coco_message_quit {
  unsigned char type;
};

struct coco_message_name {
  unsigned char type;
  char name[COCO_PLAYER_NAME_MAX];
};

struct coco_message_join {
  unsigned char type;
  coco_nationality nationality;
};

struct coco_message_play {
  unsigned char type;
};

struct coco_message_chat {
  unsigned char type;        /* takes a word with pad */
  coco_u32 players;         /* bit vector, in network byte order */
  char message[COCO_CHAT_MAX];
};

#define COCO_NOMINATE_NOT_IN_USE        0 /* only in sstate */
#define COCO_NOMINATE_ADD_GOOD_GUY_COMP 1 /* act_object unused */
#define COCO_NOMINATE_ADD_BAD_GUY_COMP  2 /* act_object unused */
#define COCO_NOMINATE_BOOT_COMP         3 /* act_object == player_id */

#define COCO_MAX_NOMINATIONS 10 /* because there are 10 digit keys on kbd */

struct coco_message_nominate {
  unsigned char type;
  /* serial number of action [0-COCO_MAX_NOMINATIONS), or -1 if not yet
     assigned. */
  signed char serial; 
  unsigned char action_code; /* COCO_NOMINATE_* above */
  unsigned char act_object;  /* interpretation varies by action_code */
  coco_bool vote;
};


struct coco_message_talk {
  unsigned char type;        /* takes a word with pad */
  coco_u32 from_player;     /* in network byte order, COCO_U32_MAX means
                               not from any player, its from the
                               system. */
  char message[COCO_CHAT_MAX];
};

#define COCO_GAME_STATUS_SIZE (COCO_MESSAGE_SIZE - 8*(1+COCO_PLAYER_NAME_MAX) - 2)

struct coco_message_game {
  unsigned char type;
  unsigned char your_player_id;
  char game_status_string[COCO_GAME_STATUS_SIZE];
  coco_nationality nationalities[COCO_MAX_PLAYERS]; /* neutral if not in_use */
  char names[COCO_PLAYER_NAME_MAX][COCO_MAX_PLAYERS];
};

struct coco_message_version {
  unsigned char type;
  unsigned char major;
  unsigned char minor;
};

struct coco_message_full {
  unsigned char type;
};

struct coco_message_port {
  unsigned char type;
  unsigned short port; /* in network byte order */
};



union coco_message {
  struct coco_message_every every;
  struct coco_message_name name;
  struct coco_message_join join;
  struct coco_message_play play;
  struct coco_message_chat chat;
  struct coco_message_nominate nominate;
  struct coco_message_talk talk;
  struct coco_message_game game;
  struct coco_message_quit quit;
  struct coco_message_full full;
  struct coco_message_port port;
  struct coco_message_version version;
};

typedef union coco_message *coco_msg_ptr;

extern struct coco_message_version coco_my_version;

coco_bool coco_read_message( struct coco_stream *str, 
                             union coco_message *return_buff,
                             coco_bool *end_of_file_p );
coco_bool coco_write_message( struct coco_stream *str,
                              union coco_message *msg );

void coco_network_shared_setup( void );

void coco_reset_coco_players_array( void );

struct protoent *coco_get_protocol( char *name );

coco_bool coco_set_fd_blocking( int fd, coco_bool block );
coco_bool coco_get_fd_blocking( int fd );

void coco_close_fd( int fd, coco_bool shutdown_too );

int coco_make_udp_fd( unsigned short *port_return,
                      enum coco_network_error *err_return, 
                      int *err_arg_return );

/* ____ */

struct coco_client_to_server_packet {
  unsigned char player_id;
  struct coco_keyboard_pointer_state input_state;
};



/* gamesho.c */

/* These determine what can be detected by a player.  This info is also
   used by the server.  */
#define COCO_OFFICIAL_WINDOW_W 640 /* 700 */
#define COCO_OFFICIAL_WINDOW_H 480 /* 450 */

#define COCO_VIEWABLE_H 416
/* the missing pixel is a border */
#define COCO_RADAR_Y 417
#define COCO_RADAR_H 32


#define COCO_COPTER_RASTER_COUNT 4 /* no associated rasters */
struct coco_raster *coco_cobra_rasters[COCO_COPTER_RASTER_COUNT];
struct coco_raster *coco_bubble_copter_rasters[COCO_COPTER_RASTER_COUNT];
#define COCO_SMALL_METAL_RASTER_COUNT 2
extern struct coco_raster *coco_small_metal_shrapnel_rasters[COCO_SMALL_METAL_RASTER_COUNT];
#define COCO_MED_METAL_RASTER_COUNT 3
extern struct coco_raster *coco_med_metal_shrapnel_rasters[COCO_MED_METAL_RASTER_COUNT];
#define COCO_BIG_METAL_RASTER_COUNT 4
extern struct coco_raster *coco_big_metal_shrapnel_rasters[COCO_BIG_METAL_RASTER_COUNT];
#define COCO_SMOKE_RASTER_COUNT 3
extern struct coco_raster *coco_smoke_rasters[COCO_SMOKE_RASTER_COUNT];
#define COCO_CLOUD_RASTER_COUNT 1
extern struct coco_raster *coco_cloud_rasters[COCO_CLOUD_RASTER_COUNT];
#define COCO_BALLOON_RASTER_COUNT 3
extern struct coco_raster *coco_balloon_rasters[COCO_BALLOON_RASTER_COUNT];
#define COCO_BUNKER_RASTER_COUNT 2
struct coco_raster *coco_bunker_rasters[COCO_BUNKER_RASTER_COUNT];
#define COCO_PILLBOX_RASTER_COUNT 1
struct coco_raster *coco_pillbox_rasters[COCO_PILLBOX_RASTER_COUNT];
#define COCO_TRAFFIC_SIGN_RASTER_COUNT 1
extern struct coco_raster *coco_traffic_sign_rasters[COCO_TRAFFIC_SIGN_RASTER_COUNT];
#define COCO_HUT_RASTER_COUNT 3
extern struct coco_raster *coco_hut_rasters[COCO_HUT_RASTER_COUNT];
#define COCO_LANDING_PAD_RASTER_COUNT 1
extern struct coco_raster *coco_landing_pad_rasters[COCO_LANDING_PAD_RASTER_COUNT];
#define COCO_HEADQUARTERS_RASTER_COUNT 2
extern struct coco_raster *coco_headquarters_rasters[COCO_HEADQUARTERS_RASTER_COUNT];
#define COCO_GUN_RASTER_COUNT 2
extern struct coco_raster *coco_gun_rasters[COCO_GUN_RASTER_COUNT];
#define COCO_GUN_RUBBLE_RASTER_COUNT 1
extern struct coco_raster *coco_gun_rubble_rasters[COCO_GUN_RUBBLE_RASTER_COUNT];
#define COCO_DEMO_VAN_RASTER_COUNT 4
extern struct coco_raster *coco_demo_van_rasters[COCO_DEMO_VAN_RASTER_COUNT];
#define COCO_TANK_RASTER_COUNT 4
extern struct coco_raster *coco_tank_rasters[COCO_TANK_RASTER_COUNT];
#define COCO_BOMB_RASTER_COUNT 5
extern struct coco_raster *coco_bomb_rasters[COCO_BOMB_RASTER_COUNT];
#ifdef COCO_MISSILES_AS_RASTERS
#define COCO_MISSILE_RASTER_COUNT 12
extern struct coco_raster *coco_missile_rasters[COCO_MISSILE_RASTER_COUNT];
#endif /* COCO_MISSILES_AS_RASTERS */
#define COCO_FIRETRUCK_RASTER_COUNT 4
struct coco_raster *coco_firetruck_rasters[COCO_FIRETRUCK_RASTER_COUNT];
#define COCO_INFANTRY_RASTER_COUNT 16
extern struct coco_raster *coco_infantry_rasters[COCO_INFANTRY_RASTER_COUNT];
#define COCO_ENGINEER_RASTER_COUNT 16
extern struct coco_raster *coco_engineer_rasters[COCO_ENGINEER_RASTER_COUNT];
#define COCO_EXPLOSION_RASTER_COUNT 12
extern struct coco_raster *coco_explosion_rasters[COCO_EXPLOSION_RASTER_COUNT];

/* these also apply to engineers, the thinly veiled subclasses of infantry */
#define COCO_INFANTRY_WALK_COUNT 5
#define COCO_INFANTRY_WALK_WEST_BASE 0
#define COCO_INFANTRY_WALK_EAST_BASE 5
#define COCO_INFANTRY_FREE_FALL_COUNT 4
#define COCO_INFANTRY_FREE_FALL_BASE 10
#define COCO_INFANTRY_GOOD_GUY_CHUTE 14
#define COCO_INFANTRY_BAD_GUY_CHUTE 15

#define COCO_EXPLOSION_AIRBURST_BASE 0
#define COCO_EXPLOSION_AIRBURST_COUNT 3
#define COCO_EXPLOSION_BOMB_BASE COCO_EXPLOSION_AIRBURST_COUNT
#define COCO_EXPLOSION_BOMB_COUNT 5
#define COCO_EXPLOSION_NAPALM_BASE \
  (COCO_EXPLOSION_AIRBURST_COUNT + COCO_EXPLOSION_BOMB_COUNT)
#define COCO_EXPLOSION_NAPALM_COUNT 4
#define COCO_EXPLOSION_ANTI_PERSONELL_BASE COCO_EXPLOSION_AIRBURST_COUNT
#define COCO_EXPLOSION_ANTI_PERSONELL_COUNT \
  (COCO_EXPLOSION_BOMB_COUNT + COCO_EXPLOSION_NAPALM_COUNT)

coco_mini_point 
coco_cobra_rotor_points[COCO_COPTER_RASTER_COUNT];
coco_mini_point 
coco_cobra_weapon_hardpoints[COCO_COPTER_RASTER_COUNT];
coco_mini_point 
coco_cobra_center_of_mass[COCO_COPTER_RASTER_COUNT];
coco_mini_point 
coco_bubble_copter_rotor_points[COCO_COPTER_RASTER_COUNT];
coco_mini_point 
coco_bubble_copter_weapon_hardpoints[COCO_COPTER_RASTER_COUNT];
coco_mini_point 
coco_bubble_copter_center_of_mass[COCO_COPTER_RASTER_COUNT];

/* also change coco_ideal_frame_rate in gamesho.c */
extern struct timeval coco_ideal_frame_rate;
#define COCO_IDEAL_FRAMES_PER_SECOND 14
#define COCO_RESPAWN_SECONDS 4
#define COCO_RESPAWN_BASE_SNAP_SECONDS 1

coco_mini_point coco_tank_hardpoints[COCO_TANK_RASTER_COUNT];

coco_mini_point coco_gun_hardpoints[COCO_GUN_RASTER_COUNT];

struct coco_player {
  coco_bool in_use; /* must be true before referencing other fields */
  unsigned int index;           /* index into coco_players[] */
  coco_nationality nationality;
  coco_bool sent_play_message; /* defined server side only */

  /* these should be in the sstate, not here */
  float funds; /* defined server side only, accessed through
                coco_add_funds and coco_subtract_funds_available */
  coco_u32 copters; /* defined server side only */

  char name[COCO_PLAYER_NAME_MAX];
};

/* clients see the variable coco_have_game in client.c,
   which must be true before you can look at this array.
   This guy is initialized in coco_network_shared_setup() */
extern struct coco_player coco_players[COCO_MAX_PLAYERS];

/* SOUND */

/* coco_sound_type identifies the type of a sound.  Whether it is a scream
   or an explosion.  Actuall sounds are sent to the client using
   sobjects/cobjects, that have a coco_sound_type identifying what
   recording to play. */
typedef unsigned char coco_sound_type;


/* extern coco_sound_type coco_max_loaded_sound; private to client */

#define COCO_SOUND_AIRBURST           0
#define COCO_SOUND_MISSILE_LAUNCH     1
#define COCO_SOUND_COPTER_CANNON      2
#define COCO_SOUND_SPARK              3
#define COCO_SOUND_ROCKET_HIT         4
#define COCO_SOUND_BOMB_HIT           5
#define COCO_SOUND_SCREAM1            6
#define COCO_SOUND_SCREAM2            7
#define COCO_SOUND_SCREAM3            8
#define COCO_SOUND_ROCKET_LAUNCH      9
#define COCO_SOUND_BIG_AIRBURST       10
#define COCO_SOUND_RIFLE              11
#define COCO_SOUND_TANK_GUN           12
#define COCO_SOUND_COPTER_TAKE_OFF    13


/* sounds get 0-(coco_game_sounds_size - 1), and have some arrays that big
   in gamesho. */
#define COCO_GAME_SOUNDS_SIZE         14


#define COCO_FX_NONE             255

extern char *coco_game_sound_names[COCO_GAME_SOUNDS_SIZE];
/* in seconds */
extern float coco_game_sound_durations[COCO_GAME_SOUNDS_SIZE];

extern coco_gnum coco_game_fx_radius[COCO_GAME_SOUNDS_SIZE];

/* sound.c -- a virtual source file supported by (at least) mute-sound.c
   and openal-sound.c */
/* Api split into a seperate file to promote porting. */
#include "sound.h"

/* screenshot.c */
coco_bool coco_screenshot( char *file_name );

/* client.c */

/* These may change if the user figures out how to resize despite our best
   efforts.  They are not used by the server.  The game graphics are
   allowed to break if the user resizes the window. */
extern coco_gnum coco_real_window_w;
extern coco_gnum coco_real_window_h;

extern char coco_game_status_string[COCO_GAME_STATUS_SIZE];

typedef int coco_clientstate;
#define COCO_CLIENT_TABULA_RASA 0
#define COCO_CLIENT_DEMO 1 /* splash movie or idle thereafter */
#define COCO_CLIENT_CHOOSE_SERVER_DIALOG 2
#define COCO_CLIENT_CHATROOM_DIALOG 3
#define COCO_CLIENT_PLAY 4
#define COCO_CLIENT_IN_GAME_ESCAPE_DIALOG 5
#define COCO_CLIENT_GAME_OVER 6

coco_clientstate coco_get_client_state( void );
void coco_set_client_state( coco_clientstate state );

void coco_pointer_button_hold( unsigned int btn, coco_gnum x,
                               coco_gnum y, void *arg );
void coco_pointer_motion( coco_bool change, coco_gnum x,
                           coco_gnum y, void *arg );
void coco_key_press( coco_key key, char *string_or_null, 
                     coco_key_state_ref keystate );
void coco_key_release( coco_key key, coco_key_state_ref keystate );
/* void coco_key_hold( coco_key key ); Not implemented or called */

void coco_refresh_window();
void coco_init_starscape( void );

/* super light version of coco_sobject to save network bandwidth */
typedef union coco_cobject coco_cobject;

struct coco_every_cobject {
  unsigned char class_index;
  char pad;
  coco_cnum x;
};

struct coco_raster_cobject {
  unsigned char class_index;
  unsigned char raster_index;
  coco_cnum x;
};


struct coco_angle_cobject {
  unsigned char class_index;
  char pad;
  coco_cnum x;
  coco_cnum pad2;
  coco_angle angle;
};

/* "inherits" from coco_raster_cobject and angle_cobject */
struct coco_raster_barrel_cobject {
  unsigned char class_index;
  unsigned char raster_index;
  coco_cnum x;
  coco_cnum pad;
  coco_angle barrel_angle;
};

struct coco_y_cobject {
  unsigned char class_index;
  char pad;
  coco_cnum x;
  coco_cnum y;
};

struct coco_sound_cobject {
  unsigned char class_index;
  coco_sound_type sound_type;
  coco_cnum x;
  coco_cnum y;
};

struct coco_raster_y_cobject {
  unsigned char class_index;
  unsigned char raster_index;
  coco_cnum x;
  coco_cnum y;
};

/* 16 bit signed height instead of raster and angle (for balloon lines) */
struct coco_yh_cobject {
  unsigned char class_index;
  unsigned char hibyte;
  coco_cnum x;
  coco_cnum y;
  unsigned char lobyte;
};

struct coco_spark_cobject {
  unsigned char class_index;
  signed char dx;
  coco_cnum x;
  coco_cnum y;
  signed char dy;
};


/* "inherits" from coco_raster_y_cobject */
struct coco_metal_shrapnel_cobject {
  unsigned char class_index;
  unsigned char raster_index;
  coco_cnum x;
  coco_cnum y;
  coco_color_index color;
};

struct coco_raster_y_angle_cobject {
  unsigned char class_index;
  unsigned char raster_index;
  coco_cnum x;
  coco_cnum y;
  coco_angle angle;
};


union coco_cobject {
  struct coco_every_cobject every;
  struct coco_y_cobject y;
  struct coco_yh_cobject yh;
  struct coco_angle_cobject a;
  struct coco_raster_cobject r;
  struct coco_raster_barrel_cobject raster_barrel;
  struct coco_raster_y_cobject ry;
  struct coco_sound_cobject sound;
  struct coco_raster_y_angle_cobject rya;
  struct coco_metal_shrapnel_cobject metal_shrapnel;
  struct coco_spark_cobject spark;
};

#define COCO_COBJECT_CL_INDEX( cobject )\
  ((cobject)->every.class_index)
#define COCO_COBJECT_CL( cobject ) \
  (coco_sobject_classes + (COCO_COBJECT_CL_INDEX( cobject )))
#define COCO_COBJECT_X( cobject ) ((cobject)->every.x)
#define COCO_COBJECT_Y( cobject ) (coco_cobject_y_helper( cobject ))
#define COCO_COBJECT_Z( cobject ) (COCO_COBJECT_CL( cobject )->z)
#define COCO_COBJECT_RASTER_INDEX( cobject ) (coco_cobject_raster_index_helper( cobject ))
#define COCO_COBJECT_RASTER( cobject ) \
  COCO_COBJECT_CL( cobject )->rasters[COCO_COBJECT_RASTER_INDEX( cobject )]
#define COCO_COBJECT_SHRAPNEL_COLOR_INDEX( cobject ) \
  ((cobject)->metal_shrapnel.color)
/* only defined on tanks and guns: */
#define COCO_COBJECT_ANGLE( cobject ) \
  ((cobject)->a.angle)
/* Only defined for yh (balloon_line) cobjects */
#define COCO_COBJECT_H( cobject ) \
  (((int)((cobject)->yh.hibyte<<8))|((int)((cobject)->yh.lobyte)))
#define COCO_COBJECT_SPARK_DX( cobject ) ((cobject)->spark.dx)
#define COCO_COBJECT_SPARK_DY( cobject ) ((cobject)->spark.dy)
#define COCO_COBJECT_SOUND( cobject ) ((cobject)->sound.sound_type)

coco_cnum coco_cobject_y_helper( coco_cobject *cobject );
coco_cnum coco_cobject_ground_unit_y( coco_cobject *cobject );
coco_cnum coco_cobject_y_y( coco_cobject *cobject );

unsigned char coco_cobject_raster_index_helper( coco_cobject *cobject );
unsigned char coco_cobject_simple_raster_index( coco_cobject *cobject );
unsigned char coco_cobject_standard_raster_index( coco_cobject *cobject );

/* moved to after the definition of coco_sobject_cl */
/*
void coco_cobject_rasteroid_draw( coco_cobject *cobject, 
                                  coco_sobject_cl *cla );
*/

/* super-duper light radar blip 32 bit number */
struct coco_cblip {
  coco_rxnum x;
  coco_rynum y;
  coco_color_index color_index;
};


#define COCO_CSTATE_BUFFER_SIZE( cobject_count, cblip_count ) \
     (sizeof( struct coco_cstate_serial_header ) \
      + (cobject_count) * sizeof( coco_cobject ) \
      + (cblip_count)   * sizeof( struct coco_cblip ))

void coco_play_intro_movie( void );

void coco_client_exec( void );
struct coco_stream *coco_client_connect_to_server( enum coco_network_error *err_return, 
                                                   int *err_arg_return );

void coco_client_shutdown( coco_bool exit_too );

void coco_replay_intro_movie( void );

typedef int coco_notification_id;
typedef void (* message_fn)( void *, union coco_message * );

coco_bool coco_send_join_message( coco_nationality nat );
coco_bool coco_chat( char *message );

coco_notification_id 
coco_add_message_notification( int message_type, message_fn fn, void *data );


void
coco_delete_message_notification( int message_type, coco_notification_id id );


void coco_display_notice( char *short_string );

extern coco_bool coco_have_game_message;



coco_nationality coco_get_player_nationality( coco_nationality de_fault );


void coco_client_disconnect( void );
void coco_send_play_message_maybe( void );


/* server.c */

#define COCO_GROUND_UNIT_WARP_IN_X 80
#define COCO_GROUND_UNIT_WARP_OUT_X 600
#define COCO_FIRETRUCK_RELOAD_X 500
#define COCO_COPTER_MAX_ANGLE_OF_ATTACK 10 /* coco_angle = 14 degrees */
#define COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK 8 /* coco_angle */
#define COCO_COPTER_MAX_ANGULAR_SPEED 4 /* coco_angle/frame */
/* DX_MAX and DY_MAX are true for gunship, not true for bubble_copter,
   which is slower. */
#define COCO_COPTER_DX_MAX 22
#define COCO_COPTER_DY_MAX 14
#define COCO_BULLET_SPEED 30 /* can't be higher than balloon_w */
#define COCO_BULLET_LIFESPAN 20
#define COCO_BULLET_RANGE \
  (COCO_BULLET_SPEED * (COCO_BULLET_LIFESPAN-1))
#define COCO_TANK_GUN_MAX_ELEVATION 20
/* slowing down the tank bullets is good, because it helps them hit
   those oh-so-thin infantry.  */
#define COCO_TANK_INFANTRY_BULLET_SPEED 10
#define COCO_INFANTRY_GUN_RANGE (COCO_TANK_INFANTRY_BULLET_SPEED \
                                 * (COCO_BULLET_LIFESPAN-1))
#define COCO_TANK_GUN_RANGE (COCO_TANK_INFANTRY_BULLET_SPEED\
                             * (COCO_BULLET_LIFESPAN - 1))
#define COCO_FIRETRUCK_RANGE COCO_OFFICIAL_WINDOW_W
#define COCO_HEADQUARTERS_MISSILE_RANGE ((COCO_OFFICIAL_WINDOW_W>>1)+100)
#define COCO_MISSILE_ACCELL (75/COCO_IDEAL_FRAMES_PER_SECOND)
#define COCO_MISSILE_MAX_ANGULAR_SPEED (180/COCO_IDEAL_FRAMES_PER_SECOND)
#define COCO_GRAVITY_ACCELL 3
#define COCO_FLYING_MAN_ACCELL 1
#define COCO_BULLET_DAMAGE 1
#define COCO_BOMB_DAMAGE 4
#define COCO_MISSILE_DAMAGE 12
#define COCO_SHRAPNEL_DAMAGE 1
#define COCO_COPTER_DAMAGE 15
#define COCO_GROUND_Y 8
#define COCO_GROUND_UNIT_DX_MAX 1
#define COCO_GUN_BARREL_LENGTH 8 /* must be power of 2 */
#define COCO_BALLOON_FLOOR (COCO_GROUND_Y + 35) /* higher than highest bldng */
#define COCO_BALLOON_LINE_CEILING \
 (COCO_VIEWABLE_H - 100 - COCO_BALLOON_E_RASTER_H)
#define COCO_HUT_BALLOON_REINCARNATION_FRAMES \
 (COCO_IDEAL_FRAMES_PER_SECOND<<2)
#define COCO_INFANTRY_CHUTE_Y 16
#define COCO_ROCKET_LEFT_ANGLE 142
#define COCO_ROCKET_RIGHT_ANGLE 246

/* This implementation is unsafe in situations where sstate->sobjects or
   sstate->sobject_size might change, iterate manually. */

#define COCO_DO_SSTATE_OBJECTS( sobject_var, sstate, body )\
{\
  coco_sobject *sobject_var = (sstate)->sobjects;\
  coco_sobject *coco__sobject_end__= sobject_var + (sstate)->sobject_size;\
  for (; sobject_var < coco__sobject_end__; sobject_var++)\
    if (COCO_SOBJECT_IN_USE( sobject_var ))\
      body\
}

#include "forward-sobject.h"
#include "methods.h" /* COCO_SOBJECT_RASTER( sobject ) function in here */
#include "sobject.h"

/* Pseudo accessors. */

#define COCO_SOBJECT_RASTER( sobject, index ) \
 (COCO_SOBJECT_RASTERS( sobject )[index])
#define COCO_SOBJECT_X1( sobject ) \
 (COCO_SOBJECT_X( sobject ) + COCO_SOBJECT_W( sobject ))
#define COCO_SOBJECT_X_CENTER( sobject ) \
 (COCO_SOBJECT_X( sobject ) + (COCO_SOBJECT_W( sobject )>>1))
#define COCO_SOBJECT_Y1( sobject ) \
 (COCO_SOBJECT_Y( sobject ) + COCO_SOBJECT_H( sobject ))
#define COCO_SOBJECT_Y_CENTER( sobject ) \
 (COCO_SOBJECT_Y( sobject ) + (COCO_SOBJECT_H( sobject )>>1))

/* A minor substructure of coco_sstate */
struct coco_team {
  int landing_pad_index;
  coco_bool summon_enable;       /* true if can summon */
  coco_u32 summon_disable_until; /* a time */
};

struct coco_nomination_status {
  unsigned char action_code; /* COCO_NOMINATE_* above */
  unsigned char act_object;  /* interpretation varies by action_code */
  coco_bool votes[COCO_MAX_PLAYERS];
};


/* Holds the state of the game on the server */
struct coco_sstate_struct {
  coco_bool game_in_progress;
  coco_sobject *sobjects; /* read with COCO_SSTATE_OBJECT_REF */
  int sobject_fill_index; /* all entries before this are used */
  int sobject_size;       /* number of elts in sobjects */
  coco_u32 time;
  coco_gnum board_w;

  /* COCO_NEUTRAL == game in progress, otherwise nationality of winner. */
  coco_nationality game_over_nationality; 
  coco_u32 game_over_time;

  
  struct coco_team teams[2]; /* indexed by (nationality - 1) */
  /* only readable if the player is players[i].in_use: */
  int camera_indicies[COCO_MAX_PLAYERS]; /* -1 if currently no cam */
  coco_bool respawn[COCO_MAX_PLAYERS];
  coco_u32 respawn_times[COCO_MAX_PLAYERS];
  coco_bool comp_respawn[COCO_MAX_COMPS];
  coco_u32 comp_respawn_times[COCO_MAX_COMPS];
  coco_sobject_cl *favorite_copter_class[COCO_MAX_PLAYERS];
  struct coco_nomination_status nominations[COCO_MAX_NOMINATIONS];
  unsigned char nominations_cursor;
};

#define COCO_SSTATE_OBJECT_REF( sstate, obj_idx ) ((sstate)->sobjects + (obj_idx))

#define COCO_SSTATE_CAMERA( sstate, player_idx ) \
  ((sstate)->sobjects + (sstate)->camera_indicies[player_idx])

/* needs to loop through sstate.cameras and sstate.sobjects. */
void coco_sstate_delete_sobject( coco_sstate *sstate, 
                                 coco_sobject *sobject );

/* reader for sstate */
coco_sstate *coco_get_sstate( void );
coco_gnum coco_get_board_w( void );
void coco_server_exec( void );


coco_sobject *coco_sstate_allocate_sobject( coco_sstate *sstate );

void coco_sobject_set_raster_index( coco_sobject *sobject, unsigned char index );

void coco_free_fall_adjust( coco_sobject *sobject, coco_gnum ddy,
                            signed char friction_shift );

void coco_copter_start_coords( coco_sstate *sstate, coco_nationality nat,
                               int player_index,
                               coco_gnum *x_return, coco_gnum *y_return );

void coco_randomize_metal_shrapnel( coco_sobject *sobject );

void
coco_make_shrapnel( unsigned count, coco_sstate *sstate,
                    coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy );

coco_sobject *
coco_hut_deploy_balloon( coco_sobject *hut, coco_sstate *sstate );

void
coco_get_collision_recti( coco_sobject *sobject, coco_gnum *x0, coco_gnum *x1,
                          coco_gnum *y0, coco_gnum *y1 );


coco_sstate *
coco_init_sstate( coco_sstate *sstate, coco_gnum width );


coco_sobject *
coco_infantry_init_from_ground_by_list( coco_sobject *inst, 
                                        coco_sobject_cl *cla,
                                        long *arguments );
coco_sobject *
coco_init_infantry_from_ground( coco_sobject *inst, coco_sobject_cl *cla, 
                                coco_gnum x, coco_nationality nationality );

coco_sobject *
coco_hut_init_by_list( coco_sobject *sobject, coco_sobject_cl *cla, 
                       coco_lnum *args );

void coco_add_funds( int player_index, float funds );
coco_bool coco_subtract_funds_available( int player_index, float funds );


void
coco_make_snd( coco_sound_type sound, coco_sstate *sstate, 
               coco_gnum x, coco_gnum y );

struct coco_comp {
  coco_bool in_use;
  coco_nationality nationality;
  float funds;
  coco_u32 copters;
  struct coco_keyboard_pointer_state kpstate;
  char name[COCO_PLAYER_NAME_MAX];
};


/* comp.c */

void
coco_comp_think( struct coco_comp *self, 
                 struct coco_cstate_serial_header *cstate,
                 coco_cobject *cobjs,
                 struct coco_cblip *cblips,
                 struct coco_keyboard_pointer_state *state_return );


/* late gamesho.c */
void coco_sobject_weapon_hardpoint_coords( coco_sobject *sobject,
                                           coco_gnum *x_return,
                                           coco_gnum *y_return );

/* ccl.h */

char *coco_map_level_text( char *filename, char **error_message );
char *coco_load_level_definition( char *from, coco_sstate **sstate_return );


/* globals.c : parsed command line switches */
extern int coco_command_line_inhibit_double_buffer;
extern int coco_port;
extern FILE *coco_server_log; /* write using coco_log_printf() */
extern char *coco_server;
extern coco_bool coco_server_specified_p;
extern coco_bool coco_server_malloced_p;
extern int coco_blend_alpha;
extern int coco_jamming;
extern char *coco_level_text;
extern const char *coco_level_directory;
extern const char *coco_sound_directory;
extern const char *coco_keybindings;
extern const char *coco_mini_manual;

coco_bool coco_version_compatible_p( short major, short minor, 
                                     enum coco_version_domain domain );


/* command-line.c -- a virtual file supported by simple-command-line.c and
   popt-command-line.c */

coco_bool coco_process_server_arguments( int argc, char **argv );
coco_bool coco_process_client_arguments( int argc, char **argv );


/* main.c */

/* nothing exported -- which is good, since main won't be compiled in the gnome branch. */

#endif /* COCO_H */
