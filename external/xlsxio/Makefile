# Makefile of the library "external/xlsxio"
# David Weenink, 22 November 2026

CPPFLAGS += -I ../../melder -I ../expat -DUSE_MINIZIP

OBJECTS = xlsxio_read.o xlsxio_read_sharedstrings.o

.PHONY: all clean

all: libxlsxio.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libxlsxio.a

libxlsxio.a: $(OBJECTS)
	touch libxlsxio.a
	rm libxlsxio.a
	$(AR) cq libxlsxio.a $(OBJECTS)
	$(RANLIB) libxlsxio.a

$(OBJECTS): *.h ../../melder/*.h
