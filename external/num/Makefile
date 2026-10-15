# Makefile of the library "external/num"
# Paul Boersma, 23 May 2026

CPPFLAGS += -I ../../melder

OBJECTS = median_of_ninthers.o

.PHONY: all clean

all: libnum.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libnum.a

libnum.a: $(OBJECTS)
	ar rus libnum.a $(OBJECTS)

$(OBJECTS): *.h ../../melder/*.h
