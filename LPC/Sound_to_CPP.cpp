/* Sound_to_CPP.cpp
 *
 * Copyright (C) 2026 David Weenink
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

#include "Sound_to_CPP.h"

autoCPP Sound_to_CPP (constSound me, double pitchFloor, double pitchCeiling, double dt, double maximumFrequency, 
	double preEmphasisFrequency, bool subtractTrendBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow,
	kVector_peakInterpolation peakInterpolationType,
	double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	try {
		autoPowerCepstrogram ps = Sound_to_PowerCepstrogram (me, pitchFloor, dt, maximumFrequency, preEmphasisFrequency);
		if (subtractTrendBeforeSmoothing) {
			ps = PowerCepstrogram_subtractTrend (ps.get(), qstartFit, qendFit, lineType, fitMethod).move();
		}
		autoPowerCepstrogram smoothed = PowerCepstrogram_smooth (ps.get(), timeAveragingWindow, quefrencyAveragingWindow);
		autoCPP cpp = PowerCepstrogram_to_CPP (smoothed.get(), pitchFloor, pitchCeiling,
			peakInterpolationType,  qstartFit,  qendFit, lineType, fitMethod);
		return cpp;
	} catch (MelderError) {
		Melder_throw (me, U": canot create CPP.");
	}
}


/* End of file Sound_to_CPP.cpp */
