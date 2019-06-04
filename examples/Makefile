SUBDIRS:= $(shell ls)

#---------------------------------------------------------------------------------
all: examples
#---------------------------------------------------------------------------------
	@rm -fr AllCompiled
	@mkdir -p AllCompiled
	@find . -name "*.nds" -exec cp -fv {} AllCompiled \;

examples:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;

#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	@rm -fr AllCompiled
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE)  -C $$i clean || { exit 1;} fi; done;
