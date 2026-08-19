# Makefile of the library "foned"
# Paul Boersma, 18 August 2026

CPPFLAGS += -I ../kar -I ../melder -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../LPC -I ../fon -I ../external/portaudio -I ../external/flac -I ../external/mp3 -I ../external/espeak

OBJECTS = SoundRecorder.o \
   FunctionArea.o SoundArea.o SoundAnalysisArea.o TextGridArea.o \
   SpectrumArea.o PointArea.o PitchArea.o SpectrogramArea.o \
   RealTierArea.o PitchTierArea.o IntensityTierArea.o DurationTierArea.o AmplitudeTierArea.o \
   FormantGridArea.o MovieArea.o \
   FunctionEditor.o \
   PitchEditor.o SoundEditor.o SpectrumEditor.o SpectrogramEditor.o PointEditor.o \
   RealTierEditor.o PitchTierEditor.o IntensityTierEditor.o \
   DurationTierEditor.o AmplitudeTierEditor.o \
   ManipulationEditor.o TextGridEditor.o FormantGridEditor.o \
   RunnerMFC.o MovieWindow.o CorpusEditor.o

.PHONY: all clean

all: libfoned.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libfoned.a

libfoned.a: $(OBJECTS)
	touch libfoned.a
	rm libfoned.a
	$(AR) cq libfoned.a $(OBJECTS)
	$(RANLIB) libfoned.a

$(OBJECTS): *.h ../external/portaudio/*.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../LPC/*.h ../fon/*.h ../external/flac/*.h ../external/mp3/mp3.h
