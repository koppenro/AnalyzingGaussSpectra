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

all: CalibrationLine

CalibrationLine.o : CalibrationLine.cc
	$(CC) $(CCFLAGS) -c CalibrationLine.cc -o CalibrationLine.o

CalibrationLine : CalibrationLine.o
	$(LD) CalibrationLine.o $(LDFLAGS) $(GLIBS) -o CalibrationLine

clean:
	rm *.o
	rm *~
