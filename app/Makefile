.PHONY:	clean all build install

CC ?=  gcc
INSTALL_PROGRAM ?= install
CFLAGS ?= -Wall -Wextra
LDFLAGS ?= 
LIBDIR = ../src
INCLUDEDIR = ../include/platformstats
SOURCES = main.c
APPS = platformstats
APPSTMPS = main.out

bindir ?= /usr/bin

all: build

build: $(APPSTMPS)

install: $(APPSTMPS)
	$(INSTALL_PROGRAM) -D -m 755 platformstats $(DESTDIR)$(bindir)/platformstats

%.out: %.o
	$(CC) $(LDFLAGS) -o platformstats main.o -L$(LIBDIR) -lplatformstats

%.o: %.c
	$(CC) -I$(INCLUDEDIR) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(APPS) *.o
