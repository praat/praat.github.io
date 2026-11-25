/* SoundFrames.cpp
 *
 * Copyright (C) 2025 David Weenink
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

#include "SoundFrames.h"

Thing_implement (SoundFrames, Thing, 0);

void structSoundFrames :: init (constSound input, double effectiveAnalysisWidth, double timeStep,
	kSound_windowShape windowShape, bool subtractChannelMean)
{
	our inputSound = input;
	our physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	if (timeStep == 0.0) {
		// calculate output_dt
	}
	output -> dx = timeStep;
	Sampled_shortTermAnalysis (inputSound, physicalAnalysisWidth, output -> dx, & output -> nx, & output -> x1);
	initCommon (windowShape, subtractChannelMean);
}
	
void structSoundFrames :: init_fixedSampling (constSound input, mutableSampled output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, bool subtractChannelMean)
{
	Melder_assert (input -> xmin == output -> xmin && input -> xmax == output -> xmax);
	our inputSound = input;
	our output = output;
	our physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);	
	initCommon (windowShape, subtractChannelMean);
}

void structSoundFrames :: initCommon (kSound_windowShape windowShape, bool subtractChannelMean)
{
	our windowShape = windowShape;
	our subtractChannelMean = subtractChannelMean;
	soundFrameSize = getSoundFrameSize (physicalAnalysisWidth, inputSound -> dx);
	windowFunction = raw_VEC (soundFrameSize);   // TODO: move out of thread repetition
	soundFrame = raw_VEC (soundFrameSize);
	windowShape_into_VEC (windowShape, windowFunction.get());
	frameAsSound = Sound_create (inputSound -> ny, 0.0, soundFrameSize * inputSound -> dx, soundFrameSize,
		inputSound -> dx, 0.5 * inputSound -> dx);
	Melder_assert (soundFrame.size == soundFrameSize);
}

Sound structSoundFrames :: getFrame (integer iframe) {
	const double midTime = Sampled_indexToX (output, iframe);
	integer startSample = Sampled_xToNearestIndex (inputSound, midTime - 0.5 * physicalAnalysisWidth); // approximation
	const integer numberOfChannels = inputSound -> ny;
	for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
		VEC soundChannel = inputSound -> z.row (ichannel), frameChannel = frameAsSound -> z.row (ichannel);
		integer currentSample = startSample;
		for (integer i = 1; i <= soundFrame.size; i ++, currentSample ++)
			frameChannel [i] = (( currentSample > 0 && currentSample <= inputSound -> nx) ? soundChannel [currentSample] : 0.0 );

		if (subtractChannelMean)
			centre_VEC_inout (frameChannel, nullptr);

		frameChannel  *=  windowFunction.get();
	}

	return frameAsSound.get();
}

VEC structSoundFrames :: getMonoFrame (integer iframe) {
	getFrame (iframe);
	for (integer i = 1; i <= soundFrameSize; i ++)
		soundFrame [i] = Sampled_getValueAtSample (frameAsSound.get(), i, 0, 0); // average channels
	return soundFrame.get();
}

autoSoundFrames SoundFrames_create_fixedSampling (constSound input, mutableSampled output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, bool subtractFrameMean)
{
	try {
		autoSoundFrames me = Thing_new (SoundFrames);
		my init_fixedSampling (input, output, effectiveAnalysisWidth, windowShape, subtractFrameMean);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundFrames (with Sampled) could not be created.");
	}
}

autoSoundFrames SoundFrames_create (constSound input, double effectiveAnalysisWidth,
	double timeStep, kSound_windowShape windowShape,
	bool subtractFrameChannelMean)
{
	try {
		autoSoundFrames me = Thing_new (SoundFrames);
		my init (input, effectiveAnalysisWidth, timeStep, windowShape, subtractFrameChannelMean);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundFrames could not be created.");
	}
}

/* End of file SoundFrames.cpp */
