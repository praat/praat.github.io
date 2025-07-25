/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2000-2009  Josh Coalson
 * Copyright (C) 2011-2025  Xiph.Org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//ppgb #ifdef HAVE_CONFIG_H
#include "flac_config.h"
//ppgb #endif

#include <math.h>
#include <string.h>
#include "flac_share_compat.h"
#include "flac_private_bitmath.h"
#include "flac_private_fixed.h"
#include "flac_private_macros.h"
#include "flac_FLAC_assert.h"

#ifdef local_abs
#undef local_abs
#endif
#define local_abs(x) ((uint32_t)((x)<0? -(x) : (x)))

#ifdef local_abs64
#undef local_abs64
#endif
#define local_abs64(x) ((uint64_t)((x)<0? -(x) : (x)))

#ifdef FLAC__INTEGER_ONLY_LIBRARY
/* rbps stands for residual bits per sample
 *
 *             (ln(2) * err)
 * rbps = log  (-----------)
 *           2 (     n     )
 */
static FLAC__fixedpoint local__compute_rbps_integerized(FLAC__uint32 err, FLAC__uint32 n)
{
	FLAC__uint32 rbps;
	uint32_t bits; /* the number of bits required to represent a number */
	int fracbits; /* the number of bits of rbps that comprise the fractional part */

	FLAC__ASSERT(sizeof(rbps) == sizeof(FLAC__fixedpoint));
	FLAC__ASSERT(err > 0);
	FLAC__ASSERT(n > 0);

	FLAC__ASSERT(n <= FLAC__MAX_BLOCK_SIZE);
	if(err <= n)
		return 0;
	/*
	 * The above two things tell us 1) n fits in 16 bits; 2) err/n > 1.
	 * These allow us later to know we won't lose too much precision in the
	 * fixed-point division (err<<fracbits)/n.
	 */

	fracbits = (8*sizeof(err)) - (FLAC__bitmath_ilog2(err)+1);

	err <<= fracbits;
	err /= n;
	/* err now holds err/n with fracbits fractional bits */

	/*
	 * Whittle err down to 16 bits max.  16 significant bits is enough for
	 * our purposes.
	 */
	FLAC__ASSERT(err > 0);
	bits = FLAC__bitmath_ilog2(err)+1;
	if(bits > 16) {
		err >>= (bits-16);
		fracbits -= (bits-16);
	}
	rbps = (FLAC__uint32)err;

	/* Multiply by fixed-point version of ln(2), with 16 fractional bits */
	rbps *= FLAC__FP_LN2;
	fracbits += 16;
	FLAC__ASSERT(fracbits >= 0);

	/* FLAC__fixedpoint_log2 requires fracbits%4 to be 0 */
	{
		const int f = fracbits & 3;
		if(f) {
			rbps >>= f;
			fracbits -= f;
		}
	}

	rbps = FLAC__fixedpoint_log2(rbps, fracbits, (uint32_t)(-1));

	if(rbps == 0)
		return 0;

	/*
	 * The return value must have 16 fractional bits.  Since the whole part
	 * of the base-2 log of a 32 bit number must fit in 5 bits, and fracbits
	 * must be >= -3, these assertion allows us to be able to shift rbps
	 * left if necessary to get 16 fracbits without losing any bits of the
	 * whole part of rbps.
	 *
	 * There is a slight chance due to accumulated error that the whole part
	 * will require 6 bits, so we use 6 in the assertion.  Really though as
	 * long as it fits in 13 bits (32 - (16 - (-3))) we are fine.
	 */
	FLAC__ASSERT((int)FLAC__bitmath_ilog2(rbps)+1 <= fracbits + 6);
	FLAC__ASSERT(fracbits >= -3);

	/* now shift the decimal point into place */
	if(fracbits < 16)
		return rbps << (16-fracbits);
	else if(fracbits > 16)
		return rbps >> (fracbits-16);
	else
		return rbps;
}

static FLAC__fixedpoint local__compute_rbps_wide_integerized(FLAC__uint64 err, FLAC__uint32 n)
{
	FLAC__uint32 rbps;
	uint32_t bits; /* the number of bits required to represent a number */
	int fracbits; /* the number of bits of rbps that comprise the fractional part */

	FLAC__ASSERT(sizeof(rbps) == sizeof(FLAC__fixedpoint));
	FLAC__ASSERT(err > 0);
	FLAC__ASSERT(n > 0);

	FLAC__ASSERT(n <= FLAC__MAX_BLOCK_SIZE);
	if(err <= n)
		return 0;
	/*
	 * The above two things tell us 1) n fits in 16 bits; 2) err/n > 1.
	 * These allow us later to know we won't lose too much precision in the
	 * fixed-point division (err<<fracbits)/n.
	 */

	fracbits = (8*sizeof(err)) - (FLAC__bitmath_ilog2_wide(err)+1);

	err <<= fracbits;
	err /= n;
	/* err now holds err/n with fracbits fractional bits */

	/*
	 * Whittle err down to 16 bits max.  16 significant bits is enough for
	 * our purposes.
	 */
	FLAC__ASSERT(err > 0);
	bits = FLAC__bitmath_ilog2_wide(err)+1;
	if(bits > 16) {
		err >>= (bits-16);
		fracbits -= (bits-16);
	}
	rbps = (FLAC__uint32)err;

	/* Multiply by fixed-point version of ln(2), with 16 fractional bits */
	rbps *= FLAC__FP_LN2;
	fracbits += 16;
	FLAC__ASSERT(fracbits >= 0);

	/* FLAC__fixedpoint_log2 requires fracbits%4 to be 0 */
	{
		const int f = fracbits & 3;
		if(f) {
			rbps >>= f;
			fracbits -= f;
		}
	}

	rbps = FLAC__fixedpoint_log2(rbps, fracbits, (uint32_t)(-1));

	if(rbps == 0)
		return 0;

	/*
	 * The return value must have 16 fractional bits.  Since the whole part
	 * of the base-2 log of a 32 bit number must fit in 5 bits, and fracbits
	 * must be >= -3, these assertion allows us to be able to shift rbps
	 * left if necessary to get 16 fracbits without losing any bits of the
	 * whole part of rbps.
	 *
	 * There is a slight chance due to accumulated error that the whole part
	 * will require 6 bits, so we use 6 in the assertion.  Really though as
	 * long as it fits in 13 bits (32 - (16 - (-3))) we are fine.
	 */
	FLAC__ASSERT((int)FLAC__bitmath_ilog2(rbps)+1 <= fracbits + 6);
	FLAC__ASSERT(fracbits >= -3);

	/* now shift the decimal point into place */
	if(fracbits < 16)
		return rbps << (16-fracbits);
	else if(fracbits > 16)
		return rbps >> (fracbits-16);
	else
		return rbps;
}
#endif

#ifndef FLAC__INTEGER_ONLY_LIBRARY
uint32_t FLAC__fixed_compute_best_predictor(const FLAC__int32 data[], uint32_t data_len, float residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#else
uint32_t FLAC__fixed_compute_best_predictor(const FLAC__int32 data[], uint32_t data_len, FLAC__fixedpoint residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#endif
{
	FLAC__uint32 total_error_0 = 0, total_error_1 = 0, total_error_2 = 0, total_error_3 = 0, total_error_4 = 0;
	uint32_t order;
#if 0
	/* This code has been around a long time, and was written when compilers weren't able
	 * to vectorize code. These days, compilers are better in optimizing the next block
	 * which is also much more readable
	 */
	FLAC__int32 last_error_0 = data[-1];
	FLAC__int32 last_error_1 = data[-1] - data[-2];
	FLAC__int32 last_error_2 = last_error_1 - (data[-2] - data[-3]);
	FLAC__int32 last_error_3 = last_error_2 - (data[-2] - 2*data[-3] + data[-4]);
	FLAC__int32 error, save;
	uint32_t i;
	/* total_error_* are 64-bits to avoid overflow when encoding
	 * erratic signals when the bits-per-sample and blocksize are
	 * large.
	 */
	for(i = 0; i < data_len; i++) {
		error  = data[i]     ; total_error_0 += local_abs(error);                      save = error;
		error -= last_error_0; total_error_1 += local_abs(error); last_error_0 = save; save = error;
		error -= last_error_1; total_error_2 += local_abs(error); last_error_1 = save; save = error;
		error -= last_error_2; total_error_3 += local_abs(error); last_error_2 = save; save = error;
		error -= last_error_3; total_error_4 += local_abs(error); last_error_3 = save;
	}
#else
	int i;
	for(i = 0; i < (int)data_len; i++) {
		total_error_0 += local_abs(data[i]);
		total_error_1 += local_abs(data[i] - data[i-1]);
		total_error_2 += local_abs(data[i] - 2 * data[i-1] + data[i-2]);
		total_error_3 += local_abs(data[i] - 3 * data[i-1] + 3 * data[i-2] - data[i-3]);
		total_error_4 += local_abs(data[i] - 4 * data[i-1] + 6 * data[i-2] - 4 * data[i-3] + data[i-4]);
	}
#endif


	/* prefer lower order */
	if(total_error_0 <= flac_min(flac_min(flac_min(total_error_1, total_error_2), total_error_3), total_error_4))
		order = 0;
	else if(total_error_1 <= flac_min(flac_min(total_error_2, total_error_3), total_error_4))
		order = 1;
	else if(total_error_2 <= flac_min(total_error_3, total_error_4))
		order = 2;
	else if(total_error_3 <= total_error_4)
		order = 3;
	else
		order = 4;

	/* Estimate the expected number of bits per residual signal sample. */
	/* 'total_error*' is linearly related to the variance of the residual */
	/* signal, so we use it directly to compute E(|x|) */
	FLAC__ASSERT(data_len > 0 || total_error_0 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_1 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_2 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_3 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_4 == 0);
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	residual_bits_per_sample[0] = (float)((total_error_0 > 0) ? log(M_LN2 * (double)total_error_0 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[1] = (float)((total_error_1 > 0) ? log(M_LN2 * (double)total_error_1 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[2] = (float)((total_error_2 > 0) ? log(M_LN2 * (double)total_error_2 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[3] = (float)((total_error_3 > 0) ? log(M_LN2 * (double)total_error_3 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[4] = (float)((total_error_4 > 0) ? log(M_LN2 * (double)total_error_4 / (double)data_len) / M_LN2 : 0.0);
#else
	residual_bits_per_sample[0] = (total_error_0 > 0) ? local__compute_rbps_integerized(total_error_0, data_len) : 0;
	residual_bits_per_sample[1] = (total_error_1 > 0) ? local__compute_rbps_integerized(total_error_1, data_len) : 0;
	residual_bits_per_sample[2] = (total_error_2 > 0) ? local__compute_rbps_integerized(total_error_2, data_len) : 0;
	residual_bits_per_sample[3] = (total_error_3 > 0) ? local__compute_rbps_integerized(total_error_3, data_len) : 0;
	residual_bits_per_sample[4] = (total_error_4 > 0) ? local__compute_rbps_integerized(total_error_4, data_len) : 0;
#endif

	return order;
}

#ifndef FLAC__INTEGER_ONLY_LIBRARY
uint32_t FLAC__fixed_compute_best_predictor_wide(const FLAC__int32 data[], uint32_t data_len, float residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#else
uint32_t FLAC__fixed_compute_best_predictor_wide(const FLAC__int32 data[], uint32_t data_len, FLAC__fixedpoint residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#endif
{
	FLAC__uint64 total_error_0 = 0, total_error_1 = 0, total_error_2 = 0, total_error_3 = 0, total_error_4 = 0;
	uint32_t order;
	int i;

	for(i = 0; i < (int)data_len; i++) {
		total_error_0 += local_abs(data[i]);
		total_error_1 += local_abs(data[i] - data[i-1]);
		total_error_2 += local_abs(data[i] - 2 * data[i-1] + data[i-2]);
		total_error_3 += local_abs(data[i] - 3 * data[i-1] + 3 * data[i-2] - data[i-3]);
		total_error_4 += local_abs(data[i] - 4 * data[i-1] + 6 * data[i-2] - 4 * data[i-3] + data[i-4]);
	}

	/* prefer lower order */
	if(total_error_0 <= flac_min(flac_min(flac_min(total_error_1, total_error_2), total_error_3), total_error_4))
		order = 0;
	else if(total_error_1 <= flac_min(flac_min(total_error_2, total_error_3), total_error_4))
		order = 1;
	else if(total_error_2 <= flac_min(total_error_3, total_error_4))
		order = 2;
	else if(total_error_3 <= total_error_4)
		order = 3;
	else
		order = 4;

	/* Estimate the expected number of bits per residual signal sample. */
	/* 'total_error*' is linearly related to the variance of the residual */
	/* signal, so we use it directly to compute E(|x|) */
	FLAC__ASSERT(data_len > 0 || total_error_0 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_1 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_2 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_3 == 0);
	FLAC__ASSERT(data_len > 0 || total_error_4 == 0);
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	residual_bits_per_sample[0] = (float)((total_error_0 > 0) ? log(M_LN2 * (double)total_error_0 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[1] = (float)((total_error_1 > 0) ? log(M_LN2 * (double)total_error_1 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[2] = (float)((total_error_2 > 0) ? log(M_LN2 * (double)total_error_2 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[3] = (float)((total_error_3 > 0) ? log(M_LN2 * (double)total_error_3 / (double)data_len) / M_LN2 : 0.0);
	residual_bits_per_sample[4] = (float)((total_error_4 > 0) ? log(M_LN2 * (double)total_error_4 / (double)data_len) / M_LN2 : 0.0);
#else
	residual_bits_per_sample[0] = (total_error_0 > 0) ? local__compute_rbps_wide_integerized(total_error_0, data_len) : 0;
	residual_bits_per_sample[1] = (total_error_1 > 0) ? local__compute_rbps_wide_integerized(total_error_1, data_len) : 0;
	residual_bits_per_sample[2] = (total_error_2 > 0) ? local__compute_rbps_wide_integerized(total_error_2, data_len) : 0;
	residual_bits_per_sample[3] = (total_error_3 > 0) ? local__compute_rbps_wide_integerized(total_error_3, data_len) : 0;
	residual_bits_per_sample[4] = (total_error_4 > 0) ? local__compute_rbps_wide_integerized(total_error_4, data_len) : 0;
#endif

	return order;
}

#ifndef FLAC__INTEGER_ONLY_LIBRARY
#define CHECK_ORDER_IS_VALID(macro_order)		\
if(order_##macro_order##_is_valid && total_error_##macro_order < smallest_error) { \
	order = macro_order;				\
	smallest_error = total_error_##macro_order ;	\
	residual_bits_per_sample[ macro_order ] = (float)((total_error_##macro_order > 0) ? log(M_LN2 * (double)total_error_##macro_order / (double)data_len) / M_LN2 : 0.0); \
}							\
else							\
	residual_bits_per_sample[ macro_order ] = 34.0f;
#else
#define CHECK_ORDER_IS_VALID(macro_order)		\
if(order_##macro_order##_is_valid && total_error_##macro_order < smallest_error) { \
	order = macro_order;				\
	smallest_error = total_error_##macro_order ;	\
	residual_bits_per_sample[ macro_order ] = (total_error_##macro_order > 0) ? local__compute_rbps_wide_integerized(total_error_##macro_order, data_len) : 0; \
}							\
else							\
	residual_bits_per_sample[ macro_order ] = 34 * FLAC__FP_ONE;
#endif


#ifndef FLAC__INTEGER_ONLY_LIBRARY
uint32_t FLAC__fixed_compute_best_predictor_limit_residual(const FLAC__int32 data[], uint32_t data_len, float residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#else
uint32_t FLAC__fixed_compute_best_predictor_limit_residual(const FLAC__int32 data[], uint32_t data_len, FLAC__fixedpoint residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#endif
{
	FLAC__uint64 total_error_0 = 0, total_error_1 = 0, total_error_2 = 0, total_error_3 = 0, total_error_4 = 0, smallest_error = UINT64_MAX;
	FLAC__uint64 error_0, error_1, error_2, error_3, error_4;
	FLAC__bool order_0_is_valid = true, order_1_is_valid = true, order_2_is_valid = true, order_3_is_valid = true, order_4_is_valid = true;
	uint32_t order = 0;
	int i;

	for(i = -4; i < (int)data_len; i++) {
		error_0 = local_abs64((FLAC__int64)data[i]);
		error_1 = (i > -4) ? local_abs64((FLAC__int64)data[i] - data[i-1]) : 0 ;
		error_2 = (i > -3) ? local_abs64((FLAC__int64)data[i] - 2 * (FLAC__int64)data[i-1] + data[i-2]) : 0;
		error_3 = (i > -2) ? local_abs64((FLAC__int64)data[i] - 3 * (FLAC__int64)data[i-1] + 3 * (FLAC__int64)data[i-2] - data[i-3]) : 0;
		error_4 = (i > -1) ? local_abs64((FLAC__int64)data[i] - 4 * (FLAC__int64)data[i-1] + 6 * (FLAC__int64)data[i-2] - 4 * (FLAC__int64)data[i-3] + data[i-4]) : 0;

		total_error_0 += error_0;
		total_error_1 += error_1;
		total_error_2 += error_2;
		total_error_3 += error_3;
		total_error_4 += error_4;

		/* residual must not be INT32_MIN because abs(INT32_MIN) is undefined */
		if(error_0 > INT32_MAX)
			order_0_is_valid = false;
		if(error_1 > INT32_MAX)
			order_1_is_valid = false;
		if(error_2 > INT32_MAX)
			order_2_is_valid = false;
		if(error_3 > INT32_MAX)
			order_3_is_valid = false;
		if(error_4 > INT32_MAX)
			order_4_is_valid = false;
	}

	CHECK_ORDER_IS_VALID(0);
	CHECK_ORDER_IS_VALID(1);
	CHECK_ORDER_IS_VALID(2);
	CHECK_ORDER_IS_VALID(3);
	CHECK_ORDER_IS_VALID(4);

	return order;
}

#ifndef FLAC__INTEGER_ONLY_LIBRARY
uint32_t FLAC__fixed_compute_best_predictor_limit_residual_33bit(const FLAC__int64 data[], uint32_t data_len, float residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#else
uint32_t FLAC__fixed_compute_best_predictor_limit_residual_33bit(const FLAC__int64 data[], uint32_t data_len, FLAC__fixedpoint residual_bits_per_sample[FLAC__MAX_FIXED_ORDER+1])
#endif
{
	FLAC__uint64 total_error_0 = 0, total_error_1 = 0, total_error_2 = 0, total_error_3 = 0, total_error_4 = 0, smallest_error = UINT64_MAX;
	FLAC__uint64 error_0, error_1, error_2, error_3, error_4;
	FLAC__bool order_0_is_valid = true, order_1_is_valid = true, order_2_is_valid = true, order_3_is_valid = true, order_4_is_valid = true;
	uint32_t order = 0;
	int i;

	for(i = -4; i < (int)data_len; i++) {
		error_0 = local_abs64(data[i]);
		error_1 = (i > -4) ? local_abs64(data[i] - data[i-1]) : 0 ;
		error_2 = (i > -3) ? local_abs64(data[i] - 2 * data[i-1] + data[i-2]) : 0;
		error_3 = (i > -2) ? local_abs64(data[i] - 3 * data[i-1] + 3 * data[i-2] - data[i-3]) : 0;
		error_4 = (i > -1) ? local_abs64(data[i] - 4 * data[i-1] + 6 * data[i-2] - 4 * data[i-3] + data[i-4]) : 0;

		total_error_0 += error_0;
		total_error_1 += error_1;
		total_error_2 += error_2;
		total_error_3 += error_3;
		total_error_4 += error_4;

		/* residual must not be INT32_MIN because abs(INT32_MIN) is undefined */
		if(error_0 > INT32_MAX)
			order_0_is_valid = false;
		if(error_1 > INT32_MAX)
			order_1_is_valid = false;
		if(error_2 > INT32_MAX)
			order_2_is_valid = false;
		if(error_3 > INT32_MAX)
			order_3_is_valid = false;
		if(error_4 > INT32_MAX)
			order_4_is_valid = false;
	}

	CHECK_ORDER_IS_VALID(0);
	CHECK_ORDER_IS_VALID(1);
	CHECK_ORDER_IS_VALID(2);
	CHECK_ORDER_IS_VALID(3);
	CHECK_ORDER_IS_VALID(4);

	return order;
}

void FLAC__fixed_compute_residual(const FLAC__int32 data[], uint32_t data_len, uint32_t order, FLAC__int32 residual[])
{
	const int idata_len = (int)data_len;
	int i;

	switch(order) {
		case 0:
			FLAC__ASSERT(sizeof(residual[0]) == sizeof(data[0]));
			memcpy(residual, data, sizeof(residual[0])*data_len);
			break;
		case 1:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - data[i-1];
			break;
		case 2:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - 2*data[i-1] + data[i-2];
			break;
		case 3:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - 3*data[i-1] + 3*data[i-2] - data[i-3];
			break;
		case 4:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - 4*data[i-1] + 6*data[i-2] - 4*data[i-3] + data[i-4];
			break;
		default:
			FLAC__ASSERT(0);
	}
}

void FLAC__fixed_compute_residual_wide(const FLAC__int32 data[], uint32_t data_len, uint32_t order, FLAC__int32 residual[])
{
	const int idata_len = (int)data_len;
	int i;

	switch(order) {
		case 0:
			FLAC__ASSERT(sizeof(residual[0]) == sizeof(data[0]));
			memcpy(residual, data, sizeof(residual[0])*data_len);
			break;
		case 1:
			for(i = 0; i < idata_len; i++)
				residual[i] = (FLAC__int64)data[i] - data[i-1];
			break;
		case 2:
			for(i = 0; i < idata_len; i++)
				residual[i] = (FLAC__int64)data[i] - 2*(FLAC__int64)data[i-1] + data[i-2];
			break;
		case 3:
			for(i = 0; i < idata_len; i++)
				residual[i] = (FLAC__int64)data[i] - 3*(FLAC__int64)data[i-1] + 3*(FLAC__int64)data[i-2] - data[i-3];
			break;
		case 4:
			for(i = 0; i < idata_len; i++)
				residual[i] = (FLAC__int64)data[i] - 4*(FLAC__int64)data[i-1] + 6*(FLAC__int64)data[i-2] - 4*(FLAC__int64)data[i-3] + data[i-4];
			break;
		default:
			FLAC__ASSERT(0);
	}
}

void FLAC__fixed_compute_residual_wide_33bit(const FLAC__int64 data[], uint32_t data_len, uint32_t order, FLAC__int32 residual[])
{
	const int idata_len = (int)data_len;
	int i;

	switch(order) {
		case 0:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i];
			break;
		case 1:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - data[i-1];
			break;
		case 2:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - 2*data[i-1] + data[i-2];
			break;
		case 3:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - 3*data[i-1] + 3*data[i-2] - data[i-3];
			break;
		case 4:
			for(i = 0; i < idata_len; i++)
				residual[i] = data[i] - 4*data[i-1] + 6*data[i-2] - 4*data[i-3] + data[i-4];
			break;
		default:
			FLAC__ASSERT(0);
	}
}

#if defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION) && !defined(FUZZING_BUILD_MODE_FLAC_SANITIZE_SIGNED_INTEGER_OVERFLOW)
/* The attribute below is to silence the undefined sanitizer of oss-fuzz.
 * Because fuzzing feeds bogus predictors and residual samples to the
 * decoder, having overflows in this section is unavoidable. Also,
 * because the calculated values are audio path only, there is no
 * potential for security problems */
__attribute__((no_sanitize("signed-integer-overflow")))
#endif
void FLAC__fixed_restore_signal(const FLAC__int32 residual[], uint32_t data_len, uint32_t order, FLAC__int32 data[])
{
	int i, idata_len = (int)data_len;

	switch(order) {
		case 0:
			FLAC__ASSERT(sizeof(residual[0]) == sizeof(data[0]));
			memcpy(data, residual, sizeof(residual[0])*data_len);
			break;
		case 1:
			for(i = 0; i < idata_len; i++)
				data[i] = residual[i] + data[i-1];
			break;
		case 2:
			for(i = 0; i < idata_len; i++)
				data[i] = residual[i] + 2*data[i-1] - data[i-2];
			break;
		case 3:
			for(i = 0; i < idata_len; i++)
				data[i] = residual[i] + 3*data[i-1] - 3*data[i-2] + data[i-3];
			break;
		case 4:
			for(i = 0; i < idata_len; i++)
				data[i] = residual[i] + 4*data[i-1] - 6*data[i-2] + 4*data[i-3] - data[i-4];
			break;
		default:
			FLAC__ASSERT(0);
	}
}

void FLAC__fixed_restore_signal_wide(const FLAC__int32 residual[], uint32_t data_len, uint32_t order, FLAC__int32 data[])
{
	int i, idata_len = (int)data_len;

	switch(order) {
		case 0:
			FLAC__ASSERT(sizeof(residual[0]) == sizeof(data[0]));
			memcpy(data, residual, sizeof(residual[0])*data_len);
			break;
		case 1:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + (FLAC__int64)data[i-1];
			break;
		case 2:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + 2*(FLAC__int64)data[i-1] - (FLAC__int64)data[i-2];
			break;
		case 3:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + 3*(FLAC__int64)data[i-1] - 3*(FLAC__int64)data[i-2] + (FLAC__int64)data[i-3];
			break;
		case 4:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + 4*(FLAC__int64)data[i-1] - 6*(FLAC__int64)data[i-2] + 4*(FLAC__int64)data[i-3] - (FLAC__int64)data[i-4];
			break;
		default:
			FLAC__ASSERT(0);
	}
}

#if defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION) && !defined(FUZZING_BUILD_MODE_FLAC_SANITIZE_SIGNED_INTEGER_OVERFLOW)
/* The attribute below is to silence the undefined sanitizer of oss-fuzz.
 * Because fuzzing feeds bogus predictors and residual samples to the
 * decoder, having overflows in this section is unavoidable. Also,
 * because the calculated values are audio path only, there is no
 * potential for security problems */
__attribute__((no_sanitize("signed-integer-overflow")))
#endif
void FLAC__fixed_restore_signal_wide_33bit(const FLAC__int32 residual[], uint32_t data_len, uint32_t order, FLAC__int64 data[])
{
	int i, idata_len = (int)data_len;

	switch(order) {
		case 0:
			for(i = 0; i < idata_len; i++)
				data[i] = residual[i];
			break;
		case 1:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + data[i-1];
			break;
		case 2:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + 2*data[i-1] - data[i-2];
			break;
		case 3:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + 3*data[i-1] - 3*data[i-2] + data[i-3];
			break;
		case 4:
			for(i = 0; i < idata_len; i++)
				data[i] = (FLAC__int64)residual[i] + 4*data[i-1] - 6*data[i-2] + 4*data[i-3] - data[i-4];
			break;
		default:
			FLAC__ASSERT(0);
	}
}
