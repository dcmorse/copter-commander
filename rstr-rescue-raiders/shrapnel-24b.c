#include "../raster.h"
#include "shrapnel-24b.h"

static unsigned char coco_pixels[COCO_SHRAPNEL_24B_RASTER_W*COCO_SHRAPNEL_24B_RASTER_H>>3] = {
  224,0,0,0,
  240,0,0,0,
  252,63,128,0,
  248,63,192,0,
  240,63,192,0,
  176,63,128,0,
  240,63,128,0,
  96,63,128,0,
  0,127,128,0,
  0,255,192,0,
  1,255,224,0,
  7,255,248,0,
  127,255,254,0,
  127,255,248,0,
  127,255,224,0,
  63,255,192,0,
  15,255,192,0,
  7,255,192,0,
  31,255,0,0,
  31,255,0,0,
  27,255,0,0,
  14,255,0,0,
  15,191,0,0,
  8,0,128,0,
};
struct coco_raster coco_shrapnel_24b_raster = {
  COCO_SHRAPNEL_24B_RASTER_W,
  COCO_SHRAPNEL_24B_RASTER_H,
  COCO_BITMAP, 0,
coco_pixels};
