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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h> /* for alloca. (not stdlib because of --ansi) */
#include <assert.h>
#include "coco.h"

unsigned short coco_parse_port( char *strng, char *cl_arg )
{
  long result;
  char *end;
  result = strtol(strng, &end, 0);
  if (!((result >= 0) && 
        (result < (1<<16)) && 
        ((end - strng) == strlen( strng ))))
    coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                        "Couldn't parse port \"%s\".\n", cl_arg );
  return result;
}


/* return the new index, i.e. idx+args_consumed */
int coco_process_shared_arguments( int idx, int argc, char **argv )
{
  if ((0==strcmp( "--version", argv[idx] )))
    coco_version_message();
  else
    if ((0==strcmp( "-?", argv[idx] )) || (0==strcmp( "--help", argv[idx] )))
      coco_help_message();
  else
    if (0==strcmp( "--usage", argv[idx] ))
      coco_usage_message( COCO_TRUE );
  else
    if (0==strcmp( "--list-levels", argv[idx] ))
      coco_list_levels_message();
  else
    if ((0==strcmp( "-p", argv[idx] )) || (0==strcmp( "--port", argv[idx] )))
      {
        idx++;
        if (idx<argc)
          coco_port = coco_parse_port( argv[idx], argv[idx] );
        else coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                                 "Missing argument to \"%s\" option.\n",
                                 argv[idx-1] );
        return idx+1;
      }
  else if (0==strncmp( "--port=", argv[idx], 7 ))
    { 
      coco_port = coco_parse_port( argv[idx] + 7, argv[idx] );
      return idx+1;
    }
  return idx;
}


/* Returns true if the main program should run */
coco_bool coco_process_server_arguments( int argc, char **argv )
{
  int j, i=0;
  coco_bool level_file_name_malloced_p = COCO_FALSE;
  char *log_file_name = NULL;
  char *level_file_name = NULL;
  argc-=2; 
  argv+=2;
  while (i < argc)
    {
      j = coco_process_shared_arguments( i, argc, argv );
      if (j!=i)
        { i=j; continue; }
      else if ((0==strcmp( "--log", argv[i] )))
        {
          i++;
          if (i<argc)
            log_file_name = argv[i];
          else coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                                   "Missing argument to \"%s\" option.\n",
                                   argv[i-1] );
        }
      else if (0==strncmp( "--log=", argv[i], 10 ))
        log_file_name = argv[i] + 10;
      else if ((0==strcmp( "-q", argv[i] )) || 
               (0==strcmp( "--quiet", argv[i] )))
        log_file_name = "/dev/null";
      else if ((0==strcmp( "--level", argv[i] )))
        {
          i++;
          if (i<argc)
            level_file_name = argv[i];
          else coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                                   "Missing argument to \"%s\" option.\n",
                                   argv[i-1] );
        }
      else if (0==strncmp( "--level=", argv[i], 10 ))
        level_file_name = argv[i] + 10;
      else coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                               "Unknown option \"%s\".\n",
                               argv[i] );
      i++;
    }
  coco_set_log_file( log_file_name );
  if (!coco_massage_level_file_name( &level_file_name,
                                     &level_file_name_malloced_p ))
    coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                       "Couldn't massage %s into a valid level file name.\n",
                       level_file_name );
  coco_level_text = coco_map_level_text( level_file_name, NULL );
  if (level_file_name_malloced_p) coco_free( level_file_name );
  return COCO_TRUE;
}



/* Returns true if the main program should run */
coco_bool coco_process_client_arguments( int argc, char **argv )
{
  int i, j;
  int gnome_arg_start = -1;
  int gnome_arg_end = -1;

  i=1;
  while (i<argc)
    {
      j = coco_process_shared_arguments( i, argc, argv );
      if (j!=i)
        { i=j; continue; }
      else if ((0==strcmp( "-s", argv[i] )) || 
               (0==strcmp( "--server", argv[i] )))
        {
          i++;
          if (i<argc)
            {
              coco_server = argv[i];
              coco_server_specified_p = COCO_TRUE;
            }
          else coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                                   "Missing argument to \"%s\" option.\n",
                                   argv[i-1] );
        }
      else if (0==strncmp( "--server=", argv[i], 9 ))
        {
          coco_server = argv[i] + 9;
          coco_server_specified_p = COCO_TRUE;
        }
      else if (0==strcmp( "--single-buffer", argv[i] ))
        coco_command_line_inhibit_double_buffer = COCO_TRUE;
      else if (0==strcmp( "--blend-alpha", argv[i] ))
        coco_blend_alpha = 1;
      else if (0==strcmp( "--no-blend-alpha", argv[i] ))
        coco_blend_alpha = 0;
      else if (0==strcmp( "--gnome-options", argv[i] ))
        {
          int k;
          for (k=i+1; (k<argc)&&strcmp( "--coco-options", argv[k] ); k++)
            ;
          gnome_arg_start = i+1;
          gnome_arg_end = k;
          i = k;
        }
      else coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                               "Unknown option \"%s\".\n",
                               argv[i] );
      i++;
    }
  coco_process_gnome_args( gnome_arg_start, gnome_arg_end, argv );
  return COCO_TRUE;
}

