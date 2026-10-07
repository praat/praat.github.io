# Makefile of the library "external/lame"
# David Weenink and Paul Boersma, 23 May 2026

CPPFLAGS += -I ../../melder

OBJECTS = bitstream.o encoder.o fft.o gain_analysis.o mpglib_interface.o id3tag.o lame.o \
	newmdct.o psymodel.o quantize.o quantize_pvt.o set_get.o vbrquantize.o \
	reservoir.o tables.o takehiro.o util.o VbrTag.o \
	version.o presets.o vector_xmm_quantize_sub.o

.PHONY: all clean

all: liblame.a

clean:
	$(RM) $(OBJECTS)
	$(RM) liblame.a

liblame.a: $(OBJECTS)
	ar rus liblame.a $(OBJECTS)

$(OBJECTS): *.h

