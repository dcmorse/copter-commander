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

/* endian.c: print the byte ordering of the machine
   % cc -o endian endian.c
   (on little endian such as x86 linux)
   % endian
   4321
   (on big endian such as ?sparc?)
   % endian
   1234

*/

/* Allow COCO_INT32 to be passed in at compile time */
#ifndef COCO_INT32
#define COCO_INT32 int
#endif /* COCO_INT32 */


#include <stdio.h>
#include <assert.h>

int
main( void )
{
  COCO_INT32 bytes = 0x01020304;
  char *ptr;
  int i;
  if ( 4!=sizeof( COCO_INT32 ) )
    {
      fprintf( stderr, "endian requires sizeof( COCO_INT32 ) = 4, "
               "but it is = %d\n", (int) sizeof( COCO_INT32 ) );
      return 1;
    }
  ptr = (char *) &bytes;
  for (i=0; i<4; i++)
    printf( "%d", ptr[i] );
  putchar( '\n' );
  return 0;
}
