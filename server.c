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

/* hack to actually get timercmp under glibc
   Need to actually ensure using glibc before using _BSD_SOURCE. */
#include <sys/time.h>
#ifndef timercmp
/* glibc 2.2 or 2.3 won't define timercmp unless either non "-ansi" or
   _BSD_SOURCE */
#define timercmp(a, b, CMP) 						      \
 (((a)->tv_sec == (b)->tv_sec) ? 					      \
  ((a)->tv_usec CMP (b)->tv_usec) : 					      \
  ((a)->tv_sec CMP (b)->tv_sec))
#endif


#include <time.h>               /* for time() */
#include <sys/types.h>          /* for select() */
#include <unistd.h>             /* for select() */
#include <sys/types.h>          /* for socket() */
#include <sys/socket.h>         /* for socket() */
#include <assert.h>             /* for struct protoent */
#include <netinet/in.h>         /* i guess this is how i get sockaddr_in */
#include <netdb.h>       
#include <errno.h>
#include <stdlib.h>             /* for abs, rand */
#include <string.h>             /* for memcpy */
#include <math.h>               /* for stinkin fmod (%, hello?) */
#include "coco.h"
#include "rasters.h"       /* for raster dimensions */

/* NULL iff not launced with --tutorial flag, otherwise points
 * to the right entry of coco_tutorial_infos. */
struct coco_tutorial_info *coco_tutorial = NULL;

void
coco_cam_bounds( coco_sstate *sstate, coco_sobject *cam_object, /* possibly null */ 
                 int p_or_c_index,
                 coco_bool is_comp,
                 coco_gnum *cam_left,
                 coco_gnum *cam_right );


coco_sobject *
coco_find_camera( coco_sstate *sstate, int p_or_c_index, coco_bool is_comp );

void *
coco_format_sstate_to_buffer( coco_sstate *sstate, int p_or_c_index, 
                              coco_bool is_comp, coco_bool network_byte_order,
                              size_t *buffer_size, /* written nonnull */
                              coco_cobject **cobjects, /* written nonnull */
                              struct coco_cblip **cblips ); /* written nonnull */


void coco_add_hazard_pay( coco_sstate *sstate );


coco_sobject *
coco_make_hut_and_friends( coco_sstate *sstate, coco_gnum x, 
                           coco_nationality nationality );


static coco_bool coco_send_everyone_game_message( void );
static coco_bool coco_send_everyone_compflux_message( void );
static coco_bool coco_send_everyone_votestate_message( coco_sstate *sstate );
static coco_bool coco_send_player_compflux_messages( int plr_idx );

static coco_sstate the_coco_sstate; /* = { COCO_FALSE, NULL, NULL, 0, 0, 0 }; */

/* reader for sstates, called by toplevel functions */
coco_sstate *
coco_get_sstate( void )
{
  return &the_coco_sstate;
}

coco_gnum 
coco_get_board_w()
{
  return the_coco_sstate.board_w;
}

coco_bool coco_handle_player_quit( int player_index );

/* hack to keep the sky from falling. */
static coco_bool coco_allocate_sobject_may_resize_p = COCO_TRUE; 

void
coco_ensure_free_sobjects( coco_sstate *sstate, int free_count )
{
  int new_size;
  coco_sobject *new_ptr;
  assert( coco_allocate_sobject_may_resize_p );
  { 
    coco_sobject *sobject  = sstate->sobjects;
    coco_sobject *end=   sobject  + sstate->sobject_size;
    for (; sobject < end; sobject++)
      if (!COCO_SOBJECT_IN_USE( sobject ))
        {
          free_count--;
          if (free_count <= 0) return;
        }
  };
  coco_cookie( "free_count ended up being %d\n", free_count );
  new_size = free_count + sstate->sobject_size;
  new_ptr
    = coco_realloc( sstate->sobjects, new_size * sizeof( coco_sobject ) );
  if (NULL == new_ptr)
    coco_warn_printf( "Realloc returned NULL.  This is "
                      "looking mighty bad...\n" );
  else 
    {
      int i;
      for (i=sstate->sobject_size; i<new_size; i++)
        {
          COCO_SOBJECT_IN_USE( new_ptr + i ) = COCO_FALSE;
          COCO_SOBJECT_INDEX( new_ptr + i ) = i;
        }
      coco_cookie( "Growing sobjects.  %p - %p (%d) now invalid, %p - %p (%d) "
                   "now valid\n", sstate->sobjects, 
                   (char *)(sstate->sobjects+sstate->sobject_size), 
                   sstate->sobject_size, new_ptr,
                   (char *)(new_ptr + new_size), new_size );
      sstate->sobject_size = new_size;
      sstate->sobjects = new_ptr;
    }
}

/* may fail returning null.  Suckorif. */
coco_sobject *
coco_sstate_allocate_sobject( coco_sstate *sstate )
{
  int fill_index = sstate->sobject_fill_index;
  int size = sstate->sobject_size;
  coco_sobject *sobjects = sstate->sobjects;
  while (fill_index < size)
    {
      coco_sobject *sobject = sobjects + fill_index;
      fill_index++;
      if (COCO_FALSE == COCO_SOBJECT_IN_USE( sobject ))
        {
          sstate->sobject_fill_index = fill_index;
          COCO_SOBJECT_IN_USE( sobject ) = COCO_TRUE;
          return sobject;
        }
    }
  /* ran out of room in sobjects */
  if (coco_allocate_sobject_may_resize_p)
    {
      /* obsoleted by coco_ensure_free_sobjects:
         int size = sstate->sobject_size;
         coco_sobject *sobjects = sstate->sobjects;
         int newsize = size + 128;
         int i;
         sstate->sobject_fill_index = size;
         sstate->sobject_size = newsize;
         sstate->sobjects = sobjects
         = coco_realloc( sobjects, newsize * sizeof( coco_sobject ) );
         for (i=size; i<newsize; i++)
         {
         coco_sobject *sobject = sobjects + i;
         COCO_SOBJECT_IN_USE( sobject ) = COCO_FALSE;
         COCO_SOBJECT_INDEX( sobject ) = i;
         }
      */
      coco_ensure_free_sobjects( sstate, 512 );
      return coco_sstate_allocate_sobject( sstate );
    }
  else return NULL;
}

void
coco_schedule_comp_respawn_maybe( int comp_index, coco_sstate *sstate )
{
  if (coco_comps[comp_index].queue_remove)
    {
      coco_comps[comp_index].in_use = COCO_FALSE;
      coco_send_everyone_compflux_message();
      coco_cookie( "Removed comp %d", comp_index );
    }
  else
    {
      sstate->comp_respawn[comp_index] = COCO_TRUE;
      sstate->comp_respawn_times[comp_index]
        = COCO_U32_ADD_N_MODULAR( sstate->time, 
                                  COCO_IDEAL_FRAMES_PER_SECOND
                                  * COCO_RESPAWN_SECONDS );
      coco_cookie( "scheduling comp %d respawn\n", comp_index );
    }
}


void 
coco_sstate_delete_sobject( coco_sstate *sstate, 
                            coco_sobject *sobject )
{
  signed char cam_index = COCO_SOBJECT_CAMERA_INDEX( sobject );
  signed char comp_index = COCO_SOBJECT_COMP_INDEX( sobject );
  unsigned int index = COCO_SOBJECT_INDEX( sobject );
  int fill_index = sstate->sobject_fill_index;
  coco_bool inviso_cam_p
    = coco_sobject_subtype_p( COCO_SOBJECT_CL( sobject ),
                              COCO_INVISO_CAMERA_CLASS );
  assert( COCO_SSTATE_OBJECT_REF( sstate, index ) == sobject );
  assert( (-1 == cam_index)
          || COCO_SSTATE_CAMERA( sstate, cam_index ) == sobject );
  COCO_SOBJECT_IN_USE( COCO_SSTATE_OBJECT_REF( sstate, index ) ) = COCO_FALSE;
  if (index < fill_index)
    sstate->sobject_fill_index = index;
  if ((-1 != cam_index) && !inviso_cam_p)
    {
      sstate->camera_indicies[cam_index] = -1;
      sstate->respawn[cam_index] = COCO_TRUE;
      sstate->respawn_times[cam_index]
        = COCO_U32_ADD_N_MODULAR( sstate->time, 
                                  COCO_IDEAL_FRAMES_PER_SECOND
                                  * COCO_RESPAWN_SECONDS );
    }
  if (-1 != comp_index)
    coco_schedule_comp_respawn_maybe( comp_index, sstate );
}

void
coco_init_sstate( coco_sstate *sstate, coco_gnum width )
{
  int i;
  sstate->game_in_progress = COCO_FALSE;
  sstate->level_name = NULL;
  sstate->sobjects = NULL;
  sstate->sobject_fill_index = 0;
  sstate->sobject_size = 0;
  /* does nothing */
  /*
  for (i=0; i < sstate->sobject_size; i++)
    COCO_SOBJECT_IN_USE( COCO_SSTATE_OBJECT_REF( sstate, i ) ) = COCO_FALSE;
  */
  sstate->time = 0;
  sstate->board_w = width;
  sstate->game_over_nationality = COCO_NEUTRAL;
  for (i=0; i < COCO_MAX_PLAYERS; i++)
    {
      sstate->respawn[i] = COCO_FALSE;
      sstate->favorite_copter_class[i] = COCO_COBRA_CLASS;
    }
  for (i=0; i < COCO_MAX_COMPS; i++)
    {
      sstate->comp_respawn[i] = COCO_FALSE;
    }
  for (i=0; i < 2; i++)
    {
      /* pad_index will be initialized later */
      sstate->teams[i].landing_pad_index = -1;
      sstate->teams[i].summon_enable = COCO_TRUE;
    }
  for (i=0; i < COCO_MAX_PLAYERS; i++)
    /* these will be initialized later too */
    sstate->camera_indicies[i] = -1;
  for (i=0; i < COCO_MAX_NOMINATIONS; i++)
    sstate->nominations[i].action_code = COCO_NOMINATE_NOT_IN_USE;
  sstate->nominations_cursor = 0;
}


#ifdef COCO_INIT_HOOK /* an old, bad idea */
void coco_finalize_sobject( coco_sobject *sobject, coco_sobject_cl *cl )
{
  COCO_SOBJECT_HEALTH( sobject ) = COCO_SOBJECT_HEALTH_MAX( sobject );
  COCO_SOBJECT_DX_PUBLIC( sobject ) = COCO_SOBJECT_DX( sobject );
  COCO_SOBJECT_DY_PUBLIC( sobject ) = COCO_SOBJECT_DY( sobject );
}
#endif

/* _NETWORK_CRUD___________________________________________________ */


struct coco_stream coco_streams[COCO_MAX_PLAYERS];
int coco_udp_ports[COCO_MAX_PLAYERS]; /* initialized to -1s, in HOST byte order */
coco_u32 coco_client_addresses[COCO_MAX_PLAYERS]; /* initialzed to 0, in HOST byte order */

/* return -1 on error */
int 
coco_find_player_index_by_fd( fd )
{
  int i;
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    if (coco_players[i].in_use && (coco_streams[i].fd == fd))
      return i;
  return -1;
}

static struct coco_keyboard_pointer_state
  coco_remote_input_states[COCO_MAX_PLAYERS];
static struct coco_keyboard_pointer_state
  coco_old_remote_input_states[COCO_MAX_PLAYERS];

struct coco_stream coco_shut_out_stream;
struct coco_message_full coco_server_full_message = { COCO_MESSAGE_FULL };

/* return a statically allocated game message, which depends on the
   players array. */
struct coco_message_game *
coco_message_game( unsigned char player_id )
{
  static struct coco_message_game game = { COCO_MESSAGE_GAME, 0, "forming" };
  int i;
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    {
      struct coco_player *player = coco_players + i;
      coco_bool in_use;
      char *name;
      in_use = player->in_use;
      name = in_use ? player->name : "";
      game.nationalities[i] = in_use ? player->nationality : COCO_NEUTRAL;
      memcpy( game.names[i], name, strlen( name ) );
    }
  game.your_player_id = player_id;
  return &game;
}



void
coco_reset_player_globals( int player_index )
{
  int i = player_index;
  coco_udp_ports[i] = -1;
  coco_client_addresses[i] = 0;
  coco_remote_input_states[i].client_time = COCO_U32_MAX;
  coco_clear_bit_vec( coco_remote_input_states[i].key_state,
                      COCO_KEYLIMIT, 0 );
  coco_remote_input_states[i].pointer_x = 0;
  coco_remote_input_states[i].pointer_y = 0;
  coco_old_remote_input_states[i] = coco_remote_input_states[i];
}


void
coco_server_setup( int *tcpfd, int *out_udpfd, 
                   int *in_udpfd, unsigned short *in_udp_port )
{
  struct protoent *tcp;
  int listenfd;
  struct sockaddr_in sockaddr;
  assert( tcpfd );
  coco_network_shared_setup();
  {
    int i;
    for (i=0; i<COCO_MAX_PLAYERS; i++)
      coco_reset_player_globals( i );
  }
  tcp = coco_get_protocol("tcp");
  listenfd = socket( PF_INET, SOCK_STREAM, tcp->p_proto );
  if (-1==listenfd)
    coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                       "Couldn't create tcp socket.\n");
  sockaddr.sin_family = PF_INET;
  sockaddr.sin_port = htons( coco_port );
  sockaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  if (-1==bind( listenfd, (struct sockaddr *) &sockaddr, sizeof( sockaddr ) ))
    coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                       "Bind failed with errno=%d: %s\n",
                       errno, coco_errno_string( errno ) );
  if (-1==listen( listenfd, SOMAXCONN ))
    coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                       "listen failed with errno=%d: %s\n", errno, 
                       coco_errno_string( errno ) );
  coco_set_fd_blocking( listenfd, COCO_FALSE );
  *tcpfd = listenfd;
  *out_udpfd = *in_udpfd = coco_make_udp_fd( in_udp_port, NULL, NULL );
}

union coco_message *
coco_get_port_message( unsigned short udp_port )
{
  static coco_bool used = COCO_FALSE;
  static union coco_message msg;
  if (used)
    {
      assert( msg.port.port == htons( udp_port ) );
    }
  else;
    {
      used = COCO_TRUE;
      msg.port.type = COCO_MESSAGE_PORT;
      msg.port.port = htons( udp_port );
    }
  return &msg;
}


void 
coco_handle_connection_request( int listenfd, 
                                unsigned short in_udp_port, 
                                coco_sstate *sstate_or_null)
{ 
  struct sockaddr connection;
  unsigned int connlength = sizeof( connection );
  int connfd;
  connfd = accept( listenfd, &connection, &connlength );
  if (-1 == connfd)
    if ((connfd==EAGAIN) || (connfd==EWOULDBLOCK))
      return;
    else if ((connfd==ENETDOWN) ||
             (connfd==EPROTO) ||
             (connfd==ENOPROTOOPT) ||
             (connfd==EHOSTDOWN) ||
             (connfd==ENONET) ||
             (connfd==EHOSTUNREACH) ||
             (connfd==EOPNOTSUPP) ||
             (connfd==ENETUNREACH))
      {
        /* see man accept(2) on why we take all this crab. */
        coco_handle_connection_request( listenfd, 
                                        in_udp_port, sstate_or_null );
        return;
      }
    else coco_warn_printf( "accept failed with errno=%d: %s\n", errno, 
                           coco_errno_string( errno ) );
  else
    {
      int i;
      struct coco_stream *str;
      for (i=0; i<COCO_MAX_PLAYERS; i++)
        if (!coco_players[i].in_use)
          {
            coco_players[i].in_use = COCO_TRUE;
            coco_players[i].nationality 
              = (i&1) ? COCO_BAD_GUYS : COCO_GOOD_GUYS;
            coco_players[i].sent_play_message = COCO_FALSE;
            if (sstate_or_null && sstate_or_null->game_in_progress)
              {
                coco_players[i].funds = 0;
                coco_players[i].copters = 0;
              }
            else
              {
                coco_players[i].funds = 16;
                coco_players[i].copters = 3;
              }
            if (sstate_or_null)
              {
                sstate_or_null->respawn[i] = COCO_TRUE;
                sstate_or_null->respawn_times[i]
                  = COCO_U32_ADD_N_MODULAR( sstate_or_null->time, 
                                            COCO_IDEAL_FRAMES_PER_SECOND
                                            * COCO_RESPAWN_SECONDS );
              }
            break;
          }
      str = (COCO_MAX_PLAYERS == i) ? 
        &coco_shut_out_stream :
        coco_streams + i;
      str->fd = connfd;
      str->buffer_in_use = 0;
      if (i >= COCO_MAX_PLAYERS)
        {
          /* intentionally ignore return value: */
          coco_write_message( &coco_shut_out_stream, 
                              (coco_msg_ptr) &coco_server_full_message );
          coco_close_fd( connfd, COCO_TRUE );
          return;
        }
      if ((!coco_write_message( str, (union coco_message *) &coco_my_version ))
          ||(!coco_write_message( str, coco_get_port_message( in_udp_port ) ))
          ||(!coco_write_message( str, (union coco_message *) coco_message_game( i )))
          ||(!coco_send_player_compflux_messages( i )))
        /* intentionally ignore return value: */
        coco_handle_player_quit( i );

#ifdef COCO_IGNORE
      /* Used to auto-send a play message if game was in progress.  Now
         that there is an actuall GUI, we want to wait for them to pick
         initial teams and such. */
      if (sstate_or_null && sstate_or_null->game_in_progress)
        {
          play_message.every.type = COCO_MESSAGE_PLAY;
          if (!coco_write_message( str, &play_message ))
            /* intentionally ignore return value: */
            coco_handle_player_quit( i );
        }
#endif /* COCO_IGNORE */
        
      coco_set_fd_blocking( connfd, COCO_FALSE );
    }
}

/* returns true iff we should keep the server up */
coco_bool
coco_handle_player_quit( int player_index )
{
  coco_bool some_players = COCO_FALSE;
  struct coco_player *player = coco_players + player_index;
  /* clean sstate */
  coco_sstate *sstate;
  int index, i;
  sstate = coco_get_sstate();
  if (sstate && sstate->game_in_progress)
    {
      index = sstate->camera_indicies[player_index];
      if (-1 != index) 
        {
          coco_sobject *sobject = COCO_SSTATE_OBJECT_REF( sstate, index );
          /* this sets respawn to true */
          coco_sobject_delete( sobject, COCO_SOBJECT_CL( sobject ), sstate );
          /* but it can't hurt to make it false. */
          sstate->respawn[player_index] = COCO_FALSE;
        }
    }
  player->in_use = COCO_FALSE;
  coco_close_fd( coco_streams[player_index].fd, COCO_TRUE );

  coco_reset_player_globals( player_index );

  for (i=0; i<COCO_MAX_PLAYERS; i++)
    if (coco_players[i].in_use)
      {
        some_players = COCO_TRUE;
        break;
      }
  return some_players && coco_send_everyone_game_message();
}

/* returns true iff server should be kept up. */
static coco_bool
coco_send_play_and_mark_thusly( int player_index )
{
  union coco_message play;
  play.every.type = COCO_MESSAGE_PLAY;
  if (coco_write_message( coco_streams + player_index, &play ))
    return coco_players[player_index].sent_play_message = COCO_TRUE;
  else return coco_handle_player_quit( player_index );
}


struct coco_comp coco_comps[COCO_MAX_COMPS];



/* returns idx or -1 */
int
coco_add_comp( coco_nationality nat, coco_sstate *sstate )
{
  static int name_offset = -1;
  coco_bool found = COCO_FALSE;
  int i; 

  if (name_offset < 0)
    name_offset = time( NULL ) % coco_ncomp_names;
  for (i=0; i<COCO_MAX_COMPS; i++)
    if (!coco_comps[i].in_use)
      {
        unsigned char name_index = (name_offset + i) % coco_ncomp_names;
        coco_comps[i].nationality = nat;
        coco_comps[i].funds = 0;
        coco_comps[i].copters = 0;
        coco_comps[i].queue_remove = COCO_FALSE;
        coco_comps[i].name_index = name_index;
        coco_comps[i].in_use = COCO_TRUE;
        coco_comps[i].goto_area = sstate->board_w>>1;
        coco_schedule_comp_respawn_maybe( i, sstate );
        coco_send_everyone_compflux_message();
        found = COCO_TRUE;
        break;
      }
  if (!found)
    {
      coco_warn_printf( "Could not add comp because there are already %d\n",
                        COCO_MAX_COMPS );
      return -1;
    }
  return i;
}



int
coco_queue_comp_removal( coco_nationality nat, coco_sstate *sstate )
{
  int i;
  for (i=0; i<COCO_MAX_COMPS; i++)
    if (coco_comps[i].in_use
        && (coco_comps[i].nationality == nat)
        && !coco_comps[i].queue_remove)
      {
        coco_comps[i].queue_remove = COCO_TRUE;
        return i;
      }
  /* this is done when the comp is really removed:
     coco_send_everyone_compflux_message(); */
  return -1;
}

void
coco_init_comps( coco_sstate *sstate )
{
  int i;
  for (i=0; i<COCO_MAX_COMPS; i++)
    {
      coco_comps[i].in_use = COCO_FALSE;
      coco_comps[i].index = i;
      coco_comps[i].cruise_altitude = COCO_VIEWABLE_H;
      coco_comps[i].overgun = 5;
      coco_comps[i].refuelling = COCO_FALSE;
      coco_comps[i].queue_remove = COCO_FALSE;
    }
  for (i=0; i<coco_command_line_left_comps; i++)
    coco_add_comp( COCO_BAD_GUYS, sstate );
  for (i=0; i<coco_command_line_right_comps; i++)
    coco_add_comp( COCO_GOOD_GUYS, sstate );
}


void
coco_handle_nominate_message( struct coco_message_nominate *msg, 
                              coco_sstate *sstate, int player_idx )
{
  int cursor;
  struct coco_nomination_status *status;
  if (-1 == msg->serial)
    {
      /* new nomination */
      int i;

      cursor = (1 + sstate->nominations_cursor) % COCO_MAX_NOMINATIONS;
      sstate->nominations_cursor = cursor;

      status = sstate->nominations + cursor;

      status->action_code = msg->action_code;
      status->act_object = msg->act_object;
      for (i=0; i<COCO_MAX_PLAYERS; i++)
        status->votes[i] = COCO_FALSE;
      status->votes[player_idx] = msg->vote;
    }
  else
    {
      /* existing nominations */
      cursor = msg->serial;
      status = sstate->nominations + cursor;
      if ((status->action_code == msg->action_code)
          && (status->act_object == msg->act_object))
        status->votes[player_idx] = msg->vote;
      else coco_cookie( "Stale vote from player %d for nomination "
                        "0x%x ignored", player_idx, cursor );
    }
  /* review votes */
  {
    int votes_for = 0;
    int votes_against = 0;
    int i;
    coco_bool passing;
    unsigned char code = msg->action_code;

    for (i=0; i<COCO_MAX_PLAYERS; i++)
      if (coco_players[i].in_use)
        {
          if (status->votes[i])
            votes_for++;
          else 
            votes_against++;
        }
    passing = (votes_for > votes_against);

    coco_cookie( "After vote %s for player %d, tally is %d for,"
                 " %d against, looking to %s\n",
                 msg->vote ? "FOR" : "AGAINST", player_idx, 
                 votes_for, votes_against, passing ? "PASS" : "FAIL" );

    if (passing && ((code == COCO_NOMINATE_ADD_GOOD_GUY_COMP)
                    || (code == COCO_NOMINATE_ADD_BAD_GUY_COMP)))
      {
        coco_nationality nat = (code == COCO_NOMINATE_ADD_GOOD_GUY_COMP)
          ? COCO_GOOD_GUYS : COCO_BAD_GUYS;
        coco_bool one_opp_player = COCO_FALSE;
        coco_bool one_opp_favors = COCO_FALSE;
        int i;
        for (i=0; i<COCO_MAX_PLAYERS; i++)
          if (coco_players[i].in_use 
              && (coco_players[i].nationality!=nat))
            {
              one_opp_player = COCO_TRUE;
              if (status->votes[i])
                {
                  one_opp_favors = COCO_TRUE;
                  break;
                }
            }
        passing &= (one_opp_favors || ! one_opp_player);

        coco_cookie( "After doing the representation test, passing = %s\n",
                     passing ? "PASS" : "FAIL" );
      }
    if (passing)
      {
        switch (code) {
        case COCO_NOMINATE_ADD_GOOD_GUY_COMP:
        case COCO_NOMINATE_ADD_BAD_GUY_COMP:
          { 
            coco_nationality nat = (code==COCO_NOMINATE_ADD_GOOD_GUY_COMP)
              ? COCO_GOOD_GUYS : COCO_BAD_GUYS;
            int comp;
            comp = coco_add_comp( nat, sstate );
          }
          break;
        case COCO_NOMINATE_BOOT_GOOD_GUY_COMP:
        case COCO_NOMINATE_BOOT_BAD_GUY_COMP:
          {
            coco_nationality nat = (code==COCO_NOMINATE_BOOT_GOOD_GUY_COMP)
              ? COCO_GOOD_GUYS : COCO_BAD_GUYS;
            coco_queue_comp_removal( nat, sstate );
          }
          break;
        default:
          coco_warn_printf( "Unrecognised code in "
                            "coco_handle_nominate_message: %d.\n", code );
          break;
        }
        status->action_code = COCO_NOMINATE_NOT_IN_USE;
      }
  }
}

/* returns true iff server should be kept up. */
coco_bool
coco_handle_server_message( int player_index )
{
  coco_bool end_of_file_p;
  struct coco_player *player;
  struct coco_stream *str;
  union coco_message message;
  int need_game_refresh = 0;
  int need_votestate_refresh = 0;
  player = coco_players + player_index;
  str = coco_streams + player_index;
  while (1)
    {
      coco_bool read_result;
      read_result = coco_read_message( str, &message, &end_of_file_p );
      if (end_of_file_p)
        {
          coco_cookie( "propagating eof...\n" );
          return coco_handle_player_quit( player_index );
        }
      if (!read_result)
        break;
      switch (message.every.type) {
      case COCO_MESSAGE_QUIT:
        return coco_handle_player_quit( player_index );
        break;
      case COCO_MESSAGE_NAME:
        coco_cookie( "Client changing name to \"%s\".\n", message.name.name );
        memcpy( player->name, message.name.name, COCO_PLAYER_NAME_MAX );
        need_game_refresh = 1;
        break;
      case COCO_MESSAGE_JOIN:
        player->nationality = message.join.nationality;
        need_game_refresh = 1;
        coco_cookie( "Got join\n" );
        break;
      case COCO_MESSAGE_PLAY:
        {
          coco_sstate *sstate = coco_get_sstate();
          /* special case: if the game has not started yet
             then a play from any client start all clients.  
             because they're in the chatroom chatting. */
          if (sstate->game_in_progress)
            return coco_send_play_and_mark_thusly( player_index );
          else 
            {
              int i;
              coco_cookie( "Play message recieved, hold on...\n" );
              /* sstate_init_cherbourg( sstate ); */
              sstate->game_in_progress = COCO_TRUE;
              for (i=0; i<COCO_MAX_PLAYERS; i++)
                if (coco_players[i].in_use)
                  if (!coco_send_play_and_mark_thusly( i ))
                    return COCO_FALSE;
            }
        }
        break;
      case COCO_MESSAGE_NOMINATE:
        {
          coco_sstate *sstate = coco_get_sstate();
          coco_handle_nominate_message( &(message.nominate), sstate,
                                        player_index );
          need_votestate_refresh = 1;
          break;
        }
      case COCO_MESSAGE_CHAT:
        {
          unsigned int players;
          int i;
          struct coco_message_talk talk = { COCO_MESSAGE_TALK };
          talk.from_player = COCO_U32_HTON( player_index );
          memcpy( talk.message, message.chat.message, COCO_CHAT_MAX );
          players = COCO_U32_NTOH( message.chat.players );
          for (i=0; i<COCO_MAX_PLAYERS; i++)
            if (coco_players[i].in_use)
              if (!coco_write_message( coco_streams + i, 
                                       (coco_msg_ptr) &talk ))
                return coco_handle_player_quit( i );
        }
        break;
      case COCO_MESSAGE_VERSION:
        {
          coco_bool compatible_p 
            = coco_version_compatible_p( message.version.major, 
                                         message.version.minor,
                                         COCO_NETWORK );
          fprintf( coco_server_log, "Client is running version %d.%d (%scompatible).\n",
                   message.version.major, message.version.minor,
                   compatible_p ? "" : "in");
          if (!compatible_p)
            return coco_handle_player_quit( player_index );
        }
        break;
      default: 
        coco_warn_printf( "fell through switch form in "
                          "coco_handle_server_message.\n" );
        break;
      }
    }
  {
    coco_bool ok = COCO_TRUE;
    if (ok && need_votestate_refresh)
      ok = coco_send_everyone_votestate_message( coco_get_sstate() );
    if (ok && need_game_refresh)
      ok = coco_send_everyone_game_message();
    return ok; 
  }
}




/* returns true iff server should be kept up. */
coco_bool
coco_send_everyone_message( coco_msg_ptr msg )
{
  coco_bool up = COCO_TRUE;
  int i;
  for (i=0;i<COCO_MAX_PLAYERS;i++)
    if (coco_players[i].in_use)
      if (!coco_write_message( coco_streams + i, msg ))
        up = up && coco_handle_player_quit( i );
  return up;
}


/* returns true iff server should be kept up. */
static coco_bool
coco_send_everyone_game_message( void )
{
  coco_bool up = COCO_TRUE;
  int i;
  for (i=0;i<COCO_MAX_PLAYERS;i++)
    if (coco_players[i].in_use)
      if (!coco_write_message( coco_streams + i, 
                               (coco_msg_ptr) coco_message_game( i ) ))
        up = up && coco_handle_player_quit( i );
  return up;
}

/* returns true iff server should be kept up */
coco_bool
coco_send_everyone_votestate_message( coco_sstate *sstate )
{
  int i;
  union coco_message msg;
  msg.every.type = COCO_MESSAGE_VOTESTATE;

  for (i=0; i<COCO_MAX_NOMINATIONS; i++)
    {
      int j;
      coco_u32 vec = 0;
      msg.votestate.nominations[i].action_code
        = sstate->nominations[i].action_code;
      msg.votestate.nominations[i].act_object
        = sstate->nominations[i].act_object;
      for (j=0; j<COCO_MAX_PLAYERS; j++)
        if (sstate->nominations[i].votes[j])
          vec += 1<<j;
      msg.votestate.nominations[i].votes = COCO_U32_HTON( vec );
    }
  return coco_send_everyone_message( &msg );
}

/* returns true iff server should be kept up */
coco_bool
coco_send_everyone_compflux_message( void )
{
  union coco_message msg;
  int i;
  msg.every.type = COCO_MESSAGE_COMPFLUX;
  for (i=0; i<COCO_MAX_COMPS; i++)
    if (coco_comps[i].in_use)
      {
        msg.compflux.comps[i].in_use = COCO_TRUE;
        msg.compflux.comps[i].name_index = coco_comps[i].name_index;
        msg.compflux.comps[i].nationality = coco_comps[i].nationality;
      }
    else msg.compflux.comps[i].in_use = COCO_FALSE;
  return coco_send_everyone_message( &msg );
}

/* returns true iff player should be kept connected */
coco_bool
coco_send_player_compflux_messages( int plr_idx )
{
  int i;
  union coco_message msg;
  msg.every.type = COCO_MESSAGE_COMPFLUX;
  for (i=0; i<COCO_MAX_COMPS; i++)
    if (coco_comps[i].in_use)
      {
        msg.compflux.comps[i].in_use = COCO_TRUE;
        msg.compflux.comps[i].name_index = coco_comps[i].name_index;
        msg.compflux.comps[i].nationality = coco_comps[i].nationality;
      }
    else msg.compflux.comps[i].in_use = COCO_FALSE;
  return coco_write_message( coco_streams + plr_idx, &msg );
}

/* returns true if obj1 died */
coco_bool
coco_sstate_execute_collision( coco_sstate *sstate, 
                               coco_sobject *obj1, coco_sobject *obj2 )
{
  coco_sobject *obja, *objb;
  coco_nationality nata, natb;
  coco_bool stat1, stat2;
  coco_bool *stata, *statb;

  if (COCO_SOBJECT_COLLISION_PRIORITY( obj1 ) 
      < COCO_SOBJECT_COLLISION_PRIORITY( obj2 ))
    {
      obja = obj1;
      objb = obj2;
      stata = &stat1;
      statb = &stat2;
    }
  else
    {
      obja = obj2;
      objb = obj1;
      stata = &stat2;
      statb = &stat1;
    }

  nata = COCO_SOBJECT_NATIONALITY( obja );
  natb = COCO_SOBJECT_NATIONALITY( objb );

  if ((nata != natb) || (nata == COCO_NEUTRAL))
    coco_hostile_collision( obja, COCO_SOBJECT_CL( obja ), objb,
                            sstate, stata, statb );
  else
    coco_friendly_collision( obja, COCO_SOBJECT_CL( obja ), objb,
                             sstate, stata, statb );
  return stat1;
}

void
coco_get_collision_recti( coco_sobject *sobject, coco_gnum *x0, coco_gnum *x1,
                          coco_gnum *y0, coco_gnum *y1 )
{
  coco_gnum x = COCO_SOBJECT_X( sobject );
  coco_gnum y = COCO_SOBJECT_Y( sobject );
  coco_mini_rectangle mr = COCO_SOBJECT_COLLISION_RECTANGLE( sobject );
  if (COCO_MINI_RECTANGLE_NULL_P( mr ))
    {
      *x0 = x;
      *y0 = y;
      *x1 = x + COCO_SOBJECT_W( sobject );
      *y1 = y + COCO_SOBJECT_H( sobject );
    }
  else if (COCO_SOBJECT_FACING( sobject ) > 0)
    {
      coco_gnum w = COCO_SOBJECT_W( sobject );
      coco_gnum h = COCO_SOBJECT_H( sobject );
      *x0 = x + w - COCO_MINI_RECTANGLE_X1( mr );
      *y0 = y + h - COCO_MINI_RECTANGLE_Y1( mr );
      *x1 = x + w - COCO_MINI_RECTANGLE_X0( mr );
      *y1 = y + h - COCO_MINI_RECTANGLE_Y0( mr );
    }
  else
    {
      *x0 = x + COCO_MINI_RECTANGLE_X0( mr );
      *y0 = y + COCO_MINI_RECTANGLE_Y0( mr );
      *x1 = x + COCO_MINI_RECTANGLE_X1( mr );
      *y1 = y + COCO_MINI_RECTANGLE_Y1( mr );
    }
}

void 
coco_sstate_find_collisions( coco_sstate *sstate )
{
  int i1, i2;
  assert( coco_allocate_sobject_may_resize_p );
  for (i1=0; i1 < sstate->sobject_size; i1++)
    if (COCO_SOBJECT_IN_USE( COCO_SSTATE_OBJECT_REF( sstate, i1 ) ))
      { 
        coco_sobject *obj1 = COCO_SSTATE_OBJECT_REF( sstate, i1 );
        coco_phase phase1 = COCO_SOBJECT_PHASE( obj1 );
        unsigned int index1 = COCO_SOBJECT_INDEX( obj1 );
        if ((COCO_VAPOR == phase1) || COCO_SOBJECT_NEW_FLAG( obj1 ))
          continue;
        for (i2=0; i2 < sstate->sobject_size; i2++)
          {
            coco_sobject *obj2 = COCO_SSTATE_OBJECT_REF( sstate, i2 );
            if ((!COCO_SOBJECT_IN_USE( obj2 ))
                || COCO_SOBJECT_NEW_FLAG( obj2 )
                || (index1 <= COCO_SOBJECT_INDEX( obj2 )))
              continue;
            {
              coco_gnum x0_1, x1_1, y0_1, y1_1, x0_2, x1_2, y0_2, y1_2;
              coco_phase phase2 = COCO_SOBJECT_PHASE( obj2 );
              if ((COCO_VAPOR == phase2)
                  || ((phase1 == COCO_EXPLOSIVE) && (phase2 == COCO_EXPLOSIVE))
                  /* || ((nat1 == nat2)&&(nat1!=COCO_NEUTRAL)) COLLISION OVERHAUL */
                  )
                continue;
              coco_get_collision_recti( obj1, &x0_1, &x1_1, &y0_1, &y1_1 );
              coco_get_collision_recti( obj2, &x0_2, &x1_2, &y0_2, &y1_2 );
              if ((((x0_1 <= x0_2) && (x0_2 <= x1_1))
                   || ((x0_1 <= x1_2) && (x1_2 <= x1_1))
                   || ((x0_2 <= x0_1) && (x0_1 <= x1_2))
                   || ((x0_2 <= x1_1) && (x1_1 <= x1_2)))
                  &&
                  (((y0_1 <= y0_2) && (y0_2 <= y1_1))
                   || ((y0_1 <= y1_2) && (y1_2 <= y1_1))
                   || ((y0_2 <= y0_1) && (y0_1 <= y1_2))
                   || ((y0_2 <= y1_1) && (y1_1 <= y1_2))))
                {
                  /* COLLISION OVERHAUL
                     if (((nat1==COCO_NEUTRAL)
                     && coco_sobject_subtype_p( COCO_SOBJECT_CL( obj1 ),
                     COCO_BUILDING_CLASS )))
                     coco_sobject_neutral_building_collide
                     ( obj2, COCO_SOBJECT_CL( obj2 ), obj1 );
                     else if (((nat2==COCO_NEUTRAL)
                     && coco_sobject_subtype_p( COCO_SOBJECT_CL( obj2 ),
                     COCO_BUILDING_CLASS )))
                     coco_sobject_neutral_building_collide
                     ( obj1, COCO_SOBJECT_CL( obj1 ), obj2 );
                     else 
                  */
                  if (coco_sstate_execute_collision( sstate, obj1, obj2 ))
                    break;
                  else obj1 = sstate->sobjects + i1; /* obj1 may have moved */
                }
            }
          }
      }
}

void
coco_sobject_set_raster_index( coco_sobject *sobject, unsigned char index )
{
  coco_gnum nx, ny, ox, oy;
  unsigned char ori = COCO_SOBJECT_RASTER_INDEX( sobject );
  unsigned char nri = index;
  struct coco_raster *raster;
#ifdef COCO_DEBUG
  assert( nri < COCO_SOBJECT_RASTER_COUNT( sobject ) );
#endif
  raster = COCO_SOBJECT_RASTER( sobject, nri );
  coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ),
                                ori, &ox, &oy );
  coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ),
                                nri, &nx, &ny );
  COCO_SOBJECT_X( sobject ) = COCO_SOBJECT_X( sobject ) - nx + ox;
  COCO_SOBJECT_Y( sobject ) = COCO_SOBJECT_Y( sobject ) - ny + oy;

  /* an ugly way of making copters different.  SHould split out to method,
     but too lazy right now.  Basically copters get their w/h from
     canonicals, not from the raster, wich is a texture map, and so 2^n *
     2^m, not fully cropped.  */
  if (!coco_sobject_subtype_p( COCO_SOBJECT_CL( sobject ), COCO_COPTER_CLASS ))
    {
      COCO_SOBJECT_W( sobject ) = raster->w;
      COCO_SOBJECT_H( sobject ) = raster->h;
    }
  COCO_SOBJECT_RASTER_INDEX( sobject ) = nri;
}

#define COCO_METAL_LUT_SIZE 5
void
coco_randomize_metal_shrapnel( coco_sobject *sobject )
{
  const static coco_color color_lut[COCO_METAL_LUT_SIZE]
    = { COCO_INDEX_RED, COCO_INDEX_BLUE, COCO_INDEX_GREEN,
        COCO_INDEX_LIGHT_BLUE, COCO_INDEX_BROWN };
  COCO_METAL_SHRAPNEL_COLOR_INDEX( sobject )
    = color_lut[rand() % COCO_METAL_LUT_SIZE];
  coco_sobject_set_raster_index( sobject, 
                                 rand() % COCO_SOBJECT_RASTER_COUNT( sobject ));
}
#undef COCO_METAL_LUT_SIZE



#define COCO_SHRAPNEL_TABLE_SIZE 4

void
coco_make_shrapnel( unsigned count, coco_sstate *sstate,
                    coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy )
{
  for (; count>0; count--)
    {
      static coco_sobject_cl *klasses[COCO_SHRAPNEL_TABLE_SIZE]
        = { COCO_SMALL_METAL_SHRAPNEL_CLASS,
            COCO_SMALL_METAL_SHRAPNEL_CLASS,
            COCO_MED_METAL_SHRAPNEL_CLASS,
            COCO_BIG_METAL_SHRAPNEL_CLASS 
        };
      coco_sobject *shrapnel = coco_sstate_allocate_sobject( sstate );
      coco_sobject_cl *klass = klasses[0]; /* [rand() % COCO_SHRAPNEL_TABLE_SIZE]; */
      coco_gnum sdx, sdy;
      if (NULL==shrapnel)
        return;
      sdx = dx + (rand()&30) - 15;
      sdy = dy + (rand()&30) - 15;
      coco_init_metal_shrapnel( shrapnel, klass, x, y, sdx, sdy );
      coco_randomize_metal_shrapnel( shrapnel );
    }
}

#undef COCO_SHRAPNEL_TABLE_SIZE

/* friction shift closer to zero means more drag.  4 is pretty draggy.  -1
   means don't do any drag. */
void
coco_free_fall_adjust( coco_sobject *sobject, coco_gnum ddy,
                       signed char friction_shift )
{
  coco_gnum dy_max = COCO_SOBJECT_DY_MAX( sobject );
  coco_gnum dx_max = COCO_SOBJECT_DX_MAX( sobject );
  coco_gnum dy_goal = COCO_SOBJECT_DY( sobject ) - abs( ddy );
  coco_gnum dx_goal = COCO_SOBJECT_DX( sobject );
  if (friction_shift >= 0)
    {
      dx_goal -= (dx_goal==-1) ? 0 : (dx_goal>>friction_shift);
      dy_goal -= dy_goal>>friction_shift;
    }
  COCO_SOBJECT_DY( sobject ) = COCO_CLAMP( -dy_max, dy_goal, dy_max );
  COCO_SOBJECT_DX( sobject ) = COCO_CLAMP( -dx_max, dx_goal, dx_max );
}

/* returns the balloon_line */
coco_sobject *
coco_hut_deploy_balloon( coco_sobject *hut, coco_sstate *sstate )
{
  coco_gnum x, y;
  coco_nationality nat;
  coco_sobject *line, *balloon;

  line = coco_sstate_allocate_sobject( sstate );
  balloon = coco_sstate_allocate_sobject( sstate );


  if (NULL == balloon) return NULL;
  y = COCO_GROUND_Y + COCO_SOBJECT_H( hut );
  x = COCO_SOBJECT_X( hut );
  nat = COCO_SOBJECT_NATIONALITY( hut );
  coco_init_balloon( balloon, COCO_BALLOON_CLASS, 
                     x + ((COCO_SOBJECT_W( hut )
                           - COCO_BALLOON_CLASS->canonical_w)
                          >>1), 
                     y+1, 
                     COCO_GOOD_GUYS, /* ri=gg for a second */
                     line ? nat : COCO_NEUTRAL );
  coco_sobject_set_raster_index( balloon, (unsigned char) nat );
  COCO_SOBJECT_DY( balloon ) = 1; /* get it rolling */
  COCO_BALLOON_FACTORY_FRESH( balloon ) = COCO_TRUE;
  if (NULL == line)
    {
      COCO_SOBJECT_NATIONALITY( balloon ) = COCO_NEUTRAL;
      return NULL;
    }
  /* coco_cookie( "Y = %d, line = %p, hut = %p\n", y, line, hut ); */
  COCO_BALLOON_LINE_INDEX( balloon ) = COCO_SOBJECT_INDEX( line );
  COCO_HUT_LINE_INDEX( hut ) = COCO_SOBJECT_INDEX( line );
  coco_init_balloon_line( line, COCO_BALLOON_LINE_CLASS, 
                          COCO_SOBJECT_X_CENTER( hut ), y, 1,
                          nat,
                          COCO_SOBJECT_INDEX( hut ),
                          COCO_SOBJECT_INDEX( balloon ) );
  /*
  coco_cookie( "hut %d's line idx = %d\n"
               "line %d's hut idx = %d, balloon_idx = %d\n"
               "balloon %d's line idx = %d\n",
               hut ? COCO_SOBJECT_INDEX( hut ) : -1,
               hut ? COCO_HUT_LINE_INDEX( hut ) : -1,
               line ? COCO_SOBJECT_INDEX( line ) : -1,
               line ? COCO_BALLOON_LINE_HUT_INDEX( line ) : -1,
               line ? COCO_BALLOON_LINE_BALLOON_INDEX( line ) : -1,
               balloon ? COCO_SOBJECT_INDEX( balloon ) : -1,
               balloon ? COCO_BALLOON_LINE_INDEX( balloon ) : -1 );
  */
  return line;
}


/* returns the hut. */
coco_sobject *
coco_make_hut_and_friends( coco_sstate *sstate, coco_gnum x, 
                           coco_nationality nationality )
{
  coco_sobject *hut;

  hut = coco_sstate_allocate_sobject( sstate );
  if (NULL == hut) return NULL;
  coco_init_hut( hut, COCO_HUT_CLASS, x, nationality );
  coco_hut_deploy_balloon( hut, sstate );
  return hut;
}

coco_sobject *
coco_hut_init_by_list( coco_sobject *sobject, coco_sobject_cl *cla, 
                       coco_lnum *args )
{
  coco_init_hut( sobject, COCO_HUT_CLASS, args[0], args[1] );
  coco_hut_deploy_balloon( sobject, coco_get_sstate() );
  return sobject;
}


coco_sobject *
coco_emit_smoke_helper( coco_sobject *sobject, coco_sstate *sstate, 
                        coco_gnum x, coco_gnum y )
{
  coco_sobject *smoke;
  struct coco_raster *smoke_raster;
  int turn_mod_6;
  int health = COCO_SOBJECT_HEALTH( sobject );
  int health_max = COCO_SOBJECT_HEALTH_MAX( sobject );
  int smokelevel =  health_max - health;
  if (health==health_max) return NULL;
  turn_mod_6 = sstate->time % 6;
  if (smokelevel > COCO_SMOKE_RASTER_COUNT)
    {
      if (turn_mod_6 == 4)
        {
          smokelevel -= COCO_SMOKE_RASTER_COUNT;
          if (smokelevel > COCO_SMOKE_RASTER_COUNT)
            {
              coco_warn_printf( "too much damage not enough smoke rasters\n" );
              smokelevel = COCO_SMOKE_RASTER_COUNT;
            }
        }
      else if (turn_mod_6 == 0)
        smokelevel = COCO_SMOKE_RASTER_COUNT;
      else smokelevel = 0;
    }
  else if (turn_mod_6 != 0)
    smokelevel = 0;
  if (smokelevel==0) return NULL;
  smoke = coco_sstate_allocate_sobject( sstate );
  if (!smoke) return NULL;
  assert( smokelevel - 1 >=0 );
  assert( smokelevel - 1 < COCO_SMOKE_CLASS->raster_count );
  smoke_raster = COCO_SMOKE_CLASS->rasters[smokelevel - 1];
  coco_init_smoke( smoke, COCO_SMOKE_CLASS, x, y, smokelevel - 1 );
  COCO_SOBJECT_X( smoke ) -= smoke_raster->w>>1;
  COCO_SOBJECT_Y( smoke ) -= smoke_raster->h>>1;
  return smoke;
}




void
coco_summon_places( coco_sobject *sobject_or_null, 
                    coco_sstate *sstate,
                    coco_nationality nationality,
                    coco_bool **summon_enable, 
                    coco_u32 **summon_disable_until )
{
  if (sobject_or_null)
    coco_sobject_summon_places( sobject_or_null, 
                                COCO_SOBJECT_CL( sobject_or_null ),
                                sstate, summon_enable, summon_disable_until );
  else
    {
      struct coco_team *team = sstate->teams + nationality - COCO_GOOD_GUYS;
      *summon_enable = & (team->summon_enable);
      *summon_disable_until = & (team->summon_disable_until);
    }
}



/* return true if something was summoned */
coco_bool
coco_summon_unit_maybe( coco_sstate *sstate, coco_sobject_cl *cl, 
                        coco_sobject *(* init_fn)
                        ( coco_sobject *, coco_sobject_cl *, 
                          coco_gnum, coco_nationality),
                        coco_nationality nationality,
                        int p_or_c_index,
                        coco_bool is_comp,
                        coco_sobject *summoner /* possibly NULL */
                        )
{
  coco_bool *summon_enable;
  coco_u32 *summon_disable_until;
  coco_summon_places( summoner, sstate, nationality, &summon_enable,
                      &summon_disable_until );
  if (*summon_enable 
      && coco_subtract_funds_available( p_or_c_index, is_comp, cl->cost ))
    {
      coco_sobject *unit = coco_sstate_allocate_sobject( sstate );
      if (unit)
        {
          coco_u32 delta_t;
          coco_gnum cw = cl->canonical_w;
          coco_bool gg = (COCO_GOOD_GUYS==nationality);
          coco_gnum x;
          if (summoner
              && coco_sobject_subtype_p( COCO_SOBJECT_CL( summoner ),
                                         COCO_CARGO_COPTER_CLASS )
              && COCO_COPTER_LANDED_P( summoner )
              && !COCO_COPTER_LANDED_ON_PAD_P( summoner ))
            {
              coco_gnum x0, y0, x1, y1;
              coco_get_collision_recti( summoner, &x0, &x1, &y0, &y1 );
              
              if (coco_sobject_subtype_p( cl, COCO_INFANTRY_CLASS ))
                /* infantry have to go in front or get sucked in as
                   passengers */
                x = gg  
                  ? x0 - cw - 1
                  : x1 + 1;
              else /* armor come out of the belly */
                x = gg
                  ? x0
                  : x1 - cw;
            }
          else
            x = gg
              ? (sstate->board_w + COCO_GROUND_UNIT_WARP_IN_X)
              : (-COCO_GROUND_UNIT_WARP_IN_X - cw);
          init_fn( unit, cl, x, nationality );
          delta_t = COCO_GROUND_UNIT_DX_MAX * (COCO_SOBJECT_W( unit ) + 5);
          *summon_enable = COCO_FALSE;
          *summon_disable_until
            = COCO_U32_ADD_N_MODULAR( sstate->time, delta_t );
          coco_make_snd( COCO_SOUND_SUMMON_POP, sstate, x, COCO_GROUND_Y );
          return COCO_TRUE;
        }
      else
        { /* refund the purchase price */
          coco_add_funds( p_or_c_index, is_comp, cl->cost );
          return COCO_FALSE;
        }
    }
  return COCO_FALSE;
}

/* coco_init_infantry( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index, coco_nationality nationality )
 */

coco_sobject *
coco_init_infantry_from_ground( coco_sobject *inst, coco_sobject_cl *cla, 
                                coco_gnum x, coco_nationality nationality )
{
  int foot_phase = rand() % COCO_INFANTRY_WALK_COUNT;
  coco_init_infantry( inst, cla, x, COCO_GROUND_Y, 0, 0, 
                      foot_phase
                      + ((nationality == COCO_GOOD_GUYS) 
                         ? COCO_INFANTRY_WALK_WEST_BASE
                         : COCO_INFANTRY_WALK_EAST_BASE),
                      nationality );
  return inst;
}

coco_sobject *
coco_infantry_init_from_ground_by_list( coco_sobject *inst,
                                        coco_sobject_cl *cla,
                                        coco_lnum *arguments )
{
  return coco_init_infantry_from_ground( inst, cla, arguments[0], arguments[1] );
}



struct coco_unit_order_keymap { 
  coco_key key;
  coco_sobject_cl *cl;
  coco_sobject *(* init_fn)
       ( coco_sobject *, coco_sobject_cl *, coco_gnum, coco_nationality);
} ;


#define COCO_UNIT_ORDER_KEYMAP_LENGTH 5

coco_bool 
coco_camera_update_state( coco_sobject *sobject, 
                          coco_sobject_cl *cl, 
                          coco_sstate *sstate )
{
  const static struct coco_unit_order_keymap 
    bindings[COCO_UNIT_ORDER_KEYMAP_LENGTH] 
    = {{ COCO_CHAR_TO_KEY( 'v' ), COCO_DEMO_VAN_CLASS, &coco_init_demo_van },
       { COCO_CHAR_TO_KEY( 't' ), COCO_TANK_CLASS, &coco_init_tank },
       { COCO_CHAR_TO_KEY( 'a' ), COCO_FIRETRUCK_CLASS, &coco_init_firetruck },
       { COCO_CHAR_TO_KEY( 'm' ), COCO_INFANTRY_CLASS,
         &coco_init_infantry_from_ground },
       { COCO_CHAR_TO_KEY( 'e' ), COCO_ENGINEER_CLASS,
         &coco_init_infantry_from_ground }};
  int cam_idx = COCO_SOBJECT_CAMERA_INDEX( sobject );
  int comp_idx = COCO_SOBJECT_COMP_INDEX( sobject );
  coco_bool is_comp = (comp_idx >= 0);
  struct coco_keyboard_pointer_state *input 
    = is_comp
    ? &(coco_comps[comp_idx].kpstate)
    : (coco_remote_input_states + cam_idx);
  int i;
  if ((is_comp || (cam_idx >= 0)))
    {
      for (i=0; i<COCO_UNIT_ORDER_KEYMAP_LENGTH; i++)
        if (coco_get_bit( input->key_state, bindings[i].key ))
          coco_summon_unit_maybe( sstate, bindings[i].cl, bindings[i].init_fn,
                                  COCO_SOBJECT_NATIONALITY( sobject ),
                                  is_comp ? comp_idx : cam_idx, is_comp,
                                  sobject );
    }
  else coco_warn_printf( "%s %d is not comp nor plr\n",
                         COCO_SOBJECT_NAME( sobject ),
                         COCO_SOBJECT_INDEX( sobject ) );

  { /* flying-player-name */
    coco_sobject *fpn;
    fpn = coco_sstate_allocate_sobject( sstate );
    if (fpn)
      {
        int p_or_c_index = is_comp
          ? COCO_SOBJECT_COMP_INDEX( sobject )
          : COCO_SOBJECT_CAMERA_INDEX( sobject );
        coco_nationality nat = COCO_SOBJECT_NATIONALITY( sobject );
        if ((nat != COCO_GOOD_GUYS)&&(nat != COCO_BAD_GUYS))
          {
            coco_warn_printf( "camera has unknown nationality %d, "
                              "its flying_player_name will be COCO_BAD_GUYS\n",
                              nat );
            nat = COCO_BAD_GUYS;
          }

        assert( p_or_c_index >= 0 );

        coco_init_flying_player_name( fpn, COCO_FLYING_PLAYER_NAME_CLASS,
                                      COCO_SOBJECT_X( sobject ),
                                      COCO_SOBJECT_Y( sobject ),
                                      nat,
                                      is_comp, p_or_c_index );
      }
  }

  /* call-next-method */
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

#undef COCO_UNIT_ORDER_KEYMAP_LENGTH

void
coco_copter_drop_trooper_on_ground_maybe( coco_sobject *sobject,
                                          coco_sstate *sstate )

{
  unsigned char troopers = COCO_COPTER_PARATROOPERS( sobject );
  unsigned char engineers = COCO_COPTER_ENGITROOPERS( sobject );
  if ((troopers > 0) || (engineers > 0))
    {
      coco_sobject *inf = coco_sstate_allocate_sobject( sstate );
      if (inf)
        {
          coco_nationality nat = COCO_SOBJECT_NATIONALITY( sobject );
          coco_gnum x0, x1, y;
          if (troopers > 0)
            COCO_COPTER_PARATROOPERS( sobject ) = troopers - 1;
          else
            COCO_COPTER_ENGITROOPERS( sobject ) = engineers - 1;
          coco_get_collision_recti( sobject, &x0, &x1, &y, &y );
          coco_init_infantry_from_ground
            ( inf, 
              (troopers > 0) ? COCO_INFANTRY_CLASS : COCO_ENGINEER_CLASS,
              (nat == COCO_GOOD_GUYS) 
              ? (x0 - COCO_INFANTRY_CLASS->canonical_w - 1) 
              : (x1 + 1),
              nat );
        }
    }
}

coco_sobject *
coco_copter_lock_on( coco_sobject *copter, coco_sstate *sstate )
{
  /* Search for closest target with a piece of its raster at the altitude
     of the copters "tar-get-ing la-ser", which is about 2 pixels from
     the lowest point in the copter's collision rectangle. */

  coco_nationality nat = COCO_SOBJECT_NATIONALITY( copter );
  coco_facing facing = COCO_SOBJECT_FACING( copter );
  coco_gnum laser_y, x0, x1, dummy, laser_x;
  coco_gnum lock_on_dx = COCO_GNUM_MAX;
  coco_sobject *lock_on = NULL;

  coco_get_collision_recti( copter, &x0, &x1, &laser_y, &dummy );
  laser_y += 2;
  laser_x = ((x0 + x1) >> 1);
  COCO_DO_SSTATE_OBJECTS( target, sstate, {
    if ((COCO_SOBJECT_PHASE( target ) == COCO_SOLID)
        && (nat != COCO_SOBJECT_NATIONALITY( target )))
      {
        coco_gnum tx = COCO_SOBJECT_X( target );
        if (((facing>0) && (tx>laser_x)) || ((facing<0) && (tx<laser_x)))
          {
            coco_gnum ty0 = COCO_SOBJECT_Y( target );
            if (ty0 <= laser_y)
              {
                coco_gnum ty1 = ty0 + COCO_SOBJECT_H( target );
                if ((ty1 >= laser_y)
                    || ((laser_y < COCO_BALLOON_FLOOR)  /* find aa guns */
                        && (ty1 < COCO_BALLOON_FLOOR))) /* find aa guns */
                  {
                    coco_gnum dx = ( abs(tx
                                         + (COCO_SOBJECT_W( target ) >> 1) 
                                         - laser_x ) );
                    if ((dx < lock_on_dx) 
                        && !(coco_sobject_subtype_p( COCO_SOBJECT_CL( target ),
                                                     COCO_MUNITION_CLASS )))
                      {
                        lock_on_dx = dx;
                        lock_on = target;
                      }
                  }
              }
          }
      }
  });
  return lock_on;
}

coco_sobject *
coco_button_maybe_launch_missile( coco_sobject *copter, coco_sstate *sstate )
{
  if ((COCO_COPTER_MISSILES( copter ) <= 0)
      || COCO_COPTER_MISSILE_COOLDOWN( copter ))
    return NULL;
  else
    {
      coco_sobject *target = coco_copter_lock_on( copter, sstate );
      if (target)
        return coco_launch_missile( copter, COCO_SOBJECT_CL( copter ),
                                   sstate, target );
      else return NULL;
    }
}


coco_sobject *
coco_cargo_copter_maybe_use_kit( coco_sobject *sobject, 
                                 coco_sstate *sstate,
                                 coco_sobject_cl *kit_class )
{
  coco_gnum kits = 0;
  coco_sobject *gun = NULL;
  if (COCO_COPTER_LANDED_P( sobject ) 
      && (!COCO_COPTER_LANDED_ON_PAD_P( sobject ))
      && (kits = COCO_COPTER_KITS( sobject ))
      && (gun  = coco_sstate_allocate_sobject( sstate )))
    {
      coco_gnum x = COCO_MINI_POINT_X( coco_sobject_com( sobject ) )
        + COCO_SOBJECT_X( sobject )
        - (kit_class->canonical_w>>1);
      if (kit_class == COCO_GUN_RUBBLE_CLASS)
        coco_init_gun_rubble( gun, kit_class, x,
                              COCO_AA_GUN_CLASS_INDEX );
      else if (kit_class == COCO_HUT_CLASS)
        coco_init_hut( gun, kit_class, x,
                       COCO_SOBJECT_NATIONALITY( sobject ) );
      else
        coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                           "Bad class index to "
                           "coco_cargo_copter_maybe_use_kit: \"%s\" %d\n",
                           kit_class->name, kit_class->cl_index );
      COCO_COPTER_KITS( sobject ) = kits - 1;
      return gun;
    }
  else 
    return NULL;
}



coco_bool 
coco_copter_update_state( coco_sobject *sobject, 
                          coco_sobject_cl *cl, 
                          coco_sstate *sstate )
{
  coco_bool fired_bullet = COCO_FALSE;
  signed char camera_index = COCO_SOBJECT_CAMERA_INDEX( sobject );
  signed char comp_index = COCO_SOBJECT_COMP_INDEX( sobject );
  struct coco_keyboard_pointer_state *input;

  if (COCO_COPTER_MISSILE_COOLDOWN( sobject )
      && (sstate->time == COCO_COPTER_MISSILE_COOLDOWN_TIME( sobject )))
    COCO_COPTER_MISSILE_COOLDOWN( sobject ) = COCO_FALSE;

  if (camera_index>=0)
    input = coco_remote_input_states + camera_index;
  else if ((comp_index>=0)
           &&(comp_index<COCO_MAX_COMPS)
           &&coco_comps[comp_index].in_use)
    input = &(coco_comps[comp_index].kpstate);
  else 
    {
      coco_warn_printf( "Copter %d neither player nor comp\n", 
                        COCO_SOBJECT_INDEX( sobject ) );
      goto get_out;
    }
  /* have to put weapons release before the x/y/dx/dy etc
     tweakers, because dy may be positive, y may be at the top of
     the screen, and they may drop a bomb.  Clip-to-screen will
     later move our dy back to 0, but in the meantime we'll have
     released a bomb with a positive dy at the top of the screen.
     This looks silly.  */
  if (!COCO_COPTER_LANDED_P( sobject ))
    {
      if (coco_get_bit( input->key_state, COCO_MPOINTER )
          || coco_get_bit( input->key_state, COCO_KEY_SHIFT ))
        {
          fired_bullet = 
            !!coco_fire_bullet( sobject, 
                                COCO_SOBJECT_CL( sobject ), sstate, NULL );
        }
      if (coco_get_bit( input->key_state, COCO_RPOINTER )
          || coco_get_bit( input->key_state, COCO_KEY_TO_CHAR( 'z' ) ))
        coco_drop_bomb( sobject, COCO_SOBJECT_CL( sobject ), sstate );
      if (coco_get_bit( input->key_state, COCO_KEY_TO_CHAR( 'c' ) ))
        coco_launch_rocket( sobject, COCO_SOBJECT_CL( sobject ), sstate );
      if (coco_get_bit( input->key_state, COCO_KEY_SPACE ))
        coco_drop_paratrooper( sobject, COCO_SOBJECT_CL( sobject ),
                               sstate );
      if (coco_get_bit( input->key_state, COCO_CHAR_TO_KEY( 'x' ) ))
        coco_button_maybe_launch_missile( sobject, sstate );
    }
  else if (coco_sobject_subtype_p( COCO_SOBJECT_CL( sobject ),
                                      COCO_CARGO_COPTER_CLASS ))
    {
      if (coco_get_bit( input->key_state, COCO_CHAR_TO_KEY( 'x' ) ))
        coco_cargo_copter_maybe_use_kit( sobject, sstate, 
                                         COCO_GUN_RUBBLE_CLASS );
      if (coco_get_bit( input->key_state, COCO_CHAR_TO_KEY( 'c' ) ))
        coco_cargo_copter_maybe_use_kit( sobject, sstate,
                                         COCO_HUT_CLASS );
    }
  if (!fired_bullet)
    COCO_COPTER_GUN_COOLDOWN( sobject ) = COCO_FALSE;
  {
    coco_bool landed_p = COCO_COPTER_LANDED_P( sobject );
    int ri = COCO_SOBJECT_RASTER_INDEX( sobject );

    /* y axis */
    coco_gnum dy_max = COCO_SOBJECT_DY_MAX( sobject );
    coco_gnum y_goal = (COCO_COPTER_FUEL( sobject )>0)
      ? COCO_CLAMP( 0, input->pointer_y, COCO_VIEWABLE_H )
      : 0;
    coco_gnum dy_goal = (y_goal - COCO_SOBJECT_Y( sobject )) >> 2;
    coco_gnum mdy_max = landed_p ? 0 : -dy_max;
    coco_gnum dy_final = COCO_CLAMP( mdy_max, dy_goal, dy_max );

    /* angle */
    coco_i32 px = COCO_CLAMP( -COCO_OFFICIAL_WINDOW_W>>1, 
                              input->pointer_x,
                              COCO_OFFICIAL_WINDOW_W>>1 );
    coco_angle rest_angle = (ri&1) ? 0 : 128;
    float angle_goal = rest_angle - (((float) px) 
                                     * COCO_COPTER_MAX_ANGLE_OF_ATTACK 
                                     / (COCO_OFFICIAL_WINDOW_W>>1)); 
    coco_angle angle_final 
      = landed_p ? rest_angle : ((int) angle_goal)&255;

    /* x axis */
    coco_facing facing = COCO_SOBJECT_FACING( sobject );
    coco_gnum dx = COCO_SOBJECT_DX( sobject );
    coco_gnum dx_max = COCO_SOBJECT_DX_MAX( sobject );
    coco_gnum ddx_raw = COCO_SOBJECT_X_FORCE( sobject );
    coco_gnum dx_goal
      = (coco_gnum) (((float) dx_max) * px
                     / ((float) (COCO_OFFICIAL_WINDOW_W>>1)));
    /* accell takes 2x as long as decel,
       takes 4x as long if facing wrong way, or near top speed */
    coco_gnum ddx_vectored = ((dx_goal <= 0) == (dx <= 0))
      ? COCO_MAX( (((facing<=0)==(dx_goal <= 0))
                   && (abs( dx ) < (dx_max>>1))) ? (ddx_raw>>1) : (ddx_raw>>2),
                  1 )
      : ddx_raw;
    coco_gnum dx_unclipped = (dx_goal <= dx) 
      ? COCO_MAX( (dx - ddx_vectored), dx_goal )
      : COCO_MIN( (dx + ddx_vectored), dx_goal );
    coco_gnum mdx_max = -dx_max;
    coco_gnum dx_final = landed_p ? 0 
      : COCO_CLAMP( mdx_max, dx_unclipped, dx_max );
    /* this new code for x axis just feels wrong
       coco_gnum dx = COCO_SOBJECT_DX( sobject );
       coco_gnum dx_max = COCO_SOBJECT_DX_MAX( sobject );
       coco_gnum engine_force = COCO_SOBJECT_X_FORCE( sobject );
       coco_gnum drag_force = dx>>5;
       coco_gnum foo = COCO_CLAMP( -50, (int) pa_final, 50 );
       coco_gnum ddx = ((((int) pa_final)) * engine_force / (127))
       - drag_force;
       coco_gnum dx_unclamped = dx + ddx;
       coco_gnum dx_final = COCO_CLAMP( -dx_max, dx_unclamped, dx_max );
    */
    /* landed */
    if ((COCO_SOBJECT_Y( sobject ) == COCO_GROUND_Y)
        && (dy_final <= 0)
        && (angle_final <= (rest_angle
                            + COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK))
        && (angle_final >= (rest_angle
                            - COCO_COPTER_MAX_LANDING_ANGLE_OF_ATTACK))
        && (COCO_COPTER_FUEL( sobject ) > 0))
      {
        COCO_COPTER_LANDED_ON_PAD_P( sobject ) = COCO_FALSE;
        landed_p = COCO_TRUE;
        dx_final = dy_final = 0;
      }
    else if (dy_final > 0) 
      landed_p = COCO_FALSE;
    /* note how if we're on pad, y>COCO_GROUND_Y, then we might just
       stay landed_p */
      
    /* begin non-local side-effects */
    COCO_SOBJECT_DX( sobject ) = dx_final;
    COCO_SOBJECT_DY( sobject ) = dy_final;
    COCO_SOBJECT_ANGLE( sobject ) = angle_final;
    COCO_COPTER_LANDED_P( sobject ) = landed_p;
    if (landed_p)
      {
        if (COCO_COPTER_LANDED_ON_PAD_P( sobject ))
          coco_copter_pad_update( sobject, COCO_SOBJECT_CL( sobject ),
                                  sstate );
        else if (coco_get_bit( input->key_state, COCO_KEY_SPACE ))
          coco_copter_drop_trooper_on_ground_maybe( sobject, sstate );
      }
    else
      COCO_COPTER_FUEL( sobject ) 
        = COCO_MAX( 1, COCO_COPTER_FUEL( sobject ) ) - 1;
  }
 get_out:
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

void
coco_copter_start_coords( coco_sstate *sstate, coco_nationality nat,
                          int player_index,
                          coco_gnum *x_return, coco_gnum *y_return )
{
  coco_sobject *pad
    = COCO_SSTATE_OBJECT_REF
    ( sstate,
      sstate->teams[((int)nat)-COCO_GOOD_GUYS].landing_pad_index );
  coco_sobject_cl *cla = (player_index>=0) 
    ? (sstate->favorite_copter_class[player_index])
    : COCO_COMP_FAVORITE_COPTER_CLASS;
  coco_mini_rectangle mr = cla->collision_rectangle;
  if (x_return)
    *x_return = COCO_SOBJECT_X( pad )
      - (((COCO_MINI_RECTANGLE_X1( mr ) - COCO_MINI_RECTANGLE_X0( mr ))
          - COCO_SOBJECT_W( pad ))
         >>1)
      - ((COCO_BAD_GUYS==nat) ? 15 : 0); /* Quick dirty hack */
  if (y_return)
    *y_return = COCO_SOBJECT_Y1( pad );
}

/* will become a generic function if something other than mech fires
   plasma. */
coco_sobject *
coco_fire_plasma( coco_sobject *sobject, coco_sobject_cl *class,
                  coco_sstate *sstate )
{
  coco_u16 e;
  unsigned short plasmas;
  coco_u32 time = sstate->time;
  coco_u32 last_time;

#ifdef COCO_DEBUG
  assert( coco_sobject_subtype_p( class, COCO_MECH_CLASS ) );
#endif

  plasmas = COCO_MECH_PLASMA( sobject );
  e = COCO_MECH_ENERGY( sobject );
  last_time = COCO_MECH_LAST_PLASMA_FIRE_TIME( sobject );
  if ((e >= COCO_PLASMA_ENERGY)
      && (plasmas > 0)
      && (COCO_U32_ADD_MODULAR( last_time ) != time)
      && (COCO_U32_ADD_N_MODULAR( last_time, 2 ) != time))
    {
      coco_gnum x, y;
      coco_sobject_weapon_hardpoint_coords( sobject, &x, &y );
      x -= COCO_PLASMA_CLASS->canonical_w>>1;
      y -= COCO_PLASMA_CLASS->canonical_h>>1;
      {
        coco_sobject *plasma;
        coco_gnum angle = COCO_SOBJECT_ANGLE( sobject );
        coco_gnum dx = (COCO_COS( angle ) * COCO_PLASMA_SPEED)
          + (COCO_SOBJECT_DX( sobject )>>1);
        coco_gnum dy = (COCO_SIN( angle ) * COCO_PLASMA_SPEED)
          + (COCO_SOBJECT_DY( sobject )>>1);

        plasma = coco_sstate_allocate_sobject( sstate );
        if (!plasma) return NULL;

        COCO_MECH_ENERGY( sobject ) = e - COCO_PLASMA_ENERGY;
        COCO_MECH_PLASMA( sobject ) = plasmas - 1;
        coco_init_plasma( plasma, COCO_PLASMA_CLASS, x, y, dx, dy,
                          COCO_SOBJECT_NATIONALITY( sobject ) );
        COCO_MECH_LAST_PLASMA_FIRE_TIME( sobject ) = sstate->time;
        coco_make_snd( COCO_SOUND_PLASMA_CANNON, sstate, x, y );
        return plasma;
      }
    }
  else return NULL;
}

coco_bool
coco_power_shield( coco_sobject *sobject, coco_sobject_cl *class,
                   coco_sstate *sstate )
{
  return COCO_FALSE;
}



#define COCO_LEG_RANGE( ri, lo, hi ) ((((ri)>=(lo))&&((ri)<(hi))) \
                                      ||(((ri)>=((lo)+COCO_LEFTY_LEG_PART)) \
                                         &&((ri)<((hi)+COCO_LEFTY_LEG_PART))))

/* Only used when its important to filter out typeomatic key presses */
#define COCO_MECH_PRESSED( key ) (coco_get_bit( input->key_state, (key) )||coco_get_bit( old_input->key_state, (key) ))

void
coco_mech_pad_update( coco_sobject *sobject, coco_sobject_cl *cl, 
                      coco_sstate *sstate )
{
  coco_u32 time = sstate->time;
  coco_health health = COCO_SOBJECT_HEALTH( sobject );
  int plasma = COCO_MECH_PLASMA( sobject );
  if (((time & COCO_PAD_HEAL_MASK) == 0)
      && (health < COCO_SOBJECT_HEALTH_MAX( sobject )))
    COCO_SOBJECT_HEALTH( sobject ) = health + 1;
  if (plasma < COCO_SOBJECT_MAX_PLASMA( sobject ))
    COCO_MECH_PLASMA( sobject ) = plasma + 1;
}


coco_sobject *
coco_make_mech_jet_exhaust( coco_sobject *mech, coco_sstate *sstate, 
                            int jstrength, coco_facing x )
{
  coco_sobject *jet = coco_sstate_allocate_sobject( sstate );
  if (jet)
    {
      coco_gnum mw, rw;
      coco_gnum jx, jy;
      int ri;
      struct coco_raster *r;

      if (jstrength==2)
        if (0==x)
          ri = 6;
        else if (x<0)
          ri = 5;
        else 
          ri = 7;
      else if (jstrength==1)
        if (0==x)
          ri = 2;
        else if (x<0)
          ri = 1;
        else 
          ri = 3;
      else if (x<0)
        ri = 0;
      else
        ri = 4;

      ri += COCO_EXPLOSION_JET_EXHAUST_BASE;
      r = coco_explosion_rasters[ri];
      mw = COCO_SOBJECT_W( mech );
      rw = r->w;

      if (jstrength && (0==x))
        jx = ((mw - rw)>>1);
      else if (x<0)
        jx = mw;
      else
        jx = - rw;

      jx += COCO_SOBJECT_X( mech ) + COCO_SOBJECT_DX( mech );
      jy =  COCO_SOBJECT_Y( mech ) + COCO_SOBJECT_DY( mech ) - r->h;

      coco_init_explosion( jet, COCO_EXPLOSION_CLASS, 0,
                           jx, jy, 0, 0, ri );
      return jet;
    }
  else return NULL;
}


coco_bool 
coco_mech_update_state( coco_sobject *sobject, 
                        coco_sobject_cl *cl, 
                        coco_sstate *sstate )
{
  coco_bool butleft, butright, butup;
  signed char camera_index = COCO_SOBJECT_CAMERA_INDEX( sobject );
  signed char comp_index = COCO_SOBJECT_COMP_INDEX( sobject );
  struct coco_keyboard_pointer_state *input;
  coco_facing run;
  coco_bool landed_p = COCO_MECH_LANDED_ON_PAD_P( sobject );
  coco_bool jump = COCO_FALSE;
  int jumped_strength = 0;
  coco_bool exhaust_p = COCO_FALSE;
  int legs = COCO_MECH_LEG_RASTER_INDEX( sobject );
  int lefty = (legs >= COCO_LEFTY_LEG_PART) ? COCO_LEFTY_LEG_PART : 0;
  int legs_out = legs;
  enum { COCO_RUN, COCO_IDLE, COCO_FLY, COCO_LAND } leg_action;
  if (COCO_COPTER_MISSILE_COOLDOWN( sobject )
      && (sstate->time == COCO_COPTER_MISSILE_COOLDOWN_TIME( sobject )))
    COCO_COPTER_MISSILE_COOLDOWN( sobject ) = COCO_FALSE;

  if (camera_index>=0)
    {
      input = coco_remote_input_states + camera_index;
    }
  else if ((comp_index>=0)
           &&(comp_index<COCO_MAX_COMPS)
           &&coco_comps[comp_index].in_use)
    input = &(coco_comps[comp_index].kpstate);
  else 
    {
      coco_warn_printf( "Mech %d neither player nor comp\n", 
                        COCO_SOBJECT_INDEX( sobject ) );
      goto get_out;
    }
  /* Above here ripped off from copter's method */

  {
    coco_u16 e = COCO_MECH_ENERGY( sobject );
    COCO_MECH_ENERGY( sobject ) 
      = COCO_MIN( e + COCO_MECH_POWER, COCO_MECH_CAPACITANCE );
  }
  
  { /* calculate angle before weapons release, so they've got fresh aim */
    coco_angle a;
    coco_gnum xmid, ymid, sx;
    coco_center_of_mass_obsolete( sobject, COCO_SOBJECT_CL( sobject ),
                                  COCO_SOBJECT_RASTER_INDEX( sobject ),
                                  &xmid, &ymid );
    /* this x calculation is duplicated sorta in copter_kaboom, for
       camera placement. */
    /* also kinda in mech_update_state */
    /* sx = screen_x of mech center_of_mass */
    sx = xmid
      - coco_camera_velocity_offset( sobject, COCO_SOBJECT_CL( sobject ),
                                     sstate );

    a = coco_atan2( input->pointer_y - (COCO_SOBJECT_Y( sobject ) + ymid),
                    input->pointer_x - sx );
    COCO_SOBJECT_ANGLE( sobject ) = a;
    /* after some thought this doesn't look right for a=10, say.  I blame
       the rasters, not the math.-dm 05-27-2001 */
    COCO_MECH_TORSO_RASTER_INDEX( sobject )
      = floor( ((a + 128.0/COCO_MAX_TORSO_PARTS) * COCO_MAX_TORSO_PARTS) 
               / 256 );
  }

  {
    coco_bit_vec_ref ib = COCO_MECH_INPUT_BUFFERS( sobject );
    int max = COCO_MECH_BUFFERED_BUTTONS_HACK * COCO_MECH_BUFFERED_TURNS_HACK;
    int i = max;
    /* shift old buffer back in time */
    for (;i>=COCO_MECH_BUFFERED_BUTTONS_HACK;i--)
      coco_set_bit( ib, i, 
                    coco_get_bit( ib, i-COCO_MECH_BUFFERED_BUTTONS_HACK ) );
    coco_set_bit( ib, 0, 
                  coco_get_bit( input->key_state, COCO_CHAR_TO_KEY( 'z' ) )
                  || coco_get_bit( input->key_state, COCO_KEY_LEFT_ARROW ) );
    coco_set_bit( ib, 1, 
                  coco_get_bit( input->key_state, COCO_CHAR_TO_KEY( 'x' ) )
                  || coco_get_bit( input->key_state, COCO_KEY_UP_ARROW ) );
    coco_set_bit( ib, 2, 
                  coco_get_bit( input->key_state, COCO_CHAR_TO_KEY( 'c' ) )
                  || coco_get_bit( input->key_state, COCO_KEY_RIGHT_ARROW ) );
#if (3 != COCO_MECH_BUFFERED_BUTTONS_HACK)
#error buffer the other buttons here
#endif
    butleft = butright = butup = COCO_FALSE;
    for (i=0; i<max; i+=COCO_MECH_BUFFERED_BUTTONS_HACK)
      if (coco_get_bit( ib, i )) { butleft = COCO_TRUE; break; }
    for (i=1; i<max; i+=COCO_MECH_BUFFERED_BUTTONS_HACK)
      if (coco_get_bit( ib, i )) { butup = COCO_TRUE; break; }
    for (i=2; i<max; i+=COCO_MECH_BUFFERED_BUTTONS_HACK)
      if (coco_get_bit( ib, i )) { butright = COCO_TRUE; break; }
  }



  if (coco_get_bit( input->key_state, COCO_LPOINTER ))
    coco_fire_plasma( sobject, COCO_SOBJECT_CL( sobject ), sstate );
  if (coco_get_bit( input->key_state, COCO_MPOINTER )
      || coco_get_bit( input->key_state, COCO_KEY_SHIFT ))
    {
      coco_warn_printf( "mech missile launch not implemented" );
      /* coco_launch_missile( sobject, COCO_SOBJECT_CL( sobject ), sstate, ? ); */
    }
  if (coco_get_bit( input->key_state, COCO_RPOINTER ))
    coco_power_shield( sobject, COCO_SOBJECT_CL( sobject ), sstate );
  run = butright - butleft;
  jump = butup;

  /* figure out what state we're in */
  if (COCO_LEG_RANGE( legs, COCO_MIN_IDLE_PARTS, COCO_MAX_IDLE_PARTS ))
    leg_action = COCO_IDLE;
  else if (COCO_LEG_RANGE( legs, COCO_MIN_WALK_PARTS, COCO_MAX_WALK_PARTS ))
    leg_action = COCO_RUN;
  else if (COCO_LEG_RANGE( legs, COCO_MIN_SQUAT_PARTS, COCO_MAX_SQUAT_PARTS ))
    leg_action = COCO_LAND;
  else if (COCO_LEG_RANGE( legs, COCO_AKIMBO_PART, COCO_AKIMBO_PART+1 ))
    leg_action = COCO_FLY;
  else
    {
      leg_action = COCO_FLY;
#ifdef COCO_DEBUG
      coco_warn_printf( "bogen leg raster index %d\n", legs );
#endif
    }

  

  
  /* gravity */
  {
    coco_gnum y = COCO_SOBJECT_Y( sobject );
    coco_gnum dy = COCO_SOBJECT_DY( sobject );
    /* want to get back to ground asap */
    coco_gnum dy_want = COCO_GROUND_Y - y;
    if (dy_want < 0)
      {
        /* but not faster than accell */
        dy_want = COCO_MAX( dy_want, dy-COCO_MECH_GRAVITY );
        /* and not faster than terminal velocity */
        dy_want = COCO_MAX( dy_want, -COCO_MECH_DY_MAX );
      }
    COCO_SOBJECT_DY( sobject ) = dy_want;
  }

  { 
    coco_gnum dx = COCO_SOBJECT_DX( sobject );
    /* dx_want_sorta only used for picking leg raster */
    /* handle jumping movement */
    if (jump)
      {
        /* due to ground effect, and leg muscles, its much cheaper energy
        wise to jump from the ground than to jump in mid air.  That's what
        this effective_e is all about.  */
        coco_u16 effective_e;
        coco_u16 e = COCO_MECH_ENERGY( sobject );
        coco_gnum dy = COCO_SOBJECT_DY( sobject );
        coco_gnum dyroom = COCO_MECH_DY_MAX - dy;
        coco_gnum dydt;
        coco_gnum thrust;
        if (COCO_SOBJECT_Y( sobject ) <= COCO_GROUND_Y)
          {
            thrust = 12;
            effective_e = e<<1;
          }
        else
          {
            thrust = 5;
            effective_e = e;
          }
        thrust = (COCO_SOBJECT_Y( sobject ) <= COCO_GROUND_Y) ? 12 : 5;
        dydt = COCO_MIN( effective_e, dyroom );
        dydt = COCO_MIN( thrust, dydt );
        COCO_SOBJECT_DY( sobject ) = dy + dydt;
        legs_out 
          = (run < 0)
          ? (COCO_AKIMBO_PART + COCO_LEFTY_LEG_PART)
          : ((run > 0)
             ? COCO_AKIMBO_PART
             : (COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_TRUE,
                (lefty + COCO_MIN_SQUAT_PARTS)) );
        COCO_MECH_ENERGY( sobject ) 
          = ((e==effective_e)?(e - dydt):(e-(dydt>>1)) );
        jumped_strength = 1 + (dydt >= 5);
        landed_p = COCO_FALSE;
      }
    /* if in air... */
    if ((COCO_SOBJECT_Y( sobject ) > COCO_GROUND_Y+1)
        && !landed_p)
      {
        if (run)
          {
            coco_gnum dxroom;
            if (run<0)
              {
                dxroom = COCO_MECH_JUMPING_DX_MAX + dx;
                COCO_SOBJECT_DX( sobject )
                  = dx - COCO_MIN( dxroom, COCO_MECH_JUMPING_DDX_MAX );
                if (!lefty)
                  legs_out = legs + COCO_LEFTY_LEG_PART;
              }
            else
              {
                dxroom = COCO_MECH_JUMPING_DX_MAX - dx;
                COCO_SOBJECT_DX( sobject )
                  = dx + COCO_MIN( dxroom, COCO_MECH_JUMPING_DDX_MAX );
                if (lefty)
                  legs_out = legs - COCO_LEFTY_LEG_PART;
              };
            exhaust_p = COCO_TRUE;
          }
        else
          {
            int squati = COCO_MIN_SQUAT_PARTS + lefty;
            struct coco_mech_part *squat = coco_leg_parts + squati;
            coco_gnum dh = coco_leg_parts[legs].dh - squat->dh;
            if ((COCO_SOBJECT_DY( sobject ) <= 0)
                && (!COCO_LEG_RANGE( legs, COCO_MIN_SQUAT_PARTS, 
                                     COCO_MIN_SQUAT_PARTS+1 ))
                && (COCO_SOBJECT_Y( sobject ) + dh >= COCO_GROUND_Y))
              {
                legs_out = squati;
                /* COCO_SOBJECT_Y( sobject ) += dh; */
                COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_TRUE;
              }
            /* flying friction */
            if (dx>0)
              COCO_SOBJECT_DX( sobject ) = dx-1;
            else if (dx<0)
              COCO_SOBJECT_DX( sobject ) = dx+1;
            exhaust_p = !!jumped_strength;
          }

      }
    else if (COCO_FLY==leg_action)
      {
        if (run)
          {
            legs_out
              = (run<0)
              ? COCO_MIN_WALK_PARTS + COCO_LEFTY_LEG_PART
              : COCO_MIN_WALK_PARTS;
          }
        else
          {
            legs_out
              = legs - COCO_AKIMBO_PART + COCO_MIN_SQUAT_PARTS;
            COCO_SOBJECT_DX( sobject ) = 0;
            COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_TRUE;
          }
      }
    else if (COCO_RUN==leg_action)
      {
        coco_facing old_run =
          COCO_BETWEENA( COCO_MIN_WALK_PARTS,
                         legs,
                         COCO_MAX_WALK_PARTS )
          ? 1
          : (COCO_BETWEENA( COCO_MIN_WALK_PARTS + COCO_LEFTY_LEG_PART,
                            legs,
                            COCO_MAX_WALK_PARTS + COCO_LEFTY_LEG_PART )
             ? -1
             : 0 );
        
        if (run && (run != old_run))
          legs_out = (run>0) ? COCO_MIN_WALK_PARTS
            : COCO_MIN_WALK_PARTS + COCO_LEFTY_LEG_PART;
        else if (run)
          {
            landed_p = COCO_FALSE;
            if (run<0)
              {
                legs_out
                  = ((legs >= (COCO_MAX_WALK_PARTS + COCO_LEFTY_LEG_PART - 1))
                     ? (COCO_MIN_WALK_PARTS + COCO_LEFTY_LEG_PART)
                     : (legs + 1));
                assert( COCO_BETWEENA( COCO_MIN_WALK_PARTS + COCO_LEFTY_LEG_PART,
                                       legs,
                                       COCO_MAX_WALK_PARTS + COCO_LEFTY_LEG_PART));
                COCO_SOBJECT_DX( sobject ) = - COCO_MECH_WALKING_DX_MAX;
              }
            else
              {
                legs_out
                  = ((legs >= (COCO_MAX_WALK_PARTS - 1))
                     ? COCO_MIN_WALK_PARTS
                     : (legs + 1));
                assert( COCO_BETWEENA( COCO_MIN_WALK_PARTS,
                                       legs,
                                       COCO_MAX_WALK_PARTS ));
                COCO_SOBJECT_DX( sobject ) = COCO_MECH_WALKING_DX_MAX;
              }
          }
        else
          {
            legs_out = COCO_MIN_IDLE_PARTS + lefty;
            COCO_SOBJECT_DX( sobject ) = 0;
          }

      }
    else if (run&&((COCO_IDLE==leg_action)||(COCO_LAND==leg_action)))
      {
        landed_p = COCO_FALSE;
        COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_FALSE;
        legs_out
          = (run<0)
          ? COCO_MIN_WALK_PARTS + COCO_LEFTY_LEG_PART
          : COCO_MIN_WALK_PARTS;
      }
    else if (COCO_IDLE==leg_action)
      {
        if ((legs==COCO_MIN_IDLE_PARTS) 
            || (legs==COCO_MIN_IDLE_PARTS + COCO_LEFTY_LEG_PART))
          {
            COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_TRUE;
            legs_out = legs + 1;
          }
        else if ((legs==COCO_MAX_IDLE_PARTS - 1) 
            || (legs==COCO_MAX_IDLE_PARTS + COCO_LEFTY_LEG_PART - 1))
          {
            COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_FALSE;
            legs_out = legs - 1;
          }
        else if (COCO_MECH_SQUATTING_DOWN( sobject ))
          legs_out = legs + 1;
        else
          legs_out = legs - 1;
      }
    else if (COCO_LAND==leg_action)
      {
        COCO_SOBJECT_DX( sobject ) = 0;
        if ((legs == COCO_MAX_SQUAT_PARTS-1)
            || (legs == COCO_MAX_SQUAT_PARTS-1 + COCO_LEFTY_LEG_PART))
          {
            legs_out = legs-1;
            COCO_MECH_SQUATTING_DOWN( sobject ) = COCO_FALSE;
          }
        else if (COCO_MECH_SQUATTING_DOWN( sobject ))
          legs_out = legs+1;
        else if ((legs==COCO_MIN_SQUAT_PARTS)
                 ||(legs==COCO_MIN_SQUAT_PARTS + COCO_LEFTY_LEG_PART))
          legs_out
            = legs - COCO_MIN_SQUAT_PARTS + COCO_MIN_IDLE_PARTS;
        else 
          legs_out = legs-1;

      }

  }
 get_out:
  /* the walking stutter problem */
  assert( (!run) 
          || (COCO_SOBJECT_Y( sobject ) != COCO_GROUND_Y+1)
          || COCO_LEG_RANGE( legs_out, COCO_MIN_WALK_PARTS, 
                             COCO_MAX_WALK_PARTS ));
  assert( COCO_BETWEENA( 0, legs_out, COCO_MAX_LEG_PARTS ) );
  COCO_MECH_LEG_RASTER_INDEX( sobject ) = legs_out;
  if (exhaust_p)
    coco_make_mech_jet_exhaust( sobject, sstate, jumped_strength, run );
  if (landed_p)
    coco_mech_pad_update( sobject, COCO_SOBJECT_CL( sobject ), sstate );
  else COCO_MECH_LANDED_ON_PAD_P( sobject ) = COCO_FALSE;
  return coco_sobject_update_state( sobject, cl->super, sstate );
}

#undef COCO_MECH_PRESSED

#undef COCO_LEG_RANGE


struct coco_tutorial_objectives
coco_update_tutorial( coco_sstate *sstate )
{
  int i;
  struct coco_tutorial_objectives to;

  if (coco_tutorial && coco_tutorial->update)
    for (i=0; i<COCO_MAX_PLAYERS; i++)
      if (coco_players[i].in_use)
        {
          coco_sobject *cam = coco_find_camera( sstate, i, COCO_FALSE );
          return coco_tutorial->update( sstate, cam, i );
        }
    
  to.minor_mission = to.major_mission = 255;
  to.angle_on = COCO_FALSE;
  return to;
}


#define COCO_PLAYABLE_MAX_ARGS 5

void
coco_initialize_playable_sobject( coco_sobject *sobject, 
                                  coco_sobject_cl *cla,
                                  int p_or_c_index, coco_bool is_comp,
                                  coco_facing face, coco_gnum x, coco_gnum y,
                                  coco_nationality nat )
{
  coco_sobject *(* fn)
    (coco_sobject *, coco_sobject_cl *, coco_lnum *);
  coco_lnum fn_args[COCO_PLAYABLE_MAX_ARGS];
  assert( COCO_PLAYABLE_MAX_ARGS == cla->init_by_list_count );
  fn = cla->init_by_list_fn;
  fn_args[0] = is_comp ? -1 : p_or_c_index;
  fn_args[1] = face;
  fn_args[2] = x;
  fn_args[3] = y;
  fn_args[4] = nat;
  fn( sobject, cla, fn_args );
  if (is_comp)
    COCO_SOBJECT_COMP_INDEX( sobject ) = p_or_c_index;
  assert( COCO_SOBJECT_FACING( sobject ) == face );
  assert( COCO_SOBJECT_CL( sobject ) == cla );
  assert( COCO_SOBJECT_X( sobject ) == x );
  assert( COCO_SOBJECT_Y( sobject ) == y );
  assert( COCO_SOBJECT_NATIONALITY( sobject ) == nat );
}

#undef COCO_PLAYABLE_MAX_ARGS

/* returns true if the game is over */
coco_bool
coco_sstate_update( coco_sstate *sstate )
{
  {
    int i;
    for (i=0; i<2; i++)
      if ((!sstate->teams[i].summon_enable)
          && (sstate->time == sstate->teams[i].summon_disable_until))
        sstate->teams[i].summon_enable = COCO_TRUE;
  }
  /* SAVE PUBLIC VELOCITIES */
  COCO_DO_SSTATE_OBJECTS ( sobject, sstate,
  {
    COCO_SOBJECT_DX_PUBLIC( sobject ) = COCO_SOBJECT_DX( sobject );
    COCO_SOBJECT_DY_PUBLIC( sobject ) = COCO_SOBJECT_DY( sobject );
  });
  /* UPDATE STATE */
  { /* can't use COCO_DO_SSTATE_OBJECTS because of array resizing. */
    int i;
    for (i=0; i<sstate->sobject_size; i++)
      {
        coco_sobject *sobject = COCO_SSTATE_OBJECT_REF( sstate, i );
        if (COCO_SOBJECT_IN_USE( sobject )
            && !COCO_SOBJECT_NEW_FLAG( sobject ))
          if (!coco_sobject_update_state
              ( sobject, COCO_SOBJECT_CL( sobject ), sstate ))
            { 
              sobject = COCO_SSTATE_OBJECT_REF( sstate, i ); /* maybe moved! */
              coco_sobject_delete( sobject, COCO_SOBJECT_CL( sobject ), 
                                   sstate );
            }
      }
  }
  COCO_DO_SSTATE_OBJECTS( sobject, sstate,
  { /* DX->X, DY->Y, CLIP-TO-BOARD */
    COCO_SOBJECT_X( sobject ) += COCO_SOBJECT_DX( sobject );
    COCO_SOBJECT_Y( sobject ) += COCO_SOBJECT_DY( sobject );
    if (COCO_SOBJECT_CLIP_TO_BOARD( sobject ))
      {
        if (COCO_SOBJECT_X( sobject ) < 0)
          COCO_SOBJECT_X( sobject ) = 0;
        else if (COCO_SOBJECT_X( sobject ) 
                 >= (sstate->board_w - COCO_SOBJECT_W( sobject )))
          COCO_SOBJECT_X( sobject ) 
            = (sstate->board_w - COCO_SOBJECT_W( sobject ));
        if (COCO_SOBJECT_Y( sobject ) < 0)
          COCO_SOBJECT_Y( sobject ) = 0;
        else if (COCO_SOBJECT_Y( sobject ) 
                 >= (COCO_VIEWABLE_H - COCO_SOBJECT_H( sobject )))
          {
            COCO_SOBJECT_Y( sobject ) 
              = (COCO_VIEWABLE_H - COCO_SOBJECT_H( sobject ));
            COCO_SOBJECT_DY( sobject )
              = COCO_MIN( COCO_SOBJECT_DY( sobject ), 0 );
          }
      }
  });
  /* DETECT COLLISIONS W OTHER SOBJECTS */
  coco_sstate_find_collisions( sstate );
  /* DETECT COLLISIONS W GROUND */
  {
    /* resolve collisions with the ground, can't use
       do-sstate-objects because objects might move.  */
    int i;
    for (i=0; i<sstate->sobject_size; i++)
      {
        coco_sobject *sobject = COCO_SSTATE_OBJECT_REF( sstate, i );
        if (COCO_SOBJECT_IN_USE( sobject )
            && ((COCO_SOBJECT_Y( sobject ) < COCO_GROUND_Y)))
          coco_ground_collide( sobject, COCO_SOBJECT_CL( sobject ), sstate );
      }
  }
  sstate->tutorial_objectives = coco_update_tutorial( sstate );
  {  
    int i;
    /* warp in new copters for human players*/
    for (i=0; i<COCO_MAX_PLAYERS; i++)
      if (coco_players[i].in_use
          && coco_players[i].sent_play_message 
          && sstate->respawn[i]
          && (sstate->time == sstate->respawn_times[i]))
        {
          coco_nationality nat = coco_players[i].nationality;
          coco_facing face;
          coco_gnum x, y;
          coco_sobject_cl *fcc;
          coco_sobject *newcop = NULL;
          coco_bool spare_copter_p;
          coco_copter_start_coords( sstate, nat, i, &x, &y );
          sstate->respawn[i] = COCO_FALSE;
          {
            int index = sstate->camera_indicies[i];
            if (-1 != index)
              {
                coco_sobject *sobject
                  = COCO_SSTATE_OBJECT_REF( sstate, index );
                if (coco_sobject_subtype_p( COCO_SOBJECT_CL( sobject ), 
                                            COCO_COPTER_CLASS ))
                  {
                    coco_warn_printf( "Time to respawn, but somehow there's "
                                      "already a copter for this player.\n" );
                    break;
                  }
                else coco_sstate_delete_sobject( sstate, sobject );
              }
          }
          spare_copter_p = (coco_players[i].copters>0);
          if (spare_copter_p)
            coco_players[i].copters--;
          else 
            coco_subtract_funds_available( i, 
                                           COCO_FALSE, 
                                           COCO_BUBBLE_COPTER_CLASS->cost );
          fcc = spare_copter_p
            ? sstate->favorite_copter_class[i]
            : COCO_BUBBLE_COPTER_CLASS;
          face = (COCO_GOOD_GUYS==nat)?-1:1;
          newcop = coco_sstate_allocate_sobject( sstate );
          assert( newcop );
          coco_initialize_playable_sobject( newcop, fcc, i, COCO_FALSE,
                                            face, x, y, nat );
          sstate->camera_indicies[i] 
            = COCO_SOBJECT_INDEX( newcop );
        }
    /* ah, if they've not decided to play yet, put this off for a nanosecond */
      else if (sstate->respawn[i]
               && (sstate->time == sstate->respawn_times[i]))
        sstate->respawn_times[i] 
          = COCO_U32_ADD_MODULAR( sstate->respawn_times[i] );

    /* warp in new copters for comps */
    for (i=0; i<COCO_MAX_COMPS; i++)
      if (coco_comps[i].in_use
          && sstate->comp_respawn[i]
          && (sstate->time == sstate->comp_respawn_times[i]))
        {
          coco_gnum x, y;
          coco_sobject *copter;
          coco_bool spare_copter_p = (coco_comps[i].copters > 0);
          coco_nationality nat = coco_comps[i].nationality;
          coco_sobject_cl *fcc;


          sstate->comp_respawn[i] = COCO_FALSE;
          if (spare_copter_p)
            coco_comps[i].copters--;
          else
            coco_comps[i].funds
              = COCO_MIN( coco_comps[i].funds
                          - COCO_BUBBLE_COPTER_CLASS->cost, 
                          0 );
          fcc = spare_copter_p 
            ? COCO_COMP_FAVORITE_COPTER_CLASS
            : COCO_BUBBLE_COPTER_CLASS;
          coco_copter_start_coords( sstate, nat, -1, &x, &y );
          copter = coco_sstate_allocate_sobject( sstate );
          if (NULL==copter)
            { /* put it off and try to recover. */
              sstate->comp_respawn[i] = COCO_TRUE;
              sstate->comp_respawn_times[i] =
                COCO_U32_ADD_MODULAR( sstate->time );
              coco_warn_printf( "not enough memory to respawn comp\n" );
            }
          coco_initialize_playable_sobject( copter, fcc, i, COCO_TRUE,
                                            (COCO_GOOD_GUYS==nat)?-1:1,
                                            x, y, nat );
        }
  }
  COCO_DO_SSTATE_OBJECTS( sobject, sstate, {
    COCO_SOBJECT_NEW_FLAG( sobject ) = COCO_FALSE;
  });
  coco_add_hazard_pay( sstate );
  sstate->time = COCO_U32_ADD_MODULAR( sstate->time );
  return ((COCO_NEUTRAL != sstate->game_over_nationality)
          && (sstate->game_over_time == sstate->time));
}

void
coco_add_hazard_pay( coco_sstate *sstate )
{
  /* Hazard pay for being on same screen as enemy copter, or enemy hq: 
     1 fund / second.  Hazard pay for being near or past enemy copter: 
     .25 fund / second.  */
  if (0 == (sstate->time % (COCO_IDEAL_FRAMES_PER_SECOND>>2)))
    {
      static coco_bool ST_is_comps[] = { COCO_FALSE, COCO_TRUE };
      static int ST_maxes[] = { COCO_MAX_PLAYERS, COCO_MAX_COMPS };
      int i, j;
      coco_bool is_comp;
      int max;
      for (j = 0; j < 2; j++)
        {
          coco_sobject *cam;
          is_comp = ST_is_comps[j];
          max = ST_maxes[j];
          for (i=0; i<max; i++)
            if ((is_comp ? (coco_comps[i].in_use) : (coco_players[i].in_use))
                && (cam = coco_find_camera( sstate, i, is_comp )))
              {
                coco_gnum camx = COCO_SOBJECT_X( cam );
                coco_nationality nat = COCO_SOBJECT_NATIONALITY( cam );
                coco_bool targ_on_screen_p = COCO_FALSE;
                coco_bool passed_targ_p = COCO_FALSE;
                COCO_DO_SSTATE_OBJECTS( targ, sstate,
                {
                  if (COCO_ENEMY_P( nat, COCO_SOBJECT_NATIONALITY( targ ) )
                      && (coco_sobject_subtype_p( COCO_SOBJECT_CL( targ ),
                                                  COCO_CAMERA_CLASS )
                          || coco_sobject_subtype_p( COCO_SOBJECT_CL( targ ),
                                                     COCO_HEADQUARTERS_CLASS )))
                    {
                      coco_gnum targx = COCO_SOBJECT_X( targ );
                      coco_gnum dist = abs( targx - camx );
                      if (dist < COCO_OFFICIAL_WINDOW_W)
                        {
                          targ_on_screen_p = COCO_TRUE;
                          break;
                        }
                      if (nat == COCO_GOOD_GUYS)
                        passed_targ_p |= (targx > camx);
                      else /* nat == COCO_BAD_GUYS */
                        passed_targ_p |= (targx < camx);
                    }
                });
                if (targ_on_screen_p)
                  coco_add_funds( i, is_comp, 0.25 );
                else if (passed_targ_p)
                  coco_add_funds( i, is_comp, 0.0675 );
              }
        }
    }
}



/* Returns true if the player shouldn't be booted.  Some error conditions
   don't merit a booting, because they will fix themselves, I hope. */
coco_bool
coco_send_cstate( int udpfd, int plr_index, void *cstate_buf, size_t buf_size )
{
  struct sockaddr_in sockaddr;
  int sendto_result;

  if (coco_udp_ports[plr_index]<0)
    {
      coco_warn_printf( "Client number %d hasn't sent in "
                        "port info yet.  Looks like he don't get"
                        "his udp packets. (Not Booting)\n" );
      return COCO_TRUE;
    }


  sockaddr.sin_family = PF_INET;
  sockaddr.sin_port = htons( (short) coco_udp_ports[plr_index] );
  sockaddr.sin_addr.s_addr = htonl( (long) coco_client_addresses[plr_index] );

  /*
    coco_cookie( "sending cstate to " );
    coco_format_ipaddr( stderr, coco_client_addresses[plr_index], 
    coco_udp_ports[plr_index], COCO_TRUE );
    coco_cookie( "\n" );
  */

  sendto_result = sendto( udpfd, cstate_buf, buf_size, 0,
                          (struct sockaddr *) &sockaddr, sizeof( sockaddr ) );
  if (-1 == sendto_result)
    switch (errno) {
    case EAGAIN:
    case ENOBUFS:
      coco_warn_printf( "Couldn't send udp packet to player"
                        " %d because the udp fd was locally"
                        " full.  Better luck next turn.\n",
                        plr_index );
      break;
    default:
      coco_warn_printf( "sendto failed with errno=%d: "
                        "%s\n", errno, 
                        coco_errno_string( errno ) );
      return COCO_FALSE;
    }
  else if (sendto_result < buf_size)
    coco_warn_printf( "Could only send %d/%d bytes of "
                      "udp packet to player %d. (Not Booting)\n",
                      sendto_result, buf_size, plr_index );
  return COCO_TRUE;
}

coco_bool
coco_buy_copter_maybe( coco_sstate *sstate, 
                       int p_or_c_index, coco_bool is_comp  )
{
  float cost = (is_comp
                ? COCO_COMP_FAVORITE_COPTER_CLASS 
                : sstate->favorite_copter_class[p_or_c_index])->cost;
  if (coco_subtract_funds_available( p_or_c_index, is_comp, cost ))
    {
      if (is_comp)
        coco_comps[p_or_c_index].copters++;
      else 
        coco_players[p_or_c_index].copters++;
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}


/* NULL on fail */
coco_sobject *
coco_find_camera( coco_sstate *sstate, int p_or_c_index, coco_bool is_comp )
{
  if (is_comp)
    COCO_DO_SSTATE_OBJECTS( sobject, sstate, {
      if (p_or_c_index == COCO_SOBJECT_COMP_INDEX( sobject ))
        return sobject;
    })
  else
    {
      int idx = sstate->camera_indicies[p_or_c_index];
      if (idx >=0)
        return COCO_SSTATE_OBJECT_REF( sstate, idx );
      else return NULL;
    }
  return NULL;
}


#define COCO_MAX_FANCY_COPTER 3

coco_sobject_cl *coco_fancy_copter_classes[COCO_MAX_FANCY_COPTER] = {
  COCO_COBRA_CLASS, COCO_CARGO_COPTER_CLASS, COCO_MECH_CLASS
};


void 
coco_server_key_press( int p_or_c_index, coco_bool is_comp,
                       coco_key key, coco_sstate *sstate )
{
  coco_sobject *cam = coco_find_camera( sstate, p_or_c_index, is_comp );
  if (cam)
    {
      
      switch (key) {
      case COCO_LPOINTER: 
        if (cam
            && coco_sobject_subtype_p( COCO_SOBJECT_CL( cam ), 
                                       COCO_COPTER_CLASS )
            && ! COCO_COPTER_LANDED_P( cam ))
          coco_toggle_facing( cam, COCO_SOBJECT_CL( cam ) );
        break;
      case COCO_CHAR_TO_KEY( 'h' ):
        coco_buy_copter_maybe( sstate, p_or_c_index, is_comp );
        break;
      case COCO_CHAR_TO_KEY( 'n' ):
        if (is_comp)
          coco_warn_printf( "Changing favorite copter class "
                            "not supported for comps.\n" );
        else 
          {
            int i;
            int new_class_idx = 0;
            coco_sobject_cl *new_fav, *old_fav
              = sstate->favorite_copter_class[p_or_c_index];
            for (i=0; i< COCO_MAX_FANCY_COPTER-1; i++)
              if (coco_fancy_copter_classes[i]
                  == sstate->favorite_copter_class[p_or_c_index])
                {
                  new_class_idx = i+1;
                  break;
                }
            new_fav
              = sstate->favorite_copter_class[p_or_c_index]
              = coco_fancy_copter_classes[new_class_idx];
            {
              int ncopters = coco_players[p_or_c_index].copters;
              float nc = new_fav->cost;
              float oc = old_fav->cost;
              float ocx = oc * ncopters;
              if (nc == 0)
                {
#ifdef COCO_DEBUG                  
                  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                                     "favorite copter class has 0 cost, would "
                                     "cause divide by zero.");
#else
                  nc = 0.001;
#endif
                }
              if (nc > oc)
                {
                  coco_players[p_or_c_index].copters = ocx / nc;
                  coco_add_funds( p_or_c_index, is_comp, fmod( ocx, nc ) );
                }
              else
                coco_add_funds( p_or_c_index, is_comp, ocx - nc * ncopters );
            }
          }
        break;
      default:
        break;
      }
    }
}

/* Should be used in leiu of setting the bits individually. */
void
coco_comp_key_press( struct coco_keyboard_pointer_state *kstate,
                     struct coco_comp *self,
                     coco_key key )
{
  coco_server_key_press( self->index, COCO_TRUE, key, coco_get_sstate() );
  coco_set_bit( kstate->key_state, key, COCO_TRUE );
}


/* Function removed, since supporting it would require work to integrate
   with comps. 
void 
coco_server_key_release( int player_idx, coco_key key, coco_sstate *sstate )
{
  /+ this function left intentionally blank, since nothing uses it now. +/

}
*/

void
coco_read_client_packet( int udpfd, coco_sstate *sstate )
{
  struct sockaddr_in sockaddr;
  struct coco_client_to_server_packet pkt;
  int recv_result, plr_idx;
  unsigned int sockaddr_size = sizeof( sockaddr );

  while (1) {
    recv_result = recvfrom( udpfd, &pkt, sizeof( pkt ), 0,
                            (struct sockaddr *) &sockaddr, &sockaddr_size ); 
    if (recv_result != sizeof( pkt ))
      {
        if (-1 == recv_result)
          if (EAGAIN == errno) return;
          else
            {
              /* coco_warn_printf( "BANG (port %d) recvfrom failed with "
                 "errno=%d: %s\n",
                 udp_port,
                 errno, coco_errno_string( errno ) ); */
              coco_warn_printf( "recvfrom failed with errno=%d: %s\n"
                                "  server wishes it knew who sent it, "
                                "so it could boot them.\n", errno, 
                                coco_errno_string( errno ) );
              return;
            }
        else 
          {
            coco_warn_printf("recvfrom retrieved the wrong number of bytes.  "
                             "Skipping this packet.\n");
            continue;
          }
      }
    plr_idx = pkt.player_id;
    if (!coco_players[plr_idx].in_use)
      coco_warn_printf( "Ignoring mouse data for non-existant player #%d\n", 
                        plr_idx );
    else
      {
        coco_bool first_packet = COCO_FALSE;

        if ((-1 == coco_udp_ports[plr_idx]) 
            && (0 == coco_client_addresses[plr_idx]))
          {
            coco_udp_ports[plr_idx] = ntohs( sockaddr.sin_port );
            coco_client_addresses[plr_idx] = (coco_u32) ntohl( sockaddr.sin_addr.s_addr );
            first_packet = COCO_TRUE;
            coco_cookie( "Got our first udp transmission from \"%s\" "
                         "(%d.%d.%d.%d:%d)\n",
                         coco_players[plr_idx].name,
                         coco_client_addresses[plr_idx] >> 24,
                         (coco_client_addresses[plr_idx] >> 16) & 255,
                         (coco_client_addresses[plr_idx] >> 8) & 255,
                         (coco_client_addresses[plr_idx] >> 0) & 255,
                         coco_udp_ports[plr_idx]
                         );

          }
        else if ((ntohs( sockaddr.sin_port ) != coco_udp_ports[plr_idx])
                 || ((coco_u32) ntohl( sockaddr.sin_addr.s_addr )
                     != coco_client_addresses[plr_idx]))
          {
            coco_u32 host = (coco_u32) ntohl( sockaddr.sin_addr.s_addr );
            coco_warn_printf( "Whoa, looks like %d.%d.%d.%d:%d is attempting "
                              "to hijack \"%s\"'s controls.\n",
                              host>>24 & 255,
                              host>>16 & 255,
                              host>>8 & 255,
                              host&255,
                              ntohs( sockaddr.sin_port ),
                              coco_players[plr_idx].name);
            continue;
          }
        pkt.input_state.client_time
          = COCO_U32_NTOH( pkt.input_state.client_time );
        pkt.input_state.pointer_x
          = COCO_I32_NTOH( pkt.input_state.pointer_x );
        pkt.input_state.pointer_y
          = COCO_I32_NTOH( pkt.input_state.pointer_y );
        if (first_packet
            || time_less_than( coco_remote_input_states[plr_idx].client_time, 
                               pkt.input_state.client_time ))
          {
            static struct coco_keyboard_pointer_state *oris, *ris;
            int key;
            oris = coco_old_remote_input_states + plr_idx;
            ris = coco_remote_input_states + plr_idx;

            *ris = pkt.input_state;
            if (first_packet) 
              *oris = pkt.input_state;
            else
              {
                /* deal with new keypresses and releases as soon as we get
                them, since the next consecutive packet, that we might
                read before the next state-update, might have the matching
                release to our press, thus some key action could appear to
                be dropped.  Unless we act now!*/
                for (key=0; key<COCO_KEYLIMIT; key++)
                  {
                    if (coco_get_bit( ris->key_state, key ))
                      {
                        if (!coco_get_bit( oris->key_state, key ))
                          {
                            if (sstate->game_in_progress)
                              coco_server_key_press( plr_idx, COCO_FALSE,
                                                     key, sstate );
                            coco_set_bit( oris->key_state, key, 1 );
                          }
                      }
                    else /* !coco_get_bit( ris... ) */
                      {
                        if (coco_get_bit( oris->key_state, key ))
                          {
                            /* function no longer exists -dm 04-02-2001
                            if (sstate->game_in_progress)
                              coco_server_key_release( plr_idx, key, sstate );
                            */
                            coco_set_bit( oris->key_state, key, 0 );
                          }
                      }
                  }
                oris->pointer_x = ris->pointer_x;
                oris->pointer_y = ris->pointer_y;
                oris->client_time = ris->client_time;
              }
          }
        else if ((coco_remote_input_states[plr_idx].client_time !=
                  pkt.input_state.client_time))
          coco_warn_printf( "recieved out-of-order keystate packet from "
                            "\"%s\"(#%d) (%d states vs %d packet)\n",
                            coco_players[plr_idx].name, plr_idx,
                            coco_remote_input_states[plr_idx].client_time,
                            pkt.input_state.client_time );
      }
  }
}


void 
coco_server_shutdown( int listenfd, int out_udpfd, int in_udpfd )
{
  int i;
  if (listenfd != -1)  coco_close_fd( listenfd, COCO_FALSE );
  if (out_udpfd != -1) coco_close_fd( out_udpfd, COCO_FALSE );
  if ((out_udpfd != in_udpfd) && (in_udpfd != -1))
    coco_close_fd( in_udpfd, COCO_FALSE );
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    if (coco_players[i].in_use)
      coco_close_fd( coco_streams[i].fd, COCO_TRUE );
  coco_log_printf( "Graceful server shutdown complete.\n" );
  exit( 0 );
}


/* suddenly not in glibc 2.1 when compiling -ansi */
#ifndef timercmp
#define timercmp(a, b, CMP) 						      \
  (((a)->tv_sec == (b)->tv_sec) ? 					      \
   ((a)->tv_usec CMP (b)->tv_usec) : 					      \
   ((a)->tv_sec CMP (b)->tv_sec))
#endif



void
coco_server_loop( int listenfd, int out_udpfd, 
                  int in_udpfd, unsigned short in_udp_port )
{
  struct timeval next_update, next_update_delta, now;
  static struct timeval time_zero = { 0, 0 };
  int i, maxfd;
  fd_set readset;
  gettimeofday( &next_update, NULL);
  coco_add_timevals( &next_update, &next_update, &coco_ideal_frame_rate );
  maxfd = COCO_MAX( listenfd, in_udpfd );
  while (1)
    {
      coco_sstate *sstate = coco_get_sstate();

      FD_ZERO( &readset );
      FD_SET( listenfd, &readset );
      FD_SET( in_udpfd, &readset );
      for (i=0; i<COCO_MAX_PLAYERS; i++)
        if (coco_players[i].in_use)
          {
            int fd = coco_streams[i].fd;
            FD_SET( fd, &readset );
            maxfd = COCO_MAX( fd, maxfd );
          }
      gettimeofday( &now, NULL );
      coco_subtract_timevals( &next_update_delta, &next_update, &now );
      /* coco_cookie( "%d %d ", next_update_delta.tv_sec, 
         next_update_delta.tv_usec ); */
      /* ((next_update_delta.tv_sec < 0) || (next_update_delta.tv_usec < 0)) */
      if (timercmp( (&next_update_delta), (&time_zero), <= ))
        {
#ifdef COCO_DEBUG
          coco_warn_printf( "negative select sleep duration -- correcting\n" );
#endif
          next_update_delta.tv_sec = 0;
          next_update_delta.tv_usec = 0;
        }
      if (-1==select( maxfd+1, &readset, NULL, NULL, 
                      (sstate->game_in_progress ? &next_update_delta : NULL) ))
        coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                           "select failed with errno=%d: %s\n", errno, 
                           coco_errno_string( errno ) );
 
      for (i=0; i<=maxfd; i++)
        if (FD_ISSET( i, &readset ))
          {
            if (listenfd == i) 
              coco_handle_connection_request( listenfd, in_udp_port, sstate );
            else if (in_udpfd == i)
              coco_read_client_packet( in_udpfd, sstate );
            /* would like to boot the player if read_client_packet fails, but
               then again, we don't know who it is that's failing.  */
            else
              {
                int index;
                index = coco_find_player_index_by_fd( i );
                if ((index<0) || (index>COCO_MAX_PLAYERS)
                    || (!coco_players[index].in_use))
                  coco_warn_printf( "Noise coming from bad fd %d.\n.", i );
                else if (!coco_handle_server_message( index ))
                  coco_server_shutdown( listenfd, out_udpfd, in_udpfd );
              }
          }
      gettimeofday( &now, NULL );
      if (sstate->game_in_progress && timercmp( &next_update, &now, <= ))
        { 
          struct sockaddr_in sockaddr;
          sockaddr.sin_family = PF_INET;

          /* calculate the new sstate */
          if (coco_sstate_update( sstate ))
            coco_server_shutdown( listenfd, out_udpfd, in_udpfd );
          coco_add_timevals( &next_update, &now, &coco_ideal_frame_rate );
          /* send it to players */
          for (i=0; i<COCO_MAX_PLAYERS; i++)
            if (coco_players[i].in_use && (-1!=coco_udp_ports[i]))
              {
                size_t size;
                void *buf
                  = coco_format_sstate_to_buffer( sstate, i, COCO_FALSE,
                                                  COCO_TRUE, &size,
                                                  NULL, NULL );
                if (!coco_send_cstate( out_udpfd, i, buf, size ))
                  if (!coco_handle_player_quit( i ))
                    coco_server_shutdown( listenfd, out_udpfd, in_udpfd );
              }
          /* have the comps do their thing */
          for (i=0; i<COCO_MAX_COMPS; i++)
            if (coco_comps[i].in_use)
              {
                size_t size;
                coco_cobject *cobjs;
                struct coco_cblip *cblips;
                void *buf;
                buf = coco_format_sstate_to_buffer( sstate, i, COCO_TRUE,
                                                    COCO_FALSE, &size,
                                                    &cobjs, &cblips );
                coco_comp_think( coco_comps + i,
                                 buf, cobjs, cblips );
              }
        }
    }
}



void 
coco_server_exec( void )
{
  int tcpfd, in_udpfd, out_udpfd;
  char *level_error;
  unsigned short in_udp_port;
  coco_sstate *sstate = coco_get_sstate();
  assert( coco_level_text );
  level_error = coco_load_level_definition( coco_level_text, sstate );
  if (level_error)
    coco_fatal_printf( COCO_ERROR_BAD_LEVEL,
                       "Problem loading level: %s\n",
                       level_error );
  coco_init_comps( sstate );
  coco_server_setup( &tcpfd, &out_udpfd, &in_udpfd, &in_udp_port );
  coco_server_loop( tcpfd, out_udpfd, in_udpfd, in_udp_port );
}

/* _Bottling up the game state for udp___________________________________ */


void
coco_metal_shrapnel_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                                coco_cobject *cobject, coco_gnum cam_left,
                                coco_bool nbo )
{
  /* call-next-method WRONG? superclass not generally cobject inheritor!*/
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  cobject->metal_shrapnel.color = COCO_METAL_SHRAPNEL_COLOR_INDEX( sobject );
}

void
coco_flying_player_name_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                                    coco_cobject *cobject, coco_gnum cam_left,
                                    coco_bool nbo )
{
  coco_cnum x, y;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  cobject->fpn.bits 
    = (1 & (COCO_SOBJECT_NATIONALITY( sobject ) - COCO_GOOD_GUYS))
    | (COCO_FLYING_PLAYER_NAME_IS_COMP( sobject ) ? 2 : 0)
    | (COCO_FLYING_PLAYER_NAME_P_OR_C_INDEX( sobject ) << 2);
  y = COCO_SOBJECT_Y( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  COCO_COBJECT_X( cobject ) = nbo ? COCO_CNUM_HTON( x ) : x;
  cobject->fpn.y            = nbo ? COCO_CNUM_HTON( y ) : y;
}


void
coco_sndobject_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                           coco_cobject *cobject, coco_gnum cam_left,
                           coco_bool nbo )
{
  /* call-next-method WRONG? */
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  cobject->sound.sound_type = COCO_SNDOBJECT_SOUND( sobject );
}


void
coco_y_angle_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                         coco_cobject *cobject, coco_gnum cam_left,
                         coco_bool nbo )
{
  /* call-next-method WRONG? */
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  cobject->a.angle = COCO_SOBJECT_ANGLE( sobject );
}

void
coco_plasma_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                        coco_cobject *cobject, coco_gnum cam_left,
                        coco_bool nbo )
{
  coco_cnum x, y;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  cobject->r.raster_index = COCO_SOBJECT_RASTER_INDEX( sobject );
  y = COCO_SOBJECT_Y( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  COCO_COBJECT_X( cobject )    = nbo ? COCO_CNUM_HTON( x ) : x;
  cobject->plasma.y            = nbo ? COCO_CNUM_HTON( y ) : y;
  cobject->plasma.bits         = COCO_PLASMA_RADIUS( sobject )<<1 |
    COCO_PLASMA_COHERENT( sobject );
}

void 
coco_rasteroid_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                           coco_cobject *cobject, coco_gnum cam_left,
                           coco_bool nbo )
{
  coco_cnum x, y;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  cobject->r.raster_index = COCO_SOBJECT_RASTER_INDEX( sobject );
  y = COCO_SOBJECT_Y( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  COCO_COBJECT_X( cobject ) = nbo ? COCO_CNUM_HTON( x ) : x;
  cobject->y.y              = nbo ? COCO_CNUM_HTON( y ) : y;
}

void 
coco_spark_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                       coco_cobject *cobject, coco_gnum cam_left,
                       coco_bool nbo )
{
  coco_cnum x, y;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  cobject->spark.dy = COCO_SPARK_SPARK_DY( sobject );
  cobject->spark.dx = COCO_SPARK_SPARK_DX( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  y = COCO_SOBJECT_Y( sobject );
  COCO_COBJECT_X( cobject ) = nbo ? COCO_CNUM_HTON( x ) : x;
  cobject->y.y              = nbo ? COCO_CNUM_HTON( y ) : y;
}


void 
coco_ground_rasteroid_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                                  coco_cobject *cobject, coco_gnum cam_left,
                                  coco_bool nbo )
{
  coco_cnum x;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  cobject->r.raster_index = COCO_SOBJECT_RASTER_INDEX( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  COCO_COBJECT_X( cobject ) = nbo ? COCO_CNUM_HTON( x ) : x;
}

void 
coco_gun_tank_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                          coco_cobject *cobject, coco_gnum cam_left,
                          coco_bool nbo )
{
  /* call-next-method, which had best be ground_rasteroid. */
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  COCO_COBJECT_ANGLE( cobject ) = COCO_SOBJECT_ANGLE( sobject );
}


void 
coco_balloon_line_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                              coco_cobject *cobject, coco_gnum cam_left,
                              coco_bool nbo )
{
  coco_cnum x, y;
  coco_gnum h = COCO_SOBJECT_H( sobject );
  cobject->yh.hibyte = h & (255<<8);
  cobject->yh.lobyte = h & 255;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  y = COCO_SOBJECT_Y( sobject );
  COCO_COBJECT_X( cobject ) = nbo ? COCO_CNUM_HTON ( x ) : x;
  cobject->y.y              = nbo ? COCO_CNUM_HTON( y ) : y;
}

void
coco_mech_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                      coco_cobject *cobject, coco_gnum cam_left,
                      coco_bool nbo )
{
  coco_cnum x, y;
  COCO_COBJECT_CL_INDEX( cobject ) = COCO_SOBJECT_CL_INDEX( sobject );
  cobject->mech.lobits = COCO_MECH_TORSO_RASTER_INDEX( sobject );
  cobject->mech.hibits  = COCO_MECH_LEG_RASTER_INDEX( sobject )
    | ((COCO_SOBJECT_NATIONALITY( sobject )-COCO_GOOD_GUYS)<<6)
    | COCO_MECH_SHIELD_POWERED( sobject )<<7;
  y = COCO_SOBJECT_Y( sobject );
  x = COCO_SOBJECT_X( sobject ) - cam_left;
  COCO_COBJECT_X( cobject ) = nbo ? COCO_CNUM_HTON( x ) : x;
  cobject->mech.y           = nbo ? COCO_CNUM_HTON( y ) : y;
}

coco_sobject *
coco_find_sobject( coco_sstate *sstate, coco_sobject_cl *cl, 
                   coco_bool subclass_ok, int nth, int nationality_or_minus_one )
{
  int count = 0;
  COCO_DO_SSTATE_OBJECTS( sobject, sstate, {
    coco_sobject_cl *cl1 = COCO_SOBJECT_CL( sobject );
    if ((subclass_ok ? coco_sobject_subtype_p( cl1, cl ) : (cl1 == cl))
        && ((-1 == nationality_or_minus_one)
            || (COCO_SOBJECT_NATIONALITY( sobject ) == nationality_or_minus_one))
        && (++count == nth))
      return sobject;
  });
  return NULL;
}


coco_sobject *
coco_find_sobject_in_range( coco_sstate *sstate, coco_sobject_cl *cl, 
                            coco_bool subclass_ok, int nth, int nationality_or_minus_one,
                            coco_gnum xmin, coco_gnum xmax )
{
  int count = 0;
  int x;
  COCO_DO_SSTATE_OBJECTS( sobject, sstate, {
    coco_sobject_cl *cl1 = COCO_SOBJECT_CL( sobject );
    if ((subclass_ok ? coco_sobject_subtype_p( cl1, cl ) : (cl1 == cl))
        && ((-1 == nationality_or_minus_one)
            || (COCO_SOBJECT_NATIONALITY( sobject ) == nationality_or_minus_one))
        && ((x=(COCO_SOBJECT_X( sobject )))>=xmin)
        && (x<xmax)
        && (++count == nth))
      return sobject;
  });
  return NULL;
}

coco_sobject *
coco_find_sobject_if( coco_sstate * sstate, 
                      coco_bool (* predicate)( coco_sobject *, void * ),
                      void *handy )
{
  COCO_DO_SSTATE_OBJECTS( sobject, sstate, {
    if (predicate( sobject, handy ))
      return sobject;
  });
  return NULL;
}

/* predicate for coco_find_sobject_if(),
   find explosions near an object.  */
coco_bool
coco_boot_camp_headquarters_mayhem( coco_sobject *sobject, void *hqp )
{
  coco_sobject *hq = hqp;
  coco_sobject_cl *scl = COCO_SOBJECT_CL( sobject );
  if (abs( COCO_SOBJECT_X( sobject ) - COCO_SOBJECT_X( hq )) < 100)
    {
      if (coco_sobject_subtype_p( scl, COCO_EXPLOSION_CLASS )
          && (COCO_BETWEENA( COCO_EXPLOSION_BOMB_BASE,
                             COCO_SOBJECT_RASTER_INDEX( sobject ),
                             COCO_EXPLOSION_BOMB_BASE 
                             + COCO_EXPLOSION_BOMB_COUNT )
              || COCO_BETWEENA( COCO_EXPLOSION_NAPALM_BASE,
                                COCO_SOBJECT_RASTER_INDEX( sobject ),
                                COCO_EXPLOSION_NAPALM_BASE 
                                + COCO_EXPLOSION_NAPALM_COUNT )))
        return COCO_TRUE;
      else if ((COCO_SOBJECT_Y( sobject ) > COCO_GROUND_Y + 1)
               && coco_sobject_subtype_p( scl, COCO_SPARK_CLASS ))
        return COCO_TRUE;
    }
  return COCO_FALSE;
}



coco_angle
coco_point_arrow_at( coco_gnum x, coco_gnum y, coco_sstate *sstate,
                              coco_sobject *cam_object_or_null,
                              int player_index )
{
  coco_gnum cam_left, cam_right, dx, dy, arrow_x;
  coco_cam_bounds( sstate, cam_object_or_null, player_index, COCO_FALSE,
                   &cam_left, &cam_right );
  arrow_x = cam_left + COCO_TUTORIAL_ARROW_X;
  dx = x - arrow_x;
  dy = y - COCO_TUTORIAL_ARROW_Y;
  return coco_atan2( dy, dx );
}

coco_angle
coco_point_arrow_at_sobject( coco_sobject *at, coco_sstate *sstate,
                             coco_sobject *cam_object_or_null, int player_index )
{
  coco_mini_point mp;
  mp = coco_sobject_com_by_raster_index( COCO_SOBJECT_CL( at ), 
                                         COCO_SOBJECT_RASTER_INDEX( at ) );
  return coco_point_arrow_at( COCO_SOBJECT_X( at ) + COCO_MINI_POINT_X( mp ),
                              COCO_SOBJECT_Y( at ) + COCO_MINI_POINT_Y( mp ),
                              sstate, cam_object_or_null, player_index );
}



#define COCO_BOOT_CAMP_INFANTRY 3

struct coco_tutorial_objectives
coco_update_boot_camp( coco_sstate *sstate, 
                       coco_sobject *cam_object_or_null,
                       int player_index )
{
  static int major_mission = 0;
  static int minor_mission = 0;
  coco_bool angle_on = COCO_FALSE;
  coco_angle angle = 0;
  coco_nationality nat = coco_players[player_index].nationality;
  coco_nationality enat
    = (nat==COCO_GOOD_GUYS) ? COCO_BAD_GUYS : COCO_GOOD_GUYS;

  switch (major_mission) {
  case 0:
    if (sstate->time >= COCO_IDEAL_FRAMES_PER_SECOND * 10)
      {
        major_mission = 1;
        minor_mission = 1;
      }
    else if (sstate->time >= COCO_IDEAL_FRAMES_PER_SECOND * 5)
      {
        coco_sobject *pill;
        minor_mission = 7; /* fly to your pillbox */
        pill = coco_find_sobject( sstate, COCO_PILLBOX_CLASS, COCO_TRUE, 1, nat );
        if (pill)
          {
            angle_on = COCO_TRUE;
            angle = coco_point_arrow_at_sobject( pill, sstate,
                                                 cam_object_or_null, player_index );
          }
        /* else something is WRONG */
      }
    break;
  case 1:
    {
      int defenders, troopers;
      coco_sobject *bunker;
      coco_gnum board_w = sstate->board_w;
      coco_gnum pick_up_x0 = board_w - COCO_OFFICIAL_WINDOW_W;
      coco_gnum pick_up_x1 = board_w + COCO_OFFICIAL_WINDOW_W;

      bunker = coco_find_sobject( sstate, COCO_BUNKER_CLASS, 
                                  COCO_FALSE, /* subclass not ok -skip pillbox */
                                  1 /* count */, -1 );
      if (!bunker)
        {
          coco_warn_printf( "Could not find checkpoint charlie!\n" );
          break;
        }
      troopers = cam_object_or_null ? COCO_COPTER_PARATROOPERS( cam_object_or_null ) : 0;
      defenders = (nat == COCO_SOBJECT_NATIONALITY( bunker ))
        ? COCO_BUNKER_DEFENDERS( bunker ) : -COCO_BUNKER_DEFENDERS( bunker );
      switch (minor_mission) {
      case 1: /* order men with 'm' */
        { 
          int seen = 0;
          while (seen + troopers + defenders <= COCO_BOOT_CAMP_INFANTRY)
            {
              coco_sobject *man;
              man = coco_find_sobject_in_range( sstate, COCO_INFANTRY_CLASS,
                                                COCO_FALSE, seen+1, nat,
                                                pick_up_x0, pick_up_x1 );
              if (NULL == man) 
                break;
              else ++seen;
            }
          if (seen + troopers + defenders > COCO_BOOT_CAMP_INFANTRY)
            minor_mission = 2;
        }
        break;
      case 2: /* climb in */
        if (troopers + defenders > COCO_BOOT_CAMP_INFANTRY)
          minor_mission = 3;
        else
          { 
            coco_sobject *man;
            man = coco_find_sobject_in_range( sstate, COCO_INFANTRY_CLASS,
                                              COCO_FALSE, 1, nat,
                                              pick_up_x0, pick_up_x1 );
            if (man)
              {
                angle_on = COCO_TRUE;
                angle = coco_point_arrow_at_sobject( man, sstate,
                                                     cam_object_or_null, player_index );
              }
            else minor_mission = 1;
          }
        break;
#define COCO_FIND_AROUND_CC( n ) \
  coco_find_sobject_in_range( sstate, COCO_INFANTRY_CLASS, \
                              COCO_FALSE, \
                              n, \
                              nat, \
                              COCO_SOBJECT_X( bunker ) + 50, \
                              COCO_SOBJECT_X( bunker ) + 350)
      case 3: /* fly to CC */
      case 4: /* press space */
        angle_on = COCO_TRUE;
        angle = coco_point_arrow_at( COCO_SOBJECT_X( bunker ) + 200,
                                     COCO_GROUND_Y,
                                     sstate, cam_object_or_null,
                                     player_index );
        if (cam_object_or_null)
          {
            if (COCO_FIND_AROUND_CC( COCO_BOOT_CAMP_INFANTRY - defenders ))
              minor_mission = 5;
            else if ((0 < (COCO_BOOT_CAMP_INFANTRY - defenders - troopers))
                     && !COCO_FIND_AROUND_CC( COCO_BOOT_CAMP_INFANTRY 
                                              - defenders - troopers ))
              minor_mission = 1;
            else
              {
                coco_gnum delta
                  = COCO_SOBJECT_X( cam_object_or_null )
                  - COCO_SOBJECT_X( bunker ) - 200;
                if (abs( delta ) < 150)
                  minor_mission = 4;
                else minor_mission = 3;
              }
          }
        break;
      case 5: /* let them walk in */
        if (defenders == COCO_BOOT_CAMP_INFANTRY)
          minor_mission = 6;
        else if (!COCO_FIND_AROUND_CC( COCO_BOOT_CAMP_INFANTRY - defenders ))
          minor_mission = 1;
        break;
#undef COCO_FIND_AROUND_CC
      case 6: /* good job */
        {
          static coco_bool congratulations_on = COCO_FALSE;
          static coco_u32 congratulations_time;
          if (!congratulations_on)
            {
              congratulations_on = COCO_TRUE;
              congratulations_time
                = COCO_U32_ADD_N_MODULAR( sstate->time,
                                          3 * COCO_IDEAL_FRAMES_PER_SECOND );
            }
          else if (sstate->time == congratulations_time)
            {
              int i;
              coco_sobject *bunker 
                = coco_find_sobject( sstate, COCO_BUNKER_CLASS, 
                                     COCO_FALSE, 1 /* count */, -1 );
              coco_gnum bunker_x 
                = bunker ? COCO_SOBJECT_X( bunker ) : sstate->board_w>>1;
              for (i=0; i<3; i++)
                {
                  coco_gnum x = (bunker_x>>1) - i*120;
                  coco_sobject *sobject
                    = coco_sstate_allocate_sobject( sstate );
                  if (!sobject) goto escape;
                  if (i<2)
                    coco_init_tank( sobject, COCO_TANK_CLASS, x, enat );
                  else 
                    coco_init_firetruck( sobject, COCO_FIRETRUCK_CLASS, 
                                         x, enat );
                }
              major_mission = 2;
              minor_mission = 8;
            escape:
              ;
            }
        }
        break;
      };
    }
    break;
  case 2:
    switch (minor_mission) {
    case 6: /* good work! */
      {
        static coco_bool congratulations_on = COCO_FALSE;
        static coco_u32 congratulations_time;
        if (!congratulations_on)
          {
            congratulations_on = COCO_TRUE;
            congratulations_time
              = COCO_U32_ADD_N_MODULAR( sstate->time,
                                        3 * COCO_IDEAL_FRAMES_PER_SECOND );
          }
        else if (sstate->time == congratulations_time)
          {
            major_mission = 3;
            minor_mission = 11;
          }
      }
      break;
    case 8: /* kill tank */
    case 9: /* dodge!! */
    case 10: /* how to fire guns */
      {
        coco_sobject *missile
          = coco_find_sobject( sstate, COCO_MISSILE_CLASS,
                               COCO_TRUE, 1, enat );
        coco_sobject *bullet
          = coco_find_sobject( sstate, COCO_BULLET_CLASS,
                               COCO_TRUE, 1, enat );
        coco_sobject *tank
          = coco_find_sobject( sstate, COCO_TANK_CLASS, 
                               COCO_TRUE, 1 /* count */, enat );
        if (missile || bullet)
          {
            coco_cookie( "missile=%p bullet=%p\n", missile, bullet );
            minor_mission = 9;
          }
        else if (tank)
          {
            coco_gnum dx, dy;
            angle_on = COCO_TRUE;
            angle 
              = coco_point_arrow_at_sobject( tank, sstate, cam_object_or_null,
                                             player_index  );
            if (cam_object_or_null)
              {
                dx = COCO_SOBJECT_X( cam_object_or_null ) 
                  - COCO_SOBJECT_X( tank );
                dy = COCO_SOBJECT_Y( cam_object_or_null )
                  - COCO_SOBJECT_Y( tank );
                if (abs( dx ) + abs( dy ) < COCO_VIEWABLE_H)
                  minor_mission = 10;
                else minor_mission = 8;
              }
            else minor_mission = 8;
          }
        else
          {
            minor_mission = 6;
          }
      }
      break;
    }
    break;
  case 3:
    switch (minor_mission) {
    case 11: /* shoot the HQ */
      {
        coco_sobject *hq = coco_find_sobject( sstate, COCO_HEADQUARTERS_CLASS,
                                              COCO_TRUE, 1, enat );
        coco_sobject *mayhem;
        if (!hq) break; /* never happens */
        mayhem = coco_find_sobject_if( sstate, 
                                       coco_boot_camp_headquarters_mayhem,
                                       (void *) hq );
        if (mayhem)
          minor_mission = 12;
      }
      break;
    case 12:
    case 13:
    case 14:
      {
        coco_sobject *van = coco_find_sobject( sstate, COCO_DEMO_VAN_CLASS,
                                               COCO_TRUE, 1, nat );
        if (!coco_find_sobject( sstate, COCO_HEADQUARTERS_CLASS,
                                COCO_TRUE, 1, enat ))
          minor_mission = 14;
        else if (!van)
          minor_mission = 12;
        else
          minor_mission = 13;
      }
    }
    break;
  default:
    break;
  }

  /* hold HQ back */
  if (major_mission != 3)
    {
      coco_sobject *hq = coco_find_sobject( sstate, COCO_HEADQUARTERS_CLASS,
                                            COCO_TRUE, 1, enat );
      if (hq)
        COCO_SOBJECT_X( hq ) = 100;
    }

  {
    struct coco_tutorial_objectives to;
    to.major_mission = major_mission;
    to.minor_mission = minor_mission;
    to.angle_on = angle_on;
    to.angle = angle;
    return to;
  }
}


coco_bool
coco_sobject_locked_on( coco_sobject *sobject, coco_sstate *sstate )
{
  int index = COCO_SOBJECT_INDEX( sobject );
  COCO_DO_SSTATE_OBJECTS( missile, sstate,
  {
    if (coco_sobject_subtype_p( COCO_SOBJECT_CL( missile ),
                                COCO_MISSILE_CLASS )
        && (COCO_MISSILE_TARGET_INDEX( missile ) == index))
      return COCO_TRUE;
  });
  return COCO_FALSE;
}



void
coco_write_cstate_serial_header( struct coco_cstate_serial_header *str, 
                                 coco_sstate *sstate, 
                                 coco_sobject *cam_object_or_null,
                                 int p_or_c_index, coco_bool is_comp,
                                 int cobject_count,
                                 int cblip_count,
                                 coco_i32 cam_left,
                                 coco_i32 camera_index )
{
  coco_bool *summon_enable;
  coco_u32 *summon_disable_until;
  int enable, lock;

  coco_summon_places( cam_object_or_null, sstate, 
                      COCO_PORC_NATIONALITY( p_or_c_index, is_comp ),
                      &summon_enable, &summon_disable_until );

  enable = ((!(*summon_enable)) << COCO_DASHINFO_BIT_SUMMON_DISABLE);
  lock = cam_object_or_null 
    ? (coco_sobject_locked_on( cam_object_or_null, sstate )
       << COCO_DASHINFO_BIT_MISSILE_LOCK)
    : 0;

  str->time = sstate->time;
  str->cobject_count = cobject_count;
  str->cblip_count = cblip_count;
  str->cobject_camera_index = camera_index;
  str->game_over_nationality = sstate->game_over_nationality;
  str->cam_left = cam_left;
  str->board_w = sstate->board_w;
  str->dx = cam_object_or_null ? COCO_SOBJECT_DX( cam_object_or_null ) : 0;
  str->dy = cam_object_or_null ? COCO_SOBJECT_DY( cam_object_or_null ) : 0;
  str->major_mission = sstate->tutorial_objectives.major_mission;
  str->minor_mission = sstate->tutorial_objectives.minor_mission;
  str->angle_on      = sstate->tutorial_objectives.angle_on;
  str->angle         = sstate->tutorial_objectives.angle;


  if (cam_object_or_null 
      && coco_sobject_subtype_p( COCO_SOBJECT_CL( cam_object_or_null ),
                                 COCO_COPTER_CLASS ))
    {
      str->dashinfo.bullets      = COCO_COPTER_BULLETS( cam_object_or_null );
      str->dashinfo.bombs        = COCO_COPTER_BOMBS( cam_object_or_null );
      str->dashinfo.missiles     = COCO_COPTER_MISSILES( cam_object_or_null );
      str->dashinfo.rockets      = COCO_COPTER_ROCKETS( cam_object_or_null );
      str->dashinfo.plasma       = 0;
      str->dashinfo.paratroopers = COCO_COPTER_PARATROOPERS( cam_object_or_null );
      str->dashinfo.engitroopers = COCO_COPTER_ENGITROOPERS( cam_object_or_null );
      str->dashinfo.fuel         = COCO_COPTER_FUEL( cam_object_or_null );
      str->dashinfo.kits         = COCO_COPTER_KITS( cam_object_or_null );
      str->dashinfo.bits         = enable | lock;
    }
  else if (cam_object_or_null
           && coco_sobject_subtype_p( COCO_SOBJECT_CL( cam_object_or_null ),
                                      COCO_MECH_CLASS ))
    {
      str->dashinfo.bullets      = 0;
      str->dashinfo.bombs        = 0;
      str->dashinfo.missiles     = 0;
      str->dashinfo.rockets      = 0;
      str->dashinfo.plasma       = COCO_MECH_PLASMA( cam_object_or_null );
      str->dashinfo.paratroopers = 0;
      str->dashinfo.engitroopers = 0;
      str->dashinfo.fuel         = COCO_MECH_ENERGY( cam_object_or_null );
      str->dashinfo.kits         = 0;
      str->dashinfo.bits         = enable | lock;
    }
  else
    {
      str->dashinfo.bullets = str->dashinfo.bombs = str->dashinfo.missiles
        = str->dashinfo.rockets = str->dashinfo.paratroopers
        = str->dashinfo.fuel = str->dashinfo.kits = 0;
      str->dashinfo.bits = enable;
    }
  str->dashinfo.copters = COCO_PORC_COPTERS( p_or_c_index, is_comp );
  str->dashinfo.funds   = COCO_PORC_FUNDS( p_or_c_index, is_comp );
  str->dashinfo.server_fps = COCO_IDEAL_FRAMES_PER_SECOND;
  str->dashinfo.fancy_class_index 
    = COCO_PORC_FAVORITE_COPTER_CLASS( p_or_c_index, is_comp )->cl_index;
  /* = sstate->favorite_copter_class[p_or_c_index]->cl_index; */
  str->dashinfo.camera_class_index
    = cam_object_or_null ? COCO_SOBJECT_CL_INDEX( cam_object_or_null ) : -1;
}


void
coco_cam_bounds( coco_sstate *sstate, coco_sobject *cam_object, /* possibly null */ 
                 int p_or_c_index,
                 coco_bool is_comp,
                 coco_gnum *cam_left,
                 coco_gnum *cam_right )
{
  int cam_x;
  if (cam_object == NULL)
    {
      if (is_comp)
        { /* they don't need to see anything */
          *cam_right = 0;
          *cam_left = sstate->board_w; 
        }
      else
        {
          coco_gnum dummy;
          coco_copter_start_coords( sstate,
                                    coco_players[p_or_c_index].nationality,
                                    p_or_c_index, 
                                    &cam_x, &dummy );
          /* cam_left is still a little wrong. */
          *cam_left = cam_x - (COCO_OFFICIAL_WINDOW_W>>1);
          *cam_right = *cam_left + COCO_OFFICIAL_WINDOW_W;
        }
    }
  else
    {
      coco_gnum xmid, ymid;
      cam_x = COCO_SOBJECT_X( cam_object );
      coco_center_of_mass_obsolete( cam_object, COCO_SOBJECT_CL( cam_object ),
                                    COCO_SOBJECT_RASTER_INDEX( cam_object ),
                                    &xmid, &ymid );
      /* this x calculation is duplicated sorta in copter_kaboom, for
         camera placement. */
      /* also kinda in mech_update_state */
      *cam_left = cam_x 
        + xmid - (COCO_OFFICIAL_WINDOW_W>>1)
        + coco_camera_velocity_offset( cam_object, 
                                       COCO_SOBJECT_CL( cam_object ),
                                       sstate );
      *cam_right = *cam_left + COCO_OFFICIAL_WINDOW_W;

    }
}


/* return a pointer to an internal buffer of client state to send */
/* p_or_c_index is either player index or comp index, depending on is_comp */
void *
coco_format_sstate_to_buffer( coco_sstate *sstate, int p_or_c_index, 
                              coco_bool is_comp, coco_bool network_byte_order,
                              size_t *buffer_size, /* written nonnull */
                              coco_cobject **cobjects, /* written nonnull */
                              struct coco_cblip **cblips ) /* written nonnull */
{
  static void *send_buffer = NULL;
  static int send_buffer_size = 0;

  int send_buffer_desired_size;
  coco_gnum board_w = sstate->board_w;
  int cam_idx;
  coco_i32 client_cam_idx = -1;
  coco_sobject *cam_object;
  coco_u32 cobject_count = 0;
  coco_u32 cblip_count = 0;
  coco_gnum cam_left, cam_right;

  cam_object = coco_find_camera( sstate, p_or_c_index, is_comp );
  cam_idx = cam_object ? COCO_SOBJECT_INDEX( cam_object ) : -1;

  coco_cam_bounds( sstate, cam_object, p_or_c_index, is_comp, &cam_left, &cam_right );

  /* calculate how much room is needed */
  COCO_DO_SSTATE_OBJECTS( sobject, sstate,
  {
    /* this counting test must be synced with test below */
    coco_blip_index ci;
    if (coco_sobject_is_visible( sobject, COCO_SOBJECT_CL( sobject ), sstate,
                                 cam_left, cam_right, cam_object ))
      {
        if ((cam_idx >= 0) && (COCO_SOBJECT_INDEX( sobject ) == cam_idx))
          client_cam_idx = cobject_count;
        cobject_count++;
      }
    ci = coco_sobject_blip_color_index( sobject, COCO_SOBJECT_CL( sobject ), 
                                        p_or_c_index, is_comp, sstate );
    /* initialize scratch blip cache */
    COCO_SOBJECT_SCRATCH_BLIP( sobject ) = ci;
    /* Only written if its visible */
    if (COCO_BLIP_TRANSPARENT != ci) 
      cblip_count++;
  });
  
  send_buffer_desired_size
    = COCO_CSTATE_BUFFER_SIZE( cobject_count, cblip_count );
  if (send_buffer_desired_size > send_buffer_size)
    { /* maybe grow send buffer */
      int requested_size = COCO_MAX( send_buffer_desired_size + 200, 1024 );
      send_buffer_size = requested_size;
      coco_cookie( "Reallocing (%p + 0x%x)", send_buffer, requested_size );
      send_buffer = coco_realloc( send_buffer, requested_size );
    }

  coco_write_cstate_serial_header( send_buffer, sstate, cam_object,
                                   p_or_c_index, is_comp,
                                   cobject_count, cblip_count, cam_left,
                                   client_cam_idx );
  if (network_byte_order)
    coco_cstate_serial_header_hton( send_buffer );

  {
    struct coco_cblip *cblip_ptr;
    coco_cobject *cobject_ptr
      = (coco_cobject *)
      (((struct coco_cstate_serial_header *) send_buffer) + 1);
    if (cobjects) *cobjects = cobject_ptr;
    /* OBJECTS */
    COCO_DO_SSTATE_OBJECTS( sobject, sstate, 
    {
      /* this writing test must be synced with test above */
      if (coco_sobject_is_visible( sobject, COCO_SOBJECT_CL( sobject ), sstate,
                                   cam_left, cam_right, cam_object ))
        {
          coco_sobject_to_cobject( sobject, COCO_SOBJECT_CL( sobject ),
                                   cobject_ptr, cam_left, network_byte_order );
          cobject_ptr++;
        }
    });
    /* BLIPS */
    cblip_ptr = (struct coco_cblip *) cobject_ptr;
    if (cblips) *cblips = cblip_ptr;
    COCO_DO_SSTATE_OBJECTS( sobject, sstate, 
    {
      coco_blip_index ci = COCO_SOBJECT_SCRATCH_BLIP( sobject );
      /* cheating clients might read "transparent" thing's coordinates, and
         gain classified info.  To prevent that we'll not send them. */
      if (ci != COCO_BLIP_TRANSPARENT)
        {
          coco_rxnum x;
          coco_rynum y;
          coco_mini_point mp;
          coco_sobject_cl *class1 = COCO_SOBJECT_CL( sobject );
          unsigned int ri = COCO_SOBJECT_RASTER_INDEX( sobject );
          mp = coco_sobject_com_by_raster_index( class1, ri );
          cblip_ptr->color_index = ci;
          x = COCO_GNUM_TO_RXNUM( COCO_SOBJECT_X( sobject ) 
                                  + COCO_MINI_POINT_X( mp ),
                                  board_w );
          y = COCO_GNUM_TO_RYNUM( COCO_SOBJECT_Y( sobject )
                                  + COCO_MINI_POINT_Y( mp ) );
          cblip_ptr->x = network_byte_order ? COCO_RXNUM_HTON( x ) : x;
          cblip_ptr->y = network_byte_order ? COCO_RYNUM_HTON( y ) : y;
          cblip_ptr++;
        }
    });
  }

  /*
  coco_cookie( "Turn %d: sending %d objects and %d blips to \"%s\".\n", 
               sstate->time, cobject_count, cblip_count,
               coco_players[p_or_c_index].name ); */
  if (buffer_size) *buffer_size = send_buffer_desired_size;
  return send_buffer;
}


void
coco_add_funds( int p_or_c_index, coco_bool is_comp, float funds )
{
  if (p_or_c_index >= 0)
    {
      if (is_comp)
        coco_comps[p_or_c_index].funds += funds;
      else 
        coco_players[p_or_c_index].funds += funds;
    }
}

coco_bool
coco_subtract_funds_available( int player_index, coco_bool is_comp, 
                               float funds )
{
  if ((!is_comp)
      && (player_index >= 0)
      && coco_players[player_index].in_use
      && (coco_players[player_index].funds >= funds))
    {
      coco_players[player_index].funds -= funds;
      return COCO_TRUE;
    }
  else if (is_comp
           && (player_index >= 0)
           && coco_comps[player_index].in_use
           && (coco_comps[player_index].funds >= funds))
    {
      coco_comps[player_index].funds -= funds;
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}

/* handy higher-level constructor */
void
coco_make_snd( coco_sound_type sound, coco_sstate *sstate, 
               coco_gnum x, coco_gnum y )
{
  coco_sobject *sobject;
  sobject = coco_sstate_allocate_sobject( sstate );
  if (sobject)
    coco_init_sndobject( sobject, COCO_SNDOBJECT_CLASS, x, y, sound );
}



coco_bool
coco_sobject_is_obscured( coco_sobject *sobject, coco_sstate *sstate )
{
  coco_gnum xx0, yy0, xx1, yy1;
  coco_znum zz;
  int ii;
  xx0 = COCO_SOBJECT_X( sobject );
  yy0 = COCO_SOBJECT_Y( sobject );
  xx1 = xx0 + COCO_SOBJECT_W( sobject );
  yy1 = yy0 + COCO_SOBJECT_H( sobject );
  zz = COCO_SOBJECT_Z( sobject );
  ii = COCO_SOBJECT_INDEX( sobject );
  /* couldn't use COCO_DO_SSTATE_OBJECTS cuz of bugs */
  { 
    coco_sobject *sobj = sstate->sobjects;
    coco_sobject *coco_sobject_end = sobj + sstate->sobject_size;
    for (; sobj < coco_sobject_end; sobj++) 
      if (COCO_SOBJECT_IN_USE( sobj ))
        {
          coco_gnum x0, y0, x1, y1;
          coco_znum z;
          int i;
          x0 = COCO_SOBJECT_X( sobj );
          y0 = COCO_SOBJECT_Y( sobj );
          x1 = x0 + COCO_SOBJECT_W( sobj );
          y1 = y0 + COCO_SOBJECT_H( sobj );
          z = COCO_SOBJECT_Z( sobj );
          i = COCO_SOBJECT_INDEX( sobj );
          if (((z > zz) || ((z == zz) && (i>ii)))
              &&
              (((x0 <= xx0) && (x1 >= xx0)) || ((x0 <= xx1) && (x1 >= xx1)))
              && 
              (((y0 <= yy0) && (y1 >= yy0)) || ((y0 <= yy1) && (y1 >= yy1))))
            {
              if ((x0 <= xx0) && (x1 >= xx1)
                  &&
                  (y0 <= yy0) && (y1 >= yy1))
                return COCO_TRUE;
            }
        }
  };
  return COCO_FALSE;
}


coco_gnum 
coco_copter_velocity_offset_old_skool( coco_sobject *camera, 
                                       coco_sobject_cl *cla,
                                       coco_sstate *sstate )
{
  /* was COCO_SOBJECT_DX_MAX( camera ), but that makes retreating cargo
     copters effectively blind: */
  coco_gnum dx_max = COCO_COPTER_DX_MAX; 
  return (dx_max>0)
    ? (COCO_SOBJECT_DX( camera ) * (COCO_OFFICIAL_WINDOW_W>>2) / dx_max)
    : 0;
}

