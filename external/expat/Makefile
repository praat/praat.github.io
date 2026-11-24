# Makefile of the library "external/expat"
# David Weenink, 24 November 2026

CPPFLAGS += -I ../../melder -I ../zlib -DNOUNCRYPT

OBJECTS = random_arc4random.o random_arc4random_buf.o random_dev_urandom.o random_getentropy.o \
	random_getrandom.o random_rand_s.o xcs.o xmlparse.o xmlrole.o xmltok.o \
	unzip.o ioapi.o

.PHONY: all clean

all: libexpat.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libexpat.a

libexpat.a: $(OBJECTS)
	touch libexpat.a
	rm libexpat.a
	$(AR) cq libexpat.a $(OBJECTS)
	$(RANLIB) libexpat.a
$(OBJECTS): *.h xmltok_impl.c xmltok_ns.c ../zlib/*.h ../../melder/*.h
