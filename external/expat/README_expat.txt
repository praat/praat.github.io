20-11-2026 djmw
libexpat: XML parser

$EXPAT = ~/projects/libexpat/expat/
$PRAATEXPAT = praat/external/expat/

1. From kdevelop
Fetch: https://github.com/libexpat/libexpat
Saved in ~/projects/libexpat

2.
cd ~/projects/libexpat
cd expat
./buildconf.sh
./configure
$EXPAT = ~/projects/libexpat/expat/

make -j4
make test

3.
mkdir ~/projects/praat/external/expat

cp 
