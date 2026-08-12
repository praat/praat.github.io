#ifndef _PermutationInversionCounter_h_
#define _PermutationInversionCounter_h_
/* PermutationInversionCounter.h
 *
 * Copyright (C) 2025 David Weenink
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

#include "Permutation.h"
//#include "PermutationInversionCounter_def.h"

Thing_define (PermutationInversionCounter, Thing) {
	constPermutation pdata, potherInverse; // links
	autoPermutation psortingOrder;
	integer numberOfElements;
	autoINTVEC workspace;
	integer totalNumberOfInversions;
	integer totalNumberOfInversionsInInterval;
	integer numberOfInversionsRegistered;
	integer numberOfInversionsToRegister;
	integer posInSortedSelectedInversionIndices;
	INTVEC inversions;			// the inversions as pairs of (ilow, ihigh) to be output, link to storage supplied by caller
	INTVEC otherInverse;		// link to potherInverse -> p.get() supplied by user
		
	constINTVEC sortedSelectedInversionIndices; // link to storage supplied by caller
	
	void newData (constPermutation p);
		
	void reset ();
		
	integer mergeInversions (integer p, integer q, integer r);
		
	integer mergeSort (integer p, integer r);
		
	integer getNumberOfInversions (constPermutation p);
		
	integer getSelectedInversionsNotInOther (constPermutation p, constPermutation otherInverse, 
		constINTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions
	);
		
	integer getSelectedInversions (
		constPermutation p, constINTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions
	);
		
	integer getInversions (constPermutation p, INTVEC const& out_inversions);
			
};

/*			
	Given a vector v with n elements, we define an inversion as i < j && v [i] > v [j].
    We use a trick to store the inversion between two elements (i,j) with only **one** number code by using the following
    lower triangular (n-1) x (n-1) matrix, with i being the **column** index and j being the **row** index!

             1  2  3  4  5  ... n-1
          +--------------- ...--
    1(2)  |  1
    2(3)  |  2  3
    3(4)  |  4  5  6
    4(5)  |  7  8  9 10
    5(6)  | 11 12 13 14 15
    ...
    n-1(n)|                   n(n-1)/2

    E.g. the inversion between element 1 and element 2 is coded with the number 1,
        the inversion between element 2 and 5 is code with the number 8.
    From inversion (i<j) to the code:
        code = (j-2)*(j-1)/2 + i
    From the code to the inversion (i,j)
        j = Melder_iroundUp (0.5 + sqrt (0.25 + 2 * code));
        i = code - (j-2) * (j - 1) / 2;

 */

inline void getInversionFromCode (integer code, integer& ilow, integer& ihigh) {
	ihigh = Melder_iroundUp (0.5 + sqrt (0.25 + 2 * code));
	ilow = code - (ihigh - 2) * (ihigh - 1) / 2;
	Melder_assert (ilow < ihigh);
}

inline integer getCodeFromInversion (integer ilow, integer ihigh) {
	Melder_assert (ilow < ihigh);
	return (ihigh - 2) * (ihigh - 1) / 2 + ilow;
}

void PermutationInversionCounter_init (integer size);

autoPermutationInversionCounter PermutationInversionCounter_create (integer size);

#endif // _PermutationInversionCounter_h_
