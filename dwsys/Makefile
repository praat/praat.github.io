# makefile for library "dwsys".
# David Weenink and Paul Boersma, 23 May 2026

CPPFLAGS += -I ../melder -I ../stat -I ../sys -I ../fon -I ../external/gsl -I ../external/num -I ../external/clapack -I ../kar -I ../dwtools

all: libdwsys.a

OBJECTS = ChebyshevSeries.o Collection_extensions.o Command.o \
	DoublyLinkedList.o Eigen.o ExtendedReal.o \
	FileInMemory.o \
	FunctionSeries.o \
	Graphics_extensions.o Index.o \
	LegendreSeries.o \
	MAT_numerics.o \
	NMF.o NUM2.o NUMhuber.o NUMmachar.o  \
	NUMcomplex.o NUMFourier.o NUMsorting.o \
	NUMmathlib.o NUMstring.o NUMselect.o \
	Permutation.o PermutationInversionCounter.o Permutation_and_Index.o \
	Polynomial.o \
	Roots.o \
	SlopeSelector.o Spline.o SVD.o WorkvectorPool.o

.PHONY: all clean

all: libdwsys.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libdwsys.a

libdwsys.a: $(OBJECTS) NUMmachar.o
	ar rus libdwsys.a $(OBJECTS)

$(OBJECTS): *.h ../melder/*.h ../stat/*.h ../sys/*.h ../fon/*.h ../external/gsl/*.h ../external/num/*.h ../dwsys/*.h ../kar/*.h

