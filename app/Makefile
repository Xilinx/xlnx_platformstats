.PHONY:	clean all build install

CC ?=  gcc
INSTALL_PROGRAM ?= install
CFLAGS ?= -Wall -Wextra
LDFLAGS ?= 
LIBDIR = ../src
INCLUDEDIR = ../include/xlnx_platformstats
SOURCES = main.c
APPS = xlnx_platformstats
APPSTMPS = main.out

bindir ?= /usr/bin

all: build

build: $(APPSTMPS)

install: $(APPSTMPS)
	$(INSTALL_PROGRAM) -D -m 755 xlnx_platformstats $(DESTDIR)$(bindir)/xlnx_platformstats

%.out: %.o
	$(CC) $(LDFLAGS) -o xlnx_platformstats main.o -L$(LIBDIR) -lxlnx_platformstats

%.o: %.c
	$(CC) -I$(INCLUDEDIR) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(APPS) *.o
