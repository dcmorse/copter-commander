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

/* This file is no longer used.  I didn't like maintaining two different
 * code bodies for parsing command lines.  Popt is at least as complex as
 * the problem its trying to solve.  So out it goes. -dm 02-26-2001 */


#include <gnome.h> /* for gnome_init_with_popt_table */
#include <popt.h>  /* in case no gnome */
#include "coco.h"

static char *coco_popt_log_file_name = "*special-stdout*";
static char *coco_popt_level_file_name = NULL;

static int coco_popt_help_interceptor = 0;
static int coco_popt_usage_interceptor = 0;

static int coco_popt_version_flag = 0;

static int coco_popt_list_levels_flag = 0;

#define COCO_SHARED_OPTIONS \
   { "port", 'p', POPT_ARG_INT, &coco_port, 0, \
       "Connect at port number.", "NUM" },\
   { "help", '?', POPT_ARG_NONE, \
     (void *) &coco_popt_help_interceptor, 0, NULL }, \
   { "usage", '\0', POPT_ARG_NONE, \
     (void *) &coco_popt_usage_interceptor, 0, NULL }, \
   { "list-levels", '\0', POPT_ARG_NONE, \
     (void *) &coco_popt_list_levels_flag, 0, NULL }



struct poptOption coco_popt_server_options[] = {
  COCO_SHARED_OPTIONS,
  { "version", '\0', POPT_ARG_NONE, 
    &coco_popt_version_flag, 0,
    "Print version number and exit", NULL },
  { "level", '\0', POPT_ARG_STRING, &coco_popt_level_file_name, 0,
    "Load level from .ccl file or built in level", "LEVEL" },
  { "log", '\0', POPT_ARG_STRING, &coco_popt_log_file_name, 0,
    "Route diagnostics to a file (default stdout)", "FILE" },
  { "quiet", 'q', POPT_ARG_VAL, &coco_popt_log_file_name, 0,
    "Do not produce diagnostics", NULL },
  { NULL, 0, 0, NULL, 0 }
};

struct poptOption coco_popt_client_options[] = {
  COCO_SHARED_OPTIONS,
  { "host", '\0', POPT_ARG_STRING, &coco_server, 0,
    "Name of computer to use as server (default localhost)", "NAME" },
  { "single-buffer", '\0', POPT_ARG_NONE, 
    &coco_command_line_inhibit_double_buffer, 0,
    "Only select single-buffered visuals", NULL },
  { "blend-alpha", '\0', POPT_ARG_NONE, 
    &coco_blend_alpha, 0,
    "Use expensive alpha-blending technique for prettier transparency.", 
    NULL },
  { NULL, 0, 0, NULL, 0 }
};

/* also frees pc if no errors were detected. */
static coco_bool
coco_popt_validate_rc( int rc, poptContext pc )
{
  if (-1 == rc)
    {
      poptFreeContext( pc );
      return COCO_TRUE;
    }
  else if (rc < 0)
    {
      const char *badness, *badness_context;
      badness = poptStrerror( rc );
      badness_context = poptBadOption( pc, POPT_BADOPTION_NOALIAS );
      coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                         "%s: %s\n", badness_context, badness );
      return COCO_FALSE; /* fool compiler */
    }
  else
    {
      coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                         "Impossible situation detected in coco-popt.\n" );
      return COCO_FALSE; /* fool compiler */
    }
}


/* return true if the program should still run. */
static coco_bool
coco_4_coco_popts( int argc, char **argv, 
                   struct poptOption *options,
                   coco_bool init_gnome_p )
{
  int rc;
  poptContext pc, prepc;

  /* There are two issues with command line processing.  
     
     First, the popt builtin --help and --usage messages are wrong,
     because they don't grok that there can be a "server" as the first
     argument.  Unfortunately, gnome turns these on for us.  So if the
     user really does specify --help or --usage, we detect it by
     processing our popt options without gnome (this is the prepc).  If
     they are detected we print out the Copter Commander specific
     help/usage and quit. 

     Second, the server doesn't use gnome at all, so calling gnome_init is
     wrong.  Not just wasteful, but wrong, because pretty soon after
     starting we'll get a:

     +-----------------------------------------------------+
     |                      WARNING                        |
     |                                                     |
     |gcoco --sm-client-id 11c0a8astoundinglylongnumber0278|
     |                                                     |
     |No response to the SaveYourself command.             |
     |The program may be slow, stopped or broken.          |
     |You may wait for it to respond or remove it.         |
     |                                                     |
     |                            [Remove Program] [Cancel]|
     +-----------------------------------------------------+

     So the server doesn't call gnome_init, it usese the prepc popt
     context call instead.  hp@redhat.com points out that we could call
     gnome_disable_master_client() instead.  But this way seems marginally
     cleaner to me, as long as the server really never calls gnome.  
  */

  prepc = poptGetContext( coco_product_name, 
                          argc, argv, options, 0 );
  rc = poptGetNextOpt( prepc );

  if (coco_popt_help_interceptor)
    coco_help_message( COCO_TRUE );
  else if (coco_popt_usage_interceptor)
    coco_usage_message( COCO_TRUE, COCO_TRUE );
  else if (coco_popt_version_flag) /* only in server opts */
    coco_version_message();
  else if (coco_popt_list_levels_flag)
    coco_list_levels_message();

  if (!init_gnome_p)
    return coco_popt_validate_rc( rc, prepc );
  else
    {
      /* return value of gnome_init_with_popt_table is undocumented. */
      gnome_init_with_popt_table( coco_product_name, coco_version_string,
                                  argc, argv, options, 0, &pc );
      /* Or with simple popt:
         pc = poptGetContext( coco_product_name, argc, argv, options, 0 ); */

      rc = poptGetNextOpt( pc );

      return coco_popt_validate_rc( rc, pc );
    }
}

coco_bool 
coco_process_client_arguments( int argc, char **argv )
{
  return coco_4_coco_popts( argc, argv, coco_popt_client_options, COCO_TRUE );
}

coco_bool 
coco_process_server_arguments( int argc, char **argv )
{
  /* since the command line arguments will start with the word "server",
     and popt in all its wisdom can't handle that, we have to copy and
     pass a fudged array in. */
  char *stdout_file_name = coco_popt_log_file_name; /* "*special-stdout*" */
  int i;
  int argv_ns_size = (sizeof( char * )) * (argc - 1);
  char **argv_no_server = alloca( argv_ns_size );
  if (NULL==argv_no_server)
    argv_no_server = coco_malloc( argv_ns_size );
  argv_no_server[0] = argv[0];
  for (i=2; i<argc; i++)
    argv_no_server[i-1] = argv[i];

  if (coco_4_coco_popts( argc-1, argv_no_server, coco_popt_server_options, 
                         COCO_FALSE ))
    {
      coco_cookie( "log_file = %s\n", coco_popt_log_file_name );
      coco_set_log_file( (coco_popt_log_file_name == stdout_file_name)
                         ? NULL
                         : (coco_popt_log_file_name
                            ? coco_popt_log_file_name
                            : "/dev/null") );
      
      { /* load level file */
        coco_bool malloced_p = COCO_FALSE;
        coco_cookie( "Input file name = %s\n", coco_popt_level_file_name );
        if (!coco_massage_level_file_name( &coco_popt_level_file_name,
                                           &malloced_p ))
          coco_fatal_printf( COCO_ERROR_BAD_COMMAND_LINE,
                             "Couldn't massage %s into a valid level"
                             " file name.\n",
                             coco_popt_level_file_name );
        coco_cookie( "Output file name = %s\n", coco_popt_level_file_name );
        coco_level_text = coco_map_level_text( coco_popt_level_file_name,
                                               NULL );
        if (malloced_p) coco_free( coco_popt_level_file_name );
      }
      return COCO_TRUE;
    }
  else return COCO_FALSE;
}
