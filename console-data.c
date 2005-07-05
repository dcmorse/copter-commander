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

/* console-data.c -- support for terminal user interface */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "coco.h"



union coco_console_actual_arg
coco_console_default_level( void )
{
  union coco_console_actual_arg a;
  a.aastr = coco_strdup( "flashpoint" );
  return a;
}

union coco_console_actual_arg
coco_console_default_port( void )
{
  union coco_console_actual_arg a;
  a.aaint = coco_port;
  return a;
}

union coco_console_actual_arg
coco_console_default_zero( void )
{
  union coco_console_actual_arg a;
  a.aaint = 0;
  return a;
}

union coco_console_actual_arg
coco_console_default_host( void )
{
  union coco_console_actual_arg a;
  a.aastr = coco_strdup( "coco_server" );
  return a;
}

union coco_console_actual_arg
coco_console_default_nickname( void )
{
  union coco_console_actual_arg a;
  a.aastr = coco_strdup( coco_nickname() );
  return a;
}

void 
coco_console_help( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  const char *help_str = aargs[0].aastr;
  coco_bool help_menu;
  struct coco_console_command *help_cmd;
  if (COCO_CONSOLE_STRING_UNBOUND == help_str)
    help_menu = COCO_TRUE;
  else
    {
      coco_bool starts_w_slash = help_str[0] == '/';
      help_menu = COCO_TRUE;
      for (help_cmd=coco_console_commands; help_cmd->name; help_cmd++)
        if (0==strcmp( help_str + (int) starts_w_slash, help_cmd->name ))
          {
            help_menu = COCO_FALSE;
            break;
          }
    }


  if (help_menu)
    { /* print help menu */
      struct coco_console_command *cmds = coco_console_commands;
      if (COCO_CONSOLE_STRING_UNBOUND == help_str)
        coco_console_printf( "To get help on a command, type /%s "
                             "COMMAND-NAME.\n",
                             cmd->name );
      else
        coco_console_printf( "Unrecognised command \"%s\".  "
                             "Here are the known ones:\n", help_str );
      while (cmds->name != NULL)
        {
          if (!cmds->undocumented)
            coco_console_printf( "/%s - %s\n", cmds->name, cmds->help );
          cmds++;
        }
    }
  else
    {
      struct coco_console_formal_arg *fml;
      coco_console_printf( "/%s", help_cmd->name );
      for (fml=help_cmd->args; fml->name; fml++)
        coco_console_printf( " [%s]", fml->name );
      coco_console_printf( "\n%s\n", help_cmd->help );
    }
}

void 
coco_console_new( struct coco_console_command *cmd, 
                  union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_join( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_disconnect( struct coco_console_command *cmd, 
                         union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_tutorial( struct coco_console_command *cmd, 
                       union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_side( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_name( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_quit( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_play( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

void 
coco_console_talk( struct coco_console_command *cmd, 
                   union coco_console_actual_arg *aargs )
{
  coco_console_printf( "sorry, function \"%s\" is not implented.\n",
                       cmd->name );
}

enum coco_console_input_parse 
coco_console_parse_any_string( const char *input, union coco_console_actual_arg *return_or_null )
{
  if (return_or_null)
    return_or_null->aastr = coco_strdup( input );
  return COCO_LEGAL;
}

enum coco_console_input_parse 
coco_console_parse_command( const char *input, union coco_console_actual_arg *return_or_null )
{
  struct coco_console_command *cmd;
  input = coco_console_skip_chars( input, coco_console_whitespace );
  if (input[0] == '/') 
    input++;
  /* worry more about trailing whitespace? */
  for (cmd=coco_console_commands; cmd->name; cmd++)
    if ((0==coco_strcasecmp( input, cmd->name )))
      {
        if (return_or_null) 
          return_or_null->aacmd = cmd;
        return COCO_LEGAL;
      }
  return COCO_INCOMPLETE;
}

enum coco_console_input_parse 
coco_console_parse_level( const char *input, union coco_console_actual_arg *return_or_null )
{
  return COCO_LEGAL;
}

enum coco_console_input_parse 
coco_console_parse_port( const char *input, union coco_console_actual_arg *return_or_null )
{
  return COCO_LEGAL;
}

enum coco_console_input_parse 
coco_console_parse_comp_count( const char *input, union coco_console_actual_arg *return_or_null )
{
  return COCO_LEGAL;
}

enum coco_console_input_parse 
coco_console_parse_host( const char *input, union coco_console_actual_arg *return_or_null )
{
  return COCO_LEGAL;
}

enum coco_console_input_parse 
coco_console_parse_nationality( const char *input, union coco_console_actual_arg *return_or_null )
{
  return COCO_LEGAL;
}


struct coco_console_command coco_console_commands[] = {
  { "help", { { "command", COCO_CAT_STRING, COCO_CAO_UNBOUND, NULL,
                coco_console_parse_command }, { NULL } },
    coco_console_help, "Get help."},
  { "new",  { { "level", COCO_CAT_STRING, COCO_CAO_PROMPT, 
                coco_console_default_level, coco_console_parse_level },
              { "port", COCO_CAT_INT, COCO_CAO_PROMPT, 
                coco_console_default_port, coco_console_parse_port },
              { "left computer players", COCO_CAT_INT, COCO_CAO_PROMPT,
                coco_console_default_zero, coco_console_parse_comp_count },
              { "right computer players", COCO_CAT_INT, COCO_CAO_PROMPT, 
                coco_console_default_zero, coco_console_parse_comp_count },
              { NULL } },
    coco_console_new, "Start a new game server, and join it." },
  { "join", { { "host", COCO_CAT_STRING, COCO_CAO_PROMPT, 
                coco_console_default_host, coco_console_parse_host },
              { "port", COCO_CAT_INT, COCO_CAO_PROMPT,
                coco_console_default_port, coco_console_parse_port },
              { NULL } },
    coco_console_join, "Join an existing game."},
  { "disconnect",
            { { NULL } },
    coco_console_disconnect, "Exit current game" },  
  { "tutorial", 
            { { "port", COCO_CAT_INT, COCO_CAO_PROMPT, 
                coco_console_default_port, coco_console_parse_port },
              { NULL } },
    coco_console_tutorial, "Learn to play " COCO_PRODUCT_NAME },
  { "side", { { "nationality", COCO_CAT_NAT, COCO_CAO_UNBOUND,
              NULL, coco_console_parse_nationality }, { NULL } },
    coco_console_side, "Choose team: either \"left\" or \"right\"."},
  { "name", { { "name", COCO_CAT_STRING_REST, COCO_CAO_DEFAULT, 
                coco_console_default_nickname, coco_console_parse_any_string },
              { NULL } },
    coco_console_name, "Choose your player name."},
  { "quit", { { NULL } }, coco_console_quit, "Exit " COCO_PRODUCT_NAME },
  { "exit", { { NULL } }, coco_console_quit, "Exit " COCO_PRODUCT_NAME, 
    COCO_TRUE },
  { "play", { { NULL } }, coco_console_play, "Make a forming game start." },
  { NULL }
};

struct coco_console_command coco_console_default_command =
  { "talk",  
            { { "text", COCO_CAT_STRING_REST, COCO_CAO_PROMPT, 
                NULL, coco_console_parse_any_string },
              { NULL } },
  coco_console_talk, "Tell the other players some text message."};

