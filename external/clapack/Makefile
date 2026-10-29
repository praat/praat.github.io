# Makefile of the library "external/clapack"
# David Weenink and Paul Boersma, 23 May 2026

CPPFLAGS += -I ../../melder

OBJECTS = blas.o \
	lapack.o lapack_dg.o lapack_dlaq.o \
	lapack_dlar.o lapack_ds.o lapack_dt.o

.PHONY: all clean

all: libclapack.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libclapack.a

libclapack.a: $(OBJECTS)
	ar rus libclapack.a $(OBJECTS)

$(OBJECTS): *.h ../../melder/*.h
