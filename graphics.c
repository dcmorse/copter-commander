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


#include <GL/gl.h>
#include <string.h>
#include <assert.h>
#include "coco.h"

/* read here but set by window.h */
coco_bool coco_double_buffer = COCO_TRUE;

static void coco_begin_alpha_blending( void );
static void coco_end_alpha_blending( void );


/* changes to this order need to be reflected in the (e.g.) COCO_INDEX_BLUE
   defines in coco.h */
/* The Oooold palette, which you can delete next time you read this.
const coco_color coco_palette[256] = {
  COCO_COLOR_TRANSPARENT,
  COCO_COLOR_BLACK,
  COCO_COLOR_RED,
  COCO_COLOR_GREEN,
  COCO_COLOR_BLUE,
  COCO_COLOR_GREY,
  COCO_COLOR_BROWN,
  COCO_COLOR_LIGHT_BLUE,
  COCO_COLOR_ORANGE,
  COCO_COLOR_WHITE,
  COCO_COLOR_YELLOW,
  COCO_COLOR_GREY_C,
  COCO_COLOR_DARK_YELLOW,
  COCO_COLOR_DARK_RED,
  COCO_COLOR_KHAKI,
};
*/

const coco_color coco_palette[/* <=256 */] = {
  COCO_COLOR_TRANSPARENT,
  COCO_COLOR_RED,
  COCO_COLOR_BLUE,
  COCO_COLOR_GREEN,
  COCO_COLOR_LIGHT_BLUE,
  COCO_COLOR_BROWN,
};

void 
coco_draw_color( coco_color color )
{
  glColor4ub( COCO_RED( color ),
              COCO_GREEN( color ),
              COCO_BLUE( color ),
              COCO_ALPHA( color ));
}

void 
coco_clear_window_color( coco_color color )
{
  glClearColor( ((float) COCO_RED( color ))/255.0,
                ((float) COCO_GREEN( color ))/255.0,
                ((float) COCO_BLUE( color ))/255.0,
                ((float) COCO_ALPHA( color ))/255.0 );
}

void 
coco_clear_window( void )
{
  glClear( GL_COLOR_BUFFER_BIT );
#ifdef COCO_DEBUG
  coco_warn_gl_errors( "clear_window" );
#endif
}


/* ________________________________________________________________ */

#define COCO_RASTER_TEXTURE_ID( raster )              \
  ((raster)->texture_id                               \
   ? (raster)->texture_id                             \
   : coco_raster_texture_id_cache_miss( raster ))     \

unsigned
coco_raster_texture_id_cache_miss( struct coco_raster *raster )
{
  GLuint id;
  GLenum texmode_gl = GL_REPLACE;

  { /* check texture sizes correct. */
    static GLint maxtex = -1;
    int i, j, dim;
    if (maxtex<0) 
      glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxtex );
    for (j=0, dim=raster->w; j<2; j++, dim=raster->h)
      for (i=0; 1; i++)
        {
          int n = 1<<i;
          if (dim == n)
            break;
          else if ((dim < n) || (n > maxtex))
            coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM,
                               "%dx%d raster at %p isn't of size 2^n X "
                               "2^m as required by OpenGL texture mapping.\n",
                               raster->w, raster->h, raster );
        }
  }

  { /* check pixel format */
    enum coco_raster_format format = raster->format;
    assert( (format == COCO_RGBA_8) || (format == COCO_RGBA_8_BLEND) );
  }

  switch (raster->texmode) {
  case COCO_MULTIPLY: texmode_gl = GL_MODULATE; break;
  case COCO_REPLACE: texmode_gl = GL_REPLACE; break;
  }


  glGenTextures( 1, &id );
  raster->texture_id = id;

  glBindTexture( GL_TEXTURE_2D, id );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texmode_gl );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 
                raster->w,
                raster->h,
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                raster->pixels );

#ifdef COCO_DEBUG
  coco_warn_gl_errors( "coco_raster_texture_id_cache_miss" );
#endif /* COCO_DEBUG */
  return id;
}

#ifdef COCO_IGNORE
void
coco_reload_texture( struct coco_raster *raster, coco_gnum w, coco_gnum h )
{
  GLuint id = raster->texture_id;
  if (0==id) return;
  glPixelStorei( GL_UNPACK_ROW_LENGTH, raster->w );
  glBindTexture( GL_TEXTURE_2D, id );
  glTexSubImage2D( GL_TEXTURE_2D,
                   0,
                   0, 0,
                   w, h,
                   GL_RGBA, GL_UNSIGNED_BYTE,
                   raster->pixels );
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
#ifdef COCO_DEBUG
  coco_warn_gl_errors( "coco_reload_texture" );
#endif /* COCO_DEBUG */
}
#endif /* COCO_IGNORE */

void
coco_begin_rotated_view( coco_gnum x, coco_gnum y,
                         coco_angle angle )
{
  /* glMatrixMode(  GL_MODELVIEW ); the default -dm 05-11-2001 */
  glPushMatrix();
  if (x||y)
    glTranslatef( x, y, 0.0 );
  if (angle) 
    glRotatef( COCO_ANGLE_TO_FLOAT_DEGREES( angle ), 0.0, 0.0, 1.0 );
}

void
coco_end_rotated_view( void )
{
  /* glMatrixMode(  GL_MODELVIEW ); the default -dm 05-11-2001 */
  glPopMatrix();
}

void
coco_begin_scaled_view( float xstretch, float ystretch )
{

  glPushMatrix();
  if ((xstretch!=0.0) || (ystretch!=0.0))
    glScalef( xstretch, ystretch, 1.0 );
}

void
coco_end_scaled_view( void )
{
  glPopMatrix();
}



/* Like coco_draw_raster, but texture mapped, and therefore affected by
   coco_begin/end_rotated_view.  The center_x/y are transformed onto the
   rectangle to be drawn in the local coordinate system 
   (x0, y0): bottom left corner to draw
   (x1, y1): upper right corner, usually (x0+w,y0+h)
   (w, h):   width and height of raster's pixels to read, since OpenGL
             requires powers of 2 per side, this lets them be any size
*/
void
coco_draw_raster_texmap( coco_gnum x0, coco_gnum y0,
                         coco_gnum x1, coco_gnum y1,
                         coco_gnum w, coco_gnum h,
                         struct coco_raster *raster )
{
  GLubyte white[4] = {255, 255, 255, 255};
  enum coco_raster_format format = raster->format;
  coco_bool do_blending 
    = (format==COCO_RGBA_8_BLEND) && coco_blend_alpha;
  GLuint id;
  if (do_blending)
    coco_begin_alpha_blending();
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  id = COCO_RASTER_TEXTURE_ID( raster );
  /* Prevent mysterious bug with plasma/COCO_MULTIPLY copter/COCO_REPLACE */
  if (raster->texmode == COCO_REPLACE) glColor4ubv( white );
  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, id ); 
  glBegin( GL_QUADS );
  {
    unsigned int rw = raster->w;
    unsigned int rh = raster->h;
    GLfloat s = (rw>0) ? (((float) w) / rw) : 0.0;
    GLfloat t = (rh>0) ? (((float) h) / rh) : 0.0;
    glTexCoord2f( 0, 0 );
    glVertex2i( x0, y0 );
    glTexCoord2f( s, 0 );
    glVertex2i( x1, y0 );
    glTexCoord2f( s, t );
    glVertex2i( x1, y1 );
    glTexCoord2f( 0, t );
    glVertex2i( x0, y1 );
  }
  glEnd();
  glDisable( GL_TEXTURE_2D );
  if (do_blending)
    coco_end_alpha_blending();
#ifdef COCO_DEBUG
  coco_warn_gl_errors( "coco_draw_raster_texmap" );
#endif /* COCO_DEBUG */
}

/* ________________________________________________________________ */


void 
coco_draw_point( coco_gnum x, coco_gnum y )
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
  glBegin( GL_POINTS );
  glVertex2i( x, y );
  glEnd();
}

void 
coco_draw_big_point( coco_gnum x, coco_gnum y, int size )
{
  glPointSize( (float) size );
  glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
  glBegin( GL_POINTS );
  glVertex2i( x, y );
  glEnd();
  glPointSize( 1.0 );
}

void 
coco_begin_point_drawing( void )
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
  glBegin( GL_POINTS );
}

void 
coco_point_drawing_vertex( coco_gnum x0, coco_gnum y0 )
{
  glVertex2i( x0, y0 );
}

void 
coco_end_point_drawing( void )
{
  glEnd();
}

void 
coco_draw_rectangle( coco_gnum x0, coco_gnum y0,
                     coco_gnum x1, coco_gnum y1 )
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glRecti( x0, y0, x1, y1 );
}

void 
coco_draw_line( coco_gnum x0, coco_gnum y0, 
                coco_gnum x1, coco_gnum y1 )
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glBegin( GL_LINES );
  glVertex2i( x0, y0 );
  glVertex2i( x1, y1 );
  glEnd();
}


void 
coco_begin_line_drawing( void )
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glBegin( GL_LINES );
}


void 
coco_end_line_drawing( void )
{
  glEnd();
}

void 
coco_line_drawing_vertex( coco_gnum x0, coco_gnum y0, 
                          coco_gnum x1, coco_gnum y1 )
{
  glVertex2i( x0, y0 );
  glVertex2i( x1, y1 );
}

void 
coco_begin_line_strip_drawing( void )
{
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glBegin( GL_LINE_STRIP );
}


void 
coco_end_line_strip_drawing( void )
{
  glEnd();
}

void 
coco_line_strip_drawing_vertex( coco_gnum x0, coco_gnum y0 )
{
  glVertex2i( x0, y0 );
}


/* Set up matrices for lower-left orthographic drawing.  Called by the
 * windowing routines, which hopefully don't know too much about core
 * gl. */
void 
coco_setup_matricies( coco_gnum w, coco_gnum h )
{
  glViewport( 0, 0, w, h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0.0, w, 0.0, h, 10.0, -10.0 );
  glMatrixMode( GL_MODELVIEW ); /* restore default */
  glLoadIdentity();

  /* This translation avoids the "shimmering string" effect when rasters
     and glvertexons are drawn to the same place.  At nichimen we called
     it the magic gl number.  Its talked about somewhere in either the red
     or green books. */
  glTranslatef( 0.125, 0.0, 0.0 );

#ifdef COCO_DEBUG
  coco_warn_gl_errors( "setup_matricies" );
#endif
}

void
coco_setup_misc_graphics( void )
{
  const GLubyte *renderer;
  glDrawBuffer( coco_double_buffer ? GL_BACK : GL_FRONT );
  glDisable( GL_DITHER ); /* as recomended by mesa user faq */
  glEnable( GL_ALPHA_TEST );
  glAlphaFunc( GL_GREATER, 0.33 );

  renderer = glGetString( GL_RENDERER );

  if (-1==coco_blend_alpha)
    coco_blend_alpha =
      !(strstr( (const char *) renderer, "Mesa" ) 
        && strstr( (const char *) renderer, "Indirect" ));
  

  printf( "GL_VENDOR     %s\n", glGetString( GL_VENDOR ) );
  printf( "GL_RENDERER   %s\n", renderer );
  printf( "GL_VERSION    %s\n", glGetString( GL_VERSION ) );
#ifdef COCO_DEBUG
  printf( "GL_EXTENSIONS %s\n", glGetString( GL_EXTENSIONS) );
#endif

#ifdef COCO_DEBUG
  coco_warn_gl_errors( "setup_misc_graphics" );
#endif
}

/* Set rasterpos to X,Y even if that's off screen. */
void
coco_rasterpos( coco_gnum x, coco_gnum y )
{
#ifdef COCO_DEBUG
  coco_warn_gl_errors( "before rasterpos" );
#endif
  if ((x<0)||(y<0))
    {
      /* OpenGL idiom for drawing rasters with lower left corners
         off-window. */
      glRasterPos2i( 0, 0 );
      glBitmap( 0, 0, 0, 0, (GLfloat) x, (GLfloat) y, NULL );
    }
  else glRasterPos2i( x, y );
#ifdef COCO_DEBUG
  coco_warn_gl_errors( "rasterpos" );
#endif
}


static void
coco_begin_alpha_blending( void )
{
  glPushAttrib( GL_COLOR_BUFFER_BIT );
  glDisable( GL_ALPHA_TEST );
  glEnable( GL_BLEND );
  /* TIFFReadRGBAImage premultiplies alpha! */
  glBlendFunc( GL_ONE /* GL_SRC_ALPHA */,  GL_ONE_MINUS_SRC_ALPHA );
}

static void
coco_end_alpha_blending( void )
{
  glPopAttrib();
}


void 
coco_draw_raster( coco_gnum x, coco_gnum y, struct coco_raster *raster )
{
  enum coco_raster_format format = raster->format;
  coco_bool do_blending 
    = (format==COCO_RGBA_8_BLEND) && coco_blend_alpha;
  if (do_blending)
    coco_begin_alpha_blending();
#ifdef COCO_MESA_BUGLIENESS
  /* Mesa wierdness -- bitmaps don't draw right with this on, top 30%
     truncated.  */
  else if (format==COCO_BITMAP)
    glDisable( GL_ALPHA_TEST );
#endif
  coco_rasterpos( x, y );
  if (format==COCO_BITMAP)
    glBitmap( raster->w, raster->h, 0.0, 0.0, 0.0, 0.0, raster->pixels );
  else glDrawPixels( raster->w, raster->h, GL_RGBA, GL_UNSIGNED_BYTE, 
                     raster->pixels );
  if (do_blending)
    coco_end_alpha_blending();
#ifdef COCO_MESA_BUGLIENESS
  else if (format==COCO_BITMAP)
    glEnable( GL_ALPHA_TEST );
#endif
#ifdef COCO_DEBUG
  coco_warn_gl_errors( "draw_raster" );
#endif
}

void
coco_warn_gl_errors( char *label )
{
  while (1)
    {
      GLenum error = glGetError();
      if (GL_NO_ERROR == error) return;
      coco_fatal_printf( 0, "GL reports error %d (0x%x) in %s.\n",
                         error, error, label ); 
    }

}


/* _Font Support___________________________________________________ */

#if (COCO_APPEARANCE == COCO_APPEARANCE_RESCUE_RAIDERS)
struct coco_font coco_default_font = {
  "-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*", NULL, 0
};
#elif (COCO_APPEARANCE == COCO_APPEARANCE_DOODLE)
struct coco_font coco_default_font = {
  "-*-lucida-bold-i-*-*-12-*-*-*-*-*-*-*", NULL, 0
};
#else
#error COCO_APPEARANCE is not a value known to graphics.h
#endif

struct coco_font coco_fixed_font = {
  "-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*", NULL, 0
};

struct coco_font coco_huge_font = {
  "-*-lucida-bold-r-*-*-34-*-*-*-*-*-*-*", NULL, 0
};

/* Now featuring multi-line support. */
void 
coco_draw_string( coco_gnum x, coco_gnum y, 
                  const char *string, int nchars_or_minus_one,
                  struct coco_font *font, enum coco_font_origin origin )
{
  int string_cursor = 0;
  int i;
  char ch = 'x';
  int font_height = coco_font_height( font );
  int yoffset = 0;
  if (origin != COCO_BASELINE) 
    {
      int font_ascent;
      coco_font_string_dimensions( "", -1, font, NULL, NULL, &font_ascent );
      yoffset = (origin==COCO_TOP) ? font_ascent : (font_ascent - font_height);
    }
  glPushAttrib( GL_LIST_BIT
                | GL_COLOR_BUFFER_BIT ); /* alpha_test on by dflt in coco */
  glListBase( font->list_base );
  glDisable( GL_ALPHA_TEST );
  do 
    {
      i = string_cursor;
      while (1)
        { 
          ch=string[i];
          if ((ch < ' ') || (ch == (char) 127)) break; /* non-printable test */
          i++;
        };
      /* coco_cookie( "rasterpos %d %d | \"%s\" %d\n", x, y + yoffset, 
         string+string_cursor, i-string_cursor ); */
      coco_rasterpos( x, y - yoffset );
      if (-1 == nchars_or_minus_one)
        glCallLists( i-string_cursor, GL_BYTE, string+string_cursor );
      else
        {
          int char_count_bound = i-string_cursor;
          int brk, char_count;
          if (char_count_bound < nchars_or_minus_one)
            {
              char_count = char_count_bound;
              nchars_or_minus_one -= char_count_bound;
              brk = 0;
            }
          else 
            { 
              char_count = nchars_or_minus_one;
              brk = 1;
            }
          glCallLists( char_count, GL_BYTE, string+string_cursor );
          if (brk) break;
        }
      string_cursor = i+1; 
      if (ch == '\n') y -= font_height;
    }
  while (ch != '\0');
  glPopAttrib();
}



/* _Refresh Inhibit_(really a window issue)________________________ */

static coco_bool coco_inhibit_refresh = COCO_FALSE;

/* When true, expose events are ignored, presumably because some animation
   is going on.  returns the old value. */
coco_bool 
coco_set_inhibit_refresh( coco_bool inhibit )
{
  coco_bool old;
  old = coco_inhibit_refresh;
  coco_inhibit_refresh = inhibit;
  return old;
}

coco_bool 
coco_get_inhibit_refresh( void )
{
  return coco_inhibit_refresh;
}

