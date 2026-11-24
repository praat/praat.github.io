# meson.build for library "expat".
# David Weenink 21 November 2026

sources_expat = '''
	random_arc4random.c random_arc4random_buf.c random_dev_urandom.c random_getentropy.c
	random_getrandom.c random_rand_s.c xcs.c xmlparse.c xmlrole.c xmltok.c'''.split()
	
sources_minizip = '''unzip.c ioapi.c '''.split()

sources = sources_expat + sources_minizip

# random_rand_s.c
libexpat = static_library (
	'expat',
	sources : sources,
	c_args: ['-DNOUNCRYPT'],
	include_directories : [zlib_inc, sys_inc, melder_inc, kar_inc]
)

libexpat_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libexpat
)


