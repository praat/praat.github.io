# Makefile of the library "external/mp3"
# Erez Volk and Paul Boersma, 23 May 2026

CPPFLAGS += -I ../../melder

OBJECTS = mp3.o \
	mad_bit.o \
	mad_decoder.o \
	mad_fixed.o \
	mad_frame.o \
	mad_huffman.o \
	mad_layer12.o \
	mad_layer3.o \
	mad_stream.o \
	mad_synth.o \
	mad_timer.o \
	mad_version.o

.PHONY: all clean

all: libmp3.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libmp3.a

libmp3.a: $(OBJECTS)
	ar rus libmp3.a $(OBJECTS)

$(OBJECTS): *.h ../../melder/*.h
