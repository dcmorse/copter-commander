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

/* sizeof.c: convert int to sizeof( int ) and vice versa, printed to stdout,
   for int, char, short, long, float, double, and size_t.

   Exit values:
   0 for match found,
   1 for no match found.
   2 for user error, bad input, etc.

   % cc -o sizeof sizeof.c
   (on x86)
   % sizeof long
   4
   % sizeof double
   % sizeof 8
   double
   (on alpha)
   % sizeof long
   8
   % sizeof 8
   long
   double
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define NTYPES 7

static char *types[NTYPES] = {
  "char",  "short",
  "int",   "long",
  "float", "double", 
  "size_t"
};

static size_t sizes[NTYPES] = {
  sizeof( char ),  sizeof( short ),
  sizeof( int ),   sizeof( long ), 
  sizeof( float ), sizeof( double ),
  sizeof( size_t )
};

void
die_usage( char *progname )
{
  int i;
  fprintf( stderr, "usage: %s NUMBER", progname );
  for (i=0; i<NTYPES; i++)
    fprintf( stderr, "|%s", types[i] );
  fprintf( stderr, "\n" );
  exit( 2 );
}


int
main( int argc, char **argv )
{
  int i;
  if (argc != 2)
    die_usage( argv[0] );
  if (isdigit( argv[1][0] ))
    { /* map size to type */
      int d;
      int exit = 2; /* default to bad exit */
      sscanf( argv[1], "%d", &d );
      for (i=0; i<NTYPES; i++)
        if (sizes[i] == d)
          {
            printf( "%s\n", types[i] );
            exit = 0;
          }
      return exit;
    }
  else for (i=0; i<NTYPES; i++) /* map type to size */
    if (0==strcmp( types[i], argv[1] ))
      {
        printf( "%d\n", (int) sizes[i] );
        return 0;
      }
  die_usage( argv[0] );
  return 2;
}
