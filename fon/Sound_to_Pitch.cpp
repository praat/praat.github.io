/* Sound_to_Pitch.cpp
 *
 * Copyright (C) 1992-2005,2007-2012,2014-2020,2023-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

/*
 * pb 2002/07/16 GPL
 * pb 2002/10/11 removed some assertions
 * pb 2003/05/20 default time step is four times oversampling
 * pb 2003/07/02 checks on NUMrealft
 * pb 2004/05/10 better error messages
 * pb 2004/10/18 auto maxnCandidates
 * pb 2004/10/18 use of constant FFT tables speeds up AC method by a factor of 1.9
 * pb 2006/12/31 compatible with stereo sounds
 * pb 2007/01/30 loop split for stereo speeds up CC method by a factor of 6
 * pb 2008/01/19 double
 * pb 2010/12/07 compatible with sounds with any number of channels
 * pb 2011/03/08 C++
 * pb 2014/05/23 threads
 */

#include "Sound_to_Pitch.h"
#include "NUM2.h"
#include "Sound_and_Spectrum.h"

#include <set>

#define AC_HANNING  0
#define AC_GAUSS  1
#define FCC_NORMAL  2
#define FCC_ACCURATE  3

static void Sound_into_PitchFrame (Sound me, Pitch_Frame pitchFrame, double t,
	double pitchFloor, int maxnCandidates, int method, double voicingThreshold, double octaveCost,
	NUMFourierTable fftTable, double dt_window, integer nsamp_window, integer halfnsamp_window,
	integer maximumLag, integer nsampFFT, integer nsamp_period, integer halfnsamp_period,
	integer brent_ixmax, integer brent_depth, double globalPeak,
	MAT const& frame, VEC const& ac, VEC const& window, VEC const& windowR,
	double *r, INTVEC const& imax, VEC const& localMean)
{
	integer leftSample = Sampled_xToLowIndex (me, t), rightSample = leftSample + 1;
	integer startSample, endSample;

	for (integer channel = 1; channel <= my ny; channel ++) {
		/*
			Compute the local mean; look one longest period to both sides.
		*/
		startSample = rightSample - nsamp_period;
		endSample = leftSample + nsamp_period;
		Melder_assert (startSample >= 1);
		Melder_assert (endSample <= my nx);
		localMean [channel] = 0.0;
		for (integer i = startSample; i <= endSample; i ++)
			localMean [channel] += my z [channel] [i];
		localMean [channel] /= 2 * nsamp_period;

		/*
			Copy a window to a frame and subtract the local mean.
			We are going to kill the DC component before windowing.
		*/
		startSample = rightSample - halfnsamp_window;
		endSample = leftSample + halfnsamp_window;
		Melder_assert (startSample >= 1);
		Melder_assert (endSample <= my nx);
		if (method < FCC_NORMAL) {
			for (integer j = 1, i = startSample; j <= nsamp_window; j ++)
				frame [channel] [j] = (my z [channel] [i ++] - localMean [channel]) * window [j];
			for (integer j = nsamp_window + 1; j <= nsampFFT; j ++)
				frame [channel] [j] = 0.0;
		} else {
			for (integer j = 1, i = startSample; j <= nsamp_window; j ++)
				frame [channel] [j] = my z [channel] [i ++] - localMean [channel];
		}
	}

	/*
		Compute the local peak; look half a longest period to both sides.
	*/
	double localPeak = 0.0;
	if ((startSample = halfnsamp_window + 1 - halfnsamp_period) < 1)
		startSample = 1;
	if ((endSample = halfnsamp_window + halfnsamp_period) > nsamp_window)
		endSample = nsamp_window;
	for (integer channel = 1; channel <= my ny; channel ++) {
		for (integer j = startSample; j <= endSample; j ++) {
			double value = fabs (frame [channel] [j]);
			if (value > localPeak)
				localPeak = value;
		}
	}
	pitchFrame -> intensity = ( localPeak > globalPeak ? 1.0 : localPeak / globalPeak );

	/*
		Compute the correlation into the array 'r'.
	*/
	if (method >= FCC_NORMAL) {
		const double startTime = t - 0.5 * (1.0 / pitchFloor + dt_window);
		integer localSpan = maximumLag + nsamp_window;
		if ((startSample = Sampled_xToLowIndex (me, startTime)) < 1)
			startSample = 1;
		if (localSpan > my nx + 1 - startSample)
			localSpan = my nx + 1 - startSample;
		const integer localMaximumLag = localSpan - nsamp_window;
		const integer offset = startSample - 1;
		longdouble sumx2 = 0.0;   // sum of squares
		for (integer channel = 1; channel <= my ny; channel ++) {
			const double *const amp = & my z [channel] [0] + offset;
			for (integer i = 1; i <= nsamp_window; i ++) {
				const double x = amp [i] - localMean [channel];
				sumx2 += x * x;
			}
		}
		longdouble sumy2 = sumx2;   // at zero lag, these are still equal
		r [0] = 1.0;
		for (integer i = 1; i <= localMaximumLag; i ++) {
			longdouble product = 0.0;
			for (integer channel = 1; channel <= my ny; channel ++) {
				const double *const amp = & my z [channel] [0] + offset;
				const double y0 = amp [i] - localMean [channel];
				const double yZ = amp [i + nsamp_window] - localMean [channel];
				sumy2 += yZ * yZ - y0 * y0;
				for (integer j = 1; j <= nsamp_window; j ++) {
					const double x = amp [j] - localMean [channel];
					const double y = amp [i + j] - localMean [channel];
					product += x * y;
				}
			}
			r [- i] = r [i] = (double) product / sqrt ((double) sumx2 * (double) sumy2);
		}
	} else {

		/*
			The FFT of the autocorrelation is the power spectrum.
		*/
		for (integer i = 1; i <= nsampFFT; i ++)
			ac [i] = 0.0;
		for (integer channel = 1; channel <= my ny; channel ++) {
			NUMfft_forward (fftTable, VEC (& frame [channel] [1], fftTable->n));   // complex spectrum
			ac [1] += frame [channel] [1] * frame [channel] [1];   // DC component
			for (integer i = 2; i < nsampFFT; i += 2)
				ac [i] += frame [channel] [i] * frame [channel] [i] + frame [channel] [i+1] * frame [channel] [i+1];   // power spectrum
			ac [nsampFFT] += frame [channel] [nsampFFT] * frame [channel] [nsampFFT];   // Nyquist frequency
		}
		NUMfft_backward (fftTable, ac);   // autocorrelation

		/*
			Normalize the autocorrelation to the value with zero lag,
			and divide it by the normalized autocorrelation of the window.
		*/
		r [0] = 1.0;
		for (integer i = 1; i <= brent_ixmax; i ++)
			r [- i] = r [i] = ac [i + 1] / (ac [1] * windowR [i + 1]);
	}

	/*
		Register the first candidate, which is always present: voicelessness.
	*/
	pitchFrame -> candidates. resize (pitchFrame -> nCandidates = 1);   // maintain invariant; no memory allocations
	pitchFrame -> candidates [1]. frequency = 0.0;   // voiceless: always present
	pitchFrame -> candidates [1]. strength = 0.0;

	/*
		Shortcut: absolute silence is always voiceless.
		We are done for this frame.
	*/
	if (localPeak == 0.0)
		return;

	/*
		Find the strongest maxima of the correlation of this frame,
		and register them as candidates.
	*/
	imax [1] = 0;
	for (integer i = 2; i < maximumLag && i < brent_ixmax; i ++)
		if (r [i] > 0.5 * voicingThreshold &&   // not too unvoiced?
			r [i] > r [i-1] && r [i] >= r [i+1])   // maximum?
	{
		integer place = 0;

		/*
			Use parabolic interpolation for first estimate of frequency,
			and sin(x)/x interpolation to compute the strength of this frequency.
		*/
		const double dr = 0.5 * (r [i+1] - r [i-1]);
		const double d2r = (r [i] - r [i-1]) + (r [i] - r [i+1]);
				// not 2.0 * r [i] - r [i-1] - r [i+1] because of rounhding errors! (bug removed 2025-04-18)
		Melder_assert (d2r > 0);
		const double frequencyOfMaximum = 1.0 / my dx / (i + dr / d2r);
		const integer offset = - brent_ixmax - 1;
		double strengthOfMaximum = /* method & 1 ? */
			NUM_interpolate_sinc (constVEC (& r [offset + 1], brent_ixmax - offset), 1.0 / my dx / frequencyOfMaximum - offset, 30)
			/* : r [i] + 0.5 * dr * dr / d2r */;
		/*
			High values due to short windows are to be reflected around 1.
		*/
		if (strengthOfMaximum > 1.0)
			strengthOfMaximum = 1.0 / strengthOfMaximum;

		/*
			Find a place for this maximum.
		*/
		if (pitchFrame->nCandidates < maxnCandidates) {   // is there still a free place?
			pitchFrame -> candidates. resize (++ pitchFrame -> nCandidates);
			place = pitchFrame -> nCandidates;
		} else {
			/*
				Try the place of the weakest candidate so far.
			*/
			double weakest = 2.0;
			for (int iweak = 2; iweak <= maxnCandidates; iweak ++) {
				/*
					High frequencies are to be favoured
					if we want to analyze a perfectly periodic signal correctly.
				*/
				const double localStrength = pitchFrame -> candidates [iweak]. strength - octaveCost *
					NUMlog2 (pitchFloor / pitchFrame -> candidates [iweak]. frequency);
				if (localStrength < weakest) {
					weakest = localStrength;
					place = iweak;
				}
			}
			/*
				If this maximum is weaker than the weakest candidate so far, give it no place.
			*/
			if (strengthOfMaximum - octaveCost * NUMlog2 (pitchFloor / frequencyOfMaximum) <= weakest)
				place = 0;
		}
		if (place) {   // have we found a place for this candidate?
			pitchFrame -> candidates [place]. frequency = frequencyOfMaximum;
			pitchFrame -> candidates [place]. strength = strengthOfMaximum;
			imax [place] = i;
		}
	}

	/*
		Second pass: for extra precision, maximize sin(x)/x interpolation ('sinc').
	*/
	for (integer i = 2; i <= pitchFrame -> nCandidates; i ++) {
		if (method != AC_HANNING || pitchFrame -> candidates [i]. frequency > 0.0 / my dx) {
			double xmid, ymid;
			const integer offset = - brent_ixmax - 1;
			ymid = NUMimproveMaximum (constVEC (& r [offset + 1], brent_ixmax - offset), imax [i] - offset,
					pitchFrame -> candidates [i]. frequency > 0.3 / my dx ? NUM_PEAK_INTERPOLATE_SINC700 : brent_depth, & xmid);
			xmid += offset;
			pitchFrame -> candidates [i]. frequency = 1.0 / my dx / xmid;
			if (ymid > 1.0)
				ymid = 1.0 / ymid;
			pitchFrame -> candidates [i]. strength = ymid;
		}
	}
}

autoPitch Sound_to_Pitch_any (Sound me,
	int method, double periodsPerWindow,
	double dt, double pitchFloor, double pitchCeiling,
	integer maxnCandidates,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost)
{
	try {
		double t1;
		integer numberOfFrames;
		integer nsampFFT;
		double interpolation_depth;
		integer brent_ixmax, brent_depth;
		double globalPeak;

		Melder_assert (maxnCandidates >= 2);
		Melder_assert (method >= AC_HANNING && method <= FCC_ACCURATE);

		if (maxnCandidates < pitchCeiling / pitchFloor)
			maxnCandidates = Melder_ifloor (pitchCeiling / pitchFloor);

		if (dt <= 0.0)
			dt = periodsPerWindow / pitchFloor / 4.0;   // e.g. 3 periods, 75 Hz: 10 milliseconds

		switch (method) {
			case AC_HANNING:
				brent_depth = NUM_PEAK_INTERPOLATE_SINC70;
				interpolation_depth = 0.5;
				break;
			case AC_GAUSS:
				periodsPerWindow *= 2;   // because Gaussian window is twice as long
				brent_depth = NUM_PEAK_INTERPOLATE_SINC700;
				interpolation_depth = 0.25;   // because Gaussian window is twice as long
				break;
			case FCC_NORMAL:
				brent_depth = NUM_PEAK_INTERPOLATE_SINC70;
				interpolation_depth = 1.0;
				break;
			case FCC_ACCURATE:
				brent_depth = NUM_PEAK_INTERPOLATE_SINC700;
				interpolation_depth = 1.0;
				break;
		}
		volatile const double duration = my dx * my nx;   // volatile, because we need to truncate to 64 bits
		if (pitchFloor < periodsPerWindow / duration)
			Melder_throw (U"To analyse this Sound, “pitch floor” must not be less than ", periodsPerWindow / duration, U" Hz.");

		/*
			Determine the number of samples in the longest period.
			We need this to compute the local mean of the sound (looking one period in both directions),
			and to compute the local peak of the sound (looking half a period in both directions).
		*/
		const integer nsamp_period = Melder_ifloor (1.0 / my dx / pitchFloor);
		const integer halfnsamp_period = nsamp_period / 2 + 1;

		Melder_clipRight (& pitchCeiling, 0.5 / my dx);

		/*
			Determine window duration in seconds and in samples.
		*/
		const double dt_window = periodsPerWindow / pitchFloor;
		integer nsamp_window = Melder_ifloor (dt_window / my dx);
		const integer halfnsamp_window = nsamp_window / 2 - 1;
		if (halfnsamp_window < 2)
			Melder_throw (U"Analysis window too short.");
		nsamp_window = halfnsamp_window * 2;

		/*
			Determine the minimum and maximum lags.
		*/
		const integer minimumLag = std::max (2_integer, Melder_ifloor (1.0 / my dx / pitchCeiling));
		const integer maximumLag = std::min (Melder_ifloor (nsamp_window / periodsPerWindow) + 2, nsamp_window);

		/*
			Determine the number of frames.
			Fit as many frames as possible symmetrically in the total duration.
			We do this even for the forward cross-correlation method,
			because that allows us to compare the two methods.
		*/
		try {
			Sampled_shortTermAnalysis (me, method >= FCC_NORMAL ? 1.0 / pitchFloor + dt_window : dt_window, dt, & numberOfFrames, & t1);
		} catch (MelderError) {
			Melder_throw (U"The pitch analysis would give zero pitch frames.");
		}

		/*
			Create the resulting pitch contour.
		*/
		autoPitch thee = Pitch_create (my xmin, my xmax, numberOfFrames, dt, t1, pitchCeiling, maxnCandidates);

		/*
			Create (too much) space for candidates.
		*/
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const Pitch_Frame pitchFrame = & thy frames [iframe];
			Pitch_Frame_init (pitchFrame, maxnCandidates);
		}

		/*
			Compute the global absolute peak for determination of silence threshold.
		*/
		std::set<double> maximums;
		size_t nmaximums = (my nx * my ny) / 100; // 99th percentile

		globalPeak = 0.0;
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			const double mean = NUMmean (my z.row (ichan));
			for (integer i = 1; i <= my nx; i ++) {
				double value = fabs (my z [ichan] [i] - mean);

				if (maximums.size() == 0 || value >= *maximums.begin()) {
					if (maximums.size() == nmaximums) {
						maximums.erase(maximums.begin());
					}
					maximums.insert(value);
					globalPeak = value;
				}
			}
		}
		if (maximums.size() == 0)
			return thee;

        globalPeak = *maximums.begin();

		autoVEC window, windowR;
		if (method >= FCC_NORMAL) {   // for cross-correlation analysis

			nsampFFT = 0;
			brent_ixmax = Melder_ifloor (nsamp_window * interpolation_depth);

		} else {   // for autocorrelation analysis

			/*
				Compute the number of samples needed for doing FFT.
				To avoid edge effects, we have to append zeroes to the window.
				The maximum lag considered for maxima is maximumLag.
				The maximum lag used in interpolation is nsamp_window * interpolation_depth.
			*/
			nsampFFT = 1;
			while (nsampFFT < nsamp_window * (1 + interpolation_depth))
				nsampFFT *= 2;

			/*
				Create buffers for autocorrelation analysis.
			*/
			windowR. resize (nsampFFT);
			window. resize (nsamp_window);
			autoNUMFourierTable fftTable = NUMFourierTable_create (nsampFFT);

			/*
				A Gaussian or Hanning window is applied against phase effects.
				The Hanning window is 2 to 5 dB better for 3 periods/window.
				The Gaussian window is 25 to 29 dB better for 6 periods/window.
			*/
			if (method == AC_GAUSS) {   // Gaussian window
				double imid = 0.5 * (nsamp_window + 1), edge = exp (-12.0);
				for (integer i = 1; i <= nsamp_window; i ++)
					window [i] = (exp (-48.0 * (i - imid) * (i - imid) /
							(nsamp_window + 1) / (nsamp_window + 1)) - edge) / (1.0 - edge);
			} else {   // Hanning window
				for (integer i = 1; i <= nsamp_window; i ++)
					window [i] = 0.5 - 0.5 * cos (NUM2pi * i / (nsamp_window + 1));
			}

			/*
				Compute the normalized autocorrelation of the window.
			*/
			for (integer i = 1; i <= nsamp_window; i ++)
				windowR [i] = window [i];
			NUMfft_forward (fftTable.get(), windowR.get());
			windowR [1] *= windowR [1];   // DC component
			for (integer i = 2; i < nsampFFT; i += 2) {
				windowR [i] = windowR [i] * windowR [i] + windowR [i + 1] * windowR [i + 1];
				windowR [i + 1] = 0.0;   // power spectrum: square and zero
			}
			windowR [nsampFFT] *= windowR [nsampFFT];   // Nyquist frequency
			NUMfft_backward (fftTable.get(), windowR.get());   // autocorrelation
			for (integer i = 2; i <= nsamp_window; i ++)
				windowR [i] /= windowR [1];   // normalize
			windowR [1] = 1.0;   // normalize

			brent_ixmax = Melder_ifloor (nsamp_window * interpolation_depth);
		}

		autoMelderProgress progress (U"Sound to Pitch...");
		if (MelderThread_TRACING)
			Melder_casual (U"channel frame time pitch");

		MelderThread_PARALLEL (numberOfFrames, 5) {
			autoMAT frame;
			autoNUMFourierTable fftTable;
			autoVEC ac;
			if (method >= FCC_NORMAL) {   // cross-correlation
				frame = zero_MAT (my ny, nsamp_window);
			} else {   // autocorrelation
				fftTable = NUMFourierTable_create (nsampFFT);
				frame = zero_MAT (my ny, nsampFFT);
				ac = zero_VEC (nsampFFT);
			}
			autoVEC rbuffer = zero_VEC (2 * nsamp_window + 1);
			double *r = & rbuffer [1 + nsamp_window];
			autoINTVEC imax = zero_INTVEC (maxnCandidates);
			autoVEC localMean = zero_VEC (my ny);
			MelderThread_FOR (iframe) {
				Pitch_Frame pitchFrame = & thy frames [iframe];
				const double time = Sampled_indexToX (thee.get(), iframe);
				if (MelderThread_IS_MASTER) {   // then we can interact with the GUI
					const double estimatedProgress = MelderThread_ESTIMATED_PROGRESS;
					Melder_progress (0.1 + 0.8 * estimatedProgress,
						U"Sound to Pitch: analysed approximately ", Melder_iround (numberOfFrames * estimatedProgress),
						U" out of ", numberOfFrames, U" frames"
					);
				}
				Sound_into_PitchFrame (me, pitchFrame, time,
					pitchFloor, maxnCandidates, method, voicingThreshold, octaveCost,
					fftTable.get(), dt_window, nsamp_window, halfnsamp_window,
					maximumLag, nsampFFT, nsamp_period, halfnsamp_period,
					brent_ixmax, brent_depth, globalPeak,
					frame.get(), ac.get(), window.get(), windowR.get(),
					r, imax.get(), localMean.get()
				);
				if (MelderThread_TRACING)
					Melder_casual (MelderThread_CHANNEL, U" ", iframe, U" ", time, U" ", pitchFrame -> candidates [1]. frequency);
			}
		} MelderThread_ENDPARALLEL

		Melder_progress (0.95, U"Sound to Pitch: path finder");
		Pitch_pathFinder (thee.get(), silenceThreshold, voicingThreshold,
				octaveCost, octaveJumpCost, voicedUnvoicedCost, pitchCeiling, Melder_debug == 31 ? true : false);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": pitch analysis not performed.");
	}
}

autoPitch Sound_to_Pitch (Sound me, double timeStep, double pitchFloor, double pitchCeiling) {
	return Sound_to_Pitch_rawAc (me, timeStep, pitchFloor, pitchCeiling,
			15, false, 0.03, 0.45, 0.01, 0.35, 0.14);
}

autoPitch Sound_to_Pitch_rawAc (Sound me,
	double timeStep, double pitchFloor, double pitchCeiling,
	integer maxnCandidates, bool veryAccurate,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost)
{
	return Sound_to_Pitch_any (me, (int) veryAccurate, 3.0,
		timeStep, pitchFloor, pitchCeiling,
		maxnCandidates,
		silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
	);
}

autoPitch Sound_to_Pitch_rawCc (Sound me,
	double timeStep, double pitchFloor, double pitchCeiling,
	integer maxnCandidates, bool veryAccurate,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost)
{
	return Sound_to_Pitch_any (me, 2 + (int) veryAccurate, 1.0,
		timeStep, pitchFloor, pitchCeiling,
		maxnCandidates,
		silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
	);
}

autoPitch Sound_to_Pitch_filteredAc (Sound me,
	double timeStep, double pitchFloor, double pitchTop,
	integer maxnCandidates, bool veryAccurate,
	double attenuationAtTop,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost)
{
	try {
		const double lowPassCutoffFrequency = pitchTop / NUMsqrt_e (-2.0 * log (attenuationAtTop));
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, true);
			for (integer ibin = 1; ibin <= spec -> nx; ibin ++) {
				const double frequency = Sampled_indexToX (spec.get(), ibin);
				const double factor = exp (-0.5 * sqr (frequency / lowPassCutoffFrequency));
				spec -> z [1] [ibin] *= factor;
				spec -> z [2] [ibin] *= factor;
			}
			autoSound him = Spectrum_to_Sound (spec.get());
			thy z.row (1)  <<=  his z.row (1).part (1, thy nx);
		} else {
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.get(), true);
				for (integer ibin = 1; ibin <= spec -> nx; ibin ++) {
					const double frequency = Sampled_indexToX (spec.get(), ibin);
					const double factor = exp (-0.5 * sqr (frequency / lowPassCutoffFrequency));
					spec -> z [1] [ibin] *= factor;
					spec -> z [2] [ibin] *= factor;
				}
				autoSound him = Spectrum_to_Sound (spec.get());
				thy z.row (ichan)  <<=  his z.row (1).part (1, thy nx);
			}
		}
		return Sound_to_Pitch_any (thee.get(), (int) veryAccurate, 3.0,
			timeStep, pitchFloor, pitchTop,
			maxnCandidates,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	} catch (MelderError) {
		Melder_throw (me, U": pitch analysis (filtered AC) not performed.");
	}
}

autoPitch Sound_to_Pitch_filteredCc (Sound me,
	double timeStep, double pitchFloor, double pitchTop,
	integer maxnCandidates, bool veryAccurate,
	double attenuationAtTop,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost)
{
	try {
		const double lowPassCutoffFrequency = pitchTop / NUMsqrt_e (-2.0 * log (attenuationAtTop));
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, true);
			for (integer ibin = 1; ibin <= spec -> nx; ibin ++) {
				const double frequency = Sampled_indexToX (spec.get(), ibin);
				const double factor = exp (-0.5 * sqr (frequency / lowPassCutoffFrequency));
				spec -> z [1] [ibin] *= factor;
				spec -> z [2] [ibin] *= factor;
			}
			autoSound him = Spectrum_to_Sound (spec.get());
			thy z.row (1)  <<=  his z.row (1).part (1, thy nx);
		} else {
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.get(), true);
				for (integer ibin = 1; ibin <= spec -> nx; ibin ++) {
					const double frequency = Sampled_indexToX (spec.get(), ibin);
					const double factor = exp (-0.5 * sqr (frequency / lowPassCutoffFrequency));
					spec -> z [1] [ibin] *= factor;
					spec -> z [2] [ibin] *= factor;
				}
				autoSound him = Spectrum_to_Sound (spec.get());
				thy z.row (ichan)  <<=  his z.row (1).part (1, thy nx);
			}
		}
		return Sound_to_Pitch_any (thee.get(), 2 + (int) veryAccurate, 1.0,
			timeStep, pitchFloor, pitchTop,
			maxnCandidates,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	} catch (MelderError) {
		Melder_throw (me, U": pitch analysis (filtered CC) not performed.");
	}
}

/* End of file Sound_to_Pitch.cpp */
