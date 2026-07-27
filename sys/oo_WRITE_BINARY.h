/* oo_WRITE_BINARY.h
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
	binput##storage (our x, _filePointer_);

#define oo_SET(type, storage, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		binput##storage (our x [_i], _filePointer_); \
	}

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (our x.size == _size); \
		vector_writeBinary_##storage (our x.get(), _filePointer_); \
	}

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	{ \
		const integer _nrow = (nrowExpression), _ncol = (ncolExpression); \
		Melder_assert (our x.nrow == _nrow && our x.ncol == _ncol); \
		matrix_writeBinary_##storage (our x.get(), _filePointer_); \
	}

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	{ \
		const integer _ndim1 = (ndim1Expression), _ndim2 = (ndim2Expression), _ndim3 = (ndim3Expression); \
		tensor3_writeBinary_##storage (our x.get(), _filePointer_); \
	}

#define oo_ENUMx(kType, storage, x)  \
	binput##storage ((int) our x, _filePointer_);

//#define oo_ENUMx_SET(kType, storage, x, setType)  \
//	for (int _i = 0; _i <= setType::MAX; _i ++) { \
//		binput##storage ((int) our x [_i], _filePointer_); \
//	}

#define oo_STRINGx(storage,x)  \
	binput##storage (our x.get(), _filePointer_);

#define oo_STRINGx_SET(storage, x, setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) { \
		binput##storage (our x [_i].get(), _filePointer_); \
	}

#define oo_STRINGx_VECTOR(storage, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (_size == our x.size); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			binput##storage (our x [_i].get(), _filePointer_);  \
		} \
	}

#define oo_STRUCT(Type, x)  \
	our x. writeBinary (_filePointer_);

#define oo_STRUCT_SET(Type, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. writeBinary (_filePointer_); \
	}

#define oo_STRUCTVEC(Type, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (our x.size == _size); \
		for (integer _i = 1; _i <= _size; _i ++) { \
			our x [_i]. writeBinary (_filePointer_); \
		} \
	}

#define oo_STRUCTMAT(Type, x, nrowExpression, ncolExpression)  \
	{ \
		const integer _nrow = (nrowExpression), _ncol = (ncolExpression); \
		Melder_assert (our x.nrow == _nrow && our x.ncol == _ncol); \
		for (integer _irow = 1; _irow <= _nrow; _irow ++) { \
			for (integer _icol = 1; _icol <= _ncol; _icol ++) { \
				our x [_irow] [_icol]. writeBinary (_filePointer_); \
			} \
		} \
	}

#define oo_OBJECT(Class, version, x)  \
	binputex ((bool) our x, _filePointer_); \
	if (our x) { \
		Data_writeBinary (our x.get(), _filePointer_); \
	}

#define oo_COLLECTION_OF(Class, x, ItemClass, version)  \
	binputinteger32BE (our x.size, _filePointer_); \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		ItemClass data = our x.at [_i]; \
		data -> struct##ItemClass :: v1_writeBinary (_filePointer_); \
	}

#define oo_COLLECTION(Class, x, ItemClass, version)  \
	binputinteger32BE (our x ? our x->size : 0, _filePointer_); \
	if (our x) { \
		for (integer _i = 1; _i <= our x->size; _i ++) { \
			ItemClass data = our x->at [_i]; \
			data -> struct##ItemClass :: v1_writeBinary (_filePointer_); \
		} \
	}

#define oo_TRANSIENT_FILE(x)

#define oo_TRANSIENT_FOLDER(x)

#define oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE(Class, x)

#define oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE(Class, x)

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: writeBinary (FILE *_filePointer_) {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v1_writeBinary (FILE *_filePointer_) { \
		Class##_Parent :: v1_writeBinary (_filePointer_);

#define oo_END_CLASS(Class)  \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  0
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  1
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  1
#define oo_DESCRIBING  0

/* End of file oo_WRITE_BINARY.h */
