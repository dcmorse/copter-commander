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

/* console-language.c -- support for terminal user interface */

#define _BSD_SOURCE /* to get strncasecmp under glibc */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "coco.h"

/* includes 1 for each argument, +1 for the final funcall, +1 for the loop */
struct coco_console_task_stack_entry 
coco_console_task_stack[COCO_CONSOLE_MAX_ARGS+2] = { { NULL } };

int coco_console_task_stack_size = 0;

void coco_console_push_task( struct coco_console_task_stack_entry *entry );

void coco_console_read_and_handle_rest( const char *isnull, 
                                        void *handy1, void *handy2 );

void coco_console_fill_argument( const char *line,
                                 void *handy1, void *handy2 );


const char *
coco_console_prompt( void )
{
  const char *prompt;
  assert( coco_console_task_stack_size >= 1 );
  assert( coco_console_task_stack_size
          < (sizeof( coco_console_task_stack )
             / sizeof( struct coco_console_task_stack_entry )) );
  prompt = coco_console_task_stack[coco_console_task_stack_size-1].prompt;
  assert( prompt );
  return prompt;
}

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



/* returns true if it could do it */
coco_bool
coco_console_read_actual_arg( const char **line_read_return,   
                              struct coco_console_formal_arg *fml,
                              union coco_console_actual_arg *acl_return,
                              coco_bool str_is_whole_line )
{
  const char *line = *line_read_return;
  switch (fml->type) {
  case COCO_CAT_INT:
    {
      char *nline = NULL;
      long val = strtol( line, &nline, 10 );
      if (nline == line)
        return COCO_FALSE;
      else
        {
          acl_return->aaint = val;
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
        acl_return->aanat = COCO_GOOD_GUYS;
      else if ((0==strncasecmp( left, line, arglen )) || 
               (0==strncasecmp( one,  line, arglen )))
        {
          acl_return->aanat = COCO_BAD_GUYS;
          line += arglen;
        }
      else if (*(coco_console_skip_chars( line, coco_console_whitespace )))
        /* non-null, non-whitespace nat */
        coco_console_error( line, "Nationality is garbled.");
      else
        return COCO_FALSE;
    }
    break;
  case COCO_CAT_STRING:
    if (!str_is_whole_line)
      {
        const char *l = line;
        size_t len;
        while (1)
          {
            char c = *l;
            if (('\0' == c) || (' ' == c) || ('	' == c))
              break;
            else l++;
          }
        len = l - line;
        if (len==0)
          return COCO_FALSE;
        acl_return->aastr = coco_strndup( line, len );
        line = l;
        break;
      }
    /* else fall through to COCO_CAT_STRING_REST */
  case COCO_CAT_STRING_REST:
    if (*line == '\0')
      return COCO_FALSE;
    acl_return->aastrrr = coco_strdup( line );
    line += strlen( line );
    break;
  }
  *line_read_return = line;
  return COCO_TRUE;
}




void
coco_console_read_and_handle( const char *line, void *ignore_a,
                              void *ignore_b )
{
  union coco_console_actual_arg *actual_args
    = (union coco_console_actual_arg *)
    coco_malloc( sizeof( union coco_console_actual_arg )
                 * COCO_CONSOLE_MAX_ARGS );
  struct coco_console_command *p;
  struct coco_console_command *c = &coco_console_default_command;
  struct coco_console_formal_arg *fml;
  union coco_console_actual_arg *acl;
  struct coco_console_task_stack_entry prompt_entries[COCO_CONSOLE_MAX_ARGS];
  int nprompt_entries = 0;
  int i;
  /* 1 from gobbling '/' */
  int len;
  coco_bool arg_read_mode = COCO_TRUE;

  /* call this function again after everything else */
  coco_console_push_eval_loop();

  /* skip whitespace */
  line = coco_console_skip_chars( line, coco_console_whitespace );

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

  { /* call continuation function after we've got the args */
    struct coco_console_task_stack_entry entry;
    entry.prompt = NULL;
    entry.prompt_malloced = COCO_FALSE;
    entry.pop_handler = coco_console_read_and_handle_rest;
    entry.pop_handy1 = actual_args;
    entry.pop_handy2 = c;
    entry.parser = NULL;
    coco_console_push_task( &entry );
  }


  for (fml = c->args, acl = actual_args; 
       fml->name != NULL; 
       fml++, acl++)
    {
      coco_cookie( "considering fml \"%s\"\n", fml->name );
      if (arg_read_mode)
        {
          if (coco_console_read_actual_arg( &line, fml, acl, COCO_FALSE ))
            continue;
          else arg_read_mode = COCO_FALSE;
        }
      switch (fml->optionality) {
      case COCO_CAO_PROMPT:
        { /* prompt for arg */
          char buf[100];
          struct coco_console_task_stack_entry *entry
            = prompt_entries + nprompt_entries;
          nprompt_entries++;
          sprintf( buf, "%s> ", fml->name );
          entry->prompt = coco_strdup( buf );
          entry->prompt_malloced = COCO_TRUE;
          entry->pop_handler = coco_console_fill_argument;
          entry->pop_handy1 = acl;
          entry->pop_handy2 = fml;
          entry->parser = fml->parser;
        }
        break;
      case COCO_CAO_DEFAULT:
        assert( fml->default_getter );
        *acl = fml->default_getter();
        break;
      case COCO_CAO_UNBOUND:
        switch (fml->type) {
        case COCO_CAT_INT: 
          acl->aaint = COCO_CONSOLE_INT_UNBOUND; break;
        case COCO_CAT_NAT:
          acl->aanat = COCO_CONSOLE_NAT_UNBOUND; break;
        case COCO_CAT_STRING:
          coco_cookie( "going unbound\n" );
          acl->aastr = COCO_CONSOLE_STRING_UNBOUND; break;
        case COCO_CAT_STRING_REST:
          acl->aastrrr = COCO_CONSOLE_STRING_UNBOUND; break;
        }
        break;
      }

    }
  /* push in reverse order */
  for (i=nprompt_entries-1; i>=0; i--)
    coco_console_push_task( prompt_entries+i );

  /* try to pop stuff that doesn't need input */
  coco_console_pop_with_line( NULL );

  /*
  for (i=0, fml = c->args; fml->name != NULL; fml++, i++)
    coco_cookie( "arg %d as int: %d\n", i, actual_args[i].aaint );
  */
}

void
coco_console_read_and_handle_rest( const char *isnull,
                                   void *handy1, void *handy2 )
{
  union coco_console_actual_arg *actual_args = handy1;
  struct coco_console_command *c = handy2;
  struct coco_console_formal_arg *fml;
  int i;

  assert( !isnull );
  assert( actual_args );
  assert( c );

  coco_cookie( "console_read_and_handle_rest for %s about to funcall\n",
               c->name );
  c->fn( c, actual_args );

  /* free char* actual args */
  for (i=0, fml = c->args; fml->name != NULL; fml++, i++)
    switch (fml->type) {
    case COCO_CAT_INT:
    case COCO_CAT_NAT:
      /* do nothing */
      break;
    case COCO_CAT_STRING:
    case COCO_CAT_STRING_REST:
      {
        char *aa = actual_args[i].aastr;
        if (aa != COCO_CONSOLE_STRING_UNBOUND)
          coco_free( aa );
      }
      break;
    }
  coco_free( actual_args );
}

void
coco_console_fill_argument( const char *line,
                            void *handy1, void *handy2 )
{
  struct coco_console_formal_arg *formal_arg = handy2;
  union coco_console_actual_arg *actual_arg = handy1;
  coco_console_read_actual_arg( &line, formal_arg, actual_arg, COCO_TRUE );
}


/* push a task */
void
coco_console_push_task( struct coco_console_task_stack_entry *entry )
{
  assert( entry );
  coco_cookie( "Pushing task \"%s\"\n",
               entry->prompt ? entry->prompt : "anonymous" );
  coco_console_task_stack[coco_console_task_stack_size++] = *entry;
}

void
coco_console_push_eval_loop( void )
{
  struct coco_console_task_stack_entry entry;
  entry.prompt = "> ";
  entry.prompt_malloced = COCO_FALSE;
  entry.pop_handler = coco_console_read_and_handle;
  entry.pop_handy1 = NULL;
  entry.pop_handy2 = NULL;
  entry.parser = coco_console_parse_any_string;
  coco_console_push_task( &entry );
}


/* pop a task, executing it for side effects */
void
coco_console_pop_exec_task( const char *line )
{
  struct coco_console_task_stack_entry *entry;
  assert( coco_console_task_stack_size > 0 );
  entry = coco_console_task_stack + --coco_console_task_stack_size;
  entry->pop_handler( line, entry->pop_handy1, entry->pop_handy2 );
  if (entry->prompt_malloced)
    coco_free( entry->prompt );
}

struct coco_console_task_stack_entry *
coco_console_peek_task( void )
{
  if (coco_console_task_stack_size > 0)
    return coco_console_task_stack + coco_console_task_stack_size - 1;
  else return NULL;
}



/* misnamed, called sometimes w/o return */
void
coco_console_pop_with_line( const char *line )
{
  const char *prompt;
  coco_bool popped = COCO_FALSE;
  if (line)
    {
      do
        {
          prompt = coco_console_peek_task()->prompt;
          coco_console_pop_exec_task( prompt ? line : NULL );
          popped = COCO_TRUE;
        }
      while (!prompt);
      line = NULL;
    }
  while (1)
    {
      prompt = coco_console_peek_task()->prompt;
      if (prompt) 
        break;
      coco_console_pop_exec_task( NULL );
      popped = COCO_TRUE;
    }
  assert( coco_console_peek_task() );
  assert( coco_console_peek_task()->prompt );
  coco_console_prepare_for_user_typing();
  if (popped)
    {
      coco_refresh_window_eventually();
    }
}
