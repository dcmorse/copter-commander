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



#include <assert.h>
#include <GL/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "coco.h"

#ifdef COCO_USE_FREETYPE


static coco_bool freetype_init = COCO_FALSE;
static FT_Library library;

static const char *font_dir = "/usr/share/fonts/truetype/ttf-bitstream-vera/";


struct coco_freetype_metrics {
  int ascent; 			/* pixels */
  int descent;			/* pixels */
  int interline_spacing;	/* pixels */
  FT_Pos widths[COCO_FONT_NCHARS-' ']; /* in 64ths of a pixel fixed-point format */
};

/* returns freshly coco_malloc'ed memory, to be coco_freed by caller */
/* Assumes its argument is a MONOCHROME bitmap, 1 bit per pixel!! */
GLubyte *
translate_freetype_bitmap_to_gl( FT_Bitmap *ft )
{
  int i, j;
  int rows = ft->rows;
  /* bits_per_row rounded up to multiples of 32, as default in OpenGL */
  int words_wide = (ft->width + 31) >> 5;
  int bytes_wide = words_wide << 2;
  size_t px_bytes = bytes_wide * rows;
  GLubyte *px = (GLubyte *) coco_malloc( px_bytes );
  assert( ft->pixel_mode == FT_PIXEL_MODE_MONO );
  for (i=0;i<px_bytes;i++) px[i]=0; /* zero px */
  for (i=0; i < rows; i++)
    for (j=0; j < (ft->width+7)>>3; j++)
      {
	int px_ss = i*bytes_wide+j;
	int ft_ss = ((rows-i-1)*ft->pitch)+j;
	/*
	coco_cookie( "i=%2d j=%2d px[%4d] = ft->buffer[%4d]  px_bytes=%d\n", 
                     i, j, px_ss, ft_ss, px_bytes ); */
	assert( px_ss < px_bytes );
	assert( px_ss >= 0 );
	px[px_ss] = ft->buffer[ft_ss];
      }
  return px;
}

void
coco_load_font( struct coco_font *font )
{
  struct coco_freetype_metrics *metrics;
  FT_Error e;
  FT_Face face;
  /*
  coco_bool bold = font->flags & COCO_BOLD;
  coco_bool italic = font->flags & COCO_ITALIC;
  */
  char *font_pathname;
  int font_pathname_length = strlen( font->name ) + strlen( font_dir ) + 50;
  int i;

  if (!freetype_init)
    {
      e = FT_Init_FreeType( &library );
      if (e)
	coco_fatal_printf( COCO_ERROR_NO_FONT, "Freetype library died with"
			   " error code %d\n", e );
      freetype_init = COCO_TRUE;
    }
  font_pathname = (char *) coco_malloc( sizeof(char) * font_pathname_length );
  sprintf( font_pathname, "%s%s.ttf", font_dir, font->name );
  e = FT_New_Face( library, font_pathname, 0, &face );
  if (e)
    coco_fatal_printf( COCO_ERROR_NO_FONT, 
		       "FT_New_Face(...\"%s\"...) got error %d\n",
		       font_pathname, (int) e );
  coco_free( font_pathname ); 
  font_pathname = NULL;
  e = FT_Set_Pixel_Sizes( face, 0, font->height_request );
  if (e)
    coco_fatal_printf( COCO_ERROR_NO_FONT, 
		       "FT_Set_Pixel_Sizes(\"%s\"...%d ) got error %d\n",
		       font->name, font->height_request, (int) e );

  assert( FT_IS_SCALABLE( face ) ); /* otherwise height, ascent, etc undefined. */
  metrics = (struct coco_freetype_metrics *)
    coco_malloc( sizeof( struct coco_freetype_metrics ) );
  font->implementation_datum = (void *) metrics;
  metrics->ascent =            face->size->metrics.ascender  /64;
  metrics->descent =         (-face->size->metrics.descender)/64;
  metrics->interline_spacing = face->size->metrics.height    /64;

  coco_cookie( "Font \"%s\" ascent=%d, descent=%d interline_spacing=%d\n",
	       font->name,
	       metrics->ascent, metrics->descent, metrics->interline_spacing );

  /* 1-bit bitmaps... */

  /* conserve display lists: lists below 'SPACE' aren't drawn anyway,
     so subtract them from list_base */
  font->list_base = glGenLists( COCO_FONT_NCHARS - ' ' ) - ' ';

  for (i=' '; i<COCO_FONT_NCHARS; i++)
    {
      FT_GlyphSlot glyph;
      FT_Bitmap *pixmap;
      e = FT_Load_Char( face, (FT_ULong) i, FT_LOAD_MONOCHROME );
      if (e)
	coco_fatal_printf( COCO_ERROR_NO_FONT,
			   "FT_Load_Char( %s, '%c'=0x%X )=>error code %d\n",
			   font->name, (char) i, (int) i, (int) e);
      if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
	{
	  e=FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
	  if (e)
	    coco_fatal_printf( COCO_ERROR_NO_FONT,
			       "FT_Render_Glyph( %s, '%c'=0x%X )=>"
			       "error code %d\n",
			       font->name, (char) i, (int) i, (int) e);
	}
      glyph = face->glyph;
      pixmap=&(glyph->bitmap);

      metrics->widths[i-' '] = glyph->advance.x;

      if ((glyph->format == FT_GLYPH_FORMAT_OUTLINE)
	  && (' '==i))
	{
	  /* special case for ' ' SPACE character!  :P */
	  glNewList( font->list_base + i, GL_COMPILE );
	  glBitmap( 0, 0, 0, 0, ((float) glyph->advance.x)/64, ((float) glyph->advance.y)/64, NULL );
	  glEndList();
	}
      else if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
	coco_fatal_printf( COCO_ERROR_NO_FONT,
			   "Glyph still isn't a bitmap.  What's its deal?\n" );
      else
	{
	  GLubyte *gl_bitmap = translate_freetype_bitmap_to_gl( pixmap );
	  glNewList( font->list_base + i, GL_COMPILE );
	  assert (sizeof( GLubyte ) == sizeof( unsigned char ));
	  glBitmap( pixmap->width, pixmap->rows,
		    -glyph->bitmap_left, pixmap->rows - glyph->bitmap_top,
		    ((float)glyph->advance.x)/64, ((float)glyph->advance.y)/64,
		    gl_bitmap );
	  glEndList();
	  coco_free( gl_bitmap );
	}
    }
  FT_Done_Face( face );
}


void 
coco_font_string_dimensions( const char *string, int nchars_or_minus_one,
			     struct coco_font *font,
			     int *w, int *h, int *ascent )
{
  struct coco_freetype_metrics *metrics = font->implementation_datum;
  int i;
  int cursor = 0;
  int line_height = coco_font_interline_spacing( font );
  int hacc = line_height;
  FT_Pos wmax = 0;
  int chars_counted = 0;
  char ch;
  if (w||h)
    while (1)
      {
        FT_Pos line_width = 0;
	coco_bool break_again = COCO_FALSE;
        i = cursor;
        while (1) /* advance i looking for line terminators */
          { 
            ch=string[i];
            if ((ch=='\0') || (ch=='\n')
                || ((nchars_or_minus_one!=-1)
                    && (chars_counted >= nchars_or_minus_one))) 
	      {
		break_again = COCO_TRUE;
		break;
	      }
	    line_width += metrics->widths[ch-' '];
            i++;
            chars_counted++;
          };

        if (wmax < line_width) wmax = line_width;
        if (break_again)
	  break;
        if (string[i+1] != '\0') hacc += line_height;
        cursor = i + 1;
      }
  if (w) *w = wmax>>6;
  if (h) *h = hacc;
  if (ascent) *ascent = coco_font_ascent( font );
}

/*
void 
coco_font_string_dimensions( const char *string, int nchars_or_minus_one,
                             struct coco_font *font,
                             int *w, int *h, int *ascent )
{
  coco_cookie( "coco_font_string_dimensions( \"%s\", %d, %s, ",
	       string, nchars_or_minus_one, font->name );
  coco_font_string_dimensions_freetype( string, nchars_or_minus_one, 
					font, w, h, ascent );
  coco_cookie( "w=%d,h=%d,ascent=%d) \n", 
	       w?*w:-100, h?*h:-100, ascent?*ascent:-100 );
}
*/

int coco_font_height( struct coco_font *font )
{
  struct coco_freetype_metrics *metrics = font->implementation_datum;
  return metrics->ascent + metrics->descent;
}
int coco_font_ascent( struct coco_font *font )
{
  struct coco_freetype_metrics *metrics = font->implementation_datum;
  return metrics->ascent;
}
int coco_font_descent( struct coco_font *font )
{
  struct coco_freetype_metrics *metrics = font->implementation_datum;
  return metrics->descent;
}
int coco_font_interline_spacing( struct coco_font *font )
{
  struct coco_freetype_metrics *metrics = font->implementation_datum;
  return metrics->interline_spacing;
}

#endif /* COCO_USE_FREETYPE */
