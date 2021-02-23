.PHONY:	clean

CC ?=  gcc
CP = cp
CFLAGS 	+= -Wall
LDFLAGS += -shared

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)
INCLUDEDIR = ../include
MAJOR = 1
MINOR = 0
VERSION = $(MAJOR).$(MINOR)
NAME := platformstats

all: include lib$(NAME).so

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
