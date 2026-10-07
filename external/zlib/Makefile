# Makefile of the library "external/zlib"
# Paul Boersma, 19 July 2026

CPPFLAGS += -I ../../melder

OBJECTS = adler32.o crc32.o gzread.o gzlib.o gzclose.o \
	inflate.o inffast.o inftrees.o zutil.o

.PHONY: all clean

all: libzlib.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libzlib.a

libzlib.a: $(OBJECTS)
	ar rus libzlib.a $(OBJECTS)

$(OBJECTS): *.h ../../melder/*.h
