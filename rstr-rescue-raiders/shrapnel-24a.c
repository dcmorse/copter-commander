#include "../raster.h"
#include "shrapnel-24a.h"

static unsigned char coco_pixels[COCO_SHRAPNEL_24A_RASTER_W*COCO_SHRAPNEL_24A_RASTER_H>>3] = {
  192,7,254,0,
  240,7,254,0,
  255,15,252,0,
  255,255,252,0,
  127,255,252,0,
  127,255,248,0,
  63,255,240,0,
  31,255,224,0,
  31,255,192,0,
  15,255,128,0,
  15,255,128,0,
  7,255,128,0,
  7,255,128,0,
  3,255,128,0,
  1,255,128,0,
  1,255,128,0,
  0,255,128,0,
  0,255,0,0,
  0,127,0,0,
  0,126,0,0,
  0,127,0,0,
  0,63,128,0,
  0,31,128,0,
  0,31,128,0,
};
struct coco_raster coco_shrapnel_24a_raster = {
  COCO_SHRAPNEL_24A_RASTER_W,
  COCO_SHRAPNEL_24A_RASTER_H,
  COCO_BITMAP, 0,
coco_pixels};
