# File: Makefile

# Makefile for Praat
# Paul Boersma, 19 July 2026

##########################
#
# To make Praat from the command line on Windows or Linux, type
#
# make
#   will make Praat with default settings
#
# There are several settings that you can either add on the command line
# or define as an environment variable.
#
##########################
#
# PRAAT_OS
#
# determines the OS and sub-OS for which Praat will be built.
# The default is to have this Makefile audo-detect the OS and sub-OS;
# this tends to work for:
#   - OS: Windows
#   - OS: Linux
#   - Sub-OS: Raspberry Pi (implying that Linux is the OS)
# but not (yet) for:
#   - Sub-OS: Chrome (only Linux will be detected as the OS)
#   - OS: FreeBSD
#
# make PRAAT_OS=windows   # will set Windows as the OS
#
# make PRAAT_OS=linux   # will set Linux as the OS, and no sub-OS
#
# make PRAAT_OS=chrome   # will set Linux as the OS, and Chrome as the sub-OS
#
# make PRAAT_OS=linuxone   # will set Linux as the OS, and LinuxONE as the sub-OS
#
# make PRAAT_OS=raspberrypi   # will set Linux as the OS, and Raspberry Pi as the sub-OS
#
# make PRAAT_OS=freebsd   # will set FreeBSD as the OS (note: this doesn't count as Linux)
#
##########################
#
# PRAAT_ARCH
#
# determines the processor architecture for which Praat will be built.
# The default is what GCC and Clang will use if an architecture is not specified, which is:
#   - armv8-a (simplest ARM64) for the MSYS2-CLANGARM64 shell (Windows) and the OrbStack ARM64 shell (Linux)
#   - x86-64 (i.e. x64-v1) for the MSYS2-CLANG64 shell (Windows) and the OrbStack x64 shell (Linux)
#   - pentium4 (i.e. basic i686) for the MSYS2-MINGW32 shell (Windows)
#   - armv6+fp for some Raspberry Pi computers (you can check this using: gcc -Q --help=target)
# Note that x64-v1 is the original 64-bit AMD/Intel chip;
# for post-2013 computers you'd opt for x64-v3 instead.
#
# make PRAAT_ARCH=arm64
#   will make Praat with the "armv8-a" setting for the architecture,
#   producing an ARM64 edition of Praat.
#
# make PRAAT_ARCH=x64v1
#   will make Praat with the "x86-64" setting for the architecture,
#   producing an x64-v1 edition of Praat.
#
# make PRAAT_ARCH=x64v3
#   will make Praat with the "x86-64-v3" setting for the architecture,
#   i.e. with support for AVX2, FMA, F16C and more,
#   producing an x64-v3 edition of Praat.
#
# make PRAAT_ARCH=i686
#   will make Praat with the "i686" setting for the architecture,
#   producing a 32-bits Windows edition of Praat.
#
# make PRAAT_ARCH=native
#   will make Praat for the architecture of the shell,
#   which will often produce a version of Praat that is fastest on your computer.
#
# Tips and tricks
#
#   To make PRAAT_ARCH=native the default, you can add an environment variable, e.g. you can put
#     export PRAAT_ARCH=native
#   in your ~/.bashrc file (or so). Once you have done that,
#     make
#   will from then on behave the same as if you typed
#     make PRAAT_ARCH=native
#   Note that an explicit command-line-specified setting, such as
#     make PRAAT_ARCH=x64v1
#   will override the environment variable.
#
##########################

# First try: explicit setting of the OS via argument or environment variable PRAAT_OS.
ifeq ($(PRAAT_OS),windows)
  OS_IS_WINDOWS := 1
  $(info OS given as Windows)
else ifeq ($(PRAAT_OS),freebsd)
  OS_IS_FREEBSD := 1
  $(info OS given as FreeBSD)
else ifeq ($(PRAAT_OS),linux)
  OS_IS_LINUX := 1
  $(info OS given as Linux)
else ifeq ($(PRAAT_OS),raspberrypi)
  OS_IS_LINUX := 1
  SUBOS_IS_RASPBERRY_PI := 1
  $(info OS given as Linux (sub-OS Raspberry Pi))
else ifeq ($(PRAAT_OS),chrome)
  OS_IS_LINUX := 1
  SUBOS_IS_CHROME := 1
  $(info OS given as Linux (sub-OS Chrome))
else ifeq ($(PRAAT_OS),linuxone)
  OS_IS_LINUX := 1
  SUBOS_IS_LINUX_ONE := 1
  $(info OS given as Linux (sub-OS LinuxONE))
# Second try: default setting, depending on heuristics.
else ifeq ($(OS),Windows_NT)
  OS_IS_WINDOWS := 1
  $(info OS detected as Windows)
else
  TRY_OS_NAME := $(shell uname -s)
  ifeq ($(TRY_OS_NAME),Linux)
    OS_IS_LINUX := 1
    TRY_RASPBERRY_PI_MODEL := $(shell cat /proc/device-tree/model 2>/dev/null)
    ifeq ($(findstring Raspberry Pi,$(TRY_RASPBERRY_PI_MODEL)),Raspberry Pi)
      SUBOS_IS_RASPBERRY_PI := 1
      $(info OS detected as Linux (sub-OS Raspberry Pi))
    else
      $(info OS detected as Linux (but not Raspberry Pi))
    endif
  endif
endif

ifeq ($(PRAAT_ARCH),arm64)
  ARCH_COMPILER_FLAGS = -march=armv8-a
else ifeq ($(PRAAT_ARCH),x64v1)
  ARCH_COMPILER_FLAGS = -march=x86-64
else ifeq ($(PRAAT_ARCH),x64v3)
  ARCH_COMPILER_FLAGS = -march=x86-64-v3
else ifeq ($(PRAAT_ARCH),i686)
  ARCH_COMPILER_FLAGS = -march=i686
else ifeq ($(PRAAT_ARCH),s390x)
  ARCH_COMPILER_FLAGS = -march=arch11 -mtune=arch13
else ifeq ($(PRAAT_ARCH),native)
  ARCH_COMPILER_FLAGS = -march=native
else
  $(info No architecture given)
  ARCH_COMPILER_FLAGS =
endif
$(info ARCHITECTURE: given as "$(PRAAT_ARCH)", resolved as "$(ARCH_COMPILER_FLAGS)")

ifeq ($(OS_IS_FREEBSD),1)
  # by Paul Boersma, Jason Bacon, Adriaan de Groot

  # Default setting for where the executable will be installed,
  # overridable by an environment variable (or on the `make` command line).
  # Also needs to be added to the include and linker paths.
  # (In the FreeBSD ports system, this is already set so nothing is overridden.)
  LOCALBASE ?= /usr/local

  PKG_CONFIG ?= pkg-config

  # FreeBSD defaults to clang, not gcc
  CC ?= cc
  CXX ?= c++
  LINK ?= $(CXX)

  SYSTEM_COMPILER_FLAGS = -DUNIX -Dlinux

  GRAPHICS_COMPILER_FLAGS := `$(PKG_CONFIG) --cflags gtk+-3.0`
  GRAPHICS_LINKER_FLAGS := `$(PKG_CONFIG) --libs gtk+-3.0`

  # -DALSA and -DJACK: Use ALSA and Jack audio in pa_unix_hostapis.c
  AUDIO_COMPILER_FLAGS := -DALSA -DJACK -DHAVE_SYS_SOUNDCARD_H
  AUDIO_LINKER_FLAGS := -lasound -ljack

  # FreeBSD pretends to be Linux for most of the code; add CPPFLAGS
  # explicitly because GNU make / gcc uses those preprocessor flags,
  # but clang does not.
  SHARED_COMPILER_FLAGS := $(SYSTEM_COMPILER_FLAGS) -D_FILE_OFFSET_BITS=64 \
    $(GRAPHICS_COMPILER_FLAGS) $(AUDIO_COMPILER_FLAGS) \
    -Wreturn-type -Wunused -Wunused-parameter -Wuninitialized -O1 -g1 -pthread $(CPPFLAGS)

  CFLAGS := -std=gnu99 $(SHARED_COMPILER_FLAGS) -Werror=implicit
    # Note: why not also -Werror=missing-prototypes ?
  CXXFLAGS := -std=c++17 $(SHARED_COMPILER_FLAGS) -Wshadow -Werror=return-type \
    -I$(LOCALBASE)/include -I$(LOCALBASE)/include/unicode
    # Note: can we add -Werror=return-type to other OSes as well?

  EXECUTABLE_FILE := praat

  NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -L$(LOCALBASE)/lib $(AUDIO_LINKER_FLAGS) -lm -lpthread -ltinfow

  AR = ar
  RANLIB = ls
  RM = rm -f
  ICON =
  MAIN_ICON =

else ifeq ($(OS_IS_WINDOWS),1)
  # by Paul Boersma

  #
  # Determine the compiler: either GCC or Clang.
  #
  # First try: explicit setting of the compiler via argument or environment variable PRAAT_COMPILER.
  ifeq ($(PRAAT_COMPILER),gcc)
    WE_HAVE_GCC := 1
  else ifeq ($(PRAAT_COMPILER),clang)
    WE_HAVE_GCC := 0
  # Second try: default setting, depending on the name of the shell.
  else ifeq ($(findstring MINGW,$(MSYSTEM)),MINGW)
    WE_HAVE_GCC := 1
  else ifeq ($(findstring CLANG,$(MSYSTEM)),CLANG)
    WE_HAVE_GCC := 0
  else
    $(error Unknown compiler)
  endif

  #
  # Compiler-specific variables:
  # - CC is a standard variable for the command that will be used by implicit `make` rules to compile C files
  # - CXX is a standard variable for the command that will be used by implicit `make` rules to compile C++ files
  # - LINKER_COMMAND is our command to invoke the linker; it should match the compiler command
  # - NON_PRAAT_LIBRARIES is our listing of the Windows libraries and the GCC or Clang libraries
  #
  ifeq ($(WE_HAVE_GCC),1)
    CC := gcc
    CXX := g++
    LINKER_COMMAND := g++
    NON_PRAAT_LIBRARIES = -lwinmm -lwsock32 -lcomctl32 -lole32 -lgdi32 -lgdiplus -lcomdlg32 -lwinspool \
      -static-libgcc -static-libstdc++ -mwindows -static -lwinpthread
      # Note: winpthread is linked statically, because it may not be available on all users' computers;
      # for the same reason, GCC's C and C++ libraries are also statically linked.
  else
    CC := clang
    CXX := clang
    LINKER_COMMAND := clang
    NON_PRAAT_LIBRARIES = -lwinmm -lwsock32 -lcomctl32 -lole32 -lgdi32 -lgdiplus -lcomdlg32 -lwinspool \
      -static -lc++ -lc++abi -mwindows
      # Note: Clang's C++ libraries are not available on all users' computers, so they are linked statically.
  endif

  #
  # Compiler flags:
  # - CFLAGS is a standard variable for the settings that will be used by implicit `make` rules to compile C files
  # - CXXFLAGS is a standard variable for the settings that will be used by implicit `make` rules to compile C++ files
  # - SHARED_COMPILER_FLAGS is our variable for the settings that will be shared between C and C++ compilation
  #
  SHARED_COMPILER_FLAGS := -municode -D_FILE_OFFSET_BITS=64 \
    $(ARCH_COMPILER_FLAGS) -O3
    # Note: the leaves many settings implicit, such as -mwin32 and -mtune=generic;
    # also -m64 (for x64-v1, x64-v3 and arm64) and -march=xxx (if not set in ARCH_COMPILER_FLAGS).
  CFLAGS := -std=gnu99 $(SHARED_COMPILER_FLAGS)
  CXXFLAGS := -std=gnu++17 $(SHARED_COMPILER_FLAGS) -Wshadow
    # Note: gnu++17 instead of c++17 is necessary to define M_PI in external code.

  EXECUTABLE_FILE = Praat.exe

  AR = ar
  RANLIB = ranlib
  WINDRES = windres
  ICON = praat_win.o
  MAIN_ICON = main/praat_win.o

else ifeq ($(OS_IS_LINUX),1)
  # by Paul Boersma, David Weenink, Anna Simmons

  # Default settings for where the executable will be installed,
  # overridable by environment variables (or on the `make` command line).
  DESTDIR ?=
  PREFIX ?= /usr/local
  BINDIR ?= $(DESTDIR)$(PREFIX)/bin
  DATADIR ?= $(DESTDIR)$(PREFIX)/share

  PKG_CONFIG ?= pkg-config

  ifeq ($(PRAAT_GRAPHICS),barren)
    GRAPHICS_COMPILER_FLAGS := -DNO_GRAPHICS
    GRAPHICS_LINKER_FLAGS :=
    EXECUTABLE_FILE = praat_barren
  else ifeq ($(PRAAT_GRAPHICS),nogui)
    GRAPHICS_COMPILER_FLAGS := -DNO_GUI `$(PKG_CONFIG) --cflags pangocairo`
    GRAPHICS_LINKER_FLAGS := `$(PKG_CONFIG) --libs pangocairo`
    EXECUTABLE_FILE = praat_nogui
  else
    GRAPHICS_COMPILER_FLAGS := `$(PKG_CONFIG) --cflags gtk+-3.0`
    GRAPHICS_LINKER_FLAGS := `$(PKG_CONFIG) --libs gtk+-3.0`
    EXECUTABLE_FILE = praat
  endif

  ifeq ($(PRAAT_AUDIO),none)
    AUDIO_COMPILER_FLAGS :=
    AUDIO_LINKER_FLAGS :=
  else ifeq ($(PRAAT_AUDIO),jack)
    AUDIO_COMPILER_FLAGS := -DJACK
    AUDIO_LINKER_FLAGS := `$(PKG_CONFIG) --libs jack`
  else ifeq ($(PRAAT_AUDIO),alsa)
    AUDIO_COMPILER_FLAGS := -DALSA
    AUDIO_LINKER_FLAGS := -lasound
  else ifeq ($(SUBOS_IS_RASPBERRY_PI),1)
    AUDIO_COMPILER_FLAGS := -DJACK
    AUDIO_LINKER_FLAGS := -ljack
  else
    AUDIO_COMPILER_FLAGS := -DALSA -DJACK -DHAVE_PULSEAUDIO
    AUDIO_LINKER_FLAGS := `$(PKG_CONFIG) --libs jack` -lpulse -lasound
  endif

  ifeq ($(SUBOS_IS_RASPBERRY_PI),1)
    SYSTEM_COMPILER_FLAGS = -DUNIX -Dlinux -Draspberrypi
  else
    SYSTEM_COMPILER_FLAGS = -DUNIX -Dlinux
  endif

  SHARED_COMPILER_FLAGS := $(SYSTEM_COMPILER_FLAGS) -D_FILE_OFFSET_BITS=64 \
    $(ARCH_COMPILER_FLAGS) \
    $(GRAPHICS_COMPILER_FLAGS) $(AUDIO_COMPILER_FLAGS) \
    -Wreturn-type -Wunused -Wunused-parameter -Wuninitialized -O3 -g1 -pthread
  ifeq ($(SUBOS_IS_CHROME),1)
    SHARED_COMPILER_FLAGS += -Dchrome
  endif

  CFLAGS += -std=gnu99 $(SHARED_COMPILER_FLAGS) -Werror=missing-prototypes -Werror=implicit
  CXXFLAGS += -std=c++17 $(SHARED_COMPILER_FLAGS) -Wshadow

  ifeq ($(PRAAT_COMPILER),clang)
    CC := clang
    CXX := clang
    LINKER_COMMAND := clang
    CXXFLAGS += -stdlib=libc++
    NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -no-pie -lc++ -lm $(AUDIO_LINKER_FLAGS) -lpthread
  else
    CC := gcc
    CXX := g++
    LINKER_COMMAND := g++
    ifeq ($(SUBOS_IS_RASPBERRY_PI),1)
      # The static library /usr/lib/gcc/arm-linux-gnueabihf/8/libstdc++fs.a is needed with GCC 8,
      # and can be removed when using GCC 9
      NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -no-pie -lm $(AUDIO_LINKER_FLAGS) \
        -static-libgcc -static-libstdc++ -lpthread -latomic -ldl -lstdc++fs
    else ifeq ($(SUBOS_IS_CHROME),1)
      NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -no-pie -lm $(AUDIO_LINKER_FLAGS) \
        -static-libgcc -static-libstdc++ -lpthread -L /usr/lib/x86_64-linux-gnu
    else ifeq ($(PRAAT_GRAPHICS),barrenXXX)
      NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -no-pie -lm $(AUDIO_LINKER_FLAGS) \
        -static-libgcc -static-libstdc++ -lpthread -L /usr/lib/x86_64-linux-gnu
    else ifeq ($(PRAAT_GRAPHICS),noguiXXX)
      NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -no-pie -lm $(AUDIO_LINKER_FLAGS) \
        -static-libgcc -static-libstdc++ -lpthread
    else
      NON_PRAAT_LIBRARIES := $(GRAPHICS_LINKER_FLAGS) -no-pie -lm $(AUDIO_LINKER_FLAGS) -lpthread
    endif
  endif

  AR = ar
  RANLIB = ls
  ICON =
  MAIN_ICON =

  INSTALL ?= install
ifeq ($(OS_IS_LINUX),1)
  BINARIES = $(shell find . -maxdepth 1 -name praat -o -name praat_nogui -o -name praat_barren)
  INSTALL_BIN = $(INSTALL) -pDm0755 $(BINARIES) -t $(BINDIR)
  INSTALL_BASHCOMPLETION = $(INSTALL) -Dm0644 main/praat.bash-completion $(DATADIR)/bash-completion/completions/praat
else
  INSTALL_BIN = $(INSTALL) -pDm0755 praat -t $(BINDIR)
  INSTALL_BASHCOMPLETION =
endif
  INSTALL_METAINFO = $(INSTALL) -Dm0644 org.praat.Praat.metainfo.xml -t $(DATADIR)/metainfo
  INSTALL_DESKTOP = $(INSTALL) -Dm0644 main/praat.desktop $(DATADIR)/applications/org.praat.Praat.desktop
  INSTALL_ICONS = \
	$(INSTALL) -Dm0644 main/praat-480.svg $(DATADIR)/icons/hicolor/scalable/apps/org.praat.Praat.svg && \
	$(INSTALL) -Dm0644 main/praat-16.png $(DATADIR)/icons/hicolor/16x16/apps/org.praat.Praat.png && \
	$(INSTALL) -Dm0644 main/praat-32.png $(DATADIR)/icons/hicolor/32x32/apps/org.praat.Praat.png && \
	$(INSTALL) -Dm0644 main/praat-48.png $(DATADIR)/icons/hicolor/48x48/apps/org.praat.Praat.png && \
	$(INSTALL) -Dm0644 main/praat-128.png $(DATADIR)/icons/hicolor/128x128/apps/org.praat.Praat.png && \
	$(INSTALL) -Dm0644 main/praat-256.png $(DATADIR)/icons/hicolor/256x256/apps/org.praat.Praat.png && \
	$(INSTALL) -Dm0644 main/praat-512.png $(DATADIR)/icons/hicolor/512x512/apps/org.praat.Praat.png
endif

# Use system's or embedded zlib library
USE_SYS_ZLIB ?= no
ifeq ($(USE_SYS_ZLIB),no)
ZLIB = external/zlib/libzlib.a
else
ZLIB = -lz
endif

# Export some variables to the makefiles in the subdirectories.
export CPPFLAGS   # listing of include files (plus any flags provided by any environment variables also called CPPFLAGS)
export CC
export CXX
export CFLAGS
export CXXFLAGS
export AR         # for creating the archive of object files in a subfolder
export RANLIB     # for creating the archive of object files in a subfolder
export RM         # for `make clean`
export WINDRES    # for compiling `main/praat_win.rc`

.PHONY: all clean install

# Makes the Praat executable in the source directory.
all: all-external all-self
	$(LINKER_COMMAND) -o $(EXECUTABLE_FILE) main/main_Praat.o $(MAIN_ICON) fon/libfon.a \
		artsynth/libartsynth.a FFNet/libFFNet.a \
		gram/libgram.a EEG/libEEG.a \
		LPC/libLPC.a dwtools/libdwtools.a sensors/libsensors.a \
		foned/libfoned.a fon/libfon.a stat/libstat.a \
		dwsys/libdwsys.a sys/libsys.a melder/libmelder.a kar/libkar.a \
		external/espeak/libespeak.a \
		external/portaudio/libportaudio.a \
		external/flac/libflac.a external/lame/liblame.a external/mp3/libmp3.a \
		external/glpk/libglpk.a \
		external/clapack/libclapack.a \
		external/gsl/libgsl.a \
		external/num/libnum.a \
		external/vorbis/libvorbis.a \
		external/opusfile/libopusfile.a \
		external/whispercpp/libwhisper.a \
		external/blake3/libblake3.a \
		$(ZLIB) \
               $(NON_PRAAT_LIBRARIES) $(LDFLAGS)

all-external:
	$(MAKE) -C external/clapack
	$(MAKE) -C external/gsl
	$(MAKE) -C external/glpk
	$(MAKE) -C external/lame
	$(MAKE) -C external/mp3
	$(MAKE) -C external/num
	$(MAKE) -C external/flac
	$(MAKE) -C external/portaudio
	$(MAKE) -C external/espeak
	$(MAKE) -C external/vorbis
	$(MAKE) -C external/opusfile
	$(MAKE) -C external/whispercpp
	$(MAKE) -C external/blake3
ifeq ($(USE_SYS_ZLIB),no)
	$(MAKE) -C external/zlib
endif

all-self:
	$(MAKE) -C kar
	$(MAKE) -C melder
	$(MAKE) -C sys
	$(MAKE) -C dwsys
	$(MAKE) -C stat
	$(MAKE) -C fon
	$(MAKE) -C foned
	$(MAKE) -C dwtools
	$(MAKE) -C LPC
	$(MAKE) -C EEG
	$(MAKE) -C sensors
	$(MAKE) -C gram
	$(MAKE) -C FFNet
	$(MAKE) -C artsynth
	$(MAKE) -C main main_Praat.o $(ICON)

clean: clean-external clean-self clean-test
	$(RM) praat

clean-external:
	$(MAKE) -C external/clapack clean
	$(MAKE) -C external/gsl clean
	$(MAKE) -C external/glpk clean
	$(MAKE) -C external/lame clean
	$(MAKE) -C external/mp3 clean
	$(MAKE) -C external/num clean
	$(MAKE) -C external/flac clean
	$(MAKE) -C external/portaudio clean
	$(MAKE) -C external/espeak clean
	$(MAKE) -C external/vorbis clean
	$(MAKE) -C external/opusfile clean
	$(MAKE) -C external/whispercpp clean
	$(MAKE) -C external/blake3 clean
ifeq ($(USE_SYS_ZLIB),no)
	$(MAKE) -C external/zlib clean
endif

clean-self:
	$(MAKE) -C kar clean
	$(MAKE) -C melder clean
	$(MAKE) -C sys clean
	$(MAKE) -C dwsys clean
	$(MAKE) -C stat clean
	$(MAKE) -C fon clean
	$(MAKE) -C foned clean
	$(MAKE) -C dwtools clean
	$(MAKE) -C LPC clean
	$(MAKE) -C EEG clean
	$(MAKE) -C sensors clean
	$(MAKE) -C gram clean
	$(MAKE) -C FFNet clean
	$(MAKE) -C artsynth clean
	$(MAKE) -C main clean

clean-test:
	$(MAKE) -C dwtest clean
	$(MAKE) -C test/dwtools clean
	$(MAKE) -C test/fon clean
	$(MAKE) -C test/kar clean
	$(MAKE) -C test/script clean
	$(MAKE) -C test/stat clean
	$(MAKE) -C test/sys clean

install:
	$(INSTALL_BIN)
	$(INSTALL_METAINFO)
	$(INSTALL_DESKTOP)
	$(INSTALL_ICONS)
	$(INSTALL_BASHCOMPLETION)
