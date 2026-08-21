/* PowerCepstrogram.cpp
 *
 * Copyright (C) 2013-2025 David Weenink
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

#include "PowerCepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "Matrix_extensions.h"
#include "NUM2.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"


#define TO10LOG(x) ((10 / NUMln10) * log ((x) + 1e-30))

integer a = sizeof(struct structMatrix);

Thing_implement (PowerCepstrogram, Matrix, 2); // derives from Matrix -> also version 2

double structPowerCepstrogram :: v_getValueAtSample (integer sampleNumber, integer row, int unit) const {
	double result = undefined;
	if (row >= 1 && row <= ny) {
		if (unit == 0)
			result = z [row] [sampleNumber];
		else
			result = 10.0 * log10 (z [row] [sampleNumber] + 1e-30); // always positive
	}
	return result;
}

autoPowerCepstrogram PowerCepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double qmin, double qmax, integer nq, double dq, double q1) {
	try {
		autoPowerCepstrogram me = Thing_new (PowerCepstrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, qmin, qmax, nq, dq, q1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PowerCepstrogram not created.");
	}
}

void PowerCepstrogram_paint (PowerCepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBmaximum, int autoscaling, double dynamicRangedB, double dynamicCompression, bool garnish) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	SampledXY_unidirectionalAutowindowY (me, & qmin, & qmax);
	integer itmin, itmax, ifmin, ifmax;
	if (Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax) == 0 ||
			Matrix_getWindowSamplesY (me, qmin - 0.49999 * my dy, qmax + 0.49999 * my dy, & ifmin, & ifmax) == 0)
		return;
	autoMatrix thee = Data_copy (me);
	MelderExtremaWithInit extrema;
	for (integer irow = 1; irow <= my ny; irow ++) {
		for (integer icol = 1; icol <= my nx; icol ++) {
			const double val = TO10LOG (my z [irow] [icol]);
			extrema.update (val);
			thy z [irow] [icol] = val;
		}
	}
	double dBminimum = dBmaximum - dynamicRangedB;
	if (autoscaling) {
		dBminimum = extrema.min;
		dBmaximum = extrema.max;
	}

	for (integer icol = 1; icol <= my nx; icol ++) {
		const double lmax = NUMmax_u (thy z.column (icol));
		if (isundef (lmax))
			return;
		const double factor = dynamicCompression * (extrema.max - lmax);
		thy z.column (icol) += factor;
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, qmin, qmax);
	Graphics_image (g, thy z.part (ifmin, ifmax, itmin, itmax),
		Matrix_columnToX (thee.get(), itmin - 0.5),
		Matrix_columnToX (thee.get(), itmax + 0.5),
		Matrix_rowToY (thee.get(), ifmin - 0.5),
		Matrix_rowToY (thee.get(), ifmax + 0.5),
		dBminimum, dBmaximum);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Quefrency (s)");
	}
}

void PowerCepstrogram_subtractTrend_inplace (mutablePowerCepstrogram me, double qminFit, double qmaxFit, 
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	autoPowerCepstrogram thee = PowerCepstrogram_subtractTrend (me, qminFit, qmaxFit, lineType, fitMethod);
	my z = copy_MAT (thy z.get());
}

autoPowerCepstrogram PowerCepstrogram_subtractTrend (constPowerCepstrogram me, double qminFit, double qmaxFit, kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod) {
	try {
		autoPowerCepstrogram thee = PowerCepstrogram_create (my xmin, my xmax, my nx, my dx, my x1,
			my ymin, my ymax, my ny, my dy, my y1);
		const integer thresholdNumberOfFramesPerThread = 10;
		
		MelderThread_PARALLEL (my nx, thresholdNumberOfFramesPerThread) {
			autoPowerCepstrum powerCepstrum = PowerCepstrum_create (my ymax, my ny);
			autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (powerCepstrum.get(), qminFit, qmaxFit, trendLineType, fitMethod);
			MelderThread_FOR (iframe) {
				powerCepstrum -> z.row (1)  <<=  my z.column (iframe);
				ws -> newData (powerCepstrum.get());
				ws -> getSlopeAndIntercept ();
				ws -> subtractTrend ();
				thy z.column (iframe)  <<=  powerCepstrum -> z.row (1);
			}
		} MelderThread_ENDPARALLEL
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no trend subtracted.");
	}
}

autoTable PowerCepstrogram_to_Table_hillenbrand (PowerCepstrogram me, double pitchFloor, double pitchCeiling) {
	try {
		const conststring32 columnNames [] = { U"time(s)", U"quefrency(s)", U"CPP(dB)", U"f0(Hz)" };
		autoTable thee = Table_createWithColumnNames (my nx, ARRAY_TO_STRVEC (columnNames));
		autoPowerCepstrum him = PowerCepstrum_create (my ymax, my ny);
		for (integer icol = 1; icol <= my nx; icol ++) {
			his z.row (1)  <<=  my z.column (icol);
			double qpeak;
			const double cpp = PowerCepstrum_getPeakProminence_hillenbrand (him.get(), pitchFloor, pitchCeiling, qpeak);
			const double time = Sampled_indexToX (me, icol);
			Table_setNumericValue (thee.get(), icol, 1, time);
			Table_setNumericValue (thee.get(), icol, 2, qpeak);
			Table_setNumericValue (thee.get(), icol, 3, cpp); // Cepstrogram_getCPPS depends on this index 3!!
			Table_setNumericValue (thee.get(), icol, 4, 1.0 / qpeak);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table with cepstral peak prominence values created.");
	}
}

static void PowerCepstrogram_into_Matrix_CPP (PowerCepstrogram me, mutableMatrix thee, bool trendSubtracted, double pitchFloor,
	double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod)
{
	Melder_assert (thy ny == 6);
	const integer numberOfFrames = thy nx, thresholdNumberOfFramesPerThread = 10;
	const double qminPeakSearch = 1.0 / pitchCeiling, qmaxPeakSearch = 1.0 / pitchFloor;
	MelderThread_PARALLEL (numberOfFrames, thresholdNumberOfFramesPerThread) {
		autoPowerCepstrum powerCepstrum = PowerCepstrum_create (my ymax, my ny);
		autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (powerCepstrum.get(), qminFit, qmaxFit, trendLineType, fitMethod);
		ws -> initPeakSearchPart (qminPeakSearch, qmaxPeakSearch, peakInterpolationType);
		MelderThread_FOR (iframe) {
			powerCepstrum -> z.row (1)  <<=  my z.column (iframe);
			ws -> newData (powerCepstrum.get());
			ws -> trendSubtracted = trendSubtracted;
			ws -> getSlopeAndIntercept ();
			ws -> slopeKnown = true;
			ws -> getPeakAndPosition ();
			ws -> getCPP ();
			thy z [1] [iframe] = Sampled_indexToX (thee, iframe);
			thy z [2] [iframe] = ws -> slope;
			thy z [3] [iframe] = ws -> intercept;
			thy z [4] [iframe] = ws -> peakdB;
			thy z [5] [iframe] = ws -> peakQuefrency;
			thy z [6] [iframe] = ws -> cpp;
		}
	} MelderThread_ENDPARALLEL
}

static autoMatrix PowerCepstrogram_to_Matrix_CPP (PowerCepstrogram me, bool trendSubtracted, double pitchFloor, double pitchCeiling,
	double deltaF0, kVector_peakInterpolation peakInterpolationType, double qminFit, double qmaxFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	try {
		/* Matrix rows: time, cppRaw, slope, intercept, cppCorrected, peakQuefrency */
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, 6.5, 6, 1.0, 1.0);
		PowerCepstrogram_into_Matrix_CPP (me, thee.get(), trendSubtracted, pitchFloor, pitchCeiling, deltaF0, peakInterpolationType, 
			qminFit, qmaxFit, lineType, fitMethod);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could mot create Matrix with CPP values. ");
	}
}

autoCPP PowerCepstrogram_to_CPP (PowerCepstrogram me, double pitchFloor, double pitchCeiling,
	kVector_peakInterpolation peakInterpolationType, double qminFit, double qmaxFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	try {
		autoMatrix mat = PowerCepstrogram_to_Matrix_CPP (me, false, pitchFloor, pitchCeiling,
			0.05, peakInterpolationType, qminFit, qmaxFit, lineType, fitMethod);
		autoCPP thee = CPP_create (my xmin, my xmax, my nx, my dx, my x1);
		for (integer iframe = 1; iframe <= my nx; iframe ++)
			thy z[1] [iframe] = mat -> z [6][iframe];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"no CPP created.");
	}
}

autoTable PowerCepstrogram_to_Table_CPP (PowerCepstrogram me, bool includeFrameNumber, bool includeTime,
	integer numberOfTimeDecimals, integer numberOfCPPdecimals, bool includePeakQuefrency, integer numberOfQuefrencyDecimals,
	double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType, double qminFit, double qmaxFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my nx, includeFrameNumber + includeTime + includePeakQuefrency + 1);
		integer icol = 0;
		if (includeFrameNumber)
			Table_renameColumn_e (thee.get(), ++ icol, U"frame");
		if (includeTime)
			Table_renameColumn_e (thee.get(), ++ icol, U"time(s)");
		if (includePeakQuefrency)
			Table_renameColumn_e (thee.get(), ++ icol, U"quefrency(s)");
		Table_renameColumn_e (thee.get(), ++ icol, U"CPP(dB)");
		autoPowerCepstrum him = PowerCepstrum_create (my ymax, my ny);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			icol = 0;
			if (includeFrameNumber)
				Table_setNumericValue (thee.get(), iframe, ++ icol, iframe);
			if (includeTime)
				Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (Sampled_indexToX (me, iframe), numberOfTimeDecimals));
			his z.row (1)  <<=  my z.column (iframe);
			double peakQuefrency;
			const double cpp = PowerCepstrum_getPeakProminence (him.get(), pitchFloor, pitchCeiling, peakInterpolationType,
				qminFit, qmaxFit, lineType, fitMethod, peakQuefrency);
			if (includePeakQuefrency)
				Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (peakQuefrency, numberOfQuefrencyDecimals));
			Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (cpp, numberOfCPPdecimals));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table with cepstral peak prominence values created.");
	}
}

autoTable PowerCepstrogram_to_Table_CPPvalues (PowerCepstrogram me, double pitchFloor, double pitchCeiling,
	double deltaF0, kVector_peakInterpolation peakInterpolationType, double qminFit, double qmaxFit,
	kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	try {
		static const conststring32 colNames [] = { U"time(s)", U"dB/s", U"intercept(dB)", U"peak(dB)", U"quefrency(s)", U"cpp(dB)" };
		autoTable thee = Table_createWithColumnNames (my nx, ARRAY_TO_STRVEC (colNames));
		if (lineType == kCepstrum_trendType::EXPONENTIAL_DECAY)
			Table_renameColumn_e (thee.get(), 2, U"dB/ln(s)");
		autoMatrix m = PowerCepstrogram_to_Matrix_CPP (me, false, pitchFloor, pitchCeiling,
			deltaF0,  peakInterpolationType,  qminFit,  qmaxFit, lineType, fitMethod);
		Melder_assert (m -> nx == my nx && m -> ny == 6);
		for (integer irow = 1; irow <= my nx; irow ++) {
			for (integer icol = 1; icol <= m -> ny; icol ++)
				Table_setNumericValue (thee.get(), irow, icol, m -> z [icol] [irow]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Could not create Table from PowerCepstrogram.");
	}
}

void PowerCepstrogram_listCPP (PowerCepstrogram me, bool includeFrameNumber, bool includeTime, 
	integer numberOfTimeDecimals, integer numberOfCPPdecimals, bool includePeakQuefrency, integer numberOfQuefrencyDecimals,
	double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType,
	double qminFit, double qmaxFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod)
{
	try {
		autoTable table = PowerCepstrogram_to_Table_CPP (me, includeFrameNumber, includeTime,
			numberOfTimeDecimals, numberOfCPPdecimals, includePeakQuefrency, numberOfQuefrencyDecimals,
			pitchFloor, pitchCeiling, deltaF0, peakInterpolationType, qminFit, qmaxFit, lineType, fitMethod);
		Table_list (table.get(), false);
	} catch (MelderError) {
		Melder_throw (me, U": CPP not listed.");
	}
}

static autoPowerCepstrogram PowerCepstrogram_smoothRectangular (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);
		/*
			1. average across time
		*/
		const integer numberOfFrames = Melder_ifloor (timeAveragingWindow / my dx);
		if (numberOfFrames > 1) {
			const double halfWindow = 0.5 * timeAveragingWindow;
			autoVEC qout = raw_VEC (my nx);
			for (integer iq = 1; iq <= my ny; iq ++) {
				for (integer iframe = 1; iframe <= my nx; iframe ++) {
					const double xmid = Sampled_indexToX (me, iframe);
					qout [iframe] = Sampled_getMean (me, xmid - halfWindow, xmid + halfWindow, iq, 0, true);
				}
				thy z.row (iq)  <<=  qout.all();
			}
		}
		/*
			2. average across quefrencies
		*/
		const integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dy);
		if (numberOfQuefrencyBins > 1) {
			autoPowerCepstrum smooth = PowerCepstrum_create (thy ymax, thy ny);
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				smooth -> z.row (1)  <<=  thy z.column (iframe);
				PowerCepstrum_smooth_inplace (smooth.get(), quefrencyAveragingWindow, 1);
				thy z.column (iframe)  <<=  smooth -> z.row (1);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static autoPowerCepstrogram PowerCepstrogram_smoothRectangular_old (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);
		/*
			1. average across time
		*/
		const integer numberOfFrames = Melder_ifloor (timeAveragingWindow / my dx);
		if (numberOfFrames > 1)
			for (integer iq = 1; iq <= my ny; iq ++)
				VECsmoothByMovingAverage_preallocated (thy z.row (iq), my z.row (iq), numberOfFrames);
		/*
			2. average across quefrencies
		*/
		const integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dy);
		if (numberOfQuefrencyBins > 1) {
			autoVEC qin = raw_VEC (thy ny);
			for (integer iframe = 1; iframe <= my nx; iframe ++) {
				qin.all()  <<=  thy z.column (iframe);
				VECsmoothByMovingAverage_preallocated (thy z.column (iframe), qin.all(), numberOfQuefrencyBins);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static autoPowerCepstrogram PowerCepstrogram_smoothGaussian (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);		
		/*
			1. average across time
		*/
		const double numberOfSigmasInWindow = 4.0;
		const double numberOfFrames = timeAveragingWindow / my dx;
		if (numberOfFrames > 1.0) {
			const double sigma = numberOfFrames / numberOfSigmasInWindow;  // 2sigma -> 95.4%, 3sigma -> 99.7 % of the data
			const integer nfft = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (my nx));   // TODO: explain edge case
			autoNUMFourierTable fourierTable = NUMFourierTable_create (nfft);
			for (integer iq = 1; iq <= my ny; iq ++) {
				VECsmooth_gaussian (thy z.row (iq), my z.row (iq), sigma, fourierTable.get());
				abs_VEC_inout (thy z.row (iq));
			}
		}
		/*
			2. average across quefrencies
		*/
		const double numberOfQuefrencyBins = quefrencyAveragingWindow / my dy;
		if (numberOfQuefrencyBins > 1.0) {
			const integer nfft = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (my ny));   // TODO: explain edge case
			autoNUMFourierTable fourierTable = NUMFourierTable_create (nfft);
			const double sigma = numberOfQuefrencyBins / numberOfSigmasInWindow;  // 2sigma -> 95.4%, 3sigma -> 99.7 % of the data
			for (integer iframe = 1; iframe <= my nx; iframe ++) {
				VECsmooth_gaussian_inplace (thy z.column (iframe), sigma, fourierTable.get());
				abs_VEC_inout (thy z.column (iframe));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

autoPowerCepstrogram PowerCepstrogram_smooth (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	if (Melder_debug == -4)
		return PowerCepstrogram_smoothRectangular_old (me, timeAveragingWindow, quefrencyAveragingWindow);
	else if (Melder_debug == -5)
		return PowerCepstrogram_smoothGaussian (me, timeAveragingWindow, quefrencyAveragingWindow);
	else
		return PowerCepstrogram_smoothRectangular (me, timeAveragingWindow, quefrencyAveragingWindow);
}

autoMatrix PowerCepstrogram_to_Matrix (PowerCepstrogram me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoPowerCepstrum PowerCepstrogram_to_PowerCepstrum_slice (PowerCepstrogram me, double time) {
	try {
		integer iframe = Sampled_xToNearestIndex (me, time);
		iframe = iframe < 1 ? 1 : iframe > my nx ? my nx : iframe;
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		thy z.row (1)  <<=  my z.column (iframe);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Cepstrum not extracted.");
	}
}

autoPowerCepstrogram Matrix_to_PowerCepstrogram (Matrix me) {
	try {
		autoPowerCepstrogram thee = Thing_new (PowerCepstrogram);
		my structMatrix :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PowerCepstrogram.");
	}
}




double PowerCepstrogram_getCPPS (PowerCepstrogram me, bool subtractTrendBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType, double qminFit, double qmaxFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	try {
		autoPowerCepstrogram flattened;
		bool trendSubtracted = subtractTrendBeforeSmoothing;
		if (subtractTrendBeforeSmoothing)
			flattened = PowerCepstrogram_subtractTrend (me, qminFit, qmaxFit, lineType, fitMethod);

		autoPowerCepstrogram smooth = PowerCepstrogram_smooth (subtractTrendBeforeSmoothing ? flattened.get() : me, timeAveragingWindow, quefrencyAveragingWindow);
		if (Melder_debug == -6) { // old algorithm
			autoTable table = PowerCepstrogram_to_Table_CPP (smooth.get(), false, false, 6, 16, false, 6, pitchFloor, pitchCeiling,
				deltaF0, peakInterpolationType, qminFit, qmaxFit, lineType, fitMethod);
			const double cpps = Table_getMean (table.get(), 1); // no frame number, no time, quefrency
			return cpps;
		} else  {
			autoMatrix cpp = PowerCepstrogram_to_Matrix_CPP (smooth.get(), trendSubtracted, pitchFloor, pitchCeiling, deltaF0,
				peakInterpolationType, qminFit, qmaxFit, lineType, fitMethod);
			const double cpps = Matrix_getMean (cpp.get(), cpp -> xmin, cpp -> xmax, 5.5, 6.5); // TODO Sampled_getMean??
			return cpps;
		}
	} catch (MelderError) {
		Melder_throw (me, U": no CPPS value calculated.");
	}
}

double PowerCepstrogram_getCPPS_hillenbrand (PowerCepstrogram me, bool subtractTrendBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling) {
	try {
		autoPowerCepstrogram him;
		if (subtractTrendBeforeSmoothing)
			him = PowerCepstrogram_subtractTrend (me, 0.001, 0, kCepstrum_trendType::LINEAR, kCepstrum_trendFit::LEAST_SQUARES);

		autoPowerCepstrogram smooth = PowerCepstrogram_smooth (subtractTrendBeforeSmoothing ? him.get() : me, timeAveragingWindow, quefrencyAveragingWindow);
		autoTable table = PowerCepstrogram_to_Table_hillenbrand (smooth.get(), pitchFloor, pitchCeiling);
		const double cpps = Table_getMean (table.get(), 3);
		return cpps;
	} catch (MelderError) {
		Melder_throw (me, U": no CPPS value calculated.");
	}
}

/* End of file PowerCepstrogram.cpp */
