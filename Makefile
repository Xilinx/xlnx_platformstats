.PHONY:	default all clean install

libdir ?= /usr/lib

default all:
	# TODO lib needs to be built first to create include dir; this should be an explicit dep
	$(MAKE) -C src
	$(MAKE) -C app
	$(MAKE) -C python-bindings

clean:
	$(MAKE) -C src $@
	$(MAKE) -C app $@
	$(MAKE) -C python-bindings $@

install: all
	$(MAKE) -C src $@
	$(MAKE) -C app $@
	$(MAKE) -C python-bindings $@
