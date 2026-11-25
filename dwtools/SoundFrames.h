#ifndef _SoundFrames_h_
#define _SoundFrames_h_
/* SoundFrames.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

//#include "melder.h"
#include "NUMFourier.h"
#include "Sound_extensions.h"
#include "Spectrum.h"

inline integer getSoundFrameSize_odd (double approximatePhysicalAnalysisWidth, double samplingPeriod) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / samplingPeriod);
	return 2 * halfFrameSamples + 1;
}

inline integer getSoundFrameSize (double physicalAnalysisWidth, double samplingPeriod) {
	Melder_assert (physicalAnalysisWidth > 0.0);
	Melder_assert (samplingPeriod > 0.0);
	return Melder_iround (physicalAnalysisWidth / samplingPeriod);
}

inline double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = (
		windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth
	);
	return physicalAnalysisWidth;
}

Thing_define (SoundFrames, Thing) {

	constSound inputSound;
	mutableSampled output;
	double physicalAnalysisWidth;			// depends on the effectiveAnalysiswidth and the window window shape
	integer soundFrameSize; 				// determined by the physicalAnalysisWidth and the samplingFrequency of the Sound
	autoSound frameAsSound;					// the (possibly multichannel) frame as a Sound
	autoVEC windowFunction;					// the actual window used of size soundFrameSize
	autoVEC soundFrame;						// convenience: average of the channels
	kSound_windowShape windowShape;			// Type: Rectangular, triangular, hamming, etc..
	bool subtractChannelMean = true;		// if true, the frame mean of each channel will be subtracted before windowing

private:
	
	void initCommon (kSound_windowShape windowShape, bool subtractFrameMean);
	
public:
	
	/*
		Calculate the output sampling from the input parameters,
		this is the default case
	*/
	void init (constSound input, double effectiveAnalysisWidth, double timeStep,
		kSound_windowShape windowShape, bool subtractChannelMean);
	
	/*
		In special cases we need the sampling (x1, dx, nx) of the output Sampled:
		like for the FormantPath.
	*/
	void init_fixedSampling (constSound input, mutableSampled output, double effectiveAnalysisWidth,
		kSound_windowShape windowShape, bool subtractChannelMean);
	
	Sound getFrame (integer iframe);
	
	VEC getMonoFrame (integer iframe);
};

autoSoundFrames SoundFrames_create_fixedSampling (constSound input, mutableSampled output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, bool subtractChannelMean);

autoSoundFrames SoundFrames_createForToSampled (constSound input, double effectiveAnalysisWidth, double timeStep,
	kSound_windowShape windowShape, bool subtractChannelMean);

#if 0
Thing_define (SoundFrameIntoSpectrum, SoundFrames) {
	autoSpectrum spectrum;
	integer fftInterpolationFactor = 1;		// 0 = DFT, 1 = FFT, 2, 4, 8 FFT with extra zero's
	integer numberOfFourierSamples;
	autoVEC fourierSamples;					// size = numberOfFourierSamples
	autoVEC fourierSamples_channel;
	autoVEC power_channelAveraged;
	autoNUMFourierTable fourierTable;		// of dimension numberOfFourierSamples;
	
	void init (constSound input, double effectiveAnalysisWidth, double timeStep,
		kSound_windowShape windowShape, bool subtractChannelMean)
	{
		SoundFrameIntoSpectrum_Parent :: init (input, effectiveAnalysisWidth, timeStep, windowShape,  subtractChannelMean);
		init_spectralPart ();
	}
	
	Spectrum getSpectrum () {
		
	}
	
private:
	
	void init_spectralPart () {
		our numberOfFourierSamples = our frameAsSound -> nx;
		if (our fftInterpolationFactor > 0) {
			our numberOfFourierSamples = Melder_iroundUpToPowerOfTwo (numberOfFourierSamples);
			for (integer imultiply = fftInterpolationFactor; imultiply > 1; imultiply --)
				our numberOfFourierSamples *= 2;
		}
		our fourierSamples = raw_VEC (numberOfFourierSamples);
		our fourierSamples_channel = raw_VEC (numberOfFourierSamples);
		our power_channelAveraged = raw_VEC (numberOfFourierSamples);
		const integer numberOfFrequencies = numberOfFourierSamples / 2 + 1;
		our fourierTable = NUMFourierTable_create (our numberOfFourierSamples);
		our spectrum = Spectrum_create (0.5 / our frameAsSound -> dx, numberOfFrequencies);
		our spectrum -> dx = 1.0 / (our frameAsSound -> dx * our numberOfFourierSamples);
	}
	
	void soundFrameToPower_channelAveraged () {
		const integer numberOfChannels = our frameAsSound -> ny;
		power_channelAveraged.get()  <<=  0.0;
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			fourierSamples.part (1, soundFrameSize)  <<=  frameAsSound -> z.row (ichannel);
			fourierSamples.part (soundFrameSize + 1, numberOfFourierSamples)  <<=  0.0;
			NUMfft_forward (fourierTable.get(), fourierSamples.get());
			for (integer i = 1; i <= numberOfFourierSamples; i ++)
				power_channelAveraged [i] += fourierSamples [i] * fourierSamples [i];
		}
		if (numberOfChannels > 1)
			power_channelAveraged.get()  /=  numberOfChannels;
	}

};
#endif /* 0 */

#endif /* _SoundFrames_h_ */
