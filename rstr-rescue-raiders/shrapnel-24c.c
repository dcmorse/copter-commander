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
#include "shrapnel-24c.h"

static unsigned char coco_pixels[(((COCO_SHRAPNEL_24C_RASTER_W+31)>>5)<<2)*COCO_SHRAPNEL_24C_RASTER_H] = {
  248, 0, 4, 0,
  248, 1, 4, 0,
  252, 7, 0, 0,
  252, 7, 128, 0,
  255, 7, 192, 0,
  255, 135, 240, 0,
  255, 199, 254, 0,
  255, 255, 254, 0,
  255, 255, 254, 0,
  255, 255, 0, 0,
  255, 254, 0, 0,
  63, 252, 0, 0,
  31, 254, 0, 0,
  31, 255, 0, 0,
  31, 255, 128, 0,
  31, 255, 192, 0,
  31, 239, 192, 0,
  31, 231, 192, 0,
  15, 227, 224, 0,
  7, 193, 240, 0,
  3, 192, 248, 0,
  0, 0, 124, 0,
  0, 0, 126, 0,
  96, 0, 63, 0,
};
struct coco_raster coco_shrapnel_24c_raster = {
  COCO_SHRAPNEL_24C_RASTER_W,
  COCO_SHRAPNEL_24C_RASTER_H,
  COCO_BITMAP, 0, coco_pixels
};
