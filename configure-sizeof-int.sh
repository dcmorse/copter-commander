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

sizeof_int=`./sizeof int` ; 
if [ "4" != $sizeof_int ] 
  then 
    exit 1
  else 
    echo typedef int coco_i32\;   
    echo typedef unsigned int coco_u32\;
    echo \#define COCO_I32_MAX INT_MAX
    echo \#define COCO_I32_MIN INT_MIN
    echo \#define COCO_U32_MAX UINT_MAX
    echo
    exit 0
fi ;
echo 
