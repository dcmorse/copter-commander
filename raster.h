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

#ifndef COCO_RASTER_H
#define COCO_RASTER_H

enum coco_raster_format {COCO_RGBA_8, COCO_RGBA_8_BLEND, COCO_BITMAP};

struct coco_raster {
  unsigned int w;
  unsigned int h;
  enum coco_raster_format format;
  unsigned texture_id; /* accessed by COCO_RASTER_TEXTURE_ID( raster ) */
  unsigned char *pixels;
};

#endif
