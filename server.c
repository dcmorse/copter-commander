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

#include <sys/time.h>           /* for select */
#include <sys/types.h>          /* for select */
#include <unistd.h>             /* for select */
#include <sys/types.h>          /* for socket() */
#include <sys/socket.h>         /* for socket() */
#include <assert.h>             /* for struct protoent */
#include <netinet/in.h>         /* i guess this is how i get sockaddr_in */
#include <netdb.h>       
#include <errno.h>
#include <stdlib.h>             /* for abs, rand */
#include <string.h>             /* for memcpy */
#include "coco.h"
#include "rasters.h"       /* for raster dimensions */

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

static coco_sstate the_coco_sstate = { COCO_FALSE, NULL, 0, 0, 0 };

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
    coco_sobject *  sobject  = sstate->sobjects;
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
coco_schedule_comp_respawn( int comp_index, coco_sstate *sstate )
{
  sstate->comp_respawn[comp_index] = COCO_TRUE;
  sstate->comp_respawn_times[comp_index]
    = COCO_U32_ADD_N_MODULAR( sstate->time, 
                              COCO_IDEAL_FRAMES_PER_SECOND
                              * COCO_RESPAWN_SECONDS );
  coco_cookie( "scheduling comp %d respawn\n", comp_index );
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
    coco_schedule_comp_respawn( comp_index, sstate );
}

coco_sstate *
coco_init_sstate( coco_sstate *sstate, coco_gnum width )
{
  int i;
  for (i=0; i < sstate->sobject_size; i++)
    COCO_SOBJECT_IN_USE( COCO_SSTATE_OBJECT_REF( sstate, i ) ) = COCO_FALSE;
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
  return sstate;
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
  if (-1==bind( listenfd, &sockaddr, sizeof( sockaddr ) ))
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
          ||(!coco_write_message( str, (union coco_message *) coco_message_game( i ))))
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

void
coco_init_comps( void )
{
  int i;
  for (i=0; i<COCO_MAX_COMPS; i++)
    coco_comps[i].in_use = COCO_FALSE;
}


void
coco_add_comp( coco_nationality nat, coco_sstate *sstate )
{
  static char *comp_names[] = {
    "Pik Pocket",
    "BioBear",
    "Cyber Chicken",
    "Damage Jackal",
    "Armor Fiend",
    "Magna Mauler",
    "HotPoker",
    "The Road Warrior",
    "Mr Funky",
    "HammerTime",
    "Beastly Thing",
    "WitchDoctor",
    "Count Chocula",
    "Funky Monkey",
    "Krusty",
  };
  coco_bool found = COCO_FALSE;
  int i; 
  for (i=0; i<COCO_MAX_COMPS; i++)
    if (!coco_comps[i].in_use)
      {
        char *name = comp_names[rand()
                               % (sizeof( comp_names )/sizeof( char * ))];
        coco_comps[i].nationality = nat;
        coco_comps[i].funds = 0;
        coco_comps[i].copters = 0;
        strncpy( coco_comps[i].name, name, COCO_PLAYER_NAME_MAX-1 );
        coco_comps[i].name[COCO_PLAYER_NAME_MAX-1] = '\0';
        coco_comps[i].in_use = COCO_TRUE;
        coco_schedule_comp_respawn( i, sstate );
        found = COCO_TRUE;
        break;
      }
  if (!found)
    coco_warn_printf( "Could not add comp because there are already %d\n",
                      COCO_MAX_COMPS );
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

      cursor = ++(sstate->nominations_cursor);
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
      switch (code) {
      case COCO_NOMINATE_ADD_GOOD_GUY_COMP:
      case COCO_NOMINATE_ADD_BAD_GUY_COMP:
        { 
          coco_nationality nat = (code==COCO_NOMINATE_ADD_GOOD_GUY_COMP)
            ? COCO_GOOD_GUYS : COCO_BAD_GUYS;
          coco_add_comp( nat, sstate );
        }
        break;
      case COCO_NOMINATE_BOOT_COMP:
        coco_warn_printf( "Comp booting is not implemented.\n" );
        break;
      default:
        coco_warn_printf( "Unrecognised code in "
                          "coco_handle_nominate_message: %d.\n", code );
        break;
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
        coco_handle_nominate_message( &(message.nominate), coco_get_sstate(),
                                      player_index );
        break;
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
  if (need_game_refresh)
    return coco_send_everyone_game_message();
  return COCO_TRUE; 
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

/* COLLISION OVERHAUL
coco_bool
coco_execute_building_collision_maybe( coco_sstate *sstate,
                                       coco_sobject *building_or_whatever,
                                       coco_sobject *obj2, 
                                       coco_bool *building_died_p )
{
  coco_sobject_cl *bcl = COCO_SOBJECT_CL( building_or_whatever );
  coco_sobject_cl *o2cl = COCO_SOBJECT_CL( obj2 );

  *building_died_p = COCO_FALSE;
  if (coco_sobject_subtype_p( bcl, COCO_BUILDING_CLASS )
      && coco_sobject_subtype_p( o2cl, COCO_GROUND_UNIT_CLASS ))
    if (coco_sobject_subtype_p( bcl, COCO_HEADQUARTERS_CLASS )
        && coco_sobject_subtype_p( o2cl, COCO_DEMO_VAN_CLASS )
        && (COCO_NEUTRAL == sstate->game_over_nationality))
      {
        sstate->game_over_nationality = COCO_SOBJECT_NATIONALITY( obj2 );
        sstate->game_over_time
          = COCO_U32_ADD_N_MODULAR( sstate->time, 
                                    IDEAL_FRAMES_PER_SECOND<<4 );
        COCO_SOBJECT_HEALTH( obj2 ) = COCO_NIGH_INVULNERABLE;
        coco_kaboom( building_or_whatever, 
                     COCO_SOBJECT_CL( building_or_whatever ),
                     sstate );
        *building_died_p = COCO_TRUE;
        return COCO_TRUE;
      }
    else return COCO_TRUE;
  else return COCO_FALSE;
} */



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
  else coco_friendly_collision( obja, COCO_SOBJECT_CL( obja ), objb,
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
  struct coco_raster *raster = COCO_SOBJECT_RASTER( sobject, nri );
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


void
coco_copter_smoke_maybe( coco_sobject *copter, coco_sstate *sstate )
{
  coco_mini_rectangle hp, rp;
  unsigned char ri;
  coco_sobject *smoke;
  struct coco_raster *smoke_raster;
  int turn_mod_6;
  int smokelevel = COCO_SOBJECT_HEALTH_MAX( copter ) 
    - COCO_SOBJECT_HEALTH( copter );
  if (smokelevel <= 0) return;
  turn_mod_6 = sstate->time % 6;
  if (turn_mod_6 != 0) return;
  if (smokelevel > COCO_SMOKE_RASTER_COUNT)
    {
      coco_warn_printf( "too much damage not enough smoke rasters\n" );
      smokelevel = COCO_SMOKE_RASTER_COUNT;
    }
  smoke = coco_sstate_allocate_sobject( sstate );
  if (!smoke) return;
  smoke_raster = COCO_SMOKE_CLASS->rasters[smokelevel - 1];
  ri = COCO_SOBJECT_RASTER_INDEX( copter );
  hp = COCO_SOBJECT_WEAPON_HARDPOINTS( copter )[ri];
  rp = COCO_SOBJECT_ROTOR_POINTS( copter )[ri];
  /* emit smoke between hardpoint and rotorpoint. */
  coco_init_smoke( smoke, COCO_SMOKE_CLASS, 
                   COCO_SOBJECT_X( copter ) + ((COCO_MINI_POINT_X( hp )
                                                + COCO_MINI_POINT_X( rp ))>>1)
                   - (smoke_raster->w>>1),
                   COCO_SOBJECT_Y( copter ) + ((COCO_MINI_POINT_Y( hp )
                                                + COCO_MINI_POINT_Y( rp ))>>1)
                   - (smoke_raster->h>>1),
                   smokelevel - 1 );
}


/* return true if something was summoned */
coco_bool
coco_summon_unit_maybe( coco_sstate *sstate, coco_sobject_cl *cl, 
                        coco_sobject *(* init_fn)
                        ( coco_sobject *, coco_sobject_cl *, 
                          coco_gnum, coco_nationality),
                        coco_nationality nationality,
                        int player_idx )
{
  struct coco_team *team = sstate->teams + nationality - COCO_GOOD_GUYS;
  if (team->summon_enable 
      && coco_subtract_funds_available( player_idx, cl->cost ))
    {
      coco_sobject *unit = coco_sstate_allocate_sobject( sstate );
      if (unit)
        {
          coco_u32 delta_t;
          init_fn( unit, cl, (COCO_GOOD_GUYS==nationality)
                   ? (sstate->board_w + COCO_GROUND_UNIT_WARP_IN_X)
                   : (-COCO_GROUND_UNIT_WARP_IN_X - cl->canonical_w),
                   nationality );
          delta_t = COCO_GROUND_UNIT_DX_MAX * (COCO_SOBJECT_W( unit ) + 5);
          team->summon_enable = COCO_FALSE;
          team->summon_disable_until
            = COCO_U32_ADD_N_MODULAR( sstate->time, delta_t );
          return COCO_TRUE;
        }
      else
        { /* refund the purchase price */
          coco_add_funds( player_idx, cl->cost );
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
  struct coco_keyboard_pointer_state *input 
    = coco_remote_input_states + COCO_SOBJECT_CAMERA_INDEX( sobject );
  int i;
  for (i=0; i<COCO_UNIT_ORDER_KEYMAP_LENGTH; i++)
    if (coco_get_bit( input->key_state, bindings[i].key ))
      coco_summon_unit_maybe( sstate, bindings[i].cl, bindings[i].init_fn,
                              COCO_SOBJECT_NATIONALITY( sobject ),
                              COCO_SOBJECT_CAMERA_INDEX( sobject ));
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



coco_bool 
coco_copter_update_state( coco_sobject *sobject, 
                          coco_sobject_cl *cl, 
                          coco_sstate *sstate )
{
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
        coco_fire_bullet( sobject, COCO_SOBJECT_CL( sobject ), sstate, NULL );
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
  coco_copter_smoke_maybe( sobject, sstate );
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
    coco_gnum dx = COCO_SOBJECT_DX( sobject );
    coco_gnum dx_max = COCO_SOBJECT_DX_MAX( sobject );
    coco_gnum ddx_raw = COCO_SOBJECT_X_FORCE( sobject );
    coco_gnum dx_goal
      = (coco_gnum) (((float) dx_max) * px
                     / ((float) (COCO_OFFICIAL_WINDOW_W>>1)));
    /* accell takes 2x as long as decel */
    coco_gnum ddx_vectored = ((dx_goal <= 0) == (dx <= 0))
      ? (ddx_raw>>1)
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
    ? (sstate->favorite_copter_class[player_index]) : COCO_COBRA_CLASS;
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
          coco_gnum x, y;
          coco_sobject_cl *fcc;
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
            coco_subtract_funds_available( i, COCO_BUBBLE_COPTER_CLASS->cost );
          fcc = spare_copter_p 
            ? sstate->favorite_copter_class[i]
            : COCO_BUBBLE_COPTER_CLASS;
          sstate->camera_indicies[i] 
            = COCO_SOBJECT_INDEX
            (coco_init_copter( coco_sstate_allocate_sobject( sstate ),
                               fcc,
                               i, (COCO_GOOD_GUYS==nat)?-1:1, 
                               x,
                               y, nat ));
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

          coco_cookie( "respawning for comp %d\n" );

          sstate->comp_respawn[i] = COCO_FALSE;
          if (spare_copter_p)
            coco_comps[i].copters--;
          else
            coco_comps[i].funds
              = COCO_MIN( coco_comps[i].funds
                          - COCO_BUBBLE_COPTER_CLASS->cost, 
                          0 );
          fcc = spare_copter_p ? COCO_COBRA_CLASS : COCO_BUBBLE_COPTER_CLASS;
          coco_copter_start_coords( sstate, nat, -1, &x, &y );
          copter = coco_sstate_allocate_sobject( sstate );
          if (NULL==copter)
            { /* put it off and try to recover. */
              sstate->comp_respawn[i] = COCO_TRUE;
              sstate->comp_respawn_times[i] =
                COCO_U32_ADD_MODULAR( sstate->time );
              coco_warn_printf( "not enough memory to respawn comp\n" );
            }
          coco_init_copter( copter, fcc,
                            -1, (COCO_GOOD_GUYS==nat)?-1:1, 
                            x, y, nat );
          COCO_SOBJECT_COMP_INDEX( copter ) = i;
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
      int i;
      for (i=0; i<COCO_MAX_PLAYERS; i++)
        if (coco_players[i].in_use && sstate->camera_indicies[i] >= 0)
          {
            coco_sobject *cam
              = COCO_SSTATE_OBJECT_REF( sstate, sstate->camera_indicies[i] );
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
              coco_add_funds( i, 0.25 );
            else if (passed_targ_p)
              coco_add_funds( i, 0.0675 );
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
                          &sockaddr, sizeof( sockaddr ) );
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


void coco_server_key_press( int player_idx, coco_key key, coco_sstate *sstate )
{
  int index = sstate->camera_indicies[player_idx];
  if (-1 != index)
    {
      coco_sobject *cam = COCO_SSTATE_OBJECT_REF( sstate, index );
      switch (key){
      case COCO_LPOINTER: 
        if (cam
            && coco_sobject_subtype_p( COCO_SOBJECT_CL( cam ), 
                                       COCO_COPTER_CLASS )
            && ! COCO_COPTER_LANDED_P( cam ))
          coco_toggle_facing( cam, COCO_SOBJECT_CL( cam ) );
        break;
      case COCO_CHAR_TO_KEY( 'h' ):
        {
          if (coco_subtract_funds_available( player_idx,
                                             sstate->favorite_copter_class
                                             [player_idx]->cost))
            coco_players[player_idx].copters++;
        }
      default:
        break;
      }
    }
}

void coco_server_key_release( int player_idx, coco_key key,
                              coco_sstate *sstate )
{
  /* this function left intentionally blank, since nothing uses it now.  */
}


void
coco_read_client_packet( int udpfd, coco_sstate *sstate )
{
  struct sockaddr_in sockaddr;
  struct coco_client_to_server_packet pkt;
  int recv_result, plr_idx;
  unsigned int sockaddr_size = sizeof( sockaddr );

  while (1) {
    recv_result = recvfrom( udpfd, &pkt, sizeof( pkt ), 0,
                            &sockaddr, &sockaddr_size ); 
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
                              coco_server_key_press( plr_idx, key, sstate );
                            coco_set_bit( oris->key_state, key, 1 );
                          }
                      }
                    else /* !coco_get_bit( ris... ) */
                      {
                        if (coco_get_bit( oris->key_state, key ))
                          {
                            if (sstate->game_in_progress)
                              coco_server_key_release( plr_idx, key, sstate );
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
                                 buf, cobjs, cblips, 
                                 &(coco_comps[i].kpstate) );
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
  coco_sstate *sstate_dummy;
  assert( coco_level_text );
  level_error = coco_load_level_definition( coco_level_text, &sstate_dummy );
  if (level_error)
    coco_fatal_printf( COCO_ERROR_BAD_LEVEL,
                       "Problem loading level \"%s\"\n",
                       level_error );
  coco_init_comps();
  coco_server_setup( &tcpfd, &out_udpfd, &in_udpfd, &in_udp_port );
  coco_server_loop( tcpfd, out_udpfd, in_udpfd, in_udp_port );
}


/* _Bottling up the game state for udp___________________________________ */


void
coco_metal_shrapnel_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                                coco_cobject *cobject, coco_gnum cam_left,
                                coco_bool nbo )
{
  /* call-next-method */
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  cobject->metal_shrapnel.color = COCO_METAL_SHRAPNEL_COLOR_INDEX( sobject );
}

void
coco_sndobject_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                           coco_cobject *cobject, coco_gnum cam_left,
                           coco_bool nbo )
{
  /* call-next-method */
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  cobject->sound.sound_type = COCO_SNDOBJECT_SOUND( sobject );
}


void
coco_y_angle_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                         coco_cobject *cobject, coco_gnum cam_left,
                         coco_bool nbo )
{
  /* call-next-method */
  coco_sobject_to_cobject( sobject, cl->super, cobject, cam_left, nbo );
  cobject->a.angle = COCO_SOBJECT_ANGLE( sobject );
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
coco_write_cstate_serial_header( struct coco_cstate_serial_header *str, 
                                 coco_sstate *sstate, 
                                 coco_sobject *cam_object_or_null,
                                 int player_index,
                                 int cobject_count,
                                 int cblip_count,
                                 coco_i32 cam_left )
{
  str->time = sstate->time;
  str->cobject_count = cobject_count;
  str->cblip_count = cblip_count;
  str->game_over_nationality = sstate->game_over_nationality;
  str->cam_left = cam_left;
  str->board_w = sstate->board_w;
  if (cam_object_or_null 
      && coco_sobject_subtype_p( COCO_SOBJECT_CL( cam_object_or_null ),
                                 COCO_COPTER_CLASS ))
    {
      str->dashinfo.bullets      = COCO_COPTER_BULLETS( cam_object_or_null );
      str->dashinfo.bombs        = COCO_COPTER_BOMBS( cam_object_or_null );
      str->dashinfo.missiles     = COCO_COPTER_MISSILES( cam_object_or_null );
      str->dashinfo.rockets      = COCO_COPTER_ROCKETS( cam_object_or_null );
      str->dashinfo.paratroopers = COCO_COPTER_PARATROOPERS( cam_object_or_null );
      str->dashinfo.engitroopers = COCO_COPTER_ENGITROOPERS( cam_object_or_null );
      str->dashinfo.fuel         = COCO_COPTER_FUEL( cam_object_or_null );
    }
  else
    str->dashinfo.bullets = str->dashinfo.bombs = str->dashinfo.missiles
      = str->dashinfo.rockets = str->dashinfo.paratroopers
      = str->dashinfo.fuel = 0;
  str->dashinfo.copters = coco_players[player_index].copters;
  str->dashinfo.funds   = coco_players[player_index].funds;
  str->dashinfo.server_fps = COCO_IDEAL_FRAMES_PER_SECOND;
  str->dashinfo.camera_class_index
    = cam_object_or_null ? COCO_SOBJECT_CL_INDEX( cam_object_or_null ) : -1;
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
  coco_sobject *cam_object;
  coco_u32 cobject_count = 0;
  coco_u32 cblip_count = 0;
  coco_gnum cam_x, cam_left, cam_right;

  if (is_comp)
    {
      cam_idx = -1;
      COCO_DO_SSTATE_OBJECTS( sobject, sstate, {
        if (p_or_c_index == COCO_SOBJECT_COMP_INDEX( sobject ))
          {
            cam_idx = COCO_SOBJECT_INDEX( sobject );
            goto done; /* don't use break inside DO_SSTATE_OBJECTS */
          }
      });
    done:
      ;
    }
  else cam_idx = sstate->camera_indicies[p_or_c_index]; 
  cam_object = (-1==cam_idx)
    ? NULL
    : COCO_SSTATE_OBJECT_REF( sstate, cam_idx );

  if (cam_object == NULL)
    {
      if (is_comp)
        { /* they don't need to see anything */
          cam_x = cam_right = 0;
          cam_left = board_w; 
        }
      else
        {
          coco_gnum dummy;
          coco_copter_start_coords( sstate,
                                    coco_players[p_or_c_index].nationality,
                                    p_or_c_index, 
                                    &cam_x, &dummy );
          /* cam_left is still a little wrong. */
          cam_left = cam_x - (COCO_OFFICIAL_WINDOW_W>>1);
          cam_right = cam_left + COCO_OFFICIAL_WINDOW_W;
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
      cam_left = cam_x 
        + xmid - (COCO_OFFICIAL_WINDOW_W>>1)
        + coco_camera_velocity_offset( cam_object, 
                                       COCO_SOBJECT_CL( cam_object ) );
      cam_right = cam_left + COCO_OFFICIAL_WINDOW_W;

    }
  /* calculate how much room is needed */
  COCO_DO_SSTATE_OBJECTS( sobject, sstate,
  {
    /* this counting test must be synced with test below */
    coco_blip_index ci;
    if (coco_sobject_is_visible( sobject, COCO_SOBJECT_CL( sobject ), 
                                 cam_left, cam_right, cam_object ))
      cobject_count++;
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
                                   p_or_c_index,
                                   cobject_count, cblip_count, cam_left );
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
      if (coco_sobject_is_visible( sobject, COCO_SOBJECT_CL( sobject ),
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
          cblip_ptr->color_index = ci;
          x = COCO_SCREEN_TO_RADAR_X( COCO_SOBJECT_X( sobject ) );
          y = COCO_SCREEN_TO_RADAR_Y( COCO_SOBJECT_Y( sobject ) );
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
coco_add_funds( int player_index, float funds )
{
  if (player_index >= 0)
    coco_players[player_index].funds += funds;
}

coco_bool
coco_subtract_funds_available( int player_index, float funds )
{
  if ((player_index >= 0)
      && coco_players[player_index].in_use
      && (coco_players[player_index].funds >= funds))
    {
      coco_players[player_index].funds -= funds;
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

