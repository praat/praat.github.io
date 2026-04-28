#ifndef _f2cP_h_
#define _f2cP_h_
/* f2cP.h
 *
 * Copyright (C) 2020 David Weenink, 2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include "cblas.h"

static inline double d_abs (double *x) {
	return abs (*x);
}

static inline double d_acos (double *x) {
	return acos (*x);
}

static inline double d_asin (double *x) {
	return asin (*x);
}

static inline double d_atan (double *x) {
	return atan (*x);
}

static inline double d_atn2 (double *x, double *y) {
	return atan2 (*x,*y);
}

static inline double d_cos (double *x) {
	return cos (*x);
}

static inline double d_cosh (double *x) {
	return cosh (*x);
}

static inline double d_dim (double *a, double *b) {
	return ( *a > *b ? *a - *b : 0 );
}

static inline double d_exp (double *x) {
	return exp (*x);
}

static inline double d_int (double *x) {
	return ( *x>0 ? floor(*x) : -floor(- *x) );
}

static inline double d_lg10 (double *x) {
	return log10 (*x);
}

static inline double d_log (double *x) {
	return log (*x);
}

// TODO djmw is this std::fmod (*x, *y) ??
static inline double d_mod (double *x, double *y) {
#ifdef IEEE_drem
	double ya = *y;
	if (ya < 0.0)
		ya = -ya;
	double xa = *x;
	double z = drem (xa, ya);
	if (xa > 0) {
		if (z < 0)
			z += ya;
		}
	else if (z > 0)
		z -= ya;
	return z;
#else
	double quotient = *x / *y;
	if (quotient >= 0)
		quotient = floor (quotient);
	else
		quotient = -floor(-quotient);
	return (*x - (*y) * quotient );
#endif
}

static inline double d_prod (float *x, float *y) {
	return  (*x) * (*y);
}

static inline double d_sign (double *a, double *b) {
	double x = (*a >= 0 ? *a : - *a);
	return ( *b >= 0 ? x : -x );
}

static inline double d_sin (double *x) {
	return sin (*x);
}

static inline double d_sinh (double *x) {
	return sinh (*x);
}

static inline double d_sqrt (double *x) {
	return sqrt (*x);
}

static inline double d_tan (double *x) {
	return tan (*x);
}

static inline double d_tanh (double *x) {
	return tanh (*x);
}

static inline double derf_ (double *x) {
	return erf (*x);
}

static inline double derfc_ (double *x) {
	return erfc (*x);
}

static inline integer i_dnnt (double *x) {
	return (integer)(*x >= 0. ? floor (*x + .5) : -floor(.5 - *x));
}

static inline integer i_nint (float *x) {
	return (integer)(*x >= 0.0 ? floor(*x + 0.5) : -floor(0.5 - *x));
}

bool lsame_(const char *ca, const char *cb);

bool lsamen_(integer *n, const char *ca, const char *cb);

static inline double pow_dd (double *ap, double *bp) {
	return pow (*ap, *bp);
}

static inline double pow_di (double *ap, integer *bp) {
	double pow = 1.0;
	double x = *ap;
	integer n = *bp;
	if (n != 0) {
		if (n < 0) {
			n = -n;
			x = 1.0 / x;
		}
		for(unsigned long u = n; ; ) {
			if (u & 01)
				pow *= x;
			if (u >>= 1)
				x *= x;
			else
				break;
		}
	}
	return pow;
}

static inline integer pow_ii (integer *ap, integer *bp) {
	integer pow, x = *ap, n = *bp;

	if (n <= 0) {
		if (n == 0 || x == 1)
			return 1;
		if (x != -1)
			return x == 0 ? 1/x : 0;
		n = -n;
	}
	unsigned long u = n;
	for (pow = 1; ; ) {
		if(u & 01)
			pow *= x;
		if(u >>= 1)
			x *= x;
		else
			break;
	}
	return pow;
}

static inline double pow_ri (float *ap, integer *bp) {
	double pow = 1.0, x = *ap;
	integer n = *bp;
	if (n != 0) {
		if (n < 0) {
			n = -n;
			x = 1/x;
		}
		for (unsigned long u = n; ; ) {
			if(u & 01)
				pow *= x;
			if(u >>= 1)
				x *= x;
			else
				break;
			}
		}
	return pow;
}

/*
	Unless compiled with -DNO_OVERWRITE, this variant of s_copy allows the
	target of an assignment to appear on its right-hand side (contrary
	to the Fortran 77 Standard, but in accordance with Fortran 90), as in  a(2:5) = a(4:7).
 */
static inline void s_copy (char *a, const char *b, integer la, integer lb) {
	char *aend = a + la;
	if(la <= lb)
#ifndef NO_OVERWRITE
		if (a <= b || a >= b + la)
#endif
			while(a < aend)
				*a++ = *b++;
#ifndef NO_OVERWRITE
		else
			for(b += la; a < aend; )
				*--aend = *--b;
#endif

	else {
		char *bend = const_cast<char *>(b) + lb;
#ifndef NO_OVERWRITE
		if (a <= b || a >= bend)
#endif
			while(b < bend)
				*a++ = *b++;
#ifndef NO_OVERWRITE
		else {
			a += lb;
			while(b < bend)
				*--a = *--bend;
			a += lb;
		}
#endif
		while(a < aend)
			*a++ = ' ';
	}
}

static inline void s_cat (char *lp, char *rpp[], integer rnp [], integer *np, integer ll) {
	char *rp;
	integer n = *np;
#ifndef NO_OVERWRITE
	integer L, m;
	char *lp0, *lp1;

	lp0 = 0;
	lp1 = lp;
	L = ll;
	integer i = 0;
	while (i < n) {
		rp = rpp [i];
		m = rnp [i++];
		if (rp >= lp1 || rp + m <= lp) {
			if ((L -= m) <= 0) {
				n = i;
				break;
			}
			lp1 += m;
			continue;
		}
		lp0 = lp;
		lp = lp1 = Melder_malloc_f (char, L = ll);
		break;
	}
	lp1 = lp;
#endif /* NO_OVERWRITE */
	for(i = 0 ; i < n ; ++ i) {
		integer nc = ll;
		if(rnp [i] < nc)
			nc = rnp [i];
		ll -= nc;
		rp = rpp [i];
		while(--nc >= 0)
			*lp++ = *rp++;
	}
	while(--ll >= 0)
		*lp++ = ' ';
#ifndef NO_OVERWRITE
	if (lp0) {
		memcpy(lp0, lp1, L);
		Melder_free (lp1);
	}
#endif
}

static inline integer s_cmp (const char *a0, const char *b0, integer la, integer lb) {
	unsigned char *a = (unsigned char *)a0;
	unsigned char *b = (unsigned char *)b0;
	unsigned char *aend = a + la;
	unsigned char *bend = b + lb;
	if(la <= lb) {
		while(a < aend)
			if(*a != *b)
				return( *a - *b );
			else {
				++a;
				++b;
			}
		while(b < bend)
			if(*b != ' ')
				return( ' ' - *b );
			else	++b;
	}
	return 0;
}

static inline void xerbla_ (const char *src, integer *info) {
	Melder_throw (Melder_peek8to32_u (src), U": parameter ", *info, U" not correct!");
}

#endif /* _f2cP_h_  */
