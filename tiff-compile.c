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

/* tiff-compile.c -- take tiffs as input and spit out raster .h and .c files */

/* Currently compile with gcc -o tiff-compile tiff-compile.c */

#include <tiffio.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h> /* for directory */
#include <dirent.h>    /* for directory */
#include "raster.h"

FILE *log_file = NULL;

char *coco_file_header =
#include "c-header.strlit"
;

char *coco_no_edit = "/* File automatically generated, do not edit! */\n";

/* malloc and return an all-uppercase copy of str */
char *
coco_strup( char *str )
{
  int i;
  int len = strlen( str ) + 1;
  char *out = (char *) malloc( len * sizeof( char ) );
  for (i=0; i<len; i++)
    out[i] = toupper( str[i] );
  return out;
}



#define COCO_FAIL_ON_FAIL( boole, str ) if (!(boole)) { *error_descrip = str; return 0; }


/* returns NULL on fail
 * WARNING: returns a STATICALLY ALLOCATED pixel field in raster_return. 
 * Subsequent calls WILL corrupt previously returned rasters! */
uint32 *
coco_tiff_load( char *filename, uint32 *w, uint32 *h, 
                char **error_descrip )
{
  static uint32 *pixels = NULL;
  static tsize_t pixels_size = 0;
  tsize_t pixels_need;
  TIFF *tif = TIFFOpen( filename, "r" );
  if (!tif) return 0;
  COCO_FAIL_ON_FAIL( TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, w ),
                     "No width tag" );
  COCO_FAIL_ON_FAIL( TIFFGetField( tif, TIFFTAG_IMAGELENGTH, h ),
                     "No height tag" );
  pixels_need = *w * *h * sizeof( uint32 );
  if (pixels_size < pixels_need)
    {
      _TIFFfree( pixels );
      COCO_FAIL_ON_FAIL( pixels = (uint32 *) _TIFFmalloc( pixels_need ),
                         "Not enough memory for raster" );
      pixels_size = pixels_need;
    }
  COCO_FAIL_ON_FAIL( TIFFReadRGBAImage( tif, *w, *h, pixels, 1 ),
                     "Couldn't read image pixels" );
  /* printf( "w = %d, h = %d, pixels[0,0]=0x%x%x%x%x\n", w, h,
             TIFFGetR( pixels[0] ), TIFFGetG( pixels[0] ),
             TIFFGetB( pixels[0] ), TIFFGetA( pixels[0] ) ); */
  TIFFClose( tif );
  return pixels;
}


void
coco_write_raster_header( FILE *s, uint32 *pixels, uint32 w, uint32 h, 
                          enum coco_raster_format fmt, 
                          enum coco_raster_texmode texmode, 
                          char *name )
{
  char *upname = coco_strup( name );
  
  fputs( coco_file_header, s );
  fputs( coco_no_edit, s );
  fprintf( s, "#define COCO_%s_RASTER_W %ld\n", upname, w);
  fprintf( s, "#define COCO_%s_RASTER_H %ld\n", upname, h);
  fprintf( s, "\n" );
  fprintf( s, "extern struct coco_raster coco_%s_raster;\n", name );
  free( upname );
}

void
coco_write_raster_source( FILE *s, uint32 *p, uint32 w, uint32 h,
                          enum coco_raster_format fmt,
                          enum coco_raster_texmode texmode,
                          unsigned char trans_r,
                          unsigned char trans_g,
                          unsigned char trans_b,
                          unsigned char trans_a,
                          char *cname, char *fname )
{
  int x, y;
  char *f;
  int bitmap = fmt==COCO_BITMAP;
  char *upname = coco_strup( cname );
  char *fmtstring = NULL;
  char *texmodestring = NULL;

  switch (fmt) {
  case COCO_RGBA_8:       fmtstring="COCO_RGBA_8";       break;
  case COCO_RGBA_8_BLEND: fmtstring="COCO_RGBA_8_BLEND"; break;
  case COCO_BITMAP:       fmtstring="COCO_BITMAP";       break;
  };
  if (!fmtstring)
    {
      fprintf( stderr, "Unhandled raster format in %s:%d\n", 
               __FILE__, __LINE__ );
      exit( 1 );
    }
  switch (texmode) {
  case COCO_MULTIPLY: texmodestring="COCO_MULTIPLY"; break;
  case COCO_REPLACE:  texmodestring="COCO_REPLACE";  break;
  };
  if (!texmodestring)
    {
      fprintf( stderr, "Unhandled texture mode in %s:%d\n", 
               __FILE__, __LINE__ );
      exit( 1 );
    }

  fputs( coco_file_header, s );
  fputs( coco_no_edit, s );
  
  fprintf( s, "\n" );
  fprintf( s, "#include \"../raster.h\"\n" );
  fprintf( s, "#include \"%s.h\"\n", fname );
  fprintf( s, "\n" );
  
  f = bitmap
    ? "static unsigned char coco_pixels[(((COCO_%s_RASTER_W+31)>>5)<<2)*COCO_%s_RASTER_H] = {\n"
    : "static unsigned char coco_pixels[COCO_%s_RASTER_W*COCO_%s_RASTER_H*4] = {\n";

  fprintf( s, f, upname, upname );
  
  if (bitmap)
    for (y=0; y < h; y++)
      {
        int ww;

        fprintf( s, " " );

        ww = w;
        while (ww & 0x1F) ww++; /* word align */

        for (x=0; x < ww; x+=8)
          {
            unsigned char acc = 0;
            int subx;
            for (subx=0; subx<8; subx++)
              {
                unsigned char r, g, b, a;
                if (x<w)
                  {
                    uint32 pix = p[x+subx+(y*w)];
                    r = TIFFGetR( pix );
                    g = TIFFGetG( pix );
                    b = TIFFGetB( pix );
                    a = TIFFGetA( pix );
                  }
                else r = g = b = a = 0;
                if (!((r==trans_r)&&(g==trans_g)&&(b==trans_b)&&(a==trans_a)))
                  {
                    int luma = (r+g+b) * a / 255;
                    if (luma > 127)
                      acc |= 1 << (7 - subx);
                  }
              }
            fprintf( s, " %d,", acc );
          }
        fprintf( s, "\n" );
      }
  else /* not bitmap */
    for (y=0; y < h; y++)
      {
        fprintf( s, " " );
        for (x=0; x < w; x++)
          {
            uint32 pix = p[x+y*w];
            int r = TIFFGetR( pix );
            int g = TIFFGetG( pix );
            int b = TIFFGetB( pix );
            int a = TIFFGetA( pix );
            if (((r==trans_r)&&(g==trans_g)&&(b==trans_b)&&(a==trans_a)))
              fprintf( s, " 0,0,0,0," );
            else
              fprintf( s, " %d,%d,%d,%d,", r, g, b, a );
          }
        fprintf( s, "\n" );
      }
  fprintf( s, "};\n" );
  fprintf( s, "struct coco_raster coco_%s_raster = {\n", cname );
  fprintf( s, "  COCO_%s_RASTER_W,\n", upname );
  fprintf( s, "  COCO_%s_RASTER_H,\n", upname );
  fprintf( s, "  %s, %s, 0, coco_pixels\n", fmtstring, texmodestring );
  fprintf( s, "};\n" );
  free( upname );
}

/* extract filename from last slash to first period, and return in
   malloced string. */
char *
coco_basename( char *path )
{
  int i, end, start;
  char *str;
  size_t strl;
  size_t l = strlen( path );
  end = l;
  start = 0;
  for (i=l-1; i>=0; i--)
    {
      char ch = path[i];
      if ((ch == '.'))
        end = i;
      else if (ch == '/')
        { start = i+1; break; }
    }
  strl = end + 1 - start;
  str = (char *) malloc( strl * sizeof( char ) );
  if (NULL==str) return NULL;
  memcpy( str, path+start, strl-1 );
  str[strl-1] = '\0';
  return str;
}


/* returns 0 on fail */
int
coco_tiff_compile( char *infile, char *outh, char *outc, char *cname,
                   enum coco_raster_format fmt, 
                   enum coco_raster_texmode texmode, 
                   unsigned char trans_r,
                   unsigned char trans_g,
                   unsigned char trans_b,
                   unsigned char trans_a,
                   char **error_descrip )
{
  uint32 w, h;
  uint32 *pixels;
  FILE *f;
  char *bname;

  pixels = coco_tiff_load( infile, &w, &h, error_descrip );
  if (!pixels) return 0;
   
  if (log_file) { fprintf( log_file, "." ); fflush( log_file ) ; }
  f = fopen( outh, "w" );
  COCO_FAIL_ON_FAIL( f, "Couldn't open output header file" );
  coco_write_raster_header( f, pixels, w, h, fmt, texmode, cname );
  COCO_FAIL_ON_FAIL( 0==fclose( f ), "Couldn't close output header file" );
  f = fopen( outc, "w" );
  COCO_FAIL_ON_FAIL( f, "Couldn't open output src file" );
  bname = coco_basename( outh );
  COCO_FAIL_ON_FAIL( bname, "Couldn't malloc" );
  coco_write_raster_source( f, pixels, w, h, fmt, texmode,
                            trans_r, trans_g, trans_b, trans_a, cname, bname );
  free( bname );
  COCO_FAIL_ON_FAIL( 0==fclose( f ), "Couldn't close src header file" );
  return 1;
}

#undef COCO_FAIL_ON_FAIL

/* concatenates directory and file.  Extra is extry bytes of return value to malloc */
char *
coco_directory_plus_file( char *d, char *f, int extra )
{
  size_t dl = strlen( d );
  size_t fl = strlen( f );
  int start;
  char *a = (char *) malloc( (dl + fl + 2 + extra) * sizeof( char ) );
  if (NULL==a) return NULL;
  memcpy( a, d, dl );
  if (d[dl-1] != '/')
    {
      a[dl] = '/';
      start = dl+1;
    }
  else start = dl;
  memcpy( a+start, f, fl+1 );
  return a;
}


int /* returns success */
coco_tiff_names( char *infile, char *outdir, 
                 char **cname, char **outh, char **outc )
{
  size_t bn_len;
  char *bn;

  if (cname) *cname=NULL;
  if (outh)  *outh=NULL;
  if (outc)  *outc=NULL;

  bn = coco_basename( infile );
  if (!bn) return 0;
  bn_len = strlen( bn );

  if (cname)
    { 
      int i;
      *cname = (char *) malloc( (bn_len+1) * sizeof( char ) );
      if (!(*cname)) return 0;
      for (i=0; i<bn_len+1; i++)
        { 
          char ch = bn[i];
          (*cname)[i] = (ch=='-') ? '_' : ch;
        }
    }
  {
    int i;
    char **outa[2];
    char **outx;
    char suf;

    outa[0] = outh;
    outa[1] = outc;
    for (i=0, suf='h'; i<2; i++, suf='c')
      {
        outx=outa[i];
        if (outx)
          {
            *outx = coco_directory_plus_file( outdir, bn, 2 );
            if (!(*outx)) return 0;
            sprintf( (*outx)+strlen( *outx ), ".%c", suf );
            assert( *outx );
          }
      }
  }
  free( bn );
  return 1;
}


/* returns 0 on false, 1 on blend, 2 on black-means-transparent,
 * but use coco_tiff_is_black_coded for discriminating between 1 & 2. */
int
coco_is_tiff_extension( char *fname )
{
  int i;
  int l = strlen( fname );
  for (i=l-1; i>=0; i--)
    {
      char ch = fname[i];
      switch (ch) {
      case '.': return (0==strcasecmp( "tiff", fname+i+1 )) ? 2 :
        ((0==strcasecmp( "tif", fname+i+1 )) ? 1 : 0);
      case '/': return 0;
      default: break;
      }
    }
  return 0;
}

int 
coco_tiff_is_black_coded( char *fname )
{
  return 2==coco_is_tiff_extension( fname );
}



/* returns 0 on fail.
 * Rip through indir / *.tif, and write translated .c and .h files to
 * outdir.  trans_* name a single special color which will be translated
 * to (0,0,0,0).  Traditionally this is (0,0,0,255)/black.  rasters_h is a
 * is written with one entry per raster.  */
int 
coco_tiff_compile_dir( char *indir, char *outdir, 
                       enum coco_raster_format fmt, 
                       enum coco_raster_texmode texmode,
                       FILE *rasters_h )
{
  DIR *dir;
  struct dirent *entry;
  dir = opendir( indir );
  if (NULL == dir)
    return 0;
  while (1)
    {
      char *fname;
      entry = readdir( dir );
      if (NULL == entry)
        break;
      fname = entry->d_name;
      if (coco_is_tiff_extension( fname ))
        {
          char *error_descrip, *cname, *outh, *outc, *outh_base;
          char *pname = coco_directory_plus_file( indir, fname, 0 );
          if (!pname)
            break;
          if (!coco_tiff_names( pname, outdir, &cname, &outh, &outc ))
            break;
          if (!(cname && outh && outc)) 
            break;;
          coco_tiff_compile( pname, outh, outc, cname, fmt, texmode,
                             0, 0, 0, 
                             coco_tiff_is_black_coded( pname ) ? 255 : 0,
                             &error_descrip );
          outh_base = coco_basename( outh );
          if (!outh_base) break;
          fprintf( rasters_h, "#include \"%s.h\"\n", outh_base );
          free( outh_base );
          free( cname );
          free( outh );
          free( outc );
          free( pname );
        }

    }
  closedir( dir );
  return 1;
}


int 
coco_tiff_compile_system( char *indir, char *outdir )
{
  char *subdirs[4]
    = { "rgba-8", "rgba-8-blend", "rgba-8-blend-multiply", "bitmap" };
  enum coco_raster_format formats[4]
    = { COCO_RGBA_8, COCO_RGBA_8_BLEND, COCO_RGBA_8_BLEND, COCO_BITMAP };
  enum coco_raster_texmode texmodes[4]
    = { COCO_REPLACE, COCO_REPLACE, COCO_MULTIPLY, COCO_REPLACE };
  int i;
  char *rasters_h = coco_directory_plus_file( outdir, "rasters.h", 0 );
  FILE *rasters_h_fd;

  if (!rasters_h) return 0;

  rasters_h_fd = fopen( rasters_h, "w" );
  if (NULL==rasters_h) return 0;
  if (log_file) fprintf( log_file, "Compiling \".tiffs\" to \".c\"s" );
  for (i=0; i<4; i++)
    {
      char *subindir = coco_directory_plus_file( indir, subdirs[i], 0 );
      if (!subindir) return 0;
      if (!coco_tiff_compile_dir( subindir, outdir, formats[i], texmodes[i],
                                  rasters_h_fd ))
        fprintf( stderr, "problems with directory \"%s\"\n", subindir );
      free( subindir );
    }
  if (log_file) fprintf( log_file, "done.\n" );
  fclose( rasters_h_fd );

  free( rasters_h );
  return 1;
}


int
main( int argc, char **argv )
{
  log_file = stdout;
  if (argc != 3)
    {
      fprintf( stderr, "Needed exactly 2 command line arguments: "
               "indir and outdir\n" );
      return 1;
    }
  if (!coco_tiff_compile_system( argv[1], argv[2] ))
    {
      printf( "Note: encountered error\n" );
      return 1;
    }
  else return 0;
}
