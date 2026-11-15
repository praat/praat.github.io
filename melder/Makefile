# Makefile of the library "melder"
# Paul Boersma, 23 May 2026

CPPFLAGS += -I ../melder -I ../sys

OBJECTS = melder.o complex.o melder_alloc.o melder_str32.o melder_colour.o \
	melder_ftoa.o melder_console.o melder_atof.o melder_files.o \
	melder_sort.o MelderString.o melder_textencoding.o melder_debug.o MelderFile.o MelderCat.o \
	melder_search.o MelderThread.o \
	melder_info.o melder_error.o melder_warning.o melder_progress.o melder_casual.o \
	melder_play.o melder_help.o melder_time.o \
	melder_audio.o melder_audiofiles.o melder_quantity.o MelderReadText.o melder_tensorio.o \
	abcio.o melder_sysenv.o melder_app.o melder_trust.o regularExp.o \
	NUMmath.o \
	NUMspecfunc.o NUMear.o NUMinterpol.o NUMmetrics.o NUMrandom.o NUMfilter.o NUMlinprog.o \
	NUM.o STR.o VEC.o MAT.o STRVEC.o

.PHONY: all clean

all: libmelder.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libmelder.a

libmelder.a: $(OBJECTS)
	ar rus libmelder.a $(OBJECTS)

$(OBJECTS): *.h ../kar/*.h ../sys/*.h ../dwsys/*.h
