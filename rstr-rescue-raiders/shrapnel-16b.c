#include "../raster.h"
#include "shrapnel-16b.h"

static unsigned char coco_pixels[COCO_SHRAPNEL_16B_RASTER_W*COCO_SHRAPNEL_16B_RASTER_H>>3] = {
  8,0,0,0,
  4,0,0,0,
  4,0,0,0,
  7,0,0,0,
  3,192,0,0,
  3,240,0,0,
  3,252,0,0,
  3,254,0,0,
  3,255,0,0,
  238,31,0,0,
  242,60,0,0,
  3,56,0,0,
  3,32,0,0,
  3,248,0,0,
  33,240,0,0,
  1,240,0,0,
};
struct coco_raster coco_shrapnel_16b_raster = {
  COCO_SHRAPNEL_16B_RASTER_W,
  COCO_SHRAPNEL_16B_RASTER_H,
  COCO_BITMAP, 0,
coco_pixels};
