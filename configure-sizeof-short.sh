#!/bin/sh

# Copter Commander - A 2d network helicopter game
# Copyright (C) 20001  David Morse
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

sizeof_short=`./sizeof short` ; 
if [ "2" != $sizeof_short ] 
  then 
    exit 1
  else 
    echo typedef short coco_i16\;   
    echo typedef unsigned short coco_u16\;
    echo \#define COCO_I16_MAX SHRT_MAX
    echo \#define COCO_I16_MIN SHRT_MIN
    echo \#define COCO_U16_MAX USHRT_MAX
    echo
    exit 0
fi ;
echo 
