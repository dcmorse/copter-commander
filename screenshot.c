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

/* snapshot.c -- produce a screen snapshot
 */

#include <tiffio.h>
#include <GL/gl.h>
#include "coco.h"

/* create a screen capture file.  This code is under the GNU GPL */
coco_bool
coco_screenshot( char *filename )
{
  static unsigned char *pixels = NULL;
  int winw = coco_real_window_w;
  int winwp = ((winw+3)>>2)<<2; /* Padded to word */
  int winh = coco_real_window_h;
  int i;
  TIFF *tif = TIFFOpen( filename, "w" );
  if (!tif) return COCO_FALSE;
  TIFFSetField( tif, TIFFTAG_IMAGEWIDTH, (uint32) winw );
  TIFFSetField( tif, TIFFTAG_IMAGELENGTH, (uint32) winh );
  TIFFSetField( tif, TIFFTAG_PLANARCONFIG, (uint16) PLANARCONFIG_CONTIG );
  TIFFSetField( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
  TIFFSetField( tif, TIFFTAG_ROWSPERSTRIP, (uint32) 1 );
  TIFFSetField( tif, TIFFTAG_SAMPLESPERPIXEL, (uint16) 3 );
  TIFFSetField( tif, TIFFTAG_BITSPERSAMPLE, (uint16) 8, (uint16) 8, (uint16) 8 );
  { /* make sure pixels is right size */
    static int pixw = -1;
    static int pixh = -1;
    if ((pixw < winwp) || (pixh < winh))
      {
        size_t size = 3 * sizeof( char ) * winwp * winh;
        pixels = coco_realloc( pixels, size );
        if (NULL == pixels) return COCO_FALSE;
        pixw = winwp;
        pixh = winh;
      }
  }
  { /* Read framebuffer into pixels[]. This part works fine */
    glReadBuffer( GL_FRONT );
    glReadPixels( 0, 0, winw, winh, GL_RGB, GL_UNSIGNED_BYTE, pixels );
  }
  for (i=0; i<winh; i++)
    TIFFWriteScanline( tif, pixels + (3*(winh-i)*winwp), i, 0 );
  TIFFClose( tif );
  return COCO_TRUE;
}
