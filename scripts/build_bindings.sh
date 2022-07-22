#COPY THIS SCRIPT TO BINDINGS DIR BEFORE EXECUTION
swig -python xlnx_platformstats_bindings.i
gcc -fPIC -Wall -Wextra -shared xlnx_platformstats_bindings_wrap.c -o _xlnx_platformstats.so -L. -L../src -lxlnx_platformstats -I../include/xlnx_platformstats -I/usr/include/python3.7m/ -lpython3.7m

