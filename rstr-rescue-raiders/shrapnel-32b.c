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
#include "shrapnel-32b.h"

static unsigned char coco_pixels[(((COCO_SHRAPNEL_32B_RASTER_W+31)>>5)<<2)*COCO_SHRAPNEL_32B_RASTER_H] = {
  255, 135, 224, 0,
  255, 231, 240, 0,
  127, 255, 240, 0,
  127, 255, 248, 0,
  63, 255, 252, 0,
  31, 255, 252, 0,
  31, 255, 255, 0,
  31, 255, 255, 0,
  31, 239, 255, 0,
  31, 193, 255, 128,
  31, 192, 255, 128,
  23, 128, 127, 192,
  27, 128, 127, 224,
  25, 0, 63, 224,
  60, 0, 51, 240,
  60, 1, 251, 248,
  62, 3, 251, 248,
  30, 3, 250, 252,
  159, 3, 251, 252,
  255, 3, 255, 252,
  255, 3, 250, 252,
  127, 129, 246, 124,
  127, 128, 240, 120,
  63, 128, 240, 124,
  31, 128, 224, 122,
  15, 192, 176, 93,
  7, 224, 48, 15,
  1, 240, 24, 3,
  0, 252, 12, 0,
  0, 60, 4, 0,
  0, 28, 0, 0,
  0, 56, 0, 0,
};
struct coco_raster coco_shrapnel_32b_raster = {
  COCO_SHRAPNEL_32B_RASTER_W,
  COCO_SHRAPNEL_32B_RASTER_H,
  COCO_BITMAP, 0, coco_pixels
};
