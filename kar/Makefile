# Makefile of the library "kar"
# Paul Boersma, 23 May 2026

CPPFLAGS += -I ../melder

OBJECTS = ipaSerifRegularPS.o longchar.o wctype_portable.o

.PHONY: all clean

all: libkar.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libkar.a

libkar.a: $(OBJECTS)
	ar rus libkar.a $(OBJECTS)

$(OBJECTS): *.h ../melder/*.h
