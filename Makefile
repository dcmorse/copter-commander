#               ***** Makefile for Copter Commander *****
# 
# IMPORTANT: Please edit the VARIABLES section before make-ing.
#
# ____________________________________________________________________________
#
# Copter Commander - A 2d network helicopter game
# Copyright (C) 2001, 2005  David Morse
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

# ____________________________________________________________________________
#                           *** INVOCATION ***
#
# (UNIX people: if 'make' complains about syntax errors, use 'gmake' instead!)
#
# make                      - make copter-commander and glx-copter-commander
# make copter-commander     - make GTK+/Gnome based version of game
# make glx-copter-commander - make bare bones gui-less version of game
# make install              - install files into the system
# make clean                - remove temporary files
# (past here maintainer only)
# make art		    - run this after modifying tiffs
# make distclean            - remove distributed temporary files (for devel)
# make tarball              - make snapshot for distribution
# make TAGS                 - make emacs tags file
# (past here not implemented)
# make srpm                 - make RPM snapshot and place in ./dist
#
# If your name is David Morse, you probably want to compile with
# make COCO_OPTIMIZATION_FLAGS=-"g -DCOCO_DEBUG" COCO_LEVEL_DIRECTORY=/home/dm/coco/levels COCO_SOUND_DIRECTORY=/home/dm/coco/sounds COCO_SOUND=openal copter-commander
# or on shadow
# make COCO_OPTIMIZATION_FLAGS=-"g -DCOCO_DEBUG" COCO_LEVEL_DIRECTORY=/roam/dm/coco/levels COCO_SOUND_DIRECTORY=/roam/dm/coco/sounds COCO_SOUND=openal copter-commander
# or if with gii
# make COCO_OPTIMIZATION_FLAGS=-"g -DCOCO_DEBUG" COCO_LEVEL_DIRECTORY=/home/dm/coco/levels COCO_SOUND_DIRECTORY=/home/dm/coco/sounds COCO_SOUND=openal copter-commander COCO_GII_INCLUDE_DIRECTORY=/usr/local/include COCO_GII_LIB_DIRECTORY=/usr/local/lib COCO_USE_GII=1
#
# 

COCO_MAJOR_VERSION=1
COCO_MINOR_VERSION=8

# ____________________________________________________________________________
#                           *** VARIABLES ***


# INSTALLATION LOCATIONS
COCO_INSTALL_DIRECTORY=/usr/local
COCO_BIN_DIRECTORY=${COCO_INSTALL_DIRECTORY}/bin
COCO_SHARE_DIRECTORY=${COCO_INSTALL_DIRECTORY}/share/copter-commander/${COCO_MAJOR_VERSION}.${COCO_MINOR_VERSION}
COCO_DOC_DIRECTORY=${COCO_SHARE_DIRECTORY}/doc
COCO_LEVEL_DIRECTORY=${COCO_SHARE_DIRECTORY}/levels
COCO_SOUND_DIRECTORY=${COCO_SHARE_DIRECTORY}/sounds

# LIBRARIES

# Mesa-derived OpenGLs won't be able to run on some machines without
# libpthread thrown in.  See ./pthread-why.txt.
COCO_BASIC_LIBS= -lm -lpthread

# GII: General Input Interface (optional & experimental)
# If support for GII is compiled in, it may be possible to get
# snappier mouse and keyboard performance.  At this time (11-29-2001)
# this is only experimental and will probaly cheese you off.
# to not use: place "#" before "COCO_USE_GII=1"
# to use: remove "#" before "COCO_USE_GII=1"
#
#COCO_USE_GII=1
COCO_GII_LIB_DIRECTORY=/usr/lib
COCO_GII_INCLUDE_DIRECTORY=/usr/include

# X11 (glx-copter-commander only)
COCO_X11_LIB_DIRECTORY=/usr/X11R6/lib
COCO_X11_INCLUDE_DIRECTORY=/usr/X11R6/include

# comment out COCO_USE_XF86VIDMODE if you don't have XFree86, 
# (but then you lose the ability to go full-screen):
COCO_USE_XF86VIDMODE=1
COCO_XF86VIDMODE_LIB_DIRECTORY=${COCO_X11_LIB_DIRECTORY}
COCO_XF86VIDMODE_INCLUDE_DIRECTORY=${COCO_X11_INCLUDE_DIRECTORY}

# Gnome is handled by the pkg-config script, no need to specify
# LIB_DIRECTORY or INCLUDE_DIRECTORY.

# comment out COCO_USE_FREETYPE if you live in a cave and don't have
# freetype2.  Note that gnome support will fall back to BROKEN pango
# fonts if you do this.  YOu'll get batty text alignment.
COCO_USE_FREETYPE=1
# Freetype is handled by the freetype-config script, no need to
# specify LIB_DIRECTORY or INCLUDE_DIRECTORY.

# OpenGL (According to http://oss.sgi.com/projects/ogl-sample/ABI, these
# should be as shown, if not, consider making a symlink)
COCO_GL_LIB_DIRECTORY=/usr/lib
COCO_GL_INCLUDE_DIRECTORY=/usr/include

# LibTIFF image library
COCO_LIBTIFF_LIB_DIRECTORY=/usr/lib
COCO_LIBTIFF_INCLUDE_DIRECTORY=/usr/include

# Sound Support
# Select the sound technique that works for you, valid values are 
#  esd:    use enlightenment sound daemon, probably you have it
#          installed already.  (incompatible with glx-copter-commander)
#  openal: use driver ala www.openal.org, nice 3d effects, sucks 200Mhz cpu,
#          You probably have to download it.
#  mute:   client makes no sounds
COCO_SOUND=esd

# OpenAL sound library (only used if COCO_SOUND=openal)
COCO_OPENAL_LIB_DIRECTORY=/usr/lib
COCO_OPENAL_LIBS=-lopenal
COCO_OPENAL_INCLUDE_DIRECTORY=/usr/include

# ESD sound library (only used if COCO_SOUND=esd)
COCO_ESD_LIB_DIRECTORY=/usr/lib
COCO_ESD_INCLUDE_DIRECTORY=/usr/include
COCO_ESD_LIBS=-lesd


# C COMPILER
CC=cc
# set during debugging:
# COCO_OPTIMIZATION_FLAGS=-g -DCOCO_DEBUG
# set during release (except with egcs-2.91):
COCO_OPTIMIZATION_FLAGS=-O3
# Some gnome headers get rejected by ansi compilers.
# Files that bring in gnomish headers must use CFLAGS_UNANSI.
CFLAGS_UNANSI=-Wall ${COCO_OPTIMIZATION_FLAGS}
# While everything else uses CFLAGS.
CFLAGS=-ansi -pedantic ${CFLAGS_UNANSI}
# CFLAGS=${CFLAGS_UNANSI}

# COMMON LISP COMPILER
# (Only needed to do development, not to compile from distribution)
# (Known to work with CLISP 2000-03-06, see http://clisp.sourceforge.net)
CLISP=clisp
CLISP_FLAGS=--quiet


#                           (end of variables)
# ____________________________________________________________________________
#                         *** MAKEFILE BODY ***

ifdef COCO_USE_GII
COCO_GII_LINK_ARGUMENTS=-L${COCO_GII_LIB_DIRECTORY} -lgii
COCO_GII_INCLUDE_ARGUMENTS=-I${COCO_GII_INCLUDE_DIRECTORY}
else
# (I leave them undefined, and not null string, for hackish reasons.)
#COCO_GII_LINK_ARGUMENTS=""
#COCO_GII_INCLUDE_ARGUMENTS=""
endif

ifdef COCO_USE_XF86VIDMODE
COCO_XF86VIDMODE_LINK_ARGUMENTS=-L${COCO_XF86VIDMODE_LIB_DIRECTORY} -lXxf86vm
COCO_XF86VIDMODE_INCLUDE_ARGUMENTS=-I${COCO_XF86VIDMODE_INCLUDE_DIRECTORY}
else
# (I leave them undefined, and not null string, for unknown reasons.)
#COCO_XF86VIDMODE_LINK_ARGUMENTS=""
#COCO_XF86VIDMODE_INCLUDE_ARGUMENTS=""
endif

ifdef COCO_USE_FREETYPE
COCO_FREETYPE_LINK_ARGUMENTS=`freetype-config --libs`
COCO_FREETYPE_INCLUDE_ARGUMENTS=`freetype-config --cflags`
else
# (I leave them undefined, and not null string, for unknown reasons.)
#COCO_FREETYPE_LINK_ARGUMENTS=""
#COCO_FREETYPE_INCLUDE_ARGUMENTS=""
endif


ifeq (${COCO_SOUND},openal)
COCO_SOUND_LINK_LINE=-L${COCO_OPENAL_LIB_DIRECTORY} ${COCO_OPENAL_LIBS}
else
ifeq (${COCO_SOUND},esd)
COCO_SOUND_LINK_LINE=-L${COCO_ESD_LIB_DIRECTORY} ${COCO_ESD_LIBS}
else
#assign null string:
COCO_SOUND_LINK_LINE=
endif
endif



# official value:     COCO_APPEARANCE_RESCUE_RAIDERS
# experimental value: COCO_APPEARANCE_DOODLE 
COCO_APPEARANCE=COCO_APPEARANCE_RESCUE_RAIDERS
# this guy has to be kept up to date with the above one in the obvious way:
COCO_APPEARANCE_BASE=rescue-raiders
COCO_LISP_APPEARANCE=:coco-appearance-${COCO_APPEARANCE_BASE}
COCO_RASTER_DIRECTORY=rstr-${COCO_APPEARANCE_BASE}

# All header files
HDR_FILES = forward-sobject.h methods.h sobject.h coco.h raster.h \
            background.h configure-time.h object-time.h sound.h xf86vidmode.h \
            alloca.h

# All C files, except for those dealing with the GUI, and those dealing with sound
GAME_C_FILES = language.c clsho.c simple-command-line.c graphics.c network.c \
               gamesho.c methods.c comp.c \
               background-${COCO_APPEARANCE_BASE}.c client.c sobject.c \
	       screenshot.c server.c ccl.c globals.c main.c dashboard.c \
	       freetype.c
#              console-ui.c console-data.c console-language.c \

# Gnome GUI files
# (note how they cleverly start with "gnome-", so I can easily pass extra flags.)
GNOME_C_FILES = gnome-window.c gnome-ident.c # gnome-kennel.c

# GLX GUI files
# (note how some cleverly start with "glx-", so I can easily pass extra flags.)
GLX_C_FILES = glx-window.c not-gnome-ident.c

# OpenAL sound files
# (note how they cleverly start with "openal-", so I can easily pass extra flags.)
OPENAL_C_FILES = openal-sound.c

ESD_C_FILES = esd-sound.c

# mute sound files
MUTE_C_FILES = mute-sound.c

# sound files used, depending on COCO_SOUND choice
USED_SOUND_C_FILES = ${COCO_SOUND}-sound.c

# All C files
ALL_C_FILES = ${GAME_C_FILES} ${GNOME_C_FILES} ${GLX_C_FILES} ${OPENAL_C_FILES} ${ESD_C_FILES} ${MUTE_C_FILES} xf86vidmode.c


GAME_OBJ_FILES = ${GAME_C_FILES:.c=.o}
GNOME_OBJ_FILES = ${GNOME_C_FILES:.c=.o}
GLX_OBJ_FILES = ${GLX_C_FILES:.c=.o}
MUTE_OBJ_FILES = ${MUTE_C_FILES:.c=.o}
OPENAL_OBJ_FILES = ${OPENAL_C_FILES:.c=.o}
USED_SOUND_OBJ_FILES = ${USED_SOUND_C_FILES:.c=.o}
ALL_OBJ_FILES = ${ALL_C_FILES:.c=.o}

# the default target
all: copter-commander glx-copter-commander

# TAGS is an emacs development tool
TAGS: ${ALL_C_FILES} ${HDR_FILES}
	etags *.c *.h

# the "built" file is touched when all rasters are compiled to .o files
rstr-%/built: rstr-%/rasters.h 
	set -e;\
	CC=${CC};\
	CFLAGS="${CFLAGS}";\
	export CC CFLAGS;\
	cd rstr-$*;\
	../make-rasters.sh

# tiff-compile only does its thing if the files are out of date.
# so in essence it already manages its dependancies.
# the .PRECIOUS keeps gnu make from deleting the .h when its done.
# which is annoying, imo. -dm 05-18-2001
rstr-%/rasters.h: tiff-compile
	./tiff-compile tiffs `dirname $@`

.PRECIOUS: ${COCO_RASTER_DIRECTORY}/rasters.h

# force recompile of tiffs (changes to which are not otherwise sensed by this makefile)
art: tiff-clean tiff-compile
	./tiff-compile tiffs ${COCO_RASTER_DIRECTORY}

tiff-clean:
	rm -f ${COCO_RASTER_DIRECTORY}/*.c \
	      ${COCO_RASTER_DIRECTORY}/*.h  \
	      ${COCO_RASTER_DIRECTORY}/*.o   \
	      ${COCO_RASTER_DIRECTORY}/built ;

# FreeBSD seems to hide alloca inside of stdlib.h, even when -ansi
# So create a trampoline header file to point to the right one
alloca.h: Makefile
	if echo "#include <alloca.h>" | ${CC} ${CFLAGS} -E - > /dev/null 2> /dev/null ; \
	  then echo "#include <alloca.h>" > alloca.h ;\
	  else echo "#include <stdlib.h>" > alloca.h ;\
	fi;

# Some decisions are put off until configure time then included into
# coco.h
configure-time.h: Makefile endian sizeof configure-*.sh
	set -e ;\
	echo "/* File automatically generated by Makefile, do not edit */" \
	  > configure-time.h ;\
	echo >> configure-time.h ;\
	echo \#define COCO_MAJOR_VERSION ${COCO_MAJOR_VERSION} \
	  >> configure-time.h ;\
	echo \#define COCO_MINOR_VERSION ${COCO_MINOR_VERSION} \
	  >> configure-time.h ;\
	echo \#define COCO_VERSION_STRING \"${COCO_MAJOR_VERSION}.${COCO_MINOR_VERSION}\" \
	  >> configure-time.h ;\
	echo >> configure-time.h ;\
	echo \#define COCO_LEVEL_DIRECTORY \"${COCO_LEVEL_DIRECTORY}\" \
	  >> configure-time.h ;\
	echo \#define COCO_SOUND_DIRECTORY \"${COCO_SOUND_DIRECTORY}\" \
	  >> configure-time.h ;\
	echo >> configure-time.h ;\
	./configure-endian.sh >> configure-time.h ;\
	./configure-sizeof-int.sh >> configure-time.h ; \
	if [ $$? != "0" ] ; \
	  then \
	    echo FATAL ERROR ;\
	    echo sizeof int is not 4 ;\
	    echo fix the problem by extending configure-sizeof-int.sh then ;\
	    echo execute rm configure-time.h and restart build ;\
	fi ; \
	./configure-sizeof-short.sh >> configure-time.h ; \
	if [ $$? != "0" ] ; \
	  then \
	    echo FATAL ERROR ;\
	    echo sizeof short is not 2 ;\
	    echo fix the problem by extending configure-sizeof-short.sh then ;\
	    echo execute rm configure-time.h and restart build ;\
	fi ;
ifdef COCO_USE_GII
	set -e ;\
	echo \#define COCO_USE_GII ${COCO_USE_GII} \
	 >> configure-time.h ;\
	echo >> configure-time.h ;
endif
ifdef COCO_USE_XF86VIDMODE
	set -e ;\
	echo \#define COCO_USE_XF86VIDMODE ${COCO_USE_XF86VIDMODE} \
	 >> configure-time.h ;\
	echo >> configure-time.h ;
endif
ifdef COCO_USE_FREETYPE
	set -e ;\
	echo \#define COCO_USE_FREETYPE ${COCO_USE_FREETYPE} \
	 >> configure-time.h ;\
	echo >> configure-time.h ;
endif


# other decisions when the c objects are built.
object-time.h:
	set -e ;\
	echo "/* File automatically generated by Makefile, do not edit */" \
	  > object-time.h ;\
	echo >> object-time.h ;\
	echo \#define COCO_APPEARANCE ${COCO_APPEARANCE} \
	  >> object-time.h ;\


object-time.lisp:
	set -e ;\
	echo ";;; File automatically generated by Makefile, do not edit" \
	  > object-time.lisp ;\
	echo >> object-time.lisp ;\
	echo "(pushnew ${COCO_LISP_APPEARANCE} *features*)" \
	  >> object-time.lisp ;\

# the c object compiler, (written in lisp, ironically), that builds
# cobject.c and cobject.h
oo-compiler.fas: oo-compiler.lisp
	${CLISP} ${CLISP_FLAGS} -c oo-compiler.lisp


# generate c files from lisp inputs
sobject.c sobject.h forward-sobject.h: sobject.lisp oo-compiler.fas object-time.lisp
	${CLISP} ${CLISP_FLAGS} -i oo-compiler.fas -i object-time.lisp \
	  -i sobject.lisp -x "(emit-everything-to-files \"sobject\")"

# build gnome objects without the ansi flag, and with the gnome headers.
gnome-%.o: gnome-%.c 
	${CC} ${CFLAGS_UNANSI} -c gnome-$*.c \
          -I${COCO_GL_INCLUDE_DIRECTORY} \
	  `pkg-config --cflags libgnome-2.0 libgnomeui-2.0 gtkglext-1.0` 

# build glx objects with access to X11 and GL
glx-%.o: glx-%.c 
	${CC} ${CFLAGS} -c glx-$*.c \
	  -I${COCO_X11_INCLUDE_DIRECTORY} \
	  -I${COCO_GL_INCLUDE_DIRECTORY}

# build OpenAL objects with access to OpenAL headers
openal-%.o: openal-%.c 
	${CC} ${CFLAGS} -c openal-$*.c \
	  -I${COCO_OPENAL_INCLUDE_DIRECTORY}

# build esd objects with access to esd and libgnome headers
# Need cflags_unansi because otherwise #include <unistd.h> doesn't
# pick up fsync
esd-%.o: esd-%.c 
	${CC} ${CFLAGS_UNANSI} -c esd-$*.c \
          -I${COCO_ESD_INCLUDE_DIRECTORY} \
	  `pkg-config --cflags libgnomeui-2.0`

freetype.o: freetype.c
	${CC} ${CFLAGS} -c freetype.c \
	${COCO_FREETYPE_INCLUDE_ARGUMENTS} \
	-I${COCO_GL_INCLUDE_DIRECTORY}

client.o: client.c
	${CC} ${CFLAGS_UNANSI} -c client.c -I${COCO_GL_INCLUDE_DIRECTORY} \
	  ${COCO_GII_INCLUDE_ARGUMENTS}

screenshot.o: screenshot.c 
	${CC} ${CFLAGS} -c screenshot.c -I${COCO_GL_INCLUDE_DIRECTORY} \
          -I${COCO_LIBTIFF_INCLUDE_DIRECTORY}

xf86vidmode.o: xf86vidmode.c
	${CC} ${CFLAGS} -c xf86vidmode.c \
          ${COCO_XF86VIDMODE_INCLUDE_ARGUMENTS}

# build other objects the standard way
%.o: %.c 
	${CC} ${CFLAGS} -c $*.c \
	  -I${COCO_GL_INCLUDE_DIRECTORY}

# add depenencies for all object files
${ALL_OBJ_FILES}: ${HDR_FILES} Makefile ${COCO_RASTER_DIRECTORY}/built

GNOME_LINK_LINE=\
${CC} ${CFLAGS} -o $@ ${GAME_OBJ_FILES} ${GNOME_OBJ_FILES} xf86vidmode.o \
${USED_SOUND_OBJ_FILES} ${COCO_RASTER_DIRECTORY}/*.o \
${COCO_BASIC_LIBS} \
-L${COCO_LIBTIFF_LIB_DIRECTORY} -ltiff \
-L${COCO_GL_LIB_DIRECTORY} -lGL \
`pkg-config --libs libgnome-2.0 libgnomeui-2.0 gtkglext-1.0` \
${COCO_GII_LINK_ARGUMENTS} \
${COCO_XF86VIDMODE_LINK_ARGUMENTS} \
${COCO_FREETYPE_LINK_ARGUMENTS}


copter-commander: ${GAME_OBJ_FILES} ${GNOME_OBJ_FILES} ${USED_SOUND_OBJ_FILES} ${COCO_RASTER_DIRECTORY}/built xf86vidmode.o
	${GNOME_LINK_LINE} ${COCO_SOUND_LINK_LINE}

GLX_LINK_LINE=\
${CC} ${CFLAGS} -o $@ ${GAME_OBJ_FILES} ${GLX_OBJ_FILES}  xf86vidmode.o \
${USED_SOUND_OBJ_FILES} ${COCO_RASTER_DIRECTORY}/*.o \
${COCO_BASIC_LIBS} \
-L${COCO_LIBTIFF_LIB_DIRECTORY} -ltiff \
-L${COCO_X11_LIB_DIRECTORY} -lX11 -lXext \
-L${COCO_GL_LIB_DIRECTORY} -lGL \
${COCO_GII_LINK_ARGUMENTS} \
${COCO_XF86VIDMODE_LINK_ARGUMENTS} \
${COCO_FREETYPE_LINK_ARGUMENTS}

glx-copter-commander: ${GAME_OBJ_FILES} ${GLX_OBJ_FILES} ${USED_SOUND_OBJ_FILES} ${COCO_RASTER_DIRECTORY}/built  xf86vidmode.o
ifeq (${COCO_SOUND},esd)
	${GLX_LINK_LINE} ${COCO_SOUND_LINK_LINE} \
	`pkg-config --libs libgnome-2.0 libgnomeui-2.0`
else
	${GLX_LINK_LINE} ${COCO_SOUND_LINK_LINE}
endif

# STRingLITerals, long text files #included into c files.  Should probably
# be replaced by gnome-help-browser entries.

COPYING.strlit: COPYING string-literalize.sh
	./string-literalize.sh < COPYING > COPYING.strlit

manual.strlit: manual.txt string-literalize.sh
	./string-literalize.sh < manual.txt > manual.strlit

keybindings.strlit: keybindings.txt string-literalize.sh
	./string-literalize.sh < keybindings.txt > keybindings.strlit

c-header.strlit: c-header.txt string-literalize.sh
	./string-literalize.sh < c-header.txt > c-header.strlit

globals.o: COPYING.strlit manual.strlit keybindings.strlit

# -lm added to shut up somebody's box.  
# [ #499186 ] tiff-compile doesn't compile -dm 01-03-2002
tiff-compile: tiff-compile.c raster.h c-header.strlit
	${CC} ${CFLAGS_UNANSI} -I${COCO_LIBTIFF_INCLUDE_DIRECTORY} \
	  -o $@ $@.c -L${COCO_LIBTIFF_LIB_DIRECTORY} -ltiff -lm

sizeof: sizeof.c
	${CC} ${CFLAGS} -o sizeof sizeof.c

endian: endian.c
	${CC} ${CFLAGS} -o endian endian.c

clean: tiff-clean
	rm -f *.o copter-commander glx-copter-commander \
	  configure-time.h alloca.h *.strlit endian sizeof tiff-compile

distclean: clean
	rm -f object-time.h object-time.lisp \
	  sobject.c sobject.h forward-sobject.h \
	  oo-compiler.lib oo-compiler.fas 

COCO_TAR_DIRECTORY=copter-commander-${COCO_MAJOR_VERSION}.${COCO_MINOR_VERSION}
COCO_TAR_FILE=copter-commander-${COCO_MAJOR_VERSION}.${COCO_MINOR_VERSION}.tar

tarball: ${COCO_TAR_FILE}.gz ${COCO_TAR_FILE}.bz2
	rm -f ${COCO_TAR_FILE}

# Note that the .fas files are compiled output, but they are included in
# the source distribution.  This is just to fool make into not remaking
# them (or trying to remake them, then finding there's no clisp
# installed).
${COCO_TAR_FILE}:  forward-sobject.h  sobject.h  sobject.c oo-compiler.fas oo-compiler.lib object-time.h object-time.lisp
	set -e ;\
	rm -rf ${COCO_TAR_DIRECTORY} ${COCO_TAR_FILE} ;\
	mkdir ${COCO_TAR_DIRECTORY} ;\
	cp --archive -R Makefile  *.lisp *.fas *.c *.h *.txt *.sh \
	  copyright COPYING INSTALL DEVEL GNOME-HACKS \
	  ChangeLog copter-commander-template.spec copter-commander.png \
          tiffs  \
	 ${COCO_TAR_DIRECTORY} ;\
	rm -f ${COCO_TAR_DIRECTORY}/configure-time.h ;\
	rm -f ${COCO_TAR_DIRECTORY}/alloca.h ;\
	mkdir ${COCO_TAR_DIRECTORY}/levels ${COCO_TAR_DIRECTORY}/sounds ;\
	cp sounds/*.wav ${COCO_TAR_DIRECTORY}/sounds ;\
	cp levels/*.ccl levels/*.cct ${COCO_TAR_DIRECTORY}/levels ;\
	for f in rstr-* ; \
	  do mkdir ${COCO_TAR_DIRECTORY}/$$f; \
	  done ;\
	set +e ;\
	find ${COCO_TAR_DIRECTORY} -name \*~ -print | xargs rm -f ;\
	find ${COCO_TAR_DIRECTORY} -name CVS -print | xargs rm -rf ;\
	set -e ;\
	tar -cf ${COCO_TAR_FILE} ${COCO_TAR_DIRECTORY} ;\
	rm -rf ${COCO_TAR_DIRECTORY}

${COCO_TAR_FILE}.bz2: ${COCO_TAR_FILE}
	set -e ;\
	rm -f ${COCO_TAR_FILE}.bz2 ;\
	bzip2 -k ${COCO_TAR_FILE}

${COCO_TAR_FILE}.gz: ${COCO_TAR_FILE}
	set -e ;\
	rm -f ${COCO_TAR_FILE}.gz ;\
	gzip -c ${COCO_TAR_FILE} > ${COCO_TAR_FILE}.gz

install:
	set -e ;\
	mkdir -p \
          ${COCO_BIN_DIRECTORY} \
	  ${COCO_SHARE_DIRECTORY} \
	  ${COCO_DOC_DIRECTORY} \
	  ${COCO_LEVEL_DIRECTORY} \
	  ${COCO_SOUND_DIRECTORY} ;\
	install copter-commander ${COCO_BIN_DIRECTORY} ;\
	install glx-copter-commander ${COCO_BIN_DIRECTORY} ;\
	install -m u=rw,go=r keybindings.txt manual.txt copyright ${COCO_DOC_DIRECTORY} ;\
	install -m u=rw,go=r levels/*.ccl ${COCO_LEVEL_DIRECTORY} ;\
	install -m u=rw,go=r levels/*.cct ${COCO_LEVEL_DIRECTORY} ;\
	install -m u=rw,go=r sounds/*.wav ${COCO_SOUND_DIRECTORY}

COCO_RPM_RELEASE=1
COCO_RPM_SPEC=copter-commander-${COCO_MAJOR_VERSION}.${COCO_MINOR_VERSION}-${COCO_RPM_RELEASE}.spec

${COCO_RPM_SPEC}: copter-commander-template.spec Makefile
	sed 's/COCO_MAJOR_VERSION/${COCO_MAJOR_VERSION}/g' copter-commander-template.spec | sed 's/COCO_MINOR_VERSION/${COCO_MINOR_VERSION}/g' | sed 's/COCO_RPM_RELEASE/${COCO_RPM_RELEASE}/g' > ${COCO_RPM_SPEC}

# .rpm howto (approximate)
# make the .spec file above, eg:
# make copter-commander-1.1-1.spec
# make tarball
# su root
# cp copter-commander-1.1.tar.gz /usr/src/redhat/SOURCES
# rpm -ba ./copter-commander-1.1-1.spec

# .deb todo

# 1) "copyright" file must go in /usr/share/copter-commander/copyright, NOT
# where it is currently isntalled.
# 2) ugh, read like 3000 more pages of doc.
