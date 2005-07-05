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

/* console-graphics.c -- support for terminal user interface */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "coco.h"
#include "rasters.h" /* for close, maximize, minimize */

enum coco_console_deployment_ coco_console_deployment;

void
coco_console_set_mode( enum coco_console_deployment_ d )
{
  static coco_bool first_time = COCO_TRUE;
  coco_bool refresh = coco_console_deployment != d;
  coco_console_deployment = d;
  if ((d > COCO_HIDDEN) && first_time)
    {
      first_time = COCO_FALSE;
      coco_console_push_eval_loop();
      coco_console_prepare_for_user_typing();
    }
  if (refresh) coco_refresh_window_eventually();
}

/* used by callers outside this file */
enum coco_console_deployment_
coco_console_get_mode( void )
{
  return coco_console_deployment;
}

void
coco_set_console_deployment( enum coco_console_deployment_ d )
{
  coco_console_deployment = d;
}

/* only relevant when not fullscreen */
#define COCO_CONSOLE_LINES 3 /* lines of text to display */

#define COCO_CONSOLE_X_MARGIN 5 /* pixels */
#define COCO_CONSOLE_Y_MARGIN 5 /* pixels */

/* actually 1 less than this because of sentinel */
#define COCO_CONSOLE_MAX_LINES 30

#define COCO_CONSOLE_MAX_CHARS_PER_LINE 512 /* max chars_per_line */

void
coco_console_metrics( int *x, int *y, int *w, int *h, struct coco_font **font,
                      int *nlines_ )
{ /* sheeesh, clean this up! */
  struct coco_font *ffont = &coco_default_font;
  int fis = COCO_FONT_INTERLINE_SPACING( ffont );
  int nlines;
  if (font) *font = &coco_default_font;
  switch (coco_console_deployment) {
  case COCO_HIDDEN: 
    nlines = 0; 
    if (x) *x=0; if (y) *y=0;
    if (w) *w=0; if (h) *h=0;
    if (nlines_) *nlines_ = nlines;
    return; /* ! */
  case COCO_MINIMAL: nlines = COCO_CONSOLE_LINES; break;
  case COCO_FULLSCREEN:
    nlines = ((coco_real_window_h-COCO_CONSOLE_Y_MARGIN) / fis);
    break;
  }
  if (nlines_) *nlines_ = nlines;
  if (x) *x = 0;
  if (w) *w = coco_real_window_w;
  if (coco_console_deployment == COCO_FULLSCREEN)
    {
      if (y) *y = 0;
      if (h) *h = coco_real_window_h;
    }
  else
    {
      coco_gnum hh = COCO_MAX( fis*nlines+COCO_CONSOLE_Y_MARGIN,
                               coco_real_window_h - COCO_DASHBOARD_Y );
      if (h) *h = hh;
      if (y) *y = (coco_real_window_h - hh);
    }
}




int coco_console_chars_insert_idx = 0;
/* big n*m "frame buffer" of chars.  The extra 1 char on the end is to
   detect memory corruption.  */
char coco_console_chars[COCO_CONSOLE_MAX_LINES
                       *COCO_CONSOLE_MAX_CHARS_PER_LINE + 1]
 = {((char) 1)}; /* (char) 1 terminated */

int coco_console_cursor_line = 0;
int coco_console_cursor_char = 0;

void
coco_console_debug_print( void )
{
  int i;
  for (i=0; i<COCO_CONSOLE_MAX_LINES; i++)
    {
      const char *line = coco_console_chars 
        + i * COCO_CONSOLE_MAX_CHARS_PER_LINE;
      coco_cookie( "%2d%s%s\n", i, 
                   (i==coco_console_chars_insert_idx) ? "*" : " ",
                   (*line == (char) 1) ? "^A" : line );
    }
}


void
coco_console_printf( const char *fmt, ... )
{
  char buf[COCO_CONSOLE_MAX_CHARS_PER_LINE*8];
  int scrbuflen;
  char *ptr;
  char *optr;
  va_list ap;
  va_start( ap, fmt );
  assert( !coco_console_chars[COCO_CONSOLE_MAX_LINES
                             * COCO_CONSOLE_MAX_CHARS_PER_LINE] );

  { /* if anything already on line, include it */
    char *scrbuf;
    int idx = coco_console_chars_insert_idx;
    scrbuf = coco_console_chars + (idx*COCO_CONSOLE_MAX_CHARS_PER_LINE);
    if (*scrbuf == ((char) 1))
      scrbuflen = 0;
    else
      {
        scrbuflen = strlen( scrbuf );
        /* coco_cookie( "copying found scrbuf string %s\n", scrbuf ); */
        memcpy( buf, scrbuf, scrbuflen );
      }
  }
  if (vsnprintf( buf+scrbuflen, (sizeof( buf ) / sizeof( char )) - scrbuflen,
                 fmt, ap ) 
      > COCO_CONSOLE_MAX_CHARS_PER_LINE)
    coco_warn_printf( "coco_console_printf had to truncate due to long"
                      " line\n" );
  for (optr=ptr=buf; 1; ptr++)
    {
      char ch = *ptr;
      if ((ch == '\n') || (ch == '\0'))
        {
          char *scrbuf;
          int dif;
          int idx = coco_console_chars_insert_idx;
          scrbuf = coco_console_chars + (idx*COCO_CONSOLE_MAX_CHARS_PER_LINE);
          dif = ptr-optr;
          memcpy( scrbuf, optr, dif*sizeof( char ) );
          scrbuf[dif] = '\0';
          /*
          coco_cookie( "line %d: copying %d chars of \"%s\"\n", 
                       idx, strlen( scrbuf ), scrbuf );
          */
          optr = ptr+1;
          if (ch == '\0')
            break;
          coco_console_chars_insert_idx = (idx+1) % COCO_CONSOLE_MAX_LINES;
        }
    }
  coco_console_chars[((coco_console_chars_insert_idx+1)
                      % COCO_CONSOLE_MAX_LINES)
                    * COCO_CONSOLE_MAX_CHARS_PER_LINE]
    = ((char) 1);
  assert( !coco_console_chars[COCO_CONSOLE_MAX_LINES
                             * COCO_CONSOLE_MAX_CHARS_PER_LINE] );
  va_end( ap );
}


void
coco_console_set_current_string( const char *string )
{
  /* This implementation doesn't work because it string-appends, not
     string-overwrites:
     coco_console_printf( "%s", string ); */
  /* This implementation is faster and works: */
  char *buf = coco_console_chars
    + (coco_console_chars_insert_idx*COCO_CONSOLE_MAX_CHARS_PER_LINE);
  strncpy( buf, string, COCO_CONSOLE_MAX_CHARS_PER_LINE );
  buf[COCO_CONSOLE_MAX_CHARS_PER_LINE-1] = '\0';
}

char coco_console_input_line[COCO_CONSOLE_MAX_CHARS_PER_LINE] = "";


void
coco_console_draw( void )
{
  struct coco_font *font;
  int x, y, w, h, x1, y1, line, nlines;
  if (coco_console_deployment == COCO_HIDDEN) 
    return;
  assert( !coco_console_chars[COCO_CONSOLE_MAX_LINES
                             * COCO_CONSOLE_MAX_CHARS_PER_LINE] );
  coco_console_metrics( &x, &y, &w, &h, &font, &nlines );
  x1 = x + w;
  y1 = y + h;
  coco_draw_color( COCO_COLOR_CONSOLE_BG );
  coco_draw_rectangle( x, y, x1-1, y1-1 );
  coco_draw_color( COCO_COLOR_CONSOLE_FG );
  coco_draw_rectangle_hollow( x, y, x1-1, y1-1 );
  line = 0;
  while (line < nlines)
    {
      int xx, yy;
      char *str = coco_console_chars
        + (((coco_console_chars_insert_idx - line + COCO_CONSOLE_MAX_LINES)
            % COCO_CONSOLE_MAX_LINES)
           * COCO_CONSOLE_MAX_CHARS_PER_LINE);
      if (((char) 1) == *str) break;
      xx = x + COCO_CONSOLE_X_MARGIN;
      yy = y + COCO_FONT_INTERLINE_SPACING( font )*line
        + COCO_CONSOLE_Y_MARGIN;
      if (line == coco_console_cursor_line)
        {
          int xxx, cl;
          cl = COCO_MIN( coco_console_cursor_char, strlen( str ) );
          xxx = xx + coco_font_string_width( str, cl, font );
          coco_draw_color( COCO_COLOR_RED );
          coco_draw_rectangle( xxx, yy-1, xxx + 3,
                               yy+coco_font_height( font )+1 );
          coco_draw_color( COCO_COLOR_CONSOLE_FG );
        }
      coco_draw_string( xx, yy, str, -1, font, COCO_BOTTOM );
      line++;
    }
  
  { /* draw wm icons */
    int i, ix;
    int space = 3;
    for (i=0, ix=x1; i<2; i++)
      {
        struct coco_raster *r = ((i==0) 
                                 ? &coco_close_raster 
                                 : (((coco_console_deployment==COCO_FULLSCREEN)
                                     ? &coco_minimize_raster
                                     : &coco_maximize_raster)));
        ix -= space + r->w;
        coco_draw_raster( ix, y1-space-r->h, r );
      }
  }
  assert( !coco_console_chars[COCO_CONSOLE_MAX_LINES
                             * COCO_CONSOLE_MAX_CHARS_PER_LINE] );
}



void
coco_console_set_current_string_from_input( void )
{
  char buf[COCO_CONSOLE_MAX_CHARS_PER_LINE];
  const char *prompt = coco_console_prompt();
  size_t prompt_len = strlen( prompt );
  strncpy( buf, prompt, COCO_CONSOLE_MAX_CHARS_PER_LINE );
  strncpy( buf+prompt_len,
           coco_console_input_line, 
           COCO_CONSOLE_MAX_CHARS_PER_LINE - prompt_len );
  buf[COCO_CONSOLE_MAX_CHARS_PER_LINE-1] = '\0';
  coco_console_set_current_string( buf );
}

void
coco_console_prepare_for_user_typing( void )
{
  const char *prompt;
  /*
  prompt = coco_console_prompt();
  coco_console_cursor_line = 0;
  coco_console_cursor_char = strlen( prompt );
  coco_console_set_current_string( prompt );
  coco_console_input_line[0] = '\0';
  */
  coco_console_printf( "\n" );
  coco_console_input_line[0] = '\0';
  coco_console_set_current_string_from_input();
  prompt = coco_console_prompt();
  coco_console_cursor_char = strlen( prompt );
}


void
coco_console_mouse_button( int x, int y, coco_key button, coco_bool is_press )
{
}


void
coco_console_key( coco_key key, char *string_or_null, 
                  coco_bool is_press )
{
  coco_bool need_refresh = COCO_FALSE;
  if (coco_key_special( key ))
    {
      if (COCO_BETWEENI( COCO_LPOINTER, key, COCO_5POINTER ))
        {
          struct coco_keyboard_pointer_state *state = 
            coco_get_keyboard_pointer_state();
          coco_console_mouse_button( state->pointer_x, state->pointer_y,
                                     key, is_press );
        }
      else if (key == COCO_KEY_TAB)
        coco_cookie( "do something with TAB\n" );
      else if (key == COCO_KEY_RETURN)
        {
          coco_console_printf( "\n" );
          coco_console_pop_with_line( coco_console_input_line );
        }
      else if (key == COCO_KEY_ESCAPE)
        coco_cookie( "do something with ESCAPE\n" );
      else if ((key >= COCO_KEY_SHIFT) && (key <= COCO_KEY_HYPER))
        ;
      else if (key == COCO_KEY_LEFT_ARROW)
        {
          const char *prompt = coco_console_prompt();
          size_t prlen = strlen( prompt );
          coco_console_cursor_char
            = COCO_MAX( prlen, coco_console_cursor_char - 1 );
          need_refresh = COCO_TRUE;
        }
      else if (key == COCO_KEY_RIGHT_ARROW)
        {
          const char *prompt = coco_console_prompt();
          coco_console_cursor_char 
            = COCO_MIN( strlen( coco_console_input_line ) + strlen( prompt ),
                        coco_console_cursor_char+1 );
          need_refresh = COCO_TRUE;
        }
      else if (key == COCO_KEY_UP_ARROW)
        coco_cookie( "do something with UP_ARROW\n" );
      else if (key == COCO_KEY_DOWN_ARROW)
        coco_cookie( "do something with DOWN_ARROW\n" );
      else if (key == COCO_KEY_DELETE)
        {
          const char *prompt = coco_console_prompt();
          size_t prlen = strlen( prompt );
          if (prlen < coco_console_cursor_char)
            {
              char ch;
              char *where;
              coco_console_cursor_char--;    
              where = coco_console_input_line
                + coco_console_cursor_char - prlen;
              do
                {
                  ch = *(where+1);
                  *where = ch;
                  where++;
                }
              while (ch!='\0');
              need_refresh = COCO_TRUE;
            }
          coco_console_set_current_string_from_input();
        }
      else coco_cookie( "unknown special key %d\n", (int) key );
    }
  else if (string_or_null && *string_or_null)
    {
      const char *prompt = coco_console_prompt();
      size_t prompt_len = strlen( prompt );
      coco_strsplice( coco_console_input_line, 
                      coco_console_cursor_char - prompt_len,
                      string_or_null );
      coco_console_set_current_string_from_input();
      coco_console_cursor_char += strlen( string_or_null );
      need_refresh = COCO_TRUE;
    }
  else
    coco_cookie( "non-special key %d has no associated string\n",
                 (int) key );
  if (need_refresh)
    {
      coco_refresh_window_eventually();
      coco_cookie( "refreshing eventually because of key-fu\n" );
    }
}
