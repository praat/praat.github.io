praat/external/opusfile/READ_ME.TXT
David Weenink and Paul Boersma, 20 August 2021

This file describes the adaptations made to libopusfile-0.12 sources, the part of Opus that directly interacts with Praat,
to make them compilable with the C++ compiler and compatible with Praat.

We adapted the header files and sources to be able to compile with our compilers.
We replaced all memory allocations with the corresponding Melder routines.

In a subdirectory "opus" we copied the Opus-1.3.1 sources which also include a "celt" and a "silk" directory.
The file opus_encoder.c that was clearly not meant for decoding was left out.
The celt and silk directories were left intact, even their encoder parts. However, all their platform- subdirectories were removed.
The silk code also needs a subdirectory "float" in order to work properly with floating-point arithmetic.

All instances of #ifdef HAVE_CONFIG_H were commented out,
so as not to have to define HAVE_CONFIG_H as a Praat-wide global symbol if we are working in an IDE (Xcode).
The file config.h had to be renamed opus__config.h, so as not to clash with three other files called config.h
in Praat (under espeak and vorbis and flac; they also have to be renamed).
The file opusfile_stream.cpp uses the nonportable Windows function _filelengthi64(),
so we adapted that to use fseeko() instead, which is available with x86_64-w64-mingw32;
the same file uses _get_osfhandle(), for which we included <io.h>.

The files opus/opus_compare.c en opus/celt/custom_demo.c contain "main" functions and are not needed.

Rename internal.h to opusfile_internal.h, and adapt the #include statements accordingly.
