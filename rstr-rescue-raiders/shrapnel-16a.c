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
#include "shrapnel-16a.h"

static unsigned char coco_pixels[(((COCO_SHRAPNEL_16A_RASTER_W+31)>>5)<<2)*COCO_SHRAPNEL_16A_RASTER_H] = {
  6, 0, 0, 0,
  15, 0, 0, 0,
  63, 128, 0, 0,
  255, 192, 0, 0,
  127, 240, 0, 0,
  63, 240, 0, 0,
  15, 240, 0, 0,
  7, 240, 0, 0,
  3, 240, 0, 0,
  3, 224, 0, 0,
  15, 240, 0, 0,
  7, 248, 0, 0,
  3, 252, 0, 0,
  1, 254, 0, 0,
  0, 254, 0, 0,
  0, 46, 0, 0,
};
struct coco_raster coco_shrapnel_16a_raster = {
  COCO_SHRAPNEL_16A_RASTER_W,
  COCO_SHRAPNEL_16A_RASTER_H,
  COCO_BITMAP, 0, coco_pixels
};
