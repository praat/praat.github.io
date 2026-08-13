/* VEC.cpp
 *
 * Copyright (C) 2017-2021,2023,2025,2026 Paul Boersma
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
#include "../dwsys/NUM2.h"   /* for NUMsort2 */
#include "../external/blake3/blake3.h"

#ifdef macintosh
	#include "macport_on.h"
	#include <Accelerate/Accelerate.h>
	#include "macport_off.h"
#endif

#if defined (macintosh)
void _add_macfast_VEC_out (VECVU const& target, constVECVU const& x, constVECVU const& y) noexcept {
	integer n = target.size;
	vDSP_vaddD (& x [1], x.stride, & y [1], y.stride, & target [1], target.stride, integer_to_uinteger_a (n));
	/*
		Speed if always vDSP_vaddD:
			//9.3,1.26,0.21, 0.10,0.42,0.70, 1.17,1.97,5.32
		Speed if always explicit loop:
			0.61,1.99,4.32, 4.39,2.61,1.51, 0.85,0.50,0.41
		Combined:
			6.1,1.19,0.23, 0.13,0.41,0.70, 1.19,2.02,2.45
	*/
}
#endif

autoVEC from_to_VEC (double from, double to) {
	const integer numberOfElements = Melder_ifloor (to - from + 1.0);
	if (numberOfElements < 1)
		return autoVEC ();
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (double) (i - 1);
	return result;
}
autoINTVEC from_to_INTVEC (integer from, integer to) {
	const integer numberOfElements = to - from + 1;
	if (numberOfElements < 1)
		return autoINTVEC ();
	autoINTVEC result = raw_INTVEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (i - 1);
	return result;
}

autoVEC from_to_by_VEC (double from, double to, double by) {
	Melder_require (by != 0.0,
		U"from_to_by#: cannot have a step (“by”) of zero.");
	/*
		The following algorithm works for both positive and negative `by`.
	*/
	const integer numberOfElements = Melder_ifloor ((to - from) / by + 1.0);
	if (numberOfElements < 1)
		return autoVEC ();
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (double) (i - 1) * by;
	return result;
}
autoINTVEC from_to_by_INTVEC (integer from, integer to, integer by) {
	Melder_require (by != 0,
		U"from_to_by#: cannot have a step (“by”) of zero.");
	/*
		The following algorithm works for both positive and negative `by`.
	*/
	const integer numberOfElements = (to - from) / by + 1;
	if (numberOfElements < 1)
		return autoINTVEC ();
	autoINTVEC result = raw_INTVEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (i - 1) * by;
	return result;
}
/*@praat
	assert from_to_count# (0, 10, 5) = { 0, 2.5, 5, 7.5, 10 }
@*/
autoVEC from_to_count_VEC (double from, double to, integer count) {
	Melder_require (count >= 2,
		U"from_to_count#: cannot have fewer than two elements.");
	autoVEC result = raw_VEC (count);
	const double by = (to - from) / (count - 1);
	for (integer i = 1; i < count; i ++)
		result [i] = from + (double) (i - 1) * by;
	result [count] = to;
	return result;
}
autoINTVEC from_to_count_INTVEC (integer from, integer to, integer count) {
	Melder_require (count >= 2,
		U"from_to_count#: cannot have fewer than two elements.");
	autoINTVEC result = raw_INTVEC (count);
	const integer by = (to - from) / (count - 1);
	for (integer i = 1; i < count; i ++)
		result [i] = from + (i - 1) * by;
	result [count] = to;
	return result;
}

autoVEC between_by_VEC (double from, double to, double by) {
	Melder_require (by != 0.0,
		U"between_by#: cannot have a step (“by”) of zero.");
	/*
		The following algorithm works for both positive and negative `by`.
	*/
	const integer numberOfElements = Melder_ifloor ((to - from) / by + 1.0);
	if (numberOfElements < 1)
		return autoVEC ();
	const double spaceNeeded = (numberOfElements - 1) * by;
	const double spaceOnEdges = (to - from) - spaceNeeded;
	const double first = from + 0.5 * spaceOnEdges;
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = first + (double) (i - 1) * by;
	return result;
}

/*@praat
	assert between_count# (0, 10, 5) = { 1, 3, 5, 7, 9 }
@*/
autoVEC between_count_VEC (double from, double to, integer count) {
	Melder_require (count >= 0,
		U"between_count#: cannot have fewer than zero elements.");
	if (count < 1)
		return autoVEC ();
	const double by = (to - from) / count;
	autoVEC result = raw_VEC (count);
	for (integer i = 1; i <= count; i ++)
		result [i] = from + (double) (i - 0.5) * by;
	return result;
}

void mul_VEC_out (VECVU const& target, constVECVU const& vec, constMATVU const& mat) {
	Melder_assert (mat.nrow == vec.size);
	Melder_assert (target.size == mat.ncol);
	if ((true)) {
		for (integer icol = 1; icol <= mat.ncol; icol ++) {
			target [icol] = 0.0;
			for (integer irow = 1; irow <= mat.nrow; irow ++)
				target [icol] += vec [irow] * mat [irow] [icol];
		}
	} else if ((false)) {
		for (integer icol = 1; icol <= mat.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, vec.size,
				const double *px = & vec [1];
				const double *py = & mat [1] [icol],
				(longdouble) *px * (longdouble) *py,
				(px += vec.stride, py += mat.rowStride)   // this goes way beyond the confines of mat
			)
			target [icol] = double (sum);
		}
	}
}

autoVEC mul_VEC (constVECVU const& vec, constMATVU const& mat) {
	autoVEC result = raw_VEC (mat.ncol);
	mul_VEC_out (result.get(), vec, mat);
	return result;
}

void mul_VEC_out (VECVU const& target, constMATVU const& mat, constVECVU const& vec) {
	Melder_assert (vec.size == mat.ncol);
	Melder_assert (target.size == mat.nrow);
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++)
				target [i] += mat [i] [j] * vec [j];
		} else {
			target [i] = NUMinner (mat.row (i), vec);
		}
	}
}

autoVEC mul_VEC (constMATVU const& mat, constVECVU const& vec) {
	autoVEC result = raw_VEC (mat.nrow);
	mul_VEC_out (result.all(), mat, vec);
	return result;
}

void power_VEC_out (VECVU const& target, constVECVU const& vec, double power) {
	if (power == 2.0) {
		for (integer i = 1; i <= target.size; i ++)
			target [i] = vec [i] * vec [i];
	} else if (power < 0.0) {
		if (power == -1.0) {
			for (integer i = 1; i <= target.size; i ++) {
				Melder_require (vec [i] != 0.0,
					U"Cannot raise zero to a negative power.");
				target [i] = 1.0 / vec [i];
			}
		} else if (power == -2.0) {
			for (integer i = 1; i <= target.size; i ++) {
				Melder_require (vec [i] != 0.0,
					U"Cannot raise zero to a negative power.");
				target [i] = 1.0 / (vec [i] * vec [i]);
			}
		} else {
			for (integer i = 1; i <= target.size; i ++) {
				Melder_require (vec [i] != 0.0,
					U"Cannot raise zero to a negative power.");
				target [i] = pow (vec [i], power);
			}
		}
	} else {
		for (integer i = 1; i <= target.size; i ++)
			target [i] = pow (vec [i], power);
	}
}
void power_VEC_inout (VECVU const& vec, double power) {
	power_VEC_out (vec, vec, power);
}

autoVEC splitByWhitespace_VEC (conststring32 string) {
	if (! string)
		return autoVEC();   // accept null pointer input
	const integer n = NUMnumberOfTokens (string);
	if (n == 0)
		return autoVEC();
	autoVEC result (n, MelderArray::kInitializationType::ZERO);

	integer itoken = 0;
	const char32 *p = & string [0];
	for (;;) {
		Melder_skipHorizontalOrVerticalSpace (& p);
		if (*p == U'\0')
			break;
		const char32 *beginOfInk = p;
		p ++;   // step over first nonspace
		p = Melder_findEndOfInk (p);
		result [++ itoken] = Melder_atof (beginOfInk);
	}
	return result;
}

/*
	Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
	1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
	Overlap is allowed. Ranges can go up and down.
*/
autoINTVEC splitByWhitespaceWithRanges_INTVEC (conststring32 ranges) {
	autoINTVEC elements = raw_INTVEC (0);
	integer previousElement = 0;
	const char32 *p = & ranges [0];
	for (;;) {
		while (Melder_isHorizontalSpace (*p))
			p ++;
		if (*p == U'\0')
			break;
		if (Melder_isAsciiDecimalNumber (*p) || *p == '-' && Melder_isAsciiDecimalNumber (p [1])) {
			const integer currentElement = Melder_atoi (p);
			* elements.append() = currentElement;
			previousElement = currentElement;
			do {
				p ++;
			} while (Melder_isAsciiDecimalNumber (*p));
		} else if (*p == U':') {
			Melder_require (previousElement != 0,
				U"The range should not start with a colon.");
			do {
				p ++;
			} while (Melder_isHorizontalSpace (*p));
			Melder_require (*p != U'\0',
				U"The range should not end with a colon.");
			Melder_require (Melder_isAsciiDecimalNumber (*p) || *p == '-' && Melder_isAsciiDecimalNumber (p [1]),
				U"End of range should be a whole number.");
			const integer currentElement = Melder_atoi (p);
			if (currentElement > previousElement)
				for (integer ielement = previousElement + 1; ielement <= currentElement; ielement ++)
					* elements.append() = ielement;
			else
				for (integer ielement = previousElement - 1; ielement >= currentElement; ielement --)
					* elements.append() = ielement;
			previousElement = currentElement;
			do {
				p ++;
			} while (Melder_isAsciiDecimalNumber (*p));
		} else {
			Melder_throw (U"Start of range should be a whole number.");
		}
	}
	return elements;
}

autoVEC cast_VEC (constINTVEC const& intvec) {
	autoVEC result = raw_VEC (intvec.size);
	for (integer i = 1; i <= intvec.size; i ++)
		result [i] = intvec [i];
	return result;
}

autoINTVEC ifloor_INTVEC (constVEC const& vec) {
	autoINTVEC result = raw_INTVEC (vec.size);
	for (integer i = 1; i <= vec.size; i ++)
		result [i] = Melder_ifloor (vec [i]);
	return result;
}

autoINTVEC iround_INTVEC (constVEC const& vec) {
	autoINTVEC result = raw_INTVEC (vec.size);
	for (integer i = 1; i <= vec.size; i ++)
		result [i] = Melder_iround (vec [i]);
	return result;
}

autoINTVEC iceiling_INTVEC (constVEC const& vec) {
	autoINTVEC result = raw_INTVEC (vec.size);
	for (integer i = 1; i <= vec.size; i ++)
		result [i] = Melder_iceiling (vec [i]);
	return result;
}

void to_INTVEC_out (INTVECVU const& x) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = i;
}

autoVEC to_VEC (double to) {
	const integer numberOfElements = Melder_ifloor (to);
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = (double) i;
	return result;
}
autoINTVEC to_INTVEC (integer to) {
	const integer numberOfElements = to;
	autoINTVEC result = raw_INTVEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = i;
	return result;
}

static autoVEC outputBlake3 (blake3_hasher *hasher) {
	uint8 output [32];
	blake3_hasher_finalize (hasher, output, 32);   // 256 bits
	autoVEC result = raw_VEC (8);
	result [1] = ((uint32) output [0] << 0) | ((uint32) output [1] << 8)
			| ((uint32) output [2] << 16) | ((uint32) output [3] << 24);
	result [2] = ((uint32) output [4] << 0) | ((uint32) output [5] << 8)
			| ((uint32) output [6] << 16) | ((uint32) output [7] << 24);
	result [3] = ((uint32) output [8] << 0) | ((uint32) output [9] << 8)
			| ((uint32) output [10] << 16) | ((uint32) output [11] << 24);
	result [4] = ((uint32) output [12] << 0) | ((uint32) output [13] << 8)
			| ((uint32) output [14] << 16) | ((uint32) output [15] << 24);
	result [5] = ((uint32) output [16] << 0) | ((uint32) output [17] << 8)
			| ((uint32) output [18] << 16) | ((uint32) output [19] << 24);
	result [6] = ((uint32) output [20] << 0) | ((uint32) output [21] << 8)
			| ((uint32) output [22] << 16) | ((uint32) output [23] << 24);
	result [7] = ((uint32) output [24] << 0) | ((uint32) output [25] << 8)
			| ((uint32) output [26] << 16) | ((uint32) output [27] << 24);
	result [8] = ((uint32) output [28] << 0) | ((uint32) output [29] << 8)
			| ((uint32) output [30] << 16) | ((uint32) output [31] << 24);
	return result;
}

autoVEC blake3_VEC (constVECVU const& vec) {
	blake3_hasher hasher;
	blake3_hasher_init (& hasher);
	for (integer i = 1; i <= vec.size; i ++) {
		const double value_float = vec [i];
		const uint64 value_int = * (uint64 *) & value_float;
		uint8 bytes [8];
		bytes [0] = (uint8) (value_int >> 0);   // little-endian
		bytes [1] = (uint8) (value_int >> 8);   // little-endian
		bytes [2] = (uint8) (value_int >> 16);   // little-endian
		bytes [3] = (uint8) (value_int >> 24);   // little-endian
		bytes [4] = (uint8) (value_int >> 32);   // little-endian
		bytes [5] = (uint8) (value_int >> 40);   // little-endian
		bytes [6] = (uint8) (value_int >> 48);   // little-endian
		bytes [7] = (uint8) (value_int >> 56);   // little-endian
		blake3_hasher_update (& hasher, bytes, 8);
	}
	return outputBlake3 (& hasher);
}

autoVEC blake3_VEC (constMATVU const& mat) {
	blake3_hasher hasher;
	blake3_hasher_init (& hasher);
	for (integer irow = 1; irow <= mat.nrow; irow ++)
		for (integer icol = 1; icol <= mat.ncol; icol ++) {
			const double value_float = mat [irow] [icol];
			const uint64 value_int = * (uint64 *) & value_float;
			uint8 bytes [8];
			bytes [0] = (uint8) (value_int >> 0);   // little-endian
			bytes [1] = (uint8) (value_int >> 8);   // little-endian
			bytes [2] = (uint8) (value_int >> 16);   // little-endian
			bytes [3] = (uint8) (value_int >> 24);   // little-endian
			bytes [4] = (uint8) (value_int >> 32);   // little-endian
			bytes [5] = (uint8) (value_int >> 40);   // little-endian
			bytes [6] = (uint8) (value_int >> 48);   // little-endian
			bytes [7] = (uint8) (value_int >> 56);   // little-endian
			blake3_hasher_update (& hasher, bytes, 8);
		}
	return outputBlake3 (& hasher);
}

autoVEC blake3_VEC (conststring32 str) {
	blake3_hasher hasher;
	blake3_hasher_init (& hasher);
	for (const char32 *p = & str [0]; *p != U'\0'; p ++) {
		const uint32 value_int = (uint32) *p;
		uint8 bytes [4];
		bytes [0] = (uint8) (value_int >> 0);   // little-endian
		bytes [1] = (uint8) (value_int >> 8);   // little-endian
		bytes [2] = (uint8) (value_int >> 16);   // little-endian
		bytes [3] = (uint8) (value_int >> 24);   // little-endian
		blake3_hasher_update (& hasher, bytes, 4);
	}
	return outputBlake3 (& hasher);
}

/* End of file VEC.cpp */
