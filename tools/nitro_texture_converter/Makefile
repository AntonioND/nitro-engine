# SPDX-License-Identifier: MIT
#
# Copyright (c) 2008-2009, 2019, Antonio Niño Díaz
#
# This file is part of Nitro Engine

PKG_CONFIG	:= pkg-config
PNGCFLAGS	:= `${PKG_CONFIG} --static --cflags libpng`
PNGLDFLAGS	:= `${PKG_CONFIG} --static --libs-only-L libpng`
PNGLDLIBS	:= `${PKG_CONFIG} --static --libs-only-l libpng`

CFLAGS		:= -Wall
RM		:= rm -rf

all: nitro_texture_converter

objs := \
	convert_a1rgb5.o \
	convert_a3rgb32.o \
	convert_a5rgb8.o \
	convert_depthbmp.o \
	convert_rgb16.o \
	convert_rgb256.o \
	convert_rgb4.o \
	load_png.o \
	nitro_texture_converter.o \
	palette.o \

nitro_texture_converter: ${objs}
	${CC} ${CFLAGS} ${PNGLDFLAGS} -o $@ ${objs} ${PNGLDLIBS}

.c.o:
	${CC} ${CFLAGS} ${PNGCFLAGS} -c -o $@ $<

clean:
	${RM} nitro_texture_converter ${objs}
