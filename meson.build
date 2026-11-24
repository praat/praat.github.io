# build system for praat
# David Weenink and Paul Boersma, 21 November 2026

project(
  'praat',
  'cpp',
  'c',
  default_options : ['cpp_std=c++17', 'c_std=c99']
)

system_libs = []
compiler_arguments_c = []
compiler_arguments_cpp = []

os_name = build_machine.system()

if os_name == 'linux'

	common_compiler_arguments = '''-DUNIX -Dlinux -DALSA -DHAVE_PULSEAUDIO -D_FILE_OFFSET_BITS=64 -Wreturn-type 
		-DWORDS_BIGENDIAN=0 -DPA_LITTLE_ENDIAN 
		-Wunused -Wunused-parameter -Wuninitialized'''.split()
	compiler_arguments_cpp = common_compiler_arguments
	compiler_arguments_c = ['-std=gnu99'] + common_compiler_arguments + ['-Wshadow']
	system_libs = '-no-pie -lpulse -lasound -lpthread -L/usr/lib/x86_64-linux-gnu'.split()
	
elif os_name == 'freebsd'
	
elif os_name == 'darwin' # either OSX or iOS

elif os_name == 'cygwin'

elif os_name == 'android' # chrome os

	common_compiler_arguments = '''-DUNIX -Dlinux -Dchrome -DALSA -DHAVE_PULSEAUDIO -D_FILE_OFFSET_BITS=64
		-Wreturn-type -Wunusedninja: error: '/usr/lib/x86_64-linux-gnu/libm.a', needed by 'main/praat', missing and no known rule to make  -Wunused-parameter -Wuninitialized'''
	compiler_arguments_cpp = ['cpp_std=g++'] + common_compiler_arguments + ['-Wshadow']
	compiler_arguments_c = ['-std=gnu99'] + common_compiler_arguments + ['-Werror=missing-prototypes -Werror=implicit']
	system_libs = '-no-pie -lm -lpulse -lasound -static-libgcc -static-libstdc++ -L/usr/lib/x86_64-linux-gnu'.split()
else
	message ('Unknown os_name ================ ', os_name)
endif

dwsys_inc = include_directories ('dwsys')
dwtools_inc = include_directories ('dwtools')
EEG_inc = include_directories ('EEG')
fon_inc = include_directories ('fon')
foned_inc = include_directories ('foned')
gram_inc = include_directories ('gram')
kar_inc = include_directories ('kar')
LPC_inc = include_directories ('LPC')
melder_inc = include_directories ('melder')
sensors_inc = include_directories ('sensors')
stat_inc = include_directories ('stat')
sys_inc = include_directories ('sys')

clapack_inc = include_directories ('external/clapack')
espeak_inc = include_directories ('external/espeak')
expat_inc = include_directories ('external/expat')
flac_inc = include_directories ('external/flac')
gsl_inc = include_directories ('external/gsl')
mp3_inc = include_directories ('external/mp3')
num_inc = include_directories ('external/num')
opusfile_inc = include_directories ('external/opusfile')
portaudio_inc = include_directories ('external/portaudio')
vorbis_inc = include_directories ('external/vorbis')
whisper_inc = include_directories ('external/whispercpp')
blake3_inc = include_directories ('external/blake3')
xlsxio_inc = include_directories ('external/xlsxio')
zlib_inc = include_directories ('external/zlib')

threads_dep = dependency('threads')

gtk_dep = dependency('gtk+-3.0')

add_project_arguments (compiler_arguments_c, language: 'c')
add_project_arguments (compiler_arguments_cpp, language: 'cpp')

praat_dirs = ['artsynth', 'dwsys', 'dwtools', 'EEG', 'FFNet', 'fon', 'foned', 'gram', 'kar', 'LPC', 'melder', 'sensors', 'stat', 'sys']
praat_external_dirs = ['clapack', 'espeak', 'expat', 'flac', 'gsl', 'glpk', 'lame', 'num', 'mp3', 'opusfile', 'portaudio', 'vorbis', 'whispercpp', 'blake3', 'xlsxio', 'zlib']

foreach dir : praat_dirs
	subdir (dir)
endforeach
foreach dir : praat_external_dirs
	subdir ('external' / dir)
endforeach

praat_libs_dep = [libartsynth_dep, libdwsys_dep, libdwtools_dep, libEEG_dep, libFFNet_dep, libfon_dep, libfoned_dep, libgram_dep, libkar_dep, libLPC_dep, libmelder_dep, libsensors_dep, libstat_dep, libsys_dep]
praat_external_libs_dep = [libclapack_dep, libblake3_dep, libespeak_dep, libflac_dep, libgsl_dep, libglpk_dep, liblame_dep, libmp3_dep, libnum_dep, libopusfile_dep, libportaudio_dep, libvorbis_dep, libwhisper_dep, libxlsxio_dep, libexpat_dep, libzlib_dep]

subdir ('main')
