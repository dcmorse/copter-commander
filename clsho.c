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

#include <string.h>
#include <alloca.h>
#include <assert.h>
#include "coco.h"

coco_bool
coco_level_filename_p( char *filename )
{
  if (NULL==filename)
    return COCO_FALSE;
  else
    {
      size_t len = strlen( filename );
      return (len >= (1+COCO_LEVEL_SUFFIX_LENGTH))
        && (0==strcmp( "." COCO_LEVEL_SUFFIX, filename + len
                       - (COCO_LEVEL_SUFFIX_LENGTH + 1) ));
    }
}

static coco_bool
coco_find_first_level_file( char *filename, void *raw_data )
{
  if (coco_level_filename_p( filename ))
    {
      char **data = raw_data;
      assert( data );
      *data = coco_malloc( strlen( filename )
                           + strlen( coco_level_directory ) + 2 );
      sprintf( *data, "%s/%s", coco_level_directory, filename );
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}

/* returns true if it thinks the new filename is good. */
coco_bool
coco_massage_level_file_name( char **level_file_name_in_out, 
                              coco_bool *level_file_name_malloced_p_in_out )
{
  assert( level_file_name_in_out && level_file_name_malloced_p_in_out );
  if (NULL == *level_file_name_in_out)
    {
      int success;
      success = coco_map_directory( coco_level_directory, 
                                    coco_find_first_level_file,
                                    (void *) level_file_name_in_out );
      *level_file_name_malloced_p_in_out = COCO_TRUE;
      if (success != 0)
        coco_fatal_printf( COCO_ERROR_LEVEL_FILE_NOT_FOUND,
                           "Problem scanning %s for built in levels: %s\n",
                           coco_level_directory,
                           coco_errno_string( success ) );
      if (NULL==level_file_name_in_out)
        coco_fatal_printf( COCO_ERROR_LEVEL_FILE_NOT_FOUND,
                           "Could not find any .ccl levels in %s\n",
                           coco_level_directory );
    }
  if (coco_level_filename_p( *level_file_name_in_out ))
    return COCO_TRUE;
  else
    {
      int try_path_length = strlen( coco_level_directory )
        + 1 /* "/" */
        + strlen( *level_file_name_in_out )
        + 1 /* "." */
        + COCO_LEVEL_SUFFIX_LENGTH
        + 1; /* null-char */
      char *try_path = coco_malloc( try_path_length * sizeof( char ) );
      sprintf( try_path, "%s/%s.%s", coco_level_directory,
               *level_file_name_in_out, COCO_LEVEL_SUFFIX );
      if (coco_probe_file( try_path ))
        {
          *level_file_name_in_out = try_path;
          *level_file_name_malloced_p_in_out = COCO_TRUE;
          return COCO_TRUE;
        }
      else 
        {
          coco_free( try_path );
          return COCO_FALSE;
        }
    }
}

void
coco_set_log_file( char *log_file_name )
{
  if (log_file_name)
    {
      coco_server_log = fopen( log_file_name, "w" );
      if (NULL == coco_server_log)
        coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                           "Couldn't open log file \"%s\"",
                           log_file_name );
    }
  else coco_server_log = stderr;
}


void
coco_usage_message( coco_bool exit_p )
{
  printf( "Usage: %s server [SERVER-OPTION|SHARED-OPTION]...\n"
          "       %s [CLIENT-OPTION|SHARED-OPTION]...\n", 
          coco_run_name, 
          coco_run_name );
  if (exit_p) exit( 0 );
}


void 
coco_help_message()
{
  coco_usage_message( COCO_FALSE );
  printf( "%s\n"
          "\n"
          "Shared Options:\n"
          "  -p PORT, --port=PORT    Connect on port number PORT (default %d).\n"
          "  -?, --help              Show this help message.\n"
          "  --version               Show version number.\n"
          "  --usage                 Show brief usage message.\n"
          "  --list-levels           Show built in levels.\n"
          "\n"
          "Client Options:\n"
          "  -s HOST, --server=HOST  Search HOST for games (default %s).\n"
          "  --single-buffer         Use single buffered visuals only.\n"
          "  --blend-alpha           Use pretty but expensive graphics.\n"
          "  --gnome-options         send further options to gnome (--coco-options terminated)\n"
          "\n"
          "Server Options:\n"
          "  --log=FILE              Route diagnostics to a file (default stdout).\n"
          "  -q, --quiet             Do not produce diagnostics.\n"
          "  --level=FILE            Load level from file.\n",

          coco_product_description, coco_port, coco_server );
  exit( 0 );
}

void 
coco_version_message( void )
{
  printf( "%s %s %d.%d\n", coco_gui_description_string,
          coco_product_name, COCO_MAJOR_VERSION, 
          COCO_MINOR_VERSION );
  exit( 0 );
}

coco_bool
coco_list_level_file_callback( char *filename, void *data )
{
  if (coco_level_filename_p( filename ))
    {
      int size = strlen( filename ) - COCO_LEVEL_SUFFIX_LENGTH;
      char *filename_suffixless = alloca( size );
      if (NULL==filename_suffixless)
        filename_suffixless = coco_malloc( size );
      strncpy( filename_suffixless, filename, size-1 );
      filename_suffixless[size-1] = '\0';
      printf( "%s\n", filename_suffixless );
    }
  return COCO_FALSE;
}


void
coco_list_levels_message( void )
{
  int success;
  success = coco_map_directory( coco_level_directory, 
                                coco_list_level_file_callback,
                                NULL );
  if (success!=0)
    coco_fatal_printf( COCO_ERROR_LEVEL_FILE_NOT_FOUND,
                       "Looking for built in levels in %s: %s\n",
                       coco_level_directory, coco_errno_string( success ) );
  else exit( 0 );
}
