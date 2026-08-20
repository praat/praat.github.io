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

static autoVEC CPP_getSoundingValues (CPP me, double tmin, double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer imin, imax;
	integer numberOfFrames = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (numberOfFrames < 1)
		return autoVEC();
	autoVEC soundingValues = raw_VEC (numberOfFrames);
	integer numberOfSoundingFrames = 0;
	for (integer iframe = imin; iframe <= imax; iframe ++)
		if (my z [1] [iframe] > 0.0)
			soundingValues [++ numberOfSoundingFrames] = my z [1] [iframe];
	if (numberOfSoundingFrames < 1)
		return autoVEC();
	soundingValues.size = numberOfSoundingFrames;   // shrink (without reallocation)
	return soundingValues;
}

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
	autoVEC soundingValues = CPP_getSoundingValues (this, 0.0, 0.0);
	if (soundingValues.size > 0) {
		MelderInfo_writeLine (U"Cepstral Peak Prominence values of sounding frames:");
		MelderInfo_writeLine (U"   Number of sounding frames: ", soundingValues.size);
		sort_e_VEC_inout (soundingValues.get());
		MelderInfo_writeLine (U"   Median ", Melder_single (NUMquantile (soundingValues.get(), 0.50)), U" dB");
		MelderInfo_writeLine (U"   10 % = ", Melder_single (NUMquantile (soundingValues.get(), 0.10)), U" dB   90 %% = ",
				Melder_single (NUMquantile (soundingValues.get(), 0.90)), U" dB");
		MelderInfo_writeLine (U"   16 % = ", Melder_single (NUMquantile (soundingValues.get(), 0.16)), U" dB   84 %% = ",
				Melder_single (NUMquantile (soundingValues.get(), 0.84)), U" dB");
		MelderInfo_writeLine (U"   25 % = ", Melder_single (NUMquantile (soundingValues.get(), 0.25)), U" dB   75 %% = ",
				Melder_single (NUMquantile (soundingValues.get(), 0.75)), U" dB");
		MelderInfo_writeLine (U"Minimum: ", Melder_single (soundingValues [1]), U" dB");
		MelderInfo_writeLine (U"Maximum: ", Melder_single (soundingValues [soundingValues.size]), U" dB");
		MelderGaussianStats stats = NUMmeanStdev (soundingValues.all());
		MelderInfo_writeLine (U"Average: ", Melder_single (stats.mean), U" dB");
		if (soundingValues.size > 1)
			MelderInfo_writeLine (U"Standard deviation: ", Melder_single (stats.stdev), U" dB");
	}
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
	autoVEC soundingValues = CPP_getSoundingValues (me, tmin, tmax);
	return NUMmean (soundingValues.get());

}

double CPP_getStandardDeviation (CPP me, double tmin, double tmax) {
	autoVEC soundingValues = CPP_getSoundingValues (me, tmin, tmax);
	return NUMstdev (soundingValues.get());

}

double CPP_getQuantile (CPP me, double quantile) {
	autoVEC soundingValues = CPP_getSoundingValues (me, 0.0, 0.0);
	sort_e_VEC_inout (soundingValues.get());
	return NUMquantile (soundingValues.get(), quantile);
}

autoCPP CPP_and_Pitch_to_CPP_markUnvoiced (CPP me, Pitch thee) {
	Melder_require (my xmin == thy xmin && my xmax == thy xmax,
		U"The domains of the CPP and the Pitch should be equal.");
	try {
		autoCPP him = Data_copy (me);
		for (integer i = 1; i <= my nx; i ++) {
			const double time = my x1 + (i - 1) * my dx;
			if (! Pitch_isVoiced_t (thee, time))
				his z [1] [i] = 0.0;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"cannot convert to object with marked unvoiced frames.");
	}
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
