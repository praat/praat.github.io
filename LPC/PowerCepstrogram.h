#ifndef _PowerCepstrogram_h_
#define _PowerCepstrogram_h_
/* PowerCepstrogram.h
 *
 * Copyright (C) 2012-2025 David Weenink
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
	The Cepstrogram is a matrix of real numbers.
	It is the inverse-fourier-transformed logarithm of the spectrum
	of a (sound) signal.
*/

#include "PowerCepstrumWorkspace.h"
#include "CPP.h"
#include "Sound.h"
#include "Table.h"

Thing_define (PowerCepstrogram, Matrix) {
	double v_getValueAtSample (integer sampleNumber, integer level, int unit) const
			override;
};

/*
	xmin, xmax : time domain
	nx, dx, x1	: sampling in the time domain
	ymin		// Lowest quefrency.
	ymax		// Highest quefrency.
	ny			// Number of quefrencies.
	dy			// Quefrency step.
	y1			// First quefrency.

*/

autoPowerCepstrogram PowerCepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double qmin, double qmax, integer nq, double dq, double q1);

void PowerCepstrogram_paint (PowerCepstrogram me, Graphics g, double tmin, double tmax,
	double qmin, double qmax, double dBmaximum, int autoscaling, double dynamicRangedB, double dynamicCompression, bool garnish);

autoPowerCepstrogram PowerCepstrogram_smooth (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow);


autoTable PowerCepstrogram_to_Table_hillenbrand (PowerCepstrogram me, double pitchFloor, double pitchCeiling);

autoCPP PowerCepstrogram_to_CPP (PowerCepstrogram me, double pitchFloor, double pitchCeiling, 
	double deltaF0, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

autoTable PowerCepstrogram_to_Table_CPP (PowerCepstrogram me, bool includeFrameNumber, bool includeTime, 
	integer numberOfTimeDecimals, integer numberOfCPPdecimals, bool includeQuefrency, integer numberOfQuefrencyDecimals,
	double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType,
	double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

void PowerCepstrogram_listCPP (PowerCepstrogram me, bool includeFrameNumber, bool includeTime, 
	integer numberOfTimeDecimals, integer numberOfCPPdecimals, bool includePeakQuefrency, integer numberOfQuefrencyDecimals,
	double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType,
	double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

autoTable PowerCepstrogram_to_Table_CPPvalues (PowerCepstrogram me, double pitchFloor, double pitchCeiling,
	double deltaF0, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

autoPowerCepstrum PowerCepstrogram_to_PowerCepstrum_slice (PowerCepstrogram me, double time);

autoPowerCepstrogram PowerCepstrogram_subtractTrend (constPowerCepstrogram me, double qstartFit, double qendFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

void PowerCepstrogram_subtractTrend_inplace (mutablePowerCepstrogram me, double qstartFit, double qendFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

double PowerCepstrogram_getCPPS_hillenbrand (PowerCepstrogram me, bool subtractTrendBeforeSmoothing,
	double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling);

double PowerCepstrogram_getCPPS (PowerCepstrogram me, bool subtractTrendBeforeSmoothing,
	double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

autoMatrix PowerCepstrogram_to_Matrix (PowerCepstrogram me);

autoPowerCepstrogram Matrix_to_PowerCepstrogram (Matrix me);

#endif /* _PowerCepstrogram_h_ */
