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
#include "smoke-small.h"

static unsigned char coco_pixels[COCO_SMOKE_SMALL_RASTER_W*COCO_SMOKE_SMALL_RASTER_H*4] = {
  52,52,52,52, 122,122,122,122, 169,169,169,169, 166,166,166,166, 114,114,114,114, 45,45,45,45,
  124,124,124,124, 217,217,217,217, 246,246,246,246, 245,245,245,245, 211,211,211,211, 118,118,118,118,
  169,169,169,169, 247,247,247,247, 254,254,254,254, 253,253,253,253, 245,245,245,245, 172,172,172,172,
  159,159,159,159, 241,241,241,241, 253,253,253,253, 253,253,253,253, 245,245,245,245, 181,181,181,181,
  104,104,104,104, 206,206,206,206, 245,245,245,245, 248,248,248,248, 237,237,237,237, 157,157,157,157,
  42,42,42,42, 117,117,117,117, 179,179,179,179, 195,195,195,195, 166,166,166,166, 92,92,92,92,
};
struct coco_raster coco_smoke_small_raster = {
  COCO_SMOKE_SMALL_RASTER_W,
  COCO_SMOKE_SMALL_RASTER_H,
  COCO_RGBA_8_BLEND, 0, coco_pixels
};
