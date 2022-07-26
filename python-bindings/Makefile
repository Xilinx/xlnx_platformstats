.PHONY:	clean all install

#Dynamic loading for C
CC ?= gcc
INSTALL_PROGRAM ?= install
CFLAGS += -fPIC -Wall -Wextra
LDFLAGS += -shared -L../src -L.
LDLIBS += -lxlnx_platformstats -lpython$(PYTHON_BASEVERSION)
INCLUDE_DIR = ../include/xlnx_platformstats

incdir ?= /usr/include
libdir ?= /usr/lib

#Python
PYTHON_BASEVERSION ?= 3.10
PYTHON_DIR ?= python${PYTHON_BASEVERSION}
PYTHON_INCLUDE_DIR ?= ${incdir}/$(PYTHON_DIR)
PYTHON_SITEPACKAGES_DIR ?= ${libdir}/$(PYTHON_DIR)

#Swig
SWIG ?= swig
SWIGOPT ?=
IWRAP      = $(INTERFACE:.i=_wrap.i)
ISRCS      = $(IWRAP:.i=.c)
IOBJS      = $(IWRAP:.i=.o)

INTERFACE = xlnx_platformstats_bindings.i
PYBIND_LIB ?= _xlnx_platformstats.so

all: $(PYBIND_LIB)

$(ISRCS): $(INTERFACE)
	$(SWIG) -python $<

$(PYBIND_LIB): $(ISRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(ISRCS) -o $(PYBIND_LIB) $(LDLIBS) -I$(INCLUDE_DIR) -I$(PYTHON_INCLUDE_DIR)

clean:
	rm -f *.o *.so* *_wrap* *.pyc *.py __pycache__  xlnx_platformstats

install: $(PYBIND_LIB)
	$(INSTALL_PROGRAM) -d $(DESTDIR)${PYTHON_SITEPACKAGES_DIR}/xlnx_platformstats
	$(INSTALL_PROGRAM) -m 0644 xlnx_platformstats.py $(DESTDIR)${PYTHON_SITEPACKAGES_DIR}/xlnx_platformstats
	$(INSTALL_PROGRAM) -m 0644 _xlnx_platformstats.so $(DESTDIR)${PYTHON_SITEPACKAGES_DIR}/xlnx_platformstats
