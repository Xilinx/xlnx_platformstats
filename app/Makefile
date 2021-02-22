.PHONY:	clean

CC ?=  gcc
CFLAGS = -Wall -Wextra
LIBDIR = ../src
INCLUDEDIR = ../include/platformstats
SOURCES = main.c
APPS = main
APPSTMPS = main.out

all: build

build: $(APPSTMPS)

%.out: %.o
	$(CC) -o platformstats main.o -L$(LIBDIR) -lplatformstats

%.o: %.c
	$(CC) -I$(INCLUDEDIR) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(APPS) *.o
