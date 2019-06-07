# SPDX-License-Identifier: MIT
#
# Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

# Variables

CFLAGS		:= -g -Wall
RM		:= rm -rf

# Rules to build the binary

all: dlfixer

dlfixer_obj := dlfixer.o

dlfixer: $(dlfixer_obj)
	$(CC) $(CFLAGS) -o $@ $(dlfixer_obj)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) dlfixer $(dlfixer_obj)
