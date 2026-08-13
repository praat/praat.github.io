#pragma once
/* VEC.h
 *
 * Copyright (C) 2017-2021,2023-2026 Paul Boersma
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

#define GENERATE_FIVE_TENSOR_FUNCTIONS  \
	GENERATE_ONE_TENSOR_FUNCTION (operator<<=, =) \
	GENERATE_ONE_TENSOR_FUNCTION (operator+=, +=) \
	GENERATE_ONE_TENSOR_FUNCTION (operator-=, -=) \
	GENERATE_ONE_TENSOR_FUNCTION (operator*=, *=) \
	GENERATE_ONE_TENSOR_FUNCTION (operator/=, /=)

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, double number) noexcept { \
		for (integer i = 1; i <= target.size; i ++) \
			target [i] op number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, constVECVU const& x) { \
		Melder_assert (target.size == x.size); \
		for (integer i = 1; i <= target.size; i ++) \
			target [i] op x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

struct TypeVECadd_VEC_NUM          { constVECVU const& x; double number; };
inline TypeVECadd_VEC_NUM operator+ (constVECVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECadd_VEC_NUM const& expr) { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.x [i] + expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC add_VEC (constVECVU const& x, double number) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  x  +  number;
	return result;
}

struct TypeVECsubtract_VEC_NUM          { constVECVU const& x; double number; };
inline TypeVECsubtract_VEC_NUM operator- (constVECVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECsubtract_VEC_NUM const& expr) { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.x [i] - expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC subtract_VEC (constVECVU const& x, double number) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  x  -  number;
	return result;
}

struct TypeVECmultiply_VEC_NUM          { constVECVU const& x; double number; };
inline TypeVECmultiply_VEC_NUM operator* (constVECVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECmultiply_VEC_NUM const& expr) { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.x [i] * expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC multiply_VEC (constVECVU const& x, double factor) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  x  *  factor;
	return result;
}

struct TypeVECadd_NUM_VEC          { double number; constVECVU const& x; };
inline TypeVECadd_NUM_VEC operator+ (double number, constVECVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECadd_NUM_VEC const& expr) { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.number + expr.x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC add_VEC (double number, constVECVU const& x) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  number  +  x;
	return result;
}

struct TypeVECsubtract_NUM_VEC          { double number; constVECVU const& x; };
inline TypeVECsubtract_NUM_VEC operator- (double number, constVECVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECsubtract_NUM_VEC const& expr) { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.number - expr.x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC subtract_VEC (double number, constVECVU const& x) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  number  -  x;
	return result;
}

struct TypeVECmultiply_NUM_VEC          { double number; constVECVU const& x; };
inline TypeVECmultiply_NUM_VEC operator* (double number, constVECVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECmultiply_NUM_VEC const& expr) { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.number * expr.x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC multiply_VEC (double number, constVECVU const& x) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  number  *  x;
	return result;
}

struct TypeVECadd_VEC_VEC          { constVECVU const& x; constVECVU const& y; };
inline TypeVECadd_VEC_VEC operator+ (constVECVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECadd_VEC_VEC expr) { \
		integer n = target.size; \
		Melder_assert (expr.x.size == n); \
		Melder_assert (expr.y.size == n); \
		for (integer i = 1; i <= n; i ++) \
			target [i] op expr.x [i] + expr.y [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC add_VEC (constVECVU const& x, constVECVU const& y) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  x  +  y;
	return result;
}

struct TypeVECsubtract_VEC_VEC          { constVECVU const& x; constVECVU const& y; };
inline TypeVECsubtract_VEC_VEC operator- (constVECVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECsubtract_VEC_VEC expr) { \
		integer n = target.size; \
		Melder_assert (expr.x.size == n); \
		Melder_assert (expr.y.size == n); \
		for (integer i = 1; i <= n; i ++) \
			target [i] op expr.x [i] - expr.y [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC subtract_VEC (constVECVU const& x, constVECVU const& y) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  x  -  y;
	return result;
}

struct TypeVECmultiply_VEC_VEC          { constVECVU const& x; constVECVU const& y; };
inline TypeVECmultiply_VEC_VEC operator* (constVECVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECmultiply_VEC_VEC expr) { \
		const integer n = target.size; \
		Melder_assert (expr.x.size == n); \
		Melder_assert (expr.y.size == n); \
		for (integer i = 1; i <= n; i ++) \
			target [i] op expr.x [i] * expr.y [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC multiply_VEC (constVECVU const& x, constVECVU const& y) {
	autoVEC result = raw_VEC (x.size);
	result.all()  <<=  x  *  y;
	return result;
}

extern void _add_macfast_VEC_out (VECVU const& target, constVECVU const& x, constVECVU const& y) noexcept;
inline void add_VEC_out (VECVU const& target, constVECVU const& x, constVECVU const& y) {
	const integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	#if defined (macintoshXXX)
		if (n >= 64)
			return _add_macfast_VEC_out (target, x, y);
	#endif
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] + y [i];
}

inline void abs_VEC_out (VECVU const& target, constVECVU const& v) {
	Melder_assert (target.size == v.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = fabs (v [i]);
}
inline void abs_VEC_inout (VECVU const& vec) noexcept {
	for (integer i = 1; i <= vec.size; i ++)
		vec [i] = fabs (vec [i]);
}
inline autoVEC abs_VEC (constVECVU const& vec) {
	autoVEC result = raw_VEC (vec.size);
	abs_VEC_out (result.all(), vec);
	return result;
}

inline void centre_VEC_inout (VECVU const& x, double *out_mean = nullptr) noexcept {
	const double xmean = NUMmean (x);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= xmean;
	if (out_mean)
		*out_mean = xmean;
}

inline autoVEC column_VEC (constMATVU const& source, integer columnNumber) {
	autoVEC target = raw_VEC (source.nrow);
	target.all()  <<=  source.column (columnNumber);
	return target;
}

inline void columnMeans_VEC_out (VECVU const& target, constMATVU const& x) {
	Melder_assert (target.size == x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		target [icol] = NUMmean (x.column (icol));
}
inline autoVEC columnMeans_VEC (constMATVU const& x) {
	autoVEC result = raw_VEC (x.ncol);
	columnMeans_VEC_out (result.get(), x);
	return result;
}

extern void mul_VEC_out (VECVU const& target, constVECVU const& vec, constMATVU const& mat);
extern void mul_VEC_out (VECVU const& target, constMATVU const& mat, constVECVU const& vec);
extern autoVEC mul_VEC (constVECVU const& vec, constMATVU const& mat);
extern autoVEC mul_VEC (constMATVU const& mat, constVECVU const& vec);

inline void neg_VEC_out (VECVU const& target, constVECVU const& v) {
	Melder_assert (target.size == v.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = - v [i];
}
inline void neg_VEC_inout (VECVU const& vec) noexcept {
	for (integer i = 1; i <= vec.size; i ++)
		vec [i] = - vec [i];
}
inline autoVEC neg_VEC (constVECVU const& vec) {
	autoVEC result = raw_VEC (vec.size);
	neg_VEC_out (result.all(), vec);
	return result;
}

extern void power_VEC_out (VECVU const& target, constVECVU const& vec, double power);
extern void power_VEC_inout (VECVU const& vec, double power);
inline autoVEC power_VEC (constVECVU const& vec, double power) {
	autoVEC result = raw_VEC (vec.size);
	power_VEC_out (result.all(), vec, power);
	return result;
}

inline void randomGauss_VEC_out (VECVU const& target, double mu, double sigma) {
	for (integer i = 1; i <= target.size; i ++)
		target [i] = NUMrandomGauss (mu, sigma);
}
inline autoVEC randomGauss_VEC (integer size, double mu, double sigma) {
	autoVEC result = raw_VEC (size);
	randomGauss_VEC_out (result.all(), mu, sigma);
	return result;
}
inline autoVEC randomGauss_VEC (constVECVU const& model, double mu, double sigma) {
	autoVEC result = raw_VEC (model.size);
	randomGauss_VEC_out (result.all(), mu, sigma);
	return result;
}

inline void randomUniform_VEC_out (VECVU const& target, double lowest, double highest) {
	for (integer i = 1; i <= target.size; i ++)
		target [i] = NUMrandomUniform (lowest, highest);
}
inline autoVEC randomUniform_VEC (integer size, double lowest, double highest) {
	autoVEC result = raw_VEC (size);
	randomUniform_VEC_out (result.all(), lowest, highest);
	return result;
}
inline autoVEC randomUniform_VEC (constVECVU const& model, double lowest, double highest) {
	autoVEC result = raw_VEC (model.size);
	randomUniform_VEC_out (result.all(), lowest, highest);
	return result;
}

inline void rowInners_VEC_out (VECVU const& target, constMATVU const& x, constMATVU const& y) {
	Melder_assert (y.nrow == x.nrow);
	Melder_assert (y.ncol == x.ncol);
	Melder_assert (target.size == x.nrow);
	for (integer irow = 1; irow <= target.size; irow ++)
		target [irow] = NUMinner (x.row (irow), y.row (irow));
}
inline autoVEC rowInners_VEC (constMATVU const& x, constMATVU const& y) {
	autoVEC result = raw_VEC (x.nrow);
	rowInners_VEC_out (result.all(), x, y);
	return result;
}

inline void sin_VEC_inout (VECVU const& vec) noexcept {
	for (integer i = 1; i <= vec.size; i ++)
		vec [i] = sin (vec [i]);
}

extern void shuffle_VEC_inout (VECVU const& x);
extern void shuffle_INTVEC_inout (INTVECVU const& x);

inline autoVEC shuffle_VEC (constVECVU const& x) {
	autoVEC result = copy_VEC (x);
	shuffle_VEC_inout (result.get());
	return result;
}

inline autoINTVEC shuffle_INTVEC (constINTVECVU const& x) {
	autoINTVEC result = copy_INTVEC (x);
	shuffle_INTVEC_inout (result.get());
	return result;
}

extern void sort_e_VEC_inout (VEC const& x);   // cannot be a VECVU
extern void sort_VEC_inout (VEC const& x);   // cannot be a VECVU
extern void sort_INTVEC_inout (INTVEC const& x) noexcept;   // cannot be an INTVECVU


inline autoVEC sort_e_VEC (constVECVU const& x) {
	autoVEC result = copy_VEC (x);
	sort_e_VEC_inout (result.get());
	return result;
}

inline autoINTVEC sort_INTVEC (constINTVECVU const& x) {
	autoINTVEC result = copy_INTVEC (x);
	sort_INTVEC_inout (result.get());
	return result;
}

autoVEC sort_removeUndefined_VEC (constVECVU const& vec);

inline autoINTVEC sortedSet_INTVEC (constINTVECVU const& x) {
	if (x.size <= 0)
		return autoINTVEC ();
	autoINTVEC result = sort_INTVEC (x);
	integer newSize = 1;
	for (integer i = 2; i <= x.size; i ++)
		if (result [i] != result [i - 1])
			result [++ newSize] = result [i];
	result. resize (newSize);
	return result;
}

inline autoVEC rowSums_VEC (constMATVU const& x) {
	autoVEC result = raw_VEC (x.nrow);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		result [irow] = NUMsum (x.row (irow));
	return result;
}

inline autoVEC columnSums_VEC (constMATVU const& x) {
	autoVEC result = raw_VEC (x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		result [icol] = NUMsum (x.column (icol));
	return result;
}

extern autoVEC to_VEC (double to);
extern autoVEC from_to_VEC (double from, double to);
extern autoVEC from_to_by_VEC (double from, double to, double by);
extern autoVEC from_to_count_VEC (double from, double to, integer count);
extern autoVEC between_by_VEC (double from, double to, double by);
extern autoVEC between_count_VEC (double from, double to, integer count);

autoVEC splitByWhitespace_VEC (conststring32 string);

autoINTVEC splitByWhitespaceWithRanges_INTVEC (conststring32 ranges);

autoVEC cast_VEC (constINTVEC const& intvec);
autoINTVEC ifloor_INTVEC (constVEC const& vec);
autoINTVEC iround_INTVEC (constVEC const& vec);
autoINTVEC iceiling_INTVEC (constVEC const& vec);

extern void to_INTVEC_out (INTVECVU const& x) noexcept;
extern autoINTVEC to_INTVEC (integer to);
extern autoINTVEC from_to_INTVEC (integer from, integer to);
extern autoINTVEC from_to_by_INTVEC (integer from, integer to, integer by);
extern autoINTVEC from_to_count_INTVEC (integer from, integer to, integer count);

extern autoVEC blake3_VEC (constVECVU const& vec);
extern autoVEC blake3_VEC (constMATVU const& mat);
extern autoVEC blake3_VEC (conststring32 str);

/* End of file VEC.h */
