#ifndef COCO_XF86VIDMODE_H
#define COCO_XF86VIDMODE_H

/* This gets seperate header file, since I don't want to include xlib.h in
   coco.h just for Display *. */

#include <X11/Xlib.h>
#include "coco.h"

extern coco_bool coco_set_video_mode_from_xdisplay( Display *display );
extern void coco_restore_video_mode_from_xdisplay( Display *display );

#endif /* COCO_XF86VIDMODE_H */
