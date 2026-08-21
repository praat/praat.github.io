#ifndef _NUM2_h_
#define _NUM2_h_
/* NUM2.h
 *
 * Copyright (C) 1997-2025, 2026 David Weenink
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

#include "ExtendedReal.h"
#include "MAT_numerics.h"
#include "NUMsorting.h"
#include "NUMFourier.h"

/*
	A typical iteration goes like
	do {
		previous is ...
		get value of current
	while (fabs (current - previous) > fabs (NUMeps * current);
	Because of floating point arithmetic we have a rounding differences which are large or equal to the
	machine precision eps = 2.2...e-16. To have an escape if fabs (current - previous) e<= eps we choose
	NUMeps just a little larger than eps.
*/
#define NUMeps 2.3e-16

int NUMcompareTwoIntegers (integer& a, integer& b);
int NUMcompareTwoDoubles (double& a, double& b);
/*
	return a < b ? -1 : a > b ? 1 : 0;
*/

/*
	only used to append the time info to the name of an object.
	1.2345678 -> 1_2345678
	1.2       -> 1_2
	2         -> 2
*/
char32 * NUMnumber_as_stringWithDotReplacedByUnderscore (double time);

regexp *NUMregexp_compile (conststring32 regexp);
/* Compiles a regular expression to a datastructure used by the regexp engine */

char32 *strstr_regexp (conststring32 string, conststring32 search_regexp);
/*
	Returns a pointer to the first occurrence in 'string' of the
	regular expression 'searchRE'. It returns a null pointer if
	no match is found.
*/

inline integer isqrt_roundUp (double number) {
	return ceil (sqrt (number));
}

autoSTRVEC string32vector_searchAndReplace (constSTRVEC me,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *nmatches, integer *nstringmatches, bool use_regexp);
/*
	Searches and replaces in string array of strings.
	If use_regexp != 0, 'search' and 'replace' will be interpreted
	as regular expressions. Else these strings are taken literally.

	'maximumNumberOfReplaces' is the maximum number of replaces in EACH string
	in the array of strings (you can replace ALL occurrences by making this
	number <= 0).
	The totalnumber of matches found is returned in 'nmatches'.
	The number of strings with at least one match is returned in
	'nstringmatches'.
*/

void MATprintMatlabForm (constMATVU const& m, conststring32 name);
/*
	Print a matrix in a form that can be used as input for octave/matlab.
							1 2 3
	Let A be the matrix:	4 5 6
							7 8 9
	The output from MATprintMatlabForm (A, "M") will be
	M= [1, 2, 3;
	    4, 5, 6;
	    7, 8, 9];
*/

inline bool NUMisNonNegative (constVECVU const&  vec) {
	for (integer i = 1; i <= vec.size; i ++)
		if (vec [i] < 0.0)
			return false;
	return true;
}
inline bool NUMisNonNegative (constMATVU const& mat) {
	for (integer irow = 1; irow <= mat.nrow; irow ++)
		for (integer icol = 1; icol <= mat.ncol; icol ++)
			if (mat [irow] [icol] < 0.0)
				return false;
	return true;
}

inline integer NUMmaxPos (constVECVU const& v) {
	if (NUMisEmpty (v)) return 0;
	integer index = 1;
	double maximum = v [1];
	for (integer i = 2; i <= v.size; i ++) {
		if (v [i] > maximum) {
			maximum = v [i];
			index = i;
		}
	}
	return index;	
}

#if 0
template <typename T>
inline T NUMmaxPos (constvectorview<T> const& v) {
	if (NUMisEmpty (v)) return 0;
	integer index = 1;
	T maximum = v [1];
	for (integer i = 2; i <= v.size; i ++) {
		if (v [i] > maximum) {
			maximum = v [i];
			index = i;
		}
	}
	return index;
}
#endif

inline integer NUMmaxPos (constINTVECVU const& v) {
	if (NUMisEmpty (v)) return 0;
	integer index = 1;
	integer maximum = v [1];
	for (integer i = 2; i <= v.size; i ++) {
		if (v [i] > maximum) {
			maximum = v [i];
			index = i;
		}
	}
	return index;	
}

inline integer NUMminPos (constVECVU const& v) {
	if (NUMisEmpty (v)) return 0;
	integer index = 1;
	double minimum = v [1];
	for (integer i = 2; i <= v.size; i ++) {
		if (v [i] < minimum) {
			minimum = v [i];
			index = i;
		}
	}
	return index;	
}

inline integer NUMminPos (constINTVECVU const& v) {
	if (NUMisEmpty (v))
		return 0;
	integer index = 1;
	integer minimum = v [1];
	for (integer i = 2; i <= v.size; i ++) {
		if (v [i] < minimum) {
			minimum = v [i];
			index = i;
		}
	}
	return index;	
}

inline void NUMextrema_e (constVECVU const& x, double *out_minimum, double *out_maximum) {
	if (out_minimum)
		*out_minimum = NUMmin_e (x);
	if (out_maximum)
		*out_maximum = NUMmax_e (x);
}

inline void NUMextrema_u (constVECVU const& x, double *out_minimum, double *out_maximum) {
	if (out_minimum)
		*out_minimum = NUMmin_u (x);
	if (out_maximum)
		*out_maximum = NUMmax_u (x);
}

/*
	Clip array values.
	c [i] = c [i] < min ? min : (c [i] > max ? max : c [i])
*/
inline void VECclip_inplace (double min, VECVU const& x, double max) {
	for (integer i = 1; i <= x.size; i ++)
		Melder_clip (min, & x [i], max);
}

inline bool NUMhasZeroElement (constMATVU const m) {
	for (integer irow = 1; irow <= m.nrow; irow ++)
		for (integer icol = 1; icol <= m.ncol; icol++)
			if (m [irow] [icol] == 0.0)
				return true;
	return false;
}

inline integer NUMcountNumberOfNonZeroElements (constVECVU const& v) {
	integer count = 0;
	for (integer i = 1; i <= v.size; i ++)
		if (v [i] != 0.0)
			++ count;
	return count;
}

inline double NUMmul (constVECVU const& x, constMATVU const& m, constVECVU const& y) { // x'. M . y
	Melder_assert (x.size == m.nrow);
	Melder_assert (y.size == m.ncol);
	longdouble result = 0.0;
	for (integer k = 1; k <= x.size; k ++)
		result += x [k] * NUMinner (m.row (k), y);
	return (double) result;
}	

inline autoVEC VECnorm_rows (constMATVU const& x, double power) {
	autoVEC norm = raw_VEC (x.nrow);
	for (integer irow = 1; irow <= norm.size; irow ++)
		norm [irow] = NUMnorm (x.row (irow), power);
	return norm;
}

inline void VECnormalize_inplace (VECVU const& vec, double power, double newNorm) {
	Melder_assert (newNorm > 0.0);
	double oldnorm = NUMnorm (vec, power);
	if (oldnorm > 0.0)
		vec  *=  newNorm / oldnorm;
}

inline void MATnormalize_inplace (MATVU const& mat, double power, double newNorm) {
	Melder_assert (newNorm > 0.0);
	double oldnorm = NUMnorm (mat, power);
	if (oldnorm > 0.0)
		mat  *=  newNorm / oldnorm;
}

inline void MATnormalizeRows_inplace (MATVU const& a, double power, double norm) {
	Melder_assert (norm > 0.0);
	for (integer irow = 1; irow <= a.nrow; irow ++)
		VECnormalize_inplace (a.row (irow), power, norm);
}

inline void MATnormalizeColumns_inplace (MATVU const& a, double power, double norm) {
	MATnormalizeRows_inplace (a.transpose(), power, norm);
}
/*
	Scale a [.] [j] such that sqrt (Sum(a [i] [j]^2, i=1..nPoints)) = norm.
*/

void VECsmoothByMovingAverage_preallocated (VECVU const& out, constVECVU const& in, integer window);

autoMAT MATcovarianceFromColumnCentredMatrix (constMATVU const& x, integer ndf);
/*
	Calculate covariance matrix(ncols x ncols) from data matrix (nrows x ncols);
	The matrix x must be column centered.
	covar [i] [j] = sum (k=1..nrows, x [i] [k] * x [k] [j]) / (nrows - ndf)
*/

void MATmtm_weighRows (MATVU const& result, constMATVU const& data, constVECVU const& rowWeights);

inline autoMAT newMATmtm_weighRows (constMATVU const& data, constVECVU const& rowWeights) {
	autoMAT result = raw_MAT (data.ncol, data.ncol);
	MATmtm_weighRows (result.get(), data, rowWeights);
	return result;
}

double NUMmultivariateKurtosis (constMATVU const& x, integer method);
/*
	calculate multivariate kurtosis.
	method = 1 : Schott (2001), J. of Statistical planning and Inference 94, 25-36.
*/

void NUMmad (constVEC x, double *inout_location, bool wantlocation, double *out_mad, VEC const& workSpace);
/*
	Computes the median absolute deviation, i.e., the median of the
	absolute deviations from the median, and adjust by a factor for
	asymptotically normal consistency, i.e. the returned value is 1.4826*mad which
	makes the returned value "equal" to the standard deviation if the data is normally distributed.
	You either GIVE the median location (if wantlocation = false) or it
	will be calculated (if wantlocation = true);
	
	Precondition: workSpace.size >= x.size
 */

void NUMstatistics_huber (constVEC x, double *inout_location, bool wantlocation, double *inout_scale, bool wantscale, double k_stdev, double tol, integer maximumNumberOfiterations, VEC const& workSpace);
/*
	Finds the Huber M-estimator for location with scale specified,
	scale with location specified, or both if neither is specified.
	k_stdev Winsorizes at `k_stdev' standard deviations.
	
	Precondition: workSpace.size >= x.size
*/

autoVEC newVECmonotoneRegression (constVEC x);
/*
	Find numbers xs [1..n] that have a monotone relationship with
	the numbers in x [1..n].
	The xs [i] will be ascending.
*/

autoMAT newMATlowerCholeslyInverse_fromLowerCholesky (constMAT const& m);

/* 
	Pre: matrices must be square.
*/
autoMAT newMATsymmetricPositiveDefinte_inverse (constMAT m);
autoMAT newMATsymmetric_inverse (constMAT m);
autoMAT newMATinverse (constMAT m);

void MATlowerCholesky_inplace (MAT a, double *out_lnd);

autoMAT newMATlowerCholesky (constMATVU const& a, double *out_lnd);

void MATlowerCholeskyInverse_inplace (MAT a, double *out_lnd);

inline autoMAT newMATlowerCholeskyInverse (constMAT const& a) {
	autoMAT result = copy_MAT (a);
	MATlowerCholeskyInverse_inplace (result.get(), nullptr);
	return result;
}
/*
	Calculates L^-1, where A = L.L' is a symmetric positive definite matrix
	and ln(determinant). L^-1 in lower, leave upper part intact.
*/

autoMAT newMATinverse_fromLowerCholeskyInverse (constMAT m);
/*
	Return the complete matrix inverse when only the inverse of the lower Cholesky part is given.
	Input m is a square matrix, in the lower part is the inverse of the lower Cholesky part as calculated by NUMlowerCholeskyInverse.
*/

double NUMdeterminant_fromSymmetricMatrix (constMAT m);
/*
	ln(determinant) of a symmetric p.s.d. matrix
*/

double NUMmahalanobisDistanceSquared (constMAT lowerInverse, constVEC v, constVEC m);
/*
	Calculates squared Mahalanobis distance: (v-m)'S^-1(v-m).
	Input matrix is the inverse L^-1 of the Cholesky decomposition S = L.L'
	as calculated by NUMlowerCholeskyInverse or 1-row for a diagonal matrix (nr =1)
	Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
		(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
			(L**-1.(x-m))' . (L**-1.(x-m))
*/

double NUMtrace (constMATVU const& a);
double NUMtrace2 (constMATVU const& x, constMATVU const& y);
/*
	Calculates the trace from a product matrix x*y
*/

void MATprojectColumnsOnEigenspace_preallocated (MAT projection, constMATVU const& data, constMATVU const& eigenvectors);
/* Input:
 	data [dimension, numberOfColumns]
 		contains the column vectors to be projected on the eigenspace.
  eigenvectors [numberOfEigenvectors] [dimension]
 		the eigenvectors stored as rowvectors
 Input/Output
 	projection [numberOfEigenvectors, numberOfColumns] 
 		the projected vectors from 'data'
 
 Project the columnvectors in matrix 'data' along the 'numberOfEigenvectors' eigenvectors into the matrix 'projection'.
*/


double VECdominantEigenvector_inplace (VEC inout_q, constMAT m, double tolerance);
/*
	Determines the first dominant eigenvector from a square GENERAL matrix m.
	Besides the matrix m, a first guess for the eigenvector q must
	be supplied (e.g. 1,0,...,0) and a value for tolerance (iteration
	stops when fabs(lamda [k] - lambda [k-1]) <= tolerance, where lamda [k] is
	the eigenvalue at the k-th iteration step.
	The methos is described in:
	G. Golub & C. van Loan (1996), Matrix computations, third edition,
	The Johns Hopkins University Press Ltd.,
	London, (Par. 7.3.1 The Power Method)
*/

integer NUMsolveQuadraticEquation (double a, double b, double c, double *x1, double *x2);
/*
	Finds the real roots of ax^2 + bx + c = 0.
	The number of real roots is returned and their locations in x1 and x2.
	If only one root found it is stored in x1.
	If no roots found then x1 and x2 will not be changed.
*/

autoVEC solve_VEC (constMATVU const& a, constVECVU const& b, double tol);
/*
	Solve the equation: A.x = b for x;
	a [1..nr] [1..nc], b [1..nr] and the unknown x [1..nc]
	Algorithm: s.v.d.
*/

autoMAT solve_MAT (constMATVU const& a, constMATVU const& b, double tol);
/*
	Solve the equations: A.X = B;
	a [1..nr] [1..nc], b [1..nr] [1..nc2] and the unknown x [1..nc] [1..nc2]
	Algorithm: s.v.d.
*/


/*
	Solve y = D.x + e for x, where x is sparse and e is observation noise.
	Minimize the 2-norm (y - D.x), where maximally K elements of x may be non-zero, by an iterative hard thresholding algorithm.
	D is a MxN real matrix with (many) more columns than rows, i.e. N > M. We need to find a vector x
	with maximally K non-zero elements (sparse).
	The algorithm is described in T. Blumensath & M.E. Davies (2010): "Normalised iterative hard thresholding;
	guaranteed stability and performance", IEEE Journal of Selected Topics in Signal Processing #4: 298-309.
	x in/out: the start value (you typically would start the iteration with all zeros).
*/
void VECsolveSparse_IHT (VECVU const& x, constMATVU const& d, constVECVU const& y, integer numberOfNonZeros, integer maximumNumberOfIterations, double tolerance, integer infoLevel);
autoVEC solveSparse_IHT_VEC (constMATVU const& d, constVECVU const& y, integer numberOfNonZeros, integer maximumNumberOfIterations, double tolerance, integer infoLevel);

void VECsolveNonnegativeLeastSquaresRegression (VECVU const& result, constMATVU const& m, constVECVU const& y, integer itermax, double tol, integer infoLevel);

inline autoVEC solveNonnegativeLeastSquaresRegression_VEC (constMATVU const& a, constVECVU const& y, integer itermax, double tol, integer infoLevel) {
	autoVEC result = zero_VEC (a.ncol);
	VECsolveNonnegativeLeastSquaresRegression (result.get(), a, y, itermax, tol, infoLevel);
	return result;
}
/*
	Solve the equation: A.x = y for x under the constraint: all x [i] >= 0;
	a [1..nr][1..nc], y [1..nr] and x [1..nc].
	Algorithm: Alternating least squares.
	Borg & Groenen (1997), Modern multidimensional scaling, Springer, page 180.
*/

void NUMsolveConstrainedLSQuadraticRegression (constMAT const& x, constVEC const& y, double *out_alpha, double *out_gamma);
/*
	Solve y [i] = alpha + beta * x [i] + gamma * x [i]^2, with i = 1..n,
	subject to the constraint beta^2 = 4 * alpha * gamma, for alpha and
	gamma (Least Squares).
	The input Vandermonde-matrix o [1..n,1..3] has columns with 1, x [i] and
	x [i]^2, respectively.
	The algorithm is according to:
	Jos M.F. Ten Berge (1983), A generalization of Verhelst's solution for
	a constrained regression problem in ALSCAL and related MDS-algorithms,
	Psychometrika 48, 631-638.
*/

autoVEC solveWeaklyConstrainedLinearRegression_VEC (constMAT const& a, constVEC const& y, double alpha, double delta);
/*
	Solve g(x) = ||A*x - y||^2 + alpha (x'*x - delta)^2 for x [1..m],
	where A [1..n] [1..m] is a matrix, y [1..n] a given vector, and alpha
	and delta are fixed numbers.
	This class of functions is composed of a linear regression function and
	a penalty function for the sum of squared regression weights. It is weakly
	constrained because the penalty function prohibits a relatively large
	departure of x'x from delta.
	The solution is due to:
	Jos M.F. ten Berge (1991), A general solution for a class of weakly
	constrained linear regression problems, Psychometrika 56, 601-609.
	Preconditions:
		a.nrow >= a.ncol
		alpha >= 0
*/

void NUMprocrustes (constMATVU const& x, constMATVU const& y, autoMAT *out_rotation, autoVEC *out_translation, double *out_scale);
/*
	Given two configurations x and y (nPoints x nDimensions), find the
	the Procrustes rotation/reflection matrix T, the translation vector v and the scaling
	factor s such that Y = sXT+1v' (1 is the nPoints vector with ones).
	Solution: see Borg and Groenen (1997), Modern Multidimensional Scaling, pp 340-346.
	When out_translation == nullptr or out_scale == nullptr, only the matrix T will be solved for:
	the orthogonal Procrustes transform.
*/

double NUMnrbis (double (*f)(double x, double *dfx, void *closure), double xmin, double xmax, void *closure);
/*
	Find the root of a function between xmin and xmax.
	Method: Newton-Raphson with bisection (i.e., derivative is known!).
	Error condition:
		return undefined if root not bracketed.
*/

double NUMridders (double (*f) (double x, void *closure), double xmin, double xmax, void *closure);
/*
	Return the root of a function f bracketed in [xmin, xmax].
	Error condition:
		root not bracketed.
*/

double NUMmspline (constVEC const & knot, integer order, integer i, double x);
/*
	Calculates an M-spline for a knot sequence.
	After Ramsay (1988), Monotone splines in action, Statistical Science 4.

	M-splines of order k have degree k-1.
	M-splines are zero outside interval [ knot [i], knot [i+order] ).
	First and last 'order' knots are equal, i.e.,
	knot [1] = ... = knot [order] && knot [nKnots-order+1] = ... knot [nKnots].
	Error condition: no memory.
*/

double NUMispline (constVEC const & aknot, integer order, integer i, double x);
/*
	Calculates an I-spline for simple knot sequences: only one knot at each
	interior boundary.
	After Ramsay (1988), Monotone splines in action, Statistical Science 4.

	I-splines of order k have degree k (because they Integrate an M-spline
	of degree k-1).
	In the calculation of the integral of M(x|k,t), M-splines are used that
	have two more knots, i.e., M(x|k+1,t). For reasons of efficiency we
	demand that these extra knots are given, i.e., the 'aknot []' argument
	contains the knot positions as if the spline to be integrated were an
	M(x|k+1,t) spline.
	knot [1] = ... = knot [order+1] && knot [nKnots-order] = ... knot [nKnots]
	Error condition: no memory.
*/

double NUMwilksLambda (constVEC const& lambda, integer from, integer to);
/*
	Calculate: Product (i=from..to; 1/(1+lambda [i]))
	Preconditions: to >= from
*/

double NUMlnBeta (double a, double b);
/*
	Computes the logarithm of the beta function log(B(a,b) subject to
	a and b not being negative integers.
*/

double NUMbeta2 (double z, double w);//temporarily

double NUMbetaContinuedFraction(double a, double b, double x);

double NUMfactln (integer n);
/* Returns ln (n!) */

void NUMlngamma_complex (double zr, double zi, double *out_lnr, double *out_arg);
/* Log [Gamma(z)] for z complex, z not a negative integer
 * Uses complex Lanczos method. Note that the phase part (arg)
 * is not well-determined when |z| is very large, due
 * to inevitable roundoff in restricting to (-pi, pi].
 * The absolute value part (lnr), however, never suffers.
 *
 * Calculates:
 *   lnr = log|Gamma(z)|
 *   arg = arg(Gamma(z))  in (-Pi, Pi]
 */

/***** STATISTICS: PROBABILITY DENSITY FUNCTIONS ********************/

double NUMlogNormalP (double x, double zeta, double sigma);
/* Area under log normal from 0 to x */

double NUMlogNormalQ (double x, double zeta, double sigma);
/* Area under log normal from x to +infinity */

double NUMstudentP (double t, double df);
/*
	The area under the student T-distribution from -infinity to t.
	Precondition: df > 0
*/

double NUMstudentQ (double t, double df);
/*
	The area under the student T distribution from t to +infinity.
	Precondition: df > 0
*/

double NUMfisherP (double f, double df1, double df2);
/*
	The area under Fisher's F-distribution from 0 to f
	Preconditions: f >= 0, df1 > 0, df2 > 0
*/

double NUMfisherQ (double f, double df1, double df2);
/*
	The area under Fisher's F-distribution from f to +infinity
	Preconditions: f >= 0, df1 > 0, df2 > 0
*/

double NUMinvGaussQ (double p);
/*
	Solves NUMgaussQ (x) == p for x, given p.
	Precondition: 0 < p < 1
	Method: Abramovitz & Stegun 26.2.23
	Precision: |eps(p)| < 4.5 10^-4
*/

double NUMinvChiSquareQ (double p, double df);
/*
	Solves NUMchiSquareQ (chiSquare, df) == p for chiSquare, given p, df.
	Preconditions: 0 < p < 1, df > 0
*/

double NUMinvStudentQ (double p, double df);
/*
	Solves NUMstudentQ (t, df) == p for t, given p, df.
	Preconditions: 0 < p < 1, df > 0
*/

double NUMinvFisherQ (double p, double df1, double df2);
/*
	Solves NUMfisherQ (f, df1, df2) == p for f, given p, df1, df2
	Precondition: 0 < p < 1
*/

double NUMtukeyQ (double q, double cc, double df, double rr);
/*	Computes the probability that the maximum of rr studentized
 *	ranges, each based on cc means and with df degrees of freedom
 *	for the standard error, is larger than q.
 */

double NUMinvTukeyQ (double p, double cc, double df, double rr);
/* Solves NUMtukeyQ (q, rr, cc, df) == p for q given p, rr, cc and df.
 * Computes the quantiles of the maximum of rr studentized
 * ranges, each based on cc means and with df degrees of freedom
 * for the standard error, is larger than q.
 *   p = probability (alpha)
 *  rr = no. of rows or groups
 *  cc = no. of columns or treatments
 *  df = degrees of freedom of error term
 */


/******  Frequency in Hz to other frequency reps ****/

double NUMmelToHertz2 (double mel);
/*
	Return 700 * (pow (10.0, mel / 2595.0) - 1)
*/

double NUMhertzToMel2 (double f);
/*
	Return 2595 * log10 (1 + f/700)
*/

double NUMmelToHertz3 (double mel);
/*
	Return mel < 1000 ? mel : 1000 * (exp (mel * log10(2) / 1000) - 1)
*/

double NUMhertzToMel3 (double hz);
/*
	Return hz < 1000 ? hz : 1000 * log10 (1 + hz / 1000) / log10 (2)
*/

double NUMbarkToHertz2 (double bark);
/*
	Return 650 * sinh (bark / 7)
*/

double NUMhertzToBark2 (double hz);
/*
	Return 7 * ln (hz / 650 + sqrt(1 + (hz / 650)^2))
*/

double NUMhertzToBark_traunmueller (double hz);
/*
	return 26.81 * hz /(1960 + hz) -0.53;
*/

double NUMbarkToHertz_traunmueller (double bark);
/*
	return 1960* (bark + 0.53) / (26.28 - bark);
*/

double NUMbarkToHertz_zwickerterhardt (double hz);
/*
	return 13 * atan (0.00076 * hz) + 3.5 * atan (hz / 7500);
*/

double NUMbladonlindblomfilter_amplitude (double zc, double z);
/*
	Amplitude of filter at dz (barks) from centre frequency.
	dz may be positive and negative.

	The bladonlindblomfilter function is:

	z' = zc - z + 0.474
	10 log10 F(z') = 15.81 + 7.5 z' - 17.5 sqrt( 1 + z'^2 )

	Reference: Bladon, R.A.W & Lindblom, B., (1980),
	"Modeling the judgment of vowel quality differences", JASA 69, 1414-1422.
	The filter has a bandwidth of 1.43 Bark, the maximum occurs at z = zc,
	and the slopes are -10 dB/Bark and +25 dB/Bark.
 */

double NUMsekeyhansonfilter_amplitude (double zc, double z);
/*
	Amplitude of filter at dz (barks) from centre frequency.
	dz may be positive and negative.

	The sekeyhansonfilter function is:
	z' = zc - z - 0.215
	10 log10 F(z') = 7 - 7.5 * z' - 17.5 * sqrt( 0.196 + z'^2 )

	Reference: Sekey, A. & Hanson, B.A. (1984),
	"Improved 1-Bark bandwidth auditory filter", JASA 75, 1902-1904.
	The filter function has a bandwidth of 1 Bark, the maximum response
	occurs at z=zc, and the slopes are +10 dB/Bark and -25 dB/Bark.
	It is an improved version of bladonlindblomfilter.
 */

double NUMtriangularfilter_amplitude (double fl, double fc, double fh,
	double f);
/*
	Filterfunction that intermediates in Mel frequency cepstral coefficients
	calculation.
	The filter function is

			 (f-fl)/(fc-fl)  fl < f < fc
	H(z) =   (fh-f)/(fh-fc)  fc < f < fh
			 0			   otherwise
	Preconditions:
		0 < fl < fh
 */

double NUMformantfilter_amplitude (double fc, double bw, double f);
/*
	Filterfunction with a formant-like shape on a linear freq. scale.

	H(f) = 1.0 / (dq * dq + 1.0), where
		dq = (fc * fc - f * f) / (bw * f)
	Preconditions: f > 0 && bw > 0
*/

double VECburg (VEC const& a, constVEC const& x);
/*
	Calculates linear prediction coefficients according to the algorithm
	from J.P. Burg as described by N.Anderson in Childers, D. (ed), Modern
	Spectrum Analysis, IEEE Press, 1978, 252-255.
	Returns the sum of squared sample values or 0.0 if failure
*/

autoVEC newVECburg (constVEC const& x, integer numberOfPredictionCoefficients, double *out_xms);

/*
	assertion: filtermemory.size >= filter.size
*/
void VECfilterInverse_inplace (VEC const& s, constVEC const& filter, VEC const& filterMemory);

void NUMdmatrix_to_dBs (MAT const& m, double ref, double factor, double floor);
/*
	Transforms the values in the matrix m [rb..re] [cb..ce] to dB's

	m [i] [j] = factor * 10 * log10 (m [i] [j] / ref)
	if (m [i] [j] < floor) m [i] [j] = floor;

	Preconditions:
		rb <= re
		cb <= ce
		ref > 0
		factor > 0
	Errors:
		Matrix elements < 0;
*/

autoMAT MATcosinesTable (integer  n);
/*
	Generate table with cosines.

	result [i] [j] = cos (i * pi * (j - 1/2) / npoints)
*/
void VECcosineTransform_preallocated (VEC const& target, constVEC const& x, constMAT const& cosinesTable);
void VECinverseCosineTransform_preallocated (VEC const& target, constVEC const& x, constMAT const& cosinesTable);

/******  Interpolation ****/

void NUMcubicSplineInterpolation_getSecondDerivatives (VEC const& out_y, constVEC const& x, constVEC const& y, double yp1, double ypn);
/*
	Given arrays x [1..n] and y [1..n] containing a tabulated function, i.e.,
	y [i] = f(x [i]), with x [1] < x [2] < ... < x [n], and given values yp1 and
	ypn for the first derivative of the interpolating function at point
	1 and n, respectively, this routine returns an array out_y [1..n] that
	contains the second derivative of the interpolating function at the
	tabulated point x.
	If yp1 and/or ypn are >= 10^30, the routine is signaled to
	set the corresponding boundary condition for a natural spline, with
	zero second derivative on that boundary.
*/

double NUMcubicSplineInterpolation (constVEC const& xa, constVEC const& ya, constVEC const& y2a, double x);
/*
	Given arrays xa [1..n] and ya [1..n] containing a tabulated function,
	i.e., y [i] = f(x [i]), with x [1] < x [2] < ... < x [n], and given the
	array y2a [1..n] which is the output of NUMcubicSplineInterpolation_getSecondDerivatives above, and given
	a value of x, this routine returns an interpolated value y.
*/

autoVEC newVECbiharmonic2DSplineInterpolation_getWeights (constVECVU const& x, constVECVU const& y, constVECVU const& w);
/*
	Input: x [1..numberOfPoints], y [1..numberOfPoints], (xp,yp)
	Output: interpolated result
*/

double NUMbiharmonic2DSplineInterpolation (constVECVU const& x, constVECVU const& y, constVECVU const& w, double xp, double yp);
/* Biharmonic spline interpolation based on Green's function.
	. Given z [i] values at points (x [i],y [i]) for i=1..n, 
	Get value at new point (px,py).
	1. Calculate weights w once: newVECbiharmonic2DSplineInterpolation_getWeights
	2. Interpolate at (xp,yp): NUMbiharmonic2DSplineInterpolation
	Input: x [1..numberOfPoints], y [1..numberOfPoints], z [1..numberOfPoints], weights [1..numberOfPoints]
	Output: weights [1..numberOfPoints]
	
	Preconditions: all x [i] are different and all y [i] are different.
	
	This routine inializes the numberOfPoints weigts by inverting a numberOfPoints x numberOfPoints matrix.
	D. Sandwell (1987), Biharmonic spline interpolation of GEOS-3 and SEASAT altimetr data, Geophysical Research Letters 14, 139--142
	X. Deng & Z. Tang (2011), Moving surface spline interpolation based on Green's function, Math. Geosci 43: 663--680
*/


double NUMsincpi (const double x);
/* Calculates sin(pi*x)/(pi*x) */
double NUMsinc (const double x);
/* Calculates sin(x)/(x) */

/*********************** Geometry *************************************/

integer NUMgetOrientationOfPoints (double x1, double y1, double x2, double y2, double x3, double y3);
/* Traverse points 1, 2 and 3. If we travel counter-clockwise the result will be 1,
	if we travel clockwise the result will be -1 and the result will be 0 if 3 is on the line segment between 1 and 2.
	J. O'Rourke: Computational Geometry, 2nd Edition, Code 1.5
*/

bool NUMdoLineSegmentsIntersect (double x1, double y1, double x2, double y2, double x3, double y3,
	double x4, double y4);
/* Does the line segment from (x1,y1) to (x2,y2) intersect with the line segment from (x3,y3) to (x4,y4)? */

integer NUMgetIntersectionsWithRectangle (double x1, double y1, double x2, double y2,
	double xmin, double ymin, double xmax, double ymax, double *xi, double *yi);
/* Get the intersection points of the line through the points (x1,y1) and (x2,y2) with the
	rectangle with corners (xmin, ymin) and (xmax,ymax).
	The returned value is the number of intersections found and is either 0 or 1 or 2.
*/

void NUMgetEllipseBoundingBox (double a, double b, double cospsi,
	double *out_width, double *out_height);
/*
	Get the width and the height of the bonding box around an ellipse.
	a and b are the lengths of the long axes.
	cospsi is the cosine of the angle between the a-axis and the horizontal
	x-axis (cs == 0 when a-axis and x-axis are perpendicular).

	Parametrisation of the ellipse:
		x(phi) = a cos(psi) cos(phi) - b sin (psi) sin(phi)
		y(phi) = a sin(psi) cos(phi) + b cos(psi) sin(phi)  0 <= phi <= 2 pi
	Extrema:
		d x(phi) / dphi == 0 and d y(phi) / dphi == 0
	Solution:
		x(phi1) = a cos(psi) cos(phi1) - b sin (psi) sin(phi1)
		y(phi2) = a sin(psi) cos(phi2) + b cos(psi) sin(phi2),
	where
		phi1 = arctg ( -b/a tg(psi))
		phi2 = arctg ( b/a cotg(psi))
	Special cases are psi = 0 and pi /2
*/

double NUMminimize_brent (double (*f) (double x, void *closure), double a, double b,
	void *closure, double tol, double *fx);
/*
	The function returns an estimate for the minimum location with accuracy
		3 * SQRT_EPSILON * abs(x) + tol.
	The function always obtains a local minimum which coincides with
	the global one only if a function under investigation being unimodular.
	If a function being examined possesses no local minimum within
	the given range, the function returns 'a' (if f(a) < f(b)), otherwise
	it returns the right range boundary value b.

	Algorithm

	The function makes use of the golden section procedure combined with
	parabolic interpolation.
	At every step, the program operates at three abscissae - x, v, and w.
	x - the last and the best approximation to the minimum location,
		i.e. f(x) <= f(a) or/and f(x) <= f(b)
		(if the function f has a local minimum in (a,b), then both
		conditions are fulfiled after one or two steps).
	v, w are previous approximations to the minimum location. They may
	coincide with a, b, or x (although the algorithm tries to make all
 	u, v, and w distinct). Points x, v, and w are used to construct
	interpolating parabola whose minimum will be treated as a new
	approximation to the minimum location if the former falls within
	[a,b] and reduces the range enveloping minimum more efficient than
	the golden section procedure.
	When f(x) has a second derivative positive at the minimum location
	(not coinciding with a or b) the procedure converges superlinearly
	at a rate order about 1.324
*/

void VECsmooth_gaussian_inplace (VECVU const& in_out, double sigma);
void VECsmooth_gaussian_inplace (VECVU const& in_out, double sigma, NUMFourierTable fftTable);
void VECsmooth_gaussian (VECVU const& out, constVECVU const& in, double sigma, NUMFourierTable fftTable);
/*
	Smooth the vector 'in/in_out' by convolving with a Gaussian, i.e. convolve with gaussian by
	using the Fourier Transform. Normally an FFT is used unless otherwise specified in 'fftTable"
	If fftTable == nullptr the FFT of size 2^k is used, where 2^(k-1) < n <= 2^k.
	For a given fftTable we require that fftTable->n >= n.
*/

integer NUMgetIndexFromProbability (constVEC probs, double p); //TODO HMM zero start matrices
integer NUMgetIndexFromProbability (double *probs, integer nprobs, double p);

// Fit the line y= ax+b
void NUMlineFit (constVEC x, constVEC y, double *out_m, double *out_intercept, integer method);
/* method
 * 1 least squares
 * 2 rubust incomplete Theil O(N/2)
 * 3 robust complete Theil (very slow for large N, O(N^2))
 */

void NUMfitLine_theil_preallocated (VEC const& out_lineParameters, constVEC const& x, constVEC const& y, bool wantIntercept, double oneTailedUnconfidence, bool completeMethod);

void NUMfitLine_theil (constVEC const& x, constVEC const& y, double *out_m, double *out_intercept, bool completeMethod);
/*
 * Preconditions:
 *		all x [i] should be different, i.e. x [i] != x [j] for all i = 1..(numberOfPoints - 1), j = (i+1) ..numberOfPoints
 * Algorithm:
 * Theils robust line fit method:
 * 1. Use all combination of pairs (x [i],y [i]), (x [j],y [j]) to calculate an intercept m [k] as
 *	m [k] = (y [j] - y [i]) / (x [j] - x [i]).
 *	There will be (numberOfPoints - 1) * numberOfPoints / 2 numbers m [k].
 * 2. Take the median value m of all the m [k].
 * 3. Calculate the numberOfPoints intercepts b [i] as b [i] = y [i] - m * x [i]
 * 4. Take the median value b of all the b [i] values
 * 
 * If incompleteMethod we use Theil's incomplete method to reduce the number of combinations.
 * I.e. split the data in two equal parts at n2 = numberOfPoints / 2  and then calculate the numberOfPoints/2 intercepts m [i] as
 *   m [i] = (y [n2+i] - y [i]) / (x [n2 + i] - x [i]).
 * The rest proceeds as outlined above
 */


void NUMfitLine_LS (constVEC const& x, constVEC const& y, double *out_m, double *out_intercept);

/*
	scale_lambda, shape_k > 0
*/
double NUMrandomWeibull (double scale_lambda, double shape_k);

/* The binomial distribution has the form,

   f(x) =  n!/(x!(n-x)!) * p^x (1-p)^(n-x) for integer 0 <= x <= n
		=  0							   otherwise

   This implementation follows the public domain ranlib function
   "ignbin", the bulk of which is the BTPE (Binomial Triangle
   Parallelogram Exponential) algorithm introduced in
   Kachitvichyanukul and Schmeiser [1].  It has been translated to use
   modern C coding standards.

   If n is small and/or p is near 0 or near 1 (specifically, if
   n*min(p,1-p) < SMALL_MEAN), then a different algorithm, called
   BINV, is used which has an average runtime that scales linearly
   with n*min(p,1-p).

   But for larger problems, the BTPE algorithm takes the form of two
   functions b(x) and t(x) -- "bottom" and "top" -- for which b(x) <
   f(x)/f(M) < t(x), with M = floor(n*p+p).  b(x) defines a triangular
   region, and t(x) includes a parallelogram and two tails.  Details
   (including a nice drawing) are in the paper.

   [1] Kachitvichyanukul, V. and Schmeiser, B. W.  Binomial Random
   Variate Generation.  Communications of the ACM, 31, 2 (February,
   1988) 216.

   Note, Bruce Schmeiser (personal communication) points out that if
   you want very fast binomial deviates, and you are happy with
   approximate results, and/or n and n*p are both large, then you can
   just use gaussian estimates: mean=n*p, variance=n*p*(1-p).

   This implementation by James Theiler, April 2003, after obtaining
   permission -- and some good advice -- from Drs. Kachitvichyanukul
   and Schmeiser to use their code as a starting point, and then doing
   a little bit of tweaking.

   Additional polishing for GSL coding standards by Brian Gough.
*/
integer NUMrandomBinomial (double p, integer n);
double NUMrandomBinomial_real (double p, integer n);

/*
	Generates random numbers according to a Gamma distribution with shape parameter "alpha"
	and rate parameter "beta".
	
	The Gamma distribution of order (shape) parameter alpha and rate (beta) is defined as:

		f(x; alpha, beta) = (1 / Gamma (alpha)) beta^alpha x^(alpha-1) e^(-beta.x),
		for x > 0, alpha > 0 && beta > 0.

	The method is described in
		G. Marsaglia & W. Tsang (2000): A simple method for generating gamma variables. ACM Transactions on Mathematical Software, 26(3):363-372.
	Preconditions: alpha > 0 && beta > 0.
*/
double NUMrandomGamma (const double alpha, const double beta);

// IEEE: Programs for digital signal processing section 4.3 LPTRN (modfied)
// lpc [1..n] to rc [1..n]
void VECrc_from_lpc (VEC rc, constVEC lpc);

// rc [1..n] to area [1..n], implicit: area [n+1] = 0.0001; (1 cm^2)
void VECarea_from_rc (VEC area, constVEC rc);

// area [1..n] to rc [1..n-1] (modification: LPTRN assumes area [n+1])
void VECrc_from_area (VEC rc, constVEC area);

// area [1..n] to lpc [1..n-1]! (modification: lptrn gives lpc [1] = 1 we don't)
void VEClpc_from_area (VEC lpc, constVEC area);

// lpc [1..n] to area [1..n+1], area [m+1] = 0.0001; (1 cm^2)
void VECarea_from_lpc (VEC area, constVEC lpc);
/*
 Fix indices to be in the range [lowerLimit, upperLimit].
*/
void NUMfixIndicesInRange (integer lowerLimit, integer upperLimit, integer *lowIndex, integer *highIndex);

void NUMgetEntropies (constMATVU const& m, double *out_h, double *out_hx, 
	double *out_hy,	double *out_hygx, double *out_hxgy, double *out_uygx, double *out_uxgy, double *out_uxy);

inline double NUMmean_weighted (constVEC x, constVEC w) {
	Melder_assert (x.size == w.size);
	double inproduct = NUMinner (x, w);
	double wsum = NUMsum (w);
	return inproduct / wsum;
}

inline void VECchainRows_preallocated (VECVU const& v, constMATVU const& m) {
	Melder_assert (m.nrow * m.ncol == v.size);
	integer k = 1;
	for (integer irow = 1; irow <= m.nrow; irow ++)
		for (integer icol = 1; icol <= m.ncol; icol ++)
			v [k ++] = m [irow] [icol];
}

inline autoVEC VECchainRows (constMATVU const& m) {
	autoVEC result = raw_VEC (m.nrow * m.ncol);
	VECchainRows_preallocated (result.get(), m);
	return result;
}

inline void VECchainColumns_preallocated (VEC const& v, constMATVU const& m) {
	Melder_assert (m.nrow * m.ncol == v.size);
	integer k = 1;
	for (integer icol = 1; icol <= m.ncol; icol ++)
		for (integer irow = 1; irow <= m.nrow; irow ++)
			v [k ++] = m [irow] [icol];
}

inline autoVEC VECchainColumns (constMATVU const& m) {
	autoVEC result = raw_VEC (m.nrow * m.ncol);
	VECchainColumns_preallocated (result.get(), m);
	return result;
}

/* R = X.Y.Z */
void MATmul3 (MATVU const & target, constMATVU const& X, constMATVU const& Y, constMATVU const& Z);

/* Z = X.Y.X' */
void MATmul3_XYXt (MATVU const& target, constMATVU const& X, constMATVU const& Y);

/* Z = X.Y.X where Y is a symmetric matrix */
void MATmul3_XYsXt (MATVU const& target, constMATVU const& X, constMATVU const& Y);

/*
	First row (n elements) is at v [1]..v [n],
	second row (n-1 elements) is at v [n+1],..,v [n+n-1],
	third row (n-2 elements) is at v [n+n],..,v [n+n+n-2]
	last row (1 element) is at v [n(n+1)/2].
*/
inline void MATfromUpperTriangularVector_preallocated (MAT m, constVEC v) {
	Melder_assert (v.size == m.ncol * (m.ncol + 1) / 2);
	integer irow = 1;
	for (integer inum = 1; inum <= v.size; inum ++) {
		integer nskipped = (irow - 1) * irow / 2;
		integer inumc = inum + nskipped;
		irow = (inumc - 1) / m.ncol + 1;
		integer icol = ( (inumc - 1) % m.ncol) + 1;
		m [irow] [icol] = m [icol] [irow] = v [inum];
		if (icol == m.ncol) irow ++;
	}
}

void NUMeigencmp22 (double a, double b, double c, double *out_rt1, double *out_rt2, double *out_cs1, double *out_sn1 );
/*
	This routine is copied from LAPACK.
	Computes the eigendecomposition of a 2-by-2 symmetric matrix
		[  a   b  ]
		[  b   c  ].
	on return, rt1 is the eigenvalue of larger absolute value, rt2 is the
	eigenvalue of smaller absolute value, and (cs1,sn1) is the unit right
	eigenvector for rt1, giving the decomposition

		[ cs1  sn1 ] [  a   b  ] [ cs1 -sn1 ]  =  [ rt1  0  ]
		[-sn1  cs1 ] [  b   c  ] [ sn1  cs1 ]     [  0  rt2 ].



	rt1 is accurate to a few ulps barring over/underflow.

	rt2 may be inaccurate if there is massive cancellation in the
	determinant a*c-b*b; higher precision or correctly rounded or
	correctly truncated arithmetic would be needed to compute rt2
	accurately in all cases.

	cs1 and sn1 are accurate to a few ulps barring over/underflow.

	overflow is possible only if rt1 is within a factor of 5 of overflow.
	underflow is harmless if the input data is 0 or exceeds
	underflow_threshold / macheps.
*/

void NUMpolynomial_recurrence (VEC const& pn, double a, double b, double c, constVEC const& pnm1, constVEC const& pnm2);

#endif // !_NUM2_h_
