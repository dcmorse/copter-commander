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

/* udpping -- file to test udp communication. */


#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 10101

int
main( int argc, char **argv )
{
  char buf[] = "Hello.";
  size_t buflen = strlen( buf ) + 1;
  int skt, bound, is_sender;
  struct protoent *proto;
  struct sockaddr_in sockaddr;

  assert( (2 == argc) || (1 == argc) );
  is_sender = (2 == argc);

  proto = getprotobyname( "udp" );
  assert( proto );

  skt = socket( PF_INET, SOCK_DGRAM, proto->p_proto );
  assert( skt );

  sockaddr.sin_family = PF_INET;
  sockaddr.sin_port = htons( is_sender ? 0 : PORT );
  sockaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  bound = bind( skt, &sockaddr, sizeof( sockaddr ) );
  if (0!=bound) 
    {
      fprintf( stderr, "Problems binding\n" );
      return 1;
    }

  if (is_sender)
    {
      struct hostent *server_hostent;
      struct sockaddr_in sockaddr2;
      int i, sent;
        
      server_hostent = gethostbyname( argv[1] );

      assert( server_hostent );

      sockaddr2.sin_family = PF_INET;
      sockaddr2.sin_port = htons( (short) PORT );
      sockaddr2.sin_addr.s_addr
        = ((long *) server_hostent->h_addr)[0];

      printf( "sending to 0x%lx", (long) sockaddr2.sin_addr.s_addr );
      for (i=0; i<10; i++)
        {
          putchar( '.' );
          sent = sendto( skt, buf, buflen, 0, &sockaddr2, 
                         sizeof( sockaddr2 ) );
          assert( sent == buflen );
        }
      putchar( '\n' );
    }
  else
    {
      int nbytes;
      int i;
      for (i=0; i<buflen; i++)
        buf[i] = '\0';
      printf( "About to recv()\n" );
      nbytes = recv( skt, buf, buflen, 0 );
      if (nbytes == buflen)
        {
          printf( "got \"%s\"\n", buf );
        }
      else
        {
          fprintf( stderr, "Only recieved %d bytes", nbytes );
          return 1;
        }
    }

  return 0;
}


