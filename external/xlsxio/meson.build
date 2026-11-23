# meson.build for library "xlsxio".
# David Weenink 21 November 2026

sources = '''
	xlsxio_read.c xlsxio_read_sharedstrings.c'''.split()

libxlsxio = static_library (
	'xlsxio',
	sources : sources,
	c_args : ['-DUSE_MINIZIP'],
	include_directories : [expat_inc, sys_inc, melder_inc, kar_inc]
)

libxlsxio_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libxlsxio
)


