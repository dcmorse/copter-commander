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
#include "shrapnel-32a.h"

static unsigned char coco_pixels[(((COCO_SHRAPNEL_32A_RASTER_W+31)>>5)<<2)*COCO_SHRAPNEL_32A_RASTER_H] = {
  0, 7, 128, 0,
  0, 15, 128, 0,
  0, 15, 224, 0,
  0, 31, 254, 0,
  32, 63, 254, 0,
  48, 127, 255, 0,
  56, 63, 255, 0,
  56, 63, 255, 128,
  56, 63, 255, 192,
  48, 63, 255, 240,
  16, 31, 255, 248,
  0, 31, 255, 255,
  0, 15, 255, 255,
  0, 15, 255, 255,
  0, 15, 255, 254,
  0, 15, 255, 254,
  3, 31, 255, 255,
  7, 255, 255, 254,
  1, 255, 255, 254,
  15, 255, 255, 254,
  31, 255, 255, 252,
  127, 255, 255, 252,
  127, 255, 248, 248,
  255, 255, 248, 240,
  127, 255, 248, 240,
  31, 255, 240, 192,
  15, 127, 224, 0,
  0, 127, 224, 0,
  0, 63, 224, 0,
  0, 31, 192, 0,
  0, 15, 128, 0,
  0, 7, 128, 0,
};
struct coco_raster coco_shrapnel_32a_raster = {
  COCO_SHRAPNEL_32A_RASTER_W,
  COCO_SHRAPNEL_32A_RASTER_H,
  COCO_BITMAP, 0, coco_pixels
};
