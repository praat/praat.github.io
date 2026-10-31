/* NUMinterpol.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018,2020 Paul Boersma
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

/*
 * pb 2002/03/07 GPL
 * pb 2003/06/19 ridders3 replaced with ridders
 * pb 2003/07/09 gsl
 * pb 2007/01/27 use #defines for value interpolation
 * pb 2007/08/20 built a "weird value" check into NUMviterbi (bug report by Adam Jacks)
 * pb 2011/03/29 C++
 */

#include "melder.h"
#include "../dwsys/NUM2.h"

double NUM_interpolate_sinc (constVEC const& y, double x, integer maxDepth) {
	Melder_assert (isdefined (x));
	const integer midleft = Melder_ifloor (x), midright = midleft + 1;
	double result = 0.0;
	if (y.size < 1)
		return undefined;   // there exists no best guess
	if (x < 1)
		return y [1];   // offleft: constant extrapolation
	if (x > y.size)
		return y [y.size];   // offright: constant extrapolation
	if (x == midleft)
		return y [midleft];   // the interpolated curve goes through the points
	/*
		1 < x < y.size && x not integer: interpolate.
	*/
	Melder_clipRight (& maxDepth, midright - 1);
	Melder_clipRight (& maxDepth, y.size - midleft);
	if (maxDepth <= NUM_VALUE_INTERPOLATE_NEAREST)
		return y [Melder_iround (x)];   // possible because we know that 1 <= x <= y.size
	if (maxDepth == NUM_VALUE_INTERPOLATE_LINEAR)
		return y [midleft] + (x - midleft) * (y [midright] - y [midleft]);
	if (maxDepth == NUM_VALUE_INTERPOLATE_CUBIC) {
		const double yl = y [midleft], yr = y [midright];
		const double dyl = 0.5 * (yr - y [midleft - 1]), dyr = 0.5 * (y [midright + 1] - yl);
		const double fil = x - midleft, fir = midright - x;
		return yl * fir + yr * fil - fil * fir * (0.5 * (dyr - dyl) + (fil - 0.5) * (dyl + dyr - 2 * (yr - yl)));
	}
	/*
		maxDepth >= 3: sinc interpolation
	*/
	const integer left = midright - maxDepth;   // as maxDepth <= midright - 1, we know that left >= 1
	const integer right = midleft + maxDepth;   // as maxDepth <= y.size - midleft, we know that right <= y.size
	/*
		The sinc function contains a sine wave with a phase shift of pi per sample. For sample `i`:
			sinc [i] = sin (pi * (i - x)) / (pi * (i - x)) =
			         = sin (pi * (x - i)) / (pi * (x - i))
		The sought interpolated value will then be
			result = sum_i=left^right sinc [i] * y [i] * window [i] / sum_i=left^right window [i]
		where `window` is a raised cosine:
			window [i] = 0.5 + 0.5 * cos (pi * (x - i) / (x - left + 1))    (for `left` <= `i` <= `midleft`)
			window [i] = 0.5 + 0.5 * cos (pi * (i - x) / (right - x + 1))   (for `midright` <= `i` <= `right`)

		We can check whether a more symmetric window is better, e.g.
			window [i] = 0.5 + 0.5 * cos (pi * (i - x) / (maxDepth + 0.5))    (for `left` <= `i` <= `right`)
		That turns out to be about 0.1 dB worse, but `maxDepth` + 2.0 does much better on the script `fon/resamplingDepth.praat`;
		the aliasing-suppressions are (in dB):

		Depth:  700      50      20      10      6       3       2       1
		       52.366  41.348  39.357  38.267  36.956  27.897  20.839  19.383
		       52.365  41.328  39.309  38.180  36.734  27.901  20.839  19.383   maxDepth + 0.5
		       52.261  41.298  39.269  38.068  36.485  24.870  20.839  19.383   maxDepth + 0.0
		       52.371  41.358  39.349  38.284  36.967  30.742  20.839  19.383   maxDepth + 1.0
		       52.377  41.387  39.388  38.375  37.177  32.949  20.839  19.383   maxDepth + 1.5
		       52.382  41.417  39.426  38.445  37.337  34.032  20.839  19.383   maxDepth + 2.0   but strong reflection of f=0 at fsamp
		       52.387  41.446  39.462  38.490  37.429 *34.042  20.839  19.383   maxDepth + 2.5
		       52.392  41.476  39.496 *38.510 *37.450  33.504  20.839  19.383   maxDepth + 3.0
		       52.397  41.505  39.528  38.503  37.406  32.830  20.839  19.383   maxDepth + 3.5
		       52.401  41.533  39.558  38.472  37.309  32.193  20.839  19.383   maxDepth + 4.0
		       52.443  41.856 *39.728  37.254  35.390  29.054  20.839  19.383   maxDepth + 10.0
		       52.503  42.287  39.523  35.747  34.030  28.212  20.839  19.383   maxDepth + 20.0
		       52.676  42.933  38.803  34.627  33.309  27.900  20.839  19.383   maxDepth + 50.0
		       52.951  43.185  38.434  34.349  33.163  27.847  20.839  19.383   maxDepth + 100.0
		       53.432  43.242  38.283  34.263  33.121  27.833  20.839  19.383   maxDepth + 200.0
		       54.116 *43.246  38.235  34.239  33.110  27.829  20.839  19.383   maxDepth + 400.0
		       54.766  43.245  38.222  34.232  33.107  27.828  20.839  19.383   maxDepth + 800.0
		       55.109  43.245  38.218  34.231  33.106  27.828  20.839  19.383   maxDepth + 1600.0
		       55.213  43.245  38.217  34.230  33.106  27.828  20.839  19.383   maxDepth + 3200.0
		      *55.246  43.245  38.217  34.230  33.105  27.828  20.839  19.383   maxDepth + 1e9
		       53.975  42.453 *39.728  38.347 *37.451  32.949  20.839  19.383   maxDepth * 1.5

		As we will be approximating the sine and cosine values by matrix multiplication
		(instead of recomputing them every time we need them)
		we will start where the values of the sinc function are highest, i.e. at `midleft` and `midright`.

		First interpolate over the samples on the left, i.e. for `left` <= `i` <= `midleft`.

		With
			leftPhase [i] = pi * (x - i)
			leftDepth = x - left + 1   ; or maxDepth + 0.5
		we get
			sinc [i] * window [i] = 0.5 * sin (leftPhase [i]) / leftPhase [i] * (1.0 + cos (leftPhase [i] / leftDepth))
		Iterate for `i` from `midleft` to `left`:
			result := 0
			leftPhase := pi * (x - midleft)
			windowPhase := leftPhase / leftDepth
			for i from midleft downto left
				sincTimesWindow := 0.5 * sin (leftPhase) / leftPhase * (1.0 + cos (windowPhase))
				result += y [i] * sincTimesWindow
				leftPhase += pi
				windowPhase += pi / leftDepth
			endfor
		Adding a phase of pi just reverses the sign of the sine. So we can simplify the loop:
			result := 0
			leftPhase := pi * (x - midleft)
			windowPhase := leftPhase / leftDepth
			windowPhaseStep = pi / leftDepth
			halfSinLeftPhase := 0.5 * sin (leftPhase)
			for i from midleft downto left
				sincTimesWindow := halfSinLeftPhase / leftPhase * (1.0 + cos (windowPhase))
				result += y [i] * sincTimesWindow
				leftPhase += pi
				halfSinLeftPhase := - halfSinLeftPhase
				windowPhase += windowPhaseStep
			endfor
		The computationally slow part of this is the computation of the cosine within the loop.
		This can be sped up by computing both the sine and cosine of the current `windowPhase` and then
		computing the sine and cosine of the next `windowPhase`, i.e. the current `windowPhase` plus `windowPhaseStep`:
			sin (windowPhase + windowPhaseStep) = sin (windowPhase) * cos (windowPhaseStep) + cos (windowPhase) * sin (windowPhaseStep)
			cos (windowPhase + windowPhaseStep) = cos (windowPhase) * cos (windowPhaseStep) - sin (windowPhase) * sin (windowPhaseStep)
	*/
	enum class enumWindowShape { RAISED_COSINE, RAISED_GAUSSIAN };
	constexpr enumWindowShape WINDOW_SHAPE = enumWindowShape :: RAISED_COSINE;
	if (WINDOW_SHAPE == enumWindowShape :: RAISED_COSINE) {
		{// scope: left half
			//const double leftDepth = x - left + 1.0;
			const double leftDepth = maxDepth + 0.5;
			const double windowPhaseStep = NUMpi / leftDepth;
			const double sinWindowPhaseStep = sin (windowPhaseStep);
			const double cosWindowPhaseStep = cos (windowPhaseStep);
			/*
				Initialize the phase of the sinc.
			*/
			double leftPhase = NUMpi * (x - midleft);
			double halfSinLeftPhase = 0.5 * sin (leftPhase);
			/*
				Initialize the phase of the window.
			*/
			double windowPhase = leftPhase / leftDepth;
			double sinWindowPhase = sin (windowPhase);
			double cosWindowPhase = cos (windowPhase);
			/*
				Step through the samples.
			*/
			for (integer ix = midleft; ix >= left; ix --) {
				/*
					Accumulate the contribution of this sample to the result.
				*/
				Melder_assert (ix >= 1);
				Melder_assert (ix <= y.size);
				Melder_assert (leftPhase != 0.0);
				const double sincTimesWindow = halfSinLeftPhase / leftPhase * (1.0 + cosWindowPhase);
				result += y [ix] * sincTimesWindow;
				Melder_assert (isdefined (result));
				/*
					Update the phase of the sinc.
				*/
				leftPhase += NUMpi;
				halfSinLeftPhase = - halfSinLeftPhase;
				/*
					Update the phase of the window.
				*/
				//windowPhase += windowPhaseStep;   // superfluous statement (though semantically correct)
				const double nextSinWindowPhase = cosWindowPhase * sinWindowPhaseStep + sinWindowPhase * cosWindowPhaseStep;
				const double nextCosWindowPhase = cosWindowPhase * cosWindowPhaseStep - sinWindowPhase * sinWindowPhaseStep;
				sinWindowPhase = nextSinWindowPhase;
				cosWindowPhase = nextCosWindowPhase;
			}
		}
		/*
			Then interpolate over the samples on the right, i.e. for `midright` <= `i` <= `right`.
		*/
		{// scope: right half
			//const double rightDepth = right - x + 1.0;
			const double rightDepth = maxDepth + 0.5;
			const double windowPhaseStep = NUMpi / rightDepth;
			const double sinWindowPhaseStep = sin (windowPhaseStep);
			const double cosWindowPhaseStep = cos (windowPhaseStep);
			/*
				Initialize the phase of the sinc.
			*/
			double rightPhase = NUMpi * (midright - x);
			double halfSinRightPhase = 0.5 * sin (rightPhase);
			/*
				Initialize the phase of the window.
			*/
			double windowPhase = rightPhase / rightDepth;
			double sinWindowPhase = sin (windowPhase);
			double cosWindowPhase = cos (windowPhase);
			/*
				Step through the samples.
			*/
			for (integer ix = midright; ix <= right; ix ++) {
				/*
					Accumulate the contribution of this sample to the result.
				*/
				Melder_assert (ix >= 1);
				Melder_assert (ix <= y.size);
				Melder_assert (rightPhase != 0.0);
				const double sincTimesWindow = halfSinRightPhase / rightPhase * (1.0 + cosWindowPhase);
				result += y [ix] * sincTimesWindow;
				if (isundef (result)) {
					//Melder_crash (y.size, U" ", ix, U" ", y [ix], U" ", sincTimesWindow);
				}
				//Melder_assert (isdefined (result));
				/*
					Update the phase of the sinc.
				*/
				rightPhase += NUMpi;
				halfSinRightPhase = - halfSinRightPhase;
				/*
					Update the phase of the window.
				*/
				//windowPhase += windowPhaseStep;   // superfluous statement (though semantically correct)
				const double nextSinWindowPhase = cosWindowPhase * sinWindowPhaseStep + sinWindowPhase * cosWindowPhaseStep;
				const double nextCosWindowPhase = cosWindowPhase * cosWindowPhaseStep - sinWindowPhase * sinWindowPhaseStep;
				sinWindowPhase = nextSinWindowPhase;
				cosWindowPhase = nextCosWindowPhase;
			}
		}
	} else if (WINDOW_SHAPE == enumWindowShape :: RAISED_GAUSSIAN) {
		/*
			Check whether an exponential window is better, e.g.
				window [i] = (exp (-12.0 * ((i - x) / (maxDepth + 0.5))^2) - exp (-12.0)) / (1.0 - exp (-12.0))   for `left` <= `i` <= `right`
			With
				leftPhase [i] = pi * (x - i)
				leftDepth = maxDepth + 0.5
			we get
				sinc [i] * window [i] = sin (leftPhase [i]) / leftPhase [i] *
						* exp (-12.0/pi^2 * (leftPhase [i] / leftDepth)^2) - exp (-12.0)) / (1.0 + exp (-12.0))
			Iterate for `i` from `midleft` to `left`:
				result := 0
				leftPhase := pi * (x - midleft)
				windowPhase := -12/pi * leftPhase / leftDepth
				for i from midleft downto left
					sincTimesWindow := sin (leftPhase) / leftPhase * (exp (windowPhase) - exp (-12.0)) / (1.0 + exp (-12.0))
					result += y [i] * sincTimesWindow
					leftPhase += pi
					windowPhase += pi / leftDepth
				endfor
			Adding a phase of pi just reverses the sign of the sine. So we can simplify the loop:
				result := 0
				leftPhase := pi * (x - midleft)
				windowPhase := -12/pi^2 * (leftPhase / leftDepth)^2
				sinLeftPhase := sin (leftPhase)
				for i from midleft downto left
					sincTimesWindow := sinLeftPhase / leftPhase * (exp (windowPhase) - exp (-12.0)) / (1.0 + exp (-12.0))
					result += y [i] * sincTimesWindow
					leftPhase += pi
					sinLeftPhase := - sinLeftPhase
					windowPhase = -12/pi^2 * (leftPhase / leftDepth)^2
				endfor
			The computationally slow part of this is the computation of the exponential within the loop.
			This can be sped up by computing the exponential of the current `windowPhase` and then
			computing the exponential of the next `windowPhase`, i.e. the current `windowPhase` plus `windowPhaseStep`:
				exp (windowPhase + windowPhaseStep) = exp (windowPhase) * exp (windowPhaseStep)
		*/
		{// scope: left half
			const double leftDepth = maxDepth + 0.5;
			/*
				Initialize the phase of the sinc.
			*/
			double leftPhase = NUMpi * (x - midleft);
			double sinLeftPhase = sin (leftPhase);
			/*
				Initialize the phase of the window.
			*/
			double windowPhase = (-12.0 / NUMpi / NUMpi) * sqr (leftPhase / leftDepth);
			/*
				Step through the samples.
			*/
			for (integer ix = midleft; ix >= left; ix --) {
				/*
					Accumulate the contribution of this sample to the result.
				*/
				const double sincTimesWindow = sinLeftPhase / leftPhase * (exp (windowPhase) - exp (-12.0)) / (1.0 + exp (-12.0));
				result += y [ix] * sincTimesWindow;
				/*
					Update the phase of the sinc.
				*/
				leftPhase += NUMpi;
				sinLeftPhase = - sinLeftPhase;
				/*
					Update the phase of the window.
				*/
				windowPhase += (-12.0 / NUMpi / NUMpi) * sqr (leftPhase / leftDepth);
			}
		}
		/*
			Then interpolate over the samples on the right, i.e. for `midright` <= `i` <= `right`.
		*/
		{// scope: right half
			const double rightDepth = maxDepth + 0.5;
			/*
				Initialize the phase of the sinc.
			*/
			double rightPhase = NUMpi * (midright - x);
			double sinRightPhase = sin (rightPhase);
			/*
				Initialize the phase of the window.
			*/
			double windowPhase = (-12.0 / NUMpi / NUMpi) * sqr (rightPhase / rightDepth);
			/*
				Step through the samples.
			*/
			for (integer ix = midright; ix <= right; ix ++) {
				/*
					Accumulate the contribution of this sample to the result.
				*/
				const double sincTimesWindow = sinRightPhase / rightPhase * (exp (windowPhase) - exp (-12.0)) / (1.0 + exp (-12.0));
				result += y [ix] * sincTimesWindow;
				/*
					Update the phase of the sinc.
				*/
				rightPhase += NUMpi;
				sinRightPhase = - sinRightPhase;
				/*
					Update the phase of the window.
				*/
				windowPhase += (-12.0 / NUMpi / NUMpi) * sqr (rightPhase / rightDepth);
			}
		}
	}
	//Melder_assert (isdefined (result));
	return result;
}

/********** Improving extrema **********/
#pragma mark Improving extrema

struct improve_params {
	integer depth;
	constVEC y;
	bool isMaximum;
};

static double improve_evaluate (double x, void *closure) {
	Melder_assert (isdefined (x));
	struct improve_params *me = (struct improve_params *) closure;
	const double y = NUM_interpolate_sinc (my y, x, my depth);
	return my isMaximum ? - y : y;
}

double NUMimproveExtremum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real, bool isMaximum) {
	struct improve_params params;
	double result;
	if (ixmid <= 1) {
		*ixmid_real = double (1);
		return y [1];
	}
	if (ixmid >= y.size) {
		*ixmid_real = double (y.size);
		return y [y.size];
	}
	if (interpolationDepth <= NUM_PEAK_INTERPOLATE_NONE) {
		*ixmid_real = double (ixmid);
		return y [ixmid];
	}
	if (interpolationDepth == NUM_PEAK_INTERPOLATE_PARABOLIC) {
		const double dy = 0.5 * (y [ixmid + 1] - y [ixmid - 1]);
		const double d2y = (y [ixmid] - y [ixmid - 1]) + (y [ixmid] - y [ixmid + 1]);   // beware of rounding errors
		Melder_assert (isMaximum && d2y > 0.0 || ! isMaximum && d2y < 0.0);
		*ixmid_real = ixmid + dy / d2y;
		return y [ixmid] + 0.5 * dy * dy / d2y;
	}
	/*
		Cubic or sinc interpolation.
	*/
	params. depth = (
		interpolationDepth == NUM_PEAK_INTERPOLATE_CUBIC ? NUM_VALUE_INTERPOLATE_CUBIC :
		interpolationDepth == NUM_PEAK_INTERPOLATE_SINC70 ? NUM_VALUE_INTERPOLATE_SINC70 :
		NUM_VALUE_INTERPOLATE_SINC700
	);
	params. y = y;
	params. isMaximum = isMaximum;
	*ixmid_real = NUMminimize_brent (improve_evaluate, ixmid - 1, ixmid + 1, & params, 1e-10, & result);
	Melder_assert (isdefined (*ixmid_real));
	//Melder_assert (isdefined (result));
	return isMaximum ? - result : result;
}

double NUMimproveMinimum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real) {
	return NUMimproveExtremum (y, ixmid, interpolationDepth, ixmid_real, false);
}
double NUMimproveMaximum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real) {
	return NUMimproveExtremum (y, ixmid, interpolationDepth, ixmid_real, true);
}

/********** Viterbi **********/

void NUM_viterbi (
	integer numberOfFrames, integer maxnCandidates,
	integer (*getNumberOfCandidates) (integer iframe, void *closure),
	double (*getLocalCost) (integer iframe, integer icand, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, void *closure),
	void (*putResult) (integer iframe, integer place, void *closure),
	void *closure)
{
	autoMAT delta = raw_MAT (numberOfFrames, maxnCandidates);
	autoINTMAT psi = raw_INTMAT (numberOfFrames, maxnCandidates);
	autoINTVEC numberOfCandidates = raw_INTVEC (numberOfFrames);
	for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
		numberOfCandidates [iframe] = getNumberOfCandidates (iframe, closure);
		for (integer icand = 1; icand <= numberOfCandidates [iframe]; icand ++)
			delta [iframe] [icand] = - getLocalCost (iframe, icand, closure);
	}
	for (integer iframe = 2; iframe <= numberOfFrames; iframe ++) {
		for (integer icand2 = 1; icand2 <= numberOfCandidates [iframe]; icand2 ++) {
			double maximum = -1e308;
			integer place = 0;
			for (integer icand1 = 1; icand1 <= numberOfCandidates [iframe - 1]; icand1 ++) {
				const double value = delta [iframe - 1] [icand1] + delta [iframe] [icand2]
						- getTransitionCost (iframe, icand1, icand2, closure);
				if (value > maximum) {
					maximum = value;
					place = icand1;
				}
			}
			if (place == 0)
				Melder_throw (U"Viterbi algorithm cannot compute a track because of weird values.");
			delta [iframe] [icand2] = maximum;
			psi [iframe] [icand2] = place;
		}
	}
	/*
		Find the end of the most probable path.
	*/
	integer place;
	double maximum = delta [numberOfFrames] [place = 1];
	for (integer icand = 2; icand <= numberOfCandidates [numberOfFrames]; icand ++) {
		if (delta [numberOfFrames] [icand] > maximum)
			maximum = delta [numberOfFrames] [place = icand];
	}
	/*
		Backtrack.
	*/
	for (integer iframe = numberOfFrames; iframe >= 1; iframe --) {
		putResult (iframe, place, closure);
		place = psi [iframe] [place];
	}
}

/******************/

struct parm2 {
	integer ntrack;
	integer ncomb;
	INTMAT indices;
	double (*getLocalCost) (integer iframe, integer icand, integer itrack, void *closure);
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, integer itrack, void *closure);
	void (*putResult) (integer iframe, integer place, integer itrack, void *closure);
	void *closure;
};

static integer getNumberOfCandidates_n (integer iframe, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	(void) iframe;
	return my ncomb;
}
static double getLocalCost_n (integer iframe, integer jcand, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	double localCost = 0.0;
	for (integer itrack = 1; itrack <= my ntrack; itrack ++)
		localCost += my getLocalCost (iframe, my indices [jcand] [itrack], itrack, my closure);
	return localCost;
}
static double getTransitionCost_n (integer iframe, integer jcand1, integer jcand2, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	double transitionCost = 0.0;
	for (integer itrack = 1; itrack <= my ntrack; itrack ++)
		transitionCost += my getTransitionCost (iframe,
			my indices [jcand1] [itrack], my indices [jcand2] [itrack], itrack, my closure);
	return transitionCost;
}
static void putResult_n (integer iframe, integer jplace, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	for (integer itrack = 1; itrack <= my ntrack; itrack ++)
		my putResult (iframe, my indices [jplace] [itrack], itrack, my closure);
}

void NUM_viterbi_multi (
	integer nframe, integer ncand, integer ntrack,
	double (*getLocalCost) (integer iframe, integer icand, integer itrack, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, integer itrack, void *closure),
	void (*putResult) (integer iframe, integer place, integer itrack, void *closure),
	void *closure)
{
	struct parm2 parm;

	if (ntrack > ncand) Melder_throw (U"(NUM_viterbi_multi:) "
		U"Number of tracks (", ntrack, U") should not exceed number of candidates (", ncand, U").");
	const integer ncomb = Melder_iround (NUMcombinations (ncand, ntrack));
	if (ncomb > 10'000'000) Melder_throw (U"(NUM_viterbi_multi:) "
		U"Unrealistically high number of combinations (", ncomb, U").");
	parm. ntrack = ntrack;
	parm. ncomb = ncomb;

	/*
		For ncand == 5 and ntrack == 3, parm.indices is going to contain:
			1 2 3
			1 2 4
			1 2 5
			1 3 4
			1 3 5
			1 4 5
			2 3 4
			2 3 5
			2 4 5
			3 4 5
	*/
	autoINTMAT indices = zero_INTMAT (ncomb, ntrack);
	autoINTVEC icand = to_INTVEC (ntrack);   // start out with "1 2 3"
	integer jcomb = 0;
	for (;;) {
		jcomb ++;
		for (integer itrack = 1; itrack <= ntrack; itrack ++)
			indices [jcomb] [itrack] = icand [itrack];
		integer itrack = ntrack;
		for (; itrack >= 1; itrack --) {
			if (++ icand [itrack] <= ncand - (ntrack - itrack)) {
				for (integer jtrack = itrack + 1; jtrack <= ntrack; jtrack ++)
					icand [jtrack] = icand [itrack] + jtrack - itrack;
				break;
			}
		}
		if (itrack == 0) break;
	}
	Melder_assert (jcomb == ncomb);
	parm. indices = indices.get();
	parm. getLocalCost = getLocalCost;
	parm. getTransitionCost = getTransitionCost;
	parm. putResult = putResult;
	parm. closure = closure;
	NUM_viterbi (nframe, ncomb, getNumberOfCandidates_n, getLocalCost_n, getTransitionCost_n, putResult_n, & parm);
}

/* End of file NUMinterpol.cpp */
