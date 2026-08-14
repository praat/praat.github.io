/* CPP.cpp
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

#include "Graphics.h"
#include "CPP.h"

Thing_implement (CPP, Vector, 2);

void structCPP :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", our nx);
	MelderInfo_writeLine (U"   Time step: ", our dx, U" seconds");
	MelderInfo_writeLine (U"   First frame centred at: ", our x1, U" seconds");
}

autoCPP CPP_create (double tmin, double tmax, integer nt, double dt, double t1) {
	try {
		autoCPP me = Thing_new (CPP);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, 1.0, 1.0, 1, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CPP not created.");
	}	
}

void CPP_draw (CPP me, Graphics g, double tmin, double tmax, double min, double max, bool garnish) {
	Vector_draw (me,  g, & tmin, & tmax, & min, & max, my dy, U"speckles");
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 10.0, true, true, false);
		Graphics_textLeft (g, true, U"CPP");
	}
}
/*
	draw a CPP contour into the current Graphics.
	If tmax <= tmin, draw whole time domain.
	If max <= min, scale to extrema.
*/

double CPP_getMean (CPP me, double tmin, double tmax) {
	
}

double CPP_getStandardDeviation (CPP me, double tmin, double tmax) {
	
}

double CPP_getQuantile (CPP me, double quantile) {
	
}

autoMatrix CPP_to_Matrix (CPP me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"not converted to Matrix.");
	}
}

autoCPP Matrix_to_CPP (Matrix me) {
	try {
		autoCPP thee = CPP_create (my xmin, my xmax, my nx, my dx, my x1);
		thy z.all()  <<=  my z.row (1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"not converted to CPP.");
	}
}

/* End of file CPP.cpp */
