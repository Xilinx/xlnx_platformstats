#COPY THIS SCRIPT TO BINDINGS DIR BEFORE EXECUTION
swig -python platformstats_bindings.i
gcc -fPIC -Wall -Wextra -shared platformstats_bindings_wrap.c -o _pbindings.so -L. -L../src -lplatformstats -I../include/platformstats -I/usr/include/python3.7m/ -lpython3.7m

