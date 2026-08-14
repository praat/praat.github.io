#ifndef _CPP_h_
#define _CPP_h_
/* CPP.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Vector.h"

Thing_define (CPP, Vector) {
	void v1_info ()
		override;
	int v_domainQuantity () const
		override { return MelderQuantity_TIME_SECONDS; }
};

/* 
	Cepstral Peak Prominence is a measure that shows periodicity in the Spectrum.
	Periodicity in the Spectrum is caused by periodicity in the sound if the
	sound is composed of multiple harmonics. A single sine with frequency f0 does not show 
	periodicity in the spectrum, just a single peak. Only if there are harmonics of f0,
	periodicity in the spectrum will turn up. 
	
	Attributes:
		xmin			// Start time (seconds).
		xmax > xmin		// End time (seconds).
		nx >= 1			// Number of time slices.
		dx > 0.0		// Time step (seconds).
		x1				// Centre of first time slice (seconds).
		ymin, ymax, dy, y1 = 1.0
		ny = 1
		z [1] [1..nt]
		TODO
			The Cepstral Prominence Peak value, a real number between 0 dB and > 100 dB:
			0 dB means no periodicity at all
			Normal values for speech are between 10 dB for non-voiced parts and 60 dB for monotone perfectly voiced
*/

autoCPP CPP_create (double tmin, double tmax, integer nt, double dt, double t1);

void CPP_draw (CPP me, Graphics g, double tmin, double tmax, double min, double max, bool garnish);
/*
	draw a CPP contour into the current Graphics.
	If tmax <= tmin, draw whole time domain.
	If max <= min, scale to extrema.
*/

double CPP_getMean (CPP me, double tmin, double tmax);

double CPP_getStandardDeviation (CPP me, double tmin, double tmax);

double CPP_getQuantile (CPP me, double quantile);

autoMatrix CPP_to_Matrix (CPP me);

autoCPP Matrix_to_CPP (Matrix me);

#endif // _CPP_h_
