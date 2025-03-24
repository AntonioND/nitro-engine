# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

# Tools
# -----

MAKE := make

# Targets
# -------

.PHONY: all install clean

all:
	$(MAKE) -f Makefile.blocksds
	$(MAKE) -f Makefile.blocksds NE_DEBUG=1

install: all
	$(MAKE) -f Makefile.blocksds install

clean:
	$(MAKE) -f Makefile.blocksds clean
