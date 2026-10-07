# Makefile of the library "FFNet"
# David Weenink and Paul Boersma, 23 May 2026

CPPFLAGS += -I ../kar -I ../melder -I ../sys -I ../dwtools -I ../fon -I ../dwsys -I ../stat -I ../gram

OBJECTS = FFNet.o \
	FFNet_Eigen.o FFNet_Matrix.o FFNet_PatternList.o \
	FFNet_ActivationList_Categories.o FFNet_PatternList_ActivationList.o \
	FFNet_PatternList_Categories.o \
	praat_FFNet_init.o manual_FFNet.o

.PHONY: all clean

all: libFFNet.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libFFNet.a

libFFNet.a: $(OBJECTS)
	ar rus libFFNet.a $(OBJECTS)

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwtools/*.h ../fon/*.h ../dwsys/*.h ../stat/*.h ../gram/*.h
