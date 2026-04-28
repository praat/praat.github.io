praat/external/lame/READ_ME.TXT
Paul Boersma, 4 April 2026

This file describes the changes that were needed to make the `lame` library run
within the Praat sources.

The file config.h had to be renamed to lame__config.h, so as not to clash
with other files called config.h in Praat (gsl, espeak. vorbis, flac...).

All instances of #ifdef HAVE_CONFIG_H were commented out,
so as not to have to define HAVE_CONFIG_H as a Praat-wide global symbol if we are working in an IDE (Xcode).

Remove from lame_config.h the 11 lines that try to define WORDS_BIGENDIAN.
