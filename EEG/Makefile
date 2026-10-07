# Makefile of the library "EEG"
# Paul Boersma, 23 May 2026

CPPFLAGS += -I ../kar -I ../melder -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../fon -I ../foned

OBJECTS = EEG.o EEGArea.o EEGAnalysisArea.o \
   EEGWindow.o ERPTier.o ERP.o ERPArea.o ERPWindow.o \
   praat_EEG.o manual_EEG.o

.PHONY: all clean

all: libEEG.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libEEG.a

libEEG.a: $(OBJECTS)
	ar rus libEEG.a $(OBJECTS)

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../fon/*.h ../foned/*.h
