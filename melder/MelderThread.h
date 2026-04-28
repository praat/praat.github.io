#ifndef _MelderThread_h_
#define _MelderThread_h_
/* MelderThread.h
 *
 * Copyright (C) 2014-2018,2020,2025,2026 Paul Boersma
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


integer MelderThread_getNumberOfProcessors ();

integer MelderThread_computeNumberOfThreads (
	integer numberOfElements,
	integer thresholdNumberOfElementsPerThread
);

/*
	The following is a replacement for std::this_thread::get_id(),
	which is not guaranteeed to return a unique ID.

	A case where this matters is if a throwing thread has finished,
	and then a new thread is created with the same idea and quickly throws as well.
	Admittedly, this is rare, but it is not impossible and it is easy to prevent it.
*/
integer Melder_thisThread_getUniqueID ();

void MelderThread_run (
	std::atomic <bool> *p_errorFlag,
	integer numberOfElements,
	integer thresholdNumberOfElementsPerThread,
	std::function <void (integer threadNumber, integer firstElement, integer lastElement)> const& threadFunction
);

/*
	Here is how these functions help you create simple parallel procedures.
	Our example is pitch analysis in Praat (see fon/Sound_to_Pitch.cpp for details).

	You start with defining the function:

		autoPitch Sound_to_Pitch (Sound me, integer maxnCandidates, ...)
		{

	The function can throw:
			try {

	Then you create the result of the function:

				autoPitch thee = Pitch_create (...);

	Then you create the intermediate objects that are read-only for all threads
	(so that you have to create them only once):

				autoVEC window = ...;   // the window shape
				autoVEC windowR = ...;   // the autocorrelation of the window shape

	You can open a progress bar if you want:

				autoMelderProgress progress (U"Sound to Pitch...");

	Then you define the "thread function", which is a lambda that is called for every thread.
	The thread function is not allowed to throw exceptions, because exceptions cannot pass
	directly from a newly spawned thread into the master thread. Therefore,
	any exception generated with the thread function has to translate each MelderError
	into an error flag:

				std::atomic <bool> errorFlag = false;

	You then define the thread function as a lambda.
	We hand all local variables over to the thread function by reference,
	because many variables (namely those that have to change, such as `errorFlag`,
	and those whose copy constructor has been deleted,
	such as our autoPitch `thee` and our autoVECs `window` and `windowR`,
	cannot be copied into the lambda.

				auto Sound_to_Pitch_threadFunction = [&] (integer threadNumber, integer firstFrame, integer lastFrame) {

	We could also have said [=, & errorFlag, & thee, & window, & windowR],
	whereby most variables would have been handed over to the lambda by value,
	but simply handing over all variables by reference is more straightforward and not really slower.

	The thread function can throw, so you need a `try`-`catch` pair:
					try {

	In case the thread function can call NUMrandom functions,
	they should be thread-aware (for Sound_to_Pitch, this is not the case though):

						NUMrandom_setChannel (threadNumber);

	Inside the thread function you first create the objects that are different for each thread, such as buffers:

						autoMAT frame = zero_MAT (my ny, ...);
						autoNUMFourierTable fftTable = NUMFourierTable_create (...);
						autoVEC ac = zero_VEC (...);
						autoVEC rbuffer = zero_VEC (...);
						double *r = & rbuffer [...];
						autoINTVEC imax = zero_INTVEC (maxnCandidates);
						autoVEC localMean = zero_VEC (my ny);

	You can now cycle over the frames that belong to the thread:

						for (integer iframe = firstFrame; iframe <= lastFrame; iframe ++) {

	Each frame provides an opportunity to bail out from the thread function (and therefore to abort the thread):

							if (errorFlag)
								return;   // abort this thread

	If not aborted, you can now do the pitch analysis for the present frame:
	
							Pitch_Frame pitchFrame = & thy frames [iframe];
							Sound_into_PitchFrame (me, pitchFrame, maxnCandidates,
								window.get(), windowR.get(),
								frame.get(), fftTable.get(), ac.get(),
								r, imax.get(), localMean.get()
							);

	You can update the progress bar if you are in the master thread, which has threadNumber 0.
	You can do that anywhere in the frame loop; here we do it at the end of it:

							if (threadNumber == 0) {
								const double estimatedFractionAnalysed = (iframe - firstFrame + 0.5) / (lastFrame - firstFrame + 1.0);
								Melder_progress (0.1 + 0.8 * estimatedFractionAnalysed,
									U"Sound to Pitch: analysed approximately ", Melder_iround (numberOfFrames * estimatedFractionAnalysed),
									U" out of ", numberOfFrames, U" frames"
								);
							}
						}

	The `catch` differs from the normal "catch (MelderError) { Melder_throw (...); }",
	in that it has to convert the MelderError into an error flag:

					} catch (MelderError) {
						errorFlag = true;
						return;
					}

	Finally, you close the lambda and call it:

				};
				MelderThread_run (& errorFlag, numberOfFrames, 5, false, Sound_to_Pitch_threadFunction);

	You also close the progress bar and return the result:

				Melder_progress (0.95, U"Sound to Pitch: path finder");
				return thee;

	You end with the `catch` of Sound_to_Pitch itself:

			} catch (MelderError) {
				Melder_throw (me, U": pitch analysis not performed.");
			}
		}
*/

/*
	The above procedure can be simplified a bit by using the following four macros.
	We surround the code by an extra pair of braces in order to allow multiple use within a function.
	There will also be an extra pair of braces in your written code, so as to suggest scope correctly.
*/

#define MelderThread_PARALLEL(numberOfElements, thresholdNumberOfElementsPerThread)  \
	{/* start of scope of `_errorFlag_` and `_threadFunction_` */  \
		const integer _numberOfElements_ = numberOfElements;  \
		const integer _thresholdNumberOfElementsPerThread_ = thresholdNumberOfElementsPerThread;  \
		std::atomic <bool> _errorFlag_ = false;  \
		auto _threadFunction_ = [&] (integer _threadNumber_, integer _firstElement_, integer _lastElement_) {  \
			try {  \
				NUMrandom_setChannel (_threadNumber_);  \
				Melder_thisThread_setRange (_firstElement_, _lastElement_);

#define MelderThread_FOR(ielement)  \
				for (integer ielement = _firstElement_; ielement <= _lastElement_; ielement ++) {  \
					if (_errorFlag_)  \
						return;  \
					Melder_thisThread_setCurrentElement (ielement);

#define MelderThread_ENDPARALLEL  \
				}  \
			} catch (MelderError) {  \
				_errorFlag_ = true;  \
				return;  \
			}  \
		};  \
		MelderThread_run (& _errorFlag_, _numberOfElements_, _thresholdNumberOfElementsPerThread_, _threadFunction_);  \
	}/* end of scope of `_errorFlag_` and `_threadFunction_` */

#define MelderThread_IS_MASTER  \
	(NUMrandom_getChannel () == 0)

#define MelderThread_ESTIMATED_PROGRESS  \
	Melder_thisThread_estimateProgress ()

/*
	The whole Sound_to_Pitch then becomes:

		autoPitch Sound_to_Pitch (Sound me, integer maxnCandidates, ...)
		{
			try {
				autoPitch thee = Pitch_create (...);
				autoVEC window = ...;   // the window shape
				autoVEC windowR = ...;   // the autocorrelation of the window shape

				autoMelderProgress progress (U"Sound to Pitch...");

				MelderThread_PARALLEL (numberOfFrames, 5, false) {
					autoMAT frame = zero_MAT (my ny, ...);
					autoNUMFourierTable fftTable = NUMFourierTable_create (...);
					autoVEC ac = zero_VEC (...);
					autoVEC rbuffer = zero_VEC (...);
					double *r = & rbuffer [...];
					autoINTVEC imax = zero_INTVEC (maxnCandidates);
					autoVEC localMean = zero_VEC (my ny);

					MelderThread_FOR (iframe) {
						Pitch_Frame pitchFrame = & thy frames [iframe];
						Sound_into_PitchFrame (me, pitchFrame, maxnCandidates,
							window.get(), windowR.get(),
							frame.get(), fftTable.get(), ac.get(),
							r, imax.get(), localMean.get()
						);

						if (MelderThread_IS_MASTER) {   // then we can interact with the GUI
							const double estimatedProgress = MelderThread_ESTIMATED_PROGRESS;
							Melder_progress (0.1 + 0.8 * estimatedProgress,
								U"Sound to Pitch: analysed approximately ", Melder_iround (numberOfFrames * estimatedProgress),
								U" out of ", numberOfFrames, U" frames"
							);
						}
					}
				} MelderThread_ENDPARALLEL

				Melder_progress (0.95, U"Sound to Pitch: path finder");
				return thee;
			} catch (MelderError) {
				Melder_throw (me, U": pitch analysis not performed.");
			}
		}
*/

/*
	How to parallelize an existing analysis function
	================================================

	Suppose you have something like

	autoPitch Sound_to_Pitch (Sound me) {
		try {
			autoPitch thee = ...;
			autoVEC window = ...;   // the window shape
			autoMAT frame = zero_MAT (my ny, ...);
			autoNUMFourierTable fftTable = NUMFourierTable_create (...);
			autoVEC ac = zero_VEC (...);
			autoVEC rbuffer = zero_VEC (...);
			double *r = & rbuffer [...];
			autoVEC windowR = ...;   // the autocorrelation of the window shape
			autoINTVEC imax = zero_INTVEC (maxnCandidates);
			autoVEC localMean = zero_VEC (my ny);
			autoMelderProgress progress (U"Sound to Pitch...");
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				Pitch_Frame pitchFrame = & thy frames [iframe];
				const double fractionAnalysed = (double) iframe / thy nx;
				Melder_progress (0.1 + 0.8 * fractionAnalysed,
					U"Sound to Pitch: analysed ", iframe,
					U" out of ", thy nx, U" frames"
				);
				Sound_into_PitchFrame (me, pitchFrame, maxnCandidates,
					window.get(), windowR.get(),
					frame.get(), fftTable.get(), ac.get(),
					r, imax.get(), localMean.get()
				);
			}
			Melder_progress (0.95, U"Sound to Pitch: path finder");
			return thee;
		} catch (MelderError) {
			Melder_throw (me, U": pitch analysis not performed.");
		}
	}

	The first step is to divide all those initializations into two sets:
	1. the ones that all threads can use at the same time;
	   these are:
	   1a. the target Pitch object, because each thread will write a separate part of it:
			autoPitch thee = ...;
	   1b. the "constant" things that are computed only once, before the loop over the frames:
			autoVEC window = ...;   // the window shape
			autoVEC windowR = ...;   // the autocorrelation of the window shape
	   1c. the initialization of the progress bar:
			autoMelderProgress progress (U"Sound to Pitch...");
	2. the ones of which each thread need its own copy, like buffers:
			autoMAT frame = zero_MAT (my ny, ...);
			autoNUMFourierTable fftTable = NUMFourierTable_create (...);
			autoVEC ac = zero_VEC (...);
			autoVEC rbuffer = zero_VEC (...);
			double *r = & rbuffer [...];
			autoINTVEC imax = zero_INTVEC (maxnCandidates);
			autoVEC localMean = zero_VEC (my ny);

	This reordering is the preparation. You then make the following simple changes:
	3. between 1abc and 2 above you insert:
			MelderThread_PARALLEL (thy nx, 5, false) {
	4. instead of
				for (integer iframe = 1; iframe <= thy nx; iframe ++) {
	   you write
				MelderThread_FOR (iframe) {
	5. instead of the frame-loop-closing "}" you write
				}
			} MelderThread_ENDPARALLEL
	6. you update the progress bar only in the master thread, by inserting
			if (MelderThread_IS_MASTER) {
	   and
			}
	7. instead of computing the progress as
				const double fractionAnalysed = (double) iframe / thy nx;
	   you write
				const double fractionAnalysed = MelderThread_ESTIMATED_PROGRESS;
	   and you may reword the variable name and the text to acknowledge that this progress
	   is just an estimate;
	8. you tune the "thresholdNumberOfElementsPerThread" to what turns out to be fastest
	   for low numbers of threads; see `test/manually/Sound_to_Pitch.praat` for an example.
*/

void MelderThread_debugMultithreading (bool useMultithreading, integer numberOfConcurrentThreadsToUse,
	integer minimumNumberOfElementsPerThread, bool extraAnalysisInfo);

bool MelderThread_getUseMultithreading ();

integer MelderThread_getMaximumNumberOfConcurrentThreads ();

integer MelderThread_getMinimumNumberOfElementsPerThread ();

bool MelderThread_getTraceThreads ();

/*
	The following two macros can be used in tracing.
	
	Example:
		if (MelderThread_TRACING)
			Melder_casual (MelderThread_CHANNEL, U" ", iframe, U" ", result);
*/

#define MelderThread_TRACING  \
	MelderThread_getTraceThreads ()

#define MelderThread_CHANNEL  \
	NUMrandom_getChannel ()

/*
	The following three functions are mainly on behalf of progress bars.
*/

void Melder_thisThread_setRange (integer firstElement, integer lastElement);

void Melder_thisThread_setCurrentElement (integer currentElement);

double Melder_thisThread_estimateProgress ();

/* End of file MelderThread.h */
#endif
