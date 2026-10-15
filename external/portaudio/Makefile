# Makefile of the library "external/portaudio"
# Paul Boersma, 16 May 2026

OBJECTS = \
	pa_unix_hostapis.o pa_unix_util.o pa_linux_alsa.o pa_jack.o pa_ringbuffer.o \
	pa_win_hostapis.o pa_win_util.o pa_win_wmme.o pa_win_waveformat.o \
	pa_front.o pa_debugprint.o pa_cpuload.o \
	pa_allocation.o pa_process.o pa_converters.o pa_dither.o \
	pa_stream.o

.PHONY: all clean

all: libportaudio.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libportaudio.a

libportaudio.a: $(OBJECTS)
	ar rus libportaudio.a $(OBJECTS)

$(OBJECTS): *.h
