#ifndef _Sound_to_CPP_h_
#define _Sound_to_CPP_h_
/* Sound_to_CPP.h
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

#include "Sound_to_PowerCepstrogram.h"
#include "CPP.h"


autoCPP Sound_to_CPP (constSound me, double pitchFloor, double pitchCeiling, double dt, double maximumFrequency, 
	double preEmphasisFrequency,
	bool subtractTrendBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow,
	double deltaF0, kVector_peakInterpolation peakInterpolationType,
	double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod
);

#endif /* _Sound_to_CPP_h_ */
