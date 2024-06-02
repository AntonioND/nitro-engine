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
	$(MAKE) -f Makefile.blocks
	$(MAKE) -f Makefile.blocks NE_DEBUG=1

install: all
	$(MAKE) -f Makefile.blocks install

clean:
	$(MAKE) -f Makefile.blocks clean

.PHONY: dkp dkp-clean

dkp:
	$(MAKE) -f Makefile.dkp
	$(MAKE) -f Makefile.dkp NE_DEBUG=1

dkp-clean:
	$(MAKE) -f Makefile.dkp clean
