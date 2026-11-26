#ifndef EXPAT_CONFIG_H
#define EXPAT_CONFIG_H 1
/* expat_config.h
 *
 * Copyright (C) 2026 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
	Hand-written configuration for Praat's copy of expat (used only for reading xlsx files).
	Replaces the file that expat's configure/CMake would generate, so that the build
	does not depend on whatever expat_config.h happens to be installed on the build machine
	(Ubuntu's libexpat1-dev installs /usr/include/x86_64-linux-gnu/expat_config.h;
	MSYS2's mingw-w64-*-expat installs one too; Xcode's SDK has none).
*/

/* Byte order: 1234 = little-endian, 4321 = big-endian (e.g. LinuxONE / s390x). */
#if defined (__BYTE_ORDER__) && defined (__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define BYTEORDER  4321
#else
	#define BYTEORDER  1234
#endif

/*
	Features: xlsx parts have no DTD and no user-defined entities.
*/
#define XML_GE  0               /* must be 0 or 1 */
/* #undef XML_DTD */            /* must stay undefined if XML_GE == 0 */
/* #undef XML_NS */
#define XML_CONTEXT_BYTES  1024

/*
	Entropy source for the hash salt; exactly one family per platform.
   _WIN32 needs nothing here: xmlparse.c then uses rand_s() from random_rand_s.c.
*/
#if defined (_WIN32)
	/* rand_s */
#elif defined (__APPLE__) || defined (__FreeBSD__) || defined (__OpenBSD__) || defined (__NetBSD__)
	#define HAVE_ARC4RANDOM_BUF  1
#elif defined (__linux__)
	#define HAVE_GETRANDOM  1      /* glibc >= 2.25 */
	#define XML_DEV_URANDOM  1     /* fallback if getrandom() fails */
#else
	#define XML_DEV_URANDOM  1
#endif

#endif /* EXPAT_CONFIG_H */
