SUBDIRS:= $(shell ls)

#-------------------------------------------------------------------------------
all: examples
#-------------------------------------------------------------------------------
	@rm -fr build-all
	@mkdir -p build-all
	@find . -name "*.nds" -not -path build-all -exec cp -fv {} build-all \;

# TODO: The previous target tries to copy the files from build-all into
# themselves. Exclude them correctly.

examples:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@rm -fr build-all
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE)  -C $$i clean || { exit 1;} fi; done;
