/* oo_COPY.h
 *
 * Copyright (C) 1994-2007,2009,2011-2020,2022,2024,2026 Paul Boersma
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
	thy x = our x;

#define oo_SET(type, storage, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		thy x [_i] = our x [_i]; \
	}

#define oo_ANYVEC(type, storage, x, sizeExpression)  \
	thy x = newvectorcopy (our x.all());

#define oo_ANYMAT(type, storage, x, nrowExpression, ncolExpression)  \
	thy x = newmatrixcopy (our x.all());

#define oo_ANYTEN3(type, storage, x, ndim1Expression, ndim2Expression, ndim3Expression)  \
	thy x = newtensor3copy (our x.get());

#define oo_ENUMx(kType, storage, x)  \
	thy x = our x;

//#define oo_ENUMx_SET(kType, storage, x, setType)  \
//	for (int _i = 0; _i <= (int) setType::MAX; _i ++) \
//		thy x [_i] = our x [_i];

#define oo_STRINGx(storage, x)  \
	if (our x) \
		thy x = Melder_dup (our x.get());

#define oo_STRINGx_SET(storage, x, setType)  \
	for (int _i = 0; _i <= setType::MAX; _i ++) { \
		if (our x [_i]) \
			thy x [_i] = Melder_dup (our x [_i].get()); \
	}

#define oo_STRINGx_VECTOR(storage, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (_size == our x.size); \
		if (our x) { \
			thy x = autoSTRVEC (_size); \
			for (integer _i = 1; _i <= _size; _i ++) { \
				if (our x [_i]) \
					thy x [_i] = Melder_dup (our x [_i].get()); \
			} \
		} \
	}

#define oo_STRUCT(Type, x)  \
	our x. copy (& thy x);

#define oo_STRUCT_SET(Type, x, setType)  \
	for (int _i = 0; _i <= (int) setType::MAX; _i ++) { \
		our x [_i]. copy (& thy x [_i]); \
	}

#define oo_STRUCTVEC(Type, x, sizeExpression)  \
	{ \
		const integer _size = (sizeExpression); \
		Melder_assert (_size == our x.size); \
		if (_size > 0) { \
			thy x = newvectorzero <struct##Type> (_size); \
			for (integer _i = 1; _i <= _size; _i ++) { \
				our x [_i]. copy (& thy x [_i]); \
			} \
		} \
	}

#define oo_STRUCTMAT(Type, x, nrowExpression, ncolExpression)  \
	{ \
		const integer _nrow = (nrowExpression), _ncol = (nrowExpression); \
		Melder_assert (_nrow == our x.nrow && _ncol == our x.ncol); \
		if (_nrow > 0 && _ncol > 0) { \
			thy x = newmatrixzero <struct##Type> (_nrow, _ncol); \
			for (integer _irow = 1; _irow <= _nrow; _irow ++) { \
				for (integer _icol = 1; _icol <= _ncol; _icol ++) { \
					our x [_irow] [_icol]. copy (& thy x [_irow] [_icol]); \
				} \
			} \
		} \
	}

#define oo_OBJECT(Class, version, x)  \
	if (our x) \
		thy x = Data_copy (our x.get());

#define oo_COLLECTION_OF(Class, x, ItemClass, version)  \
	if (our x._capacity > 0) { \
		thy x.at._elements = Melder_calloc (ItemClass, our x._capacity); \
		thy x.at._elements --; \
	} \
	thy x.size = our x.size; \
	thy x._capacity = our x._capacity; \
	thy x._ownItems = our x._ownItems; \
	thy x._ownershipInitialized = our x._ownershipInitialized; \
	for (integer _i = 1; _i <= our x.size; _i ++) { \
		if (our x.at [_i]) \
			thy x.at [_i] = Data_copy (our x.at [_i]).releaseToAmbiguousOwner(); \
	}

#define oo_COLLECTION(Class, x, ItemClass, version)  \
	if (our x) \
		thy x = Data_copy (our x.get());

#define oo_TRANSIENT_FILE(x)  \
	MelderFile_copy (& our x, & thy x);

#define oo_TRANSIENT_FOLDER(x)  \
	MelderFolder_copy (& our x, & thy x);

#define oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE(Class, x)  \
	thy x = our x;   // reference copy

#define oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE(Class, x)  \
	thy x = our x;   // reference copy

#define oo_DEFINE_STRUCT(Type)  \
	void struct##Type :: copy (Type thee) const {

#define oo_END_STRUCT(Type)  \
	}

#define oo_DEFINE_CLASS(Class, Parent)  \
	void struct##Class :: v1_copy (Daata _thee_Daata) const { \
		Class thee = static_cast <Class> (_thee_Daata); \
		Class##_Parent :: v1_copy (thee);

#define oo_END_CLASS(Class)  \
	}

#define oo_FROM(from)

#define oo_ENDFROM

#define oo_DECLARING  0
#define oo_DESTROYING  0
#define oo_COPYING  1
#define oo_COMPARING  0
#define oo_VALIDATING_ENCODING  0
#define oo_READING  0
#define oo_READING_TEXT  0
#define oo_READING_BINARY  0
#define oo_WRITING  0
#define oo_WRITING_TEXT  0
#define oo_WRITING_BINARY  0
#define oo_DESCRIBING  0

/* End of file oo_COPY.h */
