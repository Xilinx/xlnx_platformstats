.PHONY:	clean

#Dynamic loading for C
CC ?= gcc
CFLAGS ?= -fPIC -Wall -Wextra
LDFLAGS ?=
LIBS = -L../src -L. -lplatformstats
INCLUDES = -I../include/platformstats

#Python
PYTHON_INCLUDE ?= -I/usr/include/python3.7m/ -lpython3.7m
PYTHON_SO = .so

#Swig
SWIG ?= swig
SWIGOPT ?= 
IWRAP      = $(INTERFACE:.i=_wrap.i)
ISRCS      = $(IWRAP:.i=.c)
IOBJS      = $(IWRAP:.i=.o)

INTERFACE = platformstats_bindings.i
PYBIND_LIB ?= _pbindings.so 

all: $(PYBIND_LIB)

$(ISRCS): $(INTERFACE)
	$(SWIG) -python $<

$(PYBIND_LIB): $(ISRCS)
       $(CC) $(CFLAGS) $(LDFLAGS) -shared $(ISRCS) -o $(PYBIND_LIB) $(LIBS) $(INCLUDES) $(PYTHON_INCLUDE)

clean:
	rm -f *.o *.so* *_wrap* *.pyc *.py __pycache__  platformstats


