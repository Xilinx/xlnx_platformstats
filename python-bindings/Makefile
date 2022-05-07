.PHONY:	clean all

#Dynamic loading for C
CC ?= gcc
CFLAGS += -fPIC -Wall -Wextra
LDFLAGS += -shared -L../src -L.
LDLIBS += -lplatformstats -lpython$(PYTHON_BASEVERSION)
INCLUDE_DIR = ../include/platformstats

incdir ?= /usr/include

#Python
PYTHON_BASEVERSION ?= 3.10
PYTHON_DIR ?= python${PYTHON_BASEVERSION}
PYTHON_INCLUDE_DIR ?= ${incdir}/$(PYTHON_DIR)

#Swig
SWIG ?= swig
SWIGOPT ?=
IWRAP      = $(INTERFACE:.i=_wrap.i)
ISRCS      = $(IWRAP:.i=.c)
IOBJS      = $(IWRAP:.i=.o)

INTERFACE = platformstats_bindings.i
PYBIND_LIB ?= _platformstats.so

all: $(PYBIND_LIB)

$(ISRCS): $(INTERFACE)
	$(SWIG) -python $<

$(PYBIND_LIB): $(ISRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(ISRCS) -o $(PYBIND_LIB) $(LDLIBS) -I$(INCLUDE_DIR) -I$(PYTHON_INCLUDE_DIR)

clean:
	rm -f *.o *.so* *_wrap* *.pyc *.py __pycache__  platformstats

