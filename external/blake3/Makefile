# Makefile of the library "external/blake3"
# Paul Boersma, 27 June 2026

CPPFLAGS += -I ../../melder

OBJECTS = blake3.o blake3_dispatch.o blake3_portable.o

.PHONY: all clean

all: libblake3.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libblake3.a

libblake3.a: $(OBJECTS)
	ar rus libblake3.a $(OBJECTS)

$(OBJECTS): *.h ../../melder/*.h
