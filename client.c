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


#include <string.h>             /* for strncpy */
#include <sys/time.h>           /* for select */
#include <sys/types.h>          /* for select */
#include <unistd.h>             /* for select */
#include <errno.h>
#include <netinet/in.h>         /* for ntohs */
#include <netdb.h>              /* for gethostbyname */
#include <stdlib.h>             /* for rand() */
#include <assert.h> 
#include <math.h>               /* for cos */
#include <GL/gl.h>              /* sadly I decided to use GL here, though
                                   I'd like to just use graphics.c
                                   functions. */
#include <time.h>              /* for all the time functions */
#include <sys/socket.h>

#include "coco.h"
#include "background.h"
#include "rasters.h"


/* this variable, somewhat new, controls how much unneccesary graphics
   touches get put into the game.  0 means draw nothing unnessary, no
   matter how cheap, 10 is a typical consumer level accellerated OpenGL
   card of 2000.  Mesa wieghs in around 3.  This is not used everywhere it
   could be. -1 means it hasn't been initialzed yet, i.e. before
   coco_setup_misc_graphics().  */
int coco_eye_candy = -1;



/* _Window dimensions______________________________________________ */

/* these are read-only except to the *-window.c files */
coco_gnum coco_real_window_w = COCO_OFFICIAL_WINDOW_W;
coco_gnum coco_real_window_h = COCO_OFFICIAL_WINDOW_H;


/* ________________________________________________________________ */
static coco_bool coco_old_votestate_initialized = COCO_FALSE;
static struct coco_message_votestate coco_old_votestate;

/* ________________________________________________________________ */


/* only referenceable if coco_have_game_message. */
char coco_game_status_string[COCO_GAME_STATUS_SIZE];

void coco_call_message_notifications( int message_type, 
                                      union coco_message *message );

void coco_draw_window( void );

coco_u32 coco_draw_frame_number = 0; /* for strobing animations,
                                               goes up every redraw. */

void coco_draw_ground( coco_gnum height );

coco_bool coco_play_intro_movie_frame( coco_bool advance_frame, 
                                       coco_bool rewind );

int coco_get_client_udp_fd( void );
void coco_set_client_udp_fd( int value );
struct coco_stream *coco_get_server_message_stream( void );

void coco_write_client2server_packet( int udpfd, 
                                      struct coco_keyboard_pointer_state *
                                      input_state );


coco_clientstate coco_client_state = COCO_CLIENT_TABULA_RASA;

/* only accessible after network conection, e.g. clientstate
   COCO_CLIENT_CHATROOM_DIALOG ++.  */
static coco_u32 coco_server_addr_in_nbo = 0;

/* Controlls access to players[] */
coco_bool coco_have_game_message = COCO_FALSE;
/* whether we can send udp */
static coco_bool coco_have_port_message = COCO_FALSE;
/* whether we've gotten license to play (more or less equivalent to
   gamestate 4 at the mome). */
static coco_bool coco_have_play_message = COCO_FALSE;

static unsigned char coco_my_player_id; /* accessible if coco_have_game_message */


static coco_sound coco_game_sound_map[COCO_GAME_SOUNDS_SIZE];


coco_nationality 
coco_get_player_nationality( coco_nationality de_fault )
{
  return coco_have_game_message
    ? coco_players[coco_my_player_id].nationality 
    : de_fault;
}


static struct coco_stream coco_server_stream;

coco_clientstate coco_get_client_state( void )
{
  return coco_client_state;
}

void coco_set_client_state( coco_clientstate state )
{
  coco_client_state = state;
}

/* chat globals */
coco_bool coco_in_game_chat_mode = COCO_FALSE;
/* yes I worried about the NULL */
char coco_headered_in_game_chat_buffer[COCO_CHAT_MAX + 7] = "SEND:  ";
char *coco_in_game_chat_buffer = coco_headered_in_game_chat_buffer + 7;
int coco_in_game_chat_cursor = 0;

/* When true, don't send key info to server (except mouse buttons) */
static coco_bool coco_censor_keys = COCO_FALSE;

void coco_note_talk( void *null, union coco_message *message );
void coco_draw_talk( long tv_sec_now );

void coco_note_votestate( void *null, union coco_message *message );

/* _cobject        _____________________________________________________ */

/* the generic function */
coco_cnum 
coco_cobject_y_helper( coco_cobject *cobject )
{
  return COCO_COBJECT_CL( cobject )->cobject_y_fn( cobject );
}

/* method for ground units */
coco_cnum 
coco_cobject_ground_unit_y( coco_cobject *cobject )
{
  return COCO_GROUND_Y;
}

/* method for air units */
coco_cnum 
coco_cobject_y_y( coco_cobject *cobject )
{
  return cobject->y.y;
}

/* the generic function */
unsigned char 
coco_cobject_raster_index_helper( coco_cobject *cobject )
{
  return COCO_COBJECT_CL( cobject )->cobject_raster_index_fn( cobject );
}

/* method for single-raster units */
unsigned char 
coco_cobject_simple_raster_index( coco_cobject *cobject )
{
  return 0;
}

/* method for units which have their raster sent over the wire */
unsigned char 
coco_cobject_standard_raster_index( coco_cobject *cobject )
{
  return cobject->r.raster_index;
}

/* the generic function.  Returns -1 on don't know, or on success 
   cast result to COCO_NATIONALITY */
int 
coco_cobject_guess_nationality_helper( coco_cobject *cobject )
{
  return COCO_COBJECT_CL( cobject )->cobject_guess_nationality_fn( cobject );
}

/* default method */
int
coco_cobject_whimpy_guess_nationality( coco_cobject *cobject )
{
  return -1;
}

int
coco_mech_guess_nationality( coco_cobject *cobject )
{
  return COCO_COBJECT_MECH_NATIONALITY( cobject );
}

int
coco_raster_guess_nationality( coco_cobject *cobject )
{
  return COCO_COBJECT_RASTER_INDEX( cobject );
}

int
coco_copter_guess_nationality( coco_cobject *cobject )
{
  return (COCO_COBJECT_RASTER_INDEX( cobject ) < 2)
    ? COCO_GOOD_GUYS
    : COCO_BAD_GUYS;
}

int
coco_neutral_guess_nationality( coco_cobject *cobject )
{
  return (int) COCO_NEUTRAL;
}

int
coco_flying_player_name_guess_nationality( coco_cobject *cobject )
{
  return (int) (cobject->fpn.bits & 1) + COCO_GOOD_GUYS;
}

int
coco_hut_guess_nationality( coco_cobject *cobject )
{
  return (int) COCO_COBJECT_RASTER_INDEX( cobject );
}

int
coco_2raster_guess_nationality( coco_cobject *cobject )
{
  return (int) COCO_COBJECT_RASTER_INDEX( cobject ) + COCO_GOOD_GUYS;
}

int
coco_firetruck_guess_nationality( coco_cobject *cobject )
{
  return (COCO_COBJECT_RASTER_INDEX( cobject ) < 2)
    ? COCO_GOOD_GUYS
    : COCO_BAD_GUYS;
}

int
coco_demo_van_guess_nationality( coco_cobject *cobject )
{
  return (COCO_COBJECT_RASTER_INDEX( cobject ) < 2)
    ? COCO_GOOD_GUYS
    : COCO_BAD_GUYS;
}


int
coco_infantry_guess_nationality( coco_cobject *cobject )
{
  unsigned char ri = COCO_COBJECT_RASTER_INDEX( cobject );
  if (ri<5)
    return COCO_GOOD_GUYS;
  else if (ri<10)
    return COCO_BAD_GUYS;
  else if (ri==14)
    return COCO_GOOD_GUYS;
  else if (ri==15)
    return COCO_BAD_GUYS;
  else 
    return -1;
}




struct coco_cstate *
coco_get_cstate( void )
{
  static struct coco_cstate cstate = { NULL, 0, COCO_U32_MAX, COCO_FALSE };
  return &cstate;
}

void
coco_initialize_cstate( struct coco_cstate *cstate )
{
  cstate->in_use = COCO_FALSE;
}

#define COCO_DISCARD_PACKET( buf ) \
  recv( udpfd, &(buf), sizeof( buf ), 0 )


struct coco_cstate *
coco_read_cstate( udpfd )
{
  struct coco_cstate *cstate = coco_get_cstate();
  struct coco_cstate_serial_header header;
  int recv_result;
  coco_u32 desired_buff_size, cobject_count, cblip_count;
  int got_at_least_one_cstate = 0;

  while (1) /* for (loops=0; loops<5; loops++) */
    {
      /*
      coco_cookie( "reading cstate.  Preliminary recv (%s)...",
                   coco_get_fd_blocking( udpfd )
                   ? "BLOCKING" : "non-blocking"); */
      recv_result = recv( udpfd, &header, sizeof( header ), MSG_PEEK );
      /* coco_cookie( "done.\n" ); */

      /* read_header: */
      if ((-1 == recv_result))
        if  (EAGAIN == errno) 
          return got_at_least_one_cstate ? cstate : NULL;
        else coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                                "recv failed with errno=%d: %s\n", errno, 
                                coco_errno_string( errno ) );
      else if (sizeof( header ) > recv_result)
        {
          coco_warn_printf( "Only recv'ed %d bytes, but expected at least %d.\n"
                            "Chucking this packet and continuing with fingers"
                            " crossed.\n",
                            recv_result, sizeof( header ));
          COCO_DISCARD_PACKET( header );
          return got_at_least_one_cstate ? cstate : NULL;
        }
      coco_cstate_serial_header_ntoh( &header );
      /* figure out, based on the times, if we want this packet */
      {
        coco_u32 last_time = cstate->time;
        if (cstate->in_use && !time_less_than( last_time, header.time ) )
          {
            if (header.time && last_time)
              coco_warn_printf( "Got a packet from %d oclock, but discarding"
                                " cuz its now %d oclock.\n", header.time, 
                                last_time );
            COCO_DISCARD_PACKET( header );
            return got_at_least_one_cstate ? cstate : NULL;
          };
      }
      cobject_count     = header.cobject_count;
      cblip_count       = header.cblip_count;
      desired_buff_size = COCO_CSTATE_BUFFER_SIZE( cobject_count,
                                                   cblip_count );
      /* ensure raw_buffer big enough */
      if (cstate->raw_buffer_size < desired_buff_size )
        { 
          int requested_size = COCO_MAX( desired_buff_size + 256, 1024 );
          cstate->raw_buffer_size = requested_size;
          cstate->raw_buffer = coco_realloc( cstate->raw_buffer, requested_size );
        }
      /* coco_cookie( "about to recv..." ); */
      recv_result = recv( udpfd, cstate->raw_buffer, desired_buff_size, 0 );
      /* coco_cookie( "done.\n" ); */
      if ((-1 == recv_result))
        if  (EAGAIN == errno)
          return got_at_least_one_cstate ? cstate : NULL;
        else coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                                "recv failed with errno=%d: %s\n", errno, 
                                coco_errno_string( errno ) );
      else if (desired_buff_size > recv_result)
        {
          coco_warn_printf( "Only recv'ed %d bytes, but expected at"
                            " least %d.\n"
                            "Chucking this packet and continuing with fingers"
                            " crossed.\n",
                            recv_result, desired_buff_size );
          return NULL;
        }

      /* this code made sure there's nothing else in queue before
         proccessing this dude.  However time to process packets is so
         low, (70 microseconds) that its not worth the chance that the
         packets might be out of order, (and then dropped).

      recv_result = recv( udpfd, minibuffer, sizeof( minibuffer ), MSG_PEEK );
      if ((-1 != recv_result))
        goto read_header;
      else if (EAGAIN != errno)
        coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                           "recv failed with errno=%d: %s\n", errno, 
                           coco_errno_string( errno ) ); */

      /* actually write data ... except for buffer sizing, this is the
         first non-local side effect.  */
      got_at_least_one_cstate = 1;
      cstate->time = header.time;
      cstate->in_use = COCO_TRUE;
      cstate->game_over_nationality = header.game_over_nationality;
      cstate->camera_x = header.cam_left;
      cstate->board_w = header.board_w;
      cstate->cobjects
        = (coco_cobject *) ((char *) cstate->raw_buffer + sizeof( header ));
      cstate->cobjects_length = cobject_count;
      cstate->cblips
        = (struct coco_cblip *)(cstate->cobjects + cobject_count);
      cstate->cblips_length = cblip_count;
      cstate->major_mission = header.major_mission;
      cstate->minor_mission = header.minor_mission;
      cstate->angle_on = header.angle_on;
      if (header.angle_on)
        cstate->angle = header.angle;
      cstate->dashinfo = header.dashinfo;


      /* Obsolete
      cstate->game_over_nationality 
        = COCO_U32_NTOH( ((coco_u32 *) cstate->raw_buffer)[3] );
      cstate->camera_x 
        = COCO_U32_NTOH( ((coco_u32 *) cstate->raw_buffer)[4] );
      cstate->cobjects
        = (coco_cobject *)
        ((char *) cstate->raw_buffer + 
         COCO_CSTATE_HEADER_SIZE_IN_WORDS*sizeof( coco_u32 ));
      cstate->cobjects_length = cobject_count;
      cstate->cblips = (struct coco_cblip *)(cstate->cobjects + cobject_count);
      cstate->cblips_length = cblip_count;
      */
      /* convert the read data from network byte order to host byte order */
      /* Argh, if I'm ansi I can't include <endian.h>, so for now I pass this
         on the command line */
#ifdef COCO_NOT_BIG_ENDIAN
      {
        int i;
        for (i=cstate->cobjects_length - 1; i>=0; i--)
          {
            coco_cobject *cobject = cstate->cobjects + i;
            COCO_COBJECT_CL( cobject )->cobject_ntoh( cobject );
          }
        for (i=cstate->cblips_length - 1; i>=0; i--)
          {
            struct coco_cblip cblip = cstate->cblips[i];
            cblip.x = COCO_RXNUM_NTOH( cblip.x );
            cblip.y = COCO_RYNUM_NTOH( cblip.y );
            cstate->cblips[i] = cblip;
          }
      }
#endif  /* COCO_NOT_BIG_ENDIAN */
    }
  return got_at_least_one_cstate ? cstate : NULL;
}

/* Debugging
struct coco_cstate *
coco_read_cstate( udpfd )
{
  struct coco_cstate *result;
  coco_cookie( "Reading cstates\n" );
  result = coco_read_cstate_internal( udpfd );
  coco_cookie( "Reading cstates done\n" );
  return result;
}
*/

#undef COCO_DISCARD_PACKET


void
coco_metal_shrapnel_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_sobject_cl *scla = cla;
  coco_color color
    = COCO_COLOR_INDEX( COCO_COBJECT_SHRAPNEL_COLOR_INDEX( cobject ) );
  coco_draw_color( color );
  /* calculate and call next method */
  do scla = scla->super;
  while (scla->cobject_draw == coco_metal_shrapnel_draw);
  scla->cobject_draw( cobject, cla->super );
}

void
coco_flying_player_name_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_nationality name_nat = COCO_COBJECT_NATIONALITY_GUESS( cobject );
  coco_nationality cam_nat = coco_get_player_nationality( COCO_NEUTRAL );
  int bits = cobject->fpn.bits;
  coco_bool is_comp = (bits & 2) != 0;
  int p_or_c_index = bits >> 2;
  coco_color color = (name_nat == cam_nat) 
    ? COCO_COLOR_FRY_FLYING_PLAYER_NAME
    : COCO_COLOR_EMY_FLYING_PLAYER_NAME;
  coco_cnum x = COCO_COBJECT_X( cobject );
  coco_cnum y = COCO_COBJECT_Y( cobject );
  const char *name = is_comp
    ? coco_comp_names[coco_comps[p_or_c_index].name_index]
    : coco_players[p_or_c_index].name;
  coco_draw_color( color );
  coco_draw_string( x, y, name, -1, &coco_default_font, COCO_BOTTOM );
}


void
coco_bullet_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_gnum x = COCO_COBJECT_X( cobject );
  coco_gnum y = COCO_COBJECT_Y( cobject );
  unsigned char thickness = COCO_COBJECT_CL( cobject )->thickness;
  if (1==thickness)
    {
      coco_draw_color( COCO_COLOR_SMALL_BULLET );
      coco_draw_line( x, y, x, y-1 );
    }
  else 
    {
      coco_draw_color( COCO_COLOR_BULLET );
      coco_draw_big_point( x, y, thickness );
    }
}

void
coco_plasma_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  /* only tricky thing here is we draw the raster 
     125% of the radius given, because the raster doesn't extend
     out to the corners of the square, the user needs to think its
     bigger than it is. */
  unsigned char bits = cobject->plasma.bits;
  coco_cnum x = COCO_COBJECT_X( cobject );
  coco_cnum y = COCO_COBJECT_Y( cobject );
  int radius = bits >> 1;
  int dv = radius>>2; /* this is the 25% */
  int vradius = dv + radius;
  int coherent = bits & 1;
  int vdia =  vradius + radius;
  struct coco_raster *r = COCO_COBJECT_RASTER( cobject );
  int a = 127 + (coherent<<7);
  coco_draw_color( COCO_MAKE_COLOR( 255, 255, 255, a ) );
  coco_draw_raster_texmap( x-dv, y-dv, x+vdia, y+vdia, r->w, r->h, r );
}



void
coco_cobject_rasteroid_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_draw_raster( COCO_COBJECT_X( cobject ), 
                    COCO_COBJECT_Y( cobject ),
                    COCO_COBJECT_RASTER( cobject ) );
}

void
coco_cobject_inviso_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  ; /* don't draw invisble things, get it? */
}

/* sound "draws" by making noise. */
void
coco_cobject_sound_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  /* coco_cookie( "Sound map %d -> %d\n", COCO_COBJECT_SOUND( cobject ),
     coco_game_sound_map[COCO_COBJECT_SOUND( cobject )] ); */
  coco_sound_type type = COCO_COBJECT_SOUND( cobject );
  coco_play_sound( coco_game_sound_map[type], type,
                   COCO_COBJECT_X( cobject ),
                   COCO_COBJECT_Y( cobject ) );
}


void
coco_cobject_barreloid_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_sobject_cl *scla = cla;
  coco_sobject_cl *true_cla = COCO_COBJECT_CL( cobject );
  unsigned char ri = COCO_COBJECT_RASTER_INDEX( cobject );
  coco_mini_point hp = true_cla->weapon_hardpoints[ri];
  coco_cnum x = COCO_COBJECT_X( cobject ) + COCO_MINI_POINT_X( hp );
  coco_cnum y = COCO_COBJECT_Y( cobject ) + COCO_MINI_POINT_Y( hp );
  coco_angle a = COCO_COBJECT_ANGLE( cobject );
  unsigned char blength = true_cla->gun_barrel_length;
  glPushAttrib( GL_LINE_BIT );
  glLineWidth( 3.0 );
  coco_draw_color( COCO_COLOR_BARREL );
  coco_draw_line( x, y, COCO_COS( a )*blength + x, COCO_SIN( a )*blength + y);
  glPopAttrib();
  /* calculate and call next method */
  do scla = scla->super;
  while (scla->cobject_draw == coco_cobject_barreloid_draw);
  scla->cobject_draw( cobject, cla->super );
}

void
coco_balloon_line_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_cnum x = COCO_COBJECT_X( cobject );
  coco_cnum y = COCO_COBJECT_Y( cobject );
  coco_draw_color( COCO_COLOR_BARRAGE_BALLOON_LINE );
  coco_draw_line( x, y, x, y+COCO_COBJECT_H( cobject ) );
}

void
coco_balloon_factory_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  /* draw top 2/3 of smokestack, which is a seperate raster. */
  coco_sobject_cl *scla = cla;
  coco_cnum x = COCO_COBJECT_X( cobject );
  coco_cnum y = COCO_COBJECT_Y( cobject );
  coco_draw_raster( x+COCO_BALLOON_FACTORY_STACK_TAB, 
                    y+COCO_BALLOON_FACTORY_BASE_RASTER_H,
                    cla->rasters[1] );
  /* calculate and call next method */
  do scla = scla->super;
  while (scla->cobject_draw == coco_balloon_factory_draw);
  scla->cobject_draw( cobject, cla->super );
}


void
coco_missile_draw_by_coords( coco_gnum x, coco_gnum y, coco_angle angle, 
                             coco_sobject_cl *cl, coco_bool burn_engine )
{
  /* glMatrixMode(  GL_MODELVIEW ); the default -dm 05-11-2001 */
  glPushMatrix();
  glTranslatef( x, y, 0.0 );
  glRotatef( angle*(360.0/256.0), 0.0, 0.0, 1.0 );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  coco_draw_color( COCO_COLOR_GREY_8 );
  /* draw fins */
  if (coco_sobject_subtype_p( cl, COCO_MISSILE_CLASS ))
    {
      glBegin( GL_TRIANGLES );
      glVertex2i( -10, -6 );
      glVertex2i(   0,  0 );
      glVertex2i( -10,  6 );
      glVertex2i(  10, -5 );
      glVertex2i(  20,  0 );
      glVertex2i(  10,  5 );
      glEnd();
    }
  else
    {
      glBegin( GL_TRIANGLES );
      glVertex2i( -10, -6 );
      glVertex2i(   0,  0 );
      glVertex2i( -10,  6 );
      glEnd();
    }
  glBegin( GL_TRIANGLE_STRIP );
  if ((angle>48)&&(angle<138))
    coco_draw_color( COCO_COLOR_GREY_B );
  else if ((angle>170)&&(angle<245))
    coco_draw_color( COCO_COLOR_GREY_5 );
  else coco_draw_color( COCO_COLOR_GREY_8 );
  glVertex2i(  22,  0 );
  coco_draw_color( COCO_COLOR_GREY_8 );
  glVertex2i(  15, -2 );
  glVertex2i(  15,  2 );
  glVertex2i( -10, -2 );
  glVertex2i( -10,  2 );
  if (burn_engine)
    {
      coco_draw_color( COCO_COLOR_RED );
      glVertex2i( (- (rand()%15)) - 10,  0 );
    }
  glEnd();
  if ((angle>110)&&(angle<180))
    {
      coco_draw_color( COCO_COLOR_GREY_B );
      coco_draw_line( -10, -2, 15, -2 );
      coco_draw_color( COCO_COLOR_GREY_5 );
      coco_draw_line( -10, 2, 15, 2 );
    }
  else if ((angle<60))
    {
      coco_draw_color( COCO_COLOR_GREY_5 );
      coco_draw_line( -10, -2, 15, -2 );
      coco_draw_color( COCO_COLOR_GREY_B );
      coco_draw_line( -10, 2, 15, 2 );
    }
  else
    {
      coco_draw_color( COCO_COLOR_GREY_5 );

      if ((angle < 240) && (angle >= 180))
        coco_draw_line( -10, 2, 15, 2 );
      if (angle>=192)
        coco_draw_line( -10, -2, 15, -2 );
      if (angle < 100)
        coco_draw_line( -10, -2, 15, -2 );
      if ((angle>=75) && (angle<118))
        coco_draw_line( -10, 2, 15, 2 );
    }
  glPopMatrix();
}


void
coco_missile_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_angle angle = COCO_COBJECT_ANGLE( cobject );
  coco_gnum x = COCO_COBJECT_X( cobject );
  coco_gnum y = COCO_COBJECT_Y( cobject );
  coco_missile_draw_by_coords( x, y, angle, COCO_COBJECT_CL( cobject ), 
                               COCO_TRUE );
}

void
coco_spark_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  int rando = rand() & 255;
  coco_gnum x0, x1, y0, y1;
  coco_draw_color( COCO_MAKE_COLOR( 255, rando, rando>>1, 255 ) );
  y0 = COCO_COBJECT_Y( cobject );
  x0 = COCO_COBJECT_X( cobject );
  x1 = x0 + COCO_COBJECT_SPARK_DX( cobject );
  y1 = y0 + COCO_COBJECT_SPARK_DY( cobject );
  coco_draw_line( x0, y0, x1, y1 );
}

void
coco_copter_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_sobject_cl *cl = COCO_COBJECT_CL( cobject );
  coco_gnum cw = cl->canonical_w;
  coco_gnum ch = cl->canonical_h;
  coco_mini_point com = coco_cobject_com( cobject );
  coco_gnum comx = COCO_MINI_POINT_X( com );
  coco_gnum comy = COCO_MINI_POINT_Y( com );
  unsigned char ri = COCO_COBJECT_RASTER_INDEX( cobject );
  coco_angle angle_raw = COCO_COBJECT_ANGLE( cobject );
  coco_angle angle = (ri&1) ? angle_raw : ((angle_raw - (int) 128) & 255);

  coco_begin_rotated_view( COCO_COBJECT_X( cobject ) + comx,
                           COCO_COBJECT_Y( cobject ) + comy,
                           angle );
  coco_draw_raster_texmap( -comx, -comy,
                           cw-comx, ch-comy, 
                           cw, ch,
                           COCO_COBJECT_RASTER( cobject ) );
  {
    static unsigned char rotor_scale[8] = { 2, 5, 7, 4, 0, 3, 6, 5 };
    coco_mini_point rp;
    coco_gnum x, y, rr7, rx;
    unsigned char raster_index;

    /* draw rotor */
    raster_index = COCO_COBJECT_RASTER_INDEX( cobject );
    rp = cla->rotor_points[raster_index];
    rr7 = (cla->rotor_radius * rotor_scale[7 & coco_get_cstate()->time]);
    rx = ((double) rr7 / 7.0);
    x = COCO_MINI_POINT_X( rp ) - comx;
    y = COCO_MINI_POINT_Y( rp ) - comy;
    coco_draw_color( COCO_COLOR_ROTOR );
    coco_draw_line( x-rx, y, x+rx, y );
  }
  coco_end_rotated_view();
}



void
coco_cloud_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  if (coco_eye_candy >= 2)
    {
      /* calculate and call next method */
      coco_sobject_cl *scla = cla;
      do scla = scla->super;
      while (scla->cobject_draw == coco_cloud_draw);
      assert( scla );
      scla->cobject_draw( cobject, cla->super );
    }
  else
    {
      struct coco_raster *r = COCO_COBJECT_RASTER( cobject );
      coco_cnum x = COCO_COBJECT_X( cobject );
      coco_cnum y = COCO_COBJECT_Y( cobject );
      coco_cnum y1 = y+(r->h>>1)+5;
      coco_draw_color( COCO_COLOR_WHITE );
      coco_draw_rectangle( x+10, y+10, x+r->w-10, y1 );
      coco_draw_rectangle( x+20, y1, x+r->w-25, y+r->h-10 );
    }
}



void
coco_mech_draw( coco_cobject *cobject, coco_sobject_cl *cla )
{
  coco_cnum x = COCO_COBJECT_X( cobject );
  coco_cnum y = COCO_COBJECT_Y( cobject );
  unsigned char ti = COCO_COBJECT_MECH_TORSO_RASTER_INDEX( cobject );
  unsigned char li = COCO_COBJECT_MECH_LEG_RASTER_INDEX( cobject );
  struct coco_mech_part *tp;
  struct coco_mech_part *lp;
  coco_gnum lpdx, lpdy;

  /* added safety */
  ti = COCO_MIN( COCO_MAX_TORSO_PARTS-1, ti );
  li = COCO_MIN( COCO_MAX_LEG_PARTS-1, li );

  coco_draw_color( COCO_COLOR_WHITE );
  /*
    to see where it really is
  coco_draw_rectangle( x, y, 
                       x+COCO_MECH_CLASS->canonical_w,
                       y+COCO_MECH_CLASS->canonical_h );
  */
  lp = coco_leg_parts+li;
  lpdx = lp->dx;
  lpdy = lp->dy;
  coco_draw_raster( x+lpdx, y+lpdy, lp->r );
  tp = coco_torso_parts+ti;
  coco_draw_raster( x+tp->dx, y+tp->dy+lp->dh, tp->r );
}

/* XXX Should only disconnect gnome-copter-commander. */
void
coco_draw_game_over_message( coco_nationality nationality )
{
  char *game_over = "GAME OVER";
  char *who_wins = (COCO_GOOD_GUYS==nationality)
    ? "GREEN WINS" : "GREY WINS";
  char *bye = "window will close by itself";
  int go_w, go_h, ww_w, ww_h, bye_w, bye_h, dummy, total_h, cursor_y;
  coco_font_string_dimensions( game_over, -1, &coco_huge_font,
                               &go_w, &go_h, &dummy );
  coco_font_string_dimensions( who_wins, -1, &coco_huge_font, &ww_w, &ww_h, 
                               &dummy );
  coco_font_string_dimensions( bye, -1, &coco_default_font, 
                               &bye_w, &bye_h, &dummy );
  total_h = go_h + 10 + ww_h + 10 + bye_h;
  coco_draw_color( COCO_COLOR_FONT );
  cursor_y = (coco_real_window_h + total_h)>>1;
  coco_draw_string( (coco_real_window_w - go_w)>>1,
                    cursor_y, game_over, -1, &coco_huge_font,
                    COCO_TOP );
  cursor_y -= go_h + 10;
  coco_draw_string( (coco_real_window_w - ww_w)>>1,
                    cursor_y,
                    who_wins, -1, &coco_huge_font,
                    COCO_TOP );
  cursor_y -= ww_h + 10;
  coco_draw_string( (coco_real_window_w - bye_w)>>1,
                    cursor_y, bye, -1, &coco_default_font,
                    COCO_TOP );
  cursor_y -= bye_h + 10;
}


long coco_chat_cursor_last_blink = 0; /* second */

void coco_draw_chat( long tv_sec )
{
  if (coco_in_game_chat_mode)
    { /* draw chat (outgoing draft messages) */
      int x = 100;
      int y = 100;
      int cursor = coco_in_game_chat_cursor;
      char *string = coco_headered_in_game_chat_buffer;
      struct coco_font *font = &coco_default_font;
      coco_draw_color( COCO_COLOR_CHAT );
      coco_draw_string( x, y, string, -1, font, COCO_BOTTOM );
      if ((1 & tv_sec) == (1 & coco_chat_cursor_last_blink))
        {
          int cx, h;
          coco_font_string_dimensions( string,
                                       (cursor
                                        + coco_in_game_chat_buffer
                                        - coco_headered_in_game_chat_buffer),
                                       font, &cx, &h, NULL );
          coco_draw_color( COCO_COLOR_CURSOR );
          coco_draw_rectangle( x+cx-1, y, x+cx+2, y+h );
        }
    }

}


void
coco_draw_mission_objective( struct coco_cstate *cstate )
{
  struct coco_font *font = &coco_default_font;
  int mj = cstate->major_mission;
  int mn = cstate->minor_mission;
  int hh = COCO_FONT_INTERLINE_SPACING( font );

  coco_draw_color( COCO_COLOR_MISSION_OBJECTIVE );
  if (mj != 255)
    {
      static int last_mj = -1;
      static int last_mj_w = -1;
      int x;
      if (last_mj != mj)
        {
          last_mj = mj;
          coco_font_string_dimensions( coco_major_missions[mj], -1, font, 
                                       &last_mj_w, NULL, NULL );
        }
      x = (COCO_OFFICIAL_WINDOW_W>>1) - (last_mj_w>>1);
      coco_draw_string( COCO_MAX( x, 0 ),
                        COCO_VIEWABLE_H - (COCO_TALK_DISPLAY_SIZE+1)*hh,
                        coco_major_missions[mj],
                        -1, font, COCO_BOTTOM );
    }
  /* coco_cookie( "mj=%3d mn=%3d angle=%3d\n", mj, mn, cstate->angle_on ? cstate->angle : -1 ); */
  if (mn != 255)
    {
      int x;
      static int last_mn = -1;
      static int last_mn_w = -1;
      if (last_mn != mn)
        {
          last_mn = mn;
          coco_font_string_dimensions( coco_minor_missions[mn], -1, font, 
                                       &last_mn_w, NULL, NULL );
        }
      x = (COCO_OFFICIAL_WINDOW_W>>1) - (last_mn_w>>1);
      coco_draw_string( COCO_MAX( x, 0 ),
                        COCO_VIEWABLE_H - (COCO_TALK_DISPLAY_SIZE+2)*hh,
                        coco_minor_missions[mn],
                        -1, font, COCO_BOTTOM );
    }
  if (cstate->angle_on)
    {
      glPushMatrix(); /* model-view */
      glTranslatef( COCO_TUTORIAL_ARROW_X, COCO_TUTORIAL_ARROW_Y, 0.0 );
      glRotatef( COCO_ANGLE_TO_FLOAT_DEGREES( cstate->angle ),
                 0.0, 0.0, 1.0 );
      coco_begin_line_strip_drawing();
      coco_line_strip_drawing_vertex( 10, 10 );
      coco_line_strip_drawing_vertex( 10, 20 );
      coco_line_strip_drawing_vertex( 30, 0 );
      coco_line_strip_drawing_vertex( 10, -20 );
      coco_line_strip_drawing_vertex( 10, -10 );
      coco_line_strip_drawing_vertex( -30, -10 );
      coco_line_strip_drawing_vertex( -30, 10 );
      coco_line_strip_drawing_vertex( 10, 10 );
      coco_end_line_strip_drawing();
      glPopMatrix();
    }
}


void 
coco_cstate_draw( struct coco_cstate *cstate )
{
  coco_color line_color;
  struct timeval now;
  /*  struct timezone tz;*/
  int i;
  coco_znum z;

  gettimeofday( &now, NULL );

  /* coco_clear_window_color( COCO_COLOR_BACKGROUND );
   * coco_clear_window(); */
  if (coco_draw_background_before_everything_else)
    coco_draw_background( 0, 0, COCO_FALSE );
  coco_draw_ground( COCO_GROUND_Y );
  
  coco_draw_mission_objective( cstate );

  for (z=COCO_ZNUM_MIN; z<=COCO_ZNUM_MAX; z++)
    for (i=cstate->cobjects_length-1; i>=0; i--)
      {
        coco_cobject *cobject = cstate->cobjects + i;
        if (z==COCO_COBJECT_Z( cobject ))
          {
            coco_sobject_cl *cla = COCO_COBJECT_CL( cobject );
            cla->cobject_draw( cobject, cla );
          }
      }
  if (cstate->dashinfo.bits & (1<<COCO_DASHINFO_BIT_MISSILE_LOCK))
    line_color = COCO_COLOR_DASHBOARD_MISSILE_LOCK;
  else if (cstate->dashinfo.bits & (1<<COCO_DASHINFO_BIT_SUMMON_DISABLE))
    line_color = COCO_COLOR_DASHBOARD_SUMMON_DISABLE;
  else
    line_color = COCO_COLOR_DASHBOARD_FG;
  coco_draw_color( line_color );
  coco_draw_line( 0, COCO_RADAR_Y-1, coco_real_window_w, COCO_RADAR_Y-1 );
  coco_draw_line( 0,                  COCO_RADAR_Y+COCO_RADAR_H, 
                  coco_real_window_w, COCO_RADAR_Y+COCO_RADAR_H );
  coco_draw_dashboard( cstate );
  coco_begin_point_drawing();
  for (i=cstate->cblips_length-1; i>=0; i--)
    {
      struct coco_cblip cblip = cstate->cblips[i];
      coco_blip_index ci = cblip.color_index;
      if (ci != COCO_BLIP_TRANSPARENT)
        {
          coco_draw_color( coco_blip_to_color[ci] );
          coco_point_drawing_vertex( cblip.x, 
                                     ((coco_gnum) cblip.y) + COCO_RADAR_Y );
        }
    }
  coco_end_point_drawing();
#ifdef COCO_CONSOLE
  coco_console_draw();
#endif
  if (COCO_NEUTRAL != cstate->game_over_nationality)
    coco_draw_game_over_message( cstate->game_over_nationality );
  if (!coco_draw_background_before_everything_else)
    coco_draw_background( -cstate->camera_x, 0, COCO_FALSE );
  coco_draw_talk( now.tv_sec );
  coco_draw_chat( now.tv_sec );
}


/* __________ */



/* __________ */


void
coco_write_client2server_packet_maybe()
{
  int udpfd = coco_get_client_udp_fd();
  if ((udpfd != -1) && coco_have_play_message)
    coco_write_client2server_packet( udpfd, 
                                     coco_get_keyboard_pointer_state() );
}

void
coco_send_play_message_maybe( void )
{
  if (coco_have_game_message)
    {
      union coco_message playmsg;
      coco_cookie( "Sending server a play message...\n" );
      playmsg.every.type = COCO_MESSAGE_PLAY;
      coco_write_message( &coco_server_stream, &playmsg );
      /* wait for the response before we go change client_state */
    }
}

static coco_bool coco_gnome_disconnect_pending_p = COCO_FALSE;

void 
coco_client_disconnect( void )
{
  coco_reset_coco_players_array();
  coco_have_game_message = COCO_FALSE;
  coco_have_port_message = COCO_FALSE;
  coco_have_play_message = COCO_FALSE;
  coco_server_addr_in_nbo = 0;
  if (coco_get_client_udp_fd() != -1)
    {
      coco_close_fd( coco_get_client_udp_fd(), COCO_FALSE );
      coco_set_client_udp_fd( -1 );
    }
  {
    struct coco_stream *str = coco_get_server_message_stream();
    if (str)
      {
        if (coco_server_stream.fd != -1)
          {
            coco_close_fd( coco_server_stream.fd, COCO_TRUE );
            coco_server_stream.fd = -1;
          }
        coco_server_stream.buffer_in_use = 0;
      }
  }
  coco_set_client_state( COCO_CLIENT_DEMO );
  coco_initialize_cstate( coco_get_cstate() );
  coco_cookie( "Setting pending disconnect\n" );
  coco_gnome_disconnect_pending_p = COCO_TRUE;
}

coco_bool
coco_query_disconnected( void )
{
  if (coco_gnome_disconnect_pending_p)
    {
      coco_cookie( "pending disconnect read\n" );
      coco_gnome_disconnect_pending_p = COCO_FALSE;
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}


#ifdef COCO_IGNORE
/* HACK for NVIDIA 0.95 driver, that corrupts textures.  Apperantly fixed
   in 0.96 */
void
coco_reload_textures( void )
{
  const static int clx[] 
    = { COCO_COBRA_CLASS_INDEX, COCO_BUBBLE_COPTER_CLASS_INDEX };
  int i, j;
  for (i=0; i<2; i++)
    {
      coco_sobject_cl *cl = coco_sobject_classes + clx[i];
      coco_gnum w = cl->canonical_w;
      coco_gnum h = cl->canonical_h;
      for (j=0; j < cl->raster_count; j++)
        coco_reload_texture( cl->rasters[j], w, h );
    }
}
#endif /* COCO_IGNORE */




char *
coco_generate_screenshot_fname( char *base, int *counter )
{
  char *buf = coco_malloc( 40 );
  sprintf( buf, "/tmp/%s%03d.tif", base, ++(*counter) );
  return buf;
}

static coco_bool coco_recording_movie = COCO_FALSE;
static int coco_movie_number = 0;
static int coco_movie_frame = 0;


/* outter loop reads coco_pending_* too see if keys have been pressed.
   see coco_client_loop().  Other events may be immediately dispatched. */
void 
coco_key_press( coco_key key, char *string_or_null, 
                coco_key_state_ref keystate )
{


  /* global bindings */
  if ((COCO_CHAR_TO_KEY( 'c' ) == key)
      && coco_get_bit( keystate, COCO_KEY_CONTROL ))
    { /* screenshot feature */
      static int counter = 0;
      char buf[100];
      char *fname = coco_generate_screenshot_fname( "screen", &counter );
      if (coco_screenshot( fname ))
        sprintf( buf, "Screenshot saved to \"%s\"", fname );
      else sprintf( buf, "Screenshot to \"%s\" FAILED!", fname );
      coco_display_notice( buf );
      coco_free( fname );
      return;
    }
  /* mode-local bindings */
  switch (coco_get_client_state()) {
#ifdef COCO_CONSOLE
  case COCO_CLIENT_DEMO:
    coco_set_client_state( COCO_CLIENT_CONSOLE );
    coco_set_inhibit_refresh( COCO_FALSE );
    coco_console_set_mode( COCO_FULLSCREEN );
    /* no break */
  case COCO_CLIENT_CONSOLE:
  case COCO_CLIENT_TABULA_RASA:
  case COCO_CLIENT_CHOOSE_SERVER_DIALOG:
  case COCO_CLIENT_CHATROOM_DIALOG:
  case COCO_CLIENT_IN_GAME_ESCAPE_DIALOG:
  case COCO_CLIENT_GAME_OVER:
    if (coco_console_get_mode() < COCO_MINIMAL)
      {
        if (key == COCO_KEY_ESCAPE)
          coco_fatal_printf( COCO_ERROR_CONSOLE_NOT_IMPLEMENTED,
                             "escape pressed\n" );
        else coco_console_set_mode( COCO_MINIMAL );
      }
    else coco_console_key( key, string_or_null, COCO_TRUE );
    break;
#else /* not COCO_CONSOLE */
  case COCO_CLIENT_CHATROOM_DIALOG:
    if ((key == COCO_KEY_SPACE)
        && coco_use_keyboard_for_gui_p)
      coco_send_play_message_maybe();
    break;
  case COCO_CLIENT_DEMO:
    if (coco_use_keyboard_for_gui_p)
      switch (key) {
      case COCO_CHAR_TO_KEY( 'p' ):
        coco_cookie( "P pressed\n" );
        coco_client_connect_to_server( NULL, NULL );
        break;
      case COCO_CHAR_TO_KEY( 'l' ):
        fprintf( stdout, "%s", coco_gnu_public_license );
        break;
      case COCO_KEY_ESCAPE:
        coco_kill_window();
        break;
      }
    break;
  case COCO_CLIENT_TABULA_RASA:
  case COCO_CLIENT_CHOOSE_SERVER_DIALOG:
  case COCO_CLIENT_IN_GAME_ESCAPE_DIALOG:
  case COCO_CLIENT_GAME_OVER:
    coco_draw_color( COCO_COLOR_FONT );
    coco_draw_string( 100, 100, "No keys defined now.  Fiddle with the gui.",
                      -1, &coco_default_font, COCO_TOP );
    break;
#endif /* COCO_CONSOLE */
  case COCO_CLIENT_PLAY:
    if (coco_in_game_chat_mode)
      {
        if (key==COCO_KEY_RETURN)
          {
            coco_chat( coco_in_game_chat_buffer );
            coco_in_game_chat_mode = COCO_FALSE;
            coco_censor_keys = COCO_FALSE;
          }
        else if (key==COCO_KEY_ESCAPE)
          {
            coco_in_game_chat_mode = COCO_FALSE;
            coco_censor_keys = COCO_FALSE;
          }
        else if (key == COCO_KEY_LEFT_ARROW)
          coco_in_game_chat_cursor
            = COCO_MAX( 0, coco_in_game_chat_cursor - 1 );
        else if (key == COCO_KEY_RIGHT_ARROW)
          coco_in_game_chat_cursor
            = COCO_MIN( strlen( coco_in_game_chat_buffer ),
                        coco_in_game_chat_cursor + 1 );
        else if (key == COCO_KEY_UP_ARROW)
          coco_in_game_chat_cursor = 0;
        else if (key == COCO_KEY_DOWN_ARROW)
          coco_in_game_chat_cursor = strlen( coco_in_game_chat_buffer );
        else if ((key == COCO_KEY_DELETE) && (coco_in_game_chat_cursor > 0))
          {
            char *rest = coco_in_game_chat_buffer + coco_in_game_chat_cursor;
            memmove( rest - 1, rest, strlen( rest ) + 1 );
            coco_in_game_chat_cursor--;
          }
        else if ((key >= COCO_LPOINTER) && (key <= COCO_5POINTER))
          /* was coco_write_client2server_packet_maybe(); */
          ;/* now do nothing */
        else if (string_or_null)
          {
            char *cursor;
            char ch;
            for (cursor=string_or_null; '\0' != (ch=*cursor); cursor++)
              if ((ch < ' ') || (ch == 127))
                return; /* unprintable */
            if ((strlen( coco_in_game_chat_buffer ) + strlen( string_or_null ) + 1)
                >= COCO_CHAT_MAX)
              return; /* too long */
            coco_in_game_chat_cursor
              = coco_strsplice( coco_in_game_chat_buffer,
                                coco_in_game_chat_cursor, string_or_null );
            return;
          }
	coco_chat_cursor_last_blink = time(NULL);
        
        return; /* don't send any key, e.g. shift! */
      }
    else /* not chatting */
      {
        switch (key) {
        case COCO_KEY_TAB:
          if (!coco_get_pointer_grabbed_flag())
            coco_grab_pointer();
          return; /* don't bother telling server */
        case COCO_KEY_ESCAPE:
          coco_ungrab_pointer();
          return; /* don't bother telling server */
#ifndef COCO_CONSOLE
        case COCO_KEY_RETURN:
          coco_in_game_chat_mode = COCO_TRUE;
          coco_in_game_chat_buffer[0] = '\0';
          coco_in_game_chat_cursor = 0;
          coco_censor_keys = COCO_TRUE;
          return;
#endif /* COCO_CONSOLE */
#ifdef COCO_IGNORE
        case 'n': /* nvidia texture corruption bug */
          if ((coco_get_bit( keystate, COCO_KEY_CONTROL )))
            {
              coco_reload_textures();
              coco_display_notice( "Textures Reloaded" );
            }
          return;
#endif /* COCO_IGNORE */
        case 's': /* sound toggle */
          if (coco_get_bit( keystate, COCO_KEY_CONTROL ))
            {
              char notice[100];
              coco_set_mute( !coco_get_mute() );
              sprintf( notice, "Sound %s", coco_get_mute() ? "OFF" : "ON" );
              coco_display_notice( notice );
            }
          return;
        case 'b': /* blend toggle */
          if (coco_get_bit( keystate, COCO_KEY_CONTROL ))
            {
              char notice[100];
              coco_blend_alpha = !coco_blend_alpha;
              sprintf( notice, "Alpha Blend %s", 
                       coco_blend_alpha ? "ON" : "OFF" );
              coco_display_notice( notice );
            }
          return;
        case '0': /* voting on something */
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          {
            struct coco_stream *str = coco_get_server_message_stream();
            if ((str != NULL) && coco_old_votestate_initialized)
              {
                union coco_message msg;
                int serial = key - '0';
                msg.every.type = COCO_MESSAGE_NOMINATE;
                msg.nominate.serial = serial;
                if (!coco_old_votestate_initialized) 
                  {
                    coco_warn_printf( "Attempt to vote but "
                                      "no votestate yet saved.\n" );
                    return;
                  }
                msg.nominate.action_code
                  = coco_old_votestate.nominations[serial].action_code;
                msg.nominate.act_object
                  = coco_old_votestate.nominations[serial].act_object;
                msg.nominate.vote
                  = !(((coco_old_votestate.nominations[serial].votes)
                       >> coco_my_player_id) & 1);
                coco_write_message( str, &msg );
              }
          }
          return;
        case 'l': /* grey comp */
        case 'r': /* green comp */
          if (coco_get_bit( keystate, COCO_KEY_CONTROL ))
            {
              struct coco_stream *str = coco_get_server_message_stream();
              union coco_message msg;

              msg.every.type = COCO_MESSAGE_NOMINATE;
              msg.nominate.serial = -1; /* unknown yet */
              msg.nominate.vote = COCO_TRUE;

              if (str != NULL)
                {
                  if (coco_get_bit( keystate, COCO_KEY_SHIFT ))
                    { /* remove compute player */
                      msg.nominate.action_code 
                        = ((key == 'l')
                           ? COCO_NOMINATE_BOOT_BAD_GUY_COMP
                           : COCO_NOMINATE_BOOT_GOOD_GUY_COMP);
                      msg.nominate.act_object = 0;
                    }
                  else 
                    { /* add computer player */
                      msg.nominate.action_code
                        = ((key == 'l')
                           ? COCO_NOMINATE_ADD_BAD_GUY_COMP
                           : COCO_NOMINATE_ADD_GOOD_GUY_COMP);
                      msg.nominate.act_object = 0; /* unused, but init anyway */
                    }
                  
                  coco_write_message( str, &msg );
                }
            }
          return;
            
          /* Super-obscure undocumented feature Su-C-m: 
             record movie (on/off toggle) */
        case 'm': 
          if (coco_get_bit( keystate, COCO_KEY_CONTROL )
              && coco_get_bit( keystate, COCO_KEY_SUPER ))
            {
              if (coco_recording_movie)
                {
                  coco_recording_movie = COCO_FALSE;
                  coco_display_notice( "Movie saved\n" );
                }
              else
                {
                  coco_recording_movie = COCO_TRUE;
                  coco_movie_frame = 0;
                  coco_movie_number++;
                  coco_display_notice( "Movie recording "
                                       "(hope you've got lotsa /tmp)\n" );
                }
              return;
            }


        }
      }
    coco_write_client2server_packet_maybe();
    break;
  }
}

void 
coco_key_release( coco_key key, coco_key_state_ref keystate )
{
  coco_write_client2server_packet_maybe(); 
}

void
coco_record_movie_frame_maybe( void )
{
  if (coco_recording_movie)
    {
      char *fname;
      char buf[30];
      sprintf( buf, "movie-%d-", coco_movie_number );
      fname = coco_generate_screenshot_fname( buf, &coco_movie_frame );
      coco_screenshot( fname );
      coco_free( fname );
    }
}

void 
coco_refresh_window()
{
  /* code to limit the framerate.
  static struct timeval last = { 0, 0 };
  struct timeval now, delta;
  struct timezone tz;
  gettimeofday( &now, &tz );
  coco_subtract_timevals( &delta, &now, &last );
  if (timercmp( &delta, &coco_ideal_frame_rate, <= ))
    {
      coco_cookie( "Refreshing too fast %d, returning\n", delta.tv_usec );
      return;
    }
    last = now; */
  coco_draw_window();
  coco_swap_buffer();
  coco_record_movie_frame_maybe();
}

void
coco_draw_pointer_grab_warnings( void )
{
  struct coco_keyboard_pointer_state *is = coco_get_keyboard_pointer_state();
  coco_i32 x = is->pointer_x;
  coco_i32 y = is->pointer_y;
  struct coco_font *font = &coco_default_font;
  coco_bool in_window_p = !!coco_mouse_in_window_p();
  coco_bool grabbed_p = !!coco_get_pointer_grabbed_flag();
  char *message = grabbed_p ? "[esc] ungrabs mouse" : "[tab] grabs mouse";
  int h, w;
  coco_color clr = (coco_draw_frame_number&2)
    ? COCO_COLOR_RED : COCO_COLOR_WHITE;
  if ((in_window_p == grabbed_p))
    {
      coco_font_string_dimensions( message, -1, font, &w, &h, NULL );

      /* this snippet will do the warning if the pointer is out on the +-x
         axis too.  Its just too damn flashy for when we're grabbed though. */
      if ((!grabbed_p) && ((x<=(-0.45*coco_real_window_w))
                        ||(x>=(0.45*coco_real_window_w))))
        {
          coco_draw_color( clr );
          coco_draw_string( (x<=0)?0:(coco_real_window_w-1-w), 
                            COCO_CLAMP( 0, y, coco_real_window_h-h-1 ), 
                            message, -1, font, COCO_BOTTOM );
        }
      else if (((y<=2)
                &&((!grabbed_p)
                   ||(x<30-(coco_real_window_h>>1))
                   ||(x>(coco_real_window_w>>1)-30)))
               || (y>=coco_real_window_h-1))
        {
          coco_gnum sx = x + (coco_real_window_w>>1); /* screen x */
          coco_draw_color( clr );
          coco_draw_string( (0>=(sx-(w>>1)))
                            ? 0
                            : ((sx>=((coco_real_window_w - (w>>1)) - 1))
                               ? (coco_real_window_w - w - 1)
                               : (sx - (w>>1))), 
                            (y<=2) ? 0 : (coco_real_window_h-1), 
                            message, -1, font, (y<=2) ? COCO_BOTTOM : COCO_TOP );
        }
    }
}


void 
coco_draw_window( void )
{
  coco_clientstate state = coco_get_client_state();
  coco_draw_frame_number = COCO_U32_ADD_MODULAR( coco_draw_frame_number );
  switch (state) {
  case COCO_CLIENT_DEMO:
    coco_play_intro_movie_frame( COCO_FALSE, COCO_FALSE );
    break;
  case COCO_CLIENT_CHOOSE_SERVER_DIALOG:
    break;
  case COCO_CLIENT_CHATROOM_DIALOG:
    coco_draw_color( COCO_COLOR_BROWN );
    coco_clear_window_color( COCO_COLOR_BACKGROUND );
    coco_clear_window();
    coco_draw_string( 30, coco_real_window_h - 100, 
                      "Chat yer brains out. (network connect gogo)", -1,
                      &coco_default_font, COCO_BASELINE );
    if (coco_have_game_message)
      {
        coco_draw_string( 30, 100, 
                          "Game info recieved.  Press [SPACE] to get it on.",
                          -1, &coco_default_font, COCO_BASELINE );
      }
    break;
  case COCO_CLIENT_PLAY:
    {
      struct coco_cstate *cstate = coco_get_cstate();
      if (cstate && cstate->in_use)
        coco_cstate_draw( cstate );
      else
        coco_warn_printf( "Playing without anythingn to display.  Hm...\n" );
      coco_draw_pointer_grab_warnings();
    }
    break;
  case COCO_CLIENT_IN_GAME_ESCAPE_DIALOG:
    coco_draw_color( COCO_COLOR_BROWN );
    coco_clear_window_color( COCO_COLOR_BACKGROUND );
    coco_clear_window();
    coco_draw_string( 30, coco_real_window_h - 100, "You pressed escape.", -1,
                      &coco_default_font, COCO_BASELINE );
    break;
  case COCO_CLIENT_GAME_OVER:
    coco_draw_color( COCO_COLOR_BROWN );
    coco_clear_window_color( COCO_COLOR_BACKGROUND );
    coco_clear_window();
    coco_draw_string( 30, coco_real_window_h - 100, "Game Over.", -1, 
                      &coco_default_font, COCO_BASELINE );
    break;
  default:
    coco_warn_printf( "Got expose for unknown game state %d\n.", 
                      coco_get_client_state() );
    break;
  }
#ifdef COCO_CONSOLE
  if (state != COCO_CLIENT_PLAY)
    {
      coco_cookie( "redrawing with console\n" );
      coco_console_draw();
    }
#endif
}


/* Height should almost always be COCO_GROUND_Y */
void
coco_draw_ground( coco_gnum height )
{
  coco_draw_color( COCO_COLOR_GROUND );
#if (COCO_APPEARANCE == COCO_APPEARANCE_RESCUE_RAIDERS)
  coco_draw_rectangle( 0, 0, coco_real_window_w, height );
#elif (COCO_APPEARANCE == COCO_APPEARANCE_DOODLE)
  glPushAttrib( GL_LINE_BIT );
  glLineWidth( 3.0 );
  coco_draw_line( 0, height-1, coco_real_window_w, height-1 );
  glPopAttrib();
#else
#error COCO_APPEARANCE is not a value known to client.c
#endif
}

void
coco_replay_intro_movie( void )
{
  coco_clear_window_color( COCO_COLOR_BACKGROUND );
  coco_play_intro_movie_frame( COCO_TRUE, COCO_TRUE );
  coco_play_intro_movie();
}


void
coco_play_intro_movie( void )
{
  coco_bool inhibit;
  inhibit = coco_set_inhibit_refresh( COCO_TRUE );
  coco_clear_window_color( COCO_COLOR_BACKGROUND );
  while ((COCO_CLIENT_DEMO == coco_get_client_state())
         && coco_play_intro_movie_frame( COCO_TRUE, COCO_FALSE))
    coco_handle_ui_events();
  coco_set_inhibit_refresh( inhibit );
}

#define COCO_MOVIE_TRUCK_LTAB \
  ((COCO_OFFICIAL_WINDOW_W - COCO_COPTER_COMMANDER_RASTER_W - 4) >> 1)
#define COCO_MOVIE_TRUCK_PAUSE 100
#define COCO_MOVIE_TRUCK_FRAME \
  ((COCO_MOVIE_TRUCK_LTAB + COCO_TRUCK_RASTER_W + COCO_MOVIE_TRUCK_PAUSE )>>2)

/* movie_frame 0 must produce full width. */
void
coco_draw_movie_rotor( coco_gnum y, unsigned int movie_frame )
{
  const static coco_gnum rotor_length[4] = 
  { 0, (COCO_COPTER_COMMANDER_RASTER_W>>3)*3,
    (COCO_COPTER_COMMANDER_RASTER_W>>1)+2, 
    (COCO_COPTER_COMMANDER_RASTER_W>>2) };
  coco_gnum h = y + COCO_COPTER_COMMANDER_RASTER_H;
  coco_gnum xbase = COCO_MOVIE_TRUCK_LTAB + 2;
  coco_draw_color( COCO_COLOR_ROTOR );
  coco_draw_line( xbase + rotor_length[movie_frame&3], h,
                  xbase + COCO_COPTER_COMMANDER_RASTER_W
                  - rotor_length[movie_frame&3], h );
}

#define COCO_TRUCK_SMOKE_STACK_X 248
#define COCO_TRUCK_MIDDLE_WHEEL_X 205


void
coco_draw_intro_movie_smoke( coco_gnum truck_x, coco_gnum truck_y,
                             unsigned int movie_frame )
{
  unsigned int frame_mod_8 = movie_frame & 7;
  unsigned int frame_mod_7 = movie_frame % 7;
  /* unsigned int frame_mod_5 = movie_frame % 5; */
  if (frame_mod_8 < 3)
    {
      struct coco_raster *smoke = coco_smoke_rasters[frame_mod_8];
      coco_draw_raster( truck_x + COCO_TRUCK_SMOKE_STACK_X - smoke->w
                        - (frame_mod_8 << 4),
                        truck_y + COCO_TRUCK_RASTER_H 
                        - (smoke->h >> 1) + (frame_mod_8 << 2), 
                        smoke );
    };
  if (frame_mod_7 < 1)
    {
      struct coco_raster *smoke = coco_smoke_rasters[frame_mod_7];
      coco_draw_raster( truck_x - smoke->w - (frame_mod_7 << 4),
                        truck_y, smoke );
    };
  /* Middle wheel dust .. looks too busy ...
  if (frame_mod_5 < 2)
    {
      struct coco_raster *smoke = coco_smoke_rasters[frame_mod_5];
      coco_draw_raster( truck_x + COCO_TRUCK_MIDDLE_WHEEL_X 
                        - smoke->w - (frame_mod_5 << 4),
                        truck_y, smoke );
    }
  */
}


coco_bool
coco_play_intro_movie_frame( coco_bool advance_frame, coco_bool rewind )
{
  struct timeval mintime;
  struct timeval tmp;
  static unsigned int movie_frame = 0;

  coco_time_pad_start( &tmp );

  if (rewind) movie_frame = 0;
  coco_clear_window();

  if (movie_frame < COCO_MOVIE_TRUCK_FRAME)
    { /* truck moving to center of screen */
      coco_gnum x = (movie_frame <<2) - COCO_TRUCK_RASTER_W
        - COCO_MOVIE_TRUCK_PAUSE;
      if (coco_draw_background_before_everything_else)
        coco_draw_background( -movie_frame<<3, 0, COCO_TRUE );
      coco_draw_ground( COCO_GROUND_Y );
      coco_draw_raster( x, COCO_GROUND_Y, &coco_truck_raster );
      coco_draw_intro_movie_smoke( x, COCO_GROUND_Y, movie_frame );
      if (!coco_draw_background_before_everything_else)
        coco_draw_background( -movie_frame<<3, 0, COCO_TRUE );
    }
  else
    {
      static int last_rising_frame = 0;
      static int last_ascended_frame = 0;
      static int last_ascended_star_x, last_ascended_star_y;
      int y_theoretical, star_x;
      star_x = (-movie_frame<<3);
      y_theoretical = (movie_frame + 15 - COCO_MOVIE_TRUCK_FRAME) <<1;
      if (!((y_theoretical + (COCO_COPTER_COMMANDER_RASTER_H<<2))
            >= coco_real_window_h))
        { /* logo flying out of truck to top of screen */
          if (coco_draw_background_before_everything_else)
            coco_draw_background( star_x, 0, COCO_TRUE );
          coco_draw_ground( COCO_GROUND_Y );
          coco_draw_raster( COCO_MOVIE_TRUCK_LTAB + 2, y_theoretical,
                            &coco_copter_commander_raster );
          coco_draw_movie_rotor( y_theoretical, movie_frame );
          coco_draw_raster( COCO_MOVIE_TRUCK_LTAB, 
                            COCO_GROUND_Y, &coco_truck_raster );
          coco_draw_intro_movie_smoke( COCO_MOVIE_TRUCK_LTAB,
                                       COCO_GROUND_Y, movie_frame );
          if (!coco_draw_background_before_everything_else)
            coco_draw_background( star_x, 0, COCO_TRUE );
          last_rising_frame = movie_frame;
        }
      else 
        { /* logo pulling screen off ground */
          int copter_y = coco_real_window_h
            - (COCO_COPTER_COMMANDER_RASTER_H<<2);
          if (!(movie_frame - 60 > last_rising_frame))
            {
              int delta_frame = (movie_frame - last_rising_frame)<<1;
              int truck_y = COCO_GROUND_Y - delta_frame;
              if (coco_draw_background_before_everything_else)
                coco_draw_background( star_x, -delta_frame, COCO_TRUE );
              coco_draw_raster( COCO_MOVIE_TRUCK_LTAB + 2, 
                                copter_y, &coco_copter_commander_raster );
              coco_draw_movie_rotor( copter_y, movie_frame );
              if (truck_y+COCO_TRUCK_RASTER_H > 0)
                { /* Only draw truck & ground if visible (no visible perf boost) */
                  coco_draw_raster( COCO_MOVIE_TRUCK_LTAB, truck_y,
                                    &coco_truck_raster );
                  coco_draw_ground( COCO_GROUND_Y - delta_frame );
                }
              coco_draw_intro_movie_smoke( COCO_MOVIE_TRUCK_LTAB,
                                           truck_y, movie_frame );
              if (!coco_draw_background_before_everything_else)
                coco_draw_background( star_x, -delta_frame, COCO_TRUE );
              last_ascended_star_x = star_x;
              last_ascended_star_y = -delta_frame;
              last_ascended_frame = movie_frame;
            }
          else
            {
              while (1)        /* just to make break work */
                {
                  char buf[1000];
                  int chars_drawn = 0;
                  int chars_to_draw = movie_frame - last_ascended_frame;
                  int width, height, ascent;
                  int ybase;
                  int xbase = COCO_MOVIE_TRUCK_LTAB + 2;
                  if (coco_draw_background_before_everything_else)
                    coco_draw_background( last_ascended_star_x, 
                                          last_ascended_star_y, COCO_TRUE );
                  coco_draw_raster( xbase, 
                                    copter_y, &coco_copter_commander_raster );
                  coco_draw_movie_rotor( copter_y, 0 );
                  coco_draw_color( COCO_COLOR_FONT );
                  ybase = copter_y - COCO_COPTER_COMMANDER_RASTER_H;
                  sprintf( buf, "Version %d.%d", COCO_MAJOR_VERSION, 
                           COCO_MINOR_VERSION );
                  coco_font_string_dimensions( buf, -1, &coco_default_font,
                                               &width, &height, &ascent );
                  coco_draw_string( (coco_real_window_w - width)>>1,
                                    ybase,
                                    buf, 
                                    COCO_MIN( chars_to_draw, strlen( buf ) ),
                                    &coco_default_font, COCO_TOP );
                  chars_drawn += strlen( buf );
                  ybase -= height<<1;
                  if (chars_drawn >= chars_to_draw) break;
                  if (coco_low_tech_menu)
                    {
                      int low_tech_menu_height = 0;
                      sprintf( buf, "%s", coco_low_tech_menu );
                      coco_font_string_dimensions( buf, -1, &coco_default_font,
                                                   &width, &low_tech_menu_height,
                                                   NULL );
                      coco_draw_string( (coco_real_window_w - width)>>1,
                                        ybase,
                                        buf, 
                                        COCO_MIN( (chars_to_draw
                                                   - chars_drawn), 
                                                  strlen( buf ) ),
                                        &coco_default_font, COCO_TOP );
                      chars_drawn += strlen( buf );
                      ybase -= low_tech_menu_height + height;
                    }
                  if (chars_drawn >= chars_to_draw) break;
                  sprintf( buf, 
                           "Capture and destroy enemy units.\n"
                           "Only demolition vans can crack the\n"
                           "shell of the enemy Headquarters\n"
                           "Crawler and win the game!\n"
                           "\n"
                           "Copyright 2001 David Morse.\n"
                           "Portions Copyright 1998, 1999, 2000\n"
                           "Tim Janik and Red Hat.\n"
                           "\n"
                           "%s comes with NO\n"
                           "WARRANTY, to the extent permitted by\n"
                           "law.  You may redistribute copies of\n"
                           "%s under the terms of\n"
                           "the GNU General Public License.  %s\n",
                           coco_product_name, coco_product_name,
                           coco_gnu_info_string );
                  coco_font_string_dimensions( buf, -1, &coco_default_font,
                                               &width, NULL, NULL );
                  /* coco_dbg = 1;
                     coco_cookie( "oi\n" );
                     coco_font_string_dimensions( buf, -1, &coco_default_font,
                     &width, &height, NULL );
                     coco_cookie( "boy %d\n", width );
                     coco_dbg = 0; */
                  coco_draw_string( (coco_real_window_w - width)>>1,
                                    ybase,
                                    buf, 
                                    COCO_MIN( (chars_to_draw - chars_drawn),
                                              strlen( buf ) ),
                                    &coco_default_font, COCO_TOP );
                  chars_drawn += strlen( buf );
                  ybase -= height;
                  chars_drawn -= height;

                  break;
                }
              if (!coco_draw_background_before_everything_else)
                coco_draw_background( last_ascended_star_x, 
                                      last_ascended_star_y, COCO_TRUE );
            }
        }
    }
  /* Draws spinning missile
  {
    coco_cobject cobject;
    cobject.y.class_index = COCO_MISSILE_CLASS_INDEX;
    cobject.y.y = 400;
    cobject.y.x = 200;
    cobject.raster_barrel.barrel_angle = movie_frame>>1;
    coco_missile_draw( &cobject, COCO_MISSILE_CLASS );
  } */
  coco_swap_buffer();
  mintime.tv_sec = 0;
  mintime.tv_usec = 13000;
  coco_time_pad_end( &tmp, &mintime );
  if (advance_frame) movie_frame++;
  return movie_frame<1000;
}


/* _NETWORK CRUD___________________________________________________ */

/* readers for these variables are provided below */
static struct coco_stream coco_server_stream = { -1, 0 }; 

struct coco_stream *
coco_get_server_message_stream( void ) /* NULL on fail */
{
  return (coco_server_stream.fd >= 0) ? &coco_server_stream : NULL;
}
int 
coco_get_server_message_file_descriptor( void ) /* returns -1 on fail */
{
  return coco_server_stream.fd;
}

void
coco_write_client2server_packet( int udpfd, 
                                 struct coco_keyboard_pointer_state *
                                 input_state )
{
  int send_result;
  struct coco_client_to_server_packet twiddle_state;
  assert( coco_have_game_message );
  twiddle_state.player_id = coco_my_player_id;
  
  twiddle_state.input_state.client_time
    = COCO_U32_HTON( input_state->client_time );
  twiddle_state.input_state.pointer_x
    = COCO_I32_HTON( input_state->pointer_x );
  twiddle_state.input_state.pointer_y
    = COCO_I32_HTON( input_state->pointer_y );
  coco_copy_bit_vec( twiddle_state.input_state.key_state,
                     input_state->key_state,
                     COCO_KEYLIMIT );
  if (coco_censor_keys)
    {
      unsigned char mousies 
        = coco_get_byte( twiddle_state.input_state.key_state, COCO_LPOINTER>>3 ) & 0x1F;
      coco_clear_bit_vec( twiddle_state.input_state.key_state, COCO_KEYLIMIT, 0 );
      coco_set_byte( twiddle_state.input_state.key_state, COCO_LPOINTER>>3, mousies );
    }
  send_result = send( udpfd, &twiddle_state, sizeof( twiddle_state ), 0 );
  if ((-1 == send_result) && (EAGAIN != errno))
    coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                       "Udp send() failed with errno=%d: %s\n",
                       errno, coco_errno_string( errno ) );
#ifdef COCO_DEBUG
  if (-1 == send_result)
    coco_cookie( "EAGAIN on outbound mouse signal\n" );
#endif /* COCO_DEBUG */
}

void 
coco_cobject_y_ntoh( coco_cobject *cobject )
{
  COCO_COBJECT_X( cobject ) = COCO_CNUM_NTOH( COCO_COBJECT_X( cobject ) );
  cobject->y.y = COCO_CNUM_NTOH( cobject->y.y );
}


static coco_notification_id coco_in_game_talk_notify = -1;
static coco_notification_id coco_in_game_vote_notify = -1;



static coco_bool coco_old_compflux_initialized = COCO_FALSE;
static coco_bool coco_old_compflux_interesting = COCO_FALSE;
static struct coco_message_compflux coco_old_compflux;

void
coco_post_client_connection_init( void )
{
  coco_old_votestate_initialized = COCO_FALSE;
  coco_old_compflux_initialized = COCO_FALSE;
  coco_old_compflux_interesting = COCO_FALSE;
}

#define COCO_HCFM_NAME( index ) \
  coco_comp_names[msg->compflux.comps[index].name_index]

/* return true if interesting */
coco_bool
coco_handle_compflux_message_helper( coco_msg_ptr msg, coco_nationality nat, 
                                     char *rolestr )
{
  int i, last_i = -1, next_last_i = -1;
  int count = 0;
  char buf[COCO_PLAYER_NAME_MAX*COCO_MAX_COMPS + 200];
  for (i=0; i<COCO_MAX_COMPS; i++)
    if (msg->compflux.comps[i].in_use)
      {
        coco_nationality cfnat = msg->compflux.comps[i].nationality;
        if (cfnat == nat) 
          {
            count++;
            next_last_i = last_i;
            last_i = i;
          }
      }
    else coco_comps[i].in_use = COCO_FALSE;
  switch (count) {
  case 0:
    return COCO_FALSE;
  case 1: 
    sprintf( buf, "Computer %s is %s",
             COCO_HCFM_NAME( last_i ), rolestr);
    break;
  case 2:
    sprintf( buf, "Computers %s and %s are %s",
             COCO_HCFM_NAME( last_i ), COCO_HCFM_NAME( next_last_i ),
             rolestr );
    break;
  default: /* 3 or more */
    {
      int j;
      sprintf( buf, "Computers " );
      for (j=0;j<last_i;j++)
        if ((msg->compflux.comps[j].in_use)
            && (msg->compflux.comps[j].nationality == nat))
          sprintf( buf + strlen( buf ), "%s, ", COCO_HCFM_NAME( j ) );
      sprintf( buf + strlen( buf ), "and %s are %s",
               COCO_HCFM_NAME( last_i ), rolestr );
            
    }
  }
  coco_display_notice( buf );
  return COCO_TRUE;
}

#undef COCO_HCFM_NAME


void
coco_handle_compflux_message( coco_msg_ptr msg )
{
  coco_nationality mynat = coco_get_player_nationality( COCO_NEUTRAL );
  int j;
  for (j=0; j<COCO_MAX_COMPS; j++)
    if (msg->compflux.comps[j].in_use)
      {
        coco_comps[j].in_use = COCO_TRUE;
        coco_comps[j].index = j;
        coco_comps[j].nationality = msg->compflux.comps[j].nationality;
        coco_comps[j].name_index = msg->compflux.comps[j].name_index;
      }
    else coco_comps[j].in_use = COCO_FALSE;
  if (coco_old_compflux_initialized && coco_old_compflux_interesting)
    {
      int i;
      for (i=0; i<COCO_MAX_COMPS; i++)
        {
          coco_bool oiu, niu;
          niu = msg->compflux.comps[i].in_use;
          oiu = coco_old_compflux.comps[i].in_use;
          if (niu != oiu)
            {
              char buf[200];
              if (niu)
                sprintf( buf, "Computer %s has joined %s.", 
                         coco_comp_names[msg->compflux.comps[i].name_index],
                         (msg->compflux.comps[i].nationality == mynat)
                         ? "your side" : "the enemy" );
              else
                sprintf( buf, "Computer %s has left the game.", 
                         coco_comp_names[coco_old_compflux.comps[i]
                                        .name_index] );
              coco_display_notice( buf );
            }

        }

    }
  else
    {
      coco_nationality onat
        = (mynat==COCO_GOOD_GUYS)?COCO_BAD_GUYS:COCO_GOOD_GUYS;

      coco_old_compflux_interesting 
        |= coco_handle_compflux_message_helper( msg, mynat, "on your side." );
      coco_old_compflux_interesting 
        |= coco_handle_compflux_message_helper( msg, onat, "working for the enemy." );

      coco_old_compflux_initialized = COCO_TRUE;
    }
  coco_old_compflux = msg->compflux;
}







void 
coco_handle_client_message( struct coco_stream *str, 
                            int udpfd, 
                            coco_u32 server_addr )
{
  coco_bool end_of_file_p;
  union coco_message message;
  assert( str );
  /* coco_cookie( "Handling client message\n" );*/
  if (coco_read_message( str, &message, &end_of_file_p ))
    {
      if (end_of_file_p)
        coco_fatal_printf( COCO_ERROR_NOT_IMPLEMENTED,
                           "EOF on server stream.\n" );
      switch (message.every.type) {
      case COCO_MESSAGE_QUIT:
        coco_fatal_printf( COCO_ERROR_NOT_IMPLEMENTED,
                           "Server quit (why?).\n" );
        break;
      case COCO_MESSAGE_VERSION:
        if (!coco_version_compatible_p( message.version.major,
                                        message.version.minor,
                                        COCO_NETWORK ))
          {
            coco_client_disconnect();
            coco_notify_version_incompatible( message.version.major,
                                              message.version.minor );
          }
        break;
      case COCO_MESSAGE_FULL:
        coco_fatal_printf( COCO_ERROR_NOT_IMPLEMENTED, "Server full.\n" );
        break;
      case COCO_MESSAGE_TALK:
        coco_call_message_notifications( COCO_MESSAGE_TALK, &message );
        break;
      case COCO_MESSAGE_VOTESTATE:
        coco_call_message_notifications( COCO_MESSAGE_VOTESTATE, &message );
        break;
      case COCO_MESSAGE_GAME:
        {
          coco_bool had_game_message = coco_have_game_message;
          coco_cookie( "Got game.  You are id %d\n", 
                       (int) message.game.your_player_id );
          if (!coco_have_game_message)
            coco_initialize_cstate( coco_get_cstate() );
          coco_have_game_message = COCO_TRUE;
          coco_my_player_id = message.game.your_player_id;
          coco_cookie( "Game status is \"%s\"\n", 
                       message.game.game_status_string );
          sprintf( coco_game_status_string, "%s",
                   message.game.game_status_string );

          {
            int i;
            for (i=0; i<COCO_MAX_PLAYERS; i++)
              {
                struct coco_player *player = coco_players + i;
                coco_nationality nationality = message.game.nationalities[i];
                int in_use = ! (COCO_NEUTRAL == nationality);
                player->in_use = in_use;
                if (in_use)
                  {
                    assert( COCO_PLAYER_NAME_MAX
                            > strlen( message.game.names[i] ) );
                    memcpy( player->name, message.game.names[i],
                            COCO_PLAYER_NAME_MAX );
                    player->nationality = nationality;
                  }
              }
            coco_refresh_window_eventually();
          }

          coco_call_message_notifications( COCO_MESSAGE_GAME, &message );
        
          if ((!had_game_message) && (coco_cl_nationality != COCO_NEUTRAL))
            coco_send_join_message( coco_cl_nationality );
          if (coco_start_game_asap)
            coco_send_play_message_maybe();
          break;
        }
      case COCO_MESSAGE_PLAY:
        coco_cookie( "GOT PLAY MSG\n" );
        /* the code for reading cstates already marks it true, and this
           code was causing packet dropping due do date mis-matches after
           a disconnect/reconnect pair: 
           coco_get_cstate()->in_use = COCO_TRUE; */
        coco_set_client_state( COCO_CLIENT_PLAY );
        if (coco_in_game_talk_notify < 0)
          coco_in_game_talk_notify 
            = coco_add_message_notification( COCO_MESSAGE_TALK, coco_note_talk, NULL );
        if (coco_in_game_vote_notify < 0)
          coco_in_game_vote_notify
            = coco_add_message_notification( COCO_MESSAGE_VOTESTATE, coco_note_votestate, NULL );
        if (!coco_have_play_message) 
          {
            coco_grab_pointer();
            coco_have_play_message = COCO_TRUE;
          }
        assert( coco_have_port_message );
        coco_call_message_notifications( COCO_MESSAGE_PLAY, &message );
        break;
      case COCO_MESSAGE_PORT:
        {
          struct sockaddr_in sockaddr;
          assert (udpfd >= 0);
          if (0==coco_server_addr_in_nbo)
            coco_warn_printf( "Looks like coco_server_addr_in_nbo isn't"
                              " initialized (if your ip address really IS"
                              " 0.0.0.0, ignore this message!)\n" );
          if (!coco_have_port_message)
            {
              coco_have_port_message = COCO_TRUE;
              sockaddr.sin_family = PF_INET;
              sockaddr.sin_port = message.port.port; /* already in nbo */
              sockaddr.sin_addr.s_addr = coco_server_addr_in_nbo;
#ifdef COCO_DEBUG
              coco_cookie( "Connecting udp to " );
              coco_format_ipaddr( stderr, 
                                  COCO_U32_NTOH(coco_server_addr_in_nbo),
                                  ntohs(message.port.port ), COCO_TRUE);
              coco_cookie( "\n" );
#endif
              if (-1 == connect( udpfd, (struct sockaddr *) &sockaddr, 
                                 sizeof( sockaddr ) ))
                coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                                   "Udp connect() failed with errno=%d: %s\n",
                                   errno, coco_errno_string( errno ) );
              coco_post_client_connection_init();
            }
        }
      case COCO_MESSAGE_COMPFLUX:
        coco_handle_compflux_message( &message );
        break;
      default: 
        coco_warn_printf( "fell through switch form in "
                          "coco_handle_client_message, msg type = %d\n",
                          message.every.type );
        break;
      }
    }
  /* coco_cookie( "         client message handled.\n" ); */
}

coco_bool
coco_send_join_message( coco_nationality nat )
{
  union coco_message msg;
  struct coco_stream *str = coco_get_server_message_stream();
  msg.every.type = COCO_MESSAGE_JOIN;
  msg.join.nationality = nat;
  return str ? coco_write_message( str, &msg ) : COCO_FALSE;
}

/* copies (obviously) the message over the network. */
coco_bool
coco_chat( char *message )
{
  int i;
  int len;
  struct coco_stream *str;
  union coco_message msg;
  msg.every.type = COCO_MESSAGE_CHAT;
  msg.chat.players = 0;
  /* for now just send to everyone */
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    if (coco_players[i].in_use)
      msg.chat.players |= 1<<i;
  len = strlen( message ) + 1;
  strncpy( msg.chat.message, message, COCO_MIN( COCO_CHAT_MAX, len ) );
  msg.chat.message[COCO_CHAT_MAX-1] = '\0'; /* just to be sure */
  str = coco_get_server_message_stream();
  return str ? coco_write_message( str, &msg ) : COCO_FALSE;
}


static int client_udp_fd = -1;

int
coco_get_client_udp_fd( void )
{
  return client_udp_fd;
}

void
coco_set_client_udp_fd( int value )
{
  client_udp_fd = value;
}


/* This isn't called on window-manager-kill of the x version because
   couldn't whack WM into sending me client message.  Oh well, the
   X server nuke does a pretty good job too.  */
void
coco_client_shutdown( coco_bool exit_too )
{
  coco_exit_sound_system();
  coco_close_fd( coco_get_client_udp_fd(), COCO_FALSE );
  coco_close_fd( coco_get_server_message_file_descriptor(), COCO_TRUE );
  coco_kill_window();
  coco_cookie( "Graceful client shutdown complete.\n" );
  if (exit_too)
    exit( 0 );
}



void 
coco_client_loop( void ) 
{
  coco_bool refresh_ok;
  struct timeval heartbeat;
  /* struct timezone tz; */
  /* struct timeval seltime, pactime, xtime, donetime, drawtime, swaptime; */
  int send_mouse_count = 3;
  coco_u32 loop_pass = 0;
  fd_set readset;
  refresh_ok = coco_set_inhibit_refresh( COCO_TRUE );
  coco_set_time_cookie_mode( COCO_NONE );
  while (1)
    {
      struct coco_stream *serv_str = coco_get_server_message_stream();
      int servfd, xfd, udpfd, maxfd;

      loop_pass = COCO_U32_ADD_MODULAR( loop_pass );

      servfd = (NULL == serv_str) ? -1 : serv_str->fd;
      xfd = coco_get_x_file_descriptor();
      udpfd = coco_get_client_udp_fd();

      FD_ZERO( &readset );
      if (servfd>=0) FD_SET( servfd, &readset );
      if (xfd>=0)    FD_SET( xfd, &readset );
      if (udpfd>=0)  FD_SET( udpfd, &readset );
      maxfd = COCO_MAX( udpfd, COCO_MAX( servfd, xfd ) );
      assert( maxfd >= 0 );

      /* assumes that at least one of these fds will be open. */
      coco_time_cookie( NULL );
      coco_time_cookie( "select" );
      heartbeat = coco_ideal_frame_rate; /* linux writes this struct every time */
      if ((-1==select( maxfd + 1, &readset, NULL, NULL, &heartbeat )))
        coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                           "select failed with errno=%d: %s\n", errno, 
                           coco_errno_string( errno ) );
      coco_time_cookie( "events" );
      /* for some reason I sometimes need and find events even when xfd
         ISNTSET */
      if ((xfd>=0)/* && FD_ISSET( xfd, &readset ) */)
        {
          coco_handle_ui_events();
          if (coco_query_disconnected())
            {
              coco_cookie( "continuing main loop.\n" );
              continue; /* get new FDs */
            }
        }
      if ((servfd>=0) && (FD_ISSET( servfd, &readset )))
        {
          coco_cookie( "Getting client message\n" );
          coco_handle_client_message( serv_str, coco_get_client_udp_fd(),
                                      coco_server_addr_in_nbo );
        }
      if ((udpfd>=0) && FD_ISSET( udpfd, &readset ))
        { 
          struct coco_cstate *cstate;
          coco_time_cookie( "cstate" );
          cstate = coco_read_cstate( udpfd );
          /* coco_cookie( "Read new cstate %ssuccessfully\n", cstate ? "" : "un" ); */
          /* Naw: if (NULL == cstate) cstate = coco_get_cstate(); */
          coco_time_cookie( "draw" );
          coco_refresh_window();
#ifdef COCO_DEBUG
          coco_warn_gl_errors("outer loop");
#endif /* COCO_DEBUG */
          coco_flush_pending_plays(); /* flush sound */
        }
      else
        {
          coco_time_cookie( "cstate" );
          coco_time_cookie( "draw" );
          coco_time_cookie( "swap" );
        }


      /* gettimeofday( &xtime, &tz ); */

      /* We used to only send mouse info if
         COCO_CLIENT_PLAY == coco_get_client_state(),
         but the server needs some udp packets to latch on to in order
         to figure out how to send crap back to us.  So to facilitate
         that we start sending him our mouse coords as soon as we've set
         up network contact. */
      coco_time_cookie( "sendmouse" );
      if ((udpfd>=0) && coco_have_game_message 
          && ((send_mouse_count > 0) || ((loop_pass&7) == 0))) 
        {
          if (send_mouse_count>0) send_mouse_count--;
          coco_write_client2server_packet
            ( udpfd, coco_get_keyboard_pointer_state() );
        }
      /*
        gettimeofday( &donetime, &tz );
        coco_subtract_timevals( &donetime, &donetime, &xtime );
        coco_subtract_timevals( &xtime, &xtime, &swaptime );
        coco_subtract_timevals( &swaptime, &swaptime, &drawtime );
        coco_subtract_timevals( &drawtime, &drawtime, &pactime );
        coco_subtract_timevals( &pactime, &pactime, &seltime );
        coco_cookie( "pactime %6d drawtime %6d swaptime %6d "
        "xtime %6d donetime %6d\n",
        pactime.tv_usec, drawtime.tv_usec, swaptime.tv_usec,
        xtime.tv_usec, donetime.tv_usec);
        coco_cookie( "drawtime %4d %6d   pactime %4d %6d\n",
        drawtime.tv_sec, drawtime.tv_usec, pactime.tv_sec,
        pactime.tv_usec ); */
    }
  coco_set_inhibit_refresh( refresh_ok );
}





/* return a stream.  Side effects: initialize coco_server_stream,
   get udp set up. */
struct coco_stream *
coco_client_connect_to_server( enum coco_network_error *err_return, 
                               int *err_arg_return )
{
  int hailfd;
  struct protoent *tcp;
  struct hostent *server_hostent;
  struct sockaddr_in sockaddr;

  if (-1 != coco_server_stream.fd) 
    return &coco_server_stream; /* already did init */
 
  coco_network_shared_setup();

  /* no looking at players[] until server sends a game.*/ 
  coco_have_game_message = COCO_FALSE;
  coco_have_port_message = COCO_FALSE;
  coco_have_play_message = COCO_FALSE;

  server_hostent = gethostbyname( coco_server ); /* see also inet_addr() */

  if (NULL==server_hostent)
    {
      coco_assign_network_error( COCO_NETERROR_DNS, err_return, NULL );
      return NULL;
    }
  /* was:
  coco_server_addr_in_nbo
    = (((coco_u32) ((unsigned char) server_hostent->h_addr[0]) << 0) +
       ((coco_u32) ((unsigned char) server_hostent->h_addr[1]) << 8) +
       ((coco_u32) ((unsigned char) server_hostent->h_addr[2]) << 16) +
       ((coco_u32) ((unsigned char) server_hostent->h_addr[3]) << 24)); */

  coco_server_addr_in_nbo = ((coco_u32 *) server_hostent->h_addr)[0];

  tcp = coco_get_protocol( "tcp" );

  sockaddr.sin_family = PF_INET;
  sockaddr.sin_port = htons( coco_port );
  sockaddr.sin_addr.s_addr = coco_server_addr_in_nbo;
  hailfd = socket( PF_INET, SOCK_STREAM, tcp->p_proto );
  if (-1==hailfd)
    {
      coco_assign_network_error( COCO_NETERROR_CREATE_TCP_SOCKET,
                                 err_return, NULL );
      return NULL;
    }
  
#ifdef COCO_DEBUG
  coco_cookie( "Connecting tcp to " );
  coco_format_ipaddr( stderr, COCO_U32_NTOH(coco_server_addr_in_nbo),
                      coco_port, COCO_TRUE);
  coco_cookie( "\n" );
#endif

  if (-1==connect( hailfd, (struct sockaddr *) &sockaddr, sizeof( sockaddr ) ))
    {
      coco_assign_network_error( COCO_NETERROR_CONNECT_TCP,
                                 err_return, err_arg_return );
      return NULL;
    }

  coco_server_stream.fd = hailfd;
  /* send initialization messages */
  {
    coco_bool write_success = COCO_TRUE;

    /* version */
    write_success &= coco_write_message( &coco_server_stream, 
                                        (coco_msg_ptr) &coco_my_version );
    /* player name */
    { 
      struct coco_message_name name = { COCO_MESSAGE_NAME };
      strcpy( name.name, coco_nickname() );
      write_success 
        &= coco_write_message( &coco_server_stream, (coco_msg_ptr) &name );
    }
    if (!write_success)
      {
        coco_assign_network_error( COCO_NETERROR_COCO_WRITE_TCP,
                                   err_return, NULL );
        return NULL;
      }
  }
  coco_set_fd_blocking( coco_server_stream.fd, COCO_FALSE );

  { /* make udp fd */
    coco_set_client_udp_fd
      ( coco_make_udp_fd( NULL, err_return, err_arg_return ) );
    if (-1 == coco_get_client_udp_fd())
      return NULL;
  }
  coco_set_client_state( COCO_CLIENT_CHATROOM_DIALOG );
  coco_refresh_window_eventually();
  return &coco_server_stream;
}

/* Loads up to LENGTH sounds, contigously, and returns the number
   successfully loaded, and storing the sounds into SOUNDS_RETURN. */
int
coco_load_sound_palette( char **sound_names, 
                         int length,
                         coco_sound *sounds_return )
{
  int i;
  for (i=0; i<length; i++)
    if (!coco_load_sound( sound_names[i], i, sounds_return + i ))
      {
        coco_warn_printf( "Couldn't load sounds starting with \"%s\"\n", 
                          sound_names[i] );
        return i;
      }
  return length;
}


void
coco_display_player_flux( void *null, union coco_message *message )
{
  static coco_bool have_previous = COCO_FALSE;
  static struct coco_message_game previous;
  if (!have_previous)
    have_previous = COCO_TRUE;
  else
    {
      int i;
      for (i=0; i<COCO_MAX_PLAYERS; i++)
        {
          coco_nationality msg_nat = message->game.nationalities[i];
          coco_nationality prev_nat = previous.nationalities[i];
          int my_id = message->game.your_player_id;
          if ((msg_nat != prev_nat)
              || ((msg_nat != COCO_NEUTRAL)
                  && strcmp( message->game.names[i], previous.names[i] )))
            {
              if ((msg_nat != COCO_NEUTRAL) && (my_id != i))
                {
                  char buf[400];
                  char *sidestr;

                  if (coco_get_client_state() != COCO_CLIENT_PLAY)
                    sidestr = " the game";
                  else if (message->game.nationalities[i]
                           == (message->game.nationalities[my_id]))
                    sidestr = " your side";
                  else sidestr = " the enemy";
                  
                  sprintf( buf, "%s has joined%s.",
                           message->game.names[i], sidestr);
                  coco_display_notice( buf );
                }
              if (((msg_nat==COCO_NEUTRAL)
                   || strcmp( message->game.names[i], previous.names[i] ))
                  && (previous.names[i][0] != '\0'))
                {
                  char buf[400];
                  sprintf( buf, "%s has left the game.", previous.names[i] );
                  coco_display_notice( buf );
                }
            }
        }
    }
  previous = message->game;
}


static coco_sound_type coco_game_sounds_size_loaded = 0; /* <= COCO_GAME_SOUNDS_SIZE */

void 
coco_client_exec( void )
{
  coco_add_message_notification( COCO_MESSAGE_GAME, coco_display_player_flux,
                                 NULL );
  coco_init_sound_system();
  coco_game_sounds_size_loaded
    = coco_load_sound_palette( coco_game_sound_names, 
                               COCO_GAME_SOUNDS_SIZE,
                               coco_game_sound_map );
  coco_make_window();
  coco_set_client_state( COCO_CLIENT_DEMO );
  coco_play_intro_movie();
  coco_client_loop();
}


char coco_talk_display_talk_buffer[COCO_TALK_DISPLAY_SIZE][COCO_CHAT_MAX];
char coco_talk_display_players_buffer[COCO_TALK_DISPLAY_SIZE][COCO_PLAYER_NAME_MAX];
time_t coco_talk_display_times[COCO_TALK_DISPLAY_SIZE];

int coco_talk_display_count = 0;
int coco_talk_display_ptr = 0;

/* coco_add_message_notification( COCO_MESSAGE_TALK, coco_note_talk, NULL ); */
void
coco_note_talk( void *null, union coco_message *message )
{
  int ptr = coco_talk_display_ptr;
  coco_u32 plr = COCO_U32_NTOH( message->talk.from_player );
  strcpy( coco_talk_display_talk_buffer[ptr], message->talk.message );
  strcpy( coco_talk_display_players_buffer[ptr], 
          (plr==COCO_U32_MAX)? "" : coco_players[plr].name );
  coco_talk_display_times[ptr] = time( NULL );
  coco_talk_display_ptr = ((ptr + 1) >= COCO_TALK_DISPLAY_SIZE) ? 0 : (ptr + 1);
  coco_talk_display_count = COCO_MAX( coco_talk_display_count, COCO_TALK_DISPLAY_SIZE );
}



void
coco_note_votestate( void *null, union coco_message *message )
{
  int nom;
  int myid = coco_my_player_id;
  for (nom=0; nom<COCO_MAX_NOMINATIONS; nom++)
    {
      coco_u32 new_votes = message->votestate.nominations[nom].votes;
      coco_u32 old_votes = coco_old_votestate.nominations[nom].votes;
      new_votes = COCO_U32_NTOH( new_votes );
      old_votes = COCO_U32_NTOH( old_votes );
      if ((message->votestate.nominations[nom].action_code
           != COCO_NOMINATE_NOT_IN_USE)
          && ((!coco_old_votestate_initialized)
              || (message->votestate.nominations[nom].action_code
                  != coco_old_votestate.nominations[nom].action_code)
              || (message->votestate.nominations[nom].act_object
                  != coco_old_votestate.nominations[nom].act_object)
              || (((new_votes>>myid)&1) != ((old_votes>>myid)&1))))
        {
          coco_bool voted_for = (new_votes>>myid)&1;
          int code = message->votestate.nominations[nom].action_code;
          char str[400];
          char obj[40] = "";
          if (voted_for)
            sprintf( str, "You are voting to %s%s.  "
                     "Press %d to change your mind.", 
                     coco_nominate_action_descriptions_other_tense[code],
                     obj, nom );
          else
            sprintf( str, "To vote for %s%s, press %d.", 
                     coco_nominate_action_descriptions[code],
                     obj, nom );
          coco_display_notice( str );
        }
    }
  coco_old_votestate = message->votestate;
  coco_old_votestate_initialized = COCO_TRUE;
}


/* display something like talk, but from the system instead of from a
   player.  Copies the string its passed.  */
void
coco_display_notice( char *short_string )
{
  union coco_message message;
  message.every.type = COCO_MESSAGE_TALK;
  message.talk.from_player = COCO_U32_HTON( COCO_U32_MAX );
  strncpy( message.talk.message, short_string, 
           sizeof( message.talk.message ) );
  /* make *sure* to null term it. */
  message.talk.message[sizeof( message.talk.message )-1] = '\0';
  coco_call_message_notifications( COCO_MESSAGE_TALK, &message );
}


void
coco_draw_talk_line( int talk_entry_idx, coco_gnum x, coco_gnum y,
                     struct coco_font *font )
{
  char buff[COCO_CHAT_MAX + COCO_PLAYER_NAME_MAX + 10];
  if (*coco_talk_display_players_buffer[talk_entry_idx])
    sprintf( buff, "%s:  %s",
             coco_talk_display_players_buffer[talk_entry_idx], 
             coco_talk_display_talk_buffer[talk_entry_idx] );
  else sprintf( buff, "%s", coco_talk_display_talk_buffer[talk_entry_idx] );
  coco_draw_string( x, y, buff, -1, font, COCO_BOTTOM );
}


#define COCO_TALK_DISPLAY_TIME 12 /* seconds */

void
coco_draw_talk( long tv_sec_now )
{
  struct coco_font *font = &coco_default_font;
  int x = 100;
  int h = COCO_FONT_INTERLINE_SPACING( font );
  int ptr = coco_talk_display_ptr;
  int i;
  int y = COCO_RADAR_Y;
  coco_draw_color( COCO_COLOR_CHAT );
  for (i=ptr; i<COCO_TALK_DISPLAY_SIZE; i++)
    if ((tv_sec_now - COCO_TALK_DISPLAY_SIZE) <= coco_talk_display_times[i])
      coco_draw_talk_line( i, x, y-=h, font );
  for (i=0; i<ptr; i++)
    if ((tv_sec_now - COCO_TALK_DISPLAY_SIZE) <= coco_talk_display_times[i])
      coco_draw_talk_line( i, x, y-=h, font );
}



struct message_notifier_record {
  message_fn fn; /* Null if this record is unused. */
  void *data;
};

static struct message_notifier_record *coco_game_message_notifiers = NULL;
static int coco_game_message_notifiers_size = 0;
static struct message_notifier_record *coco_talk_message_notifiers = NULL;
static int coco_talk_message_notifiers_size = 0;
static struct message_notifier_record *coco_play_message_notifiers = NULL;
static int coco_play_message_notifiers_size = 0;
static struct message_notifier_record *coco_vote_message_notifiers = NULL;
static int coco_vote_message_notifiers_size = 0;


#define COCO_INIT_NOTIFIER_STORAGE( structs, size ) \
  switch (message_type) {  \
  case COCO_MESSAGE_TALK: \
    size = coco_talk_message_notifiers_size;\
    structs = coco_talk_message_notifiers;\
    break; \
  case COCO_MESSAGE_GAME: \
    size = coco_game_message_notifiers_size; \
    structs = coco_game_message_notifiers; \
    break;\
  case COCO_MESSAGE_PLAY: \
    size = coco_play_message_notifiers_size; \
    structs = coco_play_message_notifiers; \
    break;\
  case COCO_MESSAGE_VOTESTATE: \
    size = coco_vote_message_notifiers_size; \
    structs = coco_vote_message_notifiers; \
    break;\
  default:\
    coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,\
                       "Message type %d doesn't support notifcation.\n",\
                       message_type );\
    size = 0;\
    structs = NULL;\
  }



coco_notification_id 
coco_add_message_notification( int message_type, message_fn fn, void *data )
{
  int i, new_size;
  int increment = 8;
  int *message_notifiers_size;
  struct message_notifier_record **message_notifiers;

  switch (message_type) { 
  case COCO_MESSAGE_TALK:
    message_notifiers_size = &coco_talk_message_notifiers_size;   
    message_notifiers      = &coco_talk_message_notifiers;
    break;
  case COCO_MESSAGE_GAME:
    message_notifiers_size = &coco_game_message_notifiers_size;
    message_notifiers      = &coco_game_message_notifiers;
    break;
  case COCO_MESSAGE_PLAY:
    message_notifiers_size = &coco_play_message_notifiers_size;
    message_notifiers      = &coco_play_message_notifiers;
    break;
  case COCO_MESSAGE_VOTESTATE:
    message_notifiers_size = &coco_vote_message_notifiers_size;
    message_notifiers      = &coco_vote_message_notifiers;
    break;
  default:
    coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                       "Message type %d doesn't support notifcation.\n",
                       message_type );
    message_notifiers_size = NULL;
    message_notifiers = NULL;
  };

  for (i=0; i < *message_notifiers_size; i++)
    if (NULL == (*message_notifiers)[i].fn)
      {
        (*message_notifiers)[i].fn = fn;
        (*message_notifiers)[i].data = data;

        return (coco_notification_id) i;
      }

  /* expand array. */
  new_size = increment + *message_notifiers_size;
  *message_notifiers
    = (struct message_notifier_record *)
    coco_realloc( *message_notifiers,
                  new_size * sizeof( struct message_notifier_record ) );
  for (i=*message_notifiers_size; i<new_size; i++)
    (*message_notifiers)[i].fn = NULL;
  *message_notifiers_size = new_size;

  return coco_add_message_notification( message_type, fn, data );
}

void
coco_delete_message_notification( int message_type, coco_notification_id id )
{
  int message_notifiers_size;
  struct message_notifier_record *message_notifiers;
  COCO_INIT_NOTIFIER_STORAGE( message_notifiers, message_notifiers_size );
  assert( id >= 0 );
  assert( id < message_notifiers_size );
  assert( message_notifiers[id].fn != NULL );
  message_notifiers[id].fn = NULL;
}

void
coco_call_message_notifications( int message_type, 
                                 union coco_message *message )
{
  int message_notifiers_size;
  struct message_notifier_record *message_notifiers;
  int i;
  COCO_INIT_NOTIFIER_STORAGE( message_notifiers, message_notifiers_size );
  for (i=0; i<message_notifiers_size; i++)
    if (message_notifiers[i].fn != NULL)
      message_notifiers[i].fn( message_notifiers[i].data, message );
}



