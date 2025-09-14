# Praat: doing phonetics by computer

Welcome to Praat! Praat is a speech analysis tool used for doing phonetics by computer.
Praat can analyse, synthesize, and manipulate speech, and create high-quality pictures for your publications.
Praat was created by Paul Boersma and David Weenink of the Institute of Phonetics Sciences of the University of Amsterdam.

Some of Praat’s most prominent features are:

#### Speech analysis

Praat allows you to analyze different aspects of speech including pitch, formant, intensity, and voice quality.
You have access to spectrograms (a visual representation of sound changing over time)
and cochleagrams (a specific type of spectrogram more closely resembling how the inner ear receives sound).

#### Speech synthesis

Praat allows you to generate speech from a pitch curve and filters that you create (acoustic synthesis),
or from muscle activities (articulatory synthesis).

#### Speech manipulation

Praat gives you the ability to modify existing speech utterances. You can alter pitch, intensity, and duration of speech.

#### Speech labelling

Praat allows you to custom-label your samples using the IPA (International Phonetics Alphabet),
and annotate your sound segments based on the particular variables you are seeking to analyze.
Multi-language text-to-speech facilities allow you to segment the sound into words and phonemes.

#### Grammar models

With Praat, you can try out Optimality-Theoretic and Harmonic-Grammar learning,
as well as several kinds of neural-network models.

#### Statistical analysis

Praat allows you to perform several statistical techniques, among which
multidimensional scaling, principal component analysis, and discriminant analysis.

For more information, consult the extensive manual in Praat (under Help),
and the website [praat.org](https://praat.org), which has Praat tutorials in several languages.

For code contributions, refer to [2. Compiling the source code](#2-compiling-the-source-code) and subsequent subheadings.


## Contents
  - [Contents](#contents)
  - [1. Binary executables](#1-binary-executables)
    - [1.1. Windows binaries](#11-windows-binaries)
    - [1.2. Mac binaries](#12-mac-binaries)
    - [1.3. Linux binaries](#13-linux-binaries)
    - [1.4. Chromebook binaries](#14-chromebook-binaries)
    - [1.5. Raspberry Pi binaries](#15-raspberry-pi-binaries)
    - [1.6. Other Unix binaries (all obsolete)](#16-other-unix-binaries-all-obsolete)
  - [2. Compiling the source code](#2-compiling-the-source-code)
    - [2.1. License](#21-license)
    - [2.2. Downloading the archive](#22-downloading-the-archive)
    - [2.3. Steps to take if you want to extend Praat](#23-steps-to-take-if-you-want-to-extend-praat)
    - [2.4. The programming language](#24-the-programming-language)
  - [3. Developing Praat for one platform](#3-developing-praat-for-one-platform)
    - [3.1. Developing Praat for Windows](#31-developing-praat-for-windows)
    - [3.2. Compiling for Macintosh](#32-compiling-for-macintosh)
    - [3.3. Compiling on Linux and other Unixes](#33-compiling-on-linux-and-other-unixes)
  - [4. Developing Praat on all platforms simultaneously](#4-developing-praat-on-all-platforms-simultaneously)
    - [4.1. MacOS development set-up](#41-macos-development-set-up)
    - [4.2. Windows development set-up](#42-windows-development-set-up)
    - [4.3. Linux development set-up](#43-linux-development-set-up)
    - [4.4. Chromebook development set-up](#44-chromebook-development-set-up)
    - [4.5. Raspberry Pi development set-up](#45-raspberry-pi-development-set-up)
    - [4.6. s390x development set-up on LinuxONE](#46-s390x-development-set-up-on-linuxone)
    - [4.7. Distributing Praat](#47-distributing-praat)


## 1. Binary executables

While the [Praat website](https://praat.org) contains the latest executable for all platforms that we support
(or used to support), the [releases on GitHub](https://github.com/praat/praat.github.io/releases) contain many older executables as well.

The meaning of the names of binary files available on GitHub is as follows (editions that currently receive updates are in bold):

### 1.1. Windows binaries
- **`praatXXXX_win-arm64.zip`: zipped executable for ARM64 Windows (11 and higher)**
- **`praatXXXX_win-intel64.zip`: zipped executable for Intel64/AMD64 Windows (7 and higher)**
- **`praatXXXX_win-intel32.zip`: zipped executable for Intel32 Windows (7 and higher)**
- `praatXXXX_win64.zip`: zipped executable for Intel64/AMD64 Windows (XP and higher, or 7 and higher)
- `praatXXXX_win32.zip`: zipped executable for Intel32 Windows (XP and higher, or 7 and higher)
- `praatconXXXX_win64.zip`: zipped executable for Intel64/AMD64 Windows, console edition
- `praatconXXXX_win32.zip`: zipped executable for Intel32 Windows, console edition
- `praatconXXXX_win32sit.exe`: self-extracting StuffIt archive with executable for Intel32 Windows, console edition
- `praatXXXX_win98.zip`: zipped executable for Windows 98
- `praatXXXX_win98sit.exe`: self-extracting StuffIt archive with executable for Windows 98

### 1.2. Mac binaries
- **`praatXXXX_mac.dmg`: disk image with universal executable for (64-bit) Intel and Apple Silicon Macs (Cocoa)**
- **`praatXXXX_xcodeproj.zip`: zipped Xcode project file for the universal (64-bit) edition (Cocoa)**
- `praatXXXX_mac64.dmg`: disk image with executable for 64-bit Intel Macs (Cocoa)
- `praatXXXX_xcodeproj64.zip`: zipped Xcode project file for the 64-bit edition (Cocoa)
- `praatXXXX_mac32.dmg`: disk image with executable for 32-bit Intel Macs (Carbon)
- `praatXXXX_xcodeproj32.zip`: zipped Xcode project file for the 32-bit edition (Carbon)
- `praatXXXX_macU.dmg`: disk image with universal executable for (32-bit) PPC and Intel Macs (Carbon)
- `praatXXXX_macU.sit`: StuffIt archive with universal executable for (32-bit) PPC and Intel Macs (Carbon)
- `praatXXXX_macU.zip`: zipped universal executable for (32-bit) PPC and Intel Macs (Carbon)
- `praatXXXX_macX.zip`: zipped executable for MacOS X (PPC)
- `praatXXXX_mac9.sit`: StuffIt archive with executable for MacOS 9
- `praatXXXX_mac9.zip`: zipped executable for MacOS 9
- `praatXXXX_mac7.sit`: StuffIt archive with executable for MacOS 7

### 1.3. Linux binaries
- **`praatXXXX_linux-s390x-barren.tar.gz`: gzipped tarred executable for s390x Linux, without GUI, sound and graphics**
- **`praatXXXX_linux-s390x.tar.gz`: gzipped tarred executable for s390x Linux (GTK 3)**
- **`praatXXXX_linux-arm64-barren.tar.gz`: gzipped tarred executable for ARM64 Linux, without GUI, sound and graphics**
- **`praatXXXX_linux-arm64.tar.gz`: gzipped tarred executable for ARM64 Linux (GTK 3)**
- **`praatXXXX_linux-intel64-barren.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux, without GUI, sound and graphics**
- **`praatXXXX_linux-intel64.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux (GTK 3)**
- `praatXXXX_linux-arm64-nogui.tar.gz`: gzipped tarred executable for ARM64 Linux, without GUI and sound but with graphics (Cairo and Pango)
- `praatXXXX_linux-intel64-nogui.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux, without GUI and sound but with graphics (Cairo and Pango)
- `praatXXXX_linux64barren.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux, without GUI, sound and graphics
- `praatXXXX_linux64nogui.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux, without GUI and sound but with graphics (Cairo and Pango)
- `praatXXXX_linux64.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux (GTK 2 or 3)
- `praatXXXX_linux32.tar.gz`: gzipped tarred executable for Intel32 Linux (GTK 2)
- `praatXXXX_linux_motif64.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux (Motif)
- `praatXXXX_linux_motif32.tar.gz`: gzipped tarred executable for Intel32 Linux (Motif)

### 1.4. Chromebook binaries
- **`praatXXXX_chrome-arm64.tar.gz`: gzipped tarred executable for Linux on ARM64 Chromebooks (GTK 3)**
- **`praatXXXX_chrome-intel64.tar.gz`: gzipped tarred executable for Intel64/AMD64 Linux on Intel64/AMD64 Chromebooks (GTK 3)**
- `praatXXXX_chrome64.tar.gz`: gzipped tarred executable for 64-bit Linux on Intel64/AMD64 Chromebooks (GTK 2 or 3)

### 1.5. Raspberry Pi binaries
- **`praatXXXX_rpi-armv7.tar.gz`: gzipped tarred executable for (32-bit) ARMv7 Linux on the Raspberry Pi 4B (GTK 3)**
- `praatXXXX_rpi_armv7.tar.gz`: gzipped tarred executable for (32-bit) ARMv7 Linux on the Raspberry Pi 4B (GTK 2 or 3)

### 1.6. Other Unix binaries (all obsolete)
- `praatXXXX_solaris.tar.gz`: gzipped tarred executable for Sun Solaris
- `praatXXXX_sgi.tar.gz`: gzipped tarred executable for Silicon Graphics Iris
- `praatXXXX_hpux.tar.gz`: gzipped tarred executable for HP-UX (Hewlett-Packard Unix)

## 2. Compiling the source code

You need the Praat source code only in the following cases:

1. you want to extend Praat’s functionality by adding C or C++ code to it; or
2. you want to understand or reuse Praat’s source code; or
3. you want to compile Praat for a computer for which we do not provide binary executables,
e.g. Linux for some non-Intel computers, FreeBSD, HP-UX, SGI, or SPARC Solaris.

Before trying to dive into Praat’s source code, you should be familiar with the working of the Praat program
and with writing Praat scripts. The Praat program can be downloaded from
https://praat.org or https://www.fon.hum.uva.nl/praat.

### 2.1. License

Most of the source code of Praat is distributed on GitHub under the General Public License,
[version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html) or later,
or [version 3](https://praat.org/manual/General_Public_License__version_3.html) or later.
However, as Praat includes software written by others,
the whole of Praat is distributed under the General Public License,
[version 3](https://praat.org/manual/General_Public_License__version_3.html) or later. This can be found [linked here](LICENSE).

See [Acknowledgments](https://praat.org/manual/Acknowledgments.html) for details on the licenses
of software libraries by others that are included in Praat.

Of course, any improvements in the Praat source code are welcomed by the authors.


### 2.2. Downloading the archive

To download the latest source code of Praat from GitHub,
click on the *zip* or *tar.gz* archive at the latest release,
or fork ("clone") the praat/praat repository at any later change.

### 2.3. Steps to take if you want to extend Praat

First make sure that the source code can be compiled as is.
Then add your own buttons by editing `main/main_Praat.cpp` or `fon/praat_Fon.cpp`.
Consult the manual page on [Programming](https://praat.org/manual/Programming_with_Praat.html).

### 2.4. The programming language

Most of the source code is written in C++, but some parts are written in C.
The code requires that your compiler supports C99 and C++17.

## 3. Developing Praat for one platform

Developing Praat means two things: *building* the Praat executable from the Praat
source code, and *testing* the correctness of the Praat executable.

**Building** is largely automated:

- On the Mac, you use an existing Xcode project (these files are included in the releases).
- On other platforms (Windows and Unixes), you use existing makefiles
  (these files are included in the source tree).

**Testing** on a platform can be done by starting up Praat on that platform,
and then go through to types of tests. Basic GUI functionality is tested as follows:

1. record a sound (New -> `Record mono Sound...`)
2. open the sound (`View & Edit`)
3. select a part of the sound (drag the mouse across the waveform)
4. play the sound (click on the rectangle below or above the selection)

The integrity of Praats’s algorithms (e.g. signal processing)
and of the Praat scripting language is tested as follows:

1. open the script `test/runAlltests.praat` (Praat -> `Open Praat script...`)
2. run the script (Run -> `Run`)
3. after 2 to 10 minutes, the Info window should contain a big “OK” graph
4. go through steps 1 through 3 for `dwtest/runAllTests.praat`
5. if you feel adventurous, try some tests in the folder `test/manually`

### 3.1. Developing Praat for Windows

On Windows, Praat is **built** through the makefiles provided in Praat’s source tree.

One could use Cygwin or MSYS2. As we like to provide not only an Intel64/AMD64 and Intel32 edition,
but an ARM64 edition as well, and Cygwin has no toolchains for ARM64, we work with MSYS2 instead.

After installing MSYS2, we see that a `mingw64` toolchain (for Praat’s Intel64/AMD64 edition)
and a `mingw32` toolchain (for Praat’s Intel32 edition) are already available.
Make sure you have installed at least `make`, `gcc`, `g++` and `pkg-config` to make those work.
To also install a `clangarm64` toolchain (for Praat’s ARM64 edition),
run `clangarm64.exe` to get a `clangarm64` shell. In that shell, run `pacman -Suy` to update and
`pacman -S mingw-w64-clang-aarch64-clang` to install the build tools package.
In the same way you can create a `clang64` toolchain and a `clang32` toolchain
(`pacman -S mingw-w64-clang-x86_64-clang` and `pacman -S mingw-w64-i686-clang`),
which are good alternatives to `mingw64` and `mingw32`.

Move the Praat sources folders somewhere in your `/home/yourname` tree,
perhaps even in three places, e.g. as `/home/yourname/praats-arm64`,
`/home/yourname/praats-intel64` and `/home/yourname/praats-intel32`;
the folders `fon` and `sys` should be visible within each of these folders.

If you now want to build Praat’s ARM64 edition, start the shell `clangarm64` and type

    cd ~/praats-arm64
    cp makefiles/makefile.defs.msys-clang ./makefile.defs
    make -j12

If you want to build Praat’s Intel64/AMD64 edition, start the shell `clang64` and type

    cd ~/praats-intel64
    cp makefiles/makefile.defs.msys-clang ./makefile.defs
    make -j12

or start the shell `mingw64` and type

    cd ~/praats-intel64
    cp makefiles/makefile.defs.msys-mingw64 ./makefile.defs
    make -j12

If you want to build Praat’s Intel32 edition, start the shell `clang32` and type

    cd ~/praats-intel32
    cp makefiles/makefile.defs.msys-clang ./makefile.defs
    make -j12

or start the shell `mingw32` and type

    cd ~/praats-intel32
    cp makefiles/makefile.defs.msys-mingw32 ./makefile.defs
    make -j12

(With Cygwin, you would install the Devel package `mingw64-x86_64-gcc-g++`
for Praat’s Intel64/AMD64 edition and `mingw64-i686-gcc-g++` for Praat’s Intel32 edition,
plus perhaps `make` and `pkg-config` if you dont’t have those yet.)

**Code-signing.** From version 6.4.25 on, we have signed the three Praat executables
with an “open-source code-signing certificate” (by Certum)
under the name “Paulus Boersma” (the Dutch-legal name of one of the authors).
This is designed to make it easier for Praat to pass the SmartScreen checks
on Windows 11. Early testing shows that the signature is seen by SmartScreen,
but that SmartScreen can still block Praat, so that users still have to
click “Run Anyway” (or “Unblock” under Properties).
It seems that the card reader for this certificate cannot be used yet for code-signing on ARM64 Windows
or on MacOS Sonoma/Sequoia, so for the moment we have to fall back on an obsolete Intel64/AMD64 Windows 10 machine
and are looking for a more robust solution.

**Testing** on multiple platform versions can be done with virtual machines
for Windows 7 (64-bit), Windows 8.1 (64-bit), 64-bit Windows 10 (1507, 1803, 22H2) and Windows 11,
for instance on an Intel64 Mac with Parallels Desktop.
On an ARM64 Mac with Parallels Desktop, you can test only on Windows 11.

### 3.2. Compiling for Macintosh

To **build** Praat on the Mac, extract the *praatXXXX_xcodeproj.zip* file
from [Praat’s latest release](https://github.com/praat/praat.github.io/releases)
into the folder that contains `sys`, `fon`, `dwtools` and so on (e.g. `~/Dropbox/Praats/src`).
Then open the project `praat.xcodeproj` in Xcode 16.3 (or later),
and edit the Intermediate and Product build paths to something that suits you
(Xcode -> Settings... -> Locations -> Derived Data -> Advanced... -> Custom -> Absolute,
then type something after Products, e.g. `~/Dropbox/Praats/bin/macos`,
as well as something after Intermediates, e.g. `~/builds/mac_intermediates`, then click Done).
After this preliminary work, choose Build or Run for the target `praat_mac`.
You can compile with the 14.2 SDK, which will work as far back as macOS 10.11 El Capitan,
which is our deployment target, and will look good even on macOS 14 Sonoma.

If you get an error message like “Code Signing Identity xxx does not match any valid, non-expired,
code-signing certificate in your keychain”, then select the target `praat_mac`, go to Info → Build,
and switch “Code Signing Identity” to “Don’t Code Sign”,
or sign with your own certificate if you have one as a registered Apple developer.

If you get lots of errors saying “Expected unqualified-id” or “Unknown type name NSString”,
then you may have to switch the Type of some .cpp file from “C++ Source” to “Objective-C++ Source”
(under “Identity and Type” in the righthand sidebar).

If you want to build Praat as a library instead of as an executable,
try the target `praat_mac_a` (static) or `praat_mac_so` (dynamic).

**Notarization.** If you want others to be able to use your Mac app,
you will probably have to not only *sign* the executable, but also *notarize* it. To this end,
do Xcode (version 16) -> Product -> Archive -> Distribute App -> Developer ID -> Upload ->
Automatically manage signing -> Upload -> ...wait... (“Package Approved”) ...wait...
(“Ready to distribute”) -> Export Notarized App). If your Praat.app was built into
`~/Dropbox/Praats/bin/macos/Configuration64`, then you can save the notarized
`Praat.app` in `~/Dropbox/Praats/bin/macos`, then drag it in the Finder to
`~/Dropbox/Praats/bin/macos/Configuration64`, overwriting the non-notarized
Praat.app that was already there. If on the way you receive an error
“App Store Connect Operation Error -- You must first sign the relevant contracts online”,
or “Couldn’t communicate with a helper application“,
you will have to log in to `developer.apple.com` and do Review Agreement -> Agree;
you may then also have to either wait for a couple of minutes,
and or go (or log in) to App Store Connect, then Business (or Agreements, Tax, and Banking)
-> Paid Apps Agreement -> View and Agree to Terms (even if you have no paid apps).

**Testing** on multiple Intel64/AMD64 platform versions can be done on older Intel64 Macs,
using virtual machines with Parallels Desktop. For instance, a 2013 Macbook Pro can handle
OS X 10.11 El Capitan, 10.12 Sierra, 10.13 High Sierra, macOS 10.14 Mojave, 10.15 Catalina,
and macOS 11 Big Sur, while a 2018 Macbook Pro can handle macOS 10.14 Mojave, 10.15 Catalina,
macOS 11 Big Sur, macOS 12 Monterey, and macOS 13 Ventura (and macOS 14 Sonoma natively).
Testing on multiple ARM64 platform versions can be done on an older ARM64 Mac,
using virtual machines with Parallels Desktop. For instance, a 2020 Mac Mini could handle
macOS 11 Big Sur, macOS 12 Monterey, and macOS 13 Ventura (and macOS 14 Sonoma natively),
while a 2023 Macbook Pro can do macOS 14 Sonoma or macOS 15 Sequoia natively.

### 3.3. Compiling on Linux and other Unixes

To set up the system libraries required for **building** with the Clang or GCC compiler,
install the necessary build tools as well as some graphics and sound packages:

    sudo apt install make rsync pkg-config
    # either:
        sudo apt install clang libc++-dev libc++abi-dev
    # or:
        sudo apt install gcc g++
    sudo apt install libgtk-3-dev
    sudo apt install libasound2-dev
    sudo apt install libpulse-dev
    sudo apt install libjack-dev

To set up your source tree for Linux, go to Praat's sources directory (where the folders `fon` and `sys` are)
and type one of the four following commands:

    # on Ubuntu command line (Intel64/AMD64 or ARM64 processor)
    # either:
        cp makefiles/makefile.defs.linux.pulse-clang ./makefile.defs
    # or:
        cp makefiles/makefile.defs.linux.pulse-gcc ./makefile.defs

    # on Ubuntu command line (s390x processor)
    cp makefiles/makefile.defs.linux.s390x.pulse ./makefile.defs

    # on Chromebook command line
    cp makefiles/makefile.defs.chrome64 ./makefile.defs

    # on Raspberry Pi command line
    cp makefiles/makefile.defs.linux.rpi ./makefile.defs
    
    # on FreeBSD command line
    cp makefiles/makefile.defs.freebsd.alsa ./makefile.defs

To build the Praat executable, type `make -j15` or so.
If your Unix isn’t Linux, you may have to edit the library names in the makefile
(you may need pthread, gtk-3, gdk-3, atk-1.0, pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0,
cairo-gobject, cairo, gio-2.0, pango-1.0, freetype, fontconfig, gobject-2.0, gmodule-2.0, 
gthread-2.0, rt, glib-2.0, asound, jack).

When compiling Praat on an external supercomputer or so, you will not have sound.
If you do have `libgtk-3-dev` (and its dependencies), do

    # on Ubuntu command line (Intel64/AMD64 or ARM64 processor)
    cp makefiles/makefile.defs.linux.silent ./makefile.defs

Then type `make -j12` or so to build the program. If your Unix isn’t Linux,
you may have to edit the library names in the makefile (you may need pthread, gtk-3, gdk-3, atk-1.0,
pangoft2-1.0, gdk_pixbuf-2.0, m, pangocairo-1.0, cairo-gobject, cairo, gio-2.0, pango-1.0, 
freetype, fontconfig, gobject-2.0, gmodule-2.0, gthread-2.0, rt, glib-2.0).

When compiling Praat for use as a server for commands from your web pages,
you may not need sound, a GUI, amd graphics. In that case, do

    # on Ubuntu command line (Intel64/AMD64 or ARM64 processor)
    # either:
        cp makefiles/makefile.defs.linux.barren-clang ./makefile.defs
    # or:
        cp makefiles/makefile.defs.linux.barren-gcc ./makefile.defs

    # on Ubuntu command line (s390x processor)
    cp makefiles/makefile.defs.linux.s390x.barren ./makefile.defs

which creates the executable `praat_barren`. Then type `make` or `make -j15` to build the program.
If your Unix isn’t Linux, you may have to edit the library names in the makefile.

The above works exactly the same for Intel64/AMD64 and ARM64 processors, with the same makefiles.

**Testing** on multiple platform versions can be done with virtual machines
for e.g. Ubuntu 20.04, Ubuntu 22.04, Fedora 35, Fedora 37, Mint 20.2,
Debian GNU Linux 10.10, CentOS 8.4, and CentOS Stream 9, 
for instance on an Intel64 Mac with Parallels Desktop.
On an ARM64 Mac, we test with virtual machines for Ubuntu 22.04, Fedora 38,
and Debian GNU Linux 12 ARM64.

## 4. Developing Praat on all platforms simultaneously

At the time of writing (5 January 2024), we develop 12 of the 13 Praat editions on a single
computer, which is a 2023 M3 Macbook Pro. The Mac edition is built natively with Xcode,
the three Windows editions are built via Parallels Desktop 19,
and the six Linux editions and the two Chromebook editions are built via OrbStack;
only the Raspberry Pi edition is built separately (on a Raspberry Pi).
We put all 13 editions into a `bin` folder on Dropbox, so that it is easy to test
the Windows and Linux editions on other computers.

In the following we assume that you want to create all of those editions as well.
We hope that our example will be useful to you.

### 4.1. MacOS development set-up

Your source code folders, such as `fon` and `sys`,
will reside in a folder like `~/Dropbox/Praats/src`,
where you also put `praat.xcodeproj`, as described above in 3.2.
On our 2023 Mac with Xcode 16.3, building Praat with Command-B or Command-R,
after cleaning the build folder with Shift-Command-K,
takes only 56 seconds for the ARM64 part and Intel64 part together (optimization level O3).

### 4.2. Windows development set-up

On a Windows 10 or Windows 11 computer, you can install MSYS2 or Cygwin,
and create some `praats` folders, as described above in 3.1.

If you work under Parallels Desktop on an ARM64 Mac,
you will want MSYS2, because it has an edition for ARM64.
Your source tree will reside on the Windows disk,
which can be much faster than building directly on the MacOS disk.
To move the source from the MacOS disk to the Windows disk,
you “mount” the MacOS disk from MSYS2 or Cygwin; this is easy:
in Parallels Desktop, choose `Windows 11 ARM` -> `Configure`,
then `Options`, then `Sharing`, then `Share Mac`, and set `Share folders` to `Home folder only`
(if this scares you, then use `Custom Folders` instead).
Your MacOS home folder (i.e. `/Users/yourname`) is now visible anywhere on Windows
as the `Z` drive (or so); from any of the three MSYS shells you can access it as `/z`,
and from the Cygwin terminal you can access it as `/cygdrive/z`.

When developing Praat for Windows, you just edit your files in Xcode;
do not forget to save them (as you do e.g. by building in Xcode).
Then, just as you use Command-B and Command-R in Xcode,
you will be able to type `praat-build` (which only builds) or `praat-run` (which builds and runs)
into your MSYS2 shell. To accomplish this,
add the following definitions into `/home/yourname/.bashrc` (i.e. in your MSYS2 Shell home folder),
so that `bash` will automatically execute them whenever you start your
MSYS shell or Cygwin terminal (you will need to have installed `rsync` and `make`).
On our 2023 Mac, the ARM64 edition will be the default,
but the Intel64/AMD64 and Intel32 versions will also be available;
as the same `.bashrc` file is shared among all three editions,
we use the environment variable `MSYSTEM` to differentiate between the three:

    # in MSYS2:~/.bashrc
    if [[ "$MSYSTEM" == "CLANGARM64" ]]; then
        BUILD_FOLDER="~/praats-arm64"
        MAKEFILE_DEFS="makefiles/makefile.defs.msys-arm64"
    elif [[ "$MSYSTEM" == "MINGW64" ]]; then
        BUILD_FOLDER="~/praats-intel64"
        MAKEFILE_DEFS="makefiles/makefile.defs.msys-mingw64"
    elif [[ "$MSYSTEM" == "MINGW32" ]]; then
        BUILD_FOLDER="~/praats-intel32"
        MAKEFILE_DEFS="makefiles/makefile.defs.msys-mingw32"
    fi
    ORIGINAL_SOURCES="/z/Dropbox/Praats/src"
    EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praat-build="( cd $BUILD_FOLDER &&\
        rsync -rptvz $ORIGINAL_SOURCES/ $EXCLUDES . &&\
        cp $MAKEFILE_DEFS ./makefile.defs &&\
        make -j12 )"
    alias praat="$BUILD_FOLDER/Praat.exe"
    alias praat-run="praat-build && praat"

This also defines `praat` for running Praat without first rebuilding it.
The cycle from editing Praat on the Mac to running the new version on Windows therefore takes only two steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praat-run` on your Windows 11 (under Parallels Desktop on your Mac) in one of the three MSYS2 shells.

On our 2023 Mac, the three builds cost 86 seconds for ARM64,
212 seconds for Intel64/AMD64 (under emulation), and 390 seconds for Intel32 (also under emulation).

### 4.3. Linux development set-up

On an Ubuntu 20.04 or 22.04 computer, create a folder `praats` in your home folder,
as described above in 3.3.

If you work under Parallels Desktop (19 or later) on an Intel64 Mac,
choose `Ubuntu 20.04 or 22.04` -> `Configure`,
then `Options`, then `Sharing`, then `Share Mac`, and set `Share folders` to `Home folder only`
(or use `Custom Folders` instead).
Your MacOS home folder (i.e. `/Users/yourname`) is now visible on the Ubuntu desktop
as `Home`, and from the `Terminal` you can access it as `/media/psf/Home`.

However, on an ARM64 Mac this procedure with Parallels Desktop works only for the ARM64 edition.
With OrbStack we can instead create the Intel64/AMD64 edition as well
(and building the ARM64 edition is also faster). Your Mac home folder is known
simply as `/Users/yourname` or so.

When developing Praat for Linux, you just edit and save your files in Xcode.
You will be able to type `praat-build` (which only builds) or `praat-run` (which builds and runs)
into your `Terminal` after you add the following definitions into
`/home/yourname/.bash_aliases` in your Ubuntu home folder
(this will be run automatically by `.bashrc` whenever you start a `Terminal` window,
assuming that it uses the `bash` shell; please note the subtle but crucial difference
between `/Users/yourname` and `/home/yourname`):

    # in Ubuntu:/home/yourname/.bash_aliases
    ORIGINAL_SOURCES="/Users/yourname/Dropbox/Praats/src"
    EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praat-build="( cd ~/praats &&\
        rsync -rptvz $ORIGINAL_SOURCES/ $EXCLUDES . &&\
        cp makefiles/makefile.defs.linux.pulse-clang makefile.defs &&\
        make -j15 )"
    alias praat="~/praats/praat"
    alias praat-run="praat-build && praat"

In OrbStack, if you don’t have a GUI, try `praat-run --version` instead;
but note that you can have a GUI by running XQuartz. With XQuartz running,
you type something like `xhost +192.168.1.99` (if that’s the local IP address of your computer)
into the XQuartz terminal window (or put `xhost +192.168.1.99` and `exec quartz-wm` into your `.xinitrc` file),
and type something like `export DISPLAY=192.168.1.99:0` (depending on your local IP address)
in your OrbStack window (or into your `.bashrc` file), followed by `praat` into your OrbStack window;
the Praat-for-Linux Objects and Picture windows will then show up on your Mac screen.

On our 2023 Mac, building Praat this way from scratch takes 42 seconds for the ARM64 edition
and 130 seconds (under emulation) for the Intel64/AMD64 edition (optimization level O3).

To build `praat_barren`, create a folder `praatsb`, and define

    # in Ubuntu:~/.bash_aliases
    alias praatb-build="( cd ~/praatsb &&\
        rsync -rptvz $ORIGINAL_SOURCES/ $EXCLUDES . &&\
        cp makefiles/makefile.defs.linux.barren-clang makefile.defs &&\
        make -j15 )"
    alias praatb="~/praatsb/praat_barren"
    alias praatb-run="praatb-build && praatb"

You test `praat_barren` briefly by typing

    # on Ubuntu command line
    praatb --version

To build Praat for Chrome64 (64-bit Intel Chromebooks only),
create a folder `praatc`, and define

    # in Ubuntu:~/.bash_aliases
    alias praatc-build="( cd ~/praatsc &&\
        rsync -rptvz $ORIGINAL_SOURCES/ EXCLUDES . &&\
        cp makefiles/makefile.defs.chrome64 makefile.defs &&\
        make -j15 )"
    alias praatc="~/praatsc/praat"
    alias praatc-run="praatc-build && praat"

To test Praat for Chrome64, you can just run it on Ubuntu by typing `praatc`,
or you transfer it to a Chromebook for the real test.

### 4.4. Chromebook development set-up

Parallels Desktop 19 has no emulator for Chrome, so the choice is between
building Praat on a Chromebook directly or building Praat on Ubuntu 20.04 or 22.04.
On a 2019 HP Chromebook with Intel processor, building Praat takes
a forbidding 27 minutes.

So we choose to build Praat on Ubuntu (under Parallels Desktop on an Intel64 Mac),
because building the Intel Chrome64 edition on OrbStack Ubuntu 20.04 takes only
63 seconds (ARM64) or 215 seconds (Intel64/AMD64). If you have the Linux set-up described in 4.3,
you can do this with the `praatc-build` command.

Next, you need a way to get the executable `praat` from Mac/Ubuntu to your Chromebook.
The distributors of Praat do this via an intermediary university computer;
let’s call this computer-in-the-middle `fon.hum.uva.nl`
(not coincidentally, that’s the name of the computer that hosts `praat.org`).
If you have an account on that computer (say it’s called `yourname`),
then you can access that account with `ssh`, and it is best to do that without
typing your password each time. To accomplish this, type

    # on Ubuntu command line
    ssh-keygen

on your Ubuntu. This gives you a file `~/.ssh/id_rsa.pub` on your Ubuntu,
which contains your public `ssh` key. You should append the contents of this `id_rsa.pub`
to the file `~/.ssh/authorized_keys` on your intermediary computer. From that moment on,
your intermediary computer will accept `rsync -e ssh` calls from your Ubuntu.
On the intermediary computer, create a folder `~/builds`, and a folder `chrome64` inside that.
If you now define

    # in Ubuntu:~/.bash_aliases
    praatc-put="rsync -tpvz ~/praatsc/praat yourname@fon.hum.uva.nl:~/builds/chrome64"
    praatc-mid="praatc-build && praatc-put"

you can build and send Praat for Chrome to the intermediary computer by just typing

    # on Ubuntu command line
    praatc-mid

On your Chromebook, start up Linux (see the Chromebook download page for details),
create a directory `~/praats` there, and define the following:

    # in Chromebook:~/.bash_aliases
    alias praat-get="( cd ~/praats &&\
        rsync -tpvz yourname@fon.hum.uva.nl:~/builds/chrome64/praat . )"
    alias praat="~/praats/praat"
    alias praat-run="praat-get && praat"

From then on, you can use

    # on Chromebook command line
    praat-run

to fetch Praat from the intermediary computer and run it.

The cycle from editing Praat on the Mac to running it on your Chromebook therefore takes only three steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praatc-mid` on your Ubuntu (under Parallels Desktop on your Mac);
3. type `praat-run` on your Chromebook.

For edits in a `cpp` file (no changes in header files), this whole cycle can be performed within 15 seconds.

### 4.5. Raspberry Pi development set-up

One could perhaps create the Raspberry Pi edition by cross-compiling on Ubuntu 20.04 or 22.04.
If any reader of these lines has precise instructions, we would like to know about it
(the main problem is how to install the GTK etc libraries in the Raspberry Pi toolchain,
or how to get `dpkg` under Ubuntu-buster to actually find `armhf` libraries).

Till then, you build on the Raspberry Pi itself. Your could do that via an intermediary computer
(analogously to what we described above for Chromebook), but you can also do it directly
if you include your Raspberry Pi in the same local network as your Mac and switch on SSH
on your Raspberry Pi (via Raspberry ->  `Preferences` -> `Raspberry Pi Configuration`
-> `Interfaces` -> `SSH` -> `Enable`. You add your Mac’s public SSH key to your Raspberry Pi with

    # on Mac command line
    ssh-keygen   # only if you have no SSH key yet
    ssh-copy-id pi@192.168.1.2   # or whatever your Pi’s static IP address is

On your Raspberry Pi, you create a folder `~/praats`,
after which you can push the sources from your Mac to your Raspberry Pi with

    # in Mac:~/.bash_profile
    ORIGINAL_SOURCES="~/Praats/src"
    EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praats-putpi="rsync -rptvz -e ssh $EXCLUDES \
        $ORIGINAL_SOURCES/ pi@192.168.1.2:~/praats"

On the Raspberry Pi, you define

    # in RaspberryPi:~/.bash_aliases
    alias praat-build="( cd ~/praats &&\
        cp makefiles/makefile.defs.linux.rpi makefile.defs &&\
        make -j4 )"
    alias praat="~/praats/praat"
    alias praat-run="praat-build && praat"

after which you can build and run Praat with

    # on Raspberry Pi command line
    praat-run

Thus, the cycle from editing Praat on the Mac to running it on your Raspberry Pi therefore takes three steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praats-putpi` on your Mac;
3. type `praat-run` on your Raspberry Pi, perhaps via `ssh -X pi@192.168.1.2` in your Mac terminal.

From clean sources this takes around 19 minutes (on a Raspberry Pi 4B),
but if no header files change, then it can be done in approximately 20 seconds.

### 4.6. s390x development set-up on LinuxONE

Once you have a (permanent) open-source LinuxONE account (https://community.ibm.com/zsystems/form/l1cc-oss-vm-request/),
you will probably have an SSH key generated in a `*.pem` file,
which you moved for instance to `~/Dropbox/Praats/ssh/mylinux1key.pem`.

On your LinuxONE virtual machine, you create folders `~/praats` and `~/praatsb`,
after which you can push the sources from your Mac to your LinuxONE VM with

    # in Mac:~/.bash_profile
    ORIGINAL_SOURCES="~/Praats/src"
    EXCLUDES='--exclude="*.xcodeproj" --exclude="Icon*" --exclude=".*" --exclude="*kanweg*"'
    alias praats-putone="rsync -rptvz -e \"ssh -i ~/Dropbox/Praats/ssh/mylinux1key.pem\" $EXCLUDES \
        $ORIGINAL_SOURCES/ linux1@199.199.99.99:~/praats"

where instead of `199.199.99.99` you use the IP address that the LinuxONE owners sent to you.
In your LinuxONE VM, you define

    # in LinuxONE:~/.bash_profile
    alias praat-build="( cd ~/praats &&\
        cp makefiles/makefile.defs.linux.s390.pulse makefile.defs &&\
        make -j4 )"
    alias praat="~/praats/praat"
    alias praat-run="praat-build && praat"
    alias praatb-build="( cd ~/praatsb &&\
        cp makefiles/makefile.defs.linux.s390.barren makefile.defs &&\
        make -j4 )"
    alias praatb="~/praatsb/praat_barren"
    alias praatb-run="praatb-build && praatb"

after which you can build and run Praat with

    # on LinuxONE command line
    praat-run

Thus, the cycle from editing Praat on the Mac to running it on your LinuxONE VM therefore takes three steps:

1. edit and save the source code in Xcode on your Mac;
2. type `praats-putone` on your Mac;
3. type `praat-run` on your LinuxONE VM,
   perhaps via `ssh -X -i ~/Dropbox/Praats/ssh/mylinux1key.pem linux1@199.199.99.99` in your Mac terminal.

### 4.7. Distributing Praat

If you want to distribute your version of Praat, you can do so on GitHub and/or on a website
(at least, that’s how the main authors do it). Both of these venues require that you have
all the executables in one place. The guide below refers to the creation of packages
for all platforms for Praat version 9.9.99, although your version number will be different.
The packages will be collected in the directory `~/Praats/www` on the Mac.

If you follow the location mentioned in the `.xcodeproj` file, the Mac binary will reside
in a place like `~/Dropbox/Praats/bin/Configuration64`.

After notarizing the Mac binary (see above under 3.2),
you include the executable in a `.dmg` disk image, with the following commands:

    # on Mac command line
    PRAAT_WWW="~/Dropbox/Praats/www"
    PRAAT_VERSION=9999
    cd ~/Dropbox/Praats/bin/macos/Configuration64
    hdiutil create -fs HFS+ -ov -srcfolder Praat.app -volname Praat_${PRAAT_VERSION} praat_${PRAAT_VERSION}.dmg
    hdiutil convert -ov -format UDZO -o ${PRAAT_WWW}/praat${PRAAT_VERSION}_mac.dmg praat_${PRAAT_VERSION}.dmg
    rm praat_${PRAAT_VERSION}.dmg

You also need to distribute the `.xcodeproj` file, which is actually a folder, so that you have to zip it:

    # on Mac command line
    ORIGINAL_SOURCES="~/Dropbox/Praats/src"
    cd $ORIGINAL_SOURCES
    zip -r $PRAAT_WWW/praat${PRAAT_VERSION}_xcodeproj.zip praat.xcodeproj

The Windows executables have to be sent from your Cygwin terminal or MSYS shell to your Mac.
It is easiest to do this without a version number (so that you have to supply the number only once),
so you send them to the intermediate Mac folders `~/Dropbox/Praats/bin/win-intel64`
and `~/Dropbox/Praats/bin/win-intel32` and `~/Dropbox/Praats/bin/win-arm64`.
On MSYS you can define:

    # in MSYS:~/.bashrc
    alias praat-dist="praat-build && rsync -t ~/praats/Praat.exe /z/Dropbox/Praats/bin/win-arm64"
    alias praat64-dist="praat64-build && rsync -t ~/praats64/Praat.exe /z/Dropbox/Praats/bin/win-intel64"
    alias praat32-dist="praat32-build && rsync -t ~/praats32/Praat.exe /z/Dropbox/Praats/bin/win-intel32"

so that you can “upload” the two executables to the Mac with

    # on MSYS command line
    praat-dist
    praat64-dist
    praat32-dist

The four Linux executables have to be sent from your Ubuntu terminal to your Mac,
namely to the folder `~/Dropbox/Praats/bin/linux_intel64` or `~/Dropbox/Praats/bin/linux_arm64`
(each of which will contain `praat` and `praat_barren`), and to the folder
`~/Dropbox/Praats/bin/chrome_intel64` or `~/Dropbox/Praats/bin/chrome_arm64`
(which will contain only `praat`).
On Ubuntu you can define

    # in MSYS2 Intel64/AMD64 Ubuntu:~/.bash_aliases
    alias praat-dist="praat-build && rsync -t ~/praats/praat /Users/yourname/Dropbox/Praats/bin/linux-intel64"
    alias praatb-dist="praatb-build && rsync -t ~/praatsb/praat_barren /Users/yourname/Dropbox/Praats/bin/linux-intel64"
    alias praatc-dist="praatc-build && rsync -t ~/praatsc/praat /Users/yourname/Dropbox/Praats/bin/chrome-intel64"

    # in MSYS2 ARM64 Ubuntu:~/.bash_aliases
    alias praat-dist="praat-build && rsync -t ~/praats/praat /Users/yourname/Dropbox/Praats/bin/linux-arm64"
    alias praatb-dist="praatb-build && rsync -t ~/praatsb/praat_barren /Users/yourname/Dropbox/Praats/bin/linux-arm64"
    alias praatc-dist="praatc-build && rsync -t ~/praatsc/praat /Users/yourname/Dropbox/Praats/bin/chrome-arm64"

so that you can “upload” the four executables to the Mac with

    # on Ubuntu command line
    praat-dist
    praatb-dist
    praatc-dist

You can fetch the Raspberry Pi edition directly from your Raspberry Pi:

    # on Mac command line
    rsync -tpvz -e ssh pi@192.168.1.2:~/praats/praat ~/Dropbox/Praats/bin/rpi-armv7

and the s390x edition directly from your LinuxONE account:

    # on Mac command line
    rsync -tpvz -e "ssh -i ~/Dropbox/Praats/ssh/mylinux1key.pem" linux1@199.199.99.99:~/praats/praat ~/Dropbox/Praats/bin/linux-s390x
    rsync -tpvz -e "ssh -i ~/Dropbox/Praats/ssh/mylinux1key.pem" linux1@199.199.99.99:~/praatsb/praat_barren ~/Dropbox/Praats/bin/linux-s390x

When the folders under `~/Dropbox/Praats/bin`, namely `win-intel64`, `win-intel32`, `win-arm64`,
`linux-intel64`, `linux-arm64`, `chrome-intel64`, `chrome-arm64` and `rpi-armv7`
all contain enough new executables (there should be 1, 1, 1, 3, 3, 1, 1 and 1, respectively),
you can issue the following commands to create the packages and install them in `~/Dropbox/Praats/www`:

    # on Mac command line
    zip $PRAAT_WWW/praat${PRAAT_VERSION}_win-intel64.zip ~/Dropbox/Praats/bin/win-intel64/Praat.exe
    zip $PRAAT_WWW/praat${PRAAT_VERSION}_win-intel32.zip ~/Dropbox/Praats/bin/win-intel32/Praat.exe
    zip $PRAAT_WWW/praat${PRAAT_VERSION}_win-arm64.zip ~/Dropbox/Praats/bin/win-arm64/Praat.exe
    ( cd ~/Dropbox/Praats/bin/linux-intel64 &&\
      tar cvf praat${PRAAT_VERSION}_linux-intel64.tar praat &&\
      gzip praat${PRAAT_VERSION}_linux-intel64.tar &&\
      mv praat${PRAAT_VERSION}_linux-intel64.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/linux-intel64 &&\
      tar cvf praat${PRAAT_VERSION}_linux-intel64-barren.tar praat_barren &&\
      gzip praat${PRAAT_VERSION}_linux-intel64-barren.tar &&\
      mv praat${PRAAT_VERSION}_linux-intel64-barren.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/chrome-intel64 &&\
      tar cvf praat${PRAAT_VERSION}_chrome-intel64.tar praat &&\
      gzip praat${PRAAT_VERSION}_chrome-intel64.tar &&\
      mv praat${PRAAT_VERSION}_chrome-intel64.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/linux-arm64 &&\
      tar cvf praat${PRAAT_VERSION}_linux-arm64.tar praat &&\
      gzip praat${PRAAT_VERSION}_linux-arm64.tar &&\
      mv praat${PRAAT_VERSION}_linux-arm64.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/linux-arm64 &&\
      tar cvf praat${PRAAT_VERSION}_linux-arm64-barren.tar praat_barren &&\
      gzip praat${PRAAT_VERSION}_linux-arm64-barren.tar &&\
      mv praat${PRAAT_VERSION}_linux-arm64-barren.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/chrome-arm64 &&\
      tar cvf praat${PRAAT_VERSION}_chrome-arm64.tar praat &&\
      gzip praat${PRAAT_VERSION}_chrome-arm64.tar &&\
      mv praat${PRAAT_VERSION}_chrome-arm64.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/rpi-armv7 &&\
      tar cvf praat${PRAAT_VERSION}_rpi-armv7.tar praat &&\
      gzip praat${PRAAT_VERSION}_rpi-armv7.tar &&\
      mv praat${PRAAT_VERSION}_rpi-armv7.tar.gz $PRAAT_WWW )
    ( cd ~/Dropbox/Praats/bin/linux-s390x &&\
      tar cvf praat${PRAAT_VERSION}_linux-s390x.tar praat &&\
      gzip praat${PRAAT_VERSION}_linux-s390x.tar &&\
      mv praat${PRAAT_VERSION}_linux-s390x.tar.gz $PRAAT_WWW )

Finally, you can update your website and/or create a new release on GitHub.
