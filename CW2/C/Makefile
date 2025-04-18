# Makefile for Coursework Part 2

#
# C compiler and options for Intel
#

#CC=     icc -O3 -qopenmp -std=c99 -g
CC=      gcc -Og -fopenmp -std=c99 -g
LIB=    -lm
#VERSION := $(shell $(CC) --version | head -n1)

#
# C compiler and options for GNU 
#
#CC=     gcc -O3 -fopenmp
#LIB=	-lm

#
# Object files
#

OBJ1=    solver1.o function.o
OBJ2=    solver2.o function.o
OBJ3=    solver3.o function.o
#OBJ4=    solver4.o function.o
#
# Compile
#

all: solver1 solver2 solver3 #solver4

solver1:   $(OBJ1)
	$(CC) -o $@ $(OBJ1) $(LIB) 
solver2:   $(OBJ2)
	$(CC) -o $@ $(OBJ2) $(LIB)
solver3:   $(OBJ3)
	$(CC) -o $@ $(OBJ3) $(LIB)
#solver4:   $(OBJ4)
#	$(CC) -o $@ $(OBJ4) $(LIB)

.c.o:
	$(CC) -c $<

#
# Clean out object files and the executable.
#
clean:
	rm *.o solver1 solver2 solver3 #solver4
