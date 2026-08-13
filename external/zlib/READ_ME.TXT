praat/external/zlib/READ_ME.TXT
Paul Boersma, 18 July 2026

The following changes needed to be implemented to integrate zlib into Praat:

- include the following at the top of zlib.h, in order not to have to include gdclose_w:

#define NO_GZCOMPRESS

