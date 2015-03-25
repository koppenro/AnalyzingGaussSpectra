ROOTCFLAGS      = $(shell root-config --cflags)
ROOTLIBS        = $(shell root-config --libs)
ROOTGLIBS       = $(shell root-config --glibs)

CC              = g++
CCFLAGS         = -g -O -Wall
CCSOFLAGS       = -shared -fPIC

LD              = g++
LDFLAGS         = -O

CCFLAGS 	+= $(ROOTCFLAGS) # find header files
LIBS		= $(ROOTLIBS) #librarys
GLIBS		= $(ROOTGLIBS)

#########################

all: exe

main.o : main.cc
	$(CC) $(CCFLAGS) -c main.cc -o main.o

exe : main.o
	$(LD) main.o $(LDFLAGS) $(GLIBS) -o exe

clean:
	rm *.o
	rm *~
