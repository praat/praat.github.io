/* manual_LPC.cpp
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

/*
 djmw 20020422 GPL + removed "C syntax" part of manpage
 djmw 20101009 Latest modification
*/

#include "KlattGrid.h"
#include "ManPagesM.h"
#include "MFCC.h"

void manual_LPC (ManPages me);
void manual_LPC (ManPages me)
{
MAN_BEGIN (U"Candidate modelling settings...", U"djmw", 20221129)
INTRO (U"A command in the #Candidates menu of the @FormantPathEditor window.")
TERM (U"##Coefficients by track#")
DEFINITION (U"determines how many coefficients will be used in the modelling of a formant track. "

"The second number determines the number of coefficients for the modelling of formant 2, and so on. " 
	"For example, if you specify \"4 3 3\", the first three formants will be modelled. Formant 1 will be modelled with 4 "
	"coefficients which means that a third order polynomial is used for modelling. Formant 2 and formant 3 are modelled with "
	"3 coefficients which means that their tracks will be modelled as parabolas.")
TERM (U"##Variance exponent#")
DEFINITION (U"determines the power of the first term in the overall stress criterion #S. "
	"The best model is the one with the lowest value for the stress %S.")
NORMAL (U"The stress criterion #S is defined in @@Weenink (2015)@ as")
EQUATION (U"%S = (%s^2 / %k)^^%varianceExponent^ (%\\ci^^2^ / %d),")
NORMAL (U"where %s^^2^ is the sum of the squares of the standard errors of all the coefficients of the modelled formant "
	"tracks within one Formant object, %k is the total number of coefficients of these tracks, in the example given above "
	"%k is 10 (= 4 + 3 + 3), \\ci^^2^ is the sum of the chi squares of each track, %d is the sum of the "
	"degrees of freedom of each track. "
	"Because the standard error %s is proportional to bandwidth and \\ci^^2^ is inversely proportional to bandwidth, "
	"the expression for %S grows approximately as 2\\.c%varianceExponent-2 with bandwidth. Setting the %varianceExponent "
	"somewhat larger than one guarantees that for two tracks whose formants only differ in their bandwidths, the track with the "
	"larger bandwidths obtains a larger value for the stress value %S.")
MAN_END

MAN_BEGIN (U"CC: Paint...", U"djmw", 20040407)
INTRO (U"A command to paint the cepstral coefficients in shades of grey.")
ENTRY (U"Settings")
TERM (U"##From coefficient#, ##To coefficient#")
DEFINITION (U"the range of coefficients that will be represented.")
MAN_END

MAN_BEGIN (U"CC: To DTW...", U"djmw", 20221114)
INTRO (U"You can choose this command after selecting two objects with cepstral coefficients (two @MFCC's or @LFCC's). "
	"With this command you perform dynamic time warping. ")
ENTRY (U"Algorithm")
NORMAL (U"First we calculate the distance matrix %D of all the distances between the cepstral frames of the two objects. "
	"This matrix has %n__1_ rows, the number of frames in the first object, and %n__2_ columns, the number of frames "
	"in the second object.")
NORMAL (U"The elements of %D are the distances %d__%ij_ between frame %i (from the first object) and frame %j "
	"(from the second one). The %d__%ij_ are composed of three parts:")
EQUATION (U"%d__%ij_ = %w__1_ \\.c %d__1_ + %w__2_ \\.c %d__2_ + %w__3_ \\.c %d__3_,")
NORMAL (U"where %w__1_, %w__2_ and %w__3_ are user-supplied weights and ")
TERM (U"%d__1_ = \\su^^N^__%k=1_ (%c__%ik_ - %c__%jk_)^2), ")
DEFINITION (U"the sum of the squared distances between the %N cepstral coefficients of the two frames. ")
TERM (U"%d__2_ = (c__%i0_ - c__%j0_)^2")
DEFINITION (U"the difference between the average 'power' of the two frames.")
TERM (U"%d__3_ = \\su^^N^__%k=1_ (%r__%ik_ - %r__%jk_)^2), ")
DEFINITION (U"in which %r__%ik_ is the regression coefficient of the cepstral coefficient %k, calculated "
	"from the frames within a time span of %T seconds, the %%regression window length%, around the centre of frame %i.")
NORMAL (U"After calculation of all the frame distances %d__%ij_, we find the optimal path through the distance matrix %D with a "
	"Viterbi-algorithm.")
MAN_END

MAN_BEGIN (U"CC: To Matrix", U"djmw", 20011123)
INTRO (U"Copies the cepstral coefficients of the selected @CC "
	"object to a newly created @Matrix object.")
ENTRY (U"Behaviour")
EQUATION (U"%z__%ji_ = %c__%ij_, with 1 \\<_ %i \\<_ %nx and "
	"1 \\<_ j \\<_ %numberOfCoefficients__%i_,")
NORMAL (U"where %z__%ji_ is the matrix element in row %j and column %i and "
	"%c__%ij_ is the %j-th cepstral coefficient in frame %i.")
MAN_END

MAN_BEGIN (U"CPP", U"djmw", 20260814)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A CPP object represents Cepstral Peak Prominence as a function of time.")
NORMAL (U"CPP shows periodicity in the Spectrum. Periodicity in the Spectrum is caused by periodicity in the sound if the sound is composed of %%multiple% harmonics. A single sine with frequency f0 does not show periodicity in the spectrum, just a single peak. Only if there are harmonics of f0, periodicity in the spectrum will turn up.")
MAN_END

MAN_BEGIN (U"Formant: List formant slope...", U"djmw", 20210813)
INTRO (U"A command available in the @@Query submenu@ if you select a @@Formant@ object. The Info window will show the characteristics of the slope of the chosen interval as a vector with a number of values.")
ENTRY (U"Settings")
TERM (U"##Formant number#,")
DEFINITION (U"defines the formant whose slope characteristics you want.")
TERM (U"##Time range (s)#,")
DEFINITION (U"defines the start time and the end time of the segment on which the slope should be determined.")
TERM (U"##Slope curve#")
DEFINITION (U"defines the kind of curve you want to fit on the formant points to determine the slope characteristics. "
	"The options available are ##Exponential plus constant#, ##Parabolic# and ##Sigmoid plus constant#.")
ENTRY (U"The vector result")
NORMAL (U"The vector values are determined from the fit of the formant track with a an ##exponential plus constant# function "
	"%F(%t) = %a+%b\\.cexp(%c\\.c%t), or a ##parabolic# function %F(%t) = %a+%b\\.ct+%c\\.ct^2, or a ##sigmoid plus constant# function "
	"%F(%t) = %a+%b / (1 + exp(- (%t -c) / d)) "
	"on the chosen interval [%t__min_, %t__max_].")
TERM (U"##1. Average slope (Hz / s)#")
DEFINITION (U"defined as (%F__start_ - %F__end_) / (%t__max_ - %t__min_), where %F__start_ and %F__end_ are the start and "
	"end values of the fitted function %F(%t) and %t__max_ and %t__min_ are the start and end times of the interval.")
TERM (U"##2. %R^2#")
DEFINITION (U"The %R^2 value of the fit defined as %R^2 = 1 - %varianceAfter / %varianceBefore.")
TERM (U"##3. %F__start_#")
DEFINITION (U"the frequency in hertz of the function %F(%t__min_).")
TERM (U"##4. %F__end_#")
DEFINITION (U"the frequency in hertz of the function %F(%t__max_).")
TERM (U"##5. %a#")
DEFINITION (U"the parameter %a (hertz) of the function %F(%t).")
TERM (U"##6. %b#")
DEFINITION (U"the parameter %b of the function %F(%t).")
TERM (U"##7. %c#")
DEFINITION (U"the parameter %c ( / s) of the function %F(%t).")
TERM (U"##8. %d#")
DEFINITION (U"the parameter %d of the function %F(%t), if the sigmoid plus constant function was chosen.")
ENTRY (U"Remarks about the interpretation of the fit parameters.")
NORMAL (U"The returned average slope parameter is reliable only if the formant trajectory is clearly not constant and there is a large difference between the %F__start_ and the %F__end_ values. In cases where the formant trajectory shows a noisy pattern all return values have a large error margin and the determined average slope can also be unreliable.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm to fit the %%non-linear% exponential plus constant and the sigmoid plus constant functions to a series of (time, frequency) values by a non-iterative algorithm is described in @@Jacquelin (2009)@.")
MAN_END

MAN_BEGIN (U"HTK parameter file format", U"djmw", 20210311)
INTRO (U"HTK parameter format files consist of a contiguous sequence of frames preceded by a header. "
	"Each frame is a vector of either 2-byte integers or 4-byte floats. 2-byte integers are used for "
	"compressed forms and for vector quantised data. All multi-byte numbers are written as big-endian numbers.")
NORMAL (U"The HTK file format header is 12 bytes long and contains the following data:")
TERM (U"numberOfFrames (4-byte integer)")
DEFINITION (U"The number of analysis frames in a file")
TERM (U"samplePeriod (4-byte integer)")
DEFINITION (U"The sample period in units of 100 ns. A sampling frequency of 10 kHz would correspond to a sample period of 0.0001 s and to a value of 1000 in this field.")
TERM (U"frameSize (2-byte integer)")
DEFINITION (U"The number of bytes per frame.")
TERM (U"parameterKind (2-byte integer)")
DEFINITION (U"A code indication what kind of frames the file contains.")
ENTRY (U"Remarks")
NORMAL (U"The HTK parameter files do not contain specific information that identifies them as "
	"HTK parameter files. However, the file type can be deduced as follows. If we have any file and interpret "
	"the first 12 bytes as the above format specifies then we know that the first three numbers read have to "
	"be positive integers, that frameSize has to an even number and that %numberOfFrames * %frameSize + 12 must be equal to the number "
	"of bytes in the file. The chance that a random data file fulfils these conditions is very small.")
ENTRY (U"VTRFormants data in HTK parameter format")
NORMAL (U"The VTRFormants data of @@Deng et al. (2006)@ can serve as reference formant frequency values for "
	"(part of) the TIMIT acoustic phonetic corpus. These data are stored in HTK parameter files with extension \".fb\". "
	"They can be download from %%http://www.seas.ucla.edu/spapl/VTRFormants.html%.")
NORMAL (U"HTK parameter files do not contain timing information and therefore we can only calculate the domain of "
	"the Formant from external information. From the ##Deng et al. (2006)# paper we assume that the data were taken "
	"every 10 ms and therefore the best guess for the duration of the Formant equals %numberOfFrames * 0.01."
	"The value in these files for the samplePeriod is 10000 which is a factor 10 off from the correct value of 1000 in units of 100 ns.")
MAN_END

MAN_BEGIN (U"FormantPath", U"djmw", 20230225)
INTRO (U"One of the @@types of objects@ in Praat. A ##FormantPath# object maintains a path through a collection of "
	"Formant objects, each the result of a formant frequency analysis of the same sound but with a different setting "
	"of the analysis parameters.")
NORMAL (U"A FormantPath combines a collection of @@Formant@s with a TextGrid that indicates which of these formants is preferred "
	"in a certain time interval. ")
NORMAL (U"As an example we consider a FormantPath that has nine Formant objects in its collection. "
	"Suppose these formant objects were the result of nine @@Sound: To Formant (burg)...@ analyses of the ##same# sound, "
	"however, with a difference only in the \"Formant ceiling (Hz)\" parameter setting which was 4093.7, 4303.5, 4524.2, 4756.1, "
	"5000.0, 5256.4, 5525.9, 5809.2, and 6107.0 Hz, respectively. "
	"In this way, this collection of Formants functions as a set of alternative analyses where the \"Time step\", "
	"the \"Max. number of formants\", the \"Window length\" and the \"Pre-emphasis from\" are constant for each analysis and only "
	"the frequency interval in which the number of formants should fit varies. ")
NORMAL (U"You create a FormantPath from a Sound with the @@Sound: To FormantPath (burg)...@ command. The FormantPath example above could have been created from a Sound by:")
CODE (U"To FormantPath (burg): 0.005, 5.0, 5000.0, 0.025, 50.0, 0.05, 4")
NORMAL (U"The Formant object which has a ceiling of 5000 Hz, the object at the middle in the set, here corresponds to the "
	"result of the \"standard\" analysis for a male voice where the interval from 0 to 5000 Hz is chosen and we expect, "
	"on average, five formant to exist in this interval. ")
NORMAL (U"To choose your own path through the alternatives you can use Praat's @@FormantPathEditor@.")
MAN_END

MAN_BEGIN (U"FormantPath: Down to Table (optimal interval)...", U"djmw", 20210817)
INTRO (U"A command that creates a @Table with formant frequency values for the chosen interval of the selected @@FormantPath@. The values in the table are from the segment that has the lowest stress value for the fit of its formant tracks.")
MAN_END

MAN_BEGIN (U"FormantPath: Down to Table (stresses)...", U"djmw", 20230235)
INTRO (U"A command that creates a @Table with stress of fit values for the selected @@FormantPath@.")
NORMAL (U"The resulting Table has one row for each of the ceiling frequencies. The number of columns will depend on the number of "
	"formant tracks that are modelled. ")
MAN_END

MAN_BEGIN (U"Sound: To FormantPath (burg)...", U"djmw", 20230225)
INTRO (U"A command that creates a @@FormantPath@ object from each selected @@Sound@ . ")
ENTRY (U"##Settings")
NORMAL (U"The settings for ##Time step (s)#, ##Maximum number of formants#, ##Window length (s)# and ##Pre-emphasis from (Hz)# "
	"are as you would set them with the @@Sound: To Formant (burg)...@ method. "
	"The defaults are 0.005 seconds, 5.0 formants, 0.025 seconds, and 50.0 Hz, respectively.")
TERM (U"##Middle formant ceiling (Hz)")
DEFINITION (U"determines the middle formant ceiling frequency in Hz. You normaly would use 5500.0 Hz for an average female voice "
	"and 5000.0 Hz for an average male voice as you would do for the ##Formant ceiling (Hz)# setting in ##To Formant (burg)...#. "
	"However, instead of performing only one analysis with a fixed ceiling, we perform "
	"multiple analyses, each with a different ceiling frequency. The number of analyses with a %%lower% formant ceiling than the "
	"%%middle formant ceiling% is equal to the number of analyses with a %%higher% formant ceiling than the %%middle formant ceiling%. ")
TERM (U"##Ceiling step size#")
DEFINITION (U"defines the increase or decrease in the formant ceiling between two successive analyses as exp(%ceilingStepSize) "
	"when we step up, or as exp(-%ceilingStepSize) when we step down.")
TERM (U"##Number of steps up / down")
DEFINITION (U"determines the number of steps we go up as well as the number of steps we go down with respect to the %middle formant ceiling%. "
	"The ceiling frequency for the %i^^th^ step down is %middleFormantCeiling\\.cexp (-%i\\.c%ceilingStepSize) and for the %i^^th^ step up "
	"is %middleFormantCeiling\\.cexp (+%i\\.c%ceilingStepSize). The total number of analyses is always 2\\.c%numberOfStepsUpDown+1.")
ENTRY (U"Algorithm")
NORMAL (U"The following algorithm describes what is going on. ")
CODE (U"ceiling [numberOfStepsUpDown + 1] = middleCeiling")
CODE (U"for istep from 1 to 2 * numberOfStepsUpDown + 1")
	CODE1 (U"if istep <= numberOfStepsUpDown")
		CODE2 (U"ceiling [istep] = middleFormantCeiling * exp (-(numberOfStepsUpDown - istep + 1) * ceilingStepSize)")
	CODE1 (U"elsif istep > numberOfStepsUpOrDown + 1")
		CODE2 (U"ceiling [istep] = middleFormantCeiling * exp ((istep - numberOfStepsUpDown - 1) * ceilingStepSize)")
	CODE1 (U"selectObject: sound")
	CODE1 (U"formant [istep] = To Formant (burg): timeStep, maxNumberOfFormants, ceiling [istep], windowLength, preEmphasis")
CODE (U"endfor")
NORMAL (U"This description is approximate because in the \"To Formant\" step we have to guarantee that all the Formant objects get the same time sampling.")
MAN_END

MAN_BEGIN (U"FormantPathEditor", U"djmw", 20220816)
INTRO (U"One of the @@Editors@ in Praat, for editing a @@FormantPath@ object.")
NORMAL (U"You can optionally include a @Sound and a @TextGrid in this editor, by selecting both the Sound and the FormantPath together, or the Sound and the TextGrid and the FormantPath together before clicking ##View & Edit#.")
NORMAL (U"With the FormantPathEditor you can, for each interval that you select, "
	"replace its formant frequencies and bandwidths by the corresponding values "
	"from one of the alternative Formant objects in the FormantPath's collection.")
ENTRY (U"Editor layout")
NORMAL (U"The left part of the editor is similar to the layout of the @@SoundEditor@.")
NORMAL (U"The right part is called the %%selection viewer%. "
	"Here you see alternative formant frequency analyses of the selected part of the sound laid out in a grid "
	"(or of the whole visible sound window if there is no selection).")
NORMAL (U"The selection viewer shows not only a formant's frequency but also its bandwidth as a vertical line. "
	"This will give you a better impression of the analysis results because well defined formants have small bandwidths "
	"and, therefore, show short vertical lines.")
ENTRY (U"How to operate")
NORMAL (U"When you start to edit a new FormantPath object, the formants in the path are set equal to the formants of the default "
	"analysis. This guarantees that there always is a path at the start. The ceiling of this path is indicated by the fat red "
	"line in the upper part of the spectrogram. "
	"If you click in one of the rectangles in the selection viewer the values of the formant frequencies (and bandwidths) "
	"in the selected part on the left are replaced by the values present in the rectangle and the fat red line will indicate "
	"the new ceiling. The colour of the clicked rectangle on the right will also change.")
NORMAL (U"This also works the other way around: if you click near a red line in the spectrogram the corresponding selection viewer "
	"will be shown.")
ENTRY (U"Details")
NORMAL (U"The stress numbers that are displayed above the rectangles in the selection viewer "
	"are explained in @@Weenink (2015)@. Basically each number is a combined stress score of the individual formant tracks "
	"within the rectangle. Each track's stress score quantifies how well the track has been modelled. "
	"The lower this number is, the better the track is modelled by a polynomial of the chosen order. "
	"The higher the order, the more flexible the curve is and the better it can adapt to the data. "
	"The higher the order of the polynomial, the more parameters are needed in the model. "
	"You may change the number of parameters that model each track with the ##Candidate modelling settings...# option in the Candidates menu.")
MAN_END

MAN_BEGIN (U"Formants: Extract smoothest part...", U"djmw", 20140313)
INTRO (U"Extracts the part from one of the selected formants which shows the smoothest formant tracks in a given interval. ")
ENTRY (U"Settings")
SCRIPT (5, Manual_SETTINGS_WINDOW_HEIGHT (5), U""
	Manual_DRAW_SETTINGS_WINDOW (U"Formants: Extract smoothest part", 5)
	Manual_DRAW_SETTINGS_WINDOW_RANGE (U"Time range (s)", U"0.0", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_RANGE (U"Fitter formant range", U"1", U"3")
	Manual_DRAW_SETTINGS_WINDOW_FIELD (U"Order of polynomials", U"3")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN (U"Use bandwidths to model formant tracks", 1)
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN (U"Bandwidths for stress test", 0)
)
TERM (U"##Time range (s)#")
DEFINITION (U"determines the position of the intervals that have to be compared.")
TERM (U"##Fitter formant range")
DEFINITION (U"determines which formant tracks will be modelled with a polynomial function. The goodness of fit of these models "
	"will be used in the comparison.")
TERM (U"##Order of polynomials")
DEFINITION (U"determines the maximum order of the polynomials that are used in modelling each formant track. Order 0 means a "
	"model which is a constant function; this model needs only one parameter. Order 1 means a model that is a straight line "
	"function; this order needs two parameters. Order 2 means that an additional parabolic function is used in the modelling; "
	"order 2 needs therefore 3 parameters. In general an order %p model needs %p+1 parameters.")
TERM (U"##Use bandwidths to model formant tracks")
DEFINITION (U"Bandwidths give an indication about the sharpness of a spectral peak. Sharp peaks have small bandwidths and, "
	"vice versa, broad peaks have large bandwidths. The width of a peak can also be interpreted as a measure of certainty "
	"for its formant frequency value. Setting this option %%on%, the default setting, means that you force the modelling "
	"function to be closer to frequencies that are well defined, i.e. that have sharp peaks, than to the frequencies of "
	"broad peaks, if choices have to be made. The consequence is that in the model sharp peaks will be better represented than broad peaks.")
TERM (U"##Bandwidths for stress test")
DEFINITION (U"determines whether for the stress determination the formant frequencies are still needed. Not using them anymore "
	"probably gives a better indication of the stress of a track.")
MAN_END

#define PowerCepstrum_manual_pitchRange \
	U"determine the limits of the quefrency range where a peak is searched for. The lower quefrency is determined as " \
	"1 / %%pitchCeiling% and this value is in general more critical than " \
	"the value of the upper quefrency which equals 1 / %%pitchFloor%. A %%pitchCeiling% of 300 Hz will correspond to a " \
	"lower quefrency of 1/300\\~~0.0033 seconds."
	
#define PowerCepstrum_manual_trendRange \
	U"the quefrency range for which the amplitudes (in dB) will be modelled by a straight line. " \
	"In @@Hillenbrand et al. (1994)@, the lower value for this range was chosen as 0.001 s " \
	"in order to reduce the effect of very low quefrency data on the straight line fit. In our analysis this value " \
	"is not so critical if we use the robust fitting procedure. If you choose the \"Least squares\" fit method " \
	"then it matters more."

#define PowerCepstrum_manual_trendType \
	U"defines how to model the cepstrum background. We can model it with a straight line as was " \
	"done in @@Hillenbrand et al. (1994)@. The slope of this line will generally be negative " \
	"because the background amplitudes get weaker for higher quefrencies. Or, we could use an exponential " \
	"model in which the background cepstral amplitudes decay in a non-linear fashion.  "

#define PowerCepstrum_manual_fitMethod \
	U"defines how the line that models the cepstrum background is calculated. The default method, " \
	"\"Robust slow\", corresponds to @@theil regression|Theil's robust line fit@. The \"Robust\" method corresponds to the incomplete theil regression and is computationally faster but somewhat less precise. To be compatible with the past, a standard least squares " \
	"line fit can also be chosen but it is much less precise than the other two because a least squares fit is much more influenced by the peak cepstral values than the other two."

#define PowerCepstrum_manual_quefrencyAveragingWindow \
	U"determines the width of the averaging window in the quefrency domain. " \
	"By chosing a value of zero, you can prevent any smoothing in the quefrency dimension."

#define PowerCepstrogram_manual_timeAveraging \
	U"determines the width of the averaging window in the time domain. The result " \
	"of the smoothing will be that in the new smoothed PowerCepstrogram each cepstral value is the average of the cepstral values " \
	"within the averaging window that was positioned symmetrically around the center of this frame in the selected PowerCepstrogram. " \
	"By choosing a value of zero, you can prevent any smoothing in the time dimension. "

MAN_BEGIN (U"PowerCepstrogram", U"djmw", 20190909)
INTRO (U"One of the @@types of objects@ in Praat. A cepstrogram represents a time-quefrency representation of a sound. "
	"Horizontally it shows time, vertically it shows quefrency while the quefrency power density is shown as shades of grey.")
MAN_END

MAN_BEGIN (U"PowerCepstrogram: Get CPPS...", U"djmw", 20220721)
INTRO (U"A command to get the cepstral peak prominence (CPP) of the selected @@PowerCepstrogram@. ")
NORMAL (U"The returned value is the average of the cepstral peak prominences of the individual frames.")
ENTRY (U"Settings")
TERM (U"##Subtract trend before smoothing#")
DEFINITION (U"determines whether the smoothing should be performed on the Cepstrogram after the trend of each PowerCepstrum frame has been removed. Because, in general, the trends in the analysis frames will not be equal, the value of CPPS will be different whether or not smoothing is performed. If no smoothing is going on the result should be independent of this setting. ")
TERM (U"##Time averaging window (s)#")
DEFINITION (PowerCepstrogram_manual_timeAveraging)
TERM (U"##Quefrency averaging window (s)#")
DEFINITION (PowerCepstrum_manual_quefrencyAveragingWindow)
TERM (U"##Peak search pitch range (Hz)#")
DEFINITION (PowerCepstrum_manual_pitchRange)
TERM (U"##Tolerance#")
DEFINITION (U"")
TERM (U"##Interpolation#")
DEFINITION (U"determines how the @@vector peak interpolation|amplitude and position of a peak are determined@.")
TERM (U"##Trend line quefrency range (s)#")
DEFINITION (PowerCepstrum_manual_trendRange)
TERM (U"##Trend type#")
DEFINITION (PowerCepstrum_manual_trendType)
TERM (U"##Fit method#")
DEFINITION (PowerCepstrum_manual_fitMethod)
MAN_END

MAN_BEGIN (U"PowerCepstrogram: To Table (cepstral peak prominences)...", U"djmw", 20210916)
INTRO (U"A command to create a table with @@PowerCepstrum: Get peak prominence...|cepstral peak prominence@ values.")
ENTRY (U"Settings")
SCRIPT (5, Manual_SETTINGS_WINDOW_HEIGHT (7.8), U""
	Manual_DRAW_SETTINGS_WINDOW ("PowerCepstrogram: To Table (cepstral peak prominences)", 7.8)   // 9 - 4 * 0.3 (four is the number of additional radio buttons)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Peak search pitch range (Hz)", U"60.0", U"300.0")
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"Interpolation", U"None", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"parabolic", 1)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"cubic", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"sinc70", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"sinc700", 0)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Trend line quefrency range (s)", U"0.001", U"0.05")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"Trend type", U"Exponential decay")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"Fit method", U"Robust")
)
NORMAL (U"The meaning of these settings is explained @@PowerCepstrum: Get peak prominence...|here@.")
MAN_END

MAN_BEGIN(U"PowerCepstrogram: Paint...", U"djmw", 20131001)
INTRO (U"A command to draw the selected @@PowerCepstrogram@ object(s) in shades of grey.")
ENTRY (U"Settings")
SCRIPT (5, Manual_SETTINGS_WINDOW_HEIGHT (7), U""
	Manual_DRAW_SETTINGS_WINDOW (U"PowerCepstrogram: Paint", 7)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", U"0.0", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Quefrency range (s)", U"0.0", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD("Maximum (dB)", U"80.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Autoscaling",0)
	Manual_DRAW_SETTINGS_WINDOW_FIELD("Dynamic range (dB)", U"30.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD("Dynamic compression (0-1)", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish",1)
)
TERM (U"##Time range (s)")
DEFINITION (U"the time domain along the %%x% axis.")
TERM (U"##Quefrency range (s)")
DEFINITION (U"the quefency domain along the %%y% axis.")
TERM (U"##Maximum (dB)")
DEFINITION (U"cells that have cepstral values greater or equal than this value are drawn in black.")
TERM (U"##Autoscaling")
DEFINITION (U"If %%on%, overrules the effects of the previous option and the following three options. I.e. the global maximum and the "
	"global minimum cepstral values determine the maximum blackness and the minimal blackness. Values in-between have apropriate "
	"values of grey.")
TERM (U"##Dynamic range (dB)")
DEFINITION (U"All values more than %%Dynamic range% below the maximum will be drawn in white. Values in-between have apropriate "
	"values of grey.")
TERM (U"##Dynamic compression (0-1)")
DEFINITION (U"determines how much stronger weak frames should be made before drawing. Normally this parameter is between 0 and 1. "
	"If it is 0, no dynamic compression will take place. If it is 1, all time frames (vertical bands) will contain cepstral values "
	"that are drawn in black. If the global %%maximum% is set at 80 dB, %%autoscaling% is off, %%dynamic range% is 30 dB and "
	"%%dynamic compression% is 0.4 then for a frame with a local maximum of 40 dB all values will be lifted by 0.4*(80-40)=16 dB, "
	"so that its maximum will be seen at 56 dB (thus making the corresponding cell visible).")
TERM (U"##Garnish")
DEFINITION (U"Draws a box around the cepstrogram and labels the axes.")
MAN_END

MAN_BEGIN (U"PowerCepstrogram: Smooth...", U"djmw", 20201216)
INTRO (U"Smoothes the selected @PowerCepstrogram by averaging with a rectangular window. The smoothed PowerCepstrogram is the "
	"result of two separate steps. "
	"In the first step, cepstra are averaged across time. In the second step, cepstra are averaged across quefrency.")
ENTRY (U"Settings")
TERM (U"##Time averaging window (s)")
DEFINITION (PowerCepstrogram_manual_timeAveraging)
TERM (U"##Quefrency averaging window (s)")
DEFINITION (PowerCepstrum_manual_quefrencyAveragingWindow)
ENTRY (U"Note")
NORMAL (U"The following commands should reproduce the smoothing described in the @@Hillenbrand & Houde (1996)@ article, "
	"where they use a 20 ms (10 frame) time smoothing and a 1 ms (10 bin) quefrency smoothing. ")
CODE (U"selectObject (\"Sound xxx\")")
CODE (U"To PowerCepstrogram: 60.0, 0.002, 5000.0, 50.0")
CODE (U"Smooth: 0.02, 0.001")
MAN_END

MAN_BEGIN (U"Cepstrum", U"djmw", 20130616)
INTRO (U"One of the @@types of objects@ in Praat.")
ENTRY (U"Description")
NORMAL (U"A Cepstrum is the log spectrum of the log power spectrum.")
MAN_END

MAN_BEGIN (U"PowerCepstrum", U"djmw", 20200403)
INTRO (U"One of the @@types of objects@ in Praat.")
ENTRY (U"Description")
NORMAL (U"A PowerCepstrum is the power spectrum of the log power spectrum. When drawn the vertical scale "
	"will show the amplitude expressed in dB. The horizontal scale shows %%quefrency% in units of seconds. "
	"It is calculated from the ##Spectrum# by a method described at @@Spectrum: To PowerCepstrum@.")
MAN_END

MAN_BEGIN (U"PowerCepstrum: Get peak prominence...", U"djmw", 20190912)
INTRO (U"Calculates the cepstral peak prominence measure (CPP) as defined by @@Hillenbrand et al. (1994)@")
NORMAL (U"The CPP measure is the difference in amplitude between the cepstral peak and the corresponding value on the trend "
	"line that is directly below the peak (i.e., the predicted magnitude for the quefrency at the cepstral peak). "
	"The CPP measure represents how far the cepstral peak emerges from the cepstrum background. ")
ENTRY (U"Settings")
SCRIPT (7, Manual_SETTINGS_WINDOW_HEIGHT (6.8), U""
	Manual_DRAW_SETTINGS_WINDOW (U"PowerCepstrum: Get peak prominence", 6.8)   // 8 - 4 * 0.3 (four is the number of additional radio buttons)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Search peak in pitch range (s)", U"60.0", U"333.3")
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"Interpolation", U"None", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"parabolic", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"cubic", 1)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"sinc70", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_CHOICE (U"", U"sinc700", 0)
	Manual_DRAW_SETTINGS_WINDOW_RANGE (U"Trend line quefrency range (s)", U"0.001", U"0.0 (= end)")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"Fit method", U"Robust")
)
TERM (U"##Search peak in pitch range")
DEFINITION (PowerCepstrum_manual_pitchRange)
TERM (U"##Interpolation")
DEFINITION (U"determines how the @@vector peak interpolation|amplitude and position of a peak are determined@.")
TERM (U"##Trend line quefrency range")
DEFINITION (PowerCepstrum_manual_trendRange)
TERM (U"##Trend type#")
DEFINITION (PowerCepstrum_manual_trendType)
TERM (U"##Fit method")
DEFINITION (PowerCepstrum_manual_fitMethod)
ENTRY (U"Examples")
NORMAL (U"Next picture of a PowerCepstrum with its straight blue trend line and its corresponding "
	"peak prominence value was generated with the following script. Note that the first four lines in the "
	"script are only necessary to generate a PowerCepstrum of a part of a vowel. ")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000")
CODE (U"To Sound")
CODE (U"To PowerCepstrogram: 60, 0.002, 5000, 50")
CODE (U"To PowerCepstrum (slice): 0.1")
CODE (U"prominence = Get peak prominence: 60, 333.3, \"parabolic\", 0.001, 0.05, \"straight\", \"robust slow\"")
CODE (U"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"")
CODE (U"Draw: 0, 0, 0, 110, \"yes\"")
CODE (U"Colour: \"blue\"")
CODE (U"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"straight\", \"robust slow\"")
CODE (U"Colour: \"black\"")
CODE (U"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"")
SCRIPT (5, 3, U""
	"kg = Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000\n"
	"vowel = To Sound\n"
	"cepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50\n"
	"cepstrum = To PowerCepstrum (slice): 0.1\n"
	"prominence = Get peak prominence: 60, 333.3, \"parabolic\", 0.001, 0.05, \"straight\", \"robust slow\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"Draw: 0, 0, 0, 110, \"yes\"\n"
	"Colour: \"Blue\"\n"
	"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"straight\", \"robust slow\"\n"
	"Colour: \"Black\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"removeObject: kg, vowel, cepstrogram, cepstrum\n")
NORMAL (U"In the next picture the trend line is of exponential decay type and consequently the "
	"peak prominence value has changed a little bit.")
SCRIPT (5, 3, U""
	"kg = Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000\n"
	"vowel = To Sound\n"
	"cepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50\n"
	"cepstrum = To PowerCepstrum (slice): 0.1\n"
	"prominence = Get peak prominence: 60, 333.3, \"parabolic\", 0.001, 0.05, \"exponential decay\", \"robust slow\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"Draw: 0, 0, 0, 110, \"yes\"\n"
	"Colour: \"Blue\"\n"
	"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"exponential decay\", \"robust slow\"\n"
	"Colour: \"Black\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"removeObject: kg, vowel, cepstrogram, cepstrum\n")

MAN_END

MAN_BEGIN (U"PowerCepstrum: Draw...", U"djmw", 20190914)
INTRO (U"A command to draw the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Quefrency range (s)#")
DEFINITION (U"define the extrema on the horizontal scale of the picture.")
TERM (U"##Amplitude range (dB)#")
DEFINITION (U"define the extrema on the vertical scale of the picture")
ENTRY (U"Remark")
NORMAL (U"Cepstrum values are drawn as 20\\.clog10 (value +\\ep), "
	"where \\ep is a small number that avoids taking the logarithm of zero if the cepstrum value happens to be zero.")
MAN_END

MAN_BEGIN (U"PowerCepstrum: Draw trend line...", U"djmw", 20191008)
INTRO (U"Draws the line that models the background of the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Quefrency range (s)#")
DEFINITION (U"define the extrema on the horizontal scale of the picture.")
TERM (U"##Amplitude range (dB)#")
DEFINITION (U"define the extrema on the vertical scale of the picture")
TERM (U"##Trend line quefrency range (s)")
DEFINITION (PowerCepstrum_manual_trendRange)
TERM (U"##Trend type#")
DEFINITION (PowerCepstrum_manual_trendType)
TERM (U"##Fit method")
DEFINITION (PowerCepstrum_manual_fitMethod)
ENTRY (U"Examples")
NORMAL (U"The next picture shows a PowerCepstrum with %%two% drawn trend lines, a straight line in blue "
	"and an exponential decay line in green.  The picture was generated by the following script. "
	"Note that the first four lines in the script are only necessary to generate a PowerCepstrum of a part of a vowel.")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000")
CODE (U"To Sound")
CODE (U"To PowerCepstrogram: 60, 0.002, 5000, 50")
CODE (U"To PowerCepstrum (slice): 0.1")
CODE (U"Draw: 0, 0, 0, 110, \"yes\"")
CODE (U"Colour: \"Blue\"")
CODE (U"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"Straight\", \"Robust slow\"")
CODE (U"Colour: \"Green\"")
CODE (U"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"Exponential decay\", \"Robust slow\"")
CODE (U"Colour: \"Black\"")
SCRIPT (5, 3, U""
	"kg = Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000\n"
	"vowel = To Sound\n"
	"cepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50\n"
	"cepstrum = To PowerCepstrum (slice): 0.1\n"
	"Draw: 0, 0, 0, 110, \"yes\"\n"
	"Colour: \"Blue\"\n"
	"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"Straight\", \"Robust slow\"\n"
	"Colour: \"Green\"\n"
	"Draw trend line: 0, 0, 0, 110, 0.001, 0.05, \"Exponential decay\", \"Robust slow\"\n"
	"Colour: \"Black\"\n"
	"removeObject: kg, vowel, cepstrogram, cepstrum\n")
MAN_END

MAN_BEGIN (U"PowerCepstrum: Get peak...", U"djmw", 20190910)
INTRO (U"Get the amplitude of the peak in the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Search peak in pitch range (Hz)#")
DEFINITION (PowerCepstrum_manual_pitchRange)
TERM (U"##Interpolation#")
DEFINITION (U"determines how the @@vector peak interpolation|amplitude and position of a peak are determined@.")
MAN_END

MAN_BEGIN (U"PowerCepstrum: Get quefrency of peak...", U"djmw", 20190910)
INTRO (U"A command to get the quefrency of the peak in the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Search peak in pitch range (Hz)#")
DEFINITION (PowerCepstrum_manual_pitchRange)
TERM (U"##Interpolation#")
DEFINITION (U"determines how the @@vector peak interpolation|amplitude and position of a peak are determined@.")
MAN_END

MAN_BEGIN (U"PowerCepstrum: Get trend line slope...", U"djmw", 20190910)
INTRO (U"A command to calculate the slope of the line that models the cepstrum background of the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Trend line quefrency range (s)#")
DEFINITION (PowerCepstrum_manual_trendRange)
TERM (U"##Trend type#")
DEFINITION (PowerCepstrum_manual_trendType)
TERM (U"##Fit method")
DEFINITION (PowerCepstrum_manual_fitMethod)
MAN_END

MAN_BEGIN (U"PowerCepstrum: Get trend line intercept...", U"djmw", 20190910)
INTRO (U"A command to calculate the intercept of the line that models the cepstrum background of the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Trend line quefrency range (s)#")
DEFINITION (PowerCepstrum_manual_trendRange)
TERM (U"##Trend type#")
DEFINITION (PowerCepstrum_manual_trendType)
TERM (U"##Fit method")
DEFINITION (PowerCepstrum_manual_fitMethod)
MAN_END

MAN_BEGIN (U"PowerCepstrum: Smooth...", U"djmw", 20191005)
INTRO (U"A command to smooth the selected @@PowerCepstrum@ by averaging values at successive quefrencies.")
ENTRY (U"Settings")
TERM (U"##Quefrency averaging window (s)#")
DEFINITION (PowerCepstrum_manual_quefrencyAveragingWindow)
TERM (U"##Number of iterations#")
DEFINITION (U"determines how often the averaging will take place. If chosen 2, for example, the output PowerCepstrum "
	"after the first averaging will be averaged once again.")
ENTRY (U"Examples")
NORMAL (U"The figure below is the PowerCepstrum taken from an artificial /a/ vowel synthesized as follows:")
CODE (U"Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000")
CODE (U"To Sound")
CODE (U"To PowerCepstrogram: 60, 0.002, 5000, 50")
CODE (U"To PowerCepstrum (slice): 0.1")
CODE (U"prominence = Get peak prominence: 60, 333.3, \"Parabolic\", 0.001, 0.05, \"Straight\", \"Robust slow\"")
CODE (U"Draw: 0, 0, 0, 110, \"yes\"")
CODE (U"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"")
SCRIPT (5, 3, U""
	"kg = Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000\n"
	"vowel = To Sound\n"
	"cepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50\n"
	"cepstrum = To PowerCepstrum (slice): 0.3\n"
	"prominence = Get peak prominence: 60, 333.3, \"Parabolic\", 0.001, 0.05, \"Straight\", \"Robust slow\"\n"
	"Draw: 0, 0, 0, 110, \"yes\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"removeObject: kg, vowel, cepstrogram, cepstrum\n")
NORMAL (U"After 1 iteration with an averaging window of 0.0005 s the PowerCepstrum is:")
SCRIPT (5, 3, U""
	"kg = Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000\n"
	"vowel = To Sound\n"
	"cepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50\n"
	"cepstrum = To PowerCepstrum (slice): 0.3\n"
	"smooth = Smooth: 0.0005, 1\n"
	"prominence = Get peak prominence: 60, 333.3, \"Parabolic\", 0.001, 0.05, \"Straight\", \"Robust slow\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"Draw: 0, 0, 0, 110, \"yes\"\n"
	"removeObject: kg, vowel, cepstrogram, cepstrum, smooth\n")
NORMAL (U"After 2 iterations with an averaging window of 0.0005 s the PowerCepstrum is:")
SCRIPT (5, 3, U""
	"kg = Create KlattGrid from vowel: \"a\", 0.3, 125, 800, 80, 1200, 80, 2300, 100, 2800, 0.1, 1000\n"
	"vowel = To Sound\n"
	"cepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50\n"
	"cepstrum = To PowerCepstrum (slice): 0.3\n"
	"smooth = Smooth: 0.0005, 2\n"
	"prominence = Get peak prominence: 60, 333.3, \"Parabolic\", 0.001, 0.05, \"Straight\", \"Robust slow\"\n"
	"Draw: 0, 0, 0, 110, \"yes\"\n"
	"Text top: \"no\", \"Peak prominence = \" + fixed$ (prominence, 2) + \" dB\"\n"
	"removeObject: kg, vowel, cepstrogram, cepstrum, smooth\n")
MAN_END

MAN_BEGIN (U"PowerCepstrum: Subtract trend...", U"djmw", 20190914)
INTRO (U"Subtract the cepstrum trend from the selected @@PowerCepstrum@.")
ENTRY (U"Settings")
TERM (U"##Trend line quefrency range (s)#")
DEFINITION (PowerCepstrum_manual_trendRange)
TERM (U"##Trend type#")
DEFINITION (PowerCepstrum_manual_trendType)
TERM (U"##Fit method#")
DEFINITION (PowerCepstrum_manual_fitMethod)
MAN_END

MAN_BEGIN (U"Formant & Spectrogram: To IntensityTier...", U"djmw", 20130109)
INTRO (U"Determines the formant intensities from the selected Spectrogram.")
NORMAL (U"The intensities at the frequencies of the selected formant are copied from the corresponding positions in the spectrogram. "
	"If the selected formant doesn't exist in a particular frame, then a large negative value (< -200 dB) is substituted instead. "
	"Because the values in the spectrogram are expressed in Pa^^2^/Hz, the units in the intensity tier are in dB/Hz. ")
MAN_END

MAN_BEGIN (U"LFCC", U"djmw", 20040421)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type LFCC represents cepstral "
	"coefficients on a linear frequency scale as a function of time. "
	"The coefficients are represented in frames with constant sampling "
	"period.")
ENTRY (U"#LFCC commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@LPC: To LFCC...")
MAN_END

MAN_BEGIN (U"LFCC: To LPC...", U"djmw", 20040407)
INTRO (U"You can choose this command after selecting 1 or more @LFCC's.")
ENTRY (U"Settings")
TERM (U"##Number of coefficients")
DEFINITION (U"the desired number of linear predictive coefficients.")
ENTRY (U"Behaviour")
NORMAL (U"The transformation from cepstral coefficients to %a-coefficients "
	"as described in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN (U"LPC", U"djmw", 19990610)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type LPC represents filter coefficients as a function of time. "
	"The coefficients are represented in frames with constant sampling period.")
ENTRY (U"LPC commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Sound: To LPC (autocorrelation)...@")
LIST_ITEM (U"\\bu @@Sound: To LPC (covariance)...@")
LIST_ITEM (U"\\bu @@Sound: To LPC (burg)...@")
LIST_ITEM (U"\\bu @@Sound: To LPC (marple)...@")
ENTRY (U"Conversion")
LIST_ITEM (U"\\bu @@LPC: To LFCC...|To LFCC...@")
LIST_ITEM (U"\\bu @@LPC: To Spectrogram...|To Spectrogram...@")
LIST_ITEM (U"\\bu @@LPC: To Spectrum (slice)...|To Spectrum (slice)...@")
LIST_ITEM (U"\\bu @@LPC: To Polynomial (slice)...|To Polynomial (slice)...@")
MAN_END

MAN_BEGIN (U"LPC: Draw gain...", U"djmw", 20040407)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Settings")
TERM (U"##From time (s)#, ##To time (seconds)#")
DEFINITION (U"the time domain along the %x-axis.")
TERM (U"##Minimum gain#, ##Maximum gain#")
DEFINITION (U"the range for the %y-axis.")
TERM (U"##Garnish")
DEFINITION (U"determines whether to draw a bounding box and axis labels.")
ENTRY (U"Behaviour")
NORMAL (U"Gain will be drawn as a function of time (gain also equals the prediction error "
	"energy). ")
MAN_END

MAN_BEGIN (U"LPC: Draw poles...", U"djmw", 20040407)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time of the nearest frame.")
ENTRY (U"Behaviour")
NORMAL (U"The @@Roots|roots@ of the @@LPC: To Polynomial (slice)...|linear prediction "
	"polynomial@, constructed from the coefficients "
	"of the analysis frame, will be drawn in the complex plane.")
MAN_END

MAN_BEGIN (U"LPC: To LFCC...", U"djmw", 20040407)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Behaviour")
NORMAL (U"The transformation from a-coefficients to cepstral coefficients "
	"as described in @@Markel & Gray (1976)@.")
ENTRY (U"Settings")
TERM (U"%%Number of coefficients%")
DEFINITION (U"the desired number of cepstral coefficients.")
MAN_END

MAN_BEGIN (U"LPC: To Matrix", U"djmw", 20011123)
INTRO (U"Copies the linear prediction coefficients of the selected @LPC "
	"object to a newly created @Matrix object.")
ENTRY (U"Behaviour")
EQUATION (U"%z__%ji_ = %a__%ij_, with 1 \\<_ %i \\<_ %nx and "
	"1 \\<_ j \\<_ %nCoefficients__%i_,")
NORMAL (U"where %z__%ji_ is the matrix element in row %j and column %i and "
	"%a__%ij_ is the %j-th linear prediction coefficient in frame %i.")
MAN_END

MAN_BEGIN (U"LPC: To Formant", U"djmw", 19970123)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Behaviour")
NORMAL (U"For each LPC frame, the zeros of the linear prediction polynomial are extracted. "
	"Zeros that are outside the unit circle are reflected into it. "
	"Next, formant frequencies and bandwidths are calculated from all the roots that have the "
	"imaginary part positive, i.e., that lie in the upper half of the unit circle. "
	"Formant frequencies smaller than 50 Hz or larger than (%nyquistFrequency - 50) are discarded. "
	"The remaining frequencies and bandwidths are sorted "
	"and copied to the Formant frame. Finally, the %gain field of the LPC is copied to the %intensity "
	"field of the Formant frame.")
ENTRY (U"Algorithm")
NORMAL (U"The root finder is Laguerre's method followed by root polishing, see @@Press "
	"et al. (1992)@.")
ENTRY (U"Warning")
LIST_ITEM (U"\\bu The formant values can be very inaccurate if you did not resample the Sound "
	"before the LPC-analysis (consult the @@Source-filter synthesis@ tutorial).")
LIST_ITEM (U"\\bu The results of the root finder may not always be accurate when more than 30 "
	"roots have to be found.")
MAN_END

MAN_BEGIN (U"LPC: To Polynomial (slice)...", U"djmw", 20260104)
INTRO (U"A command that creates a Polynomial object from each selected @LPC object.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"defines the LPC frame whose coefficients will be selected.")
ENTRY (U"Behaviour")
NORMAL (U"The linear prediction coefficients %a__1..%n_ of the LPC frame that is nearest to the"
	" given time will be copied into polynomial coefficients %c__1..%n+1_ as follows:")
EQUATION (U"%c__%i_ = %a__%n\\--%i+1_  for %i=1..%n,")
EQUATION (U"%c__%n+1_ = 1")
MAN_END

MAN_BEGIN (U"LPC: To Spectrum (slice)...", U"djmw", 20071120)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time at which the Spectrum should be calculated.")
TERM (U"##Minimum frequency resolution (Hz)")
DEFINITION (U"successive frequencies in the @Spectrum "
	"will be maximally this distance apart.")
TERM (U"##Bandwidth reduction (Hz)")
DEFINITION (U"formants with small bandwidths show up very well as peaks in the spectrum because the poles "
	"lie close to the contour along which the spectrum is computed (the unit circle in the z-plane). "
	"Peak enhancement can be realized by computing the spectrum in the z-plane along a contour of radius "
	"%r = exp (\\-- %\\pi \\.c %bandwidthReduction / %samplingFrequency). "
	"This technique is also called off-axis spectrum computation. "
	"Negative values evaluate the spectrum on a contour %outside the unit circle and therefore result in a "
	"flattened  spectrum.")
TERM (U"##De-emphasis frequency (Hz)")
DEFINITION (U"Performs de-emphasis when frequency is in the interval (0, @@Nyquist frequency@)")
ENTRY (U"Algorithm")
NORMAL (U"The Spectrum at time %t will be calculated from the %nearest LPC frame according to:")
EQUATION (U"Spectrum (%f) = \\Vr(%gain\\.c%T/%df) / (1 + \\su__%k=1..%numberOfCoefficients_ %a__%k_%z^^\\--%k^),")
NORMAL (U"where %T is the sampling period and %z = exp (\\--2 %\\pi %i %f %T) and %df is the distance in Hz "
	"between two successive components in the Spectrum.")
LIST_ITEM (U"1. Allocate a large enough buffer [1..%nfft] to perform an FFT analysis.")
LIST_ITEM (U"2. Make the first value of the buffer 1 and copy the prediction coefficients #a into "
	"the buffer. This results in buffer values: (1, %a__1_, ..., %a__%numberOfCoefficients_, 0, ..., 0).")
LIST_ITEM (U"3. If ##De-emphasis frequency# is in the interval (0, %nyquistFrequency) then \"multiply\" "
	"the buffer with (1 - %b %z^^\\--1^), where %b = exp (\\-- %\\pi %deEmphasisFrequency %T). "
	"This results in buffer values: (1, %a__1_\\--%b, %a__2_\\--%b\\.c%a__1_, ..., "
	"%a__%numberOfCoefficients_\\--%b\\.c%a__%numberOfCoefficients\\--1_, "
	"\\--%b\\.c%a__%numberOfCoefficients_, 0, ..., 0). Note that the number of values in the buffer that differ from 0 "
	"has increased by one.")
LIST_ITEM (U"4. If ##Bandwidth reduction# is greater than 0 then multiply corresponding values in the buffer by %g^^%i\\--1^ where "
	"%g = exp (2%\\pi %bandwidthReduction %T / %nfft), and %i is the position index in the buffer. "
	"%i runs from 1 to %numberOfCoefficients+1+%t, where %t equals 1 when de-emphasis was performed, "
	"else 0.")
LIST_ITEM (U"5. Calculate the FFT spectrum of the buffer with the coefficients. This results in complex "
	"amplitudes (%a__%j_,%b__%j_), %j=1..%nfft/2+1.")
LIST_ITEM (U"6. Calculate the LPC Spectrum by taking the inverse of the FFT spectrum, i.e., each complex "
	"amplitude becomes (%a__%j_,%b__%j_)^^\\--1^ = (%a__%j_,\\--%b__%j_) / (%a__%j_^2 + %b__%j_^2)")
LIST_ITEM (U"7. Multiply all values with the scale factor \\Vr(%gain\\.c%T/%df).")
MAN_END

MAN_BEGIN (U"LPC: To Spectrogram...", U"djmw", 20040407)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Settings")
TERM (U"##Minimum frequency resolution (Hz)")
DEFINITION (U"successive frequencies in the Spectrum will be maximally this distance apart")
TERM (U"##Bandwidth reduction (Hz)")
DEFINITION (U"formants with small bandwidths show up very well as darker regions in the spectrogram "
	"because the poles lie close to the contour along which a spectrum is computed (the unit circle "
	"in the z-plane). "
	"Peak enhancement can be realized by computing a spectrum in the z-plane along a contour of radius "
	"%r = exp (\\-- %\\pi \\.c %bandwidthReduction / %samplingFrequency).")
TERM (U"##De-emphasis frequency (Hz)")
DEFINITION (U"Performs de-emphasis when value is in the interval (0, @@Nyquist frequency@)")
ENTRY (U"Algorithm")
NORMAL (U"For each LPC frame the corresponding Spectrum will be calculated according to the algorithm "
	"explained in @@LPC: To Spectrum (slice)...@. "
	"For each frequency the power, i.e., the square of the complex values, will be stored in the "
	"corresponding area in the Spectrogram.")
MAN_END

MAN_BEGIN (U"LPC: To VocalTract (slice)...", U"djmw", 20050615)
INTRO (U"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time of the nearest frame, in seconds.")
TERM (U"##Length (m)")
DEFINITION (U"the length of the vocal tract, in metres.")
/*
TERM (U"##Compute length according to Wakita")
DEFINITION (U"the length of the vocal tract is calculated according "
	"to the algorithm as described in @@Wakita (1977)@.")
ENTRY (U"Behaviour")
NORMAL (U"A new @VocalTract area function is calculated from the prediction coefficients in the frame. ")
ENTRY (U"Warning")
NORMAL (U"If ##Compute length according to Wakita# is on, the optimal length is searched for in the range from 0.1 m to "
	"0.25 m. This length calculation is extremely sensitive to the number of and the positions of the formants "
	"with respect to the @@Nyquist frequency@. For example, there is a large difference "
	"between the vocal tract length estimates if the highest formant is just below or just above the "
	"Nyquist frequency. "
	"The algorithm is not very reliable in vocal tract length estimation and we do not recommend using it. " )*/
MAN_END

MAN_BEGIN (U"LPC & Sound: Filter...", U"djmw", 20040407)
INTRO (U"A command that creates a new Sound object from one @Sound and one @LPC "
	"object which have been selected together.")
ENTRY (U"Settings")
TERM (U"##Use LPC gain")
DEFINITION (U"Determines whether the gain from the LPC is used in the synthesis.")
ENTRY (U"Behaviour")
NORMAL (U"Filters the selected Sound by the selected LPC-filter.")
NORMAL (U"When the LPC-gain is used the samples in the new Sound will be "
	"multiplied with the square root of the corresponding LPC-gain value.")
NORMAL (U"In #Z-domain notation: #O(%z) = #H(%z) \\.c #E(%z), where "
	"#E(%z) is the selected filter input Sound, #H(%z) the selected LPC filter, "
	"and, #O(%z) the filter output (the new Sound that will appear in the List of objects).")
MAN_END

MAN_BEGIN (U"LPC & Sound: Filter with filter at time...", U"djmw", 20101009)
INTRO (U"Filters the selected @Sound with a static filter that is formed by the filter coefficients "
	"from only one @LPC frame.")
ENTRY (U"Settings")
TERM (U"##Channel")
DEFINITION (U"determines the sound channel to be filtered.")
TERM (U"##Use filter at time (s)")
DEFINITION (U"determines which LPC frame will be chosen to filter the sound. ")
MAN_END

MAN_BEGIN (U"LPC & Sound: Filter (inverse)", U"djmw", 19970126)
INTRO (U"A command that creates a new Sound object from one @Sound and one @LPC "
	"object which have been selected together.")
ENTRY (U"Behaviour")
NORMAL (U"Given a filter (the selected LPC) and its output (the selected Sound), "
	"its input is reconstructed (the new Sound that will appear in the List of objects).")
NORMAL (U"In Z-domain notation: #E(%z) = #O(%z) / #H(%z), where "
	"#O(%z) is the filter output Sound, #H(%z) the LPC filter, and, #E(%z) the filter "
	"input Sound. (Selecting this newly generated Sound and the LPC, choosing the command "
	"##Filter...# generates a Sound that is identical to the Sound that originated "
	"the LPC.)")
MAN_END

MAN_BEGIN (U"LPC & Sound: Filter (inverse) with filter at time...", U"djmw", 20101009)
INTRO (U"%%Inverse% filters the selected @Sound with a static inverse filter that is formed by the filter coefficients "
	"from only one @LPC frame.")
ENTRY (U"Settings")
TERM (U"##Channel")
DEFINITION (U"determines the sound channel to be filtered.")
TERM (U"##Use filter at time (s)")
DEFINITION (U"determines which LPC frame will be chosen to inverse filter the sound. ")
MAN_END

MAN_BEGIN (U"MFCC", U"djmw", 20160225)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type MFCC represents mel frequency cepstral coefficients "
	"as a function of time. The coefficients are represented in a number of frames centred at equally spaced times. "
	"at constant sampling period.")
ENTRY (U"MFCC commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Sound: To MFCC...@")
LIST_ITEM (U"\\bu @@MelSpectrogram: To MFCC...@")
MAN_END

MAN_BEGIN (U"MFCC: To MelFilter...", U"djmw", 20141023)
INTRO (U"A #deprecated command replaced by @@MFCC: To MelSpectrogram...@.")
ENTRY (U"Settings")
TERM (U"##From coefficient#, ##To coefficient#")
DEFINITION (U"the range of coefficients that will be used in the reconstruction.")
ENTRY (U"Details")
NORMAL (U"The output of the triangular filters in a mel filter bank will be "
	"reconstructed by applying the inverse cosine transform:")
EQUATION (U"%P__%j_ = 2/N (%c__0_/2 + \\Si__%k=1_^^%N-1^ %c__%k_ cos (\\pi%k(%j-0.5)/%N))),")
NORMAL (U"where %N represents the number of filters, %j runs from 1 to %N, and coefficients %c__%k_ with %k less than "
	"%%fromCoefficient% and %k larger than %%toCoefficient% take zero values in the evaluation.")
MAN_END

MAN_BEGIN (U"MFCC: To MelSpectrogram...", U"djmw", 20141023)
INTRO (U"A command to (re)construct a @MelSpectrogram object from the selected @MFCC object.")
ENTRY (U"Settings")
TERM (U"##From coefficient#, ##To coefficient#")
DEFINITION (U"the range of coefficients that will be used in the reconstruction.")
TERM (U"##Include constant term")
DEFINITION (U"selects whether or not to include the %c__0_ coefficient in the reconstruction. "
	"As can be seen from the formula below, the contribution of the %c__0_ term is equal for each filter.")
ENTRY (U"Details")
NORMAL (U"The values %P__%j_ in each frame of the MelSpectrogram will be constructed by "
	"applying the inverse Discrete Cosine Transform to the corresponding frame of the MFCC object:")
EQUATION (U"%P__%j_ = 2/N (%c__0_/2 + \\Si__%k=1_^^%N-1^ %c__%k_ cos (\\pi%k(%j-0.5)/%N))),")
NORMAL (U"where %N represents the number of filters that were used to get the MFCC object, %j runs from 1 to %N, "
	"and coefficients %c__%k_ with %k less than "
	"%%fromCoefficient% and %k larger than %%toCoefficient% take zero values in the evaluation.")
MAN_END

MAN_BEGIN (U"Sound: To PowerCepstrogram...", U"djmw", 20200403)
INTRO (U"A command that creates a @@PowerCepstrogram@ from every selected @@Sound@.")
ENTRY (U"Settings")
TERM (U"##Pitch floor (Hz)")
DEFINITION (U"determines the effective length of the analysis window as three periods of this pitch, "
	"e.g. if the pitch floor is 60 Hz, the analysis window will be 3/60 = 0.05 seconds long.")
TERM (U"##Time step (s)")
DEFINITION (U"defines the distance between the centres of subsequent frames. This determines the number of frames "
	"in the resulting PowerCepstrogram.")
TERM (U"##Maximum frequency (Hz)")
DEFINITION (U"the maximum frequency subject to analysis.")
TERM (U"##Pre-emphasis from (Hz)")
ENTRY (U"Algorithm")
NORMAL (U"The sound will first be resampled to twice the value of the %%Maximum frequency%, with "
	"the algorithm described at @@Sound: Resample...@. After this, pre-emphasis is applied with the "
	"algorithm described at @@Sound: Pre-emphasize (in-place)...@. For each analysis window a Gaussian "
	"window is applied and the ##Spectrum# is calculated. "
	"The Spectrum is then transformed to a ##PowerCepstrum# with the procedure described at @@Spectrum: To PowerCepstrum@. "
	"Finally, the values from the PowerCepstrum are stored in the vertical slice of the PowerCepstrogram.")
MAN_END

MAN_BEGIN (U"Sound: To Formant (robust)...", U"djmw", 20111027)
INTRO (U"A command that creates a @@Formant@ object from every selected @@Sound@. ")
ENTRY (U"Settings")
NORMAL (U"The settings for ##Time step (s)#, ##Maximum number of formants#, ##Formant ceiling (Hz), "
	"##Window length (s)# and ##Pre-emphasis from (Hz)# are as in @@Sound: To Formant (burg)...@. "
	" The following settings determine aspects of the iterative formant frequency refinement.")
TERM (U"%%Number of std. dev.%,")
DEFINITION (U"determines the number of standard deviations from where selective weighing of samples starts. ")
TERM (U"%%Maximum number of iterations%,")
DEFINITION (U"determines the maximum number of iterations allowed in the refinement step.")
TERM (U"%%Tolerance%,")
DEFINITION (U"detemines another stop ctriterion for the refinement step. If the relative change in variance "
	"between successive iterations is less then this value, iteration stops. Iteration stops whenever "
	"one of the two defined stop criteria is reached.")
ENTRY (U"Algorithm")
NORMAL (U"First the sound is downsampled to twice the maximum formant frequency. "
	"Next, the LPC coefficients are determined by the autocorrelation method. "
	"Finally, in an iterative procedure as described by @@Lee (1988)@, "
	"the formant frequencies and bandwidths are refined by selective weighting of samples values.")
MAN_END

MAN_BEGIN (U"Sound: LPC analysis", U"djmw", 19970126)
INTRO (U"You can perform this analysis by selecting one or more @Sound objects and "
	"choosing the appropriate command to generate an @LPC.")
NORMAL (U"The acronym LPC stands for Linear Predictive Coding.")
NORMAL (U"In the LPC analysis one tries to predict %x__%n_ on the basis of the %p previous samples,")
EQUATION (U"%x\\'p__%n_ = \\su %a__%k_ %x__%%n-k%_")
NORMAL (U"then {%a__1_, %a__2_, ..., %a__%p_} can be chosen to minimize the prediction power %%Q__p_% where")
EQUATION (U"%%Q__p_% = E [ |%x__%n_ - %x\\'p__%n_|^2].")
NORMAL (U"Several different algorithms exist for minimizing %%Q__p_%:")
LIST_ITEM (U"\\bu @@Sound: To LPC (autocorrelation)...|To LPC (autocorrelation)...@")
LIST_ITEM (U"\\bu @@Sound: To LPC (covariance)...|To LPC (covariance)...@")
LIST_ITEM (U"\\bu @@Sound: To LPC (burg)...|To LPC (burg)...@")
LIST_ITEM (U"\\bu @@Sound: To LPC (marple)...|To LPC (marple)...@")
MAN_END

#define Sound_to_LPC_COMMON_HELP(method) \
INTRO (U"With this command you create a new @LPC from every selected @Sound, " \
	"using " method " method.") \
ENTRY (U"Warning") \
NORMAL (U"You are advised not to use this command for formant analysis. " \
	"For formant analysis, instead use @@Sound: To Formant (burg)...@, " \
	"which also works via LPC (linear predictive coding). This is because " \
	"##Sound: To Formant (burg)...# lets you specify a maximum frequency, " \
	"whereas the ##To LPC# commands automatically use the @@Nyquist frequency@ " \
	"as their maximum frequency. If you do use one of the ##To LPC# commands " \
	"for formant analysis, you may therefore want to downsample the sound first. " \
	"For instance, if you want five formants below 5500 Hz but your Sound has a sampling frequency " \
	"of 44100 Hz, you have to downsample the sound to 11000 Hz with the @@Sound: Resample...@ command. " \
	"After that, you can use the ##To LPC# commands, with a prediction order of 10 or 11.") \
ENTRY (U"Settings") \
TERM (U"##Prediction order#") \
DEFINITION (U"the number of linear prediction coefficients, also called the %%number of poles%. " \
	"Choose this number at least twice as large as the number of spectral peaks that you want " \
	"to detect.") \
TERM (U"##Analysis window duration (s)") \
DEFINITION (U"the effective duration of each analysis frame, in seconds.") \
TERM (U"##Time step (s)") \
DEFINITION (U"the time step between two consecutive analysis frames.") \
TERM (U"##Pre-emphasis frequency (Hz)") \
DEFINITION (U"a +6dB / octave filtering will be applied above this frequency. " \
	"A pre-emphasis frequency of 48.47 Hz for a signal with a sampling frequency of 10 kHz " \
	"approximately corresponds to a value of %a = 0.97 for the filter %y__%n_ = %x__%n_ - %a \\.c %x__%n-1_. " \
	"The relation between %a and the pre-emphasis frequency is: " \
	"%a = exp (\\--2\\.c%\\pi\\.c%preemphasisFrequency/%samplingFrequency). " \
	"If you do not want pre-emphasis, choose a frequency greater than the @@Nyquist frequency@.")

MAN_BEGIN (U"Sound: To LPC (autocorrelation)...", U"David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("the autocorrelation")
ENTRY (U"Algorithm")
NORMAL (U"The autocorrelation algorithm is decribed in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN (U"Sound: To LPC (covariance)...", U"David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("the covariance")
ENTRY (U"Algorithm")
NORMAL (U"The covariance algorithm is decribed in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN (U"Sound: To LPC (marple)...", U"djmw", 19970126)
Sound_to_LPC_COMMON_HELP ("Marple's")
TERM (U"##Tolerance 1")
DEFINITION (U"stop the iteration when %E(%m) / %E(0) < %%Tolerance 1%, where %E(%m) is the "
	"prediction error for order %m.")
TERM (U"##Tolerance 2")
DEFINITION (U"stop the iteration when (%E(%m) - %E(%m-1)) / %E(%m-1) < %%Tolerance 2.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm is described in @@Marple (1980)@.")
MAN_END

MAN_BEGIN (U"Sound: To LPC (burg)...", U"David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("Burg's")
ENTRY (U"Algorithm")
NORMAL (U"Burg's algorithm is described in @@Anderson (1978)@")
MAN_END

MAN_BEGIN (U"Sound: To MFCC...", U"djmw", 20141022)
INTRO (U"A command that creates a @MFCC object from every selected @Sound "
	"object.")
NORMAL (U"The analysis proceeds in two steps:")
LIST_ITEM (U"1.  We perform a spectrum analysis on a mel frequency scale "
	"(see @@Sound: To MelSpectrogram...@ for details).")
LIST_ITEM (U"2.  We convert the melspectrogram values to mel frequency cepstral "
	"coefficients (see @@MelSpectrogram: To MFCC...@ for details).")
MAN_END

MAN_BEGIN (U"Spectrum: To PowerCepstrum", U"djmw", 20190908)
INTRO (U"A command to create a @PowerCepstrum from every selected @Spectrum.")
ENTRY (U"Mathematical procedure")
NORMAL (U"The spectrum %X(%f) is transformed to a new spectrum %%X%\\'p(%f), where "
	"Re(%X\\'p(%f)) = ln (Re(%X(%f))^^2^ + Im(%X(%f))^^2^) and Im(%X\\'p(%f)) = 0.")
NORMAL (U"The new spectrum %X\\'p(%f) is then transformed to a Sound %x(%t) by means of an inverse Fourier "
	"transform as is described in @@Spectrum: To Sound@. The %%squares% of the transformed "
	"values, %x(%t)^^2^, are stored in the ##PowerCepstrum#.")
MAN_END

MAN_BEGIN (U"VocalTractTier", U"djmw", 20120423)
INTRO (U"One of the @@types of objects@ in Praat. A VocalTractTier objects contains a number of (%%time%, %%VocalTract%) "
	"points, where a @@VocalTract@ represents the area function of the vocal tract expressed as m^^2^, running from the glottis to the lips.")
MAN_END

MAN_BEGIN (U"theil regression", U"djmw", 20190909)
NORMAL (U"a robust linear regression method, first proposed by @@Theil (1950)@. The slope of the regression line is estimated as "
	"the median of all pairwise slopes between each pair of points in the data set. Because this number of pairs increases quadratically "
	"with the number of data points, we have implemented a somewhat less computationally intensive procedure, the %%incomplete% "
	"theil regression. In the incomplete method we first split the data set of %N data points (%x__%i_, %y__%i_), %i = 1..%N, "
	"in two equal sets of size %N/2 and then calculate %N/2 slopes as ")
EQUATION (U"%m__%i_ = (%y__%N/2+%i_ - %y__%i_) / (%x__%N/2+%i_ - %x__%i_), for %i = 1..%N/2.")
NORMAL (U"The regression slope %m is calculated as the median of these %N/2 values %m__%i_.")
NORMAL (U"Given the slope %m, the offset %b is calculated as the median of the %N values %b__%i_= %y__%i_ - %m\\.c%x__%i_.")
NORMAL (U"The theil regression has a breakdown point of 29.3\\% , which means that it can tolerate arbitrary corruption of up to "
	"29.3\\%  of the input data-points without degradation of its accuracy")
MAN_END

MAN_BEGIN (U"Ammar et al. (2001)", U"djmw", 20200416)
NORMAL (U"G.S. Ammar, D. Calvetti, W.B. Gragg, L. Reichel (2001): \"Polynomial zero finders based on Szegö polynomials\", "
	"%%Journal of Computational and Applied Mathematics% #127: 1\\-–16.")
MAN_END

MAN_BEGIN (U"Anderson (1978)", U"djmw", 20030701)
NORMAL (U"N. Anderson (1978): \"On the calculation of filter coefficients for "
	"maximum entropy spectral analysis.\" In Childers: %%Modern Spectrum Analysis%, "
	"IEEE Press: 252\\--255.")
MAN_END

MAN_BEGIN (U"Deng et al. (2006)", U"djmw", 20210310)
NORMAL (U"L. Deng, X. Cui, R. Pruvenok, J. Huang, S. Momen, Y. Chen & A. Alwan (2006): \"A database of "
	"vocal tract resonance trajectories for research in speech processing\", %%Proceedings of the IEEE International Conference on Acoustics, Speech, and Signal Processing (ICASSP)%, Toulouse, France, May 2006.")
MAN_END

MAN_BEGIN (U"Fleisher et al. (2015)", U"djmw", 20191008)
NORMAL (U"M. Fleisher, S. Pinkert, W. Mattheus, A. Mainka & D. Mürbe (2015): \"Formant frequencies and bandwidths of the vocal "
	"transfer function are affected by the mechanical impedance of the vocal tract wall.\", %%Biomech Model Mechanobiol% #14: 719\\--733.")
MAN_END

MAN_BEGIN (U"Hawks & Miller (1995)", U"djmw", 20191008)
NORMAL (U"J. Hawks &  J. Miller (1995): \"A formant bandwidth estimation procedure for vowel synthesis.\", "
	"%%Journal of the Acoustical Society of America% #97: 1343\\--1344.")
MAN_END

MAN_BEGIN (U"Hillenbrand et al. (1994)", U"djmw", 20121017)
NORMAL (U"J. Hillenbrand, R.A. Cleveland & R.L. Erickson (1994): \"Acoustic correlates of breathy vocal quality\", "
	"%%Journal of speech and hearing research% #37: 769\\--778.")
MAN_END

MAN_BEGIN (U"Hillenbrand & Houde (1996)", U"djmw", 20121203)
NORMAL (U"J. Hillenbrand & R.A. Houde (1996): \"Acoustic correlates of breathy vocal quality: Dysphonic voices and continuous "
	"speech\", %%Journal of speech and hearing research% #39: 311\\--321.")
MAN_END

MAN_BEGIN (U"Jacquelin (2009)", U"djmw", 20210126)
NORMAL (U"J. Jacquelin (2009): \"Régressions et équations intégrales\", https://fr.scribd.com/doc/14674814/Regressions-et-equations-integrales: 1\\--84.")
MAN_END

MAN_BEGIN (U"Lee (1988)", U"djmw", 20111027)
NORMAL (U"C.-H. Lee (1988): \"On robust linear prediction of speech.\", %%IEEE Trans. on ASSP% #36: 642\\--649.")
MAN_END

MAN_BEGIN (U"Markel & Gray (1976)", U"djmw", 19980114)
NORMAL (U"J.D. Markel & A.H. Gray, Jr. (1976): %%Linear prediction of speech.% "
	"Springer Verlag, Berlin.")
MAN_END

MAN_BEGIN (U"Marple (1980)", U"djmw", 19980114)
NORMAL (U"L. Marple (1980): \"A new autoregressive spectrum analysis algorithm.\" "
	"%%IEEE Trans. on ASSP% #28, 441\\--454.")
MAN_END

MAN_BEGIN (U"Rothweiler (1999)", U"djmw", 20160507)
NORMAL (U"J. Rothweiler (1999): \"On polynomial reduction in the computation of LSP frequencies.\" "
	"%%IEEE Trans. on ASSP% #7, 592\\--594.")
MAN_END

MAN_BEGIN (U"Theil (1950)", U"djmw", 20121118)
NORMAL (U"H. Theil (1950): \"A rank-invariant method of linear and polynomial regression analysis\", "
	"%%Proceedings of Koninklijke Nederlandse Akademie van Wetenschappen% ##A.53#: 1397\\--1412.")
MAN_END

MAN_BEGIN (U"Wakita (1977)", U"djmw", 19980114)
NORMAL (U"H. Wakita (1977): \"Normalization of vowels by vocal-tract "
	"length and its application to vowel identification.\" %%IEEE Trans. on ASSP% "
	"#25: 183\\--192.")
MAN_END

MAN_BEGIN (U"Weenink (2015)", U"djmw", 20200514)
NORMAL (U"D. Weenink (2015): \"Improved formant frequency measurements of short segments\", "
	"%%Proceedings of the 18th International Congress of Phonetic Sciences%, Brighton. ")
MAN_END

}

/* End of file manual_LPC.cpp */
