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

all: FitGaussDistribution

FitGaussDistribution.o : FitGaussDistribution.cc
	$(CC) $(CCFLAGS) -c FitGaussDistribution.cc -o FitGaussDistribution.o

FitGaussDistribution : FitGaussDistribution.o
	$(LD) FitGaussDistribution.o $(LDFLAGS) $(GLIBS) -o FitGaussDistribution

clean:
	rm *.o
	rm *~
