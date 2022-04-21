.PHONY:	clean

CC ?=  gcc
CP = cp
INSTALL_PROGRAM ?= install
CFLAGS 	+= -Wall
LDFLAGS += -shared

libdir ?= usr/lib
includedir ?= usr/include

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)
INCLUDEDIR = ../include
MAJOR = 1
MINOR = 0
VERSION = $(MAJOR).$(MINOR)
NAME := platformstats

all: include lib$(NAME).so

install: all
	$(INSTALL_PROGRAM) -D -m 644 platformstats.h  $(DESTDIR)$(includedir)/platformstats.h
	$(INSTALL_PROGRAM) -D -m 644 lib$(NAME).so.$(VERSION) $(DESTDIR)$(libdir)/lib$(NAME).so.$(VERSION)
	$(CP) -d lib$(NAME).so.$(MAJOR) lib$(NAME).so $(DESTDIR)$(libdir)/

clean:
	rm -f $(OBJECTS) *.so *.so.*
	rm -rf $(INCLUDEDIR)

include:
	mkdir $(INCLUDEDIR)/platformstats -p
	$(CP) $(HEADERS) $(INCLUDEDIR)/platformstats

lib$(NAME).so.$(VERSION): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -Wl,-soname,lib$(NAME).so.$(MAJOR) -o lib$(NAME).so.$(VERSION)

lib$(NAME).so: lib$(NAME).so.$(VERSION)
	rm -f lib$(NAME).so.$(MAJOR) lib$(NAME).so
	ln -s lib$(NAME).so.$(VERSION) lib$(NAME).so.$(MAJOR)
	ln -s lib$(NAME).so.$(MAJOR) lib$(NAME).so

%.o: %.c include
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(INCLUDEDIR)/platformstats -c -fPIC $< -o $@
