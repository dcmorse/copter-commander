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
/* File automatically generated, do not edit! */

#include "../raster.h"
#include "shrapnel-32d.h"

static unsigned char coco_pixels[(((COCO_SHRAPNEL_32D_RASTER_W+31)>>5)<<2)*COCO_SHRAPNEL_32D_RASTER_H] = {
  0, 255, 192, 192,
  1, 255, 225, 192,
  1, 255, 247, 0,
  1, 255, 255, 128,
  3, 255, 252, 0,
  7, 255, 248, 0,
  7, 255, 248, 0,
  7, 255, 240, 0,
  15, 255, 224, 0,
  31, 255, 224, 0,
  63, 255, 224, 56,
  127, 255, 192, 120,
  255, 255, 192, 252,
  255, 255, 193, 254,
  255, 255, 199, 255,
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 255,
  255, 255, 255, 254,
  127, 255, 255, 252,
  31, 255, 255, 248,
  15, 255, 255, 240,
  7, 255, 255, 240,
  3, 255, 255, 224,
  0, 255, 255, 192,
  0, 31, 255, 128,
  0, 31, 254, 0,
  0, 31, 252, 0,
  0, 15, 248, 0,
  0, 7, 248, 0,
  0, 1, 240, 0,
  0, 0, 112, 0,
};
struct coco_raster coco_shrapnel_32d_raster = {
  COCO_SHRAPNEL_32D_RASTER_W,
  COCO_SHRAPNEL_32D_RASTER_H,
  COCO_BITMAP, 0, coco_pixels
};
