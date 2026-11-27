/* Sound_to_PowerCepstrogram.cpp
 *
 * Copyright (C) 2012-2025 David Weenink, 2025 Paul Boersma
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

#include "NUM2.h"
#include "Cepstrum_and_Spectrum.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "SoundFrames.h"
#include "Sound_to_PowerCepstrogram.h"

autoPowerCepstrogram Sound_to_PowerCepstrogram (constSound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency) {
	try {
		const kSound_windowShape windowShape = kSound_windowShape::GAUSSIAN_2;
		const double effectiveAnalysisWidth = 3.0 / pitchFloor; // minimum analysis window has 3 periods of lowest pitch
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
		const double physicalSoundDuration = my dx * my nx;
		volatile const double windowDuration = Melder_clippedRight (physicalAnalysisWidth, physicalSoundDuration);
		Melder_require (physicalSoundDuration >= physicalAnalysisWidth,
			U"Your sound is too short:\n"
			U"it should be longer than ", physicalAnalysisWidth, U" s."
		);
		const double samplingFrequency = 2.0 * maximumFrequency;
		autoSound input = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50_integer, preEmphasisFrequency);
		double t1;
		integer nFrames;
		Sampled_shortTermAnalysis (input.get(), windowDuration, dt, & nFrames, & t1);
		const integer soundFrameSize = getSoundFrameSize (physicalAnalysisWidth, input -> dx);
		const integer numberOfFourierSamples = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (soundFrameSize));
		const integer halfNumberOfFourierSamples = numberOfFourierSamples / 2;
		const integer numberOfFrequencies = halfNumberOfFourierSamples + 1;
		const integer numberOfChannels = my ny;
		const double qmax = 0.5 * numberOfFourierSamples / samplingFrequency, dq = 1.0 / samplingFrequency;
		autoPowerCepstrogram output = PowerCepstrogram_create (my xmin, my xmax, nFrames, dt, t1, 0, qmax, numberOfFrequencies, dq, 0);
		bool subtractFrameMean = true;
		const double powerScaling = input -> dx * input -> dx; // =amplitude_scaling^2

		MelderThread_PARALLEL (nFrames, 10) {
			autoSoundFrames soundFrames = SoundFrames_create_fixedSampling (input.get(), output.get(), effectiveAnalysisWidth, windowShape, subtractFrameMean);
			autoVEC fourierSamples = raw_VEC (numberOfFourierSamples);
			autoVEC onesidedPowerSpectrum = raw_VEC (numberOfFrequencies);
			autoNUMFourierTable fourierTable = NUMFourierTable_create (numberOfFourierSamples);		// of dimension numberOfFourierSamples;
			MelderThread_FOR (iframe) {
				/*
					Get average power spectrum of channels
					Let X(f) be the Fourier Transform, defined on the domain [-F,+F], of the real signal x(t).
					The power spectrum is defined as the Fourier transform of the autocorrelation of x(t) which
					equals |X(f)|^2
					The one-sided power spectrum is then P(f)= |X(f)|^2+|X(-f)|^2 = 2|X(f)|^2 for 0 <= f <= F
					The bin width of the first and last frequency in the onesidedPowerSpectrum is half the bin width at the other frequencies
					Do scaling and averaging together
				*/
				Sound sound = soundFrames -> getFrame (iframe);
				onesidedPowerSpectrum.get()  <<=  0.0;
				for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
					fourierSamples.part (1, soundFrameSize)  <<=  sound -> z.row (ichannel);
					fourierSamples.part (soundFrameSize + 1, numberOfFourierSamples)  <<=  0.0;
					NUMfft_forward (fourierTable.get(), fourierSamples.get());
					onesidedPowerSpectrum [1] += fourierSamples [1] * fourierSamples [1];
					for (integer i = 2; i < numberOfFrequencies; i ++) {
						double re = fourierSamples [2 * i - 2], im = fourierSamples [2 * i - 1];
						onesidedPowerSpectrum [i] += re * re + im * im;
					}
					onesidedPowerSpectrum [numberOfFrequencies] += fourierSamples [numberOfFourierSamples] * fourierSamples [numberOfFourierSamples];
				}
				onesidedPowerSpectrum.get()  *=  2.0 * powerScaling / numberOfChannels; // scaling and averaging over channels
				/*
					Get log power.
				*/
				fourierSamples [1] = log (onesidedPowerSpectrum [1] + 1e-300);
				for (integer i = 2; i < numberOfFrequencies; i ++) {
					fourierSamples [2 * i - 2] = log (onesidedPowerSpectrum [i] + 1e-300);
					fourierSamples [2 * i - 1] = 0.0;
				}
				fourierSamples [numberOfFourierSamples] = log (onesidedPowerSpectrum [numberOfFrequencies]  + 1e-300); // thx Underday
				/*
					Inverse transform
				*/
				NUMfft_backward (fourierTable.get(), fourierSamples.get());
				/*
					scale first.
				*/
				const double df = 1.0 / (sound -> dx * numberOfFourierSamples);
				fourierSamples.get()  *=  df;
				for (integer i = 1; i <= numberOfFrequencies; i ++)
					output -> z [i] [iframe] = fourierSamples [i] * fourierSamples [i];
			}
		} MelderThread_ENDPARALLEL
		return output;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrogram created.");
	}
}

//       1           2                          nfftdiv2
//    re   im    re     im                   re      im
// ((fft [1],0) (fft [2],fft [3]), (,), (,), (fft [nfft], 0))  nfft even
// ((fft [1],0) (fft [2],fft [3]), (,), (,), (fft [nfft-1], fft [nfft]))  nfft odd

#define TOLOG(x) ((1 / NUMln10) * log ((x) + 1e-30))

static void complexfftoutput_to_power (constVEC fft, VEC dbs, bool to_db) {
	double valsq = fft [1] * fft [1];
	dbs [1] = ( to_db ? TOLOG (valsq) : valsq );
	const integer nfftdiv2p1 = (fft.size + 2) / 2;
	const integer nend = ( fft.size % 2 == 0 ? nfftdiv2p1 : nfftdiv2p1 + 1 );
	for (integer i = 2; i < nend; i ++) {
		const double re = fft [i + i - 2], im = fft [i + i - 1];
		valsq = re * re + im * im;
		dbs [i] = ( to_db ? TOLOG (valsq) : valsq );
	}
	if (fft.size % 2 == 0) {
		valsq = fft [fft.size] * fft [fft.size];
		dbs [nfftdiv2p1] = ( to_db ? TOLOG (valsq) : valsq );
	}
}

autoPowerCepstrogram Sound_to_PowerCepstrogram_hillenbrand (constSound me, double pitchFloor, double dt) {
	try {
		// minimum analysis window has 3 periods of lowest pitch
		const double physicalDuration = my dx * my nx;
		const double analysisWidth = std::min (3.0 / pitchFloor, physicalDuration);

		double samplingFrequency = 1.0 / my dx;
		autoSound thee;
		if (samplingFrequency > 30000.0) {
			samplingFrequency = samplingFrequency / 2.0;
			thee = Sound_resample (me, samplingFrequency, 1);
		} else {
			thee = Data_copy (me);
		}
		/*
			Pre-emphasis with fixed coefficient 0.9
		*/
		for (integer i = thy nx; i > 1; i --)
			thy z [1] [i] -= 0.9 * thy z [1] [i - 1];

		const integer nosInWindow = Melder_ifloor (analysisWidth * samplingFrequency);
		Melder_require (nosInWindow >= 8,
			U"Analysis window too short.");

		double t1;
		integer numberOfFrames;
		Sampled_shortTermAnalysis (thee.get(), analysisWidth, dt, & numberOfFrames, & t1);
		autoVEC hamming = raw_VEC (nosInWindow);
		for (integer i = 1; i <= nosInWindow; i ++)
			hamming [i] = 0.54 - 0.46 * cos (NUM2pi * (i - 1) / (nosInWindow - 1));

		const integer nfft = Melder_clippedLeft (8_integer /* minimum possible */, Melder_iroundUpToPowerOfTwo (nosInWindow));
		const integer nfftdiv2 = nfft / 2;
		autoVEC fftbuf = zero_VEC (nfft); // "complex" array
		autoVEC spectrum = zero_VEC (nfftdiv2 + 1); // +1 needed 
		autoNUMFourierTable fftTable = NUMFourierTable_create (nfft); // sound to spectrum
		
		const double qmax = 0.5 * nfft / samplingFrequency, dq = qmax / (nfftdiv2 + 1);
		autoPowerCepstrogram him = PowerCepstrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, 0, qmax, nfftdiv2+1, dq, 0);
		
		autoMelderProgress progress (U"Sound to PowerCepstrogram...");

		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double tbegin = std::max (thy xmin, t1 + (iframe - 1) * dt - analysisWidth / 2.0);
			const integer istart = std::max (1_integer, Sampled_xToNearestIndex (thee.get(), tbegin));
			integer iend = istart + nosInWindow - 1;
			Melder_clipRight (& iend, thy nx);
			fftbuf.part (1, nosInWindow)  <<=  thy z.row (1).part (istart, iend) * hamming.all();
			fftbuf.part (nosInWindow + 1, nfft)  <<=  0.0;
			
			NUMfft_forward (fftTable.get(), fftbuf.get());
			complexfftoutput_to_power (fftbuf.get(), spectrum.get(), true); // log10(|fft|^2)
		
			centre_VEC_inout (spectrum.get()); // subtract average
			/*
				Here we diverge from Hillenbrand as he takes the fft of half of the spectral values.
				H. forgets that the actual spectrum has nfft/2+1 values. Therefore, we take the inverse
				transform because this keeps the number of samples a power of 2.
				At the same time this results in twice as many numbers in the quefrency domain, i.e. we end up with nfft/2+1
				numbers while H. has only nfft/4!
			 */
			fftbuf [1] = spectrum [1];
			for (integer i = 2; i < nfftdiv2 + 1; i ++) {
				fftbuf [i+i-2] = spectrum [i];
				fftbuf [i+i-1] = 0.0;
			}
			fftbuf [nfft] = spectrum [nfftdiv2 + 1];
			NUMfft_backward (fftTable.get(), fftbuf.get());
			for (integer i = 1; i <= nfftdiv2 + 1; i ++)
				his z [i] [iframe] = fftbuf [i] * fftbuf [i];

			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames, U"Cepstrogram analysis of frame ",
					 iframe, U" out of ", numberOfFrames, U".");
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Cepstrogram created.");
	}
}
/* End of file Sound_to_PowerCepstrogram.cpp */
