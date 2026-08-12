#ifndef _SlopeSelector_h_
#define _SlopeSelector_h_
/* SlopeSelector.h
 *
 * Copyright (C) 2025-2026 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

/*
	The algorithm to find the kth (or median) slope in O (n log(n)) time is described in
		Matousek (1991): Randomized optimal algorithm for slope selection.
		Information processing letters 39: 183-187.
	Given points p[i] = (ax[i], ay[i]), i = 1..N, with ax[i] < ax[i+1] for i = 1..N-1.
	Find the line with the k-th largest slope of all the N(N-1)/2 lines connecting
	p[i] = (ax[i],ay[i]) with p[j] = (ax[j],ay[j]) where i = 1..N,and j = i+1..N.
	We consider this problem in the dual space where we have N lines line[l] where
		line[l] is y = ax[N-l+1] * x - ay[N-l+1].
	The ordering of the lines is such that when x = -inf, line [1] has the lowest y-value and
	line [N] the highest. Therefore, the correspondence between line number 'l' and point number 'i' is
        l = N-i+1, for i = 1..N.
	The x coordinate of the intersection of line[i] and line[j] is at x = (ay[N-i+1]-ay[N-j+1])/(ax[N-i+1]-ax[N-j+1]),
	which equals the slope of the line connecting p[i] and p[j].
	In the dual space (x,y) we then search for the intersection with the k-th largest x-coordinate.
	For x = -inf the y-values of line[1]..line[N] are y[1] > y[2] > .. y[N], and
	for x = +inf the y-values of line[1]..line[N] are in reverse order y[N] > y[N-1] > .. y[1].
	If we sort the N crossings of the lines at x = a according to line number this will be a permutation
	of the numbers 1..N. The number of inversions in this permutation has to be equal to the number of intersections
	to the left of x=a (an inversion occurs when i < j && perm[i] > perm[j]).
	The number of inversions in a permutation of size N can be counted in O(N log(N)).
 */

#include "PermutationInversionCounter.h"
#include "melder.h"
#include "NUM2.h"
#include "SlopeSelector_enums.h"
#include "ExtendedReal.h"

//#include "SlopeSelector_def.h"

struct structExtendedCrossing {
	double real;
	integer low, high;
	
	friend bool operator<  (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
		auto approximatelyEqual = [&] (double numericalEqualityPrecision) {
			if (std::fabs (lhs.real) < numericalEqualityPrecision || std::fabs (rhs.real) < numericalEqualityPrecision) {
				return std::fabs (lhs.real - rhs.real) < numericalEqualityPrecision;
			}
			// Use relative difference otherwise
			return std::fabs (lhs.real - rhs.real) <= numericalEqualityPrecision * std::fmax (std::fabs (lhs.real), std::fabs (rhs.real));
		};
		const bool r = ( !approximatelyEqual (1e-12) ? (lhs.real < rhs.real) :
				(lhs.high != rhs.high ? (lhs.high > rhs.high) : (lhs.low < rhs.low)) );
		return r;
	}
	/*
		All derived from above comparison
	*/
	friend inline bool operator>  (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
		return rhs < lhs;
	}
	friend inline bool operator<= (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
		return ! (lhs > rhs);
	}
	friend inline bool operator>= (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
		return ! (lhs < rhs);
	}
	friend inline bool operator== (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
		return std::tie (lhs.real, lhs.low, lhs.high) == std::tie (rhs.real, rhs.low, rhs.high);
	}
	friend inline bool operator!= (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
		return ! (lhs == rhs);
	}
};

typedef struct structExtendedCrossing *ExtendedCrossing;
typedef const struct structExtendedCrossing *constExtendedCrossing;

Thing_define (SlopeSelector, Thing) {
	integer numberOfPoints;
    integer sampleSize;
    integer numberOfTries;
	integer maximumNumberOfTries;
    integer maximumContractionSize; // > sampleSize + numberOfPoints
	autoPermutation lineRankingAtLowX;
	autoPermutation lineRankingAtLowXPrevious;
	autoPermutation inverseOfLineRankingAtLowX;
	autoPermutation lineRankingAtHighX;
	autoPermutation lineRankingAtHighXPrevious;
	autoINTVEC sortedRandomCrossingCodes;
	integer inversionsSize;
	autoINTVEC currentInversions;
	autovector<structExtendedCrossing> xslopes;
	autoVEC buffer; // for buffering and final quantile calculations
	autovector<structExtendedCrossing> xcrossings;
	autoPermutationInversionCounter inversionCounter;

	constVEC xp;	// links to the outside world data points (by newDataPoints(x,y))
	constVEC yp;	//

	void newDataPoints (constVEC const& x, constVEC const& y);
				
	double getSlope_Siegel ();
		
	double getSlope_TheilSen ();
		
	double getIntercept (double slope);
		
	void getSlopeAndIntercept_leastSquares (double &slope, double& intercept);
				
	void getKth_TheilSen (integer k, double& kth, double& kp1th);

	double slopeQuantile_TheilSen (double factor);

	double slopeQuantile_orderNSquared (double factor);

	double slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer);

	double slopeQuantile (double factor) {
		return slopeQuantile_TheilSen (factor);
	}
};

/*
    Preconditions:
        x.size == y.size && x.size > 0
        i < j && x[i] < x[j]
*/

autoSlopeSelector SlopeSelector_create (constVEC const& x, constVEC const& y);

autoSlopeSelector SlopeSelector_create (integer numberOfPoints);

void SlopeSelector_getSlopeAndIntercept (SlopeSelector me, double &slope, double &intercept, kSlopeSelector_method method);

/****************/

void timeSlopeSelection ();

#endif /* _SlopeSelector_h_ */
