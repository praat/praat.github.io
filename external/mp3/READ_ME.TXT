Praats/external/flac/READ_ME.TXT
Paul Boersma, 5 April 2026

This file describes the adaptations to the MAD sources
that are needed to make them compatible with Praat.

Replace all occurrences of
	#if defined (WORDS_BIGENDIAN)
with
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
