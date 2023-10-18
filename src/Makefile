.PHONY:	clean

CC ?=  gcc
CP = cp
INSTALL_PROGRAM ?= install
CFLAGS 	+= -Wall
LDFLAGS += -shared

libdir ?= /usr/lib
includedir ?= /usr/include

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)
INCLUDEDIR = ../include
MAJOR = 1
MINOR = 1
VERSION = $(MAJOR).$(MINOR)
NAME := xlnx_platformstats

all: include lib$(NAME).so

install: all
	$(INSTALL_PROGRAM) -D -m 644 xlnx_platformstats.h  $(DESTDIR)$(includedir)/xlnx_platformstats.h
	$(INSTALL_PROGRAM) -D -m 644 lib$(NAME).so.$(VERSION) $(DESTDIR)$(libdir)/lib$(NAME).so.$(VERSION)
	$(CP) -d lib$(NAME).so.$(MAJOR) lib$(NAME).so $(DESTDIR)$(libdir)/

clean:
	rm -f $(OBJECTS) *.so *.so.*
	rm -rf $(INCLUDEDIR)

include:
	mkdir $(INCLUDEDIR)/xlnx_platformstats -p
	$(CP) $(HEADERS) $(INCLUDEDIR)/xlnx_platformstats

lib$(NAME).so.$(VERSION): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -Wl,-soname,lib$(NAME).so.$(MAJOR) -o lib$(NAME).so.$(VERSION) -lfreeipmi

lib$(NAME).so: lib$(NAME).so.$(VERSION)
	rm -f lib$(NAME).so.$(MAJOR) lib$(NAME).so
	ln -s lib$(NAME).so.$(VERSION) lib$(NAME).so.$(MAJOR)
	ln -s lib$(NAME).so.$(MAJOR) lib$(NAME).so

%.o: %.c include
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(INCLUDEDIR)/xlnx_platformstats -c -fPIC $< -o $@
