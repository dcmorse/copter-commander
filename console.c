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

/* terminal.c -- support for terminal user interface */

/* commands:
   /help [command]
   [message...]
   /new [level] [port] [lcomps] [rcomps]
   /join [host] [port]
   /disconnect
   /tutorial [port]
   /side [nationality]
   /name [name]
   /quit
   /play
   -- maybe someday --
   /whisper [name] [message...]
   /key [key] [fn]
   /addcomp [nationality]
   /delcomp [nationality]
   */

#define COCO_CONSOLE_INT_UNBOUND INT_MIN
#define COCO_CONSOLE_NAT_UNBOUND 1234
#define COCO_CONSOLE_STRING_UNBOUND NULL
#define COCO_CONSOLE_STRING_REST_UNBOUND NULL

union coco_console_actual_arg {
  int aaint;
  int aanat;
  char *aastr;
  char *aastrrr;
}

enum coco_console_arg_type {
  COCO_CAT_INT,
  COCO_CAT_NAT,
  COCO_CAT_STRING,
  COCO_CAT_STRING_REST /* a string that's ended by EOL not space */
}

enum coco_console_arg_optionality {
  COCO_CAO_PROMPT, /* ask user */
  COCO_CAO_DEFAULT, /* use default */
  COCO_CAO_UNBOUND /* have no value */
}

struct coco_console_formal_arg {
  char *name;
  enum coco_console_arg_type type;
  enum coco_console_arg_optionality optionality;
  /* used if optionality = COCO_CAO_DEFAULT: */
  union coco_console_actual_arg default; 
}

/* includes 1 for sentinel and 1 for good luck :) */
#define COCO_CONSOLE_MAX_ARGS 6

struct coco_console_command {
  char *name;
  struct coco_console_formal_arg args[COCO_CONSOLE_MAX_ARGS];
};

struct coco_console_command coco_console_commands[] = {
  { "help", { { "command", COCO_CAT_STRING, COCO_CAO_UNBOUND }, { NULL } } },
  { "new",  { { "level", COCO_CAT_STRING, COCO_CAO_PROMPT, fetch_level },
              { "port", COCO_CAT_INT, COCO_CAO_PROMPT, fetch_port },
              { "lcomps", COCO_CAT_INT, COCO_CAO_PROMPT, fetch_0 },
              { "rcomps", COCO_CAT_INT, COCO_CAO_PROMPT, fetch_0 },
              { NULL } } },
  { "join", { { "host", COCO_CAT_STRING, COCO_CAO_PROMPT, fetch_host },
              { "port", COCO_CAT_INT, COCO_CAO_PROMPT, fetch_port },
              { NULL } } },
  { "disconnect",
            { { NULL } } },  
  { "tutorial", 
            { { "port", COCO_CAT_INT, COCO_CAO_PROMPT, fetch_port },
              { NULL } } },
  { "side", { { "nationality", COCO_CAT_NAT, COCO_CAO_UNBOUND }, { NULL } } },
  { "name", { { "name", COCO_CAT_STRING_REST, COCO_CAO_DEFAULT, fetch_plrname }
              { NULL } } },
  { "quit", { { NULL } } },
  { "play", { { NULL } } },
  { NULL }
}

struct coco_console_command coco_console_default_command =
  { "say",  { { "text", COCO_CAT_STRING_REST, COCO_CAO_PROMPT, NULL },
              { NULL } } };

const char *coco_console_whitespace = " 	";
const char *coco_console_string = "abcdefghijklmnopqstuvwxyzABCDEFGHIJKLMNOPQSTUVWXYZ0123456789";
const char *coco_console_int = "0123456789";

void
coco_console_eval( const char *line )
{
  /* skip whitespace */
  line = coco_console_skip_chars( line, coco_console_whitespace );
  if (line[0] == '/')
    {
      union coco_console_actual_arg actual_args[COCO_CONSOLE_MAX_ARGS];
      struct coco_console_command *p;
      struct coco_console_command *c = &coco_console_default_command;
      struct coco_console_formal_arg *fml;
      /* 1 from gobbling '/' */
      int len = coco_console_skip_chars( line+1, coco_console_string ) 
        - line;
      int arg_len;
      int arg_read_mode = COCO_TRUE;
      for (p=coco_console_commands; p->name != NULL; p++)
        if (0==strncmp( line, p->name, len ))
          {
            c = p; 
            line = coco_console_skip_chars( line + len, 
                                            coco_console_whitespace )
            break;
          }
      for (fml = c->args; fml->name != NULL; fml++)
        if (arg_read_mode)
          {
            switch (fml->type) {
            case COCO_CAT_INT:
              {
                char *nline = line;
                unsigned long val = strtol( line, &nline, 10 );
                if (nline == line)
                  arg_read_mode = COCO_FALSE;
                else
                  actual_args[fml - c->args].aaint = val;
              }
              break;
            case COCO_CAT_NAT:
              break;
            case COCO_CAT_STRING:
              break;
            case COCO_CAT_STRING_REST:
              break;
            }
          }
        else /* not arg_read_mode */
          {
            ...
            switch (fml->optionality) {
            case COCO_CAO_PROMPT:
              break;
            case COCO_CAO_DEFAULT:
              break;
            case COCO_CAO_UNBOUND:
              .aaint = val;
              break;
            }
          }
    }
}


void
coco_console_printf( const char *fmt, ... )
{
  char buf[1024];
  va_list ap;
  va_start( ap, fmt );
  vsnprintf( buf, 1024, fmt, ap );
  coco_cookie( "console-printf> %s\n", buf ); /* STUB */
  va_end( ap );
}


