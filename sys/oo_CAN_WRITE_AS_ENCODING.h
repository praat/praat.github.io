/* oo_CAN_WRITE_AS_ENCODING.h
 *
 * Copyright (C) 2007,2009,2011-2020,2022,2024,2026 Paul Boersma
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

#define oo_SIMPLE(type, storage, x)

#define oo_SET(type, storage, x, setType)

#define oo_ANYVEC(type, storage, x, sizeExpression)

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)

#define oo_ENUMx(kType, storage, x)

//#define oo_ENUMx_SET(kType, storage, x, setType)

#define oo_STRINGx(storage, x)  \
	if (our x && ! Melder_isEncodable (our x.get(), encoding)) \
		return false;

#define oo_STRINGx_SET(storage, x, setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) \
		if (our x [_i] && ! Melder_isEncodable (our x [_i].get(), encoding)) \
			return false;

#define oo_STRINGx_VECTOR(storage, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (_size == our x.size); \
		if (our x) { \
			for (integer _i = 1; _i <= _size; _i ++) { \
				if (our x [_i] && ! Melder_isEncodable (our x [_i].get(), encoding)) \
					return false; \
			} \
		} \
	}

#define oo_STRUCT(Type, x)  \
	if (! our x. canWriteAsEncoding (encoding)) \
		return false;

#define oo_STRUCT_SET(Type, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		if (! our x [_i]. canWriteAsEncoding (encoding)) \
			return false; \
	}

#define oo_STRUCTVEC(Type, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (_size == our x.size); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			if (! our x [_i]. canWriteAsEncoding (encoding)) \
				return false; \
		} \
	}

#define oo_STRUCTMAT(Type, x, nrowExpression, ncolExpression)  \
	{ \
		const integer _nrow = (nrowExpression), _ncol = (nrowExpression); \
		Melder_assert (_nrow == our x.nrow && _ncol == our x.ncol); \
		for (integer _irow = 1; _irow <= _nrow; _irow ++) { \
			for (integer _icol = 1; _icol <= _ncol; _icol ++) { \
				if (! our x [_irow] [_icol]. canWriteAsEncoding (encoding)) \
					return false; \
			} \
		} \
	}

#define oo_OBJECT(Class, version, x)  \
	if (our x && ! Data_canWriteAsEncoding (our x.get(), encoding)) \
		return false;

#define oo_COLLECTION_OF(Class, x, ItemClass, version)  \
	for (integer _i = 1; _i <= x.size; _i ++) { \
		if (our x.at [_i] && ! Data_canWriteAsEncoding (our x.at [_i], encoding)) \
			return false; \
	}

#define oo_COLLECTION(Class, x, ItemClass, version)  \
	if (our x && ! Data_canWriteAsEncoding (our x.get(), encoding)) \
		return false;

#define oo_TRANSIENT_FILE(x)  \
	return true;   // because it won't actually be written

#define oo_TRANSIENT_FOLDER(x)  \
	return true;   // because it won't actually be written

#define oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE(Class, x)  \
	return true;   // because it won't actually be written

#define oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE(Class, x)  \
	return true;   // because it won't actually be written

#define oo_DEFINE_STRUCT(Type)  \
	bool struct##Type :: canWriteAsEncoding (int encoding) { \
		(void) encoding;

#define oo_END_STRUCT(Type)  \
		return true; \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	bool struct##Class :: v1_canWriteAsEncoding (int encoding) { \
		if (! Class##_Parent :: v1_canWriteAsEncoding (encoding)) \
			return false;

#define oo_END_CLASS(Class)  \
		return true; \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  1
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_CAN_WRITE_AS_ENCODING.h */
