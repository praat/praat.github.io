/* oo_READ_TEXT.h
 *
 * Copyright (C) 1994-2009,2011-2020,2022,2024,2026 Paul Boersma
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

#include "oo_undef.h"

#define oo_SIMPLE(type, storage, x)  \
	try { \
		our x = texget##storage (_textSource_); \
	} catch (MelderError) { \
		Melder_throw (U"\"" #x U"\" not read."); \
	}

#define oo_SET(type, storage, x, setType)  \
	for (integer _i = 0; _i <= (int) setType::MAX; _i ++) { \
		try { \
			our x [_i] = texget##storage (_textSource_); \
		} catch (MelderError) { \
			Melder_throw (U"Element ", _i+1, U" of \"" #x, U"\" not read."); \
		} \
	}

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		our x = vector_readText_##storage (_size, _textSource_, #x); \
	}

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	{ \
		const integer _nrow = (nrowExpression), _ncol = (ncolExpression); \
		our x = matrix_readText_##storage (_nrow, _ncol, _textSource_, #x); \
	}

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	{ \
		const integer _ndim1 = (ndim1Expression), _ndim2 = (ndim2Expression), _ndim3 = (ndim3Expression); \
		our x = tensor3_readText_##storage (_ndim1, _ndim2, _ndim3, _textSource_, #x); \
	}

#define oo_ENUMx(kType, storage, x)  \
	our x = (kType) texget##storage (_textSource_, (enum_generic_getValue) kType##_getValue);

#define oo_STRINGx(storage, x)  \
	try { \
		our x = texget##storage (_textSource_); \
	} catch (MelderError) { \
		Melder_throw (U"String \"" #x U"\" not read."); \
	}

#define oo_STRINGx_SET(storage, x, setType)  \
	for (integer _i = 0; _i <= setType::MAX; _i ++) { \
		our x [_i] = texget##storage (_textSource_); \
	}

#define oo_STRINGx_VECTOR(storage, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		if (_size > 0) { \
			our x = autoSTRVEC (_size); \
			for (integer _i = 1; _i <= _size; _i ++) { \
				try { \
					our x [_i] = texget##storage (_textSource_); \
				} catch (MelderError) { \
					Melder_throw (U"Element ", _i, U" of \"" #x U"\" not read."); \
				} \
			} \
		} \
	}

#define oo_STRUCT(Type, x)  \
	our x. readText (_textSource_, _formatVersion_);

#define oo_STRUCT_SET(Type, x, setType) \
	for (integer _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. readText (_textSource_, _formatVersion_); \
	}

#define oo_STRUCTVEC(Type, x, sizeExpression)  \
{ \
	const integer _size = (sizeExpression); \
	if (_size > 0) { \
		our x = newvectorzero <struct##Type> (_size); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			our x [_i]. readText (_textSource_, _formatVersion_); \
		} \
	} \
}

#define oo_STRUCTMAT(Type, x, nrowExpression, ncolExpression)  \
	{ \
		const integer _nrow = (nrowExpression), _ncol = (ncolExpression); \
		if (_nrow > 0 && _ncol > 0) { \
			our x = newmatrixzero <struct##Type> (_nrow, _ncol); \
			for (integer _irow = 1; _irow <= _nrow; _irow ++) { \
				for (integer _icol = 1; _icol <= _ncol; _icol ++) { \
					our x [_irow] [_icol]. readText (_textSource_, _formatVersion_); \
				} \
			} \
		} \
	}

#define oo_OBJECT(Class, formatVersion, x)  \
	{ \
		const int _formatVersion = (formatVersion); \
		if (texgetex (_textSource_) == 1) { \
			our x = Thing_new (Class); \
			our x -> v1_readText (_textSource_, _formatVersion); \
		} \
	}

#define oo_COLLECTION_OF(Class, x, ItemClass, formatVersion)  \
	{ \
		const int _formatVersion = (formatVersion); \
		const integer _n = texgetinteger (_textSource_); \
		for (integer _i = 1; _i <= _n; _i ++) { \
			auto##ItemClass _item = Thing_new (ItemClass); \
			_item -> v1_readText (_textSource_, _formatVersion); \
			our x. addItem_move (_item.move()); \
		} \
	}

#define oo_COLLECTION(Class, x, ItemClass, formatVersion)  \
	{ \
		const int _formatVersion = (formatVersion); \
		const integer _n = texgetinteger (_textSource_); \
		our x = Class##_create (); \
		for (integer _i = 1; _i <= _n; _i ++) { \
			auto##ItemClass _item = Thing_new (ItemClass); \
			_item -> v1_readText (_textSource_, _formatVersion); \
			our x -> addItem_move (_item.move()); \
		} \
	}

#define oo_TRANSIENT_FILE(x)

#define oo_TRANSIENT_FOLDER(x)

#define oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE(Class, x)

#define oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE(Class, x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: readText (MelderReadText _textSource_, int _formatVersion_) { \
		(void) _formatVersion_;

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v1_readText (MelderReadText _textSource_, int _formatVersion_) { \
		Melder_require (_formatVersion_ <= our classInfo -> version, \
			U"The format of this file is too new. Download a newer version of Praat."); \
		Class##_Parent :: v1_readText (_textSource_, _formatVersion_);

#define oo_END_CLASS(Class)  \
	}

#define oo_FROM(from)  \
	{ \
		const int _from = (from); \
		if (_formatVersion_ >= _from) {

#define oo_ENDFROM  \
		} \
	}

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  1
#define oo_READING_TEXT  1
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_READ_TEXT.h */
