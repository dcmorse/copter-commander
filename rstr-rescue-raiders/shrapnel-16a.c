#include "../raster.h"
#include "shrapnel-16a.h"

static unsigned char coco_pixels[COCO_SHRAPNEL_16A_RASTER_W*COCO_SHRAPNEL_16A_RASTER_H>>3] = {
  6,0,0,0,
  15,0,0,0,
  63,128,0,0,
  255,192,0,0,
  127,240,0,0,
  63,240,0,0,
  15,240,0,0,
  7,240,0,0,
  3,240,0,0,
  3,224,0,0,
  15,240,0,0,
  7,248,0,0,
  3,252,0,0,
  1,254,0,0,
  0,254,0,0,
  0,46,0,0,
};
struct coco_raster coco_shrapnel_16a_raster = {
  COCO_SHRAPNEL_16A_RASTER_W,
  COCO_SHRAPNEL_16A_RASTER_H,
  COCO_BITMAP, 0,
coco_pixels};
