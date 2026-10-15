# Makefile of the library "external/espeak"
# David Weenink and Paul Boersma, 23 May 2026

CPPFLAGS += -I ../../kar -I ../../melder -I ../../sys -I ../../dwsys -I ../../stat

OBJECTS = case.o categories.o common.o compiledict.o \
	dictionary.o \
	encoding.o error.o espeak_api.o \
	intonation.o klatt.o langopts.o mnemonics.o \
	numbers.o phonemelist.o proplist.o readclause.o \
	setlengths.o soundicon.o speech.o \
	ssml.o synthdata.o synthesize.o \
	tr_languages.o translate.o translateword.o voices.o wavegen.o \
	espeak_praat_FileInMemorySet_addLanguages.o espeak_praat_FileInMemorySet_addVoices.o \
	espeak_praat_FileInMemorySet_addDicts.o espeak_praat_FileInMemorySet_addPhon.o \
	espeak_praat.o

.PHONY: all clean

all: libespeak.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libespeak.a

libespeak.a: $(OBJECTS)
	ar rus libespeak.a $(OBJECTS)

$(OBJECTS):  *.h ../../kar/*.h ../../melder/*.h ../../sys/*.h ../../dwsys/*.h ../../stat/*.h

