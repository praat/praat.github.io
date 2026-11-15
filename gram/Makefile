# Makefile of the library "gram"
# Paul Boersma, 23 May 2026

CPPFLAGS += -I ../kar -I ../melder -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../fon -I ../foned

OBJECTS = Network.o \
   OTGrammar.o OTGrammarEditor.o manual_gram.o praat_gram.o OTMulti.o OTMultiEditor.o \
   OTGrammar_ex_metrics.o OTGrammar_ex_NoCoda.o OTGrammar_ex_NPA.o OTGrammar_ex_tongueRoot.o \
   OTMulti_ex_metrics.o Layer.o Net.o \
   NoulliGrid.o NoulliGridArea.o NoulliGridEditor.o CubeGrid.o CubeGridArea.o CubeGridEditor.o

.PHONY: all clean

all: libgram.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libgram.a

libgram.a: $(OBJECTS)
	ar rus libgram.a $(OBJECTS)

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../fon/*.h ../foned/*.h
