# meson.build of the library "foned"
# David Weenink, 18 August 2026

sources = '''
	SoundRecorder.cpp
   FunctionArea.cpp SoundArea.cpp SoundAnalysisArea.cpp TextGridArea.cpp
   SpectrumArea.cpp PointArea.cpp PitchArea.cpp SpectrogramArea.cpp
   RealTierArea.cpp PitchTierArea.cpp IntensityTierArea.cpp DurationTierArea.cpp AmplitudeTierArea.cpp
   FormantGridArea.cpp MovieArea.cpp
   FunctionEditor.cpp
   PitchEditor.cpp SoundEditor.cpp SpectrumEditor.cpp SpectrogramEditor.cpp PointEditor.cpp
   RealTierEditor.cpp PitchTierEditor.cpp IntensityTierEditor.cpp
   DurationTierEditor.cpp AmplitudeTierEditor.cpp
   ManipulationEditor.cpp TextGridEditor.cpp FormantGridEditor.cpp
   RunnerMFC.cpp MovieWindow.cpp CorpusEditor.cpp'''.split()

libfoned = static_library (
	'foned',
	sources : sources,
	dependencies : gtk_dep,
	include_directories : [dwsys_inc, dwtools_inc, espeak_inc, flac_inc, mp3_inc, portaudio_inc, fon_inc, kar_inc, LPC_inc, melder_inc, stat_inc, sys_inc]
)

libfoned_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libfoned
)
