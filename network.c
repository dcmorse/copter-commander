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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
/* #include <net/netinet.h>     doesn't have sockaddr_in */
#include <netinet/in.h>         /* i guess this is how i get sockaddr_in */
#include <errno.h>
#include <assert.h>
#include <unistd.h>             /* fcntl and more */
#include <fcntl.h>
#include <stdlib.h>             /* getenv */
#include <string.h>             /* memcpy */
#include "coco.h"

/* _VARIABLES______________________________________________________ */

/* tcp port on which we'll go looking for games (client), or starting
   games (server) */
int coco_port = 7500; /* not unsigned short to appease popt */



/* _NETWORK_UTILITY_FUNCTIONALITY__________________________________ */

/* name may be "tcp" or "udp". */
struct protoent *
coco_get_protocol( char *name )
{
  struct protoent *proto;
  proto = getprotobyname( name );
  if (NULL==proto)
    coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                       "getprotobyname( \"%s\" ) returned 0.\n", name );
  return proto;
}


/* shutdown_too should be true if its a connected (tcp) stream */
void 
coco_close_fd( int fd, coco_bool shutdown_too )
{
  if (fd >= 0)
    {
      if (shutdown_too && (shutdown( fd, SHUT_RDWR) == -1))
        coco_warn_printf( "shutdown failed with errno=%d: %s\n", errno, 
                          coco_errno_string( errno ) );
      if (-1==close( fd ))
        coco_warn_printf( "close failed with errno=%d: %s\n", errno, 
                          coco_errno_string( errno ) );
    }
  else coco_warn_printf( "Attempt to coco_close_fd( %d, %s )\n",
                         fd, shutdown_too ? "COCO_TRUE" : "COCO_FALSE" );
}

/* returns the old blockingness, NB we only honor blockingness of messages
   on read.  On write, its temporarily disabled, then reenabled. */
coco_bool 
coco_set_fd_blocking( int fd, coco_bool block )
{
  int flags, had_block;
  block = !!block;              /* make sure its 1 or 0 */
  flags = fcntl( fd, F_GETFL );
  if (-1==flags)
    {
      coco_warn_printf( "fcntl read failed with errno=%d: %s\n", errno, 
                         coco_errno_string( errno ) );
      errno = 0;
      flags = 0;
    }
  had_block = !(flags & O_NONBLOCK);
  if (had_block == block)
    return block;
  else if (-1 == fcntl( fd, F_SETFL,
                        block ? (flags - O_NONBLOCK) : (flags | O_NONBLOCK) ))
    {
      coco_warn_printf( "fifo will not be non-blocking.\n"
                         "fcntl write failed with errno=%d: %s\n", errno, 
                         coco_errno_string( errno ) );
      errno = 0;
    }
  return had_block;
}

coco_bool
coco_get_fd_blocking( int fd )
{
  int flags = fcntl( fd, F_GETFL );
  if (-1==flags)
    {
      coco_warn_printf( "fcntl read failed with errno=%d: %s\n", errno, 
                         coco_errno_string( errno ) );
      errno = 0;
      flags = 0;
    }
  return !(flags & O_NONBLOCK);
}


int
coco_make_udp_fd( unsigned short *port_return, 
                  enum coco_network_error *err_return, 
                  int *err_arg_return )
{
  struct sockaddr_in sockaddr;
  int fd = socket( PF_INET, SOCK_DGRAM, coco_get_protocol( "udp" )->p_proto );
  if (-1 == fd)
    {
      coco_assign_network_error( COCO_NETERROR_CREATE_UDP_SOCKET,
                                 err_return, err_arg_return );
      return -1;
    }
  sockaddr.sin_family = PF_INET;
  sockaddr.sin_port = htons( 0 ); /* any port? */
  sockaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  if (-1 == bind( fd, &sockaddr, sizeof( sockaddr ) ))
    {
      coco_assign_network_error( COCO_NETERROR_BIND_SOCKET,
                                 err_return, err_arg_return );
      return -1;
    }
  coco_set_fd_blocking( fd, COCO_FALSE );
  if (port_return)
    {
      socklen_t sockaddr_size = sizeof( sockaddr );
      if(-1 == getsockname( fd, &sockaddr, &sockaddr_size ))
        {
          coco_assign_network_error( COCO_NETERROR_GETSOCKNAME,
                                     err_return, err_arg_return );
          return -1;
        }
      assert( sockaddr_size == sizeof( sockaddr ) );
      *port_return = ntohs( sockaddr.sin_port );
    }
  return fd;
}




/* _MESSAGES__________________________________________________ */

/* used to send version info, might as well be global. */
struct coco_message_version coco_my_version = 
{ COCO_MESSAGE_VERSION, COCO_MAJOR_VERSION, COCO_MINOR_VERSION };

coco_bool 
coco_message_structrures_correct_size()
{
  /*
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_every ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_name ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_join ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_play ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_chat ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_talk ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_nominate ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_votestate ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_game ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_quit ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_full ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_port ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( struct coco_message_version ));
   * fprintf( stderr, "sizeof: %d\n", sizeof( union coco_message ) );
   */
  return sizeof( union coco_message ) == COCO_MESSAGE_SIZE;
}


coco_bool 
coco_read_message( struct coco_stream *str, union coco_message *return_buff,
                   coco_bool *end_of_file_p )
{
  int read_result;
  assert( str->buffer_in_use < COCO_MESSAGE_SIZE );
  *end_of_file_p = COCO_FALSE;
  read_result = read( str->fd, str->buffer + str->buffer_in_use, 
                      COCO_MESSAGE_SIZE - str->buffer_in_use );
  if (-1 == read_result)
    {
      if (EAGAIN!=errno)
        {
          coco_warn_printf( "Couldn't read(), errno = %d: %s.\n"
                            "Treating it as an End-Of-File.\n", 
                            errno, coco_errno_string( errno ) );
          *end_of_file_p = COCO_TRUE;
        }
      return COCO_FALSE;
    }
  else if (0==read_result)
    {
      coco_cookie( "Read EOF\n" );
      *end_of_file_p = COCO_TRUE;
      return COCO_FALSE;
    }
  else {
    str->buffer_in_use += read_result;
    if (COCO_MESSAGE_SIZE == str->buffer_in_use)
      {
        memcpy( return_buff, str->buffer, sizeof( union coco_message ) );
        str->buffer_in_use = 0;
        return COCO_TRUE;
      }
    else
      {
        assert( str->buffer_in_use < COCO_MESSAGE_SIZE );
        return COCO_FALSE;
      }
  }
}

/* Returns success.
   Writing a message will always block, at present.  */
coco_bool
coco_write_message( struct coco_stream *str, union coco_message *msg )
{
  int result;
  int written = 0;
  int blocking;
  blocking = coco_set_fd_blocking( str->fd, COCO_TRUE );
  while (written < sizeof( union coco_message ))
    {
      result = write( str->fd, msg + written,
                      sizeof( union coco_message ) - written );
      if (-1 == result)
        {
          coco_warn_printf( "Couldn't write(), errno = %d: %s.\n", 
                            errno, coco_errno_string( errno ) );
          return COCO_FALSE;
        }
      else written += result;
    }
  coco_set_fd_blocking( str->fd, blocking );
  return COCO_TRUE;
}

/* _Shared Setup___________________________________________________ */

void
coco_reset_coco_players_array( void )
{
  int i;
  for (i=0; i<COCO_MAX_PLAYERS; i++)
    {
      coco_players[i].in_use = COCO_FALSE;
      coco_players[i].index = i;
      coco_players[i].nationality = COCO_NEUTRAL;
      /* funds, copters init'd elsewhere */
      coco_players[i].name[0] = '\0';
      /* coco_players[i].sent_play_message = COCO_FALSE; init'd elsewhere */
    }

}


/* preform trivial memory initializations and checks */
void 
coco_network_shared_setup( void )
{
  /* better go screw with the message defs again. */
  assert( coco_message_structrures_correct_size() );

  /* If this fails better consider changing u32 and i32 to long. */
  assert( 4 == sizeof( coco_u32 ) );
  assert( 4 == sizeof( coco_i32 ) );
  assert( 2 == sizeof( coco_u16 ) );
  assert( 2 == sizeof( coco_i16 ) );

  coco_reset_coco_players_array();
}

/* EBADF EFAULT ENOTSOCK EISCONN ECONNREFUSED ETIMEDOUT ENETUNREACH EADDRINUSE EINPROGRESS EALREADY EAGAIN EAFNOSUPPORT EACCES EPERM
   EINTR EAGAIN EIO EISDIR EBADF EINVAL EFAULT 


*/


/* creates (by coco_malloc) a string and writes a user-readable network
   error message into it.  Destroy by coco_free().  */
char *
coco_user_level_network_error_message( enum coco_network_error cerror,
                                       int unix_errno_or_zero, 
                                       coco_bool spawned_server_p )
{
  if ((COCO_NETERROR_CONNECT_TCP == cerror)
      && (ECONNREFUSED == unix_errno_or_zero))
    { /* special case. */
      char *static_str
        = spawned_server_p
        ? "TCP Connection refused.\n"
          "The server may not have\n"
          "started correctly."
        : "TCP Connection refused.\n"
          "There may be no server\n"
          "running at that location.";
      char *str = coco_malloc( strlen( static_str ) * sizeof( char ) );
      strcpy( str, static_str );
      return str;
    }
  else
    {
      char *cerrstr = coco_network_error_string( cerror );
      char *errnostr = unix_errno_or_zero 
        ? coco_errno_string( unix_errno_or_zero )
        : NULL;
      /* 50 is the constant format in the sprintf below: */
      int retstrsize = (sizeof( char )) 
        * (50 + strlen( cerrstr ) + (errnostr ? strlen( errnostr ) : 0));
      char *retstr = coco_malloc( retstrsize );
      /* changing this format string?  Change the 50 in the
         errrstrsize allocation too. */
      sprintf( retstr, "Error joining game:\n%s%s%s.", 
               cerrstr, errnostr ? " -- " : "", errnostr ? errnostr : "" );
      return retstr;
    }
}



char *
coco_network_error_string( enum coco_network_error error )
{
  static char *messages[] = {
    "No error",
    "Could not resolve domain name",
    "Creating TCP Socket",
    "Connecting TCP Socket",
    "Writing TCP Data",
    "Creating UDP Socket",
    "Binding Socket",
    "getsockname",
  };
  return messages[error];
}

/* convenience function for assigning network errors to possibly null
   pointers.  (null==fatal error now) */
void
coco_assign_network_error( enum coco_network_error err,
                           enum coco_network_error *err_return, 
                           int *err_arg_return_or_null )
{
  if (err_return)
    {
      *err_return = err;
      if (err_arg_return_or_null)
        *err_arg_return_or_null = errno;
    }
  else 
    if (err_arg_return_or_null)
      coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                         "%s: %s\n",
                         coco_network_error_string( err ), 
                         coco_errno_string( errno ) );

    else
      coco_fatal_printf( COCO_ERROR_NETWORK_BROKEN,
                         "%s\n",
                         coco_network_error_string( err ) );

}

void
coco_cstate_serial_header_hton( struct coco_cstate_serial_header *str )
{
  str->time          = COCO_U32_HTON( str->time );
  str->cobject_count = COCO_U32_HTON( str->cobject_count );
  str->cblip_count   = COCO_U32_HTON( str->cblip_count );
  str->cobject_camera_index = COCO_I32_HTON( str->cobject_camera_index );
  str->cam_left      = COCO_I32_HTON( str->cam_left );
  str->board_w       = COCO_I32_HTON( str->board_w );
  str->dx            = COCO_I32_HTON( str->dx );
  str->dy            = COCO_I32_HTON( str->dy );
  str->dashinfo.bullets       = COCO_U32_HTON( str->dashinfo.bullets );
  str->dashinfo.bombs         = COCO_U32_HTON( str->dashinfo.bombs );
  str->dashinfo.missiles      = COCO_U32_HTON( str->dashinfo.missiles );
  str->dashinfo.rockets       = COCO_U32_HTON( str->dashinfo.rockets );
  str->dashinfo.plasma        = COCO_U32_HTON( str->dashinfo.plasma );
  str->dashinfo.paratroopers  = COCO_U32_HTON( str->dashinfo.paratroopers );
  str->dashinfo.engitroopers  = COCO_U32_HTON( str->dashinfo.engitroopers );
  str->dashinfo.copters       = COCO_U32_HTON( str->dashinfo.copters );
  str->dashinfo.funds         = COCO_U32_HTON( str->dashinfo.funds );
  str->dashinfo.fuel          = COCO_U32_HTON( str->dashinfo.fuel );
  str->dashinfo.camera_class_index
    = COCO_I32_HTON( str->dashinfo.camera_class_index );
}

void
coco_cstate_serial_header_ntoh( struct coco_cstate_serial_header *str )
{
  str->time          = COCO_U32_NTOH( str->time );
  str->cobject_count = COCO_U32_NTOH( str->cobject_count );
  str->cblip_count   = COCO_U32_NTOH( str->cblip_count );
  str->cobject_camera_index = COCO_I32_NTOH( str->cobject_camera_index );
  str->cam_left      = COCO_I32_NTOH( str->cam_left );
  str->board_w       = COCO_I32_NTOH( str->board_w );
  str->dx            = COCO_I32_NTOH( str->dx );
  str->dy            = COCO_I32_NTOH( str->dy );
  str->dashinfo.bullets       = COCO_U32_NTOH( str->dashinfo.bullets );
  str->dashinfo.bombs         = COCO_U32_NTOH( str->dashinfo.bombs );
  str->dashinfo.missiles      = COCO_U32_NTOH( str->dashinfo.missiles );
  str->dashinfo.rockets       = COCO_U32_NTOH( str->dashinfo.rockets );
  str->dashinfo.plasma        = COCO_U32_NTOH( str->dashinfo.plasma );
  str->dashinfo.paratroopers  = COCO_U32_NTOH( str->dashinfo.paratroopers );
  str->dashinfo.engitroopers  = COCO_U32_NTOH( str->dashinfo.engitroopers );
  str->dashinfo.copters       = COCO_U32_NTOH( str->dashinfo.copters );
  str->dashinfo.funds         = COCO_U32_NTOH( str->dashinfo.funds );
  str->dashinfo.fuel          = COCO_U32_NTOH( str->dashinfo.fuel );
  str->dashinfo.camera_class_index
    = COCO_I32_NTOH( str->dashinfo.camera_class_index );
}
