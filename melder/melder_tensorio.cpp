/* melder_tensorio.cpp
 *
 * Copyright (C) 1992-2018,2020,2026 Paul Boersma
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

#include "melder.h"

/*** Typed I/O functions for vectors and matrices. ***/

#define FUNCTION(T,storage)  \
	void vector_writeText_##storage (const constvector<T>& vec, MelderFile file, conststring32 name) { \
		texputintro (file, name, U" []: ", vec.size >= 1 ? nullptr : U"(empty)", 0,0,0); \
		for (integer i = 1; i <= vec.size; i ++) \
			texput##storage (file, vec [i], name, U" [", Melder_integer (i), U"]", 0,0); \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) Melder_throw (U"Write error."); \
	} \
	void vector_writeBinary_##storage (const constvector<T>& vec, FILE *f) { \
		for (integer i = 1; i <= vec.size; i ++) \
			binput##storage (vec [i], f); \
		if (feof (f) || ferror (f)) Melder_throw (U"Write error."); \
	} \
	autovector<T> vector_readText_##storage (integer size, MelderReadText text, const char *name) { \
		autovector<T> result = newvectorzero<T> (size); \
		for (integer i = 1; i <= size; i ++) { \
			try { \
				result [i] = texget##storage (text); \
			} catch (MelderError) { \
				Melder_throw (U"Could not read ", Melder_peek8to32_u (name), U" [", i, U"]."); \
			} \
		} \
		return result; \
	} \
	autovector<T> vector_readBinary_##storage (integer size, FILE *f) { \
		autovector<T> result = newvectorzero<T> (size); \
		for (integer i = 1; i <= size; i ++) { \
			result [i] = binget##storage (f); \
		} \
		return result; \
	} \
	void matrix_writeText_##storage (const constmatrix<T>& mat, MelderFile file, conststring32 name) { \
		texputintro (file, name, U" [] []: ", mat.nrow >= 1 ? nullptr : U"(empty)", 0,0,0); \
		for (integer irow = 1; irow <= mat.nrow; irow ++) { \
			texputintro (file, name, U" [", Melder_integer (irow), U"]:", 0,0); \
			for (integer icol = 1; icol <= mat.ncol; icol ++) { \
				texput##storage (file, mat [irow] [icol], name, U" [", Melder_integer (irow), U"] [", Melder_integer (icol), U"]"); \
			} \
			texexdent (file); \
		} \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) Melder_throw (U"Write error."); \
	} \
	void matrix_writeBinary_##storage (const constmatrix<T>& mat, FILE *f) { \
		for (integer irow = 1; irow <= mat.nrow; irow ++) { \
			for (integer icol = 1; icol <= mat.ncol; icol ++) \
				binput##storage (mat [irow] [icol], f); \
		} \
		if (feof (f) || ferror (f)) Melder_throw (U"Write error."); \
	} \
	automatrix<T> matrix_readText_##storage (integer nrow, integer ncol, MelderReadText text, const char *name) { \
		automatrix<T> result = newmatrixzero<T> (nrow, ncol); \
		for (integer irow = 1; irow <= nrow; irow ++) for (integer icol = 1; icol <= ncol; icol ++) { \
			try { \
				result [irow] [icol] = texget##storage (text); \
			} catch (MelderError) { \
				Melder_throw (U"Could not read ", Melder_peek8to32_u (name), U" [", irow, U"] [", icol, U"]."); \
			} \
		} \
		return result; \
	} \
	automatrix<T> matrix_readBinary_##storage (integer nrow, integer ncol, FILE *f) { \
		automatrix<T> result = newmatrixzero<T> (nrow, ncol); \
		for (integer irow = 1; irow <= nrow; irow ++) for (integer icol = 1; icol <= ncol; icol ++) \
			result [irow] [icol] = binget##storage (f); \
		return result; \
	} \
	void tensor3_writeText_##storage (const consttensor3<T>& ten3, MelderFile file, conststring32 name) { \
		texputintro (file, name, U" [] [] []: ", ten3.ndim1 >= 1 && ten3.ndim2 >= 1 && ten3.ndim3 >= 1 ? nullptr : U"(empty)", 0,0,0); \
		for (integer idim1 = 1; idim1 <= ten3.ndim1; idim1 ++) { \
			texputintro (file, name, U" [", Melder_integer (idim1), U"]:", 0,0); \
			for (integer idim2 = 1; idim2 <= ten3.ndim2; idim2 ++) { \
				texputintro (file, name, U" [", Melder_integer (idim1), U"] [", Melder_integer (idim2), U"]:", 0,0); \
				for (integer idim3 = 1; idim3 <= ten3.ndim3; idim3 ++) { \
					texput##storage (file, ten3 [idim1] [idim2] [idim3], name, U" [", Melder_integer (idim1), U"] [", Melder_integer (idim2), U"] [", Melder_integer (idim3), U"]"); \
				} \
			} \
			texexdent (file); \
		} \
		texexdent (file); \
		if (feof (file -> filePointer) || ferror (file -> filePointer)) Melder_throw (U"Write error."); \
	} \
	void tensor3_writeBinary_##storage (const consttensor3<T>& ten3, FILE *f) { \
		for (integer idim1 = 1; idim1 <= ten3.ndim1; idim1 ++) { \
			for (integer idim2 = 1; idim2 <= ten3.ndim2; idim2 ++) { \
				for (integer idim3 = 1; idim3 <= ten3.ndim3; idim3 ++) { \
					binput##storage (ten3 [idim1] [idim2] [idim3], f); \
				} \
			} \
		} \
		if (feof (f) || ferror (f)) Melder_throw (U"Write error."); \
	} \
	autotensor3<T> tensor3_readText_##storage (integer ndim1, integer ndim2, integer ndim3, MelderReadText text, const char *name) { \
		autotensor3<T> result = newtensor3zero<T> (ndim1, ndim2, ndim3); \
		for (integer idim1 = 1; idim1 <= result.ndim1; idim1 ++) { \
			for (integer idim2 = 1; idim2 <= result.ndim2; idim2 ++) { \
				for (integer idim3 = 1; idim3 <= result.ndim3; idim3 ++) { \
					try { \
						result [idim1] [idim2] [idim3] = texget##storage (text); \
					} catch (MelderError) { \
						Melder_throw (U"Could not read ", Melder_peek8to32_u (name), U" [", idim1, U"] [", idim2, U"] [", idim3, U"]."); \
					} \
				} \
			} \
		} \
		return result; \
	} \
	autotensor3<T> tensor3_readBinary_##storage (integer ndim1, integer ndim2, integer ndim3, FILE *f) { \
		autotensor3<T> result = newtensor3zero<T> (ndim1, ndim2, ndim3); \
		for (integer idim1 = 1; idim1 <= result.ndim1; idim1 ++) { \
			for (integer idim2 = 1; idim2 <= result.ndim2; idim2 ++) { \
				for (integer idim3 = 1; idim3 <= result.ndim3; idim3 ++) { \
					result [idim1] [idim2] [idim3] = binget##storage (f); \
				} \
			} \
		} \
		return result; \
	}

FUNCTION (signed char, i8)
FUNCTION (int, i16)
FUNCTION (long, i32)
FUNCTION (integer, integer32BE)
FUNCTION (integer, integer16BE)
FUNCTION (unsigned char, u8)
FUNCTION (unsigned int, u16)
FUNCTION (unsigned long, u32)
FUNCTION (double, r32)
FUNCTION (double, r64)
FUNCTION (dcomplex, c64)
FUNCTION (dcomplex, c128)
FUNCTION (bool, eb)
#undef FUNCTION

/* End of file melder_tensorio.cpp */
