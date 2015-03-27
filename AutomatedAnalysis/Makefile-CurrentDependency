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

all: CurrentDependency

CurrentDependency.o : CurrentDependency.cc
	$(CC) $(CCFLAGS) -c CurrentDependency.cc -o CurrentDependency.o

CurrentDependency : CurrentDependency.o
	$(LD) CurrentDependency.o $(LDFLAGS) $(GLIBS) -o CurrentDependency

clean:
	rm *.o
	rm *~
