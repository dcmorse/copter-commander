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
#include "shrapnel-32c.h"

static unsigned char coco_pixels[(((COCO_SHRAPNEL_32C_RASTER_W+31)>>5)<<2)*COCO_SHRAPNEL_32C_RASTER_H] = {
  0, 63, 0, 0,
  0, 127, 0, 0,
  0, 127, 0, 0,
  0, 255, 128, 0,
  0, 255, 192, 0,
  0, 255, 224, 28,
  0, 255, 240, 8,
  0, 127, 248, 16,
  0, 63, 252, 32,
  16, 31, 254, 64,
  120, 31, 255, 64,
  124, 31, 255, 192,
  254, 31, 255, 240,
  255, 223, 255, 248,
  255, 255, 255, 252,
  255, 255, 255, 252,
  127, 255, 255, 252,
  127, 255, 255, 254,
  63, 255, 255, 255,
  31, 255, 255, 248,
  7, 255, 255, 224,
  3, 255, 255, 128,
  1, 255, 254, 0,
  0, 255, 252, 0,
  0, 127, 248, 0,
  0, 127, 240, 0,
  0, 63, 224, 0,
  0, 31, 224, 0,
  0, 15, 224, 0,
  0, 7, 224, 0,
  0, 3, 224, 0,
  0, 1, 224, 0,
};
struct coco_raster coco_shrapnel_32c_raster = {
  COCO_SHRAPNEL_32C_RASTER_W,
  COCO_SHRAPNEL_32C_RASTER_H,
  COCO_BITMAP, 0, coco_pixels
};
