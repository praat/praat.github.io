20-11-2026 djmw

Four parts:

Part 1: the actual xlsx reader:

1. mkdir praat/external/xlsxio

Download from https://github.com/brechtsanders/xlsxio/tree/master/lib the files
xlsxio_read.c, xlsxio_read_sharedstrings.c, xlsxio_private.h, xlsxio_read.h,
xlsxio_read_sharedstrings.h, xlsxio_version.h to praat/external/xlsxio

2. Create the corresponding meson.build & Makefile
with the '-DUSE_MINIZIP' switch for compiling



Part 2: Download the XML parser

2.1. mkdir praat/external/expat

$EXPAT = ~/projects/libexpat/expat/
$PRAATEXPAT = praat/external/expat/

2.2 Fetch: https://github.com/libexpat/libexpat
Save it in ~/projects/libexpat and configure it:

cd ~/projects/libexpat/expat
./buildconf.sh
./configure

make -j4
make test

2.3. cp the *.c and the *.h files from $EXPAT/lib to $PRAATEXPAT

Part 3:

Download zip code from https://github.com/madler/zlib/blob/master/contrib/minizip/ to the external/expat
unzip.c ioapi.c unzip.h
Use the '-DNOUNCRYPT' switch for the build.

Part 4:

Making the parts work together, compilable on all platforms and use the Praat error mechanism:

4.1 Include a expat_config.h (written by Claude)

4.2 Each random_*.c file needs a platform guard. Without one, the build fails on Windows and Mac.
Also, random_rand_s.c, which Windows needs, isn’t built at all.

4.3 Include Praat's error handling which is C++ only. Currently XML parse errors are hidden by expat.
we therefore needt to adapt xlsxio_read.c to record the error to use this in our Table_readFromXLSXFile.

4.4 Make many routines static in xlsxio_read.c.

