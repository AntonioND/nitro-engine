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

.PHONY: dkp dkp-clean

dkp:
	$(MAKE) -f Makefile.dkp
	$(MAKE) -f Makefile.dkp NE_DEBUG=1

dkp-clean:
	$(MAKE) -f Makefile.dkp clean
