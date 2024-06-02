# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

.PHONY: all clean

MAKE	:= make

all:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			cd $$i; \
			$(MAKE) --no-print-directory || exit 1 ; \
			cd ..; \
		fi; \
	done;
	@mkdir -p build-all
	@find . -name "*.nds" -not -path build-all -exec cp -fv {} build-all \;

clean:
	@for i in `ls`; do \
		if test -e $$i/Makefile ; then \
			cd $$i; \
			$(MAKE) clean --no-print-directory || exit 1 ; \
			cd ..; \
		fi; \
	done;
	@rm -fr build-all
