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

#define _BSD_SOURCE /* hack to get strncasecmp under glibc */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "coco.h"

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
  char *aastr; /* coco_malloc'ed */
  char *aastrrr; /* coco_malloc'ed */
};

enum coco_console_arg_type {
  COCO_CAT_INT,
  COCO_CAT_NAT,
  COCO_CAT_STRING,
  COCO_CAT_STRING_REST /* a string that's ended by EOL not space */
};

enum coco_console_arg_optionality {
  COCO_CAO_PROMPT, /* ask user */
  COCO_CAO_DEFAULT, /* use default */
  COCO_CAO_UNBOUND /* have no value */
};

struct coco_console_formal_arg {
  char *name;
  enum coco_console_arg_type type;
  enum coco_console_arg_optionality optionality;
  /* used if optionality = COCO_CAO_DEFAULT: */
  union coco_console_actual_arg de_fault; /* default is C lang keyword */
};

/* includes 1 for sentinel and 1 for good luck :) */
#define COCO_CONSOLE_MAX_ARGS 6

struct coco_console_command {
  char *name;
  struct coco_console_formal_arg args[COCO_CONSOLE_MAX_ARGS];
};

/* just stubs for now.  eventually are functions */
#define fetch_level { 0 }
#define fetch_port { 0 }
#define fetch_0 { 0 }
#define fetch_host { 0 }
#define fetch_plrname { 0 }

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
  { "name", { { "name", COCO_CAT_STRING_REST, COCO_CAO_DEFAULT, fetch_plrname },
              { NULL } } },
  { "quit", { { NULL } } },
  { "play", { { NULL } } },
  { NULL }
};

struct coco_console_command coco_console_default_command =
  { "say",  
            { { "text", COCO_CAT_STRING_REST, COCO_CAO_PROMPT },
              { NULL } } };

const char *coco_console_whitespace = " 	";
const char *coco_console_ident = "abcdefghijklmnopqstuvwxyzABCDEFGHIJKLMNOPQSTUVWXYZ0123456789";
const char *coco_console_int = "0123456789";

const char *
coco_console_skip_chars( const char *line, const char *charset )
{
  char l;
  const char *li;
  for (li=line; (l=*li) != '\0'; li++)
    {
      const char *cs;
      char c;
      coco_bool found = COCO_FALSE;
      for (cs=charset; (c=*cs) != '\0'; cs++)
        if (l == c)
          {
            found = COCO_TRUE;
            break;
          }
      if (!found) 
        return li;
    }
  return li;
}

/* temporary error handler for console errors */
void 
coco_console_error( const char *line, const char *problem )
{
  coco_fatal_printf( COCO_ERROR_NOT_IMPLEMENTED,
                     "bad line \"%s\"\nReason = %s\n", line, problem );
}



void
coco_console_eval( const char *line )
{
  /* skip whitespace */
  line = coco_console_skip_chars( line, coco_console_whitespace );
    {
      union coco_console_actual_arg actual_args[COCO_CONSOLE_MAX_ARGS];
      struct coco_console_command *p;
      struct coco_console_command *c = &coco_console_default_command;
      struct coco_console_formal_arg *fml;
      /* 1 from gobbling '/' */
      int len, i;
      int arg_read_mode = COCO_TRUE;
      if (line[0] == '/') line++; /* advanced past '/' */
      len = coco_console_skip_chars( line, coco_console_ident ) 
        - line;
      for (p=coco_console_commands; p->name != NULL; p++)
          if (0==strncmp( line, p->name, len ))
            {
              c = p; 
              line = coco_console_skip_chars( line + len, 
                                              coco_console_whitespace );
              break;
            }
      coco_cookie( "got cmd \"%s\"\n", c->name );
#define COCO_CE_ACTUAL_ARG(  ) (actual_args[fml - c->args])
      for (fml = c->args; fml->name != NULL; fml++)
        if (arg_read_mode)
          {
            coco_cookie( "binding parameter %d\n", fml - c->args );
            switch (fml->type) {
            case COCO_CAT_INT:
              {
                char *nline = NULL;
                long val = strtol( line, &nline, 10 );
                if (nline == line)
                  arg_read_mode = COCO_FALSE;
                else
                  {
                    COCO_CE_ACTUAL_ARG().aaint = val;
                    assert( nline );
                    line = nline;
                  }
              }
              break;
            case COCO_CAT_NAT:
              {
                int arglen
                  = coco_console_skip_chars( line, coco_console_ident ) - line;
                const char one[] = "1";
                const char two[] = "2";
                const char right[] = COCO_GOOD_GUYS_PRETTY_NAME;
                const char left[] = COCO_BAD_GUYS_PRETTY_NAME;
                if ((0==strncasecmp( right, line, arglen )) || 
                    (0==strncasecmp( two,   line, arglen )))
                  COCO_CE_ACTUAL_ARG().aanat = COCO_GOOD_GUYS;
                else if ((0==strncasecmp( left, line, arglen )) || 
                         (0==strncasecmp( one,  line, arglen )))
                  {
                    COCO_CE_ACTUAL_ARG().aanat = COCO_BAD_GUYS;
                    line += arglen;
                  }
                else if (*(coco_console_skip_chars( line, coco_console_whitespace )))
                  /* non-null, non-whitespace nat */
                  coco_console_error( line, "Nationality is garbled.");
                else
                  arg_read_mode = COCO_FALSE;
              }
              break;
            case COCO_CAT_STRING:
              {
                char *l = line;
                size_t len;
                while (1)
                  {
                    char c = *l;
                    if (('\0' == c) || (' ' == c) || ('	' == c))
                      break;
                    else l++;
                  }
                len = l - line;
                coco_strndup( COCO_CE_ACTUAL_ARG().aastr, line, len );
                line = l;
              }
              break;
            case COCO_CAT_STRING_REST:
              COCO_CE_ACTUAL_ARG().aastrrr = coco_strdup( line );
              line += strlen( line );
              break;
            }
          }
        else /* not arg_read_mode */
          {
            /* XXX */
            switch (fml->optionality) {
            case COCO_CAO_PROMPT:
              break;
            case COCO_CAO_DEFAULT:
              break;
            case COCO_CAO_UNBOUND:
              /* COCO_CE_ACTUAL_ARG().aaint = val; cruft */
              break;
            }
          }
#undef COCO_CE_ACTUAL_ARG      
      for (i=0, fml = c->args; fml->name != NULL; fml++, i++)
        coco_cookie( "arg %d as nat: %d\n", i, actual_args[i].aanat );
      /* free char* actual args */
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


/* only relevant when not fullscreen */
#define COCO_CONSOLE_LINES 1
#define COCO_CONSOLE_Y COCO_OFFICIAL_WINDOW_H

void
coco_console_draw( coco_bool fullscreen )
{
  coco_font *font = &coco_default_font;
  int fis = COCO_FONT_INTERLINE_SPACING( font );
  int nlines
    = fullscreen ? (coco_real_window_h / fis) : COCO_CONSOLE_LINES;
}
