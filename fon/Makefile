# Makefile of the library "fon"
# Paul Boersma and Anastasia Shchupak, 23 May 2026

CPPFLAGS += -I ../kar -I ../melder -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../LPC -I ../foned -I ../fon -I ../external/portaudio -I ../external/flac -I ../external/mp3 -I ../external/espeak -I ../external/whispercpp

OBJECTS = Transition.o Distributions_and_Transition.o \
   Function.o Sampled.o SampledXY.o Matrix.o Vector.o Polygon.o PointProcess.o \
   Matrix_and_PointProcess.o Matrix_and_Polygon.o AnyTier.o RealTier.o \
   Sound.o LongSound.o SoundSet.o Sound_files.o Sound_audio.o PointProcess_and_Sound.o Sound_PointProcess.o ParamCurve.o \
   Pitch.o Harmonicity.o Intensity.o Matrix_and_Pitch.o Sound_to_Pitch.o \
   Sound_to_Intensity.o Sound_to_Harmonicity.o Sound_to_Harmonicity_GNE.o Sound_to_PointProcess.o \
   Pitch_to_PointProcess.o Pitch_to_Sound.o Pitch_Intensity.o \
   PitchTier.o Pitch_to_PitchTier.o PitchTier_to_PointProcess.o PitchTier_to_Sound.o Manipulation.o \
   Pitch_AnyTier_to_PitchTier.o IntensityTier.o DurationTier.o AmplitudeTier.o \
   Spectrum.o Ltas.o Spectrogram.o SpectrumTier.o Ltas_to_SpectrumTier.o \
   Formant.o Image.o Sound_to_Formant.o Sound_and_Spectrogram.o \
   Sound_and_Spectrum.o Spectrum_and_Spectrogram.o Spectrum_to_Formant.o \
   FormantTier.o TextGrid.o TextGrid_files.o TextGrid_Sound.o Label.o FormantGrid.o \
   Excitation.o Cochleagram.o Cochleagram_and_Excitation.o Excitation_to_Formant.o \
   Sound_to_Cochleagram.o Spectrum_to_Excitation.o \
   VocalTract.o VocalTract_to_Spectrum.o \
   Sound_enhance.o VoiceAnalysis.o H1minusH2Tier.o \
   SpeechRecognizer.o \
   WordList.o SpellingChecker.o \
   FujisakiPitch.o \
   ExperimentMFC.o manual_ExperimentMFC.o praat_ExperimentMFC.o \
   Photo.o Movie.o \
   Corpus.o \
   manual_Picture.o manual_Manual.o \
   manual_commands.o manual_formulas.o manual_functions.o manual_scripting.o manual_demoWindow.o \
   manual_whatsnew.o manual_soundFiles.o manual_licenses.o manual_tutorials.o manual_references.o \
   manual_programming.o manual_Fon.o manual_voice.o Praat_tests.o \
   manual_glossary.o manual_Sampling.o manual_exampleSound.o \
   manual_sound.o manual_pitch.o manual_spectrum.o manual_formant.o \
   manual_pointProcess.o manual_annotation.o \
   manual_SpeechRecognizer.o \
   praat_TimeFunction.o praat_TimeTier.o praat_TimeFrameSampled.o \
   praat_Sound.o praat_Matrix.o praat_Tiers.o praat_TextGrid_init.o praat_uvafon_init.o \
   praat_SpeechRecognizer.o


.PHONY: all clean

all: libfon.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libfon.a

libfon.a: $(OBJECTS)
	ar rus libfon.a $(OBJECTS)

$(OBJECTS): *.h ../external/portaudio/*.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../LPC/*.h ../foned/*.h ../external/flac/*.h ../external/mp3/mp3.h
