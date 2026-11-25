/* Sound_and_LPC.cpp
 *
 * Copyright (C) 1994-2025 David Weenink
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

#include "SoundFrames.h"
#include "Sound_and_LPC.h"
#include "Sound_extensions.h"
#include "Spectrum.h"
#include "NUM2.h"

// TODO Remove the Sound_into_LPC variants. Only preemplasis here

void checkLPCAnalysisParameters_e (double sound_dx, integer sound_nx, double physicalAnalysisWidth, integer predictionOrder) {
	volatile const double physicalDuration = sound_dx * sound_nx;
	Melder_require (physicalAnalysisWidth <= physicalDuration,
		U"Your sound is shorter than two window lengths. "
		"Either your sound is too short or your window is too long."
	);
	// we round the minimum duration to be able to use asserterror in testing scripts.
	conststring32 minimumDurationRounded = Melder_fixed (predictionOrder * sound_dx , 5);
	const integer approximateNumberOfSamplesPerWindow = Melder_iroundDown (physicalAnalysisWidth / sound_dx);
	Melder_require (approximateNumberOfSamplesPerWindow > predictionOrder,
		U"Analysis window duration too short. For a prediction order of ", predictionOrder,
		U", the analysis window duration should be greater than ", minimumDurationRounded,
		U" s. Please increase the analysis window duration or lower the prediction order."
	);
}

static void Sound_to_LPC_common_e (constSound inputSound, int predictionOrder, double effectiveAnalysisWidth, double dt,
	double preEmphasisFrequency, autoSound& outputSound, autoLPC& outputLPC)
{
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth2 (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (inputSound -> dx, inputSound -> nx, physicalAnalysisWidth, predictionOrder);
		integer numberOfFrames;
		double t1;
		autoSound sound = Data_copy (inputSound);
		if (preEmphasisFrequency > 0) // TODO check 
			Sound_preEmphasize_inplace (sound.get(), preEmphasisFrequency);
		outputSound = sound.move();
		Sampled_shortTermAnalysis (outputSound.get(), physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		autoLPC lpc = LPC_createCompletelyInitialized (outputSound -> xmin, outputSound -> xmax, numberOfFrames, dt, t1, predictionOrder, outputSound -> dx);
		outputLPC = lpc.move();
	} catch (MelderError) {
		Melder_throw (inputSound, U": Sound and/or LPC not created from specification.");
	}
}

/*********************** Autocorrelation method *************************************************************/

void soundFrameIntoLPCFrame_auto (VEC soundFrame, LPC_Frame lpcFrame, VEC a, VEC r, VEC rc, integer& info) {
	integer order = lpcFrame -> nCoefficients, orderp1 = order + 1;
	Melder_assert (a.size >= orderp1);
	Melder_assert (r.size >= orderp1);
	Melder_assert (rc.size >= orderp1);
	info = 0;
	/*
		Compute the autocorrelations
	*/
	VEC x = soundFrame;
	for (integer i = 1; i <= orderp1; i ++)
		r [i] = NUMinner (x.part (1, x.size - i + 1), x.part (i, x.size));
	if (r [1] == 0.0) {
		/*
			The sound frame contains only zero's
		*/
		lpcFrame -> gain = 0.0;
		lpcFrame -> resize (0_integer);   // maintain invariant
		info = 1;
		return;
	}
	a [1] = 1.0;
	a [2] = rc [1] = - r [2] / r [1];
	double gain = r [1] + r [2] * rc [1];
	lpcFrame -> gain = gain;
	integer i = 1;
	for (i = 2; i <= order; i ++) {
		longdouble s = 0.0;
		for (integer j = 1; j <= i; j ++)
			s += r [i - j + 2] * a [j];
		rc [i] = - s / gain;
		for (integer j = 2; j <= i / 2 + 1; j ++) {
			const double at = a [j] + rc [i] * a [i - j + 2];
			a [i - j + 2] += rc [i] * a [j];
			a [j] = at;
		}
		a [i + 1] = rc [i];
		gain += rc [i] * s;
		if (gain <= 0.0) {
			info = 2;
			break;
		}
		lpcFrame -> gain = gain;
	}
	const integer numberOfCoefficients = i - 1;
	lpcFrame -> resize (numberOfCoefficients);
	lpcFrame -> a.part (1, numberOfCoefficients)  <<=  a.part (2, numberOfCoefficients + 1);
}

void Sound_into_LPC_auto (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, outputLPC);
	const integer thresholdNumberOfFramesPerThread = 40, order = outputLPC -> maxnCoefficients;

	MelderThread_PARALLEL (outputLPC -> nx, thresholdNumberOfFramesPerThread) {
		integer info;
		autoVEC a = raw_VEC (order + 1), r = raw_VEC (order + 1), rc = raw_VEC (order + 1);
		autoSoundFrames soundFrames = SoundFrames_create_fixedSampling (me, outputLPC, effectiveAnalysisWidth,
				kSound_windowShape::GAUSSIAN_2, true);
		MelderThread_FOR (iframe) {
			const LPC_Frame lpcFrame = & outputLPC -> d_frames [iframe];
			VEC soundFrame = soundFrames -> getMonoFrame (iframe);
			soundFrameIntoLPCFrame_auto (soundFrame, lpcFrame, a.get(), r.get(), rc.get(), info);
		}
	} MelderThread_ENDPARALLEL
}

autoLPC Sound_to_LPC_auto (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoSound inputSound;
		autoLPC outputLPC;
		Sound_to_LPC_common_e (me, predictionOrder, effectiveAnalysisWidth, dt,	preEmphasisFrequency, inputSound, outputLPC);
		Sound_into_LPC_auto (inputSound.get(), outputLPC.get(), effectiveAnalysisWidth);
		return outputLPC;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (auto) created.");
	}
}

/*********************** Covariance method *************************************************************/

void soundFrameIntoLPCFrame_covar (VEC soundFrame, LPC_Frame lpcFrame, VEC a, VEC b, VEC grc, VEC beta, VEC cc, integer & info) {
	const integer n = soundFrame.size, order = lpcFrame -> nCoefficients;

	if (lpcFrame -> nCoefficients == 0) {
		info = 6;
		return;
	}		
	constVEC x = soundFrame;
	/*
		Compute the covariances
	*/
	constVECVU xi = x.part (order + 1, n), xim1 = x.part (order, n - 1);
	double gain = NUMinner (xi, xi);
	cc [1] = NUMinner (xi, xim1);
	cc [2] = NUMinner (xim1, xim1);

	if (gain == 0.0) {
		info = 1;
		lpcFrame -> gain = gain;
		lpcFrame -> resize (0_integer);   //maintain invariant
		return;
	}
	b  <<=  0.0;
	b [1] = 1.0;
	beta [1] = cc [2];
	a [1] = 1.0;
	a [2] = grc [1] = -cc [1] / cc [2];
	lpcFrame -> gain = gain += grc [1] * cc [1];
	integer i = 2;
	for (i = 2; i <= order; i ++) { // 130
		for (integer j = 1; j <= i; j ++)
			cc [i - j + 2] = cc [i - j + 1] + x [order - i + 1] * x [order - i + j] - x [n - i + 1] * x [n - i + j];
		
		// cc[1]=0.0; for (integer j = m + 1; j <= n; j ++) cc [1] += x [j - i] * x [j];
		cc [1] = NUMinner (x.part (order + 1 - i, n - i), x.part (order + 1, n)); //30
			
		b [i * (i + 1) / 2] = 1.0;
		for (integer j = 1; j <= i - 1; j ++) { // 70
			if (beta [j] < 0.0) {
				info = 2;
				goto end;
			} else if (beta [j] == 0.0)
				continue;

			double gam = 0.0;
			for (integer k = 1; k <= j; k ++)
				gam += cc [k + 1] * b [j * (j - 1) / 2 + k]; // 50
			gam /= beta [j];
			for (integer k = 1; k <= j; k ++)
				b [i * (i - 1) / 2 + k] -= gam * b [j * (j - 1) / 2 + k]; // 60
		}

		beta [i] = 0.0;
		for (integer j = 1; j <= i; j ++)
			beta [i] += cc [j + 1] * b [i * (i - 1) / 2 + j]; // 80
		if (beta [i] <= 0.0) {
			info = 3;
			break;
		}
		double s = 0.0;
		for (integer j = 1; j <= i; j ++)
			s += cc [j] * a [j]; // 100
		grc [i] = -s / beta [i];

		for (integer j = 2; j <= i; j ++)
			a [j] += grc [i] * b [i * (i - 1) / 2 + j - 1]; // 110
		a [i + 1] = grc [i];
		s = grc [i] * grc [i] * beta [i];
		gain -= s;
		if (gain <= 0.0) {
			info = 4;
			break;
		}
		lpcFrame -> gain = gain;
	}
end:
	const integer numberOfCoefficients = i - 1;
	lpcFrame -> resize (numberOfCoefficients);
	lpcFrame -> a.part (1, numberOfCoefficients)  <<=  a.part (2, numberOfCoefficients + 1);
	
}

void Sound_into_LPC_covar (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, outputLPC);
	const integer thresholdNumberOfFramesPerThread = 40, order = outputLPC -> maxnCoefficients;

	MelderThread_PARALLEL (outputLPC -> nx, thresholdNumberOfFramesPerThread) {
		autoSoundFrames soundFrames = Thing_new (SoundFrames);
		soundFrames -> init_fixedSampling (me, outputLPC, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2, true);
		integer info;
		autoVEC a = raw_VEC (order + 1);
		autoVEC b = raw_VEC (order * (order + 1) / 2);
		autoVEC grc = raw_VEC (order);
		autoVEC beta = raw_VEC (order);
		autoVEC cc = raw_VEC (order + 1);
		MelderThread_FOR (iframe) {
			const LPC_Frame lpcFrame = & outputLPC -> d_frames [iframe];
			VEC soundFrame = soundFrames -> getMonoFrame (iframe);
			soundFrameIntoLPCFrame_covar (soundFrame, lpcFrame, a.get(), b.get(), grc.get(),
					beta.get(), cc.get(), info);
		}
	} MelderThread_ENDPARALLEL
}

autoLPC Sound_to_LPC_covar (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoSound inputSound;
		autoLPC outputLPC;
		Sound_to_LPC_common_e (me, predictionOrder, effectiveAnalysisWidth, dt,	preEmphasisFrequency, inputSound, outputLPC);
		Sound_into_LPC_covar (inputSound.get(), outputLPC.get(), effectiveAnalysisWidth);
		return outputLPC;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (covar) created.");
	}
}

/*********************** Burg method *************************************************************/

void soundFrameIntoLPCFrame_burg (VEC soundFrame, LPC_Frame lpcFrame, VEC b1, VEC b2,
	VEC aa, integer &info)
{
	const integer n = soundFrame.size, order = lpcFrame ->nCoefficients;
	VEC x = soundFrame;
	VEC a = lpcFrame -> a.get(); // always safe
	aa  <<=  0.0;
	if (n <= 2) {
		a [1] = -1.0;
		lpcFrame -> gain = ( n == 2 ? 0.5 * (x [1] * x [1] + x [2] * x [2]) : x [1] * x [1] );
		lpcFrame -> resize (1_integer);
	}

	// (3)

	double gain = NUMinner (x, x);

	if (gain == 0.0) {
		info = 1;
		lpcFrame -> gain = 0.0;
		lpcFrame -> resize (0_integer);
		return;
	}
	// (9)

	b1 [1] = x [1];
	for (integer j = 2; j <= n - 1; j ++)
		b1 [j] = b2 [j - 1] = x [j];
	b2 [n - 1] = x [n];

	for (integer i = 1; i <= order; i ++) {
		// (7)

		/*
			longdouble num = 0.0, denum = 0.0;
			for (integer j = 1; j <= n - i; j ++) {
				num += b1 [j] * b2 [j];
				denum += b1 [j] * b1 [j] + b2 [j] * b2 [j];
			}
		*/
		VEC b1part = b1.part (1, n - i), b2part = b2.part (1, n - i);
		const double num = NUMinner (b1part, b2part);
		const double denum = NUMinner (b1part, b1part) + NUMinner (b2part, b2part);
		
		if (denum <= 0.0) { // part of sound has zero amplitude
			info = 1;
			lpcFrame -> gain = 0.0;
			lpcFrame -> resize (0_integer);
			return;	// warning ill-conditioned
		}
		a [i] = 2.0 * num / denum;

		// (10)

		gain *= 1.0 - a [i] * a [i];

		// (5)

		for (integer j = 1; j <= i - 1; j ++)
			a [j] = aa [j] - a [i] * aa [i - j];

		if (i < order) {

			// (8) Watch out: i -> i+1

			for (integer j = 1; j <= i; j ++)
				aa [j] = a [j];
			for (integer j = 1; j <= n - i - 1; j ++) {
				b1 [j] -= aa [i] * b2 [j];
				b2 [j] = b2 [j + 1] - aa [i] * b1 [j + 1];
			}
		}
	}
	lpcFrame -> gain = gain; // djmw 20251021 no need to multiply by n!
	for (integer i = 1; i <= lpcFrame -> nCoefficients; i ++)
		a [i] = - a [i];
}

void Sound_into_LPC_burg (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, outputLPC);
	const integer thresholdNumberOfFramesPerThread = 40, order = outputLPC -> maxnCoefficients;

	MelderThread_PARALLEL (outputLPC -> nx, thresholdNumberOfFramesPerThread) {
		autoSoundFrames soundFrames = SoundFrames_create_fixedSampling (me, outputLPC, effectiveAnalysisWidth,
			kSound_windowShape::GAUSSIAN_2, true);
		integer info;
		autoVEC aa = raw_VEC (order);
		autoVEC b1 = raw_VEC (soundFrames -> soundFrameSize);
		autoVEC b2 = raw_VEC (soundFrames -> soundFrameSize);
		MelderThread_FOR (iframe) {
			const LPC_Frame lpcFrame = & outputLPC -> d_frames [iframe];
			VEC soundFrame = soundFrames -> getMonoFrame (iframe);
			soundFrameIntoLPCFrame_burg (soundFrame, lpcFrame, b1.get(), b2.get(), aa.get(), info);
		}
	} MelderThread_ENDPARALLEL
}

autoLPC Sound_to_LPC_burg (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		autoSound inputSound;
		autoLPC outputLPC;
		Sound_to_LPC_common_e (me, predictionOrder, effectiveAnalysisWidth, dt,	preEmphasisFrequency, inputSound, outputLPC);
		Sound_into_LPC_burg (inputSound.get(), outputLPC.get(), effectiveAnalysisWidth);
		return outputLPC;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (burg) created.");
	}
}

/*********************** Marple method *************************************************************/

void soundFrameIntoLPCFrame_marple (VEC soundFrame, LPC_Frame lpcFrame, VEC c, VEC d, VEC r,
	double tol1, double tol2, integer& info)
{
	const integer mmax = lpcFrame -> nCoefficients, n = soundFrame.size;
	VEC x = soundFrame;
	
	info = 0;
	VEC a = lpcFrame -> a.get();

	double e0 = 2.0 * NUMsum2 (x);
	lpcFrame -> gain = 0.0;
	integer m = 1;
	if (e0 == 0.0) {
		info = 1;
		lpcFrame -> resize (0_integer);
		return;
	}
	double q1 = 1.0 / e0;
	double q2 = q1 * x [1], q = q1 * x [1] * x [1], w = q1 * x [n] * x [n];
	double v = q, u = w;
	double den = 1.0 - q - w;
	double q4 = 1.0 / den, q5 = 1.0 - q, q6 = 1.0 - w;
	double h = q2 * x [n], s = h;
	double gain = e0 * den;
	q1 = 1.0 / gain;
	c [1] = q1 * x [1];
	d [1] = q1 * x [n];
	double s1 = 0.0;
	for (integer k = 1; k <= n - 1; k ++)
		s1 += x [k + 1] * x [k];
	r [1] = 2.0 * s1;
	a [1] = - q1 * r [1];
	gain *= (1.0 - a [1] * a [1]);
	while (m < mmax) {
		const double eOld = gain;
		double f = x [m + 1], b = x [n - m]; // n-1 ->n-m
		for (integer k = 1; k <= m; k ++) {
			// n-1 -> n-m
			f += x [m + 1 - k] * a [k];
			b += x [n - m + k] * a [k];
		}
		q1 = 1.0 / gain;
		q2 = q1 * f;
		const double q3 = q1 * b;
		for (integer k = m; k >= 1; k--) {
			c [k + 1] = c [k] + q2 * a [k];
			d [k + 1] = d [k] * q3 * a [k];
		}
		c [1] = q2;
		d [1] = q3;
		const double q7 = s * s;
		double y1 = f * f;
		const double y2 = v * v;
		const double y3 = b * b;
		const double y4 = u * u;
		double y5 = 2.0 * h * s;
		q += y1 * q1 + q4 * (y2 * q6 + q7 * q5 + v * y5);
		w += y3 * q1 + q4 * (y4 * q5 + q7 * q6 + u * y5);
		h = s = u = v = 0.0;
		for (integer k = 0; k <= m; k ++) {
			h += x [n - m + k] * c [k + 1];
			s += x [n - k] * c [k + 1];
			u += x [n - k] * d [k + 1];
			v += x [k + 1] * c [k + 1];
		}
		q5 = 1.0 - q;
		q6 = 1.0 - w;
		den = q5 * q6 - h * h;
		if (den <= 0.0) {
			info = 2;
			goto end; // 2: ill-conditioning
		}
		q4 = 1.0 / den;
		q1 *= q4;
		const double alf = 1.0 / (1.0 + q1 * (y1 * q6 + y3 * q5 + 2.0 * h * f * b));
		gain *= alf;
		y5 = h * s;
		double c1 = q4 * (f * q6 + b * h);
		double c2 = q4 * (b * q5 + h * f);
		const double c3 = q4 * (v * q6 + y5);
		const double c4 = q4 * (s * q5 + v * h);
		const double c5 = q4 * (s * q6 + h * u);
		const double c6 = q4 * (u * q5 + y5);
		for (integer k = 1; k <= m; k ++)
			a [k] = alf * (a [k] + c1 * c [k + 1] + c2 * d [k + 1]);
		for (integer k = 1; k <= m / 2 + 1; k ++) {
			s1 = c [k];
			const double s2 = d [k], s3 = c [m + 2 - k], s4 = d [m + 2 - k];

			c [k] += c3 * s3 + c4 * s4;
			d [k] += c5 * s3 + c6 * s4;
			if (m + 2 - k == k)
				continue;
			c [m + 2 - k] += c3 * s1 + c4 * s2;
			d [m + 2 - k] += c5 * s1 + c6 * s2;
		}
		m ++;
		c1 = x [n + 1 - m];
		c2 = x [m];
		double delta = 0.0;
		for (integer k = m - 1; k >= 1; k--) {
			r [k + 1] = r [k] - x [n + 1 - k] * c1 - x [k] * c2;
			delta += r [k + 1] * a [k];
		}
		s1 = 0.0;
		for (integer k = 1; k <= n - m; k ++)
			s1 += x [k + m] * x [k];
		r [1] = 2.0 * s1;
		delta += r [1];
		q2 = - delta / gain;
		a [m] = q2;
		for (integer k = 1; k <= m / 2; k ++) {
			s1 = a [k];
			a [k] += q2 * a [m - k];
			if (k == m - k)
				continue;
			a [m - k] += q2 * s1;
		}
		y1 = q2 * q2;
		gain *= 1.0 - y1;
		if (y1 >= 1.0) {
			info = 3;
			break; // |a [m]| > 1
		}
		if (gain < e0 * tol1) {
			info = 4;
			break;
		}
		if (eOld - gain < eOld * tol2) {
			info = 5;
			break;
		}
	}
	lpcFrame -> resize (m);
end:
	lpcFrame -> gain = gain * 0.5;   // because e0 is twice the energy
	if (!(info == 0 || info == 4 || info == 5))
		lpcFrame -> resize (m - 1);
}

void Sound_into_LPC_marple (constSound me, mutableLPC outputLPC, double effectiveAnalysisWidth, 
	double tol1, double tol2)
{
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, outputLPC);
	const integer thresholdNumberOfFramesPerThread = 40, order = outputLPC -> maxnCoefficients;

	MelderThread_PARALLEL (outputLPC -> nx, thresholdNumberOfFramesPerThread) {
		integer info;
		autoSoundFrames soundFrames = SoundFrames_create_fixedSampling (me, outputLPC, effectiveAnalysisWidth,
				kSound_windowShape::GAUSSIAN_2, true);
		autoVEC c = raw_VEC (order + 1);
		autoVEC d = raw_VEC (order + 1);
		autoVEC r = raw_VEC (order + 1);
		MelderThread_FOR (iframe) {
			const LPC_Frame lpcFrame = & outputLPC -> d_frames [iframe];
			VEC soundFrame = soundFrames -> getMonoFrame (iframe);
			soundFrameIntoLPCFrame_marple (soundFrame, lpcFrame, c.get(), d.get(), r.get(), tol1, tol2, info);
		}
	} MelderThread_ENDPARALLEL
}

autoLPC Sound_to_LPC_marple (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, 
	double preEmphasisFrequency, double tol1, double tol2)
{
	try {
		autoSound inputSound;
		autoLPC outputLPC;
		Sound_to_LPC_common_e (me, predictionOrder, effectiveAnalysisWidth, dt,	preEmphasisFrequency, inputSound, outputLPC);
		Sound_into_LPC_marple (inputSound.get(), outputLPC.get(), effectiveAnalysisWidth, tol1, tol2);
		return outputLPC;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (marple) created.");
	}
}

/*********************** PLP (Hermansky) method *************************************************************/

/*
void structSoundFrameIntoLPCFramePLP :: getFilterCharacteristics () {
	struct structCriticalBandFilter {
		double fb, fm, fe;
		integer i1, i2;
		integer iv1, iv2;
	};
	const double nyquistFrequency = 0.5 / frameAsSound -> dx;
	const double df = 1.0 / (frameAsSound -> dx * numberOfFourierSamples);
	const double fmax_bark = hertzToBark (nyquistFrequency);
	numberOfCriticalBandFilters = Melder_ifloor (fmax_bark) + 2;
	const double df_bark = fmax_bark / (numberOfCriticalBandFilters - 1);
	autovector<struct structCriticalBandFilter> filters = newvectorzero<struct structCriticalBandFilter> (numberOfCriticalBandFilters);
	for (integer ifilter = 1; ifilter <= numberOfCriticalBandFilters; ifilter ++) {
		struct structCriticalBandFilter *fstruct = & filters [ifilter];
		const double fm_bark = ifilter * df_bark;
		const double fb_bark = Melder_clippedLeft (0.0, fm_bark - 2.5);
		const double fe_bark = Melder_clippedRight (fm_bark + 1.3, fmax_bark);
		fstruct -> fb = barkToHertz (fb_bark);
		fstruct -> fe = barkToHertz (fe_bark);
		fstruct -> fm = barkToHertz (fm_bark);

	}

}*/

/*********************** Robust method (LPC & Sound) **********************************************/

Thing_implement (RobustLPCWorkspace, Thing, 0);

void structRobustLPCWorkspace :: init (constLPC inputLPC, constSound inputSound, mutableLPC outputLPC,
	double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double k_stdev, integer itermax, double tol, bool wantLocation)
{
	const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	const integer soundFrameSize = getSoundFrameSize (physicalAnalysisWidth, inputSound -> dx);
	our order = inputLPC -> maxnCoefficients;
	our inputLPC = inputLPC;
	our outputLPC = outputLPC;
	our k_stdev = k_stdev;
	our itermax = itermax;
	our tol1 = tol;
	our wantLocation = wantLocation;
	our wantScale = true;
	our location = 0.0;
	our huber_iterations = 5;
	our error = raw_VEC (soundFrameSize);
	our sampleWeights = raw_VEC (soundFrameSize);
	coefficients = raw_VEC (order);
	covariancesw = raw_VEC (order);
	covarmatrixw = raw_MAT (order, order);
	svd = SVD_create (order, order);
	SVD_setTolerance (svd.get(), tolSVD);
	computedSVDworksize = SVD_getWorkspaceSize (svd.get());
	svdwork1 = raw_VEC (computedSVDworksize);
	svdwork2 = raw_VEC (order);
	filterMemory = raw_VEC (order);
	huberwork = raw_VEC (soundFrameSize);
}

void structRobustLPCWorkspace :: resize () {
	if (currentPredictionOrder == svd -> numberOfColumns)
		return;
	Melder_assert (currentPredictionOrder <= order);
	coefficients.resize (currentPredictionOrder);
	covariancesw.resize (currentPredictionOrder);
	covarmatrixw.resize (currentPredictionOrder, currentPredictionOrder);
	SVD_resizeWithinOldBounds (svd.get(), order, order, currentPredictionOrder, currentPredictionOrder);
}

void structRobustLPCWorkspace :: setSampleWeights () {
	for (integer isamp = 1 ; isamp <= error.size; isamp ++) {
		const double absDiff = fabs (error [isamp] - location);
		sampleWeights [isamp] = std::max (1.0, k_stdev * scale / absDiff);
	}
}

void structRobustLPCWorkspace :: setCovariances () {
	MATVU covar = MATVU (covarmatrixw.part (1, currentPredictionOrder, 1, currentPredictionOrder));
	for (integer i = 1; i <= currentPredictionOrder; i ++) {
		for (integer j = i; j <= currentPredictionOrder; j ++) {
			/*
				The following inner loop will take the most CPU time of all the robust calculations
			*/
			longdouble cv1 = 0.0;
			for (integer k = currentPredictionOrder + 1; k <= soundFrame.size; k ++)
				cv1 += soundFrame [k - j] * soundFrame [k - i] * sampleWeights [k];
			covar [i] [j] = covar [j] [i] = cv1;
		}
		longdouble cv2 = 0.0;
		for (integer k = currentPredictionOrder + 1; k <= soundFrame.size; k ++)
			cv2 += soundFrame [k - i] * soundFrame [k] *  sampleWeights [k];
		covariancesw [i] = - cv2;
	}
}

void structRobustLPCWorkspace :: solvelpc () {
	svd -> u.all()  <<=  covarmatrixw.all();
	svdwork2.resize (currentPredictionOrder);
	SVD_compute (svd.get(), svdwork1.get());
	SVD_solve_preallocated (svd.get(), covariancesw.get(), coefficients.get(), svdwork2.get());
	coefficients.resize (currentPredictionOrder);   // maintain invariant
}

void structRobustLPCWorkspace :: inputFrameIntoOutputFrame (LPC_Frame inputLPCFrame, LPC_Frame outputLPCFrame, VEC soundFrame, integer& info) {
	our soundFrame = soundFrame;
	currentPredictionOrder = inputLPCFrame -> nCoefficients;
	outputLPCFrame -> resize (currentPredictionOrder);
	outputLPCFrame -> gain = inputLPCFrame -> gain;
	info = 0;
	if (currentPredictionOrder == 0) // is empty frame ?
		return;
	for (integer i = 1; i <= currentPredictionOrder; i ++)
		outputLPCFrame -> a [i] = inputLPCFrame -> a [i];
	
	VEC inout_a = outputLPCFrame -> a.get();
	iter = 0;
	scale = 1e308;
	bool farFromScale = true;
	resize ();
	filterMemory.resize (currentPredictionOrder);
	do {
		const double previousScale = scale;
		error.all()  <<=  soundFrame;
		VECfilterInverse_inplace (error.get(), inout_a, filterMemory.get());
		NUMstatistics_huber (error.get(), & location, wantLocation, & scale, wantScale, k_stdev, 
				tol1, huber_iterations, huberwork.get());
		setSampleWeights ();

		setCovariances ();
		/*
			Solve C a = [-] c
		*/
		try {
			solvelpc ();
			inout_a  <<=  coefficients.all();
			farFromScale = ( fabs (scale - previousScale) > std::max (tol1 * fabs (scale), NUMeps) );
		} catch (MelderError) {
			Melder_clearError (); // No change could be made
			info = 2; // solvelpc in error
			inputLPCFrame -> copy (outputLPCFrame);
			return;
		}
	} while (++ iter < itermax && farFromScale);
	info = 3; // maximum number of iterations
}

autoRobustLPCWorkspace RobustLPCWorkspace_create (constLPC inputLPC, constSound inputSound,
	mutableLPC outputLPC, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double k_stdev, integer itermax, double tol, bool wantlocation)
{
	try {
		autoRobustLPCWorkspace me = Thing_new (RobustLPCWorkspace);
		my init (inputLPC, inputSound, outputLPC, effectiveAnalysisWidth, windowShape, k_stdev, itermax, tol, wantlocation);
		my wantScale = true; // explicit
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create RobustLPCWorkspace.");
	}
}

void soundFrameAndLPCFrameIntoLPCFrame (VEC soundFrame, LPC_Frame inputLPCFrame, LPC_Frame outputLPCFrame,
	RobustLPCWorkspace ws, integer& info)
{
	const integer currentPredictionOrder = inputLPCFrame -> nCoefficients;
	ws -> currentPredictionOrder = currentPredictionOrder;
	ws -> inputFrameIntoOutputFrame (inputLPCFrame, outputLPCFrame, soundFrame, info);
}

void LPC_and_Sound_into_LPC_robust (constLPC inputLPC, constSound inputSound, mutableLPC outputLPC,
	double effectiveAnalysisWidth, double k_stdev, integer itermax, double tol, bool wantlocation)
{
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (inputSound, outputLPC);
	Sampleds_requireEqualDomainsAndSampling (inputLPC, outputLPC);
	const integer thresholdNumberOfFramesPerThread = 40, order = outputLPC -> maxnCoefficients;

	MelderThread_PARALLEL (outputLPC -> nx, thresholdNumberOfFramesPerThread) {
		autoSoundFrames soundFrames = SoundFrames_create_fixedSampling (inputSound, outputLPC, effectiveAnalysisWidth,
				kSound_windowShape::GAUSSIAN_2, true);
		integer info;
		autoRobustLPCWorkspace ws = RobustLPCWorkspace_create (inputLPC, inputSound, outputLPC,
				effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2, k_stdev, itermax, tol, wantlocation);
		MelderThread_FOR (iframe) {
			const LPC_Frame outputLPCFrame = & outputLPC -> d_frames [iframe];
			const LPC_Frame inputLPCFrame  = & inputLPC  -> d_frames [iframe];
			VEC soundFrame = soundFrames -> getMonoFrame (iframe);
			soundFrameAndLPCFrameIntoLPCFrame (soundFrame, inputLPCFrame, outputLPCFrame, ws.get(), info);
		}
	} MelderThread_ENDPARALLEL
}

autoLPC LPC_and_Sound_to_LPC_robust (constLPC inputLPC, constSound inputSound, double effectiveAnalysisWidth,
	double preEmphasisFrequency, double k_stdev, integer itermax, double tol, bool wantlocation)
{
	try {
		Sound_and_LPC_require_equalDomainsAndSamplingPeriods (inputSound, inputLPC);
		autoSound sound = Data_copy (inputSound);
		if (preEmphasisFrequency >= 0.0)
			Sound_preEmphasize_inplace (sound.get(), preEmphasisFrequency);
		autoLPC outputLPC = Data_copy (inputLPC);
		LPC_and_Sound_into_LPC_robust (inputLPC, sound.get(), outputLPC.get(), effectiveAnalysisWidth,
				k_stdev, itermax, tol, wantlocation);
		return outputLPC;
	} catch (MelderError) {
		Melder_throw (inputLPC, U" and ", inputSound,  U": no LPC (robust) created.");
	}
}

/*********************** Robust method (Sound) ******************************************************/


void Sound_into_LPC_robust (constSound inputSound, mutableLPC outputLPC, double effectiveAnalysisWidth, double k_stdev, integer itermax, double tol, bool wantlocation)
{
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (inputSound, outputLPC);
	autoLPC intermediateLPC = Data_copy (outputLPC);
	Sound_into_LPC_auto (inputSound, intermediateLPC.get(), effectiveAnalysisWidth);
	LPC_and_Sound_into_LPC_robust (intermediateLPC.get(), inputSound, outputLPC, effectiveAnalysisWidth,
			k_stdev, itermax, tol, wantlocation);
}

autoLPC Sound_to_LPC_robust (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt,
	double preEmphasisFrequency, double k_stdev, integer itermax, double tol, bool wantlocation)
{
	try {
		autoSound emphasizedSound;
		autoLPC outputLPC;
		Sound_to_LPC_common_e (me, predictionOrder, effectiveAnalysisWidth, dt, preEmphasisFrequency, emphasizedSound, outputLPC);
		Sound_into_LPC_robust (emphasizedSound.get(), outputLPC.get(), effectiveAnalysisWidth, k_stdev,
				itermax, tol, wantlocation);
		return outputLPC;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (robust) created.");
	}
}

/*********************** (inverse) filtering ******************************/

static void LPC_Frame_Sound_filter (constLPC_Frame me, mutableSound thee, integer channel) {
	const VEC y = thy z.row (channel);
	for (integer i = 1; i <= thy nx; i ++) {
		const integer m = ( i > my nCoefficients ? my nCoefficients : i - 1 );   // ppgb: what is m?
		for (integer j = 1; j <= m; j ++)
			y [i] -= my a [j] * y [i - j];
	}
}

autoSound LPC_Sound_filterInverse (constLPC me, constSound thee) {
	try {
		Melder_require (my samplingPeriod == thy dx,
			U"The sampling frequencies should be equal.");
		Melder_require (my xmin == thy xmin && thy xmax == my xmax,
			U"The domains of LPC and Sound should be equal.");
		
		autoSound him = Data_copy (thee);
		VEC source = his z.row (1);
		VEC sound = thy z.row (1);
		for (integer isamp = 1; isamp <= his nx; isamp ++) {
			const double sampleTime = Sampled_indexToX (him.get(), isamp);
			const integer frameNumber = Sampled_xToNearestIndex (me, sampleTime);
			if (frameNumber < 1 || frameNumber > my nx) {
				source [isamp] = 0.0;
				continue;
			}
			const LPC_Frame frame = & my d_frames [frameNumber];
			const integer maximumFilterDepth = frame -> nCoefficients;
			const integer maximumSoundDepth = isamp - 1;
			const integer usableDepth = std::min (maximumFilterDepth, maximumSoundDepth);
			for (integer icoef = 1; icoef <= usableDepth; icoef ++)
				source [isamp] += frame -> a [icoef] * sound [isamp - icoef];
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/*
	Gain used as a constant amplitude multiplier within a frame of duration my dx.
	future alternative: convolve gain with a smoother.
*/
autoSound LPC_Sound_filter (constLPC me, constSound thee, bool useGain) {
	try {
		const double xmin = std::max (my xmin, thy xmin);
		const double xmax = std::min (my xmax, thy xmax);
		Melder_require (xmin < xmax,
			U"Domains of Sound [", thy xmin, U",", thy xmax, U"] and LPC [",
			my xmin, U",", my xmax, U"] should overlap."
		);
		/*
			Resample the sound if the sampling frequencies do not match.
		*/
		autoSound source;
		if (my samplingPeriod != thy dx) {
			source = Sound_resample (thee, 1.0 / my samplingPeriod, 50);
			thee = source.get();   // reference copy; remove at end
		}

		autoSound him = Data_copy (thee);

		const integer ifirst = std::max (1_integer, Sampled_xToHighIndex (thee, xmin));
		const integer ilast = std::min (Sampled_xToLowIndex (thee, xmax), thy nx);
		for (integer isamp = ifirst; isamp <= ilast; isamp ++) {
			const double sampleTime = Sampled_indexToX (him.get(), isamp);
			const integer frameNumber = Sampled_xToNearestIndex (me, sampleTime);
			if (frameNumber < 1 || frameNumber > my nx) {
				his z [1] [isamp] = 0.0;
				continue;
			}
			const LPC_Frame frame = & my d_frames [frameNumber];
			const integer maximumFilterDepth = frame -> nCoefficients;
			const integer maximumSourceDepth = isamp - 1;
			const integer usableDepth = std::min (maximumFilterDepth, maximumSourceDepth);
			for (integer icoef = 1; icoef <= usableDepth; icoef ++)
				his z [1] [isamp] -= frame -> a [icoef] * his z [1] [isamp - icoef];
		}
		/*
			Make samples before first frame and after last frame zero.
		*/
		for (integer isamp = 1; isamp < ifirst; isamp ++)
			his z [1] [isamp] = 0.0;
		for (integer isamp = ilast + 1; isamp <= his nx; isamp ++)
			his z [1] [isamp] = 0.0;

		if (useGain) {
			for (integer isamp = ifirst; isamp <= ilast; isamp ++) {
				const double sampleTime = Sampled_indexToX (him.get(), isamp);
				const double realFrameNumber = Sampled_xToIndex (me, sampleTime);
				const integer leftFrameNumber = Melder_ifloor (realFrameNumber);
				const integer rightFrameNumber = leftFrameNumber + 1;
				const double phase = realFrameNumber - leftFrameNumber;
				if (rightFrameNumber < 1 || leftFrameNumber > my nx)
					his z [1] [isamp] = 0.0;
				else if (rightFrameNumber == 1)
					his z [1] [isamp] *= sqrt (my d_frames [1]. gain) * phase;
				else if (leftFrameNumber == my nx)
					his z [1] [isamp] *= sqrt (my d_frames [my nx]. gain) * (1.0 - phase);
				else 
					his z [1] [isamp] *=
							phase * sqrt (my d_frames [rightFrameNumber]. gain) +
							(1.0 - phase) * sqrt (my d_frames [leftFrameNumber]. gain);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterWithFilterAtTime_inplace (constLPC me, mutableSound thee, integer channel, double time) {
	integer frameIndex = Sampled_xToNearestIndex (me, time);
	Melder_clip (1_integer, & frameIndex, my nx);   // constant extrapolation
	if (channel > thy ny)
		channel = 1;
	Melder_require (frameIndex > 0 && frameIndex <= my nx,
		U"Frame should be in the range [1, ", my nx, U"].");

	if (channel > 0)
		LPC_Frame_Sound_filter (& my d_frames [frameIndex], thee, channel);
	else
		for (integer ichan = 1; ichan <= thy ny; ichan ++)
			LPC_Frame_Sound_filter (& my d_frames [frameIndex], thee, ichan);
}

autoSound LPC_Sound_filterWithFilterAtTime (constLPC me, constSound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterInverseWithFilterAtTime_inplace (constLPC me, mutableSound thee, integer channel, double time) {
	try {
		integer frameIndex = Sampled_xToNearestIndex (me, time);
		Melder_clip (1_integer, & frameIndex, my nx);   // constant extrapolation
		if (channel > thy ny)
			channel = 1;
		LPC_Frame lpc = & my d_frames [frameIndex];
		autoVEC workspace = raw_VEC (lpc -> nCoefficients);
		if (channel > 0)
			VECfilterInverse_inplace (thy z.row (channel), lpc -> a.get(), workspace.get());
		else
			for (integer ichan = 1; ichan <= thy ny; ichan ++)
				VECfilterInverse_inplace (thy z.row (ichan), lpc -> a.get(), workspace.get());
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

autoSound LPC_Sound_filterInverseWithFilterAtTime (constLPC me, constSound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterInverseWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/* End of file Sound_and_LPC.cpp */
