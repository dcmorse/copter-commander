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
#include "bomb-south.h"

static unsigned char coco_pixels[COCO_BOMB_SOUTH_RASTER_W*COCO_BOMB_SOUTH_RASTER_H*4] = {
  0,0,0,0, 0,0,0,0, 130,127,0,255, 0,0,0,0, 0,0,0,0,
  0,0,0,0, 160,140,15,255, 130,127,0,255, 130,127,0,255, 0,0,0,0,
  200,31,31,255, 200,31,31,255, 127,15,15,255, 63,15,15,255, 63,15,15,255,
  200,31,31,255, 127,15,15,255, 127,15,15,255, 127,15,15,255, 63,15,15,255,
  200,31,31,255, 127,15,15,255, 127,15,15,255, 127,15,15,255, 63,15,15,255,
  200,31,31,255, 127,15,15,255, 127,15,15,255, 127,15,15,255, 63,15,15,255,
  200,31,31,255, 127,15,15,255, 127,15,15,255, 127,15,15,255, 63,15,15,255,
  200,31,31,255, 200,31,31,255, 127,15,15,255, 63,15,15,255, 63,15,15,255,
  0,0,0,0, 200,31,31,255, 200,31,31,255, 127,15,15,255, 0,0,0,0,
  0,0,0,0, 255,255,255,255, 200,31,31,255, 255,255,255,255, 0,0,0,0,
  255,255,255,255, 255,255,255,255, 0,0,0,0, 255,255,255,255, 255,255,255,255,
};
struct coco_raster coco_bomb_south_raster = {
  COCO_BOMB_SOUTH_RASTER_W,
  COCO_BOMB_SOUTH_RASTER_H,
  COCO_RGBA_8, 0, coco_pixels
};
