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


#include <stdarg.h> /* for debugging functions */
#include <stdio.h>  /* for snprint */
#include <stdlib.h> /* for strtol */
#include <ctype.h> /* for isdigit, et al */
#include <errno.h>
#include <assert.h>
#include <string.h> /* for strlen */
#include <alloca.h> /* for alloca.  Normally stdlib would get it, but
                       we're compiling -ansi */
#include "coco.h"


/* forward definitions */

typedef struct coco_env_elt coco_env;

static char *
coco_clear_whitespace( char *from );

static char *
coco_read_lnum( char *from, coco_env *env, coco_lnum *value_return );




#define COCO_COMMENT_CHAR ';'

/* Returns true if ch is a valid character in an identifier.  Identifiers,
   by the way, are strings of length 1 or more that do not start with
   digits (or + or - followed by a digit).  */
coco_bool 
coco_isidentifier( int ch )
{
  return ((isalpha( ch ) || isdigit( ch )
           || ('-'==ch) || ('+'==ch) || ('*'==ch) || ('/'==ch)
           || (':'==ch)));
}


/* COCO_ENV: its a symbol table, featuring extra fast order-n lookup of
   variable values.  

   Variables come from two places.  There are some universal constants
   like true, false, and good-guys.  Then there are the class-identifiers
   which change automatically whenever a new class is added. 

   Note that in the ccl language, functions live in a different namespace
   from variables.

   Note also that NULL is a legal env argument to all env functions.
*/

struct coco_env_elt {
  char *name;
  coco_lnum value;
  coco_env *next;
};


coco_bool 
coco_env_lookup( coco_env *env, char *name, coco_lnum *value_return )
{
  while (env)
    if (0==strcmp( name, env->name ))
      {
        *value_return = env->value;
        return COCO_TRUE;
      }
    else env = env->next;
  return COCO_FALSE;
}

coco_env *
coco_global_env( void )
{
  static coco_env static_env[] = {
    { "t",             COCO_TRUE,          static_env+1  },
    { "nil",           COCO_FALSE,         static_env+2  },
    { "good-guys",     COCO_GOOD_GUYS,     static_env+3  },
    { "bad-guys",      COCO_BAD_GUYS,      static_env+4  },
    { "neutral",       COCO_NEUTRAL,       static_env+5  },
    { "right",         1,                  static_env+6  },
    { "left",          -1,                 static_env+7  },
    { "center",        0,                  static_env+8  },
    { "ground-y",      COCO_GROUND_Y,      static_env+9  },
    { "balloon-y-min", COCO_BALLOON_FLOOR, static_env+10 },
    { "board-h",       COCO_VIEWABLE_H,    NULL          },
  };
  static coco_env *the_env = NULL;
  if (the_env)
    return the_env;
  else
    {
      int i;
      the_env = (coco_env *) coco_malloc( COCO_SOBJECT_CLASS_COUNT 
                                          * sizeof( coco_env ) );
      for (i=0; i<COCO_SOBJECT_CLASS_COUNT; i++)
        {
          the_env[i].name = coco_sobject_classes[i].name;
          the_env[i].value = coco_sobject_classes[i].cl_index;
          the_env[i].next = the_env + i + 1;
        }
      the_env[COCO_SOBJECT_CLASS_COUNT - 1].next = static_env;
      
      { /* check for name conflicts, to be on the safe side */
        coco_env *e = the_env;
        coco_lnum dummy;
        while (e)
          if (coco_env_lookup( e->next, e->name, &dummy ))
            coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                               "Name collision on \"%s\".  Either duplicated "
                               "class-names, or a class-name conflicts with "
                               "the name of a ccl built-in variable.",
                               e->name );
          else e = e->next;
      }

      return the_env;
    }
}

#ifdef COCO_DEBUG
void
coco_env_print( FILE *stream, coco_env *env )
{
  while (env != NULL)
    {
      fprintf( stream, "\"%s\" -> %d\n", env->name, (int) env->value );
      env = env->next;
    }
}

#endif

/* returns a pointer to the first char not consumed, or NULL on error.
   The value is stored in *buf_return (unless *buf_return is NULL).
   buf_return must be large enough to hold the entire identifier plus one
   extra char for the null.  coco_read_identifier_size can scan a buffer
   to determine the length of an identifier. */
char *
coco_read_identifier( char *from, char* buf_return )
{
  int i = 0;
  from = coco_clear_whitespace( from );
  while (coco_isidentifier( from[i] ))
    {
      if (buf_return) buf_return[i] = from[i];
      i++;
    }
  if (buf_return) buf_return[i] = '\0';
  /* strings starting with digits, or + or - followed by digits,
     are numbers not identifiers.  */
  if ((i>=1))
    {
      char ch = from[0];
      if (isdigit( ch ))
        return NULL;
      else if (((ch=='+')||(ch=='-'))
               && (i>=2)
               && isdigit( from[1] ))
        return NULL;
    }
  return i ? (from + i) : NULL;
}

/* Includes the size needed for the Null. Before it didn't and most
   callers forget too too. */
int
coco_read_identifier_size( char *from )
{
  int i = 0; 
  from = coco_clear_whitespace( from );
  while (coco_isidentifier( from[i] ))
    i++;
  return i + 1; /* changed -- add the null */
}



coco_lnum 
coco_op_plus( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  if (0==argument_count)
    return 0;
  else return coco_op_plus( env, arguments, argument_count - 1 ) 
         + arguments[argument_count-1];
}

coco_lnum 
coco_op_minus( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  if (1==argument_count)
    return - arguments[0];
  else
    return arguments[0] - coco_op_plus( env, arguments + 1,
                                        argument_count - 1 );
}

coco_lnum 
coco_op_times( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  if (0==argument_count)
    return 1;
  else return coco_op_times( env, arguments, argument_count - 1 ) 
         * arguments[argument_count-1];
}

coco_lnum 
coco_op_divide( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  coco_lnum numerator, denominator;
  if (1==argument_count)
    {
      numerator = 1;
      denominator = arguments[0];
    }
  else
    {
      numerator = arguments[0];
      denominator = coco_op_times( env, arguments + 1, argument_count - 1 );
    }
  if (0==denominator)
    coco_fatal_printf( COCO_ERROR_BAD_LEVEL_DESIGN,
                       "level attempted to get us to divide %d by 0.\n",
                       numerator );
  return numerator / denominator;
}

coco_lnum 
coco_op_width( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  return coco_sobject_classes[arguments[0]].canonical_w;
}

/* (xcen x class ) -> coordinate.
   Produces the x value necessary to put the center of CLASS at X.  */
coco_lnum 
coco_op_xcen( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  return arguments[0] + (coco_sobject_classes[arguments[1]].canonical_w>>1);
}

/* (left-center class args...) -> x-coordinate
   Add args together, then subtract half the width of class. */
coco_lnum
coco_op_left_center( coco_env *env, coco_lnum *arguments, int argument_count )
{
  return coco_op_plus( env, arguments+1, argument_count-1 )
    - (coco_sobject_classes[arguments[0]].canonical_w>>1);
}

/* (right-center class args...) -> x-coordinate
   (- board-w (/ (w class) 2) args...) */
coco_lnum
coco_op_right_center( coco_env *env, coco_lnum *arguments, int argument_count )
{
  coco_lnum width;
  coco_env_lookup( env, "board-w", &width );
  return width
    - coco_op_plus( env, arguments+1, argument_count-1 )
    - (coco_sobject_classes[arguments[0]].canonical_w>>1);
}


coco_lnum 
coco_op_random( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  return rand() % arguments[0];
}

/* returns last argument */
coco_lnum 
coco_op_print( coco_env *env, coco_lnum *arguments, int argument_count  )
{
  int i;
  printf( "[ccl print] " );
  for (i=0; i<argument_count; i++)
    printf( "%d ", (int) arguments[i] );
  printf( "\n" );
  return arguments[argument_count-1];
}


struct coco_operator {
  char *name;
  int min_argument_count;
  coco_bool takes_more_args_p;
  coco_lnum (* op)( coco_env *env, coco_lnum *arguments, int argument_count );
};

#define COCO_OPERATORS_COUNT 10

static struct coco_operator coco_operators[COCO_OPERATORS_COUNT] = {
  { "+",            0, COCO_TRUE,  coco_op_plus },
  { "-",            1, COCO_TRUE,  coco_op_minus },
  { "*",            0, COCO_TRUE,  coco_op_times },
  { "/",            1, COCO_TRUE,  coco_op_divide },
  { "w",            1, COCO_FALSE, coco_op_width },
  { "left-center",  1, COCO_TRUE,  coco_op_left_center },
  { "right-center", 1, COCO_TRUE,  coco_op_right_center },
  { "random",       1, COCO_FALSE, coco_op_random },
  { "print",        1, COCO_TRUE,  coco_op_print },
};

/* returns a pointer to the first non-whitespace character.  Never fails,
   assuming its input is readable memory.  */
char *
coco_clear_whitespace( char *from )
{
  coco_bool comment_gobble = COCO_FALSE;
  if (NULL==from) 
    return NULL;
  while (1)
    {
      char ch = *from;
      if (comment_gobble)
        {
          if ('\n' == ch)
            comment_gobble = COCO_FALSE;
          else if ('\0' == ch)
            return from;
        }
      else if (isspace( ch ))
        ; /* noop */
      else if (COCO_COMMENT_CHAR == ch)
        comment_gobble = COCO_TRUE;
      else return from;
      from++;
    }
}


/* read coco_lnums from buf until a character equal to delimiter is
   encoutnered.  Allocate (using coco_malloc) an array and store those
   coco_lnums in it.  Store the array address in *arg_return, and its length in
   *nargs_return.  Return a pointer to the first char not consumed by
   coco_read_delimited_list.  If an error occurs, NULL is returned, and
   *args_return is set to NULL, and nargs_return is set to 0.  */
char *
coco_read_delimited_list( char delimiter, char *buf, coco_env *env, 
                          coco_bool length_as_first_elt,
                          coco_lnum **arg_return, int *nargs_return )
{
  char *cursor = buf;
  coco_lnum dummy;
  int count = 0;
  int count_and_length;
  int i;

  length_as_first_elt = !!length_as_first_elt;

  /*
  coco_cookie( "read_delimited_list@%d\n",
               buf - coco_level_text ); */

  while (1)
    {
      cursor = coco_read_lnum( cursor, env, &dummy );
      if (NULL == cursor) break;
      count++;
    }
  count_and_length = count + length_as_first_elt;

  *arg_return = (coco_lnum *) coco_malloc( count_and_length * sizeof( coco_lnum ) );
  *nargs_return = count_and_length;

  if (length_as_first_elt)
    (*arg_return)[0] = count;
  for (i=0, cursor=buf; i<count; i++)
    {
      cursor = coco_read_lnum( cursor, env, (*arg_return) + i
                               + length_as_first_elt);
      /* we read count coco_lnums before, we should be able to read them again. */
      assert( cursor );
    }

  assert( cursor >= buf );

  cursor = coco_clear_whitespace( cursor );

  if (*cursor == delimiter)
    return cursor + 1;
  else 
    {
      coco_cookie( "looking at the wrong thing '%c' %d\n", *cursor,
                   cursor - coco_level_text );
      coco_free( *arg_return );
      *arg_return = NULL;
      *nargs_return = 0;
      return NULL;
    }
}


/* returns a pointer to the first char not consumed, or NULL on error.
   The value is stored in *value_return. */
char *
coco_read_lnum( char *from, coco_env *env, coco_lnum *value_return )
{
  int ch;

  from = coco_clear_whitespace( from );
  ch = *from;

  if (isdigit( ch ) 
      || (((ch=='-') || (ch=='+'))
          && isdigit( from[1] )))
    {
      char *endptr;
      *value_return = strtol( from, &endptr, 0 );
      return (endptr<=from) ? NULL : endptr;
    }
  else if (coco_isidentifier( ch ))
    {
      int idsize;
      char *name_buf;
      char *after_identifier;
      idsize = coco_read_identifier_size( from ) * sizeof( char );
      name_buf = (char *) alloca( idsize );
      if (NULL==name_buf) name_buf = coco_malloc( idsize );
      after_identifier = coco_read_identifier( from, name_buf );
      if (coco_env_lookup( env, name_buf, value_return ))
        return after_identifier;
      else
        {
          coco_warn_printf( "unbound variable %s\n", name_buf );
          return NULL;
        }
    }
  else if ('(' == ch)
    {
      char *operator_name;
      int i, nargs, idsize;
      coco_lnum *args;
      
      idsize = coco_read_identifier_size( from+1 ) * sizeof( char );
      if (0>=idsize) return NULL;
      operator_name = alloca( idsize );
      if (NULL==operator_name) operator_name = coco_malloc( idsize );
      from = coco_read_identifier( from+1, operator_name );
      if (!from) return NULL;

      from = coco_read_delimited_list( ')', from, env, COCO_FALSE,
                                       &args, &nargs );
      if (NULL==from) return NULL;
      for (i=0; i<COCO_OPERATORS_COUNT; i++)
        if (0==strcmp( operator_name, coco_operators[i].name ))
          {
            if (coco_operators[i].takes_more_args_p
                ? (nargs >= coco_operators[i].min_argument_count)
                : (nargs == coco_operators[i].min_argument_count))
              {
                *value_return = coco_operators[i].op( env, args, nargs );
                coco_free( args );
                return from;
              }
            else
              { 
                coco_warn_printf( "wrong number of arguments (%d) to "
                                  "function %s\n",
                                  nargs, coco_operators[i].name );
                coco_free( args );
                return NULL;
              }
          }
      coco_free( args );
      coco_warn_printf( "Attempt to call an unknown function %s.\n",
                        operator_name );
      return NULL;
    }
  else return NULL; /* its some unreadable char, maybe nul. */
}

char *
coco_load_level_error( char *from, const char *fmt, ... )
{
  char *buf = coco_malloc( 1000 );
  va_list ap;
  va_start(ap, fmt);
  if (from)
    sprintf( buf,    "(while looking at \"%c%c%c\") ", 
             from[0], from[1], from[2] );
  else sprintf( buf, "(while looking at NULL) " );
  vsprintf ( buf+strlen( buf ), fmt, ap );
  va_end( ap );
  return buf;
}

/* if an error occurs, and error_message is non-null, stores a freshly
   malloced error string in error_message.  Otherwise errors are fatal. */
char *
coco_map_level_text( char *filename, char **error_message )
{
  FILE *stream;
  int size = 0;
  char *buf = NULL;
  int i;
  
  stream = fopen( filename, "r" );
  if (NULL==stream)
    {
      char *err_string = coco_errno_string( errno );
      if (NULL==error_message)
        coco_fatal_printf( COCO_ERROR_LEVEL_FILE_NOT_FOUND,
                           "Couldn't open \"%s\" for reading: %s\n",
                           filename, err_string );
      else 
        {
          *error_message = coco_string_malloc_copy( err_string );
          return NULL;
        }
    }


  i=0;
  while (1)
    {
      int ch = fgetc( stream );
      if (i==size)
        {
          size += 1024;
          buf = coco_realloc( buf, size );
        }
      if (EOF == ch) break;
      buf[i] = ch;
      i++;
    }
  buf[i] = '\0';
  return buf;
}


#define COCO_LEVEL_FILE_FORMAT_HACKING_MAX_UNITS 1000

/* returns NULL on success, or a coco_malloc allocated failure string on
   failure.  If the passed sstate_return is NULL, it means just check for
   syntax without actually building permanent sstate data structures.  */
char *
coco_load_level_definition( char *from, coco_sstate *sstate_return )
{
  char *level_name;
  coco_bool syntax_check_only = sstate_return == NULL;
  int ident_size;
  char *macro_buf;
   from = coco_clear_whitespace( from );
  if (*from == '(')
    from++;
  else return coco_load_level_error( from,
                                     "First non-whitespace char not"
                                     " an open-paren." );
  ident_size = coco_read_identifier_size( from ) * sizeof( char );
  if (0>=ident_size) 
    {
      char *msg = coco_malloc( 200 );
      sprintf( msg, "Couldn't find identifier after top level open paren."
               " looking at \"%c%c%c\"", from[0], from[1], from[2] );
      return msg;
    }
  macro_buf = alloca( ident_size );
  if (NULL==macro_buf) macro_buf = coco_malloc( ident_size );
  from = coco_read_identifier( from, macro_buf );
  if (NULL==from) 
    return coco_string_malloc_copy( "Coudln't find identifier after "
                                    "top level open paren." );
  if (0!=strcmp( macro_buf, "define-coco-level" ))
    {
      char *msghead = "Unrecognised top level form ";
      int taillen = strlen( macro_buf );
      char *msg = (char *) coco_malloc( strlen( msghead ) + taillen + 1 );
      sprintf( msg, "%s%s", msghead, macro_buf );
      return msg;
    }

  ident_size = coco_read_identifier_size( from );
  if (ident_size>0) 
    {
      level_name = syntax_check_only 
        ? NULL : coco_malloc( ident_size * sizeof( char ) );
      from = coco_read_identifier( from, level_name );
    }
  else coco_load_level_error( from, "missing level name" );
  {
    coco_sstate sstate;
    coco_lnum versions[2];
    coco_lnum lev_version, width;
    int i;
    for (i=0; i<2; i++)
      {
        from = coco_read_lnum( from, NULL, versions + i );
        if (NULL==from)
          return coco_load_level_error( from,
                                        "No version numbers" );
      }
    if (!coco_version_compatible_p( versions[0], versions[1], 
                                    COCO_LEVEL_FILE_FORMAT ))
      {
        char *buf = malloc( 100 );
        sprintf( buf, "Incompatible with version %d.%d\n",
                 (int) versions[0], (int) versions[1] );
        return buf;
      }
    from = coco_read_lnum( from, NULL, &lev_version );
    if (NULL==from)
      return coco_string_malloc_copy( "Couldn't find level version" );
    from = coco_read_lnum( from, NULL, &width );
    if (NULL==from)
      return coco_string_malloc_copy( "Couldn't find level width" );
    from = coco_clear_whitespace( from );

    if (!syntax_check_only)
      coco_init_sstate( &sstate, width );

    if ('(' != *from)
      return coco_load_level_error( from, "Missing open paren in units list" );
    from++;
    from = coco_clear_whitespace( from );
    {
      char *unit_list_from = from;
      int unit_count = 0;
      int i;
      char *name_table[COCO_LEVEL_FILE_FORMAT_HACKING_MAX_UNITS];
      coco_bool found_a_landing_pad = COCO_FALSE;
      for (i=0; i<COCO_LEVEL_FILE_FORMAT_HACKING_MAX_UNITS; i++)
        name_table[i] = NULL;
      /* determine number of units and note any labels in name_table */
      while (1) 
        {
          coco_bool exit_while = COCO_FALSE;
          switch (*from) {
          case '(':
            {
              int namelength;
              
              from++;

              namelength = coco_read_identifier_size( from );
              name_table[unit_count]
                = coco_malloc( namelength * sizeof( char ) );
              from = coco_read_identifier( from, name_table[unit_count] );
              if (0==(strcmp( name_table[unit_count], "nil" )))
                {
                  coco_free( name_table[unit_count] );
                  name_table[unit_count] = NULL;
                }
              /* coco_cookie( "Read %s\n", name_table[unit_count] ); */
              from = coco_clear_whitespace( from );
              if (*from == '(')
                from++;
              else return coco_load_level_error( from,
                                                 "Missing open paren inside "
                                                 "unit list.\n" );

              /*
                coco_cookie( "oi, looking at \"%c%c%c\"\n",
                from[0], from[1], from[2] );
              */

              { /* fast forward past the unit initializtion list */
                int paren_depth = 1;
                do
                  {
                    char ch;
                    from = coco_clear_whitespace( from );
                    ch = *from;
                    if ('('==ch)
                      paren_depth++;
                    else if (')'==ch)
                      paren_depth--;
                    from++;
                  }
                while (paren_depth > 0);
              }

              if (*from == ')')
                from = coco_clear_whitespace( from+1 );
              else return coco_load_level_error( from, "Missing close paren" );
              unit_count++;
            }
            break;
          case ')':
            from = coco_clear_whitespace( from+1 );
            exit_while = COCO_TRUE;
            break;
          default:
            coco_load_level_error( from, "unexpected char in middle of unit list." );
          }
          if ((exit_while)
              || (unit_count >= COCO_LEVEL_FILE_FORMAT_HACKING_MAX_UNITS))
            break;
        }

      {
        /* add labels to environment. */
        coco_env *global_env = coco_global_env();
        coco_env *local_env = NULL;
        int env_augment_size = 1; /* for board-w */
        int i, j, unt;
        for (i=0; i<unit_count; i++)
          if (name_table[i]) env_augment_size++;
        local_env = alloca( env_augment_size * sizeof( coco_env ) );
        if (NULL == local_env)
          local_env = coco_malloc( env_augment_size * sizeof( coco_env ) );
        for (i=0; i<env_augment_size; i++)
          local_env[i].next = local_env + i + 1;
        local_env[env_augment_size-1].next = global_env;

        local_env[0].name = coco_string_malloc_copy( "board-w" );
        local_env[0].value = width;

        for (i=0, j=1; i<unit_count; i++)
          if (name_table[i])
            {
              local_env[j].name = name_table[i];
              local_env[j].value = i;
              j++;
            }
        assert( j == env_augment_size );
        /* debugging cookie: coco_env_print( stderr, local_env ); */
        /* rewind to beginning */
        from = unit_list_from;

        unt = 0;
        while (1) 
          {
            coco_bool exit_while = COCO_FALSE;
            switch (*from) {
            case '(':
              {
                coco_lnum *args;
                int nargs;
              
                from++;

                from = coco_read_identifier( from, NULL );
                from = coco_clear_whitespace( from );
                if (*from == '(')
                  from++;
                else return coco_load_level_error( from,
                                                   "Missing open paren inside "
                                                   "unit list.\n" );
                from = coco_read_delimited_list( ')', from, local_env, 
                                                 COCO_FALSE,
                                                 &args, &nargs );
                if (NULL==from)
                  return coco_load_level_error( from,
                                                "error reading unit %s (#%d)",
                                                name_table[unt]
                                                ? name_table[unt]
                                                : "nil",
                                                unt );
                if (*from == ')')
                  from = coco_clear_whitespace( from+1 );
                else return coco_load_level_error( from, "Missing close paren" );
                { /* make the new unit */
                  coco_sobject *sobject
                    = syntax_check_only 
                    ? NULL
                    : coco_sstate_allocate_sobject( &sstate );
                  coco_sobject_cl *cla;
                  coco_sobject *(* fn)
                    (coco_sobject *, coco_sobject_cl *, coco_lnum *);
                  if (!COCO_BETWEENI( 0, args[0],
                                      COCO_SOBJECT_CLASS_COUNT-1 ) )
                    coco_load_level_error( from, "unit %s (#%d) has an out "
                                           "of bound class index: %d.",
                                           name_table[unt]
                                           ? name_table[unt] : "nil",
                                           unt, args[0] );

                  cla = coco_sobject_classes + args[0];
                  fn = cla->init_by_list_fn;
                  /* coco_cookie( "looking at class %s\n", cla->name ); */
                  if (!fn)
                    coco_load_level_error( from, "unit %s (#%d) is of a class"
                                           " %s which does not have an"
                                           " init-fn.",
                                           name_table[unt]
                                           ? name_table[unt] : "nil",
                                           unt,
                                           cla->name );
                  if (nargs-1 != cla->init_by_list_count)
                    coco_load_level_error( from, "unit %s (#%d) is of a class"
                                           " %s which does not have an"
                                           " init-fn.",
                                           name_table[unt]
                                           ? name_table[unt] : "nil",
                                           unt,
                                           cla->name );
                  if (coco_sobject_subtype_p( cla, COCO_LANDING_PAD_CLASS ))
                    found_a_landing_pad = COCO_TRUE;
                  if (!syntax_check_only)
                    fn( sobject, cla, args+1 );
                  coco_free( args );
                }
                unt++;
              }
              break;
            case ')':
              from = coco_clear_whitespace( from+1 );
              exit_while = COCO_TRUE;
              break;
            default:
              coco_load_level_error( from, "unexpected char in middle of unit list." );
            }
            if ((exit_while)
                || (unit_count >= COCO_LEVEL_FILE_FORMAT_HACKING_MAX_UNITS))
              break;
          }
      }
      if (syntax_check_only)
        {
          if (!found_a_landing_pad)
            return coco_string_malloc_copy( "Couldn't find a single landing "
                                            "pad in any of the units." );
        }
      else
        { /* find landing pads (won't fail on syntax_check_only, but
             hopefully we've already scanned that) */
          coco_gnum min_pad_x = COCO_GNUM_MAX;
          coco_gnum max_pad_x = COCO_GNUM_MIN;
          int min_pad_idx = -1;
          int max_pad_idx = -1;
          COCO_DO_SSTATE_OBJECTS( sobject, &sstate, {
            if (coco_sobject_subtype_p( COCO_SOBJECT_CL( sobject ), 
                                        COCO_LANDING_PAD_CLASS ))
              {
                coco_gnum x = COCO_SOBJECT_X( sobject );
                if (x < min_pad_x)
                  {
                    min_pad_x = x;
                    min_pad_idx = COCO_SOBJECT_INDEX( sobject );
                  }
                if (x > max_pad_x)
                  {
                    max_pad_x = x;
                    max_pad_idx = COCO_SOBJECT_INDEX( sobject );
                  }
              }
          });
          if (-1==min_pad_idx)
            return coco_string_malloc_copy( "Couldn't find any landing pads." );
          else
            {
              sstate.teams[0].landing_pad_index = max_pad_idx;
              sstate.teams[1].landing_pad_index = min_pad_idx;
            }
        }
      if (!syntax_check_only)
        *sstate_return = sstate;
      return NULL;
    }
  }
}


#ifdef COCO_IGNORE
void /* an old test suite */
ccl_test( void )
{
  coco_sstate *sstate;
  char *error;
  char *level = "(define-coco-level cherbourg\n"
    "1 0                                 ; coco-version\n"
    "1                                   ; level-version\n"
    "7000                                ; width\n"
    "((my-hut (hut 1000 neutral))\n"
    " (nil    (bunker 2000 good-guys 2))\n"
    " (nil    (bunker (- board-w 2000 (w bunker)) good-guys 2))))\n";
  /*
  char *cursor = level;
  while (1)
    {
      cursor = coco_read_lnum( cursor, env, &val );
      if (cursor)
        coco_cookie( "read %d\n", (int) val );
      else break;
    }
  */
  error = coco_load_level_definition( level, &sstate );
  if (error)
    coco_cookie( "ccl_test done.  Found error \"%s\".\n", error );
  else coco_cookie( "ccl_test done.  No errors detected.\n", error );
}
#endif /* COCO_IGNORE */

