#include "../raster.h"
#include "shrapnel-24c.h"

static unsigned char coco_pixels[COCO_SHRAPNEL_24C_RASTER_W*COCO_SHRAPNEL_24C_RASTER_H>>3] = {
  248,0,4,0,
  248,1,4,0,
  252,7,0,0,
  252,7,128,0,
  255,7,192,0,
  255,135,240,0,
  255,199,254,0,
  255,255,254,0,
  255,255,254,0,
  255,255,0,0,
  255,254,0,0,
  63,252,0,0,
  31,254,0,0,
  31,255,0,0,
  31,255,128,0,
  31,255,192,0,
  31,239,192,0,
  31,231,192,0,
  15,227,224,0,
  7,193,240,0,
  3,192,248,0,
  0,0,124,0,
  0,0,126,0,
  96,0,63,0,
};
struct coco_raster coco_shrapnel_24c_raster = {
  COCO_SHRAPNEL_24C_RASTER_W,
  COCO_SHRAPNEL_24C_RASTER_H,
  COCO_BITMAP, 0,
coco_pixels};
