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

#include <assert.h>
#include "coco.h"



/* diagnostic AND never worked right.
void
dump_raster( struct coco_raster *raster )
{
  /+ only works on bitmaps with widths <= 32 +/
  int i, j;
  for (j=0; j<raster->h; j++)
    {
      for (i=0; i<raster->w; i+=8)
        putchar( ((raster->pixels[(i>>3)+(j<<5)])
                  &(1<<(7 - (i&7)))) ? 'X' : ' ' );
      putchar( '\n' );
    }
}
*/


void
coco_run_sanity_checks( void )
{
  int test = coco_check_hton();
  if (test != 0)
    coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                       "coco_check_hton detects problems on test number %d\n",
                       test );
}

int main(int argc, char **argv)
{
  int server;

  coco_run_name = argv[0];

  coco_run_sanity_checks();

  server = ((argc > 1) && (0==strcmp( "server", argv[1] )));
  if (server)
    {
      if (coco_process_server_arguments( argc, argv ))
        {
          coco_server_exec();
        }
    }
  else
    {
      if (coco_process_client_arguments( argc, argv ))
        {
          coco_cookie( "port %d, host %s, buffer %s\n", 
                       coco_port, coco_server, 
                       coco_command_line_inhibit_double_buffer
                       ? "single" : "double" );
          coco_client_exec();
        }
    }
  return 0;
}

