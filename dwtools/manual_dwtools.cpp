/* manual_dwtools.cpp
 *
 * Copyright (C) 1993-2026 David Weenink, 2025,2026 Paul Boersma
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

#include "ManPagesM.h"
#include "Sound_extensions.h"
#include "TableOfReal_extensions.h"
#include "TableOfReal_and_Discriminant.h"
#include "Table_extensions.h"
#include "Configuration.h"
#include "Discriminant.h"
#include "../sensors/Electroglottogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_and_Spectrum_dft.h"


static autoTableOfReal getStandardizedLogFrequencyPolsData (bool includeLevels) {
	autoTableOfReal me = TableOfReal_create_pols1973 (includeLevels);
	for (integer i = 1; i <= my numberOfRows; i ++)
		for (integer j = 1; j <= 3; j++)
			my data [i] [j] = log10 (my data [i] [j]);
	TableOfReal_standardizeColumns (me.get());
	TableOfReal_setColumnLabel (me.get(), 1, U"standardized log (%F__1_)");
	TableOfReal_setColumnLabel (me.get(), 2, U"standardized log (%F__2_)");
	TableOfReal_setColumnLabel (me.get(), 3, U"standardized log (%F__3_)");
	if (includeLevels) {
		TableOfReal_setColumnLabel (me.get(), 4, U"standardized %L__1_");
		TableOfReal_setColumnLabel (me.get(), 5, U"standardized %L__1_");
		TableOfReal_setColumnLabel (me.get(), 6, U"standardized %L__3_");
	}
	return me;
}

static void drawPolsF1F2_log (Graphics g) {
	autoTableOfReal me = getStandardizedLogFrequencyPolsData (false);
	Graphics_setWindow (g, -2.9, 2.9, -2.9, 2.9);
	TableOfReal_drawScatterPlot (me.get(), g, 1, 2, 0, 0, -2.9, 2.9, -2.9, 2.9, 10, true, U"+", true);
}

static void drawPolsF1F2ConcentrationEllipses (Graphics g) {
	autoTableOfReal me = getStandardizedLogFrequencyPolsData (false);
	autoDiscriminant d = TableOfReal_to_Discriminant (me.get());
	Discriminant_drawConcentrationEllipses (d.get(), g, 1, false, nullptr, false, 1, 2, -2.9, 2.9, -2.9, 2.9, 12, true);
}

static void drawPolsDiscriminantConfiguration (Graphics g) {
	autoTableOfReal me = getStandardizedLogFrequencyPolsData (false);
	autoDiscriminant d = TableOfReal_to_Discriminant (me.get());
	autoConfiguration c = Discriminant_TableOfReal_to_Configuration (d.get(), me.get(), 2);
	Configuration_draw (c.get(), g, 1, 2, -2.9, 2.9, -2.9, 2.9, 0, true, U"", true);
}

static void drawBoxPlot (Graphics g) {
	const double q25 = 25, q50 = 50, q75 = 60, mean = 45;
	const double hspread = q75 - q25, r = 0.05, w = 0.2;
	const double lowerInnerFence = q25 - 1.5 * hspread;
	const double upperInnerFence = q75 + 1.5 * hspread;
	const double upperOuterFence = q75 + 3.0 * hspread;
	const double lowerWhisker = lowerInnerFence + 0.1 * hspread;
	const double upperWhisker = upperInnerFence - 0.5 * hspread;
	const double ymin = lowerWhisker - 0.1 * hspread, ymax = q75 + 4 * hspread;
	const double x = 0, dx = 0.01, xar = x + 0.7, xtl = xar + dx;
	const double xal1 = x + r + dx, xal2 = x + w + r;

	Graphics_setWindow (g, -1, 2, ymin, ymax);
	Graphics_setInner (g);
    Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);

	Graphics_line (g, x - r, lowerWhisker, x + r, lowerWhisker);
	Graphics_line (g, x, lowerWhisker, x, q25);
	Graphics_line (g, x - w, q25, x + w, q25);
	Graphics_line (g, x - w, q50, x + w, q50);
	Graphics_line (g, x - w, q75, x + w, q75);
	Graphics_line (g, x - w, q25, x - w, q75);
	Graphics_line (g, x + w, q25, x + w, q75);
	Graphics_line (g, x, q75, x, upperWhisker);
	Graphics_line (g, x - r, upperWhisker, x + r, upperWhisker);

	double y = q75 + 2.5 * hspread;
	Graphics_text (g, x, y, U"*");
	Graphics_arrow (g, xar, y, xal1, y);
	Graphics_text (g, xtl, y, U"outlier > %%upperInnerFence%");
	y = q75 + 3.5 * hspread;
	Graphics_text (g, x, y, U"o");
	Graphics_arrow (g, xar, y, xal1, y);
	Graphics_text (g, xtl, y, U"outlier > %%upperOuterFence%");
	y = upperOuterFence;
	Graphics_setLineType (g, Graphics_DOTTED);
	Graphics_line (g, -xtl, y, xtl, y);
	Graphics_text (g, xtl, y, U"%%upperOuterFence%");
	y = upperInnerFence;
	Graphics_line (g, -xtl, y, xtl, y);
	Graphics_text (g, xtl, y, U"%%upperInnerFence%");
	Graphics_line (g, x - w, mean, x + w, mean);
	Graphics_setLineType (g, Graphics_DRAWN);
	y = upperWhisker;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, U"%%upperWhisker%");
	y = lowerWhisker;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, U"%%lowerWhisker%");

	y = q75;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, U"%%q75%");
	y = q25;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, U"%%q25%");
	y = q50;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, U"%%q50%");
	y = mean;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, U"%%mean%");
	Graphics_unsetInner (g);
}

static void drawPartionedMatrix (Graphics g) {
	const double min = 0.0, max = 10.0;
	Graphics_setWindow (g, min, max, min, max);
	double x1 = 0.0;
	double x2 = max;
	double y1 = 7.0;
	double y2 = 7.0;
	Graphics_setLineType (g, Graphics_DOTTED);
	Graphics_line (g, x1, y1, x2, y2);
	x1 = x2 = 3.0;
	y1 = 0.0;
	y2 = max;
	Graphics_line (g, x1, y1, x2, y2);
	Graphics_setLineType (g, Graphics_DRAWN);
	x1 = 1.5;
	y1 = 7.0 + 3.0 / 2.0;
	Graphics_setFontSize (g, 14.0);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	Graphics_text (g, x1, y1, U"##S__yy_#");
	x1 = 3.0 + 7.0 / 2.0;
	Graphics_text (g, x1, y1, U"##S__yx_#");
	y1 = 7.0 / 2.0;
	Graphics_text (g, x1, y1, U"##S__xx_#");
	x1 = 1.5;
	Graphics_text (g, x1, y1, U"##S__xy_#");
}

static void Electroglottogram_drawStylized (Graphics g) {
	Electroglottogram_drawStylized (g, true, false);
}

static void Electroglottogram_drawStylizedLevels (Graphics g) {
	Electroglottogram_drawStylized (g, false, true);
}

static void drawSpectra (Graphics g) {
	static Spectrum dft = nullptr;
	autoSound sound = Sound_createAsPureTone (1, 0.0, 3.9799, 10000.0, 3333.0, 1.0, 0.0, 0.0);
	if (! dft)
		dft = Sound_to_Spectrum (sound.get(), false).releaseToAmbiguousOwner();
	autoSpectrum fft = Sound_to_Spectrum (sound.get(), true);
	autoSpectrum ups = Sound_to_Spectrum_resampled (sound.get(), 30);
	const double fmin = 0.0, fmax = 5000.0, dbmin = -30.0, dbmax = 110.0;
	const double fontSize = Graphics_inqFontSize (g);
	Graphics_setFontSize (g, 10.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setColour (g, Melder_SILVER);
	Spectrum_draw (fft.get(), g, fmin, fmax, dbmin, dbmax, true);
	Graphics_setColour (g, Melder_BLACK);
	Spectrum_draw (dft, g, fmin, fmax, dbmin, dbmax, false);
	Graphics_setColour (g, Melder_RED);
	Spectrum_draw (ups.get(), g, fmin, fmax, dbmin, dbmax, false);
	Graphics_setColour (g, Melder_BLACK);
	Graphics_setFontSize (g, fontSize);
}

void manual_dwtools_init (ManPages me);
void manual_dwtools_init (ManPages me) {

MAN_BEGIN (U"AffineTransform", U"djmw", 20010927)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An affine transform is a combination of a linear transformation #%A "
	"and a translation #%t that transforms a vector #%x to a new vector #%y "
	"in the following way:")
EQUATION (U"#%y = #%A x + #%t")
MAN_END

MAN_BEGIN (U"AffineTransform: Invert", U"djmw", 20011008)
INTRO (U"Get the inverse of the selected @AffineTransform object.")
NORMAL (U"The inverse from")
EQUATION (U"#%y = #%A x + #%t")
NORMAL (U"is:")
EQUATION (U"#%x = #%A^^-1^ - #%A^^-1^#%t.")
MAN_END

MAN_BEGIN (U"band filtering in the frequency domain", U"djmw", 20010404)
INTRO (U"We describe how band filtering in the frequency domain is performed.")
NORMAL (U"We start with a @Sound and end with a filter bank representation of "
	"this sound. We assume a standard analysis context: a sound divided into "
	"frames according to a certain %%window length% and %%time step%. We will "
	"simulate a filterbank with %N filters.")
NORMAL (U"The algorithm for each sound frame proceeds in the following way:")
LIST_ITEM (U"1. Apply a Gaussian window to the sound frame.")
LIST_ITEM (U"2. Convert the windowed frame into a @Spectrum object.")
LIST_ITEM (U"3. Convert the spectral amplitudes to %energy values by squaring "
	"the real and imaginary parts and multiplying by %df, the frequency "
	"distance between two successive frequency points in the spectrum. "
	"Since the Spectrum object only contains positive frequencies, "
	"we have to multiply all energy values, except the first and the last "
	"frequency, by another factor of 2 to compensate for negative frequencies.")
LIST_ITEM (U"4. For each of the %N filters in the filter bank: determine the "
	"inner product of its filter function with the energies as determined in "
	"the previous step. The result of each inner product is the energy in the "
	"corresponding filter.")
LIST_ITEM (U"5. Convert the energies in each filter to power by dividing by "
	"the %%window length%.")
LIST_ITEM (U"6. Correct the power, due to the windowing of the frame, by dividing "
	"by the integral of the %squared windowing function.")
LIST_ITEM (U"7. Convert all power values to %dB's according to 10 * log10 "
	"(%power / 4 10^^-10^).")
MAN_END

MAN_BEGIN (U"Bonferroni correction", U"djmw", 20011107)
NORMAL (U"In general, if we have %k independent significance tests "
	"at the %\\al level, the probability %p that we will get no significant "
	"differences in all these tests is simply the product of the individual "
	"probabilities: (1 - %\\al)^^%k^. "
	"For example, with %\\al = 0.05 and %k = 10 we get %p = 0.95^^10^ = 0.60. "
	"This means, however, we now have a 40\\%  chance that one of these 10 "
	"tests will turn out significant, despite each individual test only being "
	"at the 5\\%  level. "
	"In order to guarantee that the overall significance test is still at the "
	"%\\al level, we have to adapt the significance level %\\al\\'p of the "
	"individual test. ")
NORMAL (U"This results in the following relation between the overall and the "
	"individual significance level:")
EQUATION (U"(1 - %\\al\\'p)^^%k%^ = 1 - %\\al.")
NORMAL (U"This equation can easily be solved for %\\al\\'p:")
EQUATION (U"%\\al\\'p = 1 - (1-%\\al)^^1/%k^,")
NORMAL (U"which for small %\\al reduces to:")
EQUATION (U"%\\al\\'p = %\\al / %k")
NORMAL (U"This is a very simple recipe: If you want an overall significance "
	"level %\\al and you perform %k individual tests, simply divide %\\al "
	"by %k to obtain the significance level for the individual tests.")
MAN_END

MAN_BEGIN (U"box plot", U"djmw", 20111010)
INTRO (U"A box plot provides a simple graphical summary of data. These plots "
	"originate from the work of @@Tukey (1977)@.")
ENTRY (U"Definitions")
NORMAL (U"The following figure shows an annotated box plot.")
PICTURE (5.0, 5.0, drawBoxPlot)
NORMAL (U"To understand the box plot we need the following definitions:")
LIST_ITEM (U"%%q25% = lower quartile, 25\\%  of the data lie below this value")
LIST_ITEM (U"%%q50% = median, 50\\%  of the data lie below this value")
LIST_ITEM (U"%%q75% = upper quartile, 25\\%  of the data lie above this value")
NORMAL (U"The following definitions all depend on these quantiles:")
LIST_ITEM (U"%%hspread% = |%%q75% \\-- %%q25%| (50\\%  interval)")
LIST_ITEM (U"%%lowerOuterFence% = %%q25% \\-- 3.0 * %%hspread% (not in figure)")
LIST_ITEM (U"%%lowerInnerFence% = %%q25% \\-- 1.5 * %%hspread% (not in figure)")
LIST_ITEM (U"%%upperInnerFence% = %%q75% + 1.5 * %%hspread%")
LIST_ITEM (U"%%upperOuterFence% = %%q75% + 3.0 * %%hspread%")
LIST_ITEM (U"%%lowerWhisker% = smallest data value larger than %%lowerInnerFence%")
LIST_ITEM (U"%%upperWhisker% = largest data value smaller than %%upperInnerFence%")
NORMAL (U"The box plot is a summary of the data in which:")
LIST_ITEM (U"\\bu the horizontal lines of the rectangle correspond to "
	" %%q25%, %%q50% and %%q75%, respectively.")
LIST_ITEM (U"\\bu the dotted line corresponds to the mean.")
LIST_ITEM (U"\\bu the outliers outside the %%outerFences% are drawn with an 'o'.")
LIST_ITEM (U"\\bu the outliers in the intervals (%%lowerOuterFence%, %%lowerInnerFence%) "
	"and (%%upperInnerFence%, %%upperOuterFence%) are drawn with an '*'.")
LIST_ITEM (U"\\bu the whisker lines outside the rectangle connect %%q25% with %%lowerWhisker%, and, "
	"%%q75% with %%upperWhisker%, respectively. With no outliers present, the "
	"whiskers mark minimum and/or maximum of the data.")
MAN_END


MAN_BEGIN (U"BarkFilter", U"djmw", 20141023)
INTRO (U"A #deprecated @@types of objects|type of object@ in Praat. It has been replaced by @@BarkSpectrogram@.")
NORMAL (U"An object of type BarkFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%z, %t), expressed "
	"in dB's as 10*log10(power/4e-10)). In the now preferred BarkSpectrogram the power is represented instead of its dB value."
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %z__%j_ (on a Bark scale).")
ENTRY (U"Inside a BarkFilter")
NORMAL (U"With @Inspect you will see that this type contains the same attributes a @Matrix object.")
MAN_END


MAN_BEGIN (U"BarkSpectrogram", U"djmw", 20141023)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type BarkSpectrogram represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%z, %t). "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %z__%j_ (on a Bark scale).")
NORMAL (U" The bark to hertz transformation is defined as:")
EQUATION (U"hertz = 650.0 * sinh (bark / 7.0),")
NORMAL (U"while its inverse is defined as:")
EQUATION (U"bark = 7.0 * log (hertz/650 + sqrt (1 + (hertz/650)^^2^).")
ENTRY (U"Inside a BarkSpectrogram")
NORMAL (U"With @Inspect you will see that this type contains the same attributes a @Matrix object.")
MAN_END

MAN_BEGIN (U"BarkSpectrogram: Draw Sekey-Hanson auditory filters...", U"djmw", 20141023)
INTRO (U"A command to draw the auditory filters defined in @@Sekey & Hanson (1984)@.")
MAN_END

MAN_BEGIN (U"BarkSpectrogram: Paint image...", U"djmw", 20141023)
INTRO (U"A command to draw the selected @BarkSpectrogram into the @@Picture window@ in shades of grey.")
MAN_END

MAN_BEGIN (U"biharmonic spline interpolation", U"djmw", 20170915)
INTRO (U"A biharmonic spline interpolation is an interpolation of irregularly spaced two-dimensional data points. "
	"The interpolating surface is a linear combination of Green functions centered at each data point. The amplitudes of "
	"the Green functions are found by solving a linear system of equations.")
NORMAL (U"The surface %s(#%x) is expressed as")
EQUATION (U"%s(#%x)=\\Si__%j%=1_^^n^ %w__%j_ %g(#%x, #%x__%j_),")
NORMAL (U"where %n is the number of data points #%x__%j_ = (%x__%j_, %y__%j_), %g(#%x, #%x__%j_) is Green's function and %w__%j_ is the weight of data point %j. The weights %w__%j_ are determined by requiring that the surface %s(#%x) passes exactly through the %n data points, i.e.")
EQUATION (U"%s(#%x__%i_)=\\Si__%j%=1_^^n^ %w__%j_ %g(#%x__%i_, #%x__%j_), %i = 1, 2, ..., %n.")
NORMAL (U"This yields an %n\\xx%n square linear system of equations which can be solved for the %w__%j_.")
NORMAL (U"For twodimensional data Green's function is:")
EQUATION (U"%g(#%x__%i_, #%x__%j_) = |#%x__%i_ - #%x__%j_|^^2^ (ln |#%x__%i_ - #%x__%j_| - 1.0).")
NORMAL (U"See @@Sandwell (1987)@ and @@Deng & Tang (2011)@ for more information.")
MAN_END

MAN_BEGIN (U"bootstrap", U"djmw", 20141101)
INTRO (U"The bootstrap data set is a random sample of size %n "
	"drawn %%with% replacement from the sample (%x__1_,...%x__n_). This "
	"means that the bootstrap data set consists of members of the original "
	"data set, some appearing zero times, some appearing once, some appearing "
	"twice, etc.")
NORMAL (U"More information can be found in @@Efron & Tibshirani (1993)@.")
MAN_END

MAN_BEGIN (U"canonical variate", U"djmw", 20230801)
NORMAL (U"A ##canonical variate# is a new variable (variate) formed by making a linear combination of two "
	"or more variates (variables) from a data set. "
	"A linear combination of variables is the same as a weighted sum of variables. "
	"Because we can in infinitely many ways choose combinations of weights between variables in a data set, "
	"there are also infinitely many canonical variates possible. ")
NORMAL (U"In general additional constraints should be satisfied by the weights to get a meaningful canonical variate. "
	"For example, in @@Canonical correlation analysis|canonical correlation analysis@ a data set is split up into two parts, a %%dependent% and an %%independent% part. "
	"In both parts we can form a canonical variate and we choose weights that maximize the correlation between these canonical variates "
	"(there is an @@TableOfReal: To CCA...|algorithm@ that calculates these weights).")
MAN_END

MAN_BEGIN (U"Categories", U"djmw", 19960918)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type Categories represents an ordered collection of categories. Each "
	"category is a simple text string.")
ENTRY (U"Categories commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu ##Create an empty Categories#")
LIST_ITEM (U"\\bu @@FFNet & PatternList: To Categories...@")
NORMAL (U"Viewing and editing:")
LIST_ITEM (U"\\bu @CategoriesEditor")
NORMAL (U"Analysis:")
LIST_ITEM (U"\\bu @@Categories: To Confusion@")
LIST_ITEM (U"\\bu @@Categories: Difference@")
NORMAL (U"Synthesis")
LIST_ITEM (U"\\bu @@Categories: Append@")
LIST_ITEM (U"\\bu ##Categories: Permute...#")
LIST_ITEM (U"\\bu ##Categories: To unique Categories#")
ENTRY (U"Inside a Categories")
NORMAL (U"With @Inspect you will see the following attributes:")
TERM (U"%size")
DEFINITION (U"the number of simple categories.")
TERM (U"%item[]")
DEFINITION (U"the categories. Each category is an object of type #SimpleString.")
MAN_END

MAN_BEGIN (U"Categories: Difference", U"djmw", 19960918)
INTRO (U"A command to compute the difference between two selected @Categories objects.")
ENTRY (U"Behaviour")
NORMAL (U"Each element in the first object is compared with the corresponding "
	"object in the second object according to its compare method. "
	"The number of different %categories will be shown in the @@Info window@.")
MAN_END

MAN_BEGIN (U"Categories: To Confusion", U"djmw", 19960918)
INTRO (U"A command to compute the @Confusion matrix from two selected "
	"@Categories objects.")
ENTRY (U"Algorithm")
NORMAL (U"A confusion matrix is constructed from both #Categories objects in "
	"the following way: The first Categories object is considered the stimulus "
	"Categories and its unique (sorted) categories "
	"form the row indices of the confusion matrix, the unique (sorted) "
	"categories of the second object form the column indices of this matrix.")
NORMAL (U"Next, each element in the first #Categories object is compared with "
	"the corresponding object in the second object and the element in the "
	"confusion matrix addressed by this pair is incremented by 1.")
MAN_END

MAN_BEGIN (U"Categories: Append", U"djmw", 19960918)
INTRO (U"You can choose this command after selecting 2 objects of type @Categories. "
	"A new object is created that contains the second object appended after the first.")
MAN_END

MAN_BEGIN (U"Categories: Edit", U"djmw", 19960918)
INTRO (U"You can choose this command after selecting one #Categories. "
	"A @CategoriesEditor will appear on the screen, with the selected #Categories in it.")
MAN_END

MAN_BEGIN (U"CategoriesEditor", U"djmw", 20210228)
ENTRY (U"An editor for manipulating @Categories.")
NORMAL (U"To make a selection, use the left mouse button.")
NORMAL (U"@@Command-click@ extends a selection (discontinuously).")
NORMAL (U"Shift-click extends a selection contiguously.")
MAN_END

MAN_BEGIN (U"CC", U"djmw", 20010219)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"Any object that represents cepstral coefficients as a function of "
	"time.")
MAN_END

MAN_BEGIN (U"CC: Get value in frame...", U"djmw", 20140926)
INTRO (U"Get the cepstral coefficient value at a specified position in a specified frame.")
MAN_END

MAN_BEGIN (U"CC: Get c0 value in frame...", U"djmw", 20140926)
INTRO (U"Get the zeroth cepstral coefficient value in the specified frame. For a @MFCC object this value relates to energy.")
MAN_END

MAN_BEGIN (U"CCA", U"djmw", 20020323)
INTRO (U"One of the @@types of objects@ in Praat. ")
NORMAL (U"An object of type CCA represents the @@Canonical correlation "
	"analysis@ of two multivariate datasets.")
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@TableOfReal: To CCA...@")
MAN_END

MAN_BEGIN (U"CCA: Get zero correlation probability...", U"djmw", 20040407)
INTRO (U"Get the probability that for the selected @CCA object the chosen "
	"canonical correlation coefficient is different from zero.")
ENTRY (U"Setting")
TERM (U"##Index")
DEFINITION (U"is the index of the canonical correlation coefficient that "
	"you want to test.")
ENTRY (U"Algorithm")
NORMAL (U"Wilks' statistic: the probability that coefficient \\ro__%index_ "
	"differs from zero is ")
EQUATION (U" %probability = chiSquareQ (\\ci^2, %ndf),")
NORMAL (U"where the %%number of degrees of freedom% parameter equals")
EQUATION (U"%ndf = (%n__y_ - %index +1)(%n__x_ - %index +1)")
NORMAL (U"and the chi-squared parameter is")
EQUATION (U"\\ci^2 = \\--(%numberOfObservations - (%n__y_ + %n__x_ +3)/2) "
	"log (\\La__%index_),")
NORMAL (U"In the formulas above the variables %n__y_ and %n__x_ are the "
	"dimensions of the dependent and the independent data sets whose "
	"canonical correlations have been "
	"obtained, and Wilks' lambda is:")
EQUATION (U"\\La__index_ = \\Pi__%i=%index..min(%ny,%nx)_ (1 \\-- \\ro__%i_^^2^)")
MAN_END

MAN_BEGIN (U"CCA & Correlation: To TableOfReal (loadings)", U"djmw", 20020525)
INTRO (U"Determine from the selected @CCA and @Correlation objects the correlations "
	"of the canonical variables with the original variables. These correlations are "
	"called %%canonical factor loadings%, or also %%structure correlation "
	"coefficients%.")
MAN_END

MAN_BEGIN (U"CCA & Correlation: Get variance fraction...", U"djmw", 20181112)
INTRO (U"Determine from the selected @CCA and @Correlation objects the fraction of the variance "
	"explained by the selected @@canonical variate@ range.")
ENTRY (U"Settings")
TERM (U"##X or Y#")
DEFINITION (U"determines whether you select the dependent (y) or the independent (x) set.")
TERM (U"##Canonical variate range")
DEFINITION (U"determines the canonical variates (or canonical variables).")
ENTRY (U"Remarks")
NORMAL (U"1. In general the variance fractions for a particular canonical variate in the "
	"dependent and in the independent set are not the same.")
NORMAL (U"2. In general, the variance fractions for all canonical variates do not sum to 1. \n(The technical reason is that for canonical correlation analysis in general the eigenvectors are not orthogonal, i.e. they overlap and therefore, necessarily, also the variance fractions overlap.) ")
ENTRY (U"Algorithm")
NORMAL (U"The formulas can be found on page 170 of @@Cooley & Lohnes (1971)@.")
NORMAL (U"For example, the fraction of the variance explained by the %i^^th^ canonical "
	"variable in the dependent set is:")
EQUATION (U"%%fractionVariance% = ((#y__i_\\'p #R__yy_\\'p #R__yy_ #y__i_) / (#y__i_\\'p #R__yy_ #y__i_)) / %n__%y_,")
NORMAL (U"where #y__%i_ is the eigenvector for dependent canonical variable %i and #R__%%yy%_ is the correlation matrix for the %n__%y_ variables in the dependent set.")
MAN_END

MAN_BEGIN (U"CCA & Correlation: Get redundancy (sl)...", U"djmw", 20060323)
INTRO (U"Determine from the selected @CCA and @Correlation objects the Stewart-Love redundancy for the "
	"selected canonical variates.")
NORMAL (U"The Stewart-Love redundancy for a single @@canonical variate@ is the fraction of variance explained by the selected "
	"canonical variate in a set times the fraction of shared variance between the corresponding canonical variates in the two sets.")
NORMAL (U"The Stewart-Love redundancy for a canonical variate range is the sum of the individual redundancies.")
ENTRY (U"Settings")
TERM (U"##X or Y#")
DEFINITION (U"determines whether you select the dependent (y) or the independent (x) set.")
TERM (U"##Canonical variate range#")
DEFINITION (U"determines the canonical variates (or canonical variables).")
ENTRY (U"Algorithm")
NORMAL (U"The formulas can be found on page 170 of @@Cooley & Lohnes (1971)@.")
NORMAL (U"For example, the redundancy of the dependent set (y) given the independent set (x) for the %i^^%%th%^ canonical "
	"variate can be expressed as:")
EQUATION (U"%R__%i_(y) = %%varianceFraction%__%i_(y) * \\ro__%i_^2, ")
NORMAL (U"where %%varianceFraction%__%i_(y) is the @@CCA & Correlation: Get variance fraction...|variance fraction@ explained "
	"by the %i^^%%th%^ canonical variate of the dependent set, and \\ro__%i_ is the %i^^%%th%^ canonical correlation coefficient.")
NORMAL (U"The redundancy for the selected canonical variate in the dependent set shows what "
	"fraction of the variance in the %%dependent% set is already \"explained\" by "
	"the variance in the %%independent% set, i.e. this fraction could be considered as redundant.")
NORMAL (U"In the same way we can measure the redundancy of the independent (x) set giving the dependent set (y).")
ENTRY (U"Remark")
NORMAL (U"In general %R__%i_(y) \\=/ %R__%i_(x).")
MAN_END

MAN_BEGIN (U"CCA & TableOfReal: To TableOfReal (loadings)", U"djmw", 20020525)
INTRO (U"Determine from the selected @CCA and @TableOfReal objects the correlations "
	"of the canonical variables with the original variables. These correlations are "
	"called %%canonical factor loadings%, or also %%structure correlation "
	"coefficients%.")
MAN_END

MAN_BEGIN (U"CCA & TableOfReal: To TableOfReal (scores)...", U"djmw", 20040407)
INTRO (U"Determines the scores on the dependent and the independent canonical "
	"variates from the selected @CCA and @TableOfReal objects.")
ENTRY (U"Settings")
TERM (U"##Number of canonical correlations#")
DEFINITION (U"determines the dimension, i.e., the number of elements of the resulting "
	"canonical score vectors. The newly created table will have twice this number of "
	"columns because we have calculated score vectors for the dependent and the "
	"independent variates.")
ENTRY (U"Behaviour")
NORMAL (U"The scores on the dependent set are determined as #T__%y_ #Y, where "
	"#T__%y_ is the dependent part in the table and #Y is a matrix with "
	"%numberOfCanonicalCorrelations eigenvectors for the dependent variate.")
NORMAL (U"The scores for the independent variates are then determined in an analogous "
	"way as #T__%x_ #X.")
NORMAL (U"The scores for the dependent data will be in the lower numbered columns, "
	"the scores for the independent part will be in the higher numbered columns of "
	"the newly created object.")
MAN_END


MAN_BEGIN (U"Canonical correlation analysis", U"djmw", 20181118)
INTRO (U"This tutorial will show you how to perform canonical correlation "
       "analysis with Praat.")
ENTRY (U"1. Objective of canonical correlation analysis")
NORMAL (U"In canonical correlation analysis we try to find the correlations between "
	"two data sets. One data set is called the %dependent set, the other the "
	"%independent set. In Praat these two sets must reside in one "
	"@TableOfReal object. The lower numbered columns of this table will then be "
	"interpreted as the dependent part, the rest of the columns as the "
	"independent part. "
	"The dimension of (i.e. the number of columns in) the dependent part may not "
	"exceed the dimension of the independent part.")
NORMAL (U"As an example, we will use the dataset from @@Pols et al. (1973)@ "
	"with the frequencies and levels of the first three formants from the 12 "
	"Dutch monophthongal vowels as spoken in a /h\\_ t/ context by 50 male speakers. "
	"We will try to find the canonical correlation between formant frequencies "
	"(the %dependent part) and levels (the %independent part). "
	"The dimension of both groups of variates is 3. "
	"In the introduction of the "
	"@@discriminant analysis@ tutorial you can find how to get these data, "
	"how to take the logarithm of the formant frequency values and how to "
	"standardize them. The following script summarizes:")
CODE (U"pols50m = Create TableOfReal (Pols 1973): \"yes\"")
CODE (U"Formula: ~ if col < 4 then log10 (self) else self endif")
CODE (U"Standardize columns")
NORMAL (U"Before we start with the %canonical correlation analysis we will first have "
	"a look at the %Pearson correlations of this table and  "
	"calculate the @Correlation matrix. It is given by:")
CODE (U"       F1     F2     F3     L1     L2     L3")
CODE (U"F1   1     -0.338  0.191  0.384 -0.505 -0.014")
CODE (U"F2  -0.338  1      0.190 -0.106  0.526 -0.568")
CODE (U"F3   0.191  0.190  1      0.113 -0.038  0.019")
CODE (U"L1   0.384 -0.106  0.113  1     -0.038  0.085")
CODE (U"L2  -0.505  0.526 -0.038 -0.038  1      0.128")
CODE (U"L3  -0.014 -0.568  0.019  0.085  0.128  1")
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: pols50m")
CODE (U"To Correlation")
CODE (U"Draw as numbers: 1, 0, \"decimal\", 3")
NORMAL (U"The correlation matrix shows that high correlations exist between some "
	"formant frequencies and some levels. For example, the correlation "
	"coefficient between F2 and L2 equals 0.526.")
NORMAL (U"In a canonical correlation analysis of the dataset above, we try "
	"to find the linear "
	"combination %u__1_ of %F__1_, %F__2_ and %F__3_ that correlates maximally "
	"with the linear combination %v__1_ of %L__1_, %L__2_ and %L__3_. "
	"When we have found these %u__1_ and %v__1_ we next try to find a new "
	"combination %u__2_ of the formant frequencies and a new combination "
	"%v__2_ of the levels that have maximum correlation. These %u__2_ and "
	"%v__2_ should be uncorrelated with %u__1_ and %v__1_. "
	"When we express the above with formulas we have:")
EQUATION (U"%u__1_ = %y__11_%F__1_+%y__12_%F__2_ + %y__13_%F__3_")
EQUATION (U"%v__1_ = %x__11_%L__1_+%x__12_%L__2_ + %x__13_%L__3_")
EQUATION (U"\\ro(%u__1_, %v__1_) = maximum, \\ro(%u__2_, %v__2_) = submaximum, ")
EQUATION (U"\\ro(%u__2_, %u__1_) = \\ro (%u__2_, %v__1_) = \\ro (%v__2_, %v__1_) "
	"= \\ro (%v__2_, %u__1_) = 0,")
NORMAL (U"where the \\ro(%u__i_, %v__i_) are the correlations between the "
	"@@canonical variate@s %u__i_ and %v__i_ and the %y__%ij_'s and %x__%ij_'s are"
	" the ##canonical coefficients# for the dependent and the independent "
	"variates, respectively.")
ENTRY (U"2. How to perform a canonical correlation analysis")
NORMAL (U"Select the TableOfReal and choose from the dynamic menu the option "
	"@@TableOfReal: To CCA...|To CCA...@. This command is available in the "
	"\"Multivariate statistics\" action button. We fill out the form and supply "
	"3 for %%Dimension of dependent variate%. The resulting CCA object will bear "
	"the same name as the TableOfReal object. The following script summarizes:")
CODE (U"selectObject: pols50m")
CODE (U"cca = To CCA: 3")
ENTRY (U"3. How to get the canonical correlation coefficients")
NORMAL (U"You can get the canonical correlation coefficients by querying the CCA "
	"object. You will find that the three canonical correlation coefficients, "
	"\\ro(%u__1_, %v__1_), \\ro(%u__2_, %v__2_) and \\ro(%u__3_, %v__3_) are "
	" approximately 0.86, 0.53 and 0.07, respectively. "
	"The following script summarizes:")
CODE (U"cc1 = Get correlation: 1")
CODE (U"cc2 = Get correlation: 2")
CODE (U"cc3 = Get correlation: 3")
CODE (U"writeInfoLine: \"cc1 = \", cc1, \", cc2 = \", cc2, \", cc3 = \", cc3")
ENTRY (U"4. How to obtain canonical scores")
NORMAL (U"Canonical #scores, also named @@canonical variate@s, are the linear combinations:")
EQUATION (U"%u__%i_ = %y__%i1_%F__1_+%y__%i2_%F__2_ + %y__%i3_%F__3_, and,")
EQUATION (U"%v__%i_ = %x__%i1_%L__1_+%x__%i2_%L__2_ + %x__%i3_%L__3_,")
NORMAL (U"where the index %i runs from 1 to the number of correlation coefficients.")
NORMAL (U"You can get the canonical scores by selecting a CCA object together with "
	"the TableOfReal object and choose "
	"@@CCA & TableOfReal: To TableOfReal (scores)...|To TableOfReal (scores)...@")
NORMAL (U"When we now calculate the ##Correlation# matrix of these canonical variates we "
	"get the following table:")
CODE (U"       u1     u2     u3     v1     v2     v3")
CODE (U"u1     1      .      .    0.860    .      .")
CODE (U"u2     .      1      .      .    0.531    .")
CODE (U"u3     .      .      1      .      .    0.070")
CODE (U"v1   0.860    .      .      1      .      .")
CODE (U"v2     .    0.531    .      .      1      .")
CODE (U"v3     .      .    0.070    .      .      1")
NORMAL (U"The scores with a dot are zero to numerical precision. In this table the "
	"only correlations that differ from zero are the canonical correlations. "
	"The following script summarizes:")
CODE (U"selectObject: cca, pols50m")
CODE (U"To TableOfReal (scores): 3")
CODE (U"To Correlation")
CODE (U"Draw as numbers if: 1, 0, \"decimal\", 2, ~ abs(self) > 1e-14")
ENTRY (U"5. How to predict one dataset from the other")
NORMAL (U"@@CCA & TableOfReal: Predict...@")
NORMAL (U"Additional information can be found in @@Weenink (2003)@.")
MAN_END

MAN_BEGIN (U"CCA & TableOfReal: Predict...", U"djmw", 20020503)
INTRO (U"")
MAN_END

MAN_BEGIN (U"Chebyshev polynomials", U"djmw", 19990620)
INTRO (U"The Chebyshev polynomials %T__%n_(%x) of degree %n are special orthogonal polynomial functions "
	"defined on the domain [-1, 1].")
NORMAL (U"Orthogonality:")
EQUATION (U"__-1_\\in^^1^ %W(%x) %T__%i_(%x) %T__%j_(%x) %dx = \\de__%ij_")
EQUATION (U"%W(%x) = (1 \\-- %x^^2^)^^\\--1/2^   (-1 < x < 1)")
NORMAL (U"They obey certain recurrence relations:")
EQUATION (U"%T__%n_(%x) = 2 %x %T__%n-1_(%x) \\-- %T__%n-2_(%x)")
EQUATION (U"%T__0_(%x) = 1")
EQUATION (U"%T__1_(%x) = %x")
MAN_END

MAN_BEGIN (U"ChebyshevSeries", U"djmw", 19990620)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type ChebyshevSeries represents a linear combination of @@Chebyshev polynomials@ "
	"%T__%k_(%x).")
EQUATION (U"ChebyshevSeries (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %T__%k_(%x)")
MAN_END

MAN_BEGIN (U"ChebyshevSeries: To Polynomial", U"djmw", 19990620)
INTRO (U"A command to transform the selected @ChebyshevSeries object into a @@Polynomial@ object.")
NORMAL (U"We find polynomial coefficients %c__%k_ such that")
EQUATION (U"\\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x^^%k^ = \\Si__%k=1.."
	"%numberOfCoefficients_ %l__%k_ %T__%k_(%x)")
NORMAL (U"We use the recurrence relation for @@Chebyshev polynomials@ to calculate these coefficients.")
MAN_END

MAN_BEGIN (U"ClassificationTable: To Confusion...", U"djmw", 20141030)
INTRO (U"A command to create a @Confusion object from the selected @ClassificationTable object.")
ENTRY (U"Settings")
TERM (U"##Only class labels#")
DEFINITION (U"defines whether the class labels from the ClassificationTable object will be used not only as response labels but also as stimulus labels. If checked the resulting Confusion will always have equal stimulus and response labels. If not checked the stimulus labels will be determined from the row labels of the ClassificationTable object. ")
ENTRY (U"Behaviour")
NORMAL (U"In obtaining a Confusion object from a ClassificationTable we explicitly use its row labels as stimulus labels.")
MAN_END

MAN_BEGIN (U"ClassificationTable", U"djmw", 19990525)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type ClassificationTable represents the result of a classification experiment. "
	"The numbers in a row show how well a particular input matches the classes represented by the "
	"column labels. The higher the number the better the match.")
MAN_END

MAN_BEGIN (U"concentration ellipse", U"djmw", 20071113)
INTRO (U"The percentage of bivariate normally distributed data covered by an ellipse "
	"whose axes have a length of %numberOfSigmas \\.c %\\si can be obtained by integration of the probability distribution function "
	"over an elliptical area. This results in the following equation, as can be "
	"verified from equation 26.3.21 in @@Abramowitz & Stegun (1970)@:")
EQUATION (U"%percentage = (1 - exp (-%numberOfSigmas^^2^/2)) \\.c 100\\% ,")
NORMAL (U"where the %numberOfSigmas is the radius of the \"ellipse\":")
EQUATION (U"(%x/%\\si__x_)^2 + (%y/%\\si__y_)^2 = %numberOfSigmas^2.")
NORMAL (U"The %numberOfSigmas=1 ellipse covers 39.3\\%  of the data, "
	"the %numberOfSigmas=2 ellipse 86.5\\% , and "
	"the %numberOfSigmas=3 ellipse 98.9\\% .")
NORMAL (U"From the formula above we can show that if we want to cover %p percent of the data, we have to "
	"choose %numberOfSigmas as:")
EQUATION (U"%numberOfSigmas = \\Vr(-2 ln(1-%p/100)).")
NORMAL (U"For covering 95\\%  of the data we calculate %numberOfSigmas = 2.45.")
MAN_END

MAN_BEGIN (U"confidence interval", U"djmw", 20151109)
INTRO (U"The confidence interval gives an estimated range of values which "
	"is likely to include a true but unknown population parameter. "
	"The estimated range is calculated from a given set of observations.")
ENTRY (U"Examples")
NORMAL (U"At the %\\al level of significance, a two-sided confidence interval "
	"for the true mean %\\mu for normally distributed data with %observed mean %m and "
	"%true standard deviation %\\si can be constructed as:")
EQUATION (U"%%mean% - %z__\\al/2_ %\\si / \\Vr%N \\<_ %\\mu \\<_ "
	"%%mean% + %z__%\\al/2_ %\\si / \\Vr%N,")
NORMAL (U"where %z__%\\al/2_ = invGaussQ (%\\al/2) and %N is the number of observations.")
NORMAL (U"If the standard deviation is %not known, we have to estimate its value (%s) "
	"from the data and the formula above becomes:")
EQUATION (U"%%mean% - %t__%%\\al/2;N%_ %s / \\Vr%N \\<_ %\\mu \\<_ "
	"%%mean% + %t__%\\al/2;%N_ %s / \\Vr%N,")
NORMAL (U"where %t__%\\al/2;%N_ = invStudentQ (%\\al/2, %N-1).")
NORMAL (U"For %\\al=0.05 and %N=20 we get %z__0.025_=1.96 and %t__0.025;20_=2.093. "
	"This shows that for a fixed value of the standard deviation the confidence interval will always be wider if we had to estimate the standard deviation's value from the data instead of its value being known beforehand.")
MAN_END

MAN_BEGIN (U"confidence level", U"djmw", 20011105)
NORMAL (U"The confidence level is the probability value 1-\\al associated "
	"with a @@confidence interval@, where \\al is the level of significance. "
	"It can also be expressed as a percentage 100(1-\\al)\\%  and is then "
	"sometimes called the %%confidence coefficient%.")
MAN_END

MAN_BEGIN (U"Confusion", U"djmw", 20110517)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type Confusions represents a confusion matrix, with "
	"stimuli as row labels and responses as column labels. The entry at "
	"position [%i] [%j] represents the number of times response %j "
	"was given to the stimulus %i.")
ENTRY (U"Creating a Confusion from data in a text file")
NORMAL (U"Suppose you have two objects A and B. "
	"In one way or another, you have acquired the following  "
	"confusions: %\\de__%AA_ = 6, %\\de__%AB_ = 2 , %\\de__%BA_ = 1, "
	"and %\\de__%BB_ = 7.")
NORMAL (U"You can create a simple text file like the following:")
CODE (U"\"ooTextFile\"  ! to make Praat recognize your file")
CODE (U"\"Confusion\" ! The line that tells Praat about the contents")
CODE (U"2     \"A\"  \"B\"  ! Number of columns, and column labels")
CODE (U"2                   ! Number of rows")
CODE (U"\"A\"    6    2     ! Row label A, A-A value, A-B value")
CODE (U"\"B\"    1    7     ! Row label B, B-A value, B-B value")
NORMAL (U"This text file can be read with the @@Read from file...@ command. ")
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Categories: To Confusion")
LIST_ITEM (U"\\bu @@Create simple Confusion...")
NORMAL (U"Drawing")
LIST_ITEM (U"\\bu ##Draw as numbers...")
LIST_ITEM (U"\\bu ##Draw as squares...")
NORMAL (U"Query")
LIST_ITEM (U"\\bu @@Confusion: Get fraction correct|Get fraction correct")
LIST_ITEM (U"\\bu @@Confusion: Get stimulus sum...")
LIST_ITEM (U"\\bu @@Confusion: Get response sum...")
LIST_ITEM (U"\\bu ##Get grand sum")
NORMAL (U"Modification")
LIST_ITEM (U"\\bu ##Formula...")
LIST_ITEM (U"\\bu @@Confusion: Increase...")
NORMAL (U"Grouping")
LIST_ITEM (U"\\bu @@Confusion: Group stimuli...@")
LIST_ITEM (U"\\bu @@Confusion: Group responses...@")
NORMAL (U"Analysis:")
LIST_ITEM (U"\\bu @@Confusion: To Similarity...@")
LIST_ITEM (U"\\bu @@Confusion: To Dissimilarity (pdf)...@")
ENTRY (U"Inside a Confusion")
NORMAL (U"With @Inspect you will see the following attributes:")
TERM (U"%numberOfRows")
DEFINITION (U"the number of stimuli.")
TERM (U"%numberOfColumns")
DEFINITION (U"the number of responses.")
TERM (U"%rowLabels")
DEFINITION (U"the names of the stimuli.")
TERM (U"columnLabels")
DEFINITION (U"the names of the responses.")
MAN_END

MAN_BEGIN (U"Create simple Confusion...", U"djmw", 20140117)
INTRO (U"Creates a square @@Confusion|confusion matrix@ with equal stimulus labels and response labels.")
ENTRY (U"Example")
NORMAL (U"The command ##Create simple Confusion: \"simple\", \"u i a\"# results in the following Confusion:")
CODE (U"     u    i    a   ! The response labels")
CODE (U"u    0    0    0   ! Responses on stimulus  u,")
CODE (U"i    0    0    0   ! Responses on stimulus  i")
CODE (U"a    0    0    0   ! Responses on stimulus  a")
MAN_END

MAN_BEGIN (U"Confusion: Increase...", U"djmw", 20140117)
INTRO (U"Increases the contents of the corresponding cell in the selected @@Confusion@ by one.")
ENTRY (U"Settings")
TERM (U"##Stimulus# and ##Response#")
DEFINITION (U"define the cell whose value will be increased by one.")
ENTRY (U"Example")
NORMAL (U"Given the following Confusion:")
CODE (U"       u    i    a   ! The response labels")
CODE (U" u     6    2    1   ! Responses on stimulus  u,")
CODE (U" i     3    4    2   ! Responses on stimulus  i")
CODE (U" a     1    4    4   ! Responses on stimulus  a")
NORMAL (U"The command  ##Increase: \"u\", \"i\"#  results in:")
CODE (U"       u    i    a   ! The responses")
CODE (U" u     6    3    1   ! Responses on stimulus  u,")
CODE (U" i     3    4    2   ! Responses on stimulus  i")
CODE (U" a     1    4    4   ! Responses on stimulus  a")
MAN_END

MAN_BEGIN (U"Confusion: Group...", U"djmw", 20140117)
INTRO (U"Groups a number of stimuli and responses into one new category.")
ENTRY (U"Settings")
TERM (U"##Stimuli & Responses")
DEFINITION (U"defines the labels that will be grouped.")
TERM (U"##New label")
DEFINITION (U"defines the new label for the grouped labels.")
TERM (U"##New label position")
DEFINITION (U"the row/column number for the new group label.")
ENTRY (U"Example")
NORMAL (U"Given the following selected Confusion:")
CODE (U"       u    i    a   ! The response labels")
CODE (U" u     6    2    1   ! Responses on stimulus `u`")
CODE (U" i     3    4    2   ! Responses on stimulus `i`")
CODE (U" a     1    4    4   ! Responses on stimulus `a`")
NORMAL (U"After the command ##Group stimuli: \"u i\", \"high\", 0#, the new Confusion will be:")
CODE (U"         high   a   ! The new response labels")
CODE (U" high     15    3   ! Responses on group `high`")
CODE (U" a         5    4   ! Responses on stimulus `a`")
NORMAL (U"Instead after the command ##Group stimuli: \"u i\", \"high\", 2#, the new Confusion will be:")
CODE (U"        a   high  ! The new response labels")
CODE (U" a      4     5   ! Responses on stimulus `a`")
CODE (U" high   3    15   ! Responses on group `high`")
MAN_END

MAN_BEGIN (U"Confusion: Group stimuli...", U"djmw", 20140117)
INTRO (U"Groups a number of stimuli into one new category.")
ENTRY (U"Settings")
TERM (U"##Stimuli")
DEFINITION (U"defines the stimuli that will be grouped.")
TERM (U"##New label")
DEFINITION (U"defines the new label for the grouped stimuli.")
TERM (U"##New label position")
DEFINITION (U"the row number for the new group label.")
ENTRY (U"Example")
NORMAL (U"Given the following selected Confusion:")
CODE (U"       u    i    a   ! The response labels")
CODE (U" u     6    2    1   ! Responses on stimulus `u`")
CODE (U" i     3    4    2   ! Responses on stimulus `i`")
CODE (U" a     1    4    4   ! Responses on stimulus `a`")
NORMAL (U"After the command ##Group stimuli: \"u i\", \"high\", 1#, the new Confusion will be:")
CODE (U"          u    i    a   ! The response labels")
CODE (U" high     9    6    3   ! Responses on stimulus group `high`")
CODE (U" a        1    4    4   ! Responses on stimulus `a`")
MAN_END

MAN_BEGIN (U"Confusion: Group responses...", U"djmw", 20140117)
INTRO (U"Groups a number of responses into one new category.")
ENTRY (U"Settings")
TERM (U"##Responses")
DEFINITION (U"defines the responses that will be grouped.")
TERM (U"##New label")
DEFINITION (U"defines the new label for the grouped responses.")
TERM (U"##New label position")
DEFINITION (U"the column number for the new group label.")
ENTRY (U"Example")
NORMAL (U"Given the following selected Confusion:")
CODE (U"       u    i    a   ! The response labels")
CODE (U" u     6    2    1   ! Responses on stimulus `u`")
CODE (U" i     3    4    2   ! Responses on stimulus `i`")
CODE (U" a     1    4    4   ! Responses on stimulus `a`")
NORMAL (U"After the command ##Group responses: \"a i\", \"front\", 1#, the new Confusion will be:")
CODE (U"   front    i    ! The new response labels")
CODE (U" u     7    2    ! Responses on stimulus `u`")
CODE (U" i     5    4    ! Responses on stimulus `i`")
CODE (U" a     5    4    ! Responses on stimulus `a`")
MAN_END

MAN_BEGIN (U"Confusion: Get stimulus sum...", U"djmw", 20140117)
INTRO (U"Returns the number of responses for the chosen stimulus (the sum of all the numbers in the row with this stimulus label). ")
ENTRY (U"Example")
NORMAL (U"Given the following selected Confusion:")
CODE (U"       u    i    a   ! The response labels")
CODE (U" u     6    2    1   ! Responses on stimulus `u`")
CODE (U" i     3    4    2   ! Responses on stimulus `i`")
CODE (U" a     1    4    4   ! Responses on stimulus `a`")
NORMAL (U"The command ##Get stimulus sum: \"a\"# will return the number 9.")
MAN_END

MAN_BEGIN (U"Confusion: Get response sum...", U"djmw", 20140117)
INTRO (U"Returns the number of times the chosen response was given (the sum of all the numbers in the column with this response label).")
ENTRY (U"Example")
NORMAL (U"Given the following selected Confusion:")
CODE (U"       u    i    a   ! The response labelss")
CODE (U" u     6    2    1   ! Responses on stimulus `u`")
CODE (U" i     3    4    2   ! Responses on stimulus `i`")
CODE (U" a     1    4    4   ! Responses on stimulus `a`")
NORMAL (U"The command ##Get response sum: \"a\"# will return the number 7.")
MAN_END

MAN_BEGIN (U"Confusion: Condense...", U"djmw", 20130410)
INTRO (U"Groups row and column labels of the selected @Confusion object in "
	"order to reduce its dimension. ")
ENTRY (U"Settings")
TERM (U"##Search")
DEFINITION (U"the pattern to match.")
TERM (U"##Replace")
DEFINITION (U"the pattern that replaces the match(es).")
TERM (U"##Replace limit")
DEFINITION (U"limits the maximum number of times that a match/replace cycle "
	"may occur within each label.")
TERM (U"##Search and replace are")
DEFINITION (U"defines whether the search and replace strings are taken "
	"literally or as a @@Regular expressions|regular expression@.")
ENTRY (U"Behaviour")
NORMAL (U"First all row and column labels are changed according to the search "
	"and replace specification. Next all rows or columns that have the same "
	"labels are summed. ")
MAN_END

MAN_BEGIN (U"Confusion: Get fraction correct", U"djmw", 20000225)
INTRO (U"A @@Query submenu|query@ to ask the selected @Confusion matrix for the fraction of "
	"correct classifications.")
NORMAL (U"The \"fraction correct\" is defined as the quotient of the number "
	"of correct classifications and the sum of the entries in the matrix.")
NORMAL (U"Correct classifications have identical row and column labels.")
MAN_END

MAN_BEGIN (U"Confusion & ClassificationTable: Increase confusion count", U"djmw", 20230801)
INTRO (U"Increases the contents of cell(s) in the selected @@Confusion@. The cells to increase are determined by the selected "
	"@ClassificationTable.")
ENTRY (U"Behaviour")
NORMAL (U"For each row in the ClassificationTable object the contents of one cell in the Confusion we be increased by one. "
	"This cell is determined as follows: we start by finding the label of the column which the largest number in it. "
	"This label is defined as the ##response label#. We use the corresponding row label as the ##stimulus label#. The content "
	"of the cell in the Confusion object whose row and column are labeled with ##stimulus label# and ##response label#, "
	"respectively, is increased by one.")
MAN_END

MAN_BEGIN (U"Confusion: To TableOfReal (marginals)", U"djmw", 20011031)
INTRO (U"A new @TableOfReal object is created from the selected @Confusion "
	"object with one extra row and column. ")
NORMAL (U"The first element of the extra row will contain the sum of the "
	"confusions in the the first %column, the first element of the extra "
	"column will contain the sum of the confusions in the the first %row, "
	"etc... The bottom-right element will contain the sum of all confusions.")
MAN_END

MAN_BEGIN (U"Correlation", U"djmw", 19990105)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type Correlation represents the correlation coefficients "
	"of a multivariate data set.")
MAN_END

MAN_BEGIN (U"Correlation: Confidence intervals...", U"djmw", 20040407)
INTRO (U"Calculates @@confidence interval@s for the correlation coefficients "
	"from the selected @Correlation object(s) and saves these intervals in a "
	"new @TableOfReal object.")
ENTRY (U"Settings")
TERM (U"##Confidence level")
DEFINITION (U"the @@confidence level@ you want for the confidence intervals.")
TERM (U"##Number of tests")
DEFINITION (U"determines the @@Bonferroni correction@ for the significance "
	"level. If the default value (zero) is chosen, it will be set equal "
	"to the number of correlations involved (a matrix of dimension %n "
	"has %n\\.c(%n-1)/2 correlations).")
TERM (U"##Approximation")
DEFINITION (U"defines the approximation that will be used to calculate the "
	"confidence intervals. It is either Fisher's z transformation or Ruben's "
	"transformation. According to @@Boomsma (1977)@, Ruben's approximation is "
	"more accurate than Fisher's.")
ENTRY (U"Algorithm")
NORMAL (U"We obtain intervals by the large-sample conservative multiple tests "
	"with Bonferroni inequality and the Fisher or Ruben transformation. "
	"We put the upper values of the confidence intervals in the upper "
	"triangular part of the matrix and the lower values of the confidence "
	"intervals in lower triangular part of the resulting TableOfReal object.")
NORMAL (U"In %%Fisher's approximation%, for each element %r__%ij_ of the "
	"correlation matrix the confidence interval is:")
EQUATION (U"#[ tanh (%z__%ij_ - %z__%\\al\\'p_ / \\Vr(%N - 3)) , "
	"tanh (%z__%ij_ + %z__%\\al\\'p_ / \\Vr(%N - 3)) #],")
NORMAL (U"where %z__%ij_ is the Fisher z-transform of the correlation %r__%ij_:")
EQUATION (U"%z__%ij_ = 1/2 ln ((1 + %r__%ij_) / (1 - %r__%ij_)), ")
NORMAL (U"%z__%\\al\\'p_ the Bonferroni corrected %z-value "
	"%z__%\\al/(2\\.c%numberOfTests)_, ")
EQUATION (U"%\\al = 1 - %confidenceLevel,")
NORMAL (U"and %N the number of observations that the correlation matrix is "
	"based on.")
NORMAL (U"In %%Ruben's approximation% the confidence interval for element %r__%ij_ "
	"is:")
EQUATION (U"#[ %x__1_ / \\Vr(1 - %x__1_^2), %x__2_ / \\Vr(1 - %x__2_^2) #]")
NORMAL (U"in which %x__1_ and %x__2_ are the smallest and the largest root from")
EQUATION (U"%a %x^^2^ + %b %x + %c = 0, with")
EQUATION (U"%a = 2%N - 3 - %z__%\\al\\'p_^^2^")
EQUATION (U"%b = - 2 %r\\'p \\Vr((2%N - 3)(2%N - 5))")
EQUATION (U"%c = (2%N - 5 - %z__%\\al\\'p_^^2^) %r\\'p^^2^ - 2%z__%\\al\\'p_^^2^, and")
EQUATION (U"%r\\'p = %r__%ij_ / \\Vr(1 - %r__%ij_^2),")
MAN_END

MAN_BEGIN (U"Covariance", U"djmw", 19990105)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type Covariance represents the sums of squares and cross "
	"products of a multivariate data set divided by the number of observations.")
NORMAL (U"An object of type Covariance contains the same attributes as an object of "
	"type @SSCP.")
NORMAL (U"Since an object of type Covariance contains the mean values (the "
	"centroids), the covariances as well as the number of observations it has "
	"all the information necessary to be the subject of all kinds of statistical "
	"tests on means and variances.")
MAN_END

MAN_BEGIN (U"Create simple Correlation...", U"djmw", 20151230)
INTRO (U"Create a @@Correlation@ matrix with its centroid.")
ENTRY (U"Settings")
TERM (U"##Correlations")
DEFINITION (U"define the correlations. Because a correlation matrix is a symmetric matrix, only the upper triangular "
	"part of the matrix needs to be input (row-wise). If your correlation matrix is "
	"of dimension %d, your input needs %d(%d+1)/2 elements. The first %d input elements are the elements of the first "
	"row of the correaltion matrix, the next %d-1 input elements are for the second row, then %d-2 for the third row, etc.")
TERM (U"##Centroid")
DEFINITION (U"defines the centroid. ")
TERM (U"##Number of observations")
DEFINITION (U"defines the number of observations.")
MAN_END

MAN_BEGIN (U"Create simple Covariance...", U"djmw", 20101125)
INTRO (U"Create a @@Covariance@ matrix with its centroid.")
ENTRY (U"Settings")
TERM (U"##Covariances")
DEFINITION (U"define the covariances. Because a covariance matrix is a symmetric matrix, only the upper triangular "
	"part of the matrix needs to be input (row-wise). If your covariance matrix is "
	"of dimension %d, your input needs %d(%d+1)/2 elements. The first %d input elements are the elements of the first "
	"row of the covariance matrix, the next %d-1 input elements are for the second row, then %d-2 for the third row, etc.")
TERM (U"##Centroid")
DEFINITION (U"defines the centroid. ")
TERM (U"##Number of observations")
DEFINITION (U"defines the number of observations. ")
MAN_END

MAN_BEGIN (U"Covariance: Set value...", U"djmw", 20101124)
INTRO (U"Input @@Covariance@ matrix cell values.")
ENTRY (U"Constraints on input values")
TERM (U"A covariance matrix is a %%symmetric% matrix: values input at cell [%i,%j] will be automatically input at "
	"cell [%j,%i] too.")
TERM (U"All values on the diagonal should be positive numbers.")
TERM (U"The absolute value of an off-diagonal element at cell [%i,%j] should be smaller than the corresponding diagonal "
	"elements at cells [%i,%i] and [%j,%j].")
MAN_END

MAN_BEGIN (U"Covariance: Difference", U"djmw", 20090624)
INTRO (U"You can choose this command after selecting two objects of type @Covariance. ")
NORMAL (U"We test the hypothesis that the samples that gave rise to the two "
	"covariance matrices #%M__1_ and #%M__2_, have equal covariances. "
	"The test statistic is %L\\'p which is distributed as "
	"a \\ci^2 variate with %p(%p+1)/2 degrees of freedom.")
EQUATION (U"%L\\'p = %L \\.c (1 \\-- (2%p + 1 \\-- 2 / (%p + 1)) / (6 \\.c ( %N \\-- 1))),")
NORMAL (U"where, ")
EQUATION (U"%L = (%N \\-- 1) \\.c (ln determinant (#%M__1_) \\-- ln determinant "
	"(#%M__2_)) + trace (#%M__2_ \\.c #%M__1_^^\\--1^) \\-- %p), ")
NORMAL (U"%p is dimension of covariance matrix and "
	"%N is the number of observations underlying the covariance matrix. ")
NORMAL (U"For more details on this test, see e.g. page 292 of @@Morrison (1990)@.")
MAN_END

MAN_BEGIN (U"Covariance: Get significance of one mean...", U"djmw", 20040407)
INTRO (U"Gets the level of significance for one mean from the selected "
	"@Covariance object being different from a hypothesized mean.")
ENTRY (U"Settings")
TERM (U"##Index")
DEFINITION (U"the position of the element in the means vector (centroid) that "
	"you want to test. ")
TERM (U"##Value")
DEFINITION (U"the hypothesized mean %\\mu (see below).")
ENTRY (U"Behaviour")
NORMAL (U"This is the standard test on means when the variance is unknown. "
	"The test statistic is")
EQUATION (U"%t = (%mean - %\\mu) \\Vr%(N / %s^2),")
NORMAL (U"which has the Student %t distribution with %ndf = %N-1 degrees of freedom.")
NORMAL (U"In the formulas above, %mean is the element of the mean vector at "
	"position %index, %\\mu is the hypothesized mean, "
	"%N is the number of observations, %s^2 "
	"is the variance at position [%index] [%index] in the covariance matrix.")
NORMAL (U"The returned probability %p is the %%two-sided% probability")
EQUATION (U"%p = 2 * studentQ (%t, %ndf)")
NORMAL (U"A low probability %p means that the difference is significant.")
MAN_END

MAN_BEGIN (U"Covariance: Get fraction variance...", U"djmw", 20040407)
INTRO (U"A command to ask the selected @Covariance object for the fraction "
	"of the total variance that is accounted for by the selected dimension(s).")
ENTRY (U"Settings")
TERM (U"##From dimension#, ##To dimension#")
DEFINITION (U"define the range of components. By choosing both numbers equal, "
	"you get the fraction of the variance \"explained\" by that dimension.")
ENTRY (U"Details")
NORMAL (U"The total variance is the sum of the diagonal elements of the covariance "
	"matrix #C, i.e., its trace. "
	"The fraction is defined as:")
EQUATION (U"\\Si__%i=%from..%to_ %%C__ii_% / \\Si__%i=1..%numberOfRows_ %%C__ii_%")
MAN_END

MAN_BEGIN (U"Covariance: Get significance of means difference...", U"djmw", 20160102)
INTRO (U"Gets the level of significance for the %difference of two means "
	"from the selected @Covariance object being different from a hypothesized "
	"value.")
ENTRY (U"Settings")
TERM (U"##Index1#, ##Index2#")
DEFINITION (U"the positions of the two elements of the means vector whose "
	"difference is compared to the hypothesized difference.")
TERM (U"##Value")
DEFINITION (U"the hypothesized difference (%\\mu).")
TERM (U"##Paired")
DEFINITION (U"determines whether we treat the two means as being dependent. ")
TERM (U"##Equal variances")
DEFINITION (U"determines whether the distribution of the difference of the means "
	"is a Student t-distribution (see below).")
ENTRY (U"Behaviour")
NORMAL (U"This is Student's t-test for the significance of a difference of means. "
	"The test statistic is:")
EQUATION (U"%t = (%x\\-^__1_ - %x\\-^__2_ - %\\mu) \\Vr (%N / %s^2) with %ndf "
	"degrees of freedom.")
NORMAL (U"In the formula above %x\\-^__1_ and %x\\-^__2_ are the elements of the "
	"means vector, %\\mu is the hypothesized difference and %N is the number of "
	"observations. The value that we use for the (combined) variance %s^2 is:")
EQUATION (U"%s^2 = %var__1_ + %var__2_ - 2 * %covar__12_,")
NORMAL (U"when the samples are %paired, and ")
EQUATION (U"%s^2 = %var__1_ + %var__2_ ")
NORMAL (U"when they are not.")
NORMAL (U"The %var__1_ and %var__2_ are the variance components for "
	"%x\\-^__1_ and %x\\-^__2_, respectively, and %covar__12_ is their covariance."
	" When we have %%paired samples% we assume that the two variances are "
	"not independent and their covariance is subtracted, otherwise their "
	"covariance is not taken into account. Degrees of freedom parameter %ndf "
	"usually equals 2(%N-1). ")
NORMAL (U"If the two variances are significantly different, the statistic %t "
	"above is only %approximately distributed as Student's %t with "
	"degrees of freedom equal to:")
EQUATION (U"%ndf = (%N-1) \\.c (%var__1_ + %var__2_)^2 / (%var__1_^2 + "
	"%var__2_^2).")
NORMAL (U"The returned probability %p will be the %%two-sided% probability")
EQUATION (U"%p = 2 * studentQ (%t, %ndf)")
NORMAL (U"A low probability %p means that the difference is significant.")
MAN_END

MAN_BEGIN (U"Covariance: Get significance of one variance...", U"djmw", 20160102)
INTRO (U"Gets the probability for one variance from the selected "
	"@Covariance object being different from a hypothesized variance.")
ENTRY (U"Settings")
TERM (U"##Index")
DEFINITION (U"the position of the variance element %s^2.")
TERM (U"##Value")
DEFINITION (U"the hypothesized variance %\\si^2")
ENTRY (U"Behaviour")
NORMAL (U"The test statistic")
EQUATION (U"%\\ci^2 = (%N-1)%s^2 / %\\si^2,")
NORMAL (U"is distributed as a chi-squared variate with %ndf = %N-1 degrees of freedom.")
NORMAL (U"The returned probability %p will be ")
EQUATION (U"%p = chiSquareQ (%\\ci^2, %ndf)")
MAN_END

MAN_BEGIN (U"Covariance: Get significance of variance ratio...", U"djmw", 20040407)
INTRO (U"Gets the probability for the ratio of two variances "
	"from the selected @Covariance object being different from a hypothesized "
	"ratio.")
ENTRY (U"Settings")
TERM (U"##Index1#, ##Index2#")
DEFINITION (U"determine the variances.")
TERM (U"##Hypothesized ratio")
DEFINITION (U"the hypothesized ratio %F.")
ENTRY (U"Behaviour")
NORMAL (U"The test statistic")
EQUATION (U"%f = %s__1_^2 / %s__2_^2 / %ratio")
NORMAL (U"is distributed as Fisher's F distribution with %ndf__1_ = %N-1 and "
	"%ndf__2_ = %N-1 degrees of freedom for the numerator and denominator terms, "
	"respectively.")
NORMAL (U"The returned probability %p will be the %%two-sided% probability")
EQUATION (U"%p = 2 * fisherQ (%f, %ndf__1_, %ndf__2_)")
NORMAL (U"If %s__2_^2 > %s__1_^2 we use 1/%f to determine the probability.")
MAN_END

MAN_BEGIN (U"Covariances: Report multivariate mean difference...", U"djmw", 20090627)
INTRO (U"Reports the probability that the two multivariate means of the selected @@Covariance@s are equal.")
ENTRY (U"Settings")
TERM (U"##Covariances are equal")
DEFINITION (U"determines whether the test is performed as if the two covariance matrices are equal or not.")
ENTRY (U"Algorithm")
NORMAL (U"For equal covariance matrices the test is via Hotelling's T^^2^ as described in @@Morrison (1990)|Morrison (1990,@ page 141). "
	"The test statistic is %F = (%N__1_+%N__2_-%p-1)/((%N__1_+%N__2_-2)%p)\\.c T^^2^, with %p and %N__1_+%N__2_-%p-1 degrees of freedom.")
NORMAL (U"If the covariance matrices are not equal, we apply a correction on the number of degrees of freedom as "
	"proposed by @@Krishnamoorthy & Yu (2004)@. The test statistic in this case is %F = (\\nu-%p+1)/(%p\\nu)\\.c T^^2^, "
	"with %p and \\nu degrees of freedom. Here \\nu is a corrected number of degrees of freedom. ")
NORMAL (U"(The test for unequal covariances simplifies to Welch's approximate solution for the univariate t-test with unequal variances.) ")
MAN_END

MAN_BEGIN (U"Covariances: Report equality", U"djmw", 20090701)
INTRO (U"Reports the probability that the selected @@Covariance@ matrices are equal.")
NORMAL (U"We use the Bartlett test and the Wald test. According to @@Schott (2001)@, "
	"both tests are overly sensitive to violations of normality.")
MAN_END

MAN_BEGIN (U"Covariance: To TableOfReal (random sampling)...", U"djmw", 20101101)
INTRO (U"Generate a @TableOfReal object by random sampling from a multi-variate "
	"normal distribution whose @Covariance matrix is the selected object.")
ENTRY (U"Setting")
TERM (U"##Number of data points")
DEFINITION (U"determines the number of data points that will be generated. Each "
	"data point occupies one row in the generated table.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm proceeds as follows:")
LIST_ITEM (U"1. Diagonalize the covariance matrix: calculate the eigenvalues $v__%i_ and "
	"eigenvectors %#e__%i_ of the %m \\xx %m Covariance matrix. "
	"In general there will also be %m of these. Let #%E be the %m \\xx %m matrix "
	"with eigenvector %#e__%j_ in column %j (%j=1..%m).")
LIST_ITEM (U"2. Generate a vector #x whose elements %x__%k_ equal %x__%k_ = "
	"randomGauss (0, \\Vr (%v__%k_)). "
	"Each  %x__%k_ is a random deviate drawn from a Gaussian distribution with "
	"mean zero and standard deviation equal to the square root of the corresponding "
	"eigenvalue %v__%k_.")
LIST_ITEM (U"3. Rotate back: calculate the vector #y = #%E #x, obtained by multiplying the vector "
	"#x with the matrix #%E.")
LIST_ITEM (U"4. Add the centroid to #y and copy the elements of #y to the corresponding row of "
	"the TableOfReal object.")
LIST_ITEM (U"5. Repeat steps 2, 3 and 4 until the desired number of data points "
	"has been reached.")
LIST_ITEM (U"6. Copy the column labels from the Covariance object to the "
	"TableOfReal object.")
NORMAL (U"In case the covariance matrix is diagonal, the algorithm is much simpler: we can skip "
	"the first and third step.")
MAN_END

MAN_BEGIN (U"Covariance & TableOfReal: Extract quantile range...", U"djmw", 20040225)
INTRO (U"Extract those rows from the selected @TableOfReal object whose @@Mahalanobis "
	"distance@, with respect to the selected @Covariance object, are within the "
	"quantile range.")
MAN_END

MAN_BEGIN (U"Covariance & TableOfReal: To TableOfReal (mahalanobis)...", U"djmw", 20170828)
INTRO (U"Calculate @@Mahalanobis distance@ for the selected @TableOfReal with respect to the "
	"selected @Covariance object.")
ENTRY (U"Setting")
TERM (U"##Use table centroid")
DEFINITION (U"Use the mean vector calculated from the columns in the selected TableOfReal instead of the means in the selected Covariance.")
ENTRY (U"Example")
NORMAL (U"Count the number of items that are within 1, 2, 3, 4 and 5 standard deviations from the mean.")
NORMAL (U"We first create a table with only one column and 10000 rows and fill it with numbers drawn from "
	"a normal distribution with mean zero and standard deviation one. Its covariance matrix, of course, is "
	"one dimensional. We next create a table with Mahalanobis distances.")
CODE (U"n = 100000")
CODE (U"t0 = Create TableOfReal: \"table\", n, 1")
CODE (U"Formula: ~ randomGauss (0, 1)")
CODE (U"c = To Covariance")
CODE (U"selectObject: c, t0")
CODE (U"ts = To TableOfReal (mahalanobis): \"no\"")
CODE (U"")
CODE (U"for nsigma to 5")
	CODE1 (U"selectObject: ts")
	CODE1 (U"extraction = Extract rows where:  ~ self < nsigma")
	CODE1 (U"nr = Get number of rows")
	CODE1 (U"nrp = nr / n * 100")
	CODE1 (U"expect = (1 - 2 * gaussQ (nsigma)) * 100")
	CODE1 (U"writeInfoLine: nsigma, \"-sigma: \", nrp, \"%, \", expect, \"%\"")
	CODE1 (U"removeObject: extraction")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Create ChebyshevSeries...", U"djmw", 20040407)
INTRO (U"A command to create a @ChebyshevSeries from a list of coefficients.")
ENTRY (U"Settings")
TERM (U"##Xmin# and ##Xmax#")
DEFINITION (U"define the domain of the polynomials.")
TERM (U"%Coefficients")
DEFINITION (U"define the coefficients of each @@Chebyshev polynomials|Chebyshev polynomial@. "
	"The coefficient of the polynomial with the highest degree comes last.")
MAN_END

MAN_BEGIN (U"Create ISpline...", U"djmw", 20181224)
INTRO (U"A command to create an @ISpline from a list of coefficients.")
ENTRY (U"Settings")
TERM (U"##Xmin# and ##Xmax#")
DEFINITION (U"define the domain of the polynomial @spline.")
TERM (U"##Degree")
DEFINITION (U"defines the degree of the polynomial @spline.")
TERM (U"##Coefficients")
DEFINITION (U"define the coefficients of the basis polynomials.")
TERM (U"##Interior knots")
DEFINITION (U"define the positions in the domain where continuity conditions are defined.")
ENTRY (U"Behaviour")
NORMAL (U"The number of coefficients and the number of interior knots must satisfy "
	"the following relation:")
EQUATION (U"%numberOfCoefficients = %numberOfInteriorKnots + %degree")
NORMAL (U"")
MAN_END

MAN_BEGIN (U"Create MSpline...", U"djmw", 20040407)
INTRO (U"A command to create an @MSpline from a list of coefficients.")
ENTRY (U"Settings")
TERM (U"##Xmin# and ##Xmax#")
DEFINITION (U"define the domain of the polynomial @spline.")
TERM (U"##Degree")
DEFINITION (U"defines the degree of the polynomial @spline.")
TERM (U"##Coefficients")
DEFINITION (U"define the coefficients of the basis polynomials.")
TERM (U"##Interior knots")
DEFINITION (U"define the positions in the domain where continuity conditions are defined.")
ENTRY (U"Behaviour")
NORMAL (U"The number of coefficients and the number of interior knots must satisfy "
	"the following relation:")
EQUATION (U"%numberOfCoefficients = %numberOfInteriorKnots + %degree + 1")
NORMAL (U"")
MAN_END

MAN_BEGIN (U"Create Polynomial...", U"djmw", 20040407)
INTRO (U"A command to create an @Polynomial from a list of coefficients.")
ENTRY (U"Settings")
TERM (U"##Xmin# and ##Xmax#")
DEFINITION (U"define the domain of the polynomial.")
TERM (U"##Degree")
DEFINITION (U"defines the degree of the basis polynomials.")
TERM (U"##Coefficients")
DEFINITION (U"define the coefficients of the polynomial. The coefficient of the "
	"highest power of %x comes last.")
MAN_END

MAN_BEGIN (U"Create LegendreSeries...", U"djmw", 20040407)
INTRO (U"A command to create a @LegendreSeries from a list of coefficients.")
ENTRY (U"Settings")
TERM (U"##Xmin# and ##Xmax#")
DEFINITION (U"define the domain of the polynomials.")
TERM (U"##Coefficients")
DEFINITION (U"define the coefficients of each @@Legendre polynomials|Legendre polynomial@. "
	"The coefficient of the polynomial with the highest degree comes last.")
MAN_END

MAN_BEGIN (U"Create Sound as gammatone...", U"djmw", 20161013)
INTRO (U"A command to create a @Sound as a @@gammatone@.")
ENTRY (U"Settings")
TERM (U"##Name")
DEFINITION (U"the name of the resulting Sound object.")
TERM (U"##Minimum time (s)# and ##Maximum time (s)#")
DEFINITION (U"the start and end time of the resulting Sound.")
TERM (U"##Sampling frequency (Hz)")
DEFINITION (U"the @@sampling frequency@ of the resulting Sound.")
TERM (U"##Gamma")
DEFINITION (U"determines the exponent of the polynomial.")
TERM (U"##Frequency (Hz)# and ##Bandwidth (Hz)#")
DEFINITION (U"determine the frequency and damping of the cosine wave in the gammatone.")
TERM (U"##Initial phase (radians)")
DEFINITION (U"the initial phase of the cosine wave.")
TERM (U"##Addition factor# (standard value: 0)")
DEFINITION (U"determines the degree of asymmetry in the spectrum of the gammatone. "
	"The zero default value gives a gammatone. A value unequal to zero results in a "
	"so called %gammachirp. A negative value is used in auditory filter modelling to "
	"guarantee the usual direction of filter asymmetry, which corresponds to an upward "
	"glide in instantaneous frequency.")
TERM (U"##Scale amplitudes")
DEFINITION (U"determines whether the amplitudes will be scaled to fit in the range (-1, 1).")
ENTRY (U"Purpose")
NORMAL (U"to create a Sound according to the following formula:")
EQUATION (U"%t^^%\\ga\\--1^ e^^\\--2%\\pi\\.c%bandwidth\\.c%t^ "
	"cos (2%%\\pi\\.cfrequency\\.ct% + %additionFactor\\.cln(%t) + %initialPhase),")
NORMAL (U"The %gammachirp function has a monotonically frequency-modulated carrier (the chirp) with "
	"instantaneous frequency ")
EQUATION (U"%instantaneousFrequency(%t) = %frequency + %additionFactor / (2\\.c\\pi\\.c%t)")
NORMAL (U"and an envelope that is a gamma distribution function. It is a theoretically optimal "
	"auditory filter, in the sense that it leads to minimal uncertainty in the joint time and "
	"scale representation of auditory signal analysis.")
NORMAL (U"For faithful modelling of the inner ear, "
	"@@Irino & Patterson (1997)@ conclude that a value of approximately 1.5 * ERB (%frequency) "
	"is appropriate for %bandwidth. "
	"ERB stands for @@equivalent rectangular bandwidth@. Their formula for ERB is:")
EQUATION (U"ERB(%f) = 6.23 10^^\\--6^ %f^2 + 93.39 10^^\\--3^ %f + 28.52.")
NORMAL (U"To avoid @aliasing in the chirp sound, a sound is only generated during times where the "
	"instantaneous frequency is greater than zero and smaller than the @@Nyquist frequency@.")
MAN_END

MAN_BEGIN (U"Create Sound as Shepard tone...", U"djmw", 20230801)
INTRO (U"One of the commands that create a @Sound.")
ENTRY (U"Settings")
TERM (U"##Name")
DEFINITION (U"the name of the resulting Sound object.")
TERM (U"##Minimum time (s)# and ##Maximum time (s)")
DEFINITION (U"the start and end time of the resulting Sound.")
TERM (U"##Sampling frequency (Hz)")
DEFINITION (U"the @@sampling frequency@ of the resulting Sound.")
TERM (U"##Lowest frequency (Hz)")
DEFINITION (U"the frequency of the lowest component in the tone complex.")
TERM (U"##Number of components")
DEFINITION (U"the number of frequency components in the tone complex.")
TERM (U"##Frequency change (semitones/s)")
DEFINITION (U"determines how many semitones the frequency of each component will change in one second. "
	"The number of seconds needed to change one octave will then be 12 divided by ##Frequency change#. "
	"You can make rising, falling and monotonous tone complexes by choosing a positive, negative or zero value.")
TERM (U"##Amplitude range% (dB)")
DEFINITION (U"determines the relative size in decibels of the maximum and the minimum amplitude of the components in a tone complex. These relative amplitudes will then be 10^^\\--%amplitudeRange/20^. ")
TERM (U"##Octave shift fraction (0-1)")
DEFINITION (U"shifts all frequency components by this fraction at the start. You will probably only need this "
	"if you want to generate static tone complexes as the example script below shows.")
ENTRY (U"Purpose")
NORMAL (U"To create a Sound that is a continuous variant of the sound sequences "
	"used by @@Shepard (1964)@ in his "
	"experiment about the circularity in judgments of relative pitch.")
NORMAL (U"The tone consists of many sinusoidal components whose frequencies "
	"might increase exponentially in time. "
	"All frequencies are always at successive intervals of an octave and sounded simultaneously. "
	"Thus the frequency of each component above the lowest is at each moment in time exactly twice "
	"the frequency of the one just below. The amplitudes are large for the components of intermediate "
	"frequency only, and tapered off gradually to subthreshold levels for the components at the "
	"highest and lowest extremes of frequency.")
NORMAL (U"For a rising tone complex, the Sound is generated according to the following specification:")
EQUATION (U"%s(%t) = \\su__%i=1..%numberOfComponents_ %A__%i_(%t) sin (arg__%i_(%t)), where")
EQUATION (U"arg__%i_(%t) = \\in 2%\\pi f__%i_(%\\ta) %d\\ta , and")
EQUATION (U"f__%i_(%t) = %lowestFrequency \\.c 2^^(%i \\-m 1 + octaveShiftFraction + %t/(12/%frequencyChange\\_ st)^, with")
EQUATION (U"%A__%i_(%t) = 10^^((%L__min_ + (%L__max_ \\-m %L__min_) (1 \\-- cos 2%\\pi%\\te__%i_(%t)) / 2) / 20)^, where,")
EQUATION (U"%L__max_ = 0, %L__min_ = 10^^\\--%amplitudeRange/20^, and,")
EQUATION (U"%\\te__%i_(%t) = 2\\pi log2 (%f(%t) / %lowestFrequency) / %numberOfComponents.")
NORMAL (U"The maximum frequency that can be reached during a sweep by any single tone is:")
EQUATION (U"%maximumFrequency = %lowestFrequency\\.c2^^%numberOfComponents^.")
NORMAL (U"A component that reaches the maximum frequency falls instantaneously to the lowest frequency and then starts rising again.")
NORMAL (U"The absolute @@sound pressure level@ of the resulting sound will not be set, it is only guaranteed that the peak value "
	"is just below 1. You can always scale the intensity with the ##Scale Intensity...# command.")
ENTRY (U"Example")
NORMAL (U"The following script generates 12 static Shepard tone complexes, 1 semitone 'apart', "
	"with a cosine window to temper the abrupt start and finish.")
CODE (U"fadeTime = 0.010")
CODE (U"for i to 12")
	CODE1 (U"fraction = (i-1)/12")
	CODE1 (U"Create Sound as Shepard tone: \"s\" + string$ (i), 0, 0.1, 22050, 4.863, 10, 0, 34, fraction")
	CODE1 (U"Fade in: 0, 0, fadeTime, \"no\"")
	CODE1 (U"Fade out: 0, 0.1, -fadeTime, \"no\"")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Create formant table (Peterson & Barney 1952)", U"djmw", 20080509)
INTRO (U"A command to create a @Table object filled with the "
	"fundamental frequency and the first three formant frequency values from 10 "
	"American-English monophthongal vowels as spoken in a /h\\_ d/ context by 76 speakers "
	"(33 men, 28 women and 15 children). Every vowel was pronounced twice, so that there are "
	"1520 recorded vowels in total.")
ENTRY (U"Table layout")
NORMAL (U"The created table will contain 9 columns:")
TERM (U"Column 1, labelled as %Type")
DEFINITION (U"speaker type: \"m\", \"w\" or \"c\" (for %man, %woman or %child).")
TERM (U"Column 2, labelled as %Sex")
DEFINITION (U"speaker sex: either \"m\" or \"f\" (for %male or %female).")
TERM (U"Column 3, labelled as %Speaker")
DEFINITION (U"speaker id: a number from 1 to 76.")
TERM (U"Column 4, labelled as %Vowel")
DEFINITION (U"the vowel name. The following list gives the vowel in a %%h\\_ d% context word "
	"together with its representation in this column: (%heed, iy), (%hid, ih), "
	"(%head, eh), (%had, ae), (%hod, aa), (%hawed, ao), (%hood, uh), (%%who'd%, uw), "
	"(%hud, ah), (%heard, er).")
TERM (U"Column 5, labelled as %IPA")
DEFINITION (U"the IPA notation for the vowels as defined in @@Peterson & Barney (1952)@. ")
TERM (U"Column 6, labelled as %F0")
DEFINITION (U"the fundamental frequency in Hertz.")
TERM (U"Column 7, 8 and 9, labelled as %F1, %F2 and %F3")
DEFINITION (U"the frequencies in Hertz of the first three formants.")
ENTRY (U"Remarks")
NORMAL (U"We originally downloaded the data from the University of Pennsylvania FTP site, "
	"where they were reportedly based on a printed version supplied by Ignatius Mattingly. ")
NORMAL (U"About the IPA notation. We used the original notation from the Peterson & Barney article. "
	"The notation in @@Watrous (1991)@ differs for three vowels: Watrous uses /e, o, \\er/ where Peterson & Barney use /\\ef, \\ct, \\er\\hr/.")
NORMAL (U"More details about these data and how they were measured can be found in the articles"
	"@@Watrous (1991)@ and in @@Peterson & Barney (1952)@.")
MAN_END

MAN_BEGIN (U"Create formant table (Pols & Van Nierop 1973)", U"djmw", 20020620)
INTRO (U"A command to create a @Table object filled with the frequencies and the levels "
	"of the first three formants from the 12 Dutch monophthong "
	"vowels as spoken in /h\\_ t/ context by 50 male and 25 female speakers.")
ENTRY (U"Table layout")
NORMAL (U"The created table will contain 10 columns")
TERM (U"Column 1, labeled as %Sex")
DEFINITION (U"speaker sex: Either \"m\" or \"f\" (for %male or %female).")
TERM (U"Column 2, labeled as %Speaker")
DEFINITION (U"speaker id: a number from 1 to 75.")
TERM (U"Column 3, labeled as %Vowel")
DEFINITION (U"the vowel name. The following list gives the vowel in [p\\_ t] context words "
	"together with its representation in this column: (%poet, oe), (%paat, aa), "
	"(%poot, oo), (%pat, a), (%peut, eu), (%piet, ie), (%puut, uu), (%peet, ee), "
	"(%put, u), (%pet, e), (%pot, o), (%pit, i).")
TERM (U"Column 4, labeled as %IPA")
DEFINITION (U"the IPA-notation for the vowels")
TERM (U"Column 5, 6 and 7, labeled as %F1, %F2 and %F3")
DEFINITION (U"the frequencies in Hertz of the first three formants.")
TERM (U"Column 8, 9 and 10, labeled as %L1, %L2 and %L3")
DEFINITION (U"the levels in decibel below overall SPL of the first three formants.")
NORMAL (U"More details about these data and how they were measured can be found "
	"in @@Pols et al. (1973)@ and @@Van Nierop et al. (1973)@.")
MAN_END

MAN_BEGIN (U"Create formant table (Weenink 1985)", U"djmw", 20041217)
INTRO (U"A command to create a @Table object filled with the "
	"fundamental frequency and the first three formant frequency values from 12 "
	"Dutch monophthongal vowels as spoken in isolation by 30 speakers "
	"(10 men, 10 women and 10 children). Every vowel was pronounced only once, so that there are "
	"360 recorded vowels in total. A reduced form, with only the formant frequecy values, is also available "
	"as a @@Create TableOfReal (Weenink 1985)...|TableOfReal@.")
ENTRY (U"Table layout")
NORMAL (U"The created table will contain 9 columns:")
TERM (U"Column 1, labelled as %Type")
DEFINITION (U"speaker type: \"m\", \"w\" or \"c\" (for %man, %woman or %child).")
TERM (U"Column 2, labelled as %Sex")
DEFINITION (U"speaker sex: either \"m\" or \"f\" (for %male or %female).")
TERM (U"Column 3, labelled as %Speaker")
DEFINITION (U"speaker id: a number from 1 to 76.")
TERM (U"Column 4, labelled as %Vowel")
DEFINITION (U"the vowel name. The following list gives the vowel in Dutch [p\\_ t] context words "
	"together with its representation in this column: (%poet, oe), (%paat, aa), "
	"(%poot, oo), (%pat, a), (%peut, eu), (%piet, ie), (%puut, uu), (%peet, ee), "
	"(%put, u), (%pet, e), (%pot, o), (%pit, i).")
TERM (U"Column 5, labelled as %IPA")
DEFINITION (U"the IPA notation for the vowels.")
TERM (U"Column 6, labelled as %F0")
DEFINITION (U"the fundamental frequency in Hertz.")
TERM (U"Column 7, 8 and 9, labelled as %F1, %F2 and %F3")
DEFINITION (U"the frequencies in Hertz of the first three formants. ")
NORMAL (U"The formant frequency values have been determined by means of LPC analysis with a "
	"varying prediction order. See @@Weenink (1985)@.")
MAN_END

MAN_BEGIN (U"Create TableOfReal (Pols 1973)...", U"djmw", 19990426)
INTRO (U"A command to create a @TableOfReal filled with the first three formant "
	"frequency values and (optionally) the levels from the 12 Dutch monophthongal "
	"vowels as spoken in /h\\_ t/ context by 50 male speakers.")
NORMAL (U"The first three columns will contain the frequencies in Hz, the next three columns "
	"the levels in decibels below the overall SPL of the measured vowel segment. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL (U"More details about these data and how they were measured can be found in the paper of "
	"@@Pols et al. (1973)@.")
MAN_END

MAN_BEGIN (U"Create TableOfReal (Sandwell 1987)", U"djmw", 20170917)
INTRO (U"A command to create a @TableOfReal filled with the data point of Fig. 2 in the article of @@Sandwell (1987)@.")
NORMAL (U"These arbitrary sampled data points are often used in testing interpolation algorithms.")
SCRIPT (6, 6, U" "
	"tor = Create TableOfReal (Sandwell 1987)\n"
	"Draw scatter plot: 1, 2, 0, 0, -0.5, 10.5, -6, 16, 10, \"no\", \"0\", \"no\"\n"
	"Draw inner box\n"
	"Marks bottom every: 1, 2, \"yes\", \"yes\", \"no\"\n"
	"Marks left every: 1, 4, \"yes\", \"yes\", \"no\"\n"
	"removeObject: tor")
MAN_END

MAN_BEGIN (U"Create TableOfReal (Van Nierop 1973)...", U"djmw", 20041217)
INTRO (U"A command to create a @TableOfReal filled with the first three formant "
	"frequency values and (optionally) the levels from the 12 Dutch monophthongal "
	"vowels as spoken in /h\\_ t/ context by 25 female speakers.")
NORMAL (U"The first three columns will contain the frequencies in Hz, the next three columns "
	"the levels in decibels below the overall SPL of the measured vowel segment. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL (U"More details about these data and how they were measured can be found in the paper of "
	"@@Van Nierop et al. (1973)@.")
MAN_END

MAN_BEGIN (U"Create TableOfReal (Weenink 1985)...", U"djmw", 19990426)
INTRO (U"A command to create a @TableOfReal filled with the first three formant "
	"frequency values from the 12 Dutch monophthongal "
	"vowels as spoken in isolation by either 10 men or 10 women or 10 children.")
NORMAL (U"The three columns will contain the formant frequencies in Hz. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL (U"More details about these data and how they were measured can be found in the paper of "
	"@@Weenink (1985)@.")
MAN_END

MAN_BEGIN (U"Discriminant", U"djmw", 20160128)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type Discriminant represents the discriminant structure of a multivariate "
	"data set, i.e. a %%numberOfObservations%\\xx%%dimension% matrix. Each row in this data set belongs to one of %%numberOfGroups% groups (or %%classes% or %categories%, whatever terminology you prefer). " 
	"A Discriminant can be used as a classifier to discriminate between these %%numberOfGroups% groups.")
ENTRY (U"##Inside a Discriminant")
NORMAL (U"With @@Inspect@, you will see that a Discriminant contains the following data:")
LIST_ITEM (U"##eigen#")
DEFINITION (U"the @@Eigen|eigen@ structure, i.e. the eigenvalues and eigenvectors.")
LIST_ITEM (U"##numberOfGroups")
DEFINITION (U"the number of groups between which we can discriminate.")
LIST_ITEM (U"##groups")
DEFINITION (U"a list of %%dimension%\\xx%%dimension% @SSCP matrices, one for each group, %numberOfGroups in total.")
LIST_ITEM (U"##aprioriProbabilities")
DEFINITION (U"an array with %numberOfGroups apriori probabilities of group membership")
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Discriminant analysis@ tutorial")
LIST_ITEM (U"\\bu @@TableOfReal: To Discriminant@")
NORMAL (U"Drawing")
LIST_ITEM (U"\\bu Draw eigenvalues...")
LIST_ITEM (U"\\bu Draw eigenvector...")
LIST_ITEM (U"\\bu @@Discriminant: Draw sigma ellipses...|Draw sigma ellipses...@")
MAN_END

MAN_BEGIN (U"Discriminant analysis", U"djmw", 20170829)
INTRO (U"This tutorial will show you how to perform discriminant analysis with Praat.")
NORMAL (U"As an example, we will use the dataset from @@Pols et al. (1973)@ "
	"with the frequencies and levels of the first three formants from the 12 "
	"Dutch monophthongal vowels as spoken in /h\\_ t/ context by 50 male speakers. "
	"This data set has been incorporated into "
	"Praat and can be called into play with the @@Create TableOfReal "
	"(Pols 1973)...@ command that can be found under #New \\-> #Tables \\-> ##Data sets from the literature#.")
NORMAL (U"In the list of objects a new TableOfReal object will appear with 6 "
	"columns and 600 rows "
	"(50 speakers \\xx 12 vowels). The first three columns contain "
	"the formant frequencies in Hz, the last three columns contain the levels "
	"of the first three formants "
	"given in decibels below the overall sound pressure level of the measured "
	"vowel segment. Each row is labelled with a vowel label.")
NORMAL (U"Pols et al. use logarithms of frequency values, we will too. Because "
	"the measurement units in the first three columns are in Hz and in the last "
	"three columns in dB, it is probably better to standardize the columns. "
	"The following script summarizes our achievements up till now:")
CODE (U"table = Create TableOfReal (Pols 1973): \"yes\"")
CODE (U"Formula: ~ if col < 4 then log10 (self) else self fi")
CODE (U"Standardize columns")
CODE (U"# change the column labels too, for nice plot labels.")
CODE (U"Set column label (index): 1, \"standardized log (%F__1_)\"")
CODE (U"Set column label (index): 2, \"standardized log (%F__2_)\"")
CODE (U"Set column label (index): 3, \"standardized log (%F__3_)\"")
CODE (U"Set column label (index): 4, \"standardized %L__1_\"")
CODE (U"Set column label (index): 5, \"standardized %L__2_\"")
CODE (U"Set column label (index): 6, \"standardized %L__3_\"")
NORMAL (U"To get an indication of what these data look like, we make a scatter "
	"plot of the "
	"first standardized log-formant-frequency against the second standardized "
	"log-formant-frequency. With the next script fragment you can reproduce the "
	"following picture.")
CODE (U"Select outer viewport: 0, 5, 0, 5")
CODE (U"selectObject: table")
CODE (U"Draw scatter plot: 1, 2, 0, 0, -2.9, 2.9, -2.9, 2.9, 10, \"yes\", \"+\", \"yes\"")
PICTURE (5, 5, drawPolsF1F2_log)
NORMAL (U"Apart from a difference in scale this plot is the same as fig. 3 in the "
	"Pols et al. article.")
ENTRY (U"1. How to perform a discriminant analysis")
NORMAL (U"Select the TableOfReal and choose from the dynamic menu the option "
	"@@TableOfReal: To Discriminant|To Discriminant@. This command is available "
	"in the \"Multivariate statistics\" action button. The resulting Discriminant "
	"object will bear the same name as the TableOfReal object. The following "
	"script summarizes:")
CODE (U"selectObject: table")
CODE (U"discriminant = To Discriminant")
ENTRY (U"2. How to project data on the discriminant space")
NORMAL (U"You select a TableOfReal and a Discriminant object together and choose: "
	"@@Discriminant & TableOfReal: To Configuration...|To Configuration...@. "
	"One of the options of the newly created Configuration object is to draw it. "
	"The following picture shows how the data look in the plane spanned by the "
	"first two dimensions of this Configuration. The directions in this "
	"configuration are the eigenvectors from the Discriminant.")
PICTURE (5, 5, drawPolsDiscriminantConfiguration)
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: table, discriminant")
CODE (U"To Configuration: 2")
CODE (U"Select outer viewport: 0, 5, 0, 5")
CODE (U"Draw: 1, 2, -2.9, 2.9, -2.9, 2.9, 12, \"yes\", \"+\", \"yes\"")
NORMAL (U"If you are only interested in this projection, there also is a shortcut "
	"without an intermediate Discriminant object:  "
	"select the TableOfReal object and choose @@TableOfReal: To Configuration "
	"(lda)...|To Configuration (lda)...@.")
ENTRY (U"3. How to draw concentration ellipses")
NORMAL (U"Select the Discriminant object and choose @@Discriminant: Draw sigma "
	"ellipses...|Draw sigma ellipses...@. In the form you can fill out the "
	"coverage of the ellipse by way of the %%Number of sigmas% parameter. "
	"You can also select the projection "
	"plane. The next figure shows the 1-%\\si concentration ellipses in the "
	"standardized log %F__1_ vs log %F__2_ plane. When the data are multinormally distributed, "
	"a 1-%\\si ellipse will cover approximately 39.3\\%  of the data. "
	"The following code summarizes:")
CODE (U"selectObject: discriminant")
CODE (U"Draw sigma ellipses: 1.0, \"no\", 1, 2, -2.9, 2.9, -2.9, 2.9, 12, \"yes\"")
PICTURE (5, 5, drawPolsF1F2ConcentrationEllipses)
ENTRY (U"4. How to classify")
NORMAL (U"Select together the Discriminant object (the classifier), and "
	"a TableOfReal object (the data to be classified). Next you choose "
	"@@Discriminant & TableOfReal: To ClassificationTable...|To "
	"ClassificationTable@. "
	"Normally you will enable the option %%Pool covariance matrices% and "
	"the pooled covariance matrix will be used for classification.")
NORMAL (U"The ClassificationTable can be converted to a @Confusion object "
	"and its fraction correct can be queried with: "
	"@@Confusion: Get fraction correct@.")
NORMAL (U"In general you would separate your data into two independent sets, "
	"\\s{TRAIN} and \\s{TEST}. You would use \\s{TRAIN} to train the "
	"discriminant classifier and \\s{TEST} to test how well it classifies. "
	"Several possibilities for splitting a dataset into two sets exist. "
	"We mention the @@jackknife@ (\"leave-one-out\") and the "
	"@@bootstrap@ methods (\"resampling\").")
ENTRY (U"5.1 Jackknife classification")
NORMAL (U"The following script summarizes #jackknife classification of the dataset:")
CODE (U"selectObject: table")
CODE (U"numberOfRows = Get number of rows")
CODE (U"for irow to numberOfRows")
	CODE1 (U"selectObject: table")
	CODE1 (U"rowi = Extract rows where: ~ row = irow")
	CODE1 (U"selectObject: table")
	CODE1 (U"rest = Extract rows where: ~ row <> irow")
	CODE1 (U"discriminant = To Discriminant")
	CODE1 (U"plusObject: rowi")
	CODE1 (U"classification = To ClassificationTable: \"yes\", \"yes\"")
	CODE1 (U"if irow = 1")
		CODE2 (U"confusion = To Confusion: \"yes\"")
	CODE1 (U"else")
		CODE2 (U"plusObject: confusion")
		CODE2 (U"Increase confusion count")
	CODE1 (U"endif")
	CODE1 (U"removeObject: rowi, rest, discriminant, classification")
CODE (U"endfor")
CODE (U"selectObject: confusion")
CODE (U"fractionCorrect = Get fraction correct")
CODE (U"appendInfoLine: fractionCorrect, \" (fraction correct, jackknifed \", numberOfRows, \" times).\"")
CODE (U"removeObject: confusion")
ENTRY (U"5.2 Bootstrap classification")
NORMAL (U"The following script summarizes bootstrap classification.")
CODE (U"fractionCorrect = 0")
CODE (U"for i to numberOfBootstraps")
	CODE1 (U"selectObject: table")
	CODE1 (U"resampled = To TableOfReal (bootstrap)")
	CODE1 (U"discriminant = To Discriminant")
	CODE1 (U"plusObject: resampled")
	CODE1 (U"classification = To ClassificationTable: \"yes\", \"yes\"")
	CODE1 (U"confusion = To Confusion: \"yes\"")
	CODE1 (U"fc = Get fraction correct")
	CODE1 (U"fractionCorrect += fc")
	CODE1 (U"removeObject: resampled, discriminant, classification, confusion")
CODE (U"endfor")
CODE (U"fractionCorrect /= numberOfBootstraps")
CODE (U"appendInfoLine: fractionCorrect, \" (fraction correct, bootstrapped \", numberOfBootstraps, \" times).\"")
MAN_END

MAN_BEGIN (U"Discriminant: Draw sigma ellipses...", U"djmw", 20040407)
INTRO (U"A command to draw for each group from the selected @Discriminant an ellipse "
	"that covers part of the multivariate data.")
ENTRY (U"Settings")
TERM (U"##Number of sigmas")
DEFINITION (U"determines the @@concentration ellipse|data coverage@.")
TERM (U"##Discriminant plane")
DEFINITION (U"When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be drawn. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
TERM (U"##Xmin#, ##Xmax#, ##Ymin#, ##Ymax#")
DEFINITION (U"determine the limits of the drawing area.")
TERM (U"##Label size")
DEFINITION (U"determines the size of the labels at the centre of the ellipse. No "
	"labels will be drawn when a value less than or equal to zero is chosen.")
MAN_END

MAN_BEGIN (U"Discriminant: Extract within-group SSCP...", U"djmw", 20020314)
INTRO (U"Extract the @SSCP for group %%index% from the selected @Discriminant "
	"object.")
MAN_END

MAN_BEGIN (U"Discriminant: Extract pooled within-groups SSCP", U"djmw", 20020314)
INTRO (U"Extract the pooled within-group @SSCP from the selected @Discriminant "
	"object.")
MAN_END

MAN_BEGIN (U"Discriminant: Get partial discrimination probability...", U"djmw", 19981102)
INTRO (U"A command to test the selected @Discriminant for the significance of "
	"discrimination afforded by the remaining %n\\--%k eigenvectors after the acceptance of "
	"the first %k eigenvectors.")
ENTRY (U"Details")
NORMAL (U"The test statistic is:")
EQUATION (U"%\\ci^2 = \\--(%degreesOfFreedom\\--(%numberOfGroups+%dimension)/2) ln \\La\\'p, where")
EQUATION (U"%degreesOfFreedom = (%dimension\\--%k)(%numberOfGroups\\--%k\\--1), and, ")
EQUATION (U"\\La\\'p = \\Pi__%j=%k+1..%numberOfEigenvalues_ 1 / (1 + %%eigenvalue[j]%)")
MAN_END

MAN_BEGIN (U"Discriminant: Get contribution of component...", U"djmw", 19981106)
INTRO (U"A command to ask the selected @Discriminant for the contribution of the %j^^th^ "
	"discriminant function (component) to the total variance.")
ENTRY (U"Details")
NORMAL (U"The contribution is defined as:")
EQUATION (U"%%eigenvalue[j]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (U"Discriminant: Get Wilks' lambda...", U"djmw", 20040407)
INTRO (U"A command to ask the selected @Discriminant for the value of Wilks' lamda (a "
	"multivariate measure of group differences over several variables).")
ENTRY (U"Settings")
TERM (U"##From")
DEFINITION (U"the first eigenvalue number from which the value for lambda has to be calculated.")
ENTRY (U"Details")
NORMAL (U"Wilks' lambda is defined as:")
EQUATION (U"%\\La = \\Pi__%i=%from..%numberOfEigenvalues_ 1 / (1 + %eigenvalue[%i])")
NORMAL (U"Because lambda is a kind of %inverse measure, values of lambda which are near zero "
	"denote high discrimination between groups.")
MAN_END

MAN_BEGIN (U"Discriminant: Get concentration ellipse area...", U"djmw", 20040407)
INTRO (U"A command to query the @Discriminant object for the area of the concentration "
	"ellipse of one of its groups.")
ENTRY (U"Settings")
TERM (U"##Number of sigmas")
DEFINITION (U"determines the @@concentration ellipse|data coverage@.")
TERM (U"##Discriminant plane")
DEFINITION (U"When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the area of the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be calculated. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
ENTRY (U"Algorithm")
NORMAL (U"See @@SSCP: Get sigma ellipse area...")
MAN_END

MAN_BEGIN (U"Discriminant: Get confidence ellipse area...", U"djmw", 20040407)
INTRO (U"A command to query the @Discriminant object for the area of the confidence "
	"ellipse of one of its groups.")
ENTRY (U"Settings")
TERM (U"##Discriminant plane")
DEFINITION (U"When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the area of the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be calculated. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
ENTRY (U"Algorithm")
NORMAL (U"See @@SSCP: Get confidence ellipse area...")
MAN_END

MAN_BEGIN (U"Discriminant & PatternList: To Categories...", U"djmw", 20040422)
INTRO (U"A command to use the selected @Discriminant to classify each pattern from the "
	"selected @PatternList into a category.")
NORMAL (U"Arguments as in @@Discriminant & TableOfReal: To ClassificationTable...@.")
MAN_END

MAN_BEGIN (U"Discriminant & SSCP: Project", U"djmw", 20020313)
INTRO (U"A command to project the selected @SSCP object on the eigenspace "
	"defined by the selected @Discriminant object.")
NORMAL (U"Further details can be found in @@Eigen & SSCP: Project@")
MAN_END

MAN_BEGIN (U"Discriminant & TableOfReal: To ClassificationTable...", U"djmw", 20040407)
INTRO (U"A command to use the selected @Discriminant to classify each row from the "
	"selected @TableOfReal. The newly created @ClassificationTable will then contain the posterior "
	"probabilities of group membership.")
ENTRY (U"Settings")
TERM (U"##Pool covariance matrices")
DEFINITION (U"when on, all group covariance matrices are pooled and distances will be determined "
	"on the basis of only this pooled covariance matrix (see below).")
ENTRY (U"Details")
NORMAL (U"The posterior probabilities of group membership %p__%j_ for a vector #x are defined as:")
EQUATION (U"%p__%j_ = %p(%j\\| #%x) = exp (\\--%d__%j_^^2^(#%x) / 2) / "
	"\\su__%k=1..%numberOfGroups_ exp (\\--%d__%k_^^2^(#%x) / 2),")
NORMAL (U"where %d__%i_^^2^ is the generalized squared distance function:")
EQUATION (U"%d__%i_^^2^(#%x) = ((#%x\\--#%\\mu__%i_)\\'p #\\Si__%i_^^-1^ (#%x\\--#%\\mu__%i_) + "
	"ln determinant (#\\Si__%i_)) / 2 \\-- ln %aprioriProbability__%i_")
NORMAL (U"that depends on the individual covariance matrix #\\Si__%i_ and the mean "
	"#%\\mu__%i_ for group %i.")
NORMAL (U"When the covariances matrices are %pooled, the squared distance function can be reduced to:")
EQUATION (U"%d__%i_^^2^(#%x) = ((#%x\\--#%\\mu__%i_)\\'p #\\Si^^-1^ (#%x\\--#%\\mu__%i_) "
	"\\-- ln %aprioriProbability__%i_,")
NORMAL (U"and #\\Si is now the pooled covariance matrix.")
NORMAL (U"The a priori probabilities normally will have values that are related to the number of "
	"%training vectors %n__%i_ in each group:")
EQUATION (U"%aprioriProbability__%i_ = %n__%i_ / \\Si__%k=1..%numberOfGroups_ %n__%k_")
MAN_END

MAN_BEGIN (U"Discriminant & TableOfReal: To Configuration...", U"djmw", 20160119)
INTRO (U"A command to project each row in the selected @TableOfReal onto "
	"a subspace spanned by the eigenvectors of the selected @Discriminant. ")
ENTRY (U"Settings")
TERM (U"##Number of dimensions")
DEFINITION (U"specifies the dimension of the resulting @Configuration. This dimension cannot exceed a maximum dimension that is implicitly determined by the selected Discriminant. When the default value (0) is "
	"given the resulting Configuration will have the maximum dimension as allowed by Discrimininant. "
	"(Technically speaking: the number of eigenvectors (or eigenvalues) in the selected Discriminant is equal to the maximum allowed dimension.)")
ENTRY (U"Precondition")
NORMAL (U"The dimension of the Discriminant and the Configuration must conform in the sense that the number of columns in the TableOfReal and the length of an eigenvector in the Discriminant should be equal.")
NORMAL (U"See also @@Eigen & TableOfReal: Project...@.")
MAN_END

MAN_BEGIN (U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...", U"djmw", 20170828)
INTRO (U"Calculate @@Mahalanobis distance@s for the selected @TableOfReal with respect to one group in the "
	"selected @Discriminant object.")
ENTRY (U"Settings")
TERM (U"##Group label")
DEFINITION (U"defines which group mean to use for the distance calculation.")
TERM (U"##Pool covariance matrices")
DEFINITION (U"when on, use a pooled covariance matrix instead of the group covariance matrix.")
ENTRY (U"Algorithm")
NORMAL (U"See @@Covariance & TableOfReal: To TableOfReal (mahalanobis)...@.")
ENTRY (U"Example")
NORMAL (U"Calculate the number of datapoints that are within the one-sigma elipses of two different groups, i.e. "
	"the number of data points that are in the overlapping area. ")
NORMAL (U"Suppose the group labels are \\o/ and \\yc.")
CODE (U"pols50m = Create TableOfReal (Pols 1973): \"no\"")
CODE (U"Formula: ~ log10 (self)")
CODE (U"discriminant = To Discriminant")
CODE (U"selectObject: pols50m, discriminant")
CODE (U"t1 = To TableOfReal (mahalanobis): \"\\o/\", \"no\"")
CODE (U"selectObject: pols50m, discriminant")
CODE (U"t2 = To TableOfReal (mahalanobis): \"\\yc\", \"no\"")
NORMAL (U"Now we count when both the t1 and t2 values are smaller than 1 (sigma):")
CODE (U"Copy: \"tr\"")
CODE (U"Formula: ~ object [t1] < 1 and object [t2] < 1")
CODE (U"Extract rows where column: 1, \"equal to\", 1")
CODE (U"no = Get number of rows\"")
MAN_END

MAN_BEGIN (U"DTW", U"djmw", 20110603)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type DTW represents the dynamic time warp structure of "
	"two objects.")
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@CC: To DTW...@ (from 2 objects with cepstral coefficients)")
LIST_ITEM (U"\\bu ##Spectrogram: To DTW...# (from 2 Spectrogram objects)")
NORMAL (U"Query:")
LIST_ITEM (U"\\bu @@DTW: Get y time from x time...@")
LIST_ITEM (U"\\bu @@DTW: Get x time from y time...@")
MAN_END

MAN_BEGIN (U"DTW: Draw warp (x)...", U"djmw", 20071204)
INTRO (U"Draws the warp given a time on the \"x-direction\"")
MAN_END

MAN_BEGIN (U"DTW: Find path (band & slope)...", U"djmw", 20120223)
INTRO (U"Finds the optimal path for the selected @DTW that lies within the union of the sakoe-chiba band and local slope limits.")
ENTRY (U"Settings")
TERM (U"##Sakoe-Chiba band (s)#,")
DEFINITION (U"The maximum distance from the start/end of the sound where a path may start/finish.")
TERM (U"##Slope constraint#,")
DEFINITION (U"determines the maximum and minimum local slopes in the optimal path. For example, the constraint "
    "1/3 < slope < 3 forces the path locally after having taken three steps in the same direction direction to take the next step in the other direction, or after having taken two steps in the same direction to take the next step in the diagonal direction. At the same time the global consequences of the \"1/3 < slope < 3\" constraint mandates that the durations of the two domains do not differ by more than a factor of three. ")
NORMAL (U"For more information see the article of @@Sakoe & Chiba (1978)@.")
MAN_END

MAN_BEGIN (U"DTW: Get maximum consecutive steps...", U"djmw", 20050307)
INTRO (U"Get the maximum number of consecutive steps in the chosen direction along the optimal path from the selected @DTW.")
MAN_END

MAN_BEGIN (U"DTW: Get distance (weighted)", U"djmw", 20100628)
INTRO (U"Queries the selected @DTW object for the weighted distance along the minimum path.")
ENTRY (U"Algorithm")
NORMAL (U"If the distance matrix has %%nx% cells along the %%x%-direction, %%ny% cells along the %%y%-direction and the "
	"sum of the distances along the minimum path is %%S%, the weighted distance is given by %%S%/(%nx+%ny). ")
MAN_END

MAN_BEGIN (U"DTW: Get time along path...", U"djmw", 20110603)
INTRO (U"Queries the selected @DTW object for the time along the minimal path "
	"given the time along the \"%x-direction\". This command is deprecated, the new commands for querying are "
	"@@DTW: Get y time from x time...@ and @@DTW: Get x time from y time...@.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time along the %x-direction.")
ENTRY (U"Behaviour")
NORMAL (U"When the %input time is in the interval [%xmin, %xmax], the %returned "
	"time will be in the interval [%ymin, %ymax], where [%xmin, %xmax] and "
	"[%ymin, %ymax] are the domains of the two \"objects\" from which the "
	"DTW-object was constructed."
	"For all other input times we assume that the two object are aligned.")
NORMAL (U"We like to have a \"continuous\" interpretation of time for the quantized times in the %x and "
	"%y direction; we make the path piecewise linear. There are two special cases:")
NORMAL (U"1. The local path is horizontal. We calculate the %y-time from the line that connects the "
	"lower-left position of the leftmost horizontal time block to the upper-right position of the "
	"rightmost horizontal  time block.")
NORMAL (U"2. The local path is vertical. We calculate the %y-time from the line that connects the "
	"lower-left position of the bottommost vertical time block to the upper-right position of the "
	"topmost horizontal time block.")
MAN_END

MAN_BEGIN (U"DTW: Get y time from x time...", U"djmw", 20110603)
INTRO (U"Queries the selected @DTW object for the time along the %y-direction "
	"given the time along the \"%x-direction\". ")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time along the %x-direction.")
ENTRY (U"Behaviour")
NORMAL (U"When the %input time is in the interval [%xmin, %xmax], the %returned "
	"time will be in the interval [%ymin, %ymax], where [%xmin, %xmax] and "
	"[%ymin, %ymax] are the domains of the two \"objects\" from which the "
	"DTW-object was constructed."
	"For input times outside the domain we assume that the two object were aligned.")
NORMAL (U"We like to have a \"continuous\" interpretation of time for the quantized times in the %x and "
	"%y direction; we make the path a piecewise linear monotonically increasing function. "
	"There are special cases:")
NORMAL (U"1. The local path is in the %x-direction only. We calculate the %y-time from the line "
	"that connects the lower-left position of the begin cell of this path to the "
	"upper-right position of the last cell.")
NORMAL (U"2. The local path is in the x-direction only. We calculate the %y-time from the line "
	"that connects the lower-left position of lowest cell to the upper-right position of the "
	"highest cell.")
NORMAL (U"3. A cell is both part of a path in the %x- and the %y-direction. "
	"We then calculate the intersection point of the paths in the %x- and the %y-directions. "
	"The %y-times in this cell are now calculated from the two line segments that originate "
	"from this intersection point.")
MAN_END

MAN_BEGIN (U"DTW: Get x time from y time...", U"djmw", 20110603)
INTRO (U"Queries the selected @DTW object for the time along the %x-direction "
	"given the time along the \"%y-direction\". ")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time along the %y-direction.")
ENTRY (U"Behaviour")
NORMAL (U"The behaviour is like @@DTW: Get y time from x time...@")
MAN_END

MAN_BEGIN (U"DTW: Swap axes", U"djmw", 20050306)
INTRO (U"Swap the x and y-axes of the selected @DTW.")
MAN_END

MAN_BEGIN (U"DTW: To Polygon...", U"djmw", 20120223)
INTRO (U"A command to convert for a selected @DTW the Sakoe-Chiba band and the local slope constraint "
	"to a @Polygon object. The polygon will show the boundaries of the search domain for the optimal path.")
MAN_END

MAN_BEGIN (U"DTW & TextGrid: To TextGrid (warp times)", U"djmw", 20110603)
INTRO (U"Create a new TextGrid from the selected @DTW and @TextGrid by warping the "
	"times from the selected TextGrid to the newly created TextGrid.")
ENTRY (U"Algorithm")
NORMAL (U"First we check whether the y-domain of the DTW and the domain of the TextGrid are "
	"equal. If they are, a new TextGrid is created by copying the selected one. "
	"We then change its domain and make it equal to the x-domain of the DTW. "
	"Then for each tier we change the domain and @@DTW: Get x time from y time...|calculate new times@ by using the path.")
MAN_END

MAN_BEGIN (U"DTW & Sounds: Draw...", U"djmw", 20071204)
INTRO (U"Draws the distances, the path and the sounds for the selected @DTW and the two selected @@Sound|Sounds@.")
MAN_END

MAN_BEGIN (U"DTW & Sounds: Draw warp (x)...", U"djmw", 20071204)
INTRO (U"Draws the warp given a time on the \"x-direction\" for the selected @DTW and the two selected @@Sound|Sounds@.")
MAN_END

MAN_BEGIN (U"Create empty EditCostsTable...", U"djmw", 20120524)
INTRO (U"Creates an empty @@EditCostsTable@.")
ENTRY (U"Settings")
TERM (U"##Name#")
DEFINITION (U"the name of the resulting EditCostsTable object.")
TERM (U"##Number of target symbols#")
DEFINITION (U"the number of different symbols in the target symbol set that you want to give special edit cost values in the EditCostTable. "
	"The number you specify may be smaller than the actual target symbol set size because the EditCostTable has an entry for target symbols "
	"that fall in a %%rest% category. If you don't want to treat any target symbol is a special way you may set this value to 0.")
TERM (U"##Number of source symbols#")
DEFINITION (U"the number of different symbols in the source symbol set that you want to give special edit cost values in the EditCostTable. "
	"The number you specify may be smaller than the actual source symbol set size because the EditCostTable has an entry for source symbols "
	"that fall in a %rest% category. If you don't want to treat any source symbol is a special way you may set this value 0.")
MAN_END

MAN_BEGIN (U"EditCostsTable", U"djmw", 20140509)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"The EditCostsTable determines the %%string edit costs%, i.e. the costs involved in changing one string of "
	"symbols (the %%source%) into another one (the %%target%). "
	"String edit costs are generally divided into %%insertion%, %%deletion% and %%substitution% costs. "
	"The latter terms refer to the operations that may be performed on a source string to transform it to a target "
	"string. For example, to change the source string \"execution\" to the target string \"intention\" we would need "
	"one insertion (i), one deletion (d) and three substitutions (s) as the following figure shows.")
SCRIPT (4, 1.0,  U"target = Create Strings as characters: \"intention\"\n"
"source = Create Strings as characters: \"execution\"\n"
	"selectObject: source, target\n"
	"edt = To EditDistanceTable\n"
	"Draw edit operations\n"
	"removeObject: edt, target, source\n")
NORMAL (U"The figure above was produced with default values for the costs, i.e. the insertion and deletion costs were 1.0 while the "
	"substitution cost was 2.0. The actual edit distance between the target and source strings is calculated by the @@EditDistanceTable@ "
	"which uses an EditCostsTable to access the specific string edit costs. The figure above was produced by the following commands:")
CODE (U"target = Create Strings as characters: \"intention\"")
CODE (U"source = Create Strings as characters: \"execution\"")
CODE (U"plusObject: target")
CODE (U"edt = To EditDistanceTable")
CODE (U"Draw edit operations")
NORMAL (U"The default EditCostsTable which is in every new EditDistanceTable object has only two rows and two columns, "
	"where the cells in this EditCostsTable have the following interpretation:\n")
TERM (U"Cell [1] [2]:")
DEFINITION (U"defines the cost for the insertion of a target symbol in the source string. The default insertion cost is 1.0.")
TERM (U"Cell [2] [1]:")
DEFINITION (U"defines the cost of the deletion of a source symbol. The default value is 1.0.")
TERM (U"Cell [1] [1]:")
DEFINITION (U"defines the cost of substituting a target symbol for a source symbol where the target and source symbols don't match. The default substitution cost is 2.0.")
TERM (U"Cell [2] [2]:")
DEFINITION (U"defines the cost of substituting a target symbol for a source symbol where the target and source symbols do match. The deault value is 0.0.")
ENTRY (U"How to create a non-default EditCostsTable")
NORMAL (U"In general we can define a table for %%numberOfTargets% target symbols and %%numberOfSources% source symbols. These numbers "
	"do not necessarily have to be equal to the number of different symbols that may occur in the target and source strings. They only represent the number of symbols that you like to give special edit costs. "
	"The EditCostTable will provide one extra dimension to accommodate target symbol insertion costs and source symbol deletion costs and another extra dimension to represent other target and source symbols that don't have separate entries and can therefore be treated as one group. "
	"The actual dimension of the table will therefore be (%%numberOfTargets% + 2) \\xx (%%numberOfSources% + 2). This is what the cells in the non-default table mean: ")
LIST_ITEM (U"\\bu The upper matrix part of dimension %%numberOfTargets% \\xx %%numberOfSources% will show at cell [%i] [%j] the costs "
	"of substituting the %i-th target symbol for the %j-th source symbol.")
LIST_ITEM (U"\\bu The first %%numberOfSources% values in row (%%numberOfTargets% + 1) represent the costs of substituting one of the target "
	"symbols from the target %%rest% category for the source symbol in the corresponding column.  The target rest category is the group of "
	"targets that do not belong to the %%numberOfTargets% targets represented in the upper part of the matrix.")
LIST_ITEM (U"\\bu The first %%numberOfTargets% values in the column (%%numberOfSources% + 1) represent the costs of substituting the target "
	"symbol in the corresponding row for one of the source symbols from the source %%rest% category.  The source rest category is the group "
	"of source symbols that do not belong to the %%numberOfSources% source symbols represented in the upper part of the matrix.")
LIST_ITEM (U"\\bu The first %%numberOfSources% cells in the last row represent the deletion cost of the corresponding source symbols.")
LIST_ITEM (U"\\bu The first %%numberOfTargets% cells in the last column represent the insertion costs of the corresponding target symbols.")
LIST_ITEM (U"\\bu Finally the four numbers in the cells at the bottom-right corner have an interpretation analogous to the four numbers in "
	"the basic EditCostTable we discussed above (but now for the %%rest% symbols).")
ENTRY (U"Example")
NORMAL (U"If we extend the basic table with one extra target and one extra source symbol, then the EditCostTable will "
	"be a 3 by 3 table. The numbers in the following table have been chosen to be distinctive and therefore probably "
	"will not correspond to any practical situation.")
CODE (U"   s         ")
CODE (U"t 1.1 1.2 1.3")
CODE (U"  1.4 1.5 1.6")
CODE (U"  1.7 1.8 0.0")
NORMAL (U"By issuing the following series of commands this particular table can be created:")
CODE (U"Create empty EditCostsTable: \"editCosts\", 1, 1")
CODE (U"Set target symbol (index): 1, \"t\"")
CODE (U"Set source symbol (index): 1, \"s\"")
CODE (U"Set insertion costs: \"t\", 1.3")
CODE (U"Set deletion costs: \"s\", 1.7")
CODE (U"Set substitution costs: \"t\", \"s\", 1.1")
CODE (U"Set substitution costs: \"\", \"s\", 1.4")
CODE (U"Set substitution costs: \"t\", \"\", 1.2")
CODE (U"Set costs (others): 1.6, 1.8, 0, 1.5")
NORMAL (U"In the first line we create the (empty) table, we name it %%editCosts% and it creates space for one target "
	"and one source symbol. The next line defines the target symbol which becomes the label of the first row of the table. "
	"Line 3 defines the source symbol which will become the label of the first column of the table. "
	"We next define the insertion and deletion costs, they fill cells [1] [3] and [3] [1], respectively. "
	"Cell [1] [1] is filled by the command in line 6.  The command in line 7 fills cell [2] [1] which defines the cost "
	"of substituting any target symbol unequal to \"t\" for \"s\". The next line fills cell [1] [2] which defines "
	"the substitution costs of \"t\" for any source symbol unequal to \"s\". "
	"Finally, the command in the last line defines the little 2\\xx2 matrix at the bottom-right that "
	"is analogous to the default cost matrix explained above. Therefore cell [2] [2] defines the cost of substituting a "
	"target symbol unequal to \"t\" for a source symbol unequal to \"s\" where the target and source symbols don't match, while cell [3] [3] "
	"defines the costs when they do match. "
	"Cell [3] [2] defines the cost of the deletion of a source symbol unequal \"s\", while cell [2] [3] defines the cost "
	"for  the insertion of a target symbol unequal \"t\" in the source string. ")
ENTRY (U"How to use a special EditCostsTable")
NORMAL (U"After creating the special EditCostsTable you select it together with the EditDistanceTable and issue the command @@EditDistanceTable & EditCostsTable: Set new edit costs|Set new edit costs@. The EditDistanceTable will then find the minimum edit distance based on the new cost values.")
MAN_END

MAN_BEGIN (U"EditDistanceTable", U"djmw", 20140509)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An EditDistanceTable shows the accumulated distances between a target string and a source string. "
	"For example, the accumulated distances between the target string \"intention\" and the source string "
	"\"execution\" can be expressed by the following EditDistanceTable:")
SCRIPT (5, 3.5, U"target = Create Strings as characters: \"intention\"\n"
	"source = Create Strings as characters: \"execution\"\n"
	"selectObject: source, target\n"
	"edt = To EditDistanceTable\n"
	"Draw: \"decimal\", 1, 0\n"
	"removeObject: edt, target, source\n")
NORMAL (U"This figure was created by issuing the following commands:")
CODE (U"target = Create Strings as characters: \"intention\"")
CODE (U"source = Create Strings as characters: \"execution\"")
CODE (U"plusObject: target")
CODE (U"edt = To EditDistanceTable")
CODE (U"Draw: \"decimal\", 1, 0")
NORMAL (U"The target string is always displayed vertically while the source string is displayed horizontally and the origin is at the bottom-left corner of the table. "
	"Each cell of this table, dist[%i, %j], contains the accumulated distance between the first %i characters of the target and the first %j characters of the source. The cells on the path through this table which have the "
	"minimum accumulated cost are shown with boxes around them. Below we will explain how this path is calculated.")
NORMAL (U"The local directional steps in this path show which %%edit operations% we have to perform on the source string symbols to obtain the target string symbols. "
	"Three edit operations exist: (1) %%insertion% of a target symbol in the source string. This happens each time we take a step in the vertical direction along the path. (2) %%deletion% of a symbol in the source string. This happens each time we take a step in horizontal direction along the path. (3) %%substitution% of a source symbol by a target symbol happens at each diagonal step along the path.")
NORMAL (U"If we trace the path from its start at the origin to its end, we see that it first moves up, indicating the insertion of an \"i\" symbol in the source string. "
	"In the next step which is in the diagonal direction, the \"n\" target is substituted for the \"e\" source symbol. Next follows another substitution, \"t\" for \"x\". "
	"The next diagonal step substitutes \"e\" for an identical \"e\". This step is followed by a horizontal step in which the source symbol \"c\" is deleted. "
	"The next diagonal step substitutes an \"n\" for a \"u\". The path now continues in the diagonal direction until the end point and only identical substitutions occur in the last part. The following figure shows these operations more explicitly.")
SCRIPT (4, 1.5,  U"target = Create Strings as characters: \"intention\"\n"
	"source = Create Strings as characters: \"execution\"\n"
	"plusObject: target\n"
	"edt = To EditDistanceTable\n"
	"Draw edit operations\n"
	"removeObject: edt, target, source\n")
NORMAL (U"The value of the accumulated costs in a cell of the table is computed by taking the minimum of the accumulated distances from three possible paths that end in the current cell, i.e. the paths that come from the %%left%, from the %%diagonal% and from %%below%.")
CODE (U"dist[i,j] = min (d__left_, d__diag_, d__below_), ")
NORMAL (U"where ")
CODE (U" d__left _ = dist[i-1,j]   + insertionCost(target[i])")
CODE (U" d__diag _ = dist[i-1,j-1] + substitutionCost(source[j],target[i])")
CODE (U" d__below_ = dist[i,j-1]   + deletionCost(source[j])")
NORMAL (U"Since the calculation is recursive we start at the origin. After calculating the accumulative distances for each cell in the table as based on the algorithm above, the cell at the top-right position will contain the accumulated edit distance. "
	"This distance happens to be 8 for the given example. The value 8 results from using the target-indepent value of 1.0 for the insertion cost, the source-independent value of 1.0 for the deletion costs and a constant value of 2.0 for the substitution costs. "
	"If target and source symbol happen to be equal no costs are assigned, or, equivalently the substitution costs are zero if target and source symbol match. If you want more control over these costs you can create an @@EditCostsTable@ and specify your special costs and then @@EditDistanceTable & EditCostsTable: Set new edit costs|set the new edit costs@.")
NORMAL (U"If during the calculations we also keep track of which of the three cells resulted in the local minimum accumulated distance, we can use this directional "
	"information to backtrack from the cell at the top-right position to the cell at the bottom-right position and obtain the minimum path.")
MAN_END

MAN_BEGIN (U"EditDistanceTable & EditCostsTable: Set new edit costs", U"djmw", 20120522)
INTRO (U"A command available in the dynamic menu if an @@EditDistanceTable@ and an @@EditCostsTable@ are chosen together.")
NORMAL (U"New accumulated cost values will be calculated and a new path based on these values will be calculated.")
MAN_END

MAN_BEGIN (U"Eigen", U"djmw", 20260317)
INTRO (U"One of the @@types of objects@ in Praat. "
	"An object of type #Eigen represents the eigen structure of a square matrix, i.e. it contains "
	"the eigenvalues and their corresponding eigenvectors.")
NORMAL (U"If the eigenvalues are real, which is mostly the case, the eigenvalues and their corresponding eigenvectors "
	"are always sorted. The default sorting order is descending, i.e. from large to small. The sorting order can be "
	"changed to ascending order by using the @@Eigen: Sort...|Sort@ command. ")
NORMAL (U"For complex eigenvalues there is no preferred sorting order. However, because "
	"complex eigenvalues always occur in pairs with equal real and opposite imaginary parts, i.e. %a + %i %b and %a - %i %b, "
	"the one with positive imaginary value always occurs first. ")
ENTRY (U"Inside an Eigen")
NORMAL (U"With @Inspect you will see the following attributes:")
TERM (U"%numberOfEigenvalues")
DEFINITION (U"the number of eigenvalues and eigenvectors")
TERM (U"%dimension")
DEFINITION (U"the dimension of an eigenvector.")
TERM (U"%eigenvalues[1..%numberOfEigenvalues]")
DEFINITION (U"the real part of the eigenvalues.")
TERM (U"%eigenvectors[1..%numberOfEigenvalues] [1..%dimension]")
DEFINITION (U"the real parts of the eigenvectors, stored by row.")
TERM (U"%onlyReals")
DEFINITION (U"is %true if all eigenvalues are real and %false if not.")
TERM (U"%eigenvalues\\_ im[1..%numberOfEigenvalues]")
DEFINITION (U"the imaginary part of the eigenvalues.")
TERM (U"%eigenvectors\\_ im[1..%numberOfEigenvalues] [1..%dimension]")
DEFINITION (U"the imaginary parts of the eigenvectors, stored by row.")
MAN_END

MAN_BEGIN (U"Eigen: Draw eigenvalues...", U"djmw", 20040407)
INTRO (U"A command to draw the eigenvalues of the selected @Eigen object(s).")
ENTRY (U"Settings")
TERM (U"##Fraction of eigenvalues summed")
DEFINITION (U"defines whether or not fractions are plotted. Fractions %f__%i_ "
	"will be calculated for each number %e__%i_ by dividing this number by the sum of all "
	"numbers %e__%j_: %f__%i_ = %e__%i_ / \\su__%j=1..%numberOfEigenvalues_ %e__%j_.")
TERM (U"##Cumulative")
DEFINITION (U"defines whether or not cumulative values are plotted. Cumulative "
	"values %c__%i_ will be calculated for each number %e__%i_ by summing the first %i "
	"numbers %e__%j_: %c__%i_ = \\su__%j=1..%i_ %e__%j_).")
NORMAL (U"A @@Scree plot|scree@ plot can be obtained if both %%Fraction of eigenvalues summed% "
	"and %%Cumulative% are unchecked.")
MAN_END

MAN_BEGIN (U"Eigen: Draw eigenvector...", U"djmw", 20160223)
INTRO (U"A command to draw an eigenvector from the selected @Eigen.")
ENTRY (U"Settings")
TERM (U"##Eigenvector number")
DEFINITION (U"determines the eigenvector to be drawn.")
TERM (U"%Component %loadings")
DEFINITION (U"when on, the eigenvector is multiplied with the square root of the corresponding "
	"eigenvalue. (For @@PCA@-analysis this means that you will draw the so called "
	"%%component loading vector%. You will be able to compare "
	"quantitatively the elements in different component loading vectors because "
	"the %i-th element in the %j-th component loading vector gives the covariance between the %i-th "
	"original variable and the %j-th principal component.)")
TERM (U"##Element range#")
DEFINITION (U"determine the first and last element of the vector that should be drawn.")
TERM (U"##Minimum# and ##Maximum#")
DEFINITION (U"determine the lower and upper bounds of the plot (choosing #Maximum smaller than #Minimum "
	"will draw the %%inverted% eigenvector). ")
TERM (U"##Mark size#, ##Mark string#")
DEFINITION (U"determine size and type of the marks that will be drawn.")
TERM (U"##Garnish")
DEFINITION (U"determines whether a bounding box and margins will be drawn.")
MAN_END

MAN_BEGIN (U"Eigen: Get contribution of component...", U"djmw", 19981109)
INTRO (U"A command to ask the selected @Eigen for the contribution of the %j^^th^ "
	"eigenvalue to the total sum of eigenvalues.")
ENTRY (U"Details")
NORMAL (U"The contribution is defined as:")
EQUATION (U"%%eigenvalue[j]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (U"Eigen: Get cumulative contribution of components...", U"djmw", 19981109)
INTRO (U"A command to ask the selected @Eigen for the contribution of the sum of the "
	"eigenvalues[%from..%to] to the total sum of eigenvalues.")
ENTRY (U"Details")
NORMAL (U"The contribution is defined as:")
EQUATION (U"\\Si__%i=%from..%to_ %%eigenvalue[i]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (U"Eigen: Get eigenvalue...", U"djmw", 20260228)
INTRO (U"A command to query the selected @Eigen for one of its eigenvalues.")
MAN_END

MAN_BEGIN (U"Eigen: Get eigenvector element...", U"djmw", 20040225)
INTRO (U"A command to query the selected @Eigen for the %j^^th^ element of the "
	"%i^^th^ eigenvector.")
MAN_END

MAN_BEGIN (U"Eigen: Get eigenvector...", U"djmw", 20260228)
INTRO (U"A command to query the selected @Eigen for one of its eigenvectors.")
MAN_END

MAN_BEGIN (U"Eigen: Sort...", U"djmw", 20260317)
INTRO (U"A command to sort the eigenvalues and corresponding eigenvectors of the selected @Eigen.")
NORMAL (U"If some of the eigenvalues are complex this command is ineffective.")
ENTRY (U"Settings")
TERM (U"##Sort ascending")
DEFINITION (U"determines the sorting order: if %true the sorting is in ascending order, "
	"if %false the sorting is in descending order.")
MAN_END

MAN_BEGIN (U"Eigen: Extract eigenvector...", U"djmw", 20160617)
INTRO (U"Extract a specified eigenvector from the @Eigen as a @Matrix.")
ENTRY (U"Settings")
TERM (U"##Eigenvector number")
DEFINITION (U"determines the eigenvector.")
TERM (U"##Number of rows")
DEFINITION (U"determines the number of rows of the newly created Matrix. If left 0, the number of rows is determined from the dimension, i.e. the number of elements, of the eigenvector and the #numberOfColumns argument as the %dimension / %numberOfColumns, rounded to the next larger integer.")
TERM (U"##Number of columns")
DEFINITION (U"determines the number of columns of the newly created Matrix. If left 0, the number of columns is determined by from the dimension, i.e. the number of elements, of the eigenvector and the #numberOfRows argument as  %dimension / %numberOfRows, rounded to the next larger integer.\nIf both ##Number of rows# and ##Number of columns# are zero, a Matrix with only one row and %dimension columns will be created.")
ENTRY (U"Examples")
NORMAL (U"Suppose we have an eigenvector of dimension 3 with elements {0.705, 0.424, 0.566}, then the newly created Matrix will depend on the ##Number of rows# and ##Number of columns# argument as follows:")
NORMAL (U"If %numberOfRows=0 and %numberOfColumns=0, then the Matrix will have 1 row and 3 columns:")
CODE (U"0.705 0.424 0.566")
NORMAL (U"If %numberOfRows=3 and %numberOfColumns=0, then the Matrix will have 3 rows and 1 column:")
CODE (U"0.705")
CODE (U"0.424")
CODE (U"0.566")
NORMAL (U"If %numberOfRows=2 and %numberOfColumns=2, then the Matrix will have 2 rows and 2 columns:")
CODE (U"0.705 0.424")
CODE (U"0.566 0.0")
NORMAL (U"If %numberOfRows=4 and %numberOfColumns=3, then the we get:")
CODE (U"0.705 0.424 0.566")
CODE (U"0.0   0.0   0.0 ")
CODE (U"0.0   0.0   0.0 ")
CODE (U"0.0   0.0   0.0 ")
MAN_END

MAN_BEGIN (U"Eigen & Matrix: To Matrix (project columns)...", U"djmw", 20160223)
INTRO (U"A command to project the columns of the @Matrix object onto the "
	"eigenspace of the @Eigen object.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"defines the dimension, i.e., the number of rows, of the "
	"resulting Matrix object.")
ENTRY (U"Algorithm")
NORMAL (U"Project each column of the Matrix on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
EQUATION (U"%y__%ij_ = \\Si__%k=1..%dimension_ %e__ik_ %z__%kj_, where")
NORMAL (U"%y__%ij_ is the %j-th element of the %i-th row of the result, "
	"%e__%ik_ is the %k-th element of the %i-th eigenvector, "
	"%z__%kj_ is the %k-th element of the %j-th column of the selected "
	"Matrix object, and %%dimension% is the number of elements in an eigenvector.")
MAN_END

MAN_BEGIN (U"Eigen & Matrix: To Matrix (project rows)...", U"djmw", 20160223)
INTRO (U"A command to project the rows of the @Matrix object onto the "
	"eigenspace of the @Eigen object.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"defines the dimension, i.e., the number of columns, of the "
	"resulting Matrix object.")
ENTRY (U"Algorithm")
NORMAL (U"Project each row of the Matrix on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
EQUATION (U"%y__%ij_ = \\Si__%k=1..%dimension_ %e__jk_ %z__%ik_, where")
NORMAL (U"%y__%ij_ is the %j-th element of the %i-th row of the result, "
	"%e__%jk_ is the %k-th element of the %j-th eigenvector, "
	"%z__%ik_ is the %k-th element of the %i-th row of the selected "
	"Matrix object, and %%dimension% is the number of elements in an eigenvector.")
MAN_END

MAN_BEGIN (U"Eigen & SSCP: Project", U"djmw", 20020328)
INTRO (U"A command to project the @SSCP object onto the eigenspace of "
	"the @Eigen object.")
ENTRY (U"Behaviour")
NORMAL (U"Transform the SSCP object as if it was calculated in a coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
EQUATION (U"#%S__%t_ = #%E\\'p #%S #%E, where")
NORMAL (U"where #%E\\'p is the transpose of the matrix with eigenvectors #%E, "
	"#%S is the square matrix with sums of squares and crossproducts, and "
	"#%S__%t_ the newly created square matrix. The dimension of #%S__%t_ may "
	"be smaller than the dimension of #%S.")
MAN_END

MAN_BEGIN (U"Eigen & TableOfReal: Project...", U"djmw", 20040407)
INTRO (U"A command to project the rows of the @TableOfReal object onto the "
	"eigenspace of the @Eigen object.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"defines the number of dimensions, i.e., the number of columns, of the "
	"resulting object.")
ENTRY (U"Algorithm")
NORMAL (U"Project each row of the TableOfReal on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
EQUATION (U"%y__%ij_ = \\Si__%k=1..%numberOfColums_ %e__jk_ %x__%ik_, where")
NORMAL (U"%e__%jk_ is the %k-th element of the %j-th eigenvector, %x__%ik_ is "
	"the %k-th element of the %i-th row and %y__%ij_ is the %j-th element at "
	"the %i-th row of the matrix part of the resulting object.")
MAN_END

MAN_BEGIN (U"Electroglottogram", U"djmw", 20190829)
INTRO (U"One of the @@types of objects@ in Praat. The ##Electroglottogram# represents changes in vocal fold contact area during vocal fold vibration.")
ENTRY (U"The Electroglottogram waveform")
NORMAL (U"The following picture shows part of one cycle of a stereotypical (stylized)  waveform, with landmarks.")
PICTURE (5,3, Electroglottogram_drawStylized)
NORMAL (U"The orientation of the signal is in the (now) conventional way where the positive y-direction signals larger %%vocal fold contact area% (VFCA). The landmarks refer to:")
LIST_ITEM (U"\\bu a \\-- initial contact of the lower vocal fold margins;")
LIST_ITEM (U"\\bu b \\-- the upper vocal fold margins make initial (but not full) contact;")
LIST_ITEM (U"\\bu c \\-- maximum vocal fold contact reached;")
LIST_ITEM (U"\\bu d \\-- de-contacting phase initiated by separation of the lower vocal fold margins;")
LIST_ITEM (U"\\bu e \\-- upper margins start to separate;")
LIST_ITEM (U"\\bu f \\-- glottis is open, with minimal contact area.")
ENTRY (U"How to get an Electroglottogram?")
NORMAL (U"From standard electroglottography measurements generally a multi-channel sound file results. One channel of this file contains the recorded electroglottogram, the other the recorded sound. You can extract the electroglottogram with the @@Sound: Extract Electroglottogram...|Extract Electroglottogram...@ command that you will find under the ##Sound: Convert -# menu.")
ENTRY (U"Glottal opening and closure times")
NORMAL (U"Getting exact timing of the %%glottal closure instants%% (GCI) and %%glottal opening "
	"instants% (GOI) from the Electroglottogram is problematic because as @@Herbst (2019)@ "
	"notes: the vocal folds do not vibrate as a uniform mass. Rather, "
	"their vibration is characterized by phase differences along both the inferior–superior and "
	"anterior–posterior dimensions. These phase differences cause time-delayed contacting and "
	"de-contacting of the vocal folds along the respective axes. There is thus no specific instant "
	"of glottal closing and opening, but rather an interval during which the closing and opening, "
	"respectively, occur. ")
MAN_END

MAN_BEGIN (U"Electroglottogram: High-pass filter...", U"djmw", 20190827)
INTRO (U"Applies a high-pass filter to the Electroglottogram to filter away signal artifacts like baseline and amplitude drifts.")
ENTRY (U"Settings")
TERM (U"##From frequency (Hz)#")
DEFINITION (U"Frequencies lower than this frequency are suppressed.")
TERM (U"##Smoothing (Hz)")
DEFINITION (U"defines the width of the transition area between fully suppressed and fully passed "
	"frequencies. Frequencies below %%fromFrequency% will be fully suppressed, frequencies larger "
	"than %%fromFrequency%+%%smoothing% will be fully passed.")
MAN_END

MAN_BEGIN (U"Electroglottogram: First central difference...", U"djmw", 20230323)
INTRO (U"Calculates an approximation of the derivative of the @@Electroglottogram@.")
ENTRY (U"Settings")
TERM (U"##New absolute peak")
DEFINITION (U"defines the absolute peak of the approximate derivative. A value of 0.0 prevents scaling.")
ENTRY (U"Algorithm")
NORMAL (U"We take the first central difference, "
	"(d%%x%(%%t%)/d%%t%)[%%i%] = (%%x%[%%i%+1] - %%x%[%%i%-1])/(2\\De%%t%).")
NORMAL (U"The real derivative can be found by using the @@Electroglottogram: Derivative...|Derivative...@ method.")
MAN_END

MAN_BEGIN (U"Electroglottogram: To TextGrid (closed glottis)...", U"djmw", 20221115)
INTRO (U"Calculates the intervals where the glottis is closed from the selected @@Electroglottogram@.")
ENTRY (U"Settings")
TERM (U"##Pitch floor (Hz)#")
DEFINITION (U"intervals with a lower pitch will not be considered. ")
TERM (U"##Pitch ceiling (Hz)#")
DEFINITION (U"intervals with a higher pitch will not be considered.")
TERM (U"##Closing threshold#")
DEFINITION (U"the moment of closing of the vocal folds will be taken at a fixed point between a cycle's peak and valley amplitude level.")
PICTURE (4.0, 3.0, Electroglottogram_drawStylizedLevels)
DEFINITION (U"The picture shows, for a %%closingThreshold% value of 0.3, the Closed Glottis Interval that starts at time %t__1_ and ends at time %t__2_. These times were found by calculating the two level crossings at amplitude %%valley+closingThreshold(peak\\--valley)%.")
TERM (U"##Peak threshold#")
DEFINITION (U"cycles with peaks whose relative amplitudes with respect to the maximum peak are lower than this value are not considered.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm first tries to find peaks and valleys, guided by the settings for the pitch floor and ceiling. From the level crossings before and after a peak the times of glottal closing and opening are determined.")
ENTRY (U"Warning")
NORMAL (U"Getting exact timing of the %%glottal closure instants%% (GCI) and %%glottal opening "
	"instants% (GOI) from the Electroglottogram is problematic because as @@Herbst (2019)@ "
	"notes: the vocal folds do not vibrate as a uniform mass. Rather, "
	"their vibration is characterized by phase differences along both the inferior–superior and "
	"anterior–posterior dimensions. These phase differences cause time-delayed contacting and "
	"de-contacting of the vocal folds along the respective axes. There is thus no specific instant "
	"of glottal closing and opening, but rather an interval during which the closing and opening, "
	"respectively, occur. ")
MAN_END

MAN_BEGIN (U"Electroglottogram: Derivative...", U"djmw", 20230323)
INTRO (U"Calculates the derivative of the @@Electroglottogram@.")
ENTRY (U"Algorithm")
NORMAL (U"The settings and the algorithm are explained in @@Sound: To Sound (derivative)...@")
ENTRY (U"About dEGG")
NORMAL (U"The derivative of the Electroglottogram is often indicated as dEGG or DEGG. @@Henrich et al. (2004)@ used the peaks in the derivative to find the %%glottal closure instants% and sometimes also the %%glottal opening instants%. "
	"However, in their paper and also in other papers like, for example,  @@Herbst et al. (2014)@, the derivative they use is not the exact derivative as calculated in the way explained above. "
	"Instead they calculate an approximation of the derivative by taking either the first difference, "
	"(d%%x%(%%t%)/d%%t%)[%%i%] = (%%x%[%%i%] - %%x%[%%i%-1])/\\De%%t%, or by taking the first central difference, "
	"(d%%x%(%%t%)/d%%t%)[%%i%] = (%%x%[%%i%+1] - %%x%[%%i%-1])/(2\\De%%t%).")
MAN_END

MAN_BEGIN (U"Electroglottogram: To AmplitudeTier (levels)...", U"djmw", 20190831)
INTRO (U"For the selected @@Electroglottogram@, according to the chosen value of the %%closing threshold%, the amplitude at the moment of glottal closure in each glottal cycle is calculated as a proportion of the difference between the values at the peak and the valley of each cycle.")
ENTRY (U"Settings")
TERM (U"##Pitch floor (Hz)#")
DEFINITION (U"defines the lowest pitch we want to consider.")
TERM (U"##Pitch ceiling (Hz)#")
DEFINITION (U"defines the highest pitch we want to consider.")
TERM (U"##Closing threshold (0-1)#")
DEFINITION (U"defines the relative amplitude in each glottal cycle where the moment of glottal closure will be chosen. ")
MAN_END

MAN_BEGIN (U"electroglottography", U"djmw", 20190929)
INTRO (U"Electroglottography (EGG) is a low-cost, noninvasive technology for measuring changes of relative vocal fold contact area during laryngeal voice production @@Herbst (2019)|(Herbst, 2019)@.")
NORMAL (U"In electroglottography (EGG) a high-frequency, low-amperage current is passed between two "
	"electrodes placed on each side of the thyroid cartilage. Changes in vocal fold contact area "
	"(VFCA) during vocal fold vibration result in admittance variation, and the resulting "
	"(demodulated) EGG signal is proportional to the relative VFCA.")
NORMAL (U"In Praat the EGG signal is represented by the @@Electroglottogram@.")
NORMAL (U"From standard electroglottography measurements generally a multi-channel sound file results. One channel of this file contains the recorded electroglottogram, the other the recorded sound. You can extract the electroglottogram with the @@Sound: Extract Electroglottogram...|Extract Electroglottogram...@ command that you will find under the ##Sound: Convert -# menu.")
MAN_END

MAN_BEGIN (U"equivalent rectangular bandwidth", U"djmw", 19980713)
INTRO (U"The %%equivalent rectangular bandwidth% (ERB) of a filter is defined "
	"as the width of a rectangular filter whose height equals the peak gain of "
	"the filter and which passes the same total power as the filter (given a flat "
	"spectrum input such as white noise or an impulse).")
MAN_END

MAN_BEGIN (U"Excitations", U"djmw", 19960918)
INTRO (U"A collection of objects of type @Excitation. "
	"You can create an #Excitations by selecting one or more #Excitation's and "
	"selecting ##To Excitations#. You can add one or more #Excitation's to an "
	"#Excitations by selecting one #Excitations and one or more "
	"#Excitation's and selecting ##Add to Excitations# (the #Excitation's will "
	"be removed from the list of objects).")
MAN_END

MAN_BEGIN (U"Excitations: Append", U"djmw", 19960918)
INTRO (U"You can choose this command after selecting two objects of type @Excitations. ")
NORMAL (U"A new object is created that contains the second object appended after the first.")
MAN_END

MAN_BEGIN (U"Excitations: To PatternList...", U"djmw", 19960918)
INTRO (U"A command to convert every selected @Excitations to a @PatternList object.")
ENTRY (U"Setting")
TERM (U"##Join")
DEFINITION (U"the number of subsequent @Excitation objects to combine into one row of @PatternList. "
	"E.g. if an #Excitation has length 26 and %join = 2 then each row of #PatternList "
	"contains 52 elements. The number of rows in #PatternList will be %%my size% / 2. "
	"In the conversion process the elements of an #Excitation will be divided by 100.0 in order "
	"to guarantee that all patterns have values between 0 and 1.")
MAN_END

MAN_BEGIN (U"FilterBank: Draw filter functions...", U"djmw", 20030901)
INTRO (U"")
MAN_END

MAN_BEGIN (U"FilterBank: Draw frequency scales...", U"djmw", 20030901)
MAN_END

MAN_BEGIN (U"FilterBank: Get frequency in Hertz...", U"djmw", 20030901)
INTRO (U"A @@Query submenu|query@ to the selected FilterBank object.")
ENTRY (U"Return value")
NORMAL (U"a frequency value in Hertz.")
MAN_END

MAN_BEGIN (U"FilterBank: Get frequency in Bark...", U"djmw", 20030901)
MAN_END

MAN_BEGIN (U"FilterBank: Get frequency in mel...", U"djmw", 20030901)
MAN_END

MAN_BEGIN (U"FormantFilter", U"djmw", 20141022)
INTRO (U"A #deprecated @@types of objects|type of object@ in Praat. It has been replaced by @@Spectrogram@.")
NORMAL (U"An object of type FormantFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t), expressed "
	"in dB as 10*log10(power/4e-10)). In the now preferred Spectrogram the power is represented instead of its dB value. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a linear frequency scale).")
ENTRY (U"Inside a FormantFilter")
NORMAL (U"With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN (U"gammatone", U"djmw", 20100517)
INTRO (U"A gammatone is the product of a rising polynomial, a decaying exponential function, and a "
	"cosine wave.")
NORMAL (U"It can be described with the following formula:")
EQUATION (U"gammaTone (%t) = %a %t^^%\\ga\\--1^ e^^\\--2%\\pi\\.c%bandwidth\\.c%t^ "
	"cos (2%%\\pi\\.cfrequency\\.ct% + %initialPhase),")
NORMAL (U"where %\\ga determines the order of the gammatone.")
NORMAL (U"The gammatone function has a monotone carrier (the tone) with an "
	"envelope that is a gamma distribution function. The amplitude spectrum is essentially "
	"symmetric on a linear frequency scale. This function is used in some time-domain "
	"auditory models to simulate the spectral analysis performed by the basilar membrane. "
	"It was popularized in auditory modelling by @@Johannesma (1972)@. @@Flanagan (1960)@ "
	"already used it to model basilar membrane motion.")
MAN_END

MAN_BEGIN (U"generalized singular value decomposition", U"djmw", 20220111)
INTRO (U"For %m > %n, the %%generalized singular value decomposition% (gsvd) of an %m \\xx %n matrix #%A and "
	"a %p \\xx %n matrix #%B is given by the pair of factorizations")
EQUATION (U"#%A = #%U #%D__1_ [#%0, #%R] #%Q\\'p and #%B = #%V #%D__2_ [#%0, #%R] #%Q\\'p")
NORMAL (U"The matrices in these factorizations have the following properties:")
TERM (U"\\bu #%U [%m \\xx %m], #%V [%p \\xx %p] and #%Q [%n \\xx %n]")
DEFINITION (U" are orthogonal matrices. In the reconstruction formulas above we maximally need "
	"only the first %n columns of matrices #%U and #%V (when %m and/or %p are greater than %n).")
TERM (U"\\bu #%R [%r \\xx %r],")
DEFINITION (U"is an upper triangular nonsingular matrix. %r is the rank of [#%A\\'p, #%B\\'p]\\'p "
	"and %r \\<_ %n. The matrix [#%0, #%R] is %r \\xx %n and its first %n \\xx (%n \\-- %r) part "
	"is a zero matrix.")
TERM (U"\\bu #%D__1_ [%m \\xx %r] and #%D__2_ [%p \\xx %r]")
DEFINITION (U"are real, nonnegative and \"diagonal\".")
NORMAL (U"In practice, the matrices #%D__1_ and #%D__2_ are never used. Instead a shorter "
	"representation with numbers %\\al__%i_ and %\\be__%i_ is used. These numbers obey "
	"0 \\<_ \\al__%i_ \\<_ 1 and \\al__%i_^^2^ + \\be__%i_^^2^ = 1. The following relations exist:")
EQUATION (U"#%D__1_\\'p #%D__1_ + #%D__2_\\'p #%D__2_ = #%I, ")
EQUATION (U"#%D__1_\\'p #%D__1_ = #diag (%\\al__1_^^2^, ..., %\\al__%r_^^2^), and, ")
EQUATION (U"#%D__2_\\'p #%D__2_ = #diag (%\\be__1_^^2^, ..., %\\be__%r_^^2^).")
NORMAL (U"The ratios \\al__%i_ / \\be__%i_ are called the %%generalized singular values% of the "
	"pair #%A, #%B. Let %l be the rank of #%B and %k + %l (= %r) the rank of [#%A\\'p, #%B\\'p]\\'p. "
	"Then the first %k generalized singular values are infinite and the remaining %l are finite. "
	"(When %#B is of full rank then, of course, %k = 0).")
ENTRY (U"Special cases")
NORMAL (U"\\bu If #%B is a square nonsingular matrix, the gsvd of #%A and #%B is equivalent to the "
	"singular value decomposition of #%A #%B^^\\--1^.")
NORMAL (U"\\bu The generalized eigenvalues and eigenvectors of #%A\\'p #%A - %\\la  #%B\\'p #%B "
	"can be expressed in terms of the gsvd. The columns of the matrix #%X, constructed as")
CODE (U"X = Q*( I   0    )")
CODE (U"      ( 0 inv(R) ),")
NORMAL (U"form the eigenvectors. The important eigenvectors, of course, correspond "
	"to the positions where the %l eigenvalues are not infinite.")
MAN_END

MAN_BEGIN (U"Get incomplete gamma...", U"djmw", 20170531)
INTRO (U"Get the value of the @@incomplete gamma function@ for particular values of \\al and %x.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm is taken from @@Kostlan & Gokhman (1987)@.")
MAN_END

MAN_BEGIN (U"incomplete gamma function", U"djmw", 20170531)
INTRO (U"The incomplete gamma function is defined as:")
EQUATION (U"\\Ga(\\al, %x) = \\in__%x_^^\\oo^ %t^^\\al−1^e^^-%t^dt, \\Ga(\\al) = \\Ga(\\al, 0),")
NORMAL (U"where \\al and %x are complex numbers and Re(\\al) > 0.")
NORMAL (U"The complementary incomplete gamma function is defined as:")
EQUATION (U"\\ga(\\al, %x) = \\in__%0_^^%x^ %t^^\\al−1^e^^-%t^dt = \\Ga(\\al)−\\Ga(\\al, %x).")
MAN_END

MAN_BEGIN (U"IDX file format", U"djmw", 20160220)
INTRO (U"The IDX file format is a simple format for vectors and multidimensional matrices of various numerical types. ")
NORMAL (U"The basic format according to %%http://yann.lecun.com/exdb/mnist/% is:")
CODE (U"magic number")
CODE (U"size in dimension 1")
CODE (U"size in dimension 2")
CODE (U"size in dimension 3")
CODE (U"....")
CODE (U"size in dimension N")
CODE (U"data")
NORMAL (U"The magic number is four bytes long. The first 2 bytes are always 0.")
NORMAL (U"The third byte codes the type of the data:")
CODE (U"0x08: unsigned byte")
CODE (U"0x09: signed byte")
CODE (U"0x0B: short (2 bytes)")
CODE (U"0x0C: int (4 bytes)")
CODE (U"0x0D: float (4 bytes)")
CODE (U"0x0E: double (8 bytes)")
NORMAL (U"The fourth byte codes the number of dimensions of the vector/matrix: 1 for vectors, 2 for matrices....")
NORMAL (U"The sizes in each dimension are 4-byte integers (big-endian, like in most non-Intel processors).")
NORMAL (U"The data is stored like in a C array, i.e. the index in the last dimension changes the fastest.")
ENTRY (U"Behaviour")
NORMAL (U"If the storage format indicates that there are more than 2 dimensions, the resulting Matrix accumulates dimensions 2 and higher in the columns. For example, with three dimensions of size n1, n2 and n3, respectively, the resulting Matrix object will have n1 rows and %%n2\\xxn3% columns.")

ENTRY (U"Example")
NORMAL (U"The training and testing data of the MNIST database of handwritten digits at http://yann.lecun.com/exdb/mnist/ is stored in %%compressed% IDX formatted files. ")
NORMAL (U"Reading the uncompressed file %%train-images-idx3-ubyte% available at http://yann.lecun.com/exdb/mnist/ with 60000 images of 28\\xx28 pixel data, will result in a new Matrix object with 60000 rows and 784 (=28\\xx28) columns. Each cell will contain a number in the interval from 0 to 255.")
NORMAL (U"Reading the uncompressed file %%train-labels-idx1-ubyte% with 60000 labels will result in a new Matrix object with 1 row and 60000 columns. Each cell will contain a number in the interval from 0 to 9.")
MAN_END

MAN_BEGIN (U"ISpline", U"djmw", 19990627)
INTRO (U"One of the @@types of objects@ in Praat. ")
NORMAL (U"An object of type ISpline represents a linear combination of basis "
	"i@spline functions. Each basis %ispline is a monotonically increasing "
	"polynomial function of degree %p.")
EQUATION (U"ISpline (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %ispline__%k_(%x)")
MAN_END

MAN_BEGIN (U"Itakura-Saito divergence", U"djmw", 20190619)
INTRO (U"The ##Itakura-Saito divergence# is one of the many measures used to measure the similarity between an object %x and a reference %y.")
NORMAL (U"It is defined as %d(%x|%y)= %x/%y - log(%x/%y) - 1. Only if %x = %y the divergence is zero.")
NORMAL (U"It is called a divergence and not a distance, technically speaking, because it is not symmetric: %d(%x|%y) is not the same as %d(%y|%x).")
NORMAL (U"One of the advantages of the Itakura-Saito divergence is its scale invariance which means that %d(%\\lax|%\\lay)=%d(%x|%y), "
"for any number \\la. This makes it a very suitable measure for the comparison of audio spectra.")
MAN_END

MAN_BEGIN (U"jackknife", U"djmw", 20141101)
INTRO (U"A technique for estimating the bias and standard deviation of an estimate.")
NORMAL (U"Suppose we have a sample #%x = (%x__1_, %x__2_,...%x__n_) and wish to estimate "
	"the bias and standard error of an estimator \\Te. The jackknife "
	"focuses on the samples that leave out one observation at a time: "
	"the %i-th jackknife sample consists of the data with the %i-th observation "
	"removed.")
MAN_END

MAN_BEGIN (U"Kirshenbaum phonetic encoding", U"djmw", 20120413)
INTRO (U"The Kirshenbaum phonetic encoding represents International Phonetic Alphabet symbols using ascii characters. See: http://www.kirshenbaum.net/IPA/ascii-ipa.pdf. The @@eSpeak@ speech synthesizer on which our synthesizer is based accepts this encoding as text input. ")
MAN_END

MAN_BEGIN (U"LAPACK", U"djmw", 20200131)
NORMAL (U"From %%http://www.netlib.org/lapack%:")
DEFINITION (U"\"LAPACK provides routines for solving systems of simultaneous linear equations, least-squares solutions of linear systems of equations, eigenvalue problems and singular value problems. All the necessary matrix factorizations are also provided.\"")
NORMAL (U"LAPACK is written in the computer language FORTRAN. In Praat we use CLAPACK, a version which was automatically translated from FORTRAN to C.")
NORMAL (U"More information can be found in the @@Anderson et al. (1999)|LAPACK's user guide@.")
MAN_END

MAN_BEGIN (U"Legendre polynomials", U"djmw", 19990620)
INTRO (U"The Legendre polynomials %P__%n_(%x) of degree %n are special "
	"orthogonal polynomial functions defined on the domain [-1, 1].")
NORMAL (U"Orthogonality:")
EQUATION (U"__-1_\\in^^1^ %W(%x) %P__%i_(%x) %P__%j_(%x) %dx = \\de__%ij_")
EQUATION (U"%W(%x) = 1    (-1 < x < 1)")
NORMAL (U"They obey certain recurrence relations:")
EQUATION (U"%n %P__%n_(%x) = (2%n \\-- 1) %x %P__%n-1_(%x) \\-- (%n \\-- 1) %P__%n-2_(%x)")
EQUATION (U"%P__0_(%x) = 1")
EQUATION (U"%P__1_(%x) = %x")
NORMAL (U"We may %change the domain of these polynomials to [%xmin, %xmax] by "
	"using the following transformation:")
EQUATION (U"%x\\'p = (2%x \\-- (%xmax + %xmin)) / (%xmax - %xmin).")
NORMAL (U"We subsequently use %P__%k_(%x\\'p) instead of %P__%k_(%x).")
MAN_END

MAN_BEGIN (U"LegendreSeries", U"djmw", 19990620)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type LegendreSeries represents a linear combination of @@Legendre polynomials@ "
	"%P__%k_(%x).")
EQUATION (U"LegendreSeries (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %P__%k_(%x)")
MAN_END

MAN_BEGIN (U"LegendreSeries: To Polynomial", U"djmw", 19990620)
INTRO (U"A command to transform the selected @LegendreSeries object into a @@Polynomial@ object.")
NORMAL (U"We find polynomial coefficients %c__%k_ such that")
EQUATION (U"\\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x^^%k^ = "
	"\\Si__%k=1..%numberOfCoefficients_ %l__%k_ %P__%k_(%x)")
NORMAL (U"We use the recurrence relation for @@Legendre polynomials@ to calculate these coefficients.")
MAN_END

MAN_BEGIN (U"Mahalanobis distance", U"djmw", 20160120)
INTRO (U"The Mahalanobis distance is defined as the distance between a (multidimensional) point and a distribution. "
	"It is the multivariate form of the distance measured in units of standard deviation and is "
	"named after the famous Indian statistician R.P. Mahalanobis (1893 \\-- 1972).")
NORMAL (U"Given a normal distribution with covariance matrix ##S# and mean ##\\mu#, the squared Mahalanobis distance of a point "
	"##x## to the mean of this distribution is given by %d^^2^(##x#)=(##x#-##\\mu#)##\\'p###S#^^-1^(##x#-##\\mu#), where "
	"(##x#-##\\mu#)##\\'p# is the transpose of (##x#-##\\mu#).")
NORMAL (U"The distance formula above says that we have to weigh dimensions according to their covariances. If the covariance matrix ##S# "
	"happens to be diagonal the formula above reduces to %d^^2^(##x#)=\\Si__%i=1_^^N^ (%x__%i_-%\\mu__%i_)^^2^/%\\si__%i_.")
MAN_END

MAN_BEGIN (U"Matrix: Draw distribution...", U"djmw", 20041110)
INTRO (U"A command to draw the distribution histogram of the values in the selected part of a @Matrix.")
ENTRY (U"Settings")
TERM (U"##Horizontal range#, ##Vertical range#")
DEFINITION (U"determine the part of the matrix whose distribution will be drawn.")
TERM (U"##Minimum value#, ##Maximum value#")
DEFINITION (U"determine the range of values that will be considered in the distribution. "
	"To treat all bin widths equally, the range will include the %%Minimum value% and exclude the "
	"%%Maximum value% (see below).")
TERM (U"##Number of bins")
DEFINITION (U"determines the number of bars in the distribution histogram.")
TERM (U"##Minimum frequency#, ##Maximum frequency#")
DEFINITION (U"determine the limits of the vertical axis.")
ENTRY (U"Algorithm")
NORMAL (U"For a particular matrix element %z, the histogram bin number %%i% that will be incremented obeys the following relation:")
EQUATION (U"%%lowerBinBorder%__%i_ \\<_ %z <  %%lowerBinBorder%__%i_+ %%binWidth%,")
NORMAL (U"where")
EQUATION (U"%%binWidth% = (%%maximumValue% - %%minimumValue%)/%%numberOfBins%,")
NORMAL (U"and")
EQUATION (U"%%lowerBinBorder%__%i_ = %%minimumValue% + (%i - 1)\\.c%%binWidth%.")
NORMAL (U"In this way all bins will be based on exactly the same width, as each binning interval includes its lower border "
	"and excludes its upper border "
	"(i.e., each interval is closed to the left and open to the right). ")
MAN_END

MAN_BEGIN (U"Matrix: Solve equation...", U"djmw", 19961006)
INTRO (U"Solve the general matrix equation #A #x = #b for #x.")
NORMAL (U"The matrix #A can be any general %m \\xx %n matrix, #b is a %m-dimensional "
	"and #x a %n-dimensional vector. The @Matrix contains #A as its first %n columns "
	"and #b as its last column. The %n-dimensional solution is returned as a #Matrix "
	"with %n columns.")
NORMAL (U"When the number of equations (%m) is %greater than the number of unknowns (%n) the "
	"algorithm gives the best least-squares solution. If on the contrary you "
	"have %fewer equations than unknowns the solution will not be unique.")
ENTRY (U"Method")
NORMAL (U"Singular value decomposition with backsubstitution. "
	"Zero will be substituted for eigenvalues smaller than %tolerance \\.c "
	"%%maximumEigenvalue% (when the user-supplied %tolerance equals 0.0 a "
	"value of 2.2 \\.c 10^^-16^ \\.c %%numberOfUnknowns% "
	"will be used as %tolerance).")
NORMAL (U"See for more details: @@Golub & van Loan (1996)@ chapters 2 and 3.")
MAN_END

MAN_BEGIN (U"Matrix: To Eigen (special)...", U"djmw", 20260328)
INTRO (U"A command to get the @Eigen structure of a square matrix.")
ENTRY (U"Settings")
TERM (U"##Matrix type#")
DEFINITION (U"determines the matrix type. Symmetric matrices always have real eigenvalues and eigenvectors,"
	" while a general matrix can have complex eigenvalues and eigenvectors. For a general matrix"
	" the complex eigenvalues always occur in pairs with equal real parts and complex parts"
	" with opposite signs. ")
TERM (U"##Number of eigenvalues#")
DEFINITION (U"The number of eigenvalues you need. If you choose the default, 0, the number of eigenvalues"
	" will be equal to the dimension of the square matrix. If the number you choose is less than"
	" the dimension of the matrix then if the sort is %ascending the smallest eigenvalues are selected."
	" If the sort is %descending the largest eigenvalues are selected.")
TERM (U"##Sort ascending")
DEFINITION (U"if true, the eigenvalues and corresponding eigenvectors are sorted from smallest to largest,"
	" if false the values are sorted from largest to smallest. For a general matrix the eigenvalues/vectors"
	" are not sorted.")
MAN_END

MAN_BEGIN (U"Matrix: To NMF (m.u.)...", U"djmw", 20190409)
INTRO (U"A command to get the @@non-negative matrix factorization@ of a matrix by means of a multiplicative update algorithm.")
MAN_END

MAN_BEGIN (U"Matrix: To NMF (ALS)...", U"djmw", 20190409)
INTRO (U"A command to get the @@non-negative matrix factorization@ of a matrix by means of an Alternating Least Squares algorithm.")
MAN_END

MAN_BEGIN (U"Matrix: To NMF (IS)...", U"djmw", 20191025)
INTRO (U"A command to get the @@non-negative matrix factorization@ of a matrix based on the Itakura-Saito distance as was described in @@Févotte, Bertin & Durrieu (2009)@.")
MAN_END

MAN_BEGIN (U"MelFilter", U"djmw", 20141022)
INTRO (U"A #deprecated @@types of objects|type of object@ in Praat. It has been replaced by the @@MelSpectrogram@.")
NORMAL (U"An object of type MelFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t), "
	"expressed in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a Mel frequency scale).")
NORMAL (U"The frequency in mels is:")
EQUATION (U"mels = 2595 * log10 (1 + hertz / 700),")
NORMAL (U"and its inverse is:")
EQUATION (U"hertz = 700 * (10.0^^mel / 2595.0^ - 1).")
MAN_END

MAN_BEGIN (U"MelSpectrogram", U"djmw", 20141209)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type MelSpectrogram represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t)."
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a Mel frequency scale).")
NORMAL (U"The mel frequency scale is defined as:")
EQUATION (U"mel = 2595 * log10 (1 + hertz / 700),")
NORMAL (U"and its inverse is:")
EQUATION (U"hertz = 700 * (10.0^^mel / 2595.0^ - 1).")
ENTRY (U"Inside a MelSpectrogram")
NORMAL (U"With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN (U"MelSpectrogram: Paint image...", U"djmw", 20141023)
INTRO (U"A command to draw the selected @MelSpectrogram into the @@Picture window@ in shades of grey.")
MAN_END

MAN_BEGIN (U"MelSpectrogram: To MFCC...", U"djmw", 20141023)
INTRO (U"A command to create a @MFCC object from each selected @MelSpectrogram "
	"object.")
NORMAL (U"Mel frequency cepstral coefficients %c__%k_ in each frame of the MFCC object result from the output of a Discrete Cosine "
	"Transform on spectral values %P__%j_ in the corresponding frame of the MelSpectrogram. The following formula "
	"shows the relation between the values in each frame:")
EQUATION (U"%c__%k-1_ = \\Si__%j=1_^^%N^ %P__%j_ cos (\\pi(%k-1)(%j-0.5)/%N)),")
NORMAL (U"where %N represents the number of spectral values and %P__%j_ the power in dB "
	"of the %j^^%th^ spectral value (%k runs from 1 to %N).")
NORMAL (U"This transformation was first used by @@Davis & Mermelstein (1980)@.")
MAN_END

MAN_BEGIN (U"MFCC: To TableOfReal...", U"djmw", 20120504)
INTRO (U"Convert the selected @@MFCC@ object to a @@TableOfReal@ object. Each MFCC frame results "
	"in one row in the TableOfReal. If the \"Include energy\" option is chosen, the zeroth MFCC "
	"coefficient will be saved in the first column.")
MAN_END

MAN_BEGIN (U"MSpline", U"djmw", 19990627)
INTRO (U"One of the @@types of objects@ in Praat. ")
NORMAL (U"An object of type MSpline represents a linear combination of basis "
	"m@spline functions. Each basis %mspline is a positive polynomial function "
	"of degree %p.")
EQUATION (U"MSpline (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %mspline__%k_(%x)")
MAN_END

MAN_BEGIN (U"NMF", U"djmw", 20190312)
INTRO (U"An object of type ##NMF# represents the @@non-negative matrix factorization@ of a matrix.")
MAN_END

MAN_BEGIN (U"non-negative matrix factorization", U"djmw", 20230801)
INTRO (U"The ##non-negative matrix factorization## or ##NMF# is a factorization of a data matrix #%D, whose elements are all non-negative, into a feature matrix #%F and a weights matrix #%W such that #%D \\~~ #%F #%W, where the elements of #%F and #%W are also all non-negative.")
ENTRY (U"Algorithms for computing NMF")
NORMAL (U"More background on the algorithms used can be found in @@Berry et al. (2007)@")
NORMAL (U"The algorithms fall into three general classes:")
TERM (U"##1. Multiplicative updates#,")
TERM (U"##2. Alternating Least squares#,")
TERM (U"##3. Projected Gradient.#")
ENTRY (U"Multiplicative Updates")
CODE (U"initialize F and W")
CODE (U"while iter < maxinter and not convergence")
	CODE1 (U"(MU) W = W .* (F'*D) ./ (F'*F*W + 10^^−9^)")
	CODE1 (U"(MU) F = F .* (D*W') ./ (F*W*W' + 10^^−9^)")
	CODE1 (U"test for convergence")
CODE (U"endwhile")
NORMAL (U"In the multiplicative update (MU) steps above \"*\" means ordinary matrix multiplication while \".*\" and \"./\" mean elementwise matrix operations. The factors 10^^-9^ guard against division by zero.")
ENTRY (U"Alternating Least Squares")
NORMAL (U"The optimization of ##D \\~~ F*W# is not convex in both ##F# and ##W# it is convex in either ##F# or ##W#. Therefor given one, "
	"the other can be found by a simple least squares (LS) algorithm. This can be done in an alternating fashion.")
NORMAL (U"The Aternating Least Squares (ALS) algorithm is as follows:")
CODE (U"initialize F")
CODE (U"while iter < maxinter and not convergence")
	CODE1 (U"(LS) Solve for W in matrix equation F'*F*W = F'*D")
	CODE1 (U"(NONNEG) Set all negative elements in W to 0")
	CODE1 (U"(LS) Solve for F in matrix equation W*W'*F' = W*D'")
	CODE1 (U"(NONNEG) Set all negative elements in F to 0")
	CODE1 (U"test for convergence")
CODE (U"endwhile")
NORMAL (U"")
MAN_END

MAN_BEGIN (U"pairwise algorithm for computing sample variances", U"djmw", 20170806)
INTRO (U"An algorithm to compute the mean and the variance of an array of numbers. By pairwise combining array elements, "
	"the total number of arithmetic operations is reduced and therefore also the noise due to finite precision arithmetic. "
	"The algorithm is described in @@Chan, Golub & LeVeque (1979)@ and a comparison with other algorithms is presented in "
	"@@Chan, Golub & LeVeque (1983)@.")
MAN_END

MAN_BEGIN (U"PatternList", U"djmw", 20160524)  // 2026
INTRO (U"One of the @@types of objects@ in Praat.")
INTRO (U"An object of type PatternList is a list of patterns that can serve as "
	"inputs for a neural net. All elements in a PatternList have to be in the interval [0, 1].")
ENTRY (U"PatternList commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu ##Create PatternList with zeroes...#")
LIST_ITEM (U"\\bu @@TableOfReal: To PatternList and Categories...@")
NORMAL (U"Synthesis:")
LIST_ITEM (U"\\bu @@FFNet & PatternList: To Categories...@")
LIST_ITEM (U"\\bu @@PatternList & Categories: To FFNet...@")
ENTRY (U"Inside a PatternList")
NORMAL (U"With @Inspect you will see that this type contains the same "
	"attributes as a @Matrix.")
MAN_END

MAN_BEGIN (U"PCA", U"djmw", 19990323)
INTRO (U"One of the @@types of objects@ in Praat. "
	"See the @@Principal component analysis@ tutorial.")
NORMAL (U"An object of type PCA represents the principal components analysis "
	"of a multivariate dataset.")
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Principal component analysis@ tutorial")
LIST_ITEM (U"\\bu @@TableOfReal: To PCA@")
ENTRY (U"Inside a PCA")
NORMAL (U"With @Inspect you will see that this type contains the same "
	"attributes as an @Eigen with the following extras:")
TERM (U"%numberOfObservations")
DEFINITION (U"the number of observations in the multivariate dataset that originated the PCA, "
	"usually equal to the dataset's number of rows.")
TERM (U"%labels[1..%dimension]")
DEFINITION (U"the label that corresponds to each dimension.")
TERM (U"%centroid")
DEFINITION (U"the centroids of the originating multivariate data set.")
MAN_END

MAN_BEGIN (U"PCA: Get fraction variance accounted for...", U"djmw", 19990106)
INTRO (U"A command to query the selected @PCA for the fraction %%variance "
	"accounted for% by the selected components.")
ENTRY (U"Setting")
TERM (U"##Principal component range")
DEFINITION (U"defines the range of the principal components. If you choose both numbers equal, "
	"you get the fraction of the \"variance\" explained by that one component.")
ENTRY (U"Details")
NORMAL (U"The contribution is defined as:")
EQUATION (U"\\Si__%i=%from..%to_ %%eigenvalue[i]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (U"PCA: Get eigenvalue...", U"djmw", 20040225)
INTRO (U"A command to query the selected @PCA for the %i^^th^ "
	"eigenvalue.")
MAN_END

MAN_BEGIN (U"PCA: Get eigenvector element...", U"djmw", 20040225)
INTRO (U"A command to query the selected @PCA for the %j^^th^ element of the "
	"%i^^th^ eigenvector.")
MAN_END

MAN_BEGIN (U"PCA: Get equality of eigenvalues...", U"djmw", 19981102)
INTRO (U"A command to get the probability that some of the eigenvalues of the "
	"selected @PCA object are equal. A low probability means that it is not "
	"very likely that that these numbers are equal.")
NORMAL (U"We test the hypothesis %H__0_: %\\la__%from_ = ... = %\\la__%to_ "
	"that %r (= %to\\--%from+1) of the eigenvalues \\la of the covariance "
	"matrix are equal. The remaining eigenvalues are unrestricted as to their "
	"values and multiplicities. The alternative hypothesis to %H__0_ is that "
	"some of the eigenvalues in the set are distinct.")
ENTRY (U"Settings")
TERM (U"##Eigenvalue range")
DEFINITION (U"define the range of eigenvalues to be tested for equality.")
TERM (U"##Conservative test")
DEFINITION (U"when on, a more conservative estimate for %n is chosen (see below).")
ENTRY (U"Details")
NORMAL (U"The test statistic is:")
EQUATION (U"\\ci^2 = \\--%n \\Si__%j=%from..%to_ ln %eigenvalue[%j] + %n %r "
	"ln (\\Si__%j=%from..%to_ %eigenvalue[%j] / %r),")
NORMAL (U"with %r(%r+1)/2 \\--1 degrees of freedom. Here %n = %totalNumberOfCases \\-- 1.")
NORMAL (U"A special case occurs when the variation in the last %r dimensions is spherical. In a "
	"slightly more conservative test we may replace %n by %n\\--%from\\--(2%r^2+%r+2)/6%r.")
NORMAL (U"Also see @@Morrison (1990)@, page 336.")
MAN_END

MAN_BEGIN (U"PCA: Get number of components (VAF)...", U"djmw", 19990111)
INTRO (U"A command to ask the selected @PCA for the minimum number of "
	"components that are necessary "
	"to explain the given fraction %%variance accounted for%.")
ENTRY (U"Setting")
TERM (U"##Variance accounted for (fraction)")
DEFINITION (U"the fraction variance accounted for that should be explained.")
MAN_END

MAN_BEGIN (U"PCA: To TableOfReal (reconstruct 1)...", U"djmw", 20030108)
INTRO (U"A command to reconstruct a single data item. The result is stored as "
	"a @TableOfReal with only one row.")
ENTRY (U"Settings")
TERM (U"##Coefficients")
DEFINITION (U"the weight for the eigenvectors.")
NORMAL (U"The algorithm is explained in @@PCA & Configuration: To TableOfReal "
	"(reconstruct)@.")
MAN_END

MAN_BEGIN (U"PCA & Configuration: To TableOfReal (reconstruct)", U"djmw", 20030108)
INTRO (U"A command to reconstruct a @TableOfReal from the selected @Configuration"
	" and @PCA.")
NORMAL (U"The TableOfReal is reconstructed from the eigenvectors of the PCA and "
	"elements of the Configuration are the weight factors: ")
EQUATION (U"%#t__%i_ = \\Si__%k_ %c__%ik_ #%e__%k_,")
NORMAL (U"where %#t__%i_ is the %i-th row in the resulting TableOfReal object, %c__%ik_ is "
	"the element at row %i and column %k in the Configuration object and #%e__%k_ "
	"the %k-th eigenvector from the PCA object.")
MAN_END

MAN_BEGIN (U"PCA & PCA: Get angle between pc1-pc2 planes", U"djmw", 20220111)
INTRO (U"A command to calculate the angle between two planes. Each plane is spanned by the first "
	"two eigenvectors from the corresponding @@PCA@.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm is described in section 12.4.3 of @@Golub & van Loan (1996)@:")
NORMAL (U"First we form the projection of one set of eigenvectors on the other set. "
	"This results in a 2\\xx2 matrix #C:")
EQUATION (U"#C = #E__1_\\'p #E__2_,")
NORMAL (U"where #E__1_ and #E__2_ are 2\\xx%%dimension% and %%dimension%\\xx2 matrices "
	"that contain the first two eigenvectors of the PCA's, respectively.")
NORMAL (U"Next, we compute the @@singular value decomposition@ of #C:")
EQUATION (U"#C = #U #D #V\\'p")
NORMAL (U"Now the cosine of the angle between the two planes is given by the second singular value \\si__2_ and "
	"the angle in degrees is therefore:")
EQUATION (U"arccos (\\si__2_)\\.c180/\\pi")
MAN_END

MAN_BEGIN (U"PCA & PCA: To Procrustes...", U"djmw", 20041028)
INTRO (U"A command to calculate a @Procrustes from the two selected @@PCA@'s.")
NORMAL (U"Determines the orthogonal @@Procrustes transform@.")
NORMAL (U"Algorithm 12.4.1 in @@Golub & van Loan (1996)@.")
MAN_END

MAN_BEGIN (U"PCA & TableOfReal: To Configuration...", U"djmw", 19990111)
INTRO (U"A command to construct a @Configuration from the selected @TableOfReal"
	" and @PCA.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"determines the number of dimensions of the resulting Configuration.")
ENTRY (U"Algorithm")
NORMAL (U"The TableOfReal is projected on the eigenspace of the PCA, i.e., "
	"each row of the TableOfReal is treated as a vector, and the inner product "
	"with the eigenvectors of the PCA determine its coordinates in the Configuration.")
NORMAL (U"Because the algorithm performs a projection, the resulting Configuration will "
	"##only be centered#, i.e., its centroid will be at ##0#, if the data in the "
	"TableOfReal object are centered too. ")
NORMAL (U"See also @@Eigen & TableOfReal: Project...@.")
MAN_END

MAN_BEGIN (U"PCA & TableOfReal: To TableOfReal (z-scores)...", U"djmw", 20211129)
INTRO (U"A command to construct a @TableOfReal with z-scores from the selected @TableOfReal"
	" and @PCA.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"determines how many eigenvectors to use in the calculation of the z-scores and therefore the number "
	"of columns of the resulting TableOfReal.")
ENTRY (U"Algorithm")
NORMAL (U"The values %d__%ij_ in the new TableOfReal are calculated as")
EQUATION (U"%d__%ij_ = ##eigenvector#__j_\\.c ##z#__%i_,")
NORMAL (U"which is the inproduct of the %j-th eigenvector and the z-score vector ##z#__%i_ of the %i-th row whose elements %z__%ij_ are defined as")
EQUATION (U"%z__%ij_ = (%x__%ij_ - x\\-^__%j_) / sqrt (eigenvalue__%j_),")
NORMAL (U"in which %x__%ij_ is the data value at row %i and column %j of the selected TableOfReal and x\\-^__%j_ is the "
	"%j-th centroid value of the PCA. The square root of the %j-th eigenvalue is the standard deviation in "
	" the %j-th principal direction.")
MAN_END

MAN_BEGIN (U"PCA & TableOfReal: Get fraction variance...", U"djmw", 20040324)
INTRO (U"A command to query the selected @PCA and @TableOfReal object for the explained "
	"fraction of the variance if the TableOfReal object were projected onto the PCA space.")
ENTRY (U"Algorithm")
LIST_ITEM (U"1. The TableOfReal is @@TableOfReal: To Covariance|converted@ to a "
	"Covariance object.")
LIST_ITEM (U"2. The Covariance object is @@PCA & Covariance: Project|projected@ on "
	"the PCA eigenspace and the newly obtained projected Covariance object is "
	"@@Covariance: Get fraction variance...|queried@ for the fraction variance.")
MAN_END

MAN_BEGIN (U"PitchTier: To Pitch...", U"djmw", 20061128)
INTRO (U"Synthesizes a new @Pitch from the selected @PitchTier.")
MAN_END

MAN_BEGIN (U"PitchTier: Modify interval...", U"djmw", 20230801)
INTRO (U"Modifies a selected interval from the chosen @PitchTier by replacing the contents of the interval by newly defined pitch points.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (11), U""
	Manual_DRAW_SETTINGS_WINDOW (U"PitchTier: Modify interval", 11)
	Manual_DRAW_SETTINGS_WINDOW_RANGE (U"Time range (s)", U"0.0", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD (U"Relative times", U"0.0 0.5 1.0")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"...are...", U"fractions")
	"Text... 50 left y half ...of the interval duration which will be added...\n" \
	"y += 40\n" \
	"Text... 50 left y half ...to the start time of the interval.\n" \
	"y += 40\n" \
	Manual_DRAW_SETTINGS_WINDOW_FIELD (U"The \"pitch\" values", U"100.0 200.0 100.0")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"...are...", U"frequencies")
	"Text... 50 left y half ...to be added to the anchor value (if used)...\n" \
	"y += 40\n" \
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"...which is the...",U"not used")
	"Text... 50 left y half ...frequency value in the interval.\n" \
	"y += 40\n" \
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"Pitch frequency unit",U"Hertz")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start and end time of the interval where the changes will be applied.")
TERM (U"##Relative times")
DEFINITION (U"determine, together with the following option, the times of the new pitch points with respect to the start time of the interval.")
TERM (U"##...are...")
DEFINITION (U"determines how the times %t__%i_ of the new pitch points are calculated. The time of each new pitch point is determined "
	"by adding to the start time of the interval a time calculated from the relative time value. If %%t%__min_ and %%t%__max_ are "
	"the start and end time of the interval and %%r%__i_ is the %%i%^^th^ relative time, the times %t__%i_ are calculated according "
	"to the options as:")
TERM1 (U"%%fractions%")
DEFINITION1 (U"%%t%__%i_ = %t__min_ + %r__%i_ (%t__max_ \\-- %t__min_). The relative time values are fractions of the interval duration. Normally fractions are numbers in the range 0.0 to 1.0, although smaller and larger numbers are allowed.")
TERM1 (U"%%percentages%")
DEFINITION1 (U"%%t%__%i_ = %t__min_+ 0.01 %r__%i_ (%t__max_ \\-- %t__min_). The relative time values are percentages of the interval duration. Normally percentages are numbers in the range 0.0 to 100.0, although smaller and larger numbers are allowed.")
TERM1 (U"%%independent%")
DEFINITION1 (U"%%t%__%i_ = %t__min_ + %r__%i_. The relative time values specify an offset in seconds here. ")
TERM (U"##The \"pitch\" values")
DEFINITION (U"determine, together with the next two options, the frequency value of the new pitch points. Each value here must link to the corresponding time value.")
TERM (U"##...are...")
DEFINITION (U"determines the interpretation of the \"pitch\" value. Possible choices are")
TERM1 (U"%%frequencies%")
DEFINITION1 (U"the values are frequencies in hertz.")
TERM1 (U"%%fractions%")
DEFINITION1 (U"the values are fractions of a pitch value that is specified by the next option. Normally fractions are numbers in "
	"the range 0.0 to 1.0, although smaller and larger numbers are allowed.")
TERM1 (U"%%percentages%")
DEFINITION1 (U"the values are percentages of a pitch value that is specified by the next option. Normally percentages are numbers "
	"in the range 0.0 to 100.0, although smaller and larger numbers are allowed.")
TERM1 (U"%%start and slopes%")
DEFINITION1 (U"the values are a start frequency followed by slopes in Herz per second.")
TERM1 (U"%%slopes and end%")
DEFINITION1 (U"the values are slopes in herz per second followed by an end frequency in herz.")
TERM1 (U"%%music notes%")
DEFINITION1 (U"the values are music notes specified on the twelve tone scale as a0, a\\# 0, b0, c0, c\\# 0, d0, d\\# 0, e0, f0, "
	"f\\# 0, g0, g\\# 0, a1, a\\# 1, ... a4, ..., or g\\# 9. Here the octave is indicated by the number, 0 being the lowest octave "
	"and 9 the highest. The a4 is chosen to be at 440 Hz. Therefore, a0 is the note with the lowest frequency, four octaves below "
	"the a4 and corresponds to a frequency of 27.5 Hz. As a scale of reference we give a0 = 27.5 Hz, a1 = 55 Hz, a2 = 110 Hz, "
	"a3 = 220 Hz, a4 = 440 Hz, a5 = 880 Hz, a6 = 1760 Hz, a7 = 3520 Hz, a8 = 7040 Hz and a9 = 14080 Hz.")
TERM (U"##...which is the...")
DEFINITION (U"the anchor point value, if used. The following options may be given for the anchor point frequency value:")
TERM1 (U"%%not used%")
DEFINITION1 (U"no anchor point frequency value is necessary. The previous two options are sufficient to determine the new pitch "
	"frequencies. This means that the \"pitch\" values given cannot be %%fractions% or %%percentages%.")
TERM1 (U"%%current%")
DEFINITION1 (U"the current pitch frequency at the corresponding time.")
TERM1 (U"%%start%")
DEFINITION1 (U"the pitch frequency at the start of the interval.")
TERM1 (U"%%end%")
DEFINITION1 (U"the pitch frequency at the end of the interval.")
TERM1 (U"%%mean of the curve%")
DEFINITION1 (U"the @@PitchTier: Get mean (curve)...|mean of the curve@ within the interval.")
TERM1 (U" %%mean of the points%")
DEFINITION1 (U"the @@PitchTier: Get mean (points)...|mean of the points@ within the interval.")
TERM1 (U"%%maximum%")
DEFINITION1 (U"the maximum pitch frequency in the interval.")
TERM1 (U"%%minimum%")
DEFINITION1 (U"the minimum pitch frequency in the interval.")
TERM (U"##Pitch frequency unit")
DEFINITION (U"Hertz")
MAN_END

MAN_BEGIN (U"PitchTier: Modify interval (tone levels)...", U"djmw", 20170801)
INTRO (U"Modifies a selected interval from the chosen @PitchTier by replacing the contents of the interval by newly defined pitch points.")
NORMAL (U"For tone languages the pitch contours of the tones are often expressed as a sequence of tone levels instead of a sequence of real frequency values in hertz because tone levels abstract away from the possibly different pitch ranges of individual speakers.")
NORMAL (U"The tone levels %T are calculated from a given pitch %%frequency% in hertz as:")
EQUATION (U"%T = %%numberOfToneLevels% \\.c log (%%frequency% / %F__min_) / log (%F__max_ / %F__min_),")
NORMAL (U"where %F__min_ and %F__max_ are the minimum and the maximum frequency of a speaker's pitch range and %%numberOfToneLevels% "
	"is the number of levels into which the pitch range is divided. "
	"This formula maps any frequency between %F__min_ and %F__max_ to a number between 0 and %%numberOfToneLevels%.")
NORMAL (U"To get the frequency in hertz from a specified tone level %T we have to use the inverse formula:")
EQUATION (U"%%frequency% = %F__min_ \\.c 10^^(%T \\.c log (%F__max_ / %F__min_)) / %%numberOfToneLevels%)^.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (8), U""
	Manual_DRAW_SETTINGS_WINDOW (U"PitchTier: Modify interval (tone levels)", 8)
	Manual_DRAW_SETTINGS_WINDOW_RANGE (U"Time range (s)", U"0.0", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_RANGE (U"Pitch range (Hz)", U"80.0", U"200.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD (U"Number of tone levels", U"5")
	Manual_DRAW_SETTINGS_WINDOW_FIELD (U"Relative times", U"0.0 0.5 1.0")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (U"...are...",U"fractions")
	"Text... 50 left y half ...of the interval duration which will be added...\n" \
	"y += 40\n" \
	"Text... 50 left y half ...to the start time of the interval.\n" \
	"y += 40\n" \
	Manual_DRAW_SETTINGS_WINDOW_FIELD (U"Tone levels", U"2.1 2.1 5.0")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start and end time of the interval where the changes will be applied.")
TERM (U"##Pitch range (Hz)")
DEFINITION (U"The minimum and maximum frequency to which the tone levels refer.")
TERM (U"##Number of tone levels")
DEFINITION (U"The number of levels into which the pitch range is divided.")
TERM (U"##Relative times")
DEFINITION (U"determine, together with the following option, the times of the new pitch points with respect to the start time of the interval.")
DEFINITION (U"determines how the times %t__%i_ of the new pitch points are calculated. The time of each new pitch point is "
	"determined by adding to the start time of the interval a time calculated from the relative time value. If %%t%__min_ and "
	"%%t%__max_ are the start and end time of the interval and %%r%__i_ is the %%i%^^th^ relative time, the times %t__%i_ are "
	"calculated according to the options as:")
TERM1 (U"%%fractions%")
DEFINITION1 (U"%%t%__%i_ = %t__min_ + %r__%i_ (%t__max_ \\-- %t__min_). The relative time values are fractions of the interval "
	"duration. Normally fractions are numbers in the range 0.0 to 1.0, although smaller and larger numbers are allowed.")
TERM1 (U"%%percentages%")
DEFINITION1 (U"%%t%__%i_ = %t__min_+ 0.01 %r__%i_ (%t__max_ \\-- %t__min_). The relative time values are percentages of the "
	"interval duration. Normally percentages are numbers in the range 0.0 to 100.0, although smaller and larger numbers are allowed.")
TERM1 (U"%%independent%")
DEFINITION1 (U"%%t%__%i_ = %t__min_ + %r__%i_. The relative time values specify an offset in seconds here. ")

TERM (U"##Tone levels")
DEFINITION (U"specify the frequencies at the corresponding time points as tone levels.")
ENTRY (U"Algorithm")
NORMAL (U"1. The real times are calculated from the relative times.")
NORMAL (U"2. The frequencies are calculated from the tone levels.")
NORMAL (U"3. The real times and the frequencies are sorted together by time.")
NORMAL (U"4. All pitch points in the PitchTier between the first and the last time of the sorted time array are removed.")
NORMAL (U"5. The newly calculated pitch points are added to the PitchTier.")
MAN_END

MAN_BEGIN (U"Polygon: Rotate...", U"djmw", 20100418)
INTRO (U"Rotates the selected @@Polygon@ counterclockwise with respect to the given coordinates.")
MAN_END

MAN_BEGIN (U"Polygon: Get location of point...", U"djmw", 20120220)
INTRO (U"Determines whether a given point is on the ##I#nside, the ##O#utside, on an ##E#dge or on a ##V#ertex of the selected Polygon.")
ENTRY (U"Algorithm")
NORMAL (U"We determine how often a horizontal line extending from the point crosses the polygon. If the number of crossings is even, "
	"the point is on the outside, else on the inside. Special care is taken to be able to detect if a point is on the boundary of "
	"the polygon. The used algorithm is from @@Hormann & Agathos (2001)@.")
MAN_END

MAN_BEGIN (U"Polygon: Simplify", U"djmw", 20140509)
INTRO (U"Removes collinear vertices from a @@Polygon@.")
ENTRY (U"Example")
SCRIPT (4, 4,
	U"p1 = Create simple Polygon: \"p\", \"0.0 0.0 0.0 1.0 0.5 0.5 1.0 0.0 0.5 0 0 -0.5 0 -0.25\"\n"
	"Draw closed: 0, 0, 0, 0\n"
	"Colour: \"Red\"\n"
	"Draw circles: 0, 0, 0, 0, 3\n"
	"p2 = Simplify\n"
	"Colour: \"Black\"\n"
	"Paint circles: 0, 0, 0, 0, 1.5\n"
	"removeObject: p1, p2\n"
)
NORMAL (U"Given the Polygon with the seven vertices indicated by the red open circles, the Simplify action results in the Polygon "
	"with four vertices indicated by the filled black circles.")
MAN_END

MAN_BEGIN (U"Polygon: Translate...", U"djmw", 20100418)
INTRO (U"Translates the selected @@Polygon@ over the given vector.")
NORMAL (U"Given the old coordinates (%x__%i_, %y__%i_) and the translation (%x__%t_, %y__%t_), the new coordinates are:")
EQUATION (U"%x__%i_\\'p = %x__%i_ + %x__%t_")
EQUATION (U"%y__%i_\\'p = %y__%i_ + %y__%t_")
MAN_END

MAN_BEGIN (U"Polynomial", U"djmw", 19990608)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type "
	"Polynomial represents a polynomial function on a domain.")
NORMAL (U"A polynomial of degree %n is defined as:")
EQUATION (U"%p(%x) = %c__1_ + %c__2_ %x + %c__3_ %x^^2^ + ... c__%n+1_ %x^^%n^.")
NORMAL (U"The real numbers %c__%k_ are called the polynomial %coefficients.")
ENTRY (U"Commands")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Create Polynomial...@ (in the ##New menu#)")
LIST_ITEM (U"\\bu @@LPC: To Polynomial (slice)...@ (from prediction coefficients)")
LIST_ITEM (U"\\bu @@LegendreSeries: To Polynomial@")
LIST_ITEM (U"\\bu @@ChebyshevSeries: To Polynomial@")
NORMAL (U"Drawing")
LIST_ITEM (U"\\bu ##Draw...#")
NORMAL (U"Queries")
LIST_ITEM (U"\\bu @@Polynomial: Get function value...|Get function value...@: get %p(%x)")
LIST_ITEM (U"\\bu ##Get coefficient value...#: get %c__%i_")
LIST_ITEM (U"\\bu @@Polynomial: Get minimum...|Get minimum...@: minimum of %p(%x) on an interval")
LIST_ITEM (U"\\bu @@Polynomial: Get x of minimum...|Get x of minimum...@")
LIST_ITEM (U"\\bu @@Polynomial: Get maximum...|Get maximum...@: maximum of %p(%x) on an interval")
LIST_ITEM (U"\\bu @@Polynomial: Get x of maximum...|Get x of maximum...@")
LIST_ITEM (U"\\bu @@Polynomial: Get area...|Get area...@")
NORMAL (U"Modification")
LIST_ITEM (U"\\bu ##Set domain...#: new domain")
LIST_ITEM (U"\\bu ##Set coefficient value...#: change one coefficient")
NORMAL (U"Conversion")
LIST_ITEM (U"\\bu @@Polynomial: To Spectrum...|To Spectrum...@ (evaluation over unit-circle)")
LIST_ITEM (U"\\bu @@Polynomial: To Polynomial (derivative)|To Polynomial (derivative)@")
LIST_ITEM (U"\\bu @@Polynomial: To Polynomial (primitive)|To Polynomial (primitive)@")
LIST_ITEM (U"\\bu @@Polynomial: To Roots|To Roots@: roots of polynomial")
MAN_END

MAN_BEGIN (U"Polynomial: Get area...", U"djmw", 19990610)
INTRO (U"A command to compute the area below the selected @Polynomial object.")
ENTRY (U"Settings")
TERM (U"##Xmin#, ##Xmax#")
DEFINITION (U"define the interval.")
NORMAL (U"The area is defined as __%xmin_\\in^^xmax^ %p(%x) %dx.")
MAN_END

MAN_BEGIN (U"Polynomial: Get function value...", U"djmw", 19990610)
INTRO (U"A command to compute %p(%x) for the selected @Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomial: Get maximum...", U"djmw", 19990610)
INTRO (U"A command to compute, on a specified interval, the maximum value of the selected "
	"@Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomial: Get x of maximum...", U"djmw", 19990610)
INTRO (U"A command to compute, on a specified interval, the location of the maximum of the "
	"selected @Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomial: Get minimum...", U"djmw", 19990610)
INTRO (U"A command to compute, on a specified interval, the minimum value of the selected "
	"@Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomial: Get x of minimum...", U"djmw", 19990610)
INTRO (U"A command to compute, on a specified interval, the location of the minimum of the "
	"selected @Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomials: Multiply", U"djmw", 19990616)
INTRO (U"A command to multiply two @@Polynomial|polynomials@ with each other.")
NORMAL (U"The result of multiplying 1 + 2 %x and 2 \\-- %x^2 will be the polynomial:")
EQUATION (U"2 + 4 %x \\-- %x^2 \\-- 2 %x^3.")
MAN_END

MAN_BEGIN (U"Polynomial: To Polynomial (derivative)", U"djmw", 19990610)
INTRO (U"A command to compute the derivative of the selected @Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomial: To Polynomial (primitive)", U"djmw", 19990610)
INTRO (U"A command to compute the primitive of the selected @Polynomial object.")
MAN_END

MAN_BEGIN (U"Polynomial: Scale x...", U"djmw", 19990610)
INTRO (U"A command to transform the selected @Polynomial object to a new domain.")
TERM (U"##Xmin# and ##Xmax#")
DEFINITION (U"define the new domain.")
ENTRY (U"Behaviour")
NORMAL (U"The polynomial is transformed from domain [%x__min_, %x__max_] to "
	"domain [%Xmin, %Xmax] in such a way that its form stays the same. "
	"This is accomplished by first calculating:")
EQUATION (U"%f(%x\\'p) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x\\'p^^%k^, where")
EQUATION (U"%x\\'p = %a %x + %b,")
NORMAL (U"and then collecting terms of equal degree. The %a and %b are defined as")
EQUATION (U"%a = (%x__min_ \\-- %x__max_) / (%Xmin \\-- %Xmax)")
EQUATION (U"%b = %x__min_ \\-- %a %Xmin")
MAN_END

MAN_BEGIN (U"Polynomial: To Roots", U"djmw", 19990608)
INTRO (U"A command to compute the @@Roots|roots@ of the selected @Polynomial objects.")
ENTRY (U"Algorithm")
NORMAL (U"The roots are found from the polished eigenvalues of a special companion matrix. "
	"For further explanation on these methods see @@Press et al. (1992)@.")
MAN_END

MAN_BEGIN (U"Polynomial: To Spectrum...", U"djmw", 19990616)
INTRO (U"A command to compute the @@Spectrum|spectrum@ of the selected @Polynomial objects.")
ENTRY (U"Settings")
TERM (U"##Nyquist frequency (Hz)")
DEFINITION (U"defines the highest frequency in the spectrum. The lowest frequency of the spectrum "
	"will be 0 Hz.")
TERM (U"##Number of frequencies")
DEFINITION (U"defines the number of frequencies in the spectrum.")
ENTRY (U"Algorithm")
NORMAL (U"We calculate the spectrum by evaluating the polynomial at regularly spaced points %z__%k_ "
	"on the upper half of a circle with radius %r = 1 in the complex plane. The upperhalf of the "
	"unit circle, where %k\\.c%\\fi is in the interval [0, %\\pi], will be mapped to frequencies "
	"[0, @@Nyquist frequency@] in the spectrum. ")
NORMAL (U"The complex values %z__%k_ (%k=1..%numberOfFrequencies) are defined as:")
EQUATION (U"%z__%k_ = %r e^^%i %k %\\fi^, where,")
EQUATION (U"%\\fi = \\pi / (%numberOfFrequencies \\-- 1) and %r = 1.")
MAN_END

MAN_BEGIN (U"Principal component analysis", U"djmw", 20160222)
INTRO (U"This tutorial describes how you can perform principal component "
       "analysis with Praat.")
NORMAL (U"Principal component analysis (PCA) involves a mathematical procedure "
	"that transforms a number of (possibly) correlated variables into a "
	"(smaller) number of uncorrelated variables called %%principal "
	"components%. The first principal component accounts for as much of the "
	"variability in the data as possible, and each succeeding component "
	"accounts for as much of the remaining variability as possible.")
ENTRY (U"1. Objectives of principal component analysis")
LIST_ITEM (U"\\bu To discover or to reduce the dimensionality of the data set.")
LIST_ITEM (U"\\bu To identify new meaningful underlying variables.")
ENTRY (U"2. How to start")
NORMAL (U"We assume that the multi-dimensional data have been collected in a @TableOfReal data matrix, "
	"in which the rows are associated with the cases and the columns with the variables. The TableOfReal is therefore interpreted as %%numberOfRows% data vectors, each data vector has %%numberofColumns% elements.")
NORMAL (U"Traditionally, principal component analysis is performed on the "
	"@@Covariance|Covariance@ matrix or on the @@correlation|Correlation@ matrix. "
	"These matrices can be calculated from the data matrix. "
	"The covariance matrix contains scaled @@SSCP|sums of squares and cross products@. "
	"A correlation matrix is like a covariance matrix but first the variables, i.e. the columns, have been standardized.  "
	"We will have to standardize the data first if the variances of "
	"variables differ much, or if the units of measurement of the "
	"variables differ. You can standardize the data in the TableOfReal by choosing @@TableOfReal: Standardize columns|Standardize columns@.")
NORMAL (U"To perform the analysis, we select the TabelOfReal data matrix in the list of objects and choose "
	"@@TableOfReal: To PCA|To PCA@. This will result in a new PCA object in the "
	"list of objects.")
NORMAL (U"We can now make a @@Scree plot|scree@ plot of the eigenvalues, @@Eigen: Draw "
	"eigenvalues...|Draw eigenvalues...@ "
	"to get an indication of the importance of each eigenvalue. The exact "
	"contribution of each eigenvalue (or a range of eigenvalues) to the "
	"\"explained variance\" can also be queried: "
	"@@PCA: Get fraction variance accounted for...|Get fraction variance "
	"accounted for...@. You might also check for the equality of a "
	"number of eigenvalues: @@PCA: Get equality of eigenvalues...|Get equality "
	"of eigenvalues...@.")
ENTRY (U"3. Determining the number of components to keep")
NORMAL (U"There are two methods to help you to choose the number of components to keep. "
	"Both methods are based on relations between the eigenvalues.")
LIST_ITEM (U"\\bu Plot the eigenvalues, @@Eigen: Draw eigenvalues...|"
	"Draw eigenvalues...@. If the points on the graph tend to level out (show an \"elbow\"), "
	"these eigenvalues are usually close enough to zero that they can be "
	"ignored.")
LIST_ITEM (U"\\bu Limit the number of components to that number that accounts for a certain fraction of the total variance. For example, "
	"if you are satisfied with 95\\%  of the total variance explained, then use the number you get by the query @@PCA: Get number of components (VAF)...|Get number of components (VAF)...@ 0.95#.")
ENTRY (U"4. Getting the principal components")
NORMAL (U"Principal components are obtained by projecting the multivariate "
	"datavectors on the space spanned by the eigenvectors. This can be done "
	"in two ways:")
LIST_ITEM (U"1. Directly from the TableOfReal without first forming a "
	"@PCA object: "
	"@@TableOfReal: To Configuration (pca)...|To Configuration (pca)...@. "
	"You can then draw the Configuration or display its numbers. ")
LIST_ITEM (U"2. Select a PCA and a TableOfReal object together and choose "
	"@@PCA & TableOfReal: To Configuration...|To Configuration...@. "
	"In this way you project the TableOfReal onto the PCA's eigenspace.")
ENTRY (U"5. Mathematical background on principal component analysis")
NORMAL (U"The mathematical technique used in PCA is called eigen analysis: "
	"we solve for the eigenvalues and eigenvectors of a square symmetric "
	"matrix with sums of squares and cross products. "
	"The eigenvector associated with the largest eigenvalue has the same "
	"direction as the first principal component. The eigenvector associated "
	"with the second largest eigenvalue determines the direction of the second "
	"principal component. "
	"The sum of the eigenvalues equals the trace of the square matrix and the "
	"maximum number of eigenvectors equals the number of rows (or columns) of "
	"this matrix.")
ENTRY (U"6. Algorithms")
NORMAL (U"If our starting point happens to be a symmetric matrix like the covariance matrix, "
	"we solve for the eigenvalue and eigenvectors "
	"by first performing a Householder reduction to tridiagonal form, followed"
	" by the QL algorithm with implicit shifts.")
NORMAL (U"If, conversely, our starting point is the data matrix #%A , "
	"we do not have to form explicitly the matrix with sums of squares and "
	"cross products, #%A\\'p#%A. Instead, we proceed by a numerically more "
	"stable method, and form the @@singular value decomposition@ of #%A, "
	"#%U #%D #%V\\'p. The matrix #%V then contains the eigenvectors, "
	"and the squared diagonal elements of #%D contain the eigenvalues.")
MAN_END

MAN_BEGIN (U"PCA & Covariance: Project", U"djmw", 20040225)
INTRO (U"A command to project the @Covariance object onto the eigenspace of "
	"the @PCA object. ")
NORMAL (U"Further details can be found in @@Eigen & SSCP: Project@.")
MAN_END

MAN_BEGIN (U"PCA & SSCP: Project", U"djmw", 20040225)
INTRO (U"A command to project the @SSCP object onto the eigenspace of "
	"the @PCA object. ")
NORMAL (U"Further details can be found in @@Eigen & SSCP: Project@.")
MAN_END

MAN_BEGIN (U"Regular expressions", U"David Weenink & Paul Boersma", 20180401)
INTRO (U"This tutorial describes the syntax of regular expressions in Praat. ")
ENTRY (U"Introduction")
NORMAL (U"A %%regular expression% (%regex) is a text string that describes a %set "
	"of strings. Regular expressions are useful as a way to search "
	"for patterns in text strings and, optionally, replace them by another "
	"pattern.")
NORMAL (U"Some regular expressions match only one string, i.e., the set they describe has "
	"only one member. For example, the regex \"ab\" matches the string \"ab\" "
	"and no others. Other regular expressions match more than one string, i.e., the set "
	"they describe has more than one member. For example, the regex \"a*\" "
	"matches the string made up of any number (including zero) of \"a\"s. "
	"As you can see, some characters match themselves (such as \"a\" and "
	"\"b\"), and these characters are called %ordinary characters. The "
	"characters that do not match themselves, such as \"*\", are called "
	"%special characters or %metacharacters. Many special characters are only "
	"special characters in the %search regex and are ordinary characters in "
	"the substitution regex.")
NORMAL (U"You can read the rest of this tutorial sequentially with the help of "
	"the \"<1\" and \">1\" buttons.")
LIST_ITEM (U"1. @@Regular expressions 1. Special characters|Special characters@ "
	"(\\bs \\^  \\$  { } [ ] ( ) . + ? \\|  - &)")
LIST_ITEM (U"2. @@Regular expressions 2. Quantifiers|Quantifiers@ "
	"(how often do we match).")
LIST_ITEM (U"3. @@Regular expressions 3. Anchors|Anchors@ (where do we match)")
LIST_ITEM (U"4. @@Regular expressions 4. Special constructs with parentheses|"
	"Special constructs with parentheses@ (grouping constructs)")
LIST_ITEM (U"5. @@Regular expressions 5. Special control characters|"
	"Special control characters@ (difficult-to-type characters like \\bsn)")
LIST_ITEM (U"6. @@Regular expressions 6. Convenience escape sequences|"
	"Convenience escape sequences@ "
	"(\\bsd \\bsD \\bsl \\bsL \\bss \\bsS \\bsw \\bsW \\bsB)")
LIST_ITEM (U"7. @@Regular expressions 7. Octal and hexadecimal escapes|"
	"Octal and hexadecimal escapes@ (things like \\bs053 or \\bsX2B)")
LIST_ITEM (U"8. @@Regular expressions 8. Substitution special characters|"
	"Substitution special characters@ (\\bs1..\\bs9 \\bsU \\bsu \\bsL \\bsl &)")
NORMAL (U"More in-depth coverage of regular expressions can be found in "
	"@@Friedl (1997)@.")
MAN_END

MAN_BEGIN (U"Regular expressions 1. Special characters", U"djmw", 20010718)
INTRO (U"The following characters are the %meta characters that give special "
	"meaning to the regular expression search syntax:")
TERM (U"#\\bs#   the backslash %escape character.")
DEFINITION (U"The backslash gives special meaning to the character "
	"following it. For example, the combination \"\\bsn\" stands for the "
	"%newline, one of the @@Regular expressions 5. Special control characters"
	"|control characters@. The combination \"\\bsw\" stands for a \"word\" "
	"character, one of the @@Regular expressions 6. Convenience escape "
	"sequences|"
	"convenience escape sequences@ while \"\\bs1\" is one of the @@Regular "
	"expressions 8. Substitution special characters|substitution special "
	"characters@.")
LIST_ITEM1 (U"Example: The regex \"aa\\bsn\" tries to match two consecutive "
	"\"a\"s at the end of a line, inclusive the newline character itself.")
LIST_ITEM1 (U"Example: \"a\\bs+\" matches \"a+\" and not a series of one or "
	"\"a\"s.")
TERM (U"##\\^ #    the caret is the @@Regular expressions 3. "
	"Anchors|anchor@ for the start of the string, or the negation symbol.")
LIST_ITEM1 (U"Example: \"\\^ a\" matches \"a\" at the start of the string.")
LIST_ITEM1 (U"Example: \"[\\^ 0-9]\" matches any non digit.")
TERM (U"##\\$ #    the dollar sign is the @@Regular expressions 3. "
	"Anchors|anchor@ for the end of the string.")
LIST_ITEM1 (U"Example: \"b\\$ \" matches a \"b\" at the end of a line.")
LIST_ITEM1 (U"Example: \"\\^ \\$ \" matches the empty string.")
TERM (U"##{ }#    the opening and closing curly brackets are used as range @@Regular "
	"expressions 2. Quantifiers|quantifiers@.")
LIST_ITEM1 (U"Example: \"a{2,3}\" matches \"aa\" or \"aaa\".")
TERM (U"##[ ]#    the opening and closing square brackets define a character class to "
	"match a %single character.")
DEFINITION (U"The \"\\^ \" as the first character following the \"[\" negates, "
	"and the match is for the characters %not listed. "
	"The \"-\" denotes a range of characters. Inside a \"[  ]\" character "
	"class construction, most special characters are interpreted as ordinary "
	"characters.")
LIST_ITEM1 (U"Example: \"[d-f]\" is the same as \"[def]\" and matches \"d\", "
	"\"e\" or \"f\".")
LIST_ITEM1 (U"Example: \"[a-z]\" matches any lower-case characters in the "
	"alphabet.")
LIST_ITEM1 (U"Example: \"[\\^ 0-9]\" matches any character that is not an ASCII digit.")
LIST_ITEM1 (U"Example: A search for \"[][()?<>\\$ \\^ .*?\\^ ]\" in the string "
	"\"[]()?<>\\$ \\^ .*?\\^ \" followed by a replace string \"r\" has the result "
	"\"rrrrrrrrrrrrr\". Here the search string is %one character class and "
	"all the meta characters are interpreted as ordinary characters without "
	"the need to escape them.")
TERM (U"##( )#    the opening and closing parenthes3s are used for grouping "
	"characters (or other regexes).")
DEFINITION (U"The groups can be referenced in "
	"both the search and the @@Regular expressions 8. Substitution special "
	"characters|substitution@ phase. There also exist some @@Regular "
	"expressions 4. Special constructs with parentheses|special constructs "
	"with parentheses@.")
LIST_ITEM1 (U"Example: \"(ab)\\bs1\" matches \"abab\".")
TERM (U"##.#    the dot matches any character except the newline symbol.")
LIST_ITEM1 (U"Example: \".a\" matches two consecutive characters where "
	"the last one is \"a\".")
LIST_ITEM1 (U"Example: \".*\\bs.txt\\$ \" matches all strings that end in "
	"\".txt\".")
TERM (U"##*#    the asterisk is the match-zero-or-more @@Regular expressions 2. "
	"Quantifiers|quantifier@.")
LIST_ITEM1 (U"Example: \"\\^ .*\\$ \" matches an entire line. ")
TERM (U"##+#    the plus sign is the match-one-or-more quantifier.")
TERM (U"##?#    the question mark is the match-zero-or-one "
	"quantifier. The question mark is also used in  "
	"@@Regular expressions 4. Special constructs with parentheses|special "
	"constructs with parentheses@ and in @@Regular expressions 2. "
	"Quantifiers|changing match behaviour@.")
TERM (U"##\\| #    the vertical pipe separates a series of alternatives.")
LIST_ITEM1 (U"Example: \"(a|b|c)a\" matches \"aa\" or \"ba\" or \"ca\".")
TERM (U"##< >#    the smaller and greater signs are @@Regular expressions 3. "
	"Anchors|anchors@ that specify a left or right word boundary.")
TERM (U"##-#    the minus sign indicates a range in a character class (when it is "
	"not at the first position after the \"[\" opening bracket or the last "
	"position before the \"]\" closing bracket.")
LIST_ITEM1 (U"Example: \"[A-Z]\" matches any uppercase character.")
LIST_ITEM1 (U"Example: \"[A-Z-]\" or \"[-A-Z]\" match any uppercase character "
	"or \"-\".")
TERM (U"##&#    the ampersand is the \"substitute complete match\" symbol.")
MAN_END

MAN_BEGIN (U"Regular expressions 2. Quantifiers", U"djmw", 20010708)
INTRO (U"Quantifiers specify how often the preceding @@Regular expressions|"
	"regular expression@ should match.")
TERM (U"##*#   Try to match the preceding regular expression zero or more times.")
LIST_ITEM1 (U"Example: \"(ab)c*\" matches \"ab\" followed by zero or more "
	"\"c\"s, i.e., \"ab\", \"abc\", \"abcc\", \"abccc\" ...")
TERM (U"##+#   Try to match the preceding regular expression one or more times.")
LIST_ITEM1 (U"Example: \"(ab)c+\" matches \"ab\" followed by one or more "
	"\"c\"s, i.e., \"abc\", \"abcc\", \"abccc\" ...")
TERM (U"##{%m, %n}#   Try to match the preceding regular expression between %m "
	"and %n times.")
DEFINITION (U"If you leave %m out, it is assumed to be zero. If you leave "
	"%n out it is assumed to be infinity. I.e., \"{,%n}\" matches from %zero "
	"to %n times, \"{%m,}\" matches a minimum of %m times, \"{,}\" matches "
	"the same as \"*\" and \"{n}\" is shorthand for \"{n, n\"} and matches "
	"exactly %n times.")
LIST_ITEM1 (U"Example: \"(ab){1,2}\" matches \"ab\" and \"abab\".")
TERM (U"##?#   Try to match zero or one time.")
ENTRY (U"Changing match behaviour")
NORMAL (U"Default the quantifiers above try to match as much as possible, they "
	"are %greedy. "
	"You can change greedy behaviour to %lazy behaviour by adding an "
	"extra \"?\" after the quantifier.")
LIST_ITEM1 (U"Example: In the string \"cabddde\", the search \"abd{1,2}\" "
	"matches \"abdd\", while the search for \"abd{1,2}?\" matches \"abd\".")
LIST_ITEM1 (U"Example: In the string \"cabddde\", the search \"abd+\" "
	"matches \"abddd\", while the search for \"abd+?\" matches \"abd\".")
MAN_END

MAN_BEGIN (U"Regular expressions 3. Anchors", U"DAvid Weenink & Paul Boersma", 20180401)
INTRO (U"Anchors let you specify a very specific position within the search "
	"text.")
TERM (U"##\\^ #   Try to match the (following) regex at the beginning of the string.")
LIST_ITEM1 (U"Example: \"\\^ ab\" matches \"ab\" only at the beginning of a "
	"line and not, for example, in the line \"cab\".")
TERM (U"##\\$ #   Try to match the (following) regex at the end of the string.")
TERM (U"##<#    Try to match the regex at the %start of a word.")
DEFINITION (U"The character class that defines a %word can be found at the "
	"@@Regular expressions 6. Convenience escape sequences|convenience escape "
	"sequences@ page.")
TERM (U"##>#    Try to match the regex at the %end of a word.")
TERM (U"##\\bsB#   Not a word boundary")
MAN_END

MAN_BEGIN (U"Regular expressions 4. Special constructs with parentheses", U"djmw",
	20010710)
INTRO (U"Some special constructs exist with parentheses. ")
TERM (U"##(?:#%regex#)#   is a grouping-only construct.")
DEFINITION (U"They exist merely for efficiency reasons and facilitate grouping.")
TERM (U"##(?=#%regex#)#   is a positive look-ahead.")
DEFINITION (U"A match of the regular expression contained in the positive "
	"look-ahead construct is attempted. If the match succeeds, control is "
	"passed to the regex following this construct and the text consumed by "
	"this look-ahead construct is first unmatched. ")
TERM (U"##(?!#%regex#)#   is a negative look-ahead.")
DEFINITION (U"Functions like a positive look-ahead, only the "
	"%regex must %not match.")
LIST_ITEM (U"Example: \"abc(?!.*abc.*)\" searches for the %last "
	"occurrence of \"abc\" in a string.")
TERM (U"##(?i#%regex#)#   is a case insensitive regex.")
TERM (U"##(?I#%regex#)#   is a case sensitive regex.")
DEFINITION (U"Default a regex is case sensitive. ")
LIST_ITEM1 (U"Example: \"(?iaa)\" matches \"aa\", \"aA\", \"Aa\" and \"AA\".")
TERM (U"##(?n#%regex#)#   matches newlines.")
TERM (U"##(?N#%regex#)#   doesn't match newlines.")
NORMAL (U"All the constructs above do not capture text and cannot be "
	"referenced, i.e., the parentheses are not counted. However, you "
	"can make them capture text by surrounding them with %ordinary "
	"parentheses.")
MAN_END

MAN_BEGIN (U"Regular expressions 5. Special control characters", U"djmw", 20010708)
INTRO (U"Special control characters in a @@Regular expressions|regular "
	"expression@ specify characters that are difficult to type.")
TERM (U"#\\bsa  alert (bell).")
TERM (U"#\\bsb  backspace.")
TERM (U"#\\bse  ASCII escape character.")
TERM (U"#\\bsf  form feed (new page).")
TERM (U"#\\bsn  newline.")
TERM (U"#\\bsr  carriage return.")
LIST_ITEM1 (U"Example : a search for \"\\bsr\\bsn\" followed by a replace "
	"\"\\bsr\" changes Windows text files to Macintosh text files.")
LIST_ITEM1 (U"Example : a search for \"\\bsr\" followed by a replace "
	"\"\\bsn\" changes Macintosh text files to Unix text files.")
LIST_ITEM1 (U"Example : a search for \"\\bsr\\bsn\" followed by a replace "
	"\"\\bsn\" changes Windows text files to Unix text files.")
TERM (U"#\\bst  horizontal tab.")
TERM (U"#\\bsv  vertical tab.")
MAN_END

MAN_BEGIN (U"Regular expressions 6. Convenience escape sequences", U"djmw", 20010708)
INTRO (U"Convenience escape sequences in a @@Regular expressions|regular "
	"expression@ present a shorthand for some character classes.")
TERM (U"#\\bsd  matches a digit: [0-9].")
LIST_ITEM1 (U"Example: \"-?\\bsd+\" matches any integer.")
TERM (U"#\\bsD  %not a digit: [\\^ 0-9].")
TERM (U"#\\bsl  a letter: [a-zA-Z].")
TERM (U"#\\bsL  %not a letter: [\\^ a-zA-Z].")
TERM (U"#\\bss  whitespace: [ \\bst\\bsn\\bsr\\bsf\\bsv].")
TERM (U"#\\bsS  %not whitespace: [\\^  \\bst\\bsn\\bsr\\bsf\\bsv].")
TERM (U"#\\bsw  \"word\" character: [a-zA-Z0-9\\_ ].")
LIST_ITEM1 (U"Example: \"\\bsw+\" matches a \"word\", i.e., a string of one "
	"or more characters that may consist of letters, digits and underscores.")
TERM (U"#\\bsW  %not a \"word\" character: [\\^ a-zA-Z0-9\\_ ].")
TERM (U"#\\bsB  any character that is %not a word-delimiter.")
MAN_END

MAN_BEGIN (U"Regular expressions 7. Octal and hexadecimal escapes", U"djmw", 20010709)
INTRO (U"An octal number can be represented by the octal escape \"\\bs0\" "
	"and maximally three digits from the digit class [0-7]. "
	"The octal number should not exceed \\bs0377. ")
NORMAL (U"A hexadecimal number can be represented by the octal escape "
	"\"\\bsx\" or \"\\bsX\"and maximally two characters from the class "
	"[0-9A-F]. The maximum hexadecimal number should not exceed \\bsxFF. ")
LIST_ITEM1 (U"Example: \\bs053 and \\bsX2B both specify the \"+\" character.")
MAN_END

MAN_BEGIN (U"Regular expressions 8. Substitution special characters", U"djmw", 20010708)
INTRO (U"The substitution string is mostly interpreted as ordinary text except "
	"for the @@Regular expressions 5. Special control characters|"
	"special control characters@, the @@Regular expressions 7. Octal and "
	"hexadecimal escapes|octal and hexadecimal escapes@ and the following "
	"character combinations:")
TERM (U"#\\bs1 ... #\\bs9    are backreferences at sub-expressions 1 ... 9 in the match.")
DEFINITION (U"Any of the first nine sub-expressions of the match string can "
	"be inserted into the replacement string by inserting a “\\bs” followed "
	"by a digit from 1 to 9 that represents the string matched by a "
	"parenthesized expression within the regular expression. The numbering "
	"is left to right.")
LIST_ITEM1 (U"Example: A search for \"(a)(b)\" in the string \"abc\", "
	"followed by a replace \"\\bs2\\bs1\" results in \"bac\".")
TERM (U"#&    reference at entire match.")
DEFINITION (U"The entire string that was matched by the search operation will "
	"be substituted.")
LIST_ITEM1 (U"Example: a search for \".\" in the string \"abcd\" followed by "
	"the replace \"&&\" doubles every character in the result "
	"\"aabbccdd\".")
TERM (U"#\\bsU #\\bsu    to uppercase.")
DEFINITION (U"The text inserted by \"&\" or \"\\bs1\" ... \"\\bs9\" is "
	"converted to %uppercase (\"\\bsu\" only changes the %first character to "
	"uppercase).")
LIST_ITEM1 (U"Example: A search for \"(aa)\" in the string \"aabb\", "
	"followed by a replace \"\\bsU\\bs1bc\" results in the string \"AAbcbb\".")
TERM (U"#\\bsL #\\bsl    to lowercase.")
DEFINITION (U"The text inserted by \"&\" or \"\\bs1\" ... \"\\bs9\" is "
	"converted to %lowercase (\"\\bsl\" only changes the %first character to "
	"lowercase).")
LIST_ITEM1 (U"Example: A search for \"(AA)\" with a replace \"\\bsl\\bs1bc\" "
	"in the string \"AAbb\" results in the string \"aAbcbb\".")
MAN_END

MAN_BEGIN (U"Roots", U"djmw", 19990608)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type Roots "
	"represents the (complex) roots of a @@Polynomial|polynomial@ function.")
ENTRY (U"Commands")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Polynomial: To Roots@")
NORMAL (U"Drawing")
LIST_ITEM (U"\\bu ##Draw...# (in the complex plane)")
NORMAL (U"Queries")
LIST_ITEM (U"\\bu ##Get root...#: get complex root")
LIST_ITEM (U"\\bu ##Get real part of root...#")
LIST_ITEM (U"\\bu ##Get imaginary part of root...#")
MAN_END

MAN_BEGIN (U"Debug multi-threading...", U"David Weenink & Paul Boersma", 20250721) // 20250813
INTRO (U"These settings determine how fast parallelized procedures, such as analyses of sound, will be performed on your computer. "
	"You can use these settings for tuning e.g. a specific analysis procedure. "
	"These settings are \"debug settings\" that are not remembered across Praat sessions.")
ENTRY (U"Settings")
TERM (U"##Use multi-threading#")
DEFINITION (U"Only if you choose %on, the computing load will be equally divided among a number of different processors "
	"on your computer. If it’s off, you will get single-threading.")
TERM (U"##Maximum number of threads#")
DEFINITION (U"determines the number of threads that you would like to use. Most modern computers have multiple "
	"processors. One some computers (e.g. AMD64/Intel64) each processor can process two threads; "
	"on some other computers (e.g. ARM64) each processor can process one thread. "
    "If you supply 0 for this setting, then Praat will use all physical threads once.")
TERM (U"##Minimum number of frames per thread#")
DEFINITION (U"as starting up a new thread costs time, it is rarely worthwhile to distribute "
	"small numbers of frames over lots of threads. To achieve an optimal speed gain, "
	"this number should be tuned via a script, for each type of analysis separately, "
	"and perhaps for different sets of analysis settings separately.")
TERM (U"##Trace multi-threading#")
DEFINITION (U"returns extra information for each analysed frame in the Info window.")
ENTRY (U"Example")
NORMAL (U"Suppose the \"maximum number of threads\" is 10, and you want to analyse 500 frames. "
	"If the \"minimum number of frames per thread\" is 20, "
    "Praat will divide up those 500 frames over 10 threads of 50 frames each. "
    "The first thread will analyse frames 1 to 50, the second thread will analyse "
    "frames 51 till 100, the third thread will analyse frames 101 till 150, etc. The tenth thread will analyse "
    "frames 451 till 500. All these threads will be executed, in parallel, almost at the same time, which guarantees "
    "a significant decrease in analysis time as compared to single-threading.")
NORMAL (U"If the \"minimum number of frames per thread\" is 100 instead, "
    "Praat will divide up the 500 frames over 5 threads of 100 frames each.")
NORMAL (U"The amount of speed-up depends on a number of factors: starting up a thread costs time, "
	"and allocating per-thread analysis buffers costs time.")
MAN_END

MAN_BEGIN (U"Scree plot", U"djmw", 20040331)
NORMAL (U"A scree plot shows the sorted eigenvalues, from large to "
	"small, as a function of the eigenvalue index.")
MAN_END

MAN_BEGIN (U"singular value decomposition", U"djmw", 20171217)
INTRO (U"The %%singular value decomposition% (SVD) is a matrix factorization algorithm.")
NORMAL (U"For %m >= %n, the singular value decomposition of a real %m \\xx %n matrix #A is the "
	"factorization")
EQUATION (U"#A = #U #D #V\\'p,")
NORMAL (U"The matrices in this factorization have the following properties:")
TERM (U"#U [%m \\xx %n] and #V [%n \\xx %n]")
DEFINITION (U"are orthogonal matrices. The columns #u__%i_ of #U =[#u__1_, ..., #u__%n_] "
	"are the %%left singular vectors%, and the columns #v__%i_ of #V [#v__1_, ..., #v__%n_] "
	"are the %%right singular vectors%.")
TERM (U"#D [%n \\xx %n] = diag (%\\si__1_, ..., %\\si__%n_)")
DEFINITION (U"is a real, nonnegative, and diagonal matrix. Its diagonal contains the so called "
	"%%singular values% %\\si__%i_, where %\\si__1_ \\>_ ... \\>_ %\\si__%n_ \\>_ 0.")
MAN_END

MAN_BEGIN (U"solving matrix equations", U"djmw", 20230801)
INTRO (U"In this manual you will learn how to solve different kinds of equations involving matrices and vectors.")
NORMAL (U"Given a matrix #A and a vector #y, the types of equations we like to solve are of the form #y=#A\\.c#x, where #A "
	"and #y are given. The task is to find the vector #x. The first two subsections show how to deal with this equation with "
	"no constraints on the solution vector #x. Section 4 will show how to deal with the situation when the solution vector #x is constrained.")
NORMAL (U"In the equation above, the matrix and the vectors have to conform. This means that the number of rows of #A should "
	"equal the size of the #y vector and the size of the solution vector #x will always equal the number of colums of #A.")
NORMAL (U"Note: In Praat scripting we don't distinguish in notation between a vector and its transpose.")
ENTRY (U"1. Matrix A is square")
NORMAL (U"In this case an exact solution for #x is possible because if #A is \"well behaved\" we can calculate its inverse and "
	"the solution will be #x = #A^^\\-m1^\\.c#y. The function #`solve#` (%`a##`, %`y#`)# is for this type of problem.")
NORMAL (U"The following example shows an exactly solvable system because #A is a square matrix and \"well behaved\":")
CODE (U"a## = {{0, 2, 0, 1},")
CODE (U"...    {2, 2, 3, 2},")
CODE (U"...    {4,-3, 0, 1},")
CODE (U"...    {6, 1,-6,-5}}")
CODE (U"y# = {0,-2,-7,6}")
CODE (U"x# = solve# (a##, y#)")
CODE (U"writeInfoLine: x#")
NORMAL (U"My info window shows:")
NORMAL (U"-0.49999999999999967 1.000000000000001 0.3333333333333344 -2.0000000000000027")
NORMAL (U"As a check we can calculate the norm of the difference between #y and ##A\\.cx#, which should be zero for a perfect solution.")
CODE (U"appendInfoLine: norm (y# - mul# (a##, x#))")
NORMAL (U"My info window shows 1.0777744118960794e-14, which is almost zero (it is not exactly zero due to rounding errors because "
	"real numbers cannot be represented exactly in a computer.")

ENTRY (U"2. Matrix A has more rows than columns")
NORMAL (U"If the matrix has more rows than colums an exact solution is generally not possible and the best thing we can do is to "
	"find a solution vector #x that %%minimizes% the squared distance between the vectors #y and #A\\.c#x. The problem now can be "
	"posed as: find the vector #x that minimizes ||#y \\-m #A\\.c#x||^2. This is a regression problem which can be solved using "
	"@@singular value decomposition@.")
NORMAL (U"The following example shows a 5\\xx2 matrix #A. The solution therefore is a vector with two elements.")
CODE (U"a## = {{-0.84,-0.184},")
CODE (U"...    { 0.09, 1.26},")
CODE (U"...    { 0.62,-0.20},")
CODE (U"...    {-1.48,-1.03},")
CODE (U"...    { 1.29, 0.03}}")
CODE (U"y# = {-0.19, -0.90, -1.53, -2.30, 0.58}")
CODE (U"x# = solve# (a##, y#)")
CODE (U"writeInfoLine: x#")
NORMAL (U"My info window shows:")
NORMAL (U"0.5881230621928452 0.21643159712237164")
NORMAL (U"We can calculate the norm of the difference:")
CODE (U"appendInfoLine: norm (y# - mul# (a##, x#))")
NORMAL (U"My info window shows:")
NORMAL (U"2.556992185298919")
NORMAL (U"This means that no other vector #x can be found that has ||#y \\-m #A\\.c#x|| < 2.556992185298919!")

ENTRY (U"3. Matrix A has more columns than rows")
NORMAL (U"If the number of colums is larger than the number of rows the system in general is underdetermined, i.e. we can not "
	"determine a complete solution vector.")

ENTRY (U"4. The x# vector is constrained")
NORMAL (U"If there are additional constraints on the vector #x we can handle three different cases:")

ENTRY (U"4.1 Constraints on the squared norm of the solution #x")
NORMAL (U"These can be expressed in the following form: %minimize ||#y \\-m #A\\.c#x||^2 + %\\al(#x\\'p#x \\-m %\\de) for some "
	"%\\al > 0 and %\\de \\>_ 0. Here the constraint on the squared norm #x\\'p#x of #x is enforced by adding a penalty function "
	"with weighing factor %\\al.")
NORMAL (U"The function to use is #`solveWeaklyConstrained#` (%`a##`, %`y#`, %`alpha`, %`delta`)")
NORMAL (U"The function is called \"weakly constrained\" because the penalty function prohibits a relatively large departure of "
	"#x\\'p#x from %\\de. If we let \\al go to infinity we have a constrained least squares regression problem of minimizing "
	"||#y \\-m #A\\.c#x||^^2^ subject to #x\\'p#x = %\\de. The algorithm we have implemented is described by @@Ten Berge (1991)@.")

ENTRY (U"4.1.1 An example from the cited paper with an 6\\xx2 matrix #A")
CODE (U"a## = {{ 4, 1, 0.5},")
CODE (U"...    { 4,-1,-0.5},")
CODE (U"...    {-4, 1,-0.5},")
CODE (U"...    {-4,-1, 0.5},")
CODE (U"...    { 2, 0,   0},")
CODE (U"...    {-2, 0,   0}}")
CODE (U"y# = {-1,-1,-1,-1,-1,1}")
CODE (U"alpha = 6.0")
CODE (U"delta = 2 / 3")
CODE (U"x# = solveWeaklyConstrained# (a##, y#, alpha, delta)")
CODE (U"writeInfoLine: x#")
NORMAL (U"My info window shows:")
NORMAL (U"-0.0563380281690141 -3.341667830688472e-17 0.7616819283108669")

ENTRY (U"4.1.2 The solution of the regression without the constraint")
NORMAL (U"No constraints are involved if we set %\\al = 0")
CODE (U"x# = solveWeaklyConstrained# (a##, y#, 0.0, delta)")
CODE (U"writeInfoLine: x#")
NORMAL (U"My info window shows:")
NORMAL (U"-0.05555555555555557 -5.696494054485392e-17 3.0458443711512004e-16")
NORMAL (U"The same solution would have appeared if we had used the following code:")
CODE (U"x# = solve# (a##, y#)")
CODE (U"writeInfoLine: x#")
ENTRY (U"4.1.3. To enforce a solution where the norm of the solution equals one")
NORMAL (U"We choose a very large value for %\\al and set %\\de to 1.0.")
CODE (U"x# = solveWeaklyConstrained# (a##, y#, 1e100, 1.0)")
CODE (U"writeInfoLine: x#")
NORMAL (U"My info window shows: ")
NORMAL (U"-0.05633802816901411 -3.341667830688472e-17 0.9984117520251988")

ENTRY (U"4.2. Constraint of nonnegativity of the solution")
NORMAL (U"Here we constrain the elements of the solution to be nonnegative. The problem is stated as %minimize ||#y \\-m #A\\.c#x||^2 "
	"where all %x__%i_ \\>_ 0.")
NORMAL (U"This problem can be solved by an iterative alternating least squares method as described by @@Borg & Groenen (1997)@. "
	"The function to use is #`solveNonnegative#` (%`a##`, %`y#` [, %`x#`], %`maximumNumberOfIterations`, %`tolerance`, %`infoLevel`)")
NORMAL (U"The parameters have the following meaning:")
TERM (U"%`a##`, %`y#`")
DEFINITION (U"the #A matrix and the #y vector.")
TERM (U"[, %`x#`]")
DEFINITION (U"the optional vector to start the iterations. If not given the procedure starts with the zero vector.")
TERM (U"%`maximumNumberOfIterations`")
DEFINITION (U"the maximum number of iterations that should be run if the tolerance criterion is not met.")
TERM (U"%`tolerance`")
DEFINITION (U"is a criterion value that is needed to decide when to stop the iterations. If %d(%n) is the squared approximation error "
	"in the %n-th step of the iteration, i.e. %d(%n) = ||#y \\-m #A\\.c#x(%n)||^^2^, where #x(%n) is the approximation of #x at step %n, "
	"then the iteration stops if either d(n) == 0 or")
DEFINITION (U"|%d(%n) - %d(%n-1)| / ||#y||^2 < %tolerance.")
TERM (U"%`infoLevel`")
DEFINITION (U"determines the amount of information that is written to the info window during iterations. No info is written if the value is zero.")
NORMAL (U"As an example consider:")
CODE (U"a## = {{-4, 2, 2}, ")
CODE (U"       { 2, 4, 2}, ")
CODE (U"       { 1, 1, 1},")
CODE (U"       { 2,-1, 3}}")
CODE (U"y# = {1,2,1,3}")
CODE (U"result# = solveNonnegative# (a##, y#,  100, 1e-17, 0)")
CODE (U"writeInfoLine: result#")
NORMAL (U"My info window shows:")
NORMAL (U"0.17687074830212887 0 0.8594104308385341")
NORMAL (U"")
NORMAL (U"The same %`a##` and %`y#` with extra output and only three iterations:")
CODE (U"result# = solveNonnegative# (a##, y#, 3, 1e-17, 2)")
CODE (U"writeInfoLine: result#")
NORMAL (U"Now my info window shows:")
NORMAL (U"Iteration: 1, error: 2.7083144168962345")
NORMAL (U"Iteration: 2, error: 0.22835187182198863")
NORMAL (U"Iteration: 3, error: 0.019415103584264275")
NORMAL (U"Number of iterations: 3; Minimum: 0.019415103584264275")
NORMAL (U"0.18686771227425772 0.0063553925295192215 0.8542134965490019")
NORMAL (U"The solution is not reached after only 3 iterations. We use the found solution to start a new iteration:")
CODE (U"result# = solveNonnegative# (a##, y#, result#, 100, 1e-17, 1)")
CODE (U"writeInfoLine: result#")
NORMAL (U"My info window shows")
NORMAL (U"Number of iterations: 6; Minimum: 0.011337868480725613")
NORMAL (U"0.17687074830212887 0 0.8594104308385341")
NORMAL (U"The final solution has been reached after 6 extra iterations.")

ENTRY (U"4.3 Constraints on the sparseness of the solution")
NORMAL (U"As we have seen above, if the number of columns is larger than the number of rows then a unique solution does not exist "
	"in general because the number of unknowns is larger than the number of equations. However, there is an exception:")
NORMAL (U"If the matrix #A has some special properties %and the solution vector has to be sparse, i.e. most of its values are zero, "
	"then we can find the #x that minimizes ||#y \\-m #A\\.c#x||^2.")
NORMAL (U"In general an iterative procedure is needed for the minimization. We have implemented one based on %%iterative hard "
	"thresholding% which is described by @@Blumensath & Davies (2010)@. ")
NORMAL (U"#`solveSparse#` (%`a##`, %`y#` [, %`x#`], %`maximumNumberOfNonzeros`, %`maximumNumberOfIterations`, %`tolerance`, %`infoLevel`)")
NORMAL (U"The parameters have the following meaning:")
TERM (U"%`a##`, %`y#`")
DEFINITION (U"the #A matrix and the #y vector.")
TERM (U"[, %`x#`]")
DEFINITION (U"the optional vector to start the iterations. If not given the procedure starts with the zero vector.")
TERM (U"%`maximumNumberOfNonzeros`")
DEFINITION (U"the maximum number of nonzero elements in the solution vector, i.e. the sparsity.")
TERM (U"%`maximumNumberOfIterations`")
DEFINITION (U"the maximum number of iterations that should be run if the tolerance criterion is not met.")
TERM (U"%`tolerance`")
DEFINITION (U"is a criterion value that is needed to decide when to stop the iterations. If %d(%n) is the squared approximation error "
	"in the %n-th step of the iteration, i.e. %d(%n) = ||#y \\-m ##A\\.cx#(%n)||^^2^, where #x(%n) is the approximation of #x at step %n, "
	"then the iteration stops if")
DEFINITION (U"|%d(%n) \\-m %d(%n\\-m1)| / ||#y||^2 < %tolerance.")
TERM (U"%`infoLevel`")
DEFINITION (U"determines the amount of information that is written to the info window during iterations. No info is written if the value is zero.")
NORMAL (U"In the following example we first construct a sparse vector #x, with random numbers between 0.1 and 10, and a random "
	"Gaussian matrix #A. From these two we construct our #y. We then solve the sparse system and compare its solution #xs to "
	"the constructed #x.")
CODE (U"nrow = 100")
CODE (U"ncol = 1000")
CODE (U"x# = zero# (ncol)")
CODE (U"for i to size (x#)")
	CODE1 (U"x# [i] = if randomUniform (0,1) < 0.005 then randomUniform (0.1, 10) else 0.0 fi")
CODE (U"endfor")
CODE (U"# On average in x# 5 out of 1000 will be unequal zero.")
CODE (U"a# = randomGauss## (nrow, ncol, 0.0, 1.0 / nrow)")
CODE (U"y# = mul# (a##, x#)")
CODE (U"maximumNumberOfNonzeros = 10")
CODE (U"maximumNumberOfIterations = 200")
CODE (U"tolerance = 1e-17")
CODE (U"info = 0   ; no info")
CODE (U"xs# = solveSparse# (a##, y#, maximumNumberOfNonzeros, maximumNumberOfIterations, tolerance, info)")
CODE (U"# We have found the solution now check")
CODE (U"dif# = x# - xs#")
CODE (U"inner = inner (dif#, dif#)")
CODE (U"writeInfoLine: if inner > 1e-7 then \"error\" else \"OK\" endif")
NORMAL (U"My info window shows: OK.")
MAN_END

MAN_BEGIN (U"SVD", U"djmw", 20220111)
INTRO (U"An object of type ##SVD# represents the @@singular value decomposition@ of a matrix.")
ENTRY (U"SVD internals")
NORMAL (U"Given #A, an %m \\xx %n matrix with %m >= %n, the decomposition will be #A = #U #D #V\\'p. ")
NORMAL (U"In the SVD object we store the %m \\xx %n matrix #U, the %n \\xx %n matrix #V and the %%n%-dimensional vector with the singular values. ")
NORMAL (U"If it happens that for the #A matrix %m < %n, i.e. the number of rows is less than the number of columns, then we store "
	"the SVD of the transpose of #A and flag this internally. "
	"In this way we can make sure that for the matrix #U the number of columns never exceeds the number of rows and at the same time that the dimension of the matrix #V never exceeds the dimension of the matrix #U. ")
NORMAL (U"For example the SVD of a 100 \\xx 20 matrix will result in a 100 \\xx 20 matrix #U, a 20 \\xx 20 matrix #V and 20 singular values, "
	"the SVD of a 20 \\xx 100 matrix will also result in a 100 \\xx 20 matrix #U, a 20 \\xx 20 matrix #V and 20 singular values, however it will be internally flagged as being transposed.")
MAN_END

MAN_BEGIN (U"SVD: Get minimum number of singular values...", U"djmw", 20171214)
INTRO (U"A command to get the minimum number of singular values (s.v.'s) whose sum, divided by the sum of all singular values, is smaller than the given fraction.")
ENTRY (U"Examples")
NORMAL (U"Given an SVD with four s.v's 10.0, 6.0, 3.0 and 1.0. The sum of the s.v's is 20.0.")
CODE (U"Get minimum number of singular values: 0.5")
DEFINITION (U"The returned value would be 1. The first s.v. divided by the sum is 0.5 (= 10.0 / 20.0). "
	"For any fraction lower than 0.5 the query would also return 1, because the first s.v. already covers half of the total sum.")
CODE (U"Get minimum number of singular values: 0.8")
DEFINITION (U"The returned value would be 2. The sum of first two s.v.'s divided by the sum is 0.8 (= (10.0 + 6.0) / 20.0). "
	" For any fraction between 0.5 and 0.8 the query would also return 2.")
CODE (U"Get minimum number of singular values: 0.95")
DEFINITION (U"The returned value would be 3. The sum of first three s.v.'s divided by the sum is 0.95 (= (10.0 + 6.0 + 3.0) / 20.0)."
	" For any fraction between 0.8 and 0.95 the query would also return 3.")
CODE (U"Get minimum number of singular values: 0.96")
DEFINITION (U"The returned value would be 4.")
CODE (U"Get minimum number of singular values: 0.99")
DEFINITION (U"The returned value would be 4.")
MAN_END

MAN_BEGIN (U"Sound & Pitch: Change speaker...", U"djmw", 20070722)
INTRO (U"A command to create a new Sound object with manipulated characteristics "
	"from the selected @Sound and @Pitch.")
NORMAL (U"With this command you can have finer grained control over the "
	"pitch than with the @@Sound: Change speaker...@ command. "
	"Accurate pitch measurement determines the quality of the "
	"@@overlap-add@ synthesis." )
ENTRY (U"Settings")
NORMAL (U"The settings are described in @@Sound: Change speaker...@. ")
MAN_END

MAN_BEGIN (U"Sound & Pitch: Change gender...", U"djmw", 20070722)
/* INTRO (U"Deprecated: use @@Sound & Pitch: Change speaker...@") */
NORMAL (U"A command to create a new Sound object with manipulated characteristics "
	"from the selected @Sound and @Pitch.")
NORMAL (U"With this command you can have finer grained control over the "
	"pitch than with the @@Sound: Change gender...@ command. "
	"Accurate pitch measurement determines the quality of the "
	"@@overlap-add@ synthesis." )
ENTRY (U"Settings")
NORMAL (U"The settings are described in @@Sound: Change gender...@. ")
MAN_END

MAN_BEGIN (U"Sound: Change gender...", U"djmw", 20030205)   // 2023
/* INTRO (U"Deprecated: use @@Sound: Change speaker...@") */
NORMAL (U"A command to create a new @Sound with manipulated characteristics.")
ENTRY (U"Settings")
NORMAL (U"The quality of the @@overlap-add|manipulation@ depends on the pitch measurement.")
NORMAL (U"The arguments that control the pitch measurement are:")
TERM (U"##Pitch floor (Hz)# (standard value: 75 Hz)")
DEFINITION (U"pitch candidates below this frequency will not be considered.")
TERM (U"##Pitch ceiling (Hz)# (standard value: 600 Hz)")
DEFINITION (U"pitch candidates above this frequency will be ignored.")
NORMAL (U"The arguments that control the manipulation are:")
TERM (U"##Formant shift ratio")
DEFINITION (U"determines the frequencies of the formants in the newly created "
	"Sound. If this ratio equals 1 no frequency shift will occur and "
	"the formant frequencies will not change. A ratio of 1.1 will change "
	"a male voice to a voice with approximate female formant characteristics. "
	"A ratio of 1/1.1 will change a female voice to a voice with approximate male formant "
	"characteristics.")
TERM (U"##New pitch median (Hz)# (standard value: 0.0 Hz, i.e. same as original)")
DEFINITION (U"determines what the median pitch of the new Sound will be. "
	"The pitch values in the newly created Sound will be calculated from the pitch "
	"values in the selected Sound by multiplying them by a factor "
	"%%newPitchMedian / oldPitchMedian%. This factor equals 1.0 if the default "
	"value for the new pitch median (0.0) is chosen. ")
TERM (U"##Pitch range factor# (standard value: 1.0)")
DEFINITION (U"determines an %extra% scaling of the new pitch values around the %new% "
	"pitch median. A factor of 1.0 means that no additional pitch modification will occur "
	"(except the obvious one described above). A factor of 0.0 monotonizes the new "
	"sound to the new pitch median.")
TERM (U"##Duration factor# (standard value: 1.0)")
DEFINITION (U"The factor with which the sound will be lengthened. The default is 1.0. "
	"If you take a value less than 1.0, the resulting sound will be shorter than "
	"the original. A value larger than 3.0 will not work.")
NORMAL (U"If you want more control over the synthesis you can supply your own "
	"Pitch object and use the @@Sound & Pitch: Change gender...@ command. ")
ENTRY (U"Algorithm")
NORMAL (U"The shifting of frequencies is done via manipulation of the sampling frequency. "
	"Pitch and duration changes are generated with @@overlap-add@ synthesis.")
NORMAL (U"The new pitch values are calculated in a two step process. We first multiply all "
	"the pitches with the factor %%newPitchMedian / oldPitchMedian% according to:")
EQUATION (U"%newPitch = %pitch * %newPitchMedian  / %oldPitchMedian.")
NORMAL (U"It follows that if the %newPitchMedian equals the %oldPitchMedian no "
	"change in pitch values will occur in the first step.")
NORMAL (U"Subsequently, the pitch range scale factor determines the final pitch values "
	"in the following linear manner:")
EQUATION (U"%finalPitch = %newPitchMedian + (%newPitch \\-- %newPitchMedian) * %pitchRangeScaleFactor")
NORMAL (U"Hence, it follows that no further scaling occurs if %pitchRangeScaleFactor "
	"equals 1.0.")
MAN_END

MAN_BEGIN (U"Sound: Change speaker...", U"djmw", 20080515)
INTRO (U"A command to create a new @Sound with manipulated characteristics.")
ENTRY (U"Settings")
NORMAL (U"The quality of the @@overlap-add|manipulation@ depends on the pitch measurement.")
NORMAL (U"The arguments that control the pitch measurement are:")
TERM (U"##Pitch floor (Hz)# (standard value: 75 Hz)")
DEFINITION (U"pitch candidates below this frequency will not be considered.")
TERM (U"##Pitch ceiling (Hz)# (standard value: 600 Hz)")
DEFINITION (U"pitch candidates above this frequency will be ignored.")
NORMAL (U"The arguments that control the manipulation are:")
TERM (U"##Multiply formants by")
DEFINITION (U"determines the formant frequencies of the newly created sound. "
	"The formant frequency of the new sound will equal the formant frequencies of the selected sound multiplied by this number. "
	"If this number equals 1, formant frequencies will not change. A number of 1.1 will change "
	"a male voice to a voice with approximate female formant characteristics. "
	"A ratio of 1/1.1 will change a female voice to a voice with approximate male formant "
	"characteristics.")
TERM (U"##Multiply pitch by")
DEFINITION (U"determines what the pitch of the new Sound will be. "
	"The pitch values of the new sound will equal the pitch values of the selected sound multiplied by this number. A value of 1.8 will  approximately change a male's pitch to a female's pitch.")
TERM (U"##Multiply pitch range by# (standard value: 1.0)")
DEFINITION (U"determines the pitch range of the newly created sound. "
	"A factor of 1.0 means that no additional pitch modification will occur "
	"(except the obvious one described above). A factor of 0.0 monotonizes the new "
	"sound to the new pitch median. A negative number inverses the pitch range with respect to the median.")
TERM (U"##Multiply duration by# (standard value: 1.0)")
DEFINITION (U"determines how to modify the duration of the newly created sound. "
	"A value of 1.0 means that the new sound will have the same duration as the selected sound. "
	"A value less than 1.0 will result in a shortened new sound. A value larger than 2.5 will not work.")
NORMAL (U"If you want more control over the synthesis you can supply your own "
	"Pitch object and use the @@Sound & Pitch: Change speaker...@ command. ")
ENTRY (U"Algorithm")
NORMAL (U"The shifting of formant frequencies is done via manipulation of the sampling frequency. "
	"To multiply all formants by a factor of 1.10 (i.e. raising them by 10 percent), a sampling "
	"frequency of 44100 Hz is first raised to 48510 Hz (without changing "
	"the samples). After this, the sound is lengthened by a factor of 1.10 "
	"and the pitch is lowered by a factor of 1.10, so that the original "
	"duration and pitch are restored. After this, the sound is resampled "
	"to 44100 Hz (by sinc interpolation)."
	"Pitch and duration changes are generated with @@overlap-add@ synthesis.")
MAN_END

MAN_BEGIN (U"Sound: Remove noise...", U"djmw", 20121122)
INTRO (U"A command to suppress noise in the selected @Sound.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (5), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Remove noise...", 5)
	Manual_DRAW_SETTINGS_WINDOW_RANGE(U"Noise time range (s)", U"0.0", U"0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD(U"Window length (s)", U"0.025")
	Manual_DRAW_SETTINGS_WINDOW_RANGE(U"Filter frequency range (Hz)", U"80.0", U"10000.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD(U"Smoothing (Hz)", U"40.0")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU(U"Noise reduction method", U"Spectral subtraction")
)
TERM (U"##Noise time range (s)")
DEFINITION (U"the start and end time of a noise part in the sound whose characteristics will be used in the denoising. "
	"If the end time is chosen before the start time, the noise fragment will be chosen automatically around a position "
	"where the intensity is minimal. For good noise suppression it is important that the noise fragment's duration is chosen "
	"several times the length of the window.")
TERM (U"##Window length (s)")
DEFINITION (U"denoising takes place in (overlapping) windows of this length.")
TERM (U"##Filter frequency range (Hz)")
DEFINITION (U"before denoising the sound will be @@Sound: Filter (pass Hann band)...|band-pass filtered@. ")
TERM (U"##Noise reduction method")
DEFINITION (U"The method of %%spectral subtraction% was defined in @@Boll (1979)@. The variant implemented is modelled "
	"after a script by Ton Wempe.")
MAN_END

MAN_BEGIN (U"Sound: Draw where...", U"djmw", 20170829)
INTRO (U"A command to draw only those parts of a @Sound where a condition holds.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (5.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw where...", 5.6)   // 0.6 extra for the text
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU("Drawing method", "Curve")
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("Draw only those parts where the following condition holds",
		"x < xmin + (xmax - xmin) / 2; first half")
)
TERM (U"##Time range (s)")
DEFINITION (U"selects the time domain for the drawing.")
TERM (U"##Vertical range")
DEFINITION (U"defines the vertical limits; larger amplitudes will be clipped.")
TERM (U"##Draw only those parts where the following condition holds#")
DEFINITION (U"determines the part of the sound that will be drawn. All parts where the formula evaluates to true will be drawn. "
	"This formula may ##not# contain references to the sampling of the sound, i.e. don't use 'col', 'x1', 'dx' and 'ncol' in it.")
ENTRY (U"Example 1")
NORMAL (U"The following script draws all amplitudes larger than one in red.")
CODE (U"Create Sound from formula: \"s\", \"Mono\", 0, 1, 2000, ~ 1.8*sin(2*pi*5*x)+randomGauss(0,0.1)")
CODE (U"Colour: \"Red\"")
CODE (U"Draw where: 0, 0, -2, 2, \"no\", \"Curve\", ~ abs(self)>1")
CODE (U"Colour: \"Black\"")
CODE (U"Draw where: 0, 0, -2, 2, \"yes\", \"Curve\", ~ not (abs(self)>1)")
SCRIPT (8, 3,
	U"Create Sound from formula: \"s\", \"Mono\", 0, 1, 2000, ~ 1.8*sin(2*pi*5*x)+randomGauss(0,0.1)\n"
	"Colour: \"Red\"\n"
	"Draw where: 0, 0, -2, 2, \"no\", \"Curve\", ~ abs(self)>1\n"
	"Colour: \"Black\"\n"
	"Draw where:  0, 0, -2, 2, \"yes\", \"Curve\", ~ not (abs(self)>1)\n"
	"Remove\n"
)
ENTRY (U"Example 2")
NORMAL (U"Draw the second half of a sound:")
CODE (U"Draw where: 0, 0, -1, 1, \"no\", \"Curve\", ~ x > xmin + (xmax - xmin) / 2")
ENTRY (U"Example 3")
NORMAL (U"Draw only positive amplitudes:")
CODE (U"Draw where: 0, 0, -1, 1, \"no\", \"Curve\", ~ self > 0")
ENTRY (U"Example 4")
NORMAL (U"Draw parts where pitch is larger than 300 Hz in red:")
CODE (U"s = selected (\"Sound\")")
CODE (U"p = To Pitch: 0, 75, 600")
CODE (U"pt = Down to PitchTier")
CODE (U"selectObject: s")
CODE (U"Colour: ~ Red")
CODE (U"Draw where: 0, 0, -1, 1, \"yes\", \"Curve\", ~ object (pt, x) > 300")
CODE (U"Colour: ~ Black")
CODE (U"Draw where: 0, 0, -1, 1, \"yes\", \"Curve\", ~ not (object (pt, x) > 300)")
MAN_END

MAN_BEGIN (U"Sound: Fade in...", U"djmw", 20170829)
INTRO (U"A command to gradually increase the amplitude of a selected @Sound.")
ENTRY (U"Settings")
TERM (U"##Channel")
DEFINITION (U"determines whether you want to fade all channels or only a selected channel.")
TERM (U"##Time (s)")
DEFINITION (U"determines where the fade-in will take place. If %time is earlier than the start time of the sound, "
	"the start time of the sound wil be used.")
TERM (U"##Fade time (s)")
DEFINITION (U"determines the start point and the endpoint of the fade-in with respect to the %time argument. "
	"Depending on the sign of %%fadeTime%, %time is either the start or the end position of the fade-in. If %%fadeTime% "
	"is positive, fade-in will take place between %%time% and %%time+fadeTime%. If %%fadeTime% is negative, "
	"fade-in wil take place between %%time+fadeTime% and %time.")
TERM (U"##Silent from start")
DEFINITION (U"when on, makes the sound silent before the fade-in starts. "
	"When off, the sound before the fade-in starts will not be changed. ")
ENTRY (U"Algorithm")
NORMAL (U"Multiplication with the first half period of a (1-cos(x))/2 function. ")
ENTRY (U"Cross-fading two sounds")
NORMAL (U"The following script cross-fades two sounds s1 and s2 at time 1 second and leaves the result in s2.")
CODE1 (U"crossFTime = 0.5")
CODE1 (U"t = 1")
CODE1 (U"s = Create Sound from formula: \"s1\", 1, 0, 2, 44100, ~ sin(2*pi*500*x)")
CODE1 (U"Fade out: 0, t-crossFTime/2, crossFTime, \"yes\"")
CODE1 (U"Create Sound from formula: \"s2\", 1, 0, 2, 44100, ~ sin(2*pi*1000*x)")
CODE1 (U"Fade in.: 0, t-crossFTime/2, crossFTime, \"yes\"")
CODE1 (U"Formula: ~ self + object [s]")
MAN_END

MAN_BEGIN (U"Sound: Fade out...", U"djmw", 20121010)
INTRO (U"A command to gradually decrease the amplitude of a selected @Sound.")
ENTRY (U"Settings")
TERM (U"##Channel")
DEFINITION (U"determines whether you want to fade all channels or only a selected channel.")
TERM (U"##Time (s)")
DEFINITION (U"determines where the fade-out will take place. If %time is later than the end time of the sound, "
	"the end time of the sound wil be used.")
TERM (U"##Fade time (s)")
DEFINITION (U"determines the start point and the endpoint of the fade-out with respect to the %time argument. "
	"Depending on the sign of %%fadeTime%, %time is either the start or the end position of the fade-out. If %%fadeTime% "
	"is positive, fade-out will take place between %%time% and %%time+fadeTime%. If %%fadeTime% is negative, "
	"fade-out wil take place between %%time+fadeTime% and %time.")
TERM (U"##Silent to end")
DEFINITION (U"Make the sound silent after the fade-out finishes. ")
ENTRY (U"Algorithm")
NORMAL (U"Multiplication with the first half period of a (1+cos(%%x%))/2 function.")
MAN_END

MAN_BEGIN (U"Sound: Filter (gammatone)...", U"djmw", 20170829)
INTRO (U"A command to filter a Sound by a fourth-order gammatone bandpass filter.")
ENTRY (U"Settings")
TERM (U"##Centre frequency (Hz)#, ##Bandwidth (Hz)#")
DEFINITION (U"determine the passband of the filter.")
ENTRY (U"Algorithm")
NORMAL (U"The impulse response of the filter is a 4-th order @@gammatone@. The "
	"filter is implemented as the convolution of the gammatone with the sound. "
	"The gain of the filter is scaled to unity at the centre frequency.")
ENTRY (U"Remark")
NORMAL (U"The old implementation with a simple 8-th order recursive digital filter with "
	"4 zeros and 8 poles (these 8 poles consist of one conjugate pole pair to the "
	"4-th power) as suggested by  @@Slaney (1993)@ was not stable for low frequencies. ")
MAN_END

MAN_BEGIN (U"Sound: Play as frequency shifted...", U"djmw", 20230801)
INTRO (U"Plays the selected @Sound with all frequencies shifted by the same amount. This trick can be used to make "
	"audible those sounds that are normally not audible at all by human beings, like for example ultrasounds or infrasounds.")
ENTRY (U"Settings")
TERM (U"##Shift by (Hz)")
DEFINITION (U"the amount by which frequencies are shifted. A positive number shifts frequencies up, a negative number "
	"shifts frequencies down. ")
ENTRY (U"##Example")
NORMAL (U"Rodents produce sounds with frequencies far outside the human audible range. Some meaningfull squeaks of these animals "
	"are present in the frequency range from 54 kHz up to sometimes 100kHz. By choosing a shift value of -54000 Hz and a sampling "
	"frequency of 44100 Hz, all frequencies between 54000 Hz and (54000+22050=) 76050 Hz  will be shifted down by 54000 Hz. The "
	"rodents’ frequencies in the interval from 54000 Hz to 76050 Hz will therefore be mapped to the frequency interval between 0 and 22050 Hz. ")
MAN_END

MAN_BEGIN (U"Sound: To BarkSpectrogram...", U"djmw", 20141023)
INTRO (U"A command that creates a @BarkSpectrogram object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters.")
NORMAL (U"The auditory filter functions used are defined as:")
EQUATION (U"10 log %#H(%z) = 7 - 7.5 * (%z__%c_ - %z - 0.215) - 17.5 * \\Vr "
	"(0.196 + (%z__%c_ - %z - 0.215)^2)")
NORMAL (U"where %z__%c_ is the central (resonance) frequency of the filter in Bark. "
	"The bandwidths of these filters are constant and equal 1 Bark. ")
NORMAL (U"The auditory filters are defined in @@Sekey & Hanson (1984)@. You can draw these filters from "
	"a BarkSpectrogram object by selecting @@BarkSpectrogram: Draw Sekey-Hanson auditory filters...@.")
MAN_END

MAN_BEGIN (U"Sound: To FormantFilter...", U"djmw", 20141024)
INTRO (U"A #deprecated command that creates a @FormantFilter object from every selected @Sound object by "
	"@@band filtering in the frequency domain@ with a bank of filters whose bandwidths depend on the pitch of the signal.")
NORMAL (U"The analysis proceeds in two steps:")
LIST_ITEM (U"1. We perform a pitch analysis (see @@Sound: To Pitch...@ for details).")
LIST_ITEM (U"2. We perform a filter bank analysis on a linear frequency scale. The bandwidth of the filters depends on "
	"the measured pitch (see @@Sound & Pitch: To Spectrogram...@ for details).")
MAN_END

MAN_BEGIN (U"Sound: To Spectrogram (pitch-dependent)...", U"djmw", 20141024)
INTRO (U"A command that creates a @Spectrogram object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of formant filters whose bandwidths vary with the local pitch of the signal.")
NORMAL (U"The analysis proceeds in two steps:")
LIST_ITEM (U"1. We perform a pitch analysis (see @@Sound: To Pitch...@ for details).")
LIST_ITEM (U"2. We perform a filter bank analysis on a linear frequency scale. "
	"The bandwidth of the filters depends on the measured pitch (see @@Sound & Pitch: To Spectrogram...@ for details).")
MAN_END

MAN_BEGIN (U"Sound: Paint where...", U"djmw", 20170829)
INTRO (U"A command to paint only those parts of a @Sound where a condition holds. The painted area is the area "
	"between the Sound and a horizontal line at a certain level.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (6.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Paint where...", 6.6)   // 0.6 extra for the text
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colour (0-1, name, {r,g,b})", "0.5")
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Fill from level", "0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("Paint only those parts where the following condition holds",
		"1; always")
)
TERM (U"##Colour")
DEFINITION (U"defines the @@Colour|colour@ of the paint.")
TERM (U"##Time range (s)")
DEFINITION (U"selects the time domain for the drawing.")
TERM (U"##Vertical range")
DEFINITION (U"defines the vertical limits; larger amplitudes will be clipped.")
TERM (U"##Fill from level")
DEFINITION (U"defines the level of the horizontal line. ")
TERM (U"##Formula")
DEFINITION (U"determines the part of the sound that will be painted. All parts where the formula evaluates to true will be painted. "
	"This formula may ##not# contain references to the sampling of the sound, i.e. don't use 'col', 'x1', 'dx' and 'ncol' in it.")
ENTRY (U"Example 1")
NORMAL (U"The following script paints the area under a sine curve in red and the area above in green."
	"For the first paint the horizontal line is at y=-1, for the second paint the line is at y=+1. "
	"The formula always evaluates to true.")
CODE (U"s = Create Sound from formula: \"s\", 1, 0, 1, 10000, ~ 0.5*sin(2*pi*5*x)")
CODE (U"Paint where: \"Red\", 0, 0, -1, 1, -1, \"yes\", ~ 1")
CODE (U"Paint where: \"Green\", 0, 0, -1, 1, 1, \"no\", ~ 1 ")
SCRIPT (8, 5,
	U"s = Create Sound from formula: \"s\", 1, 0, 1, 10000, ~ 0.5*sin(2*pi*5*x)\n"
	"Paint where: \"Red\", 0, 0, -1, 1, -1, \"no\", ~ 1\n"
	"Paint where: \"Green\", 0, 0, -1, 1, 1, \"yes\", ~ 1\n"
	"Remove\n")
ENTRY (U"Example 2")
NORMAL (U"The following script paints the area below zero in red and the area above in green."
	"The horizontal line is now always at y=0 and we use the formula to differentiate the areas.")
CODE (U"s = Create Sound from formula: \"s\", 1, 0, 1, 10000, ~ 0.5*sin(2*pi*5*x)")
CODE (U"Paint where: \"Red\", 0, 0, -1, 1, 0, \"no\", ~ self > 0")
CODE (U"Paint where: \"Green\", 0, 0, -1, 1, 0, \"yes\", ~ self < 0")
SCRIPT (8, 5,
	U"s = Create Sound from formula: \"s\", 1, 0, 1, 10000, ~ 0.5*sin(2*pi*5*x)\n"
	"Paint where: \"Red\", 0, 0, -1, 1, 0, \"no\", ~ self < 0\n"
	"Paint where: \"Green\", 0, 0, -1, 1, 0, \"yes\", ~ self > 0\n"
	"removeObject: s\n")
ENTRY (U"Example 3")
NORMAL (U"To give an indication that the area under a 1/x curve between the points %a and %b and the area "
	"between %c and %d are equal if %b/%a = %d/%c. For example, for %a=1, %b=2, %c=4 and %d=8: ")
CODE (U"Create Sound from formula: \"1dx\", \"Mono\", 0, 20, 100, ~ 1.0 / x ")
CODE (U"Draw: 0, 20, 0, 1.5, \"yes\", \"Curve\"")
CODE (U"Paint where: \"Grey\", 0, 20, 0, 1.5, 0, \"yes\", ~ (x >= 1 and x < 2) or (x >= 4 and x < 8)")
CODE (U"One mark bottom: 1, \"yes\", \"yes\", \"no\", \"\"")
CODE (U"One mark bottom: 2, \"yes\", \"yes\", \"no\", \"\"")
CODE (U"One mark bottom: 4, \"yes\", \"yes\", \"no\", \"\"")
CODE (U"One mark bottom: 8, \"yes\", \"yes\", \"no\", \"\"")
SCRIPT (8, 5,
	U"s = Create Sound from formula: \"1dx\", \"Mono\", 0, 20, 100, ~ 1.0 / x\n"
	"Draw: 0, 20, 0, 1.5, \"yes\", \"Curve\"\n"
	"Paint where: \"Grey\", 0, 20, 0, 1.5, 0, \"yes\", ~ (x >= 1 and x < 2) or (x >= 4 and x < 8)\n"
	"One mark bottom: 1, \"yes\", \"yes\", \"no\", \"\"\n"
	"One mark bottom: 2, \"yes\", \"yes\", \"no\", \"\"\n"
	"One mark bottom: 4, \"yes\", \"yes\", \"no\", \"\"\n"
	"One mark bottom: 8, \"yes\", \"yes\", \"no\", \"\"\n"
	"removeObject: s\n")
MAN_END

MAN_BEGIN (U"Sounds: Paint enclosed...", U"djmw", 20170829)
INTRO (U"Paints the area between the two selected @@Sound@s. ")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sounds: Paint enclosed", 4)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colour (0-1, name, {r,g,b})", "0.5")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
)
TERM (U"##Colour")
DEFINITION (U"defines the @@Colour|colour@ of the paint.")
TERM (U"##Time range (s)")
DEFINITION (U"selects the time domain for the drawing.")
TERM (U"##Vertical range")
DEFINITION (U"defines the vertical limits, larger amplitudes will be clipped.")
ENTRY (U"Example")
NORMAL (U"The following script paints the area enclosed between a sine tone of 5 Hz and the straight line %y = %x/2.")
CODE (U"s1 = Create Sound from formula: \"sine\", \"Mono\", 0, 1, 10000, ~ 1/2 * sin(2*pi*5*x)\"")
CODE (U"s2 = Create Sound from formula: \"line\", \"Mono\", 0, 1, 10000, ~ x / 2")
CODE (U"plusObject (s1)")
CODE (U"Paint enclosed: \"Grey\", 0, 0, -1, 1, ~ yes")
SCRIPT ( 4, 2,
	 U"s1 = Create Sound from formula: \"sine\", \"Mono\", 0, 1, 10000, ~ 1/2 * sin(2*pi*5*x)\n"
	"s2 = Create Sound from formula: \"line\", \"Mono\", 0, 1, 10000, ~ x / 2\n"
	"selectObject: s1, s2\n"
	"Paint enclosed: \"Grey\", 0, 0, -1, 1, \"yes\"\n"
	"removeObject: s1, s2\n")
MAN_END

MAN_BEGIN (U"Sound: To Polygon...", U"djmw", 20170829)
INTRO (U"A command that creates a @@Polygon@ from a selected @@Sound@, where the Polygon's "
	" points are defined by the (%time, %amplitude) pairs of the sound. ")
ENTRY (U"Settings")
TERM (U"##Channel")
DEFINITION (U"defines which channel of the sound is used.")
TERM (U"##Time range (s)")
DEFINITION (U"defines the part of the sound whose (%time, %amplitude) pairs have to be included.")
TERM (U"##Vertical range")
DEFINITION (U"defines the vertical limits, larger amplitudes will be clipped.")
TERM (U"##Connection y-value")
DEFINITION (U"defines the y-value of the first and last point of the Polygon. This gives the opportunity to "
	" draw a closed Polygon with the horizontal connection line at any position you like. ")
ENTRY (U"Example")
NORMAL (U"The following script paints the area under a sound curve in red and the area above in green.")
CODE (U"s = Create Sound from formula: \"s\", 1, 0, 1, 10000, ~ 0.5*sin(2*pi*5*x)")
CODE (U"# Connection y-value is at amplitude -1: area under the curve.")
CODE (U"p1 = To Polygon: 1, 0, 0, -1, 1, -1")
CODE (U"Paint: \"{1,0,0}\", 0, 0, -1, 1")
CODE (U"selectObject: s")
CODE (U"# Connection y-value is now at amplitude 1: area above the curve.")
CODE (U"p2 = To Polygon: 1, 0, 0, -1, 1, 1")
CODE (U"Paint: \"{0,1,0}\", 0, 0, -1, 1")
SCRIPT (4.5, 2,
	U"s = Create Sound from formula: \"s\", 1, 0, 1, 10000, ~ 0.5*sin(2*pi*5*x)\n"
	"p1 = To Polygon: 1, 0, 0, -1, 1, -1\n"
	"Paint: \"{1,0,0}\", 0, 0, -1, 1\n"
	"selectObject: s\n"
	"p2 = To Polygon: 1, 0, 0, -1, 1, 1\n"
	"Paint: \"{0,1,0}\", 0, 0, -1, 1\n"
	"removeObject: p2, p1, s\n"
)
MAN_END

#define xxx_to_TextGrid_detectSilences_COMMON_PARAMETERS_HELP \
TERM (U"##Silence threshold (dB)") \
DEFINITION (U"determines the maximum silence intensity value in dB with respect to the maximum " \
	"intensity. For example, if %imax is the maximum intensity in dB then the maximum silence " \
	"intensity is calculated as %%imax - silenceThreshold%; intervals with an intensity smaller " \
	"than this value are considered as silent intervals.") \
TERM (U"##Minimum silent interval (s)") \
DEFINITION (U"determines the minimum duration for an interval to be considered as silent. " \
	"If you don't want the closure for a plosive to count as silent then use a large enough value.") \
TERM (U"##Minimum sounding interval (s)") \
DEFINITION (U"determines the minimum duration for an interval to be ##not# considered as silent. " \
	"This offers the possibility to filter out small intense bursts of relatively short duration.") \
TERM (U"##Silent interval label") \
DEFINITION (U"determines the label for a silent interval in the TextGrid.") \
TERM (U"##Sounding interval label") \
DEFINITION (U"determines the label for a sounding interval in the TextGrid.")

MAN_BEGIN (U"Sound: To TextGrid (silences)...", U"djmw", 20160406)
INTRO (U"A command that creates a @TextGrid in which the silent and sounding intervals of the selected @Sound are marked.")
ENTRY (U"Settings")
xxx_to_TextGrid_detectSilences_COMMON_PARAMETERS_HELP
ENTRY (U"Algorithm")
NORMAL (U"First a copy of the sound is @@Sound: Filter (pass Hann band)...|bandpass filtered@ between 80 and 8000 Hz to "
	"remove especially the low frequency noise that can have a significant influence on the intensity measurement but does not "
	"really contribute to the sound. Next the @@Sound: To Intensity...|intensity of the filtered sound@ is determined. "
	"Finally the silent and sounding intervals are determined @@Intensity: To TextGrid (silences)...|from the intensity curve@.")
MAN_END

MAN_BEGIN (U"Sound: To TextGrid (speech activity)...", U"djmw", 20260313)
INTRO (U"An old command; the same as @@Sound: To TextGrid (speech activity, LTSF)...@.")
NORMAL (U"The name changed in Praat 6.4.62, because there is also @@Sound: To TextGrid (speech activity, Silero)...@.")
MAN_END


MAN_BEGIN (U"Sound: To TextGrid (speech activity, LTSF)...", U"djmw", 20231209)
INTRO (U"A command that creates a @@TextGrid@ for the selected @@Sound@ in which the non-speech intervals and the "
	"intervals with speech activity are marked. The discrimination between the two is based on a spectral flatness measure.")
NORMAL (U"Speech activity detection, in the technical literature often referred to as voice activity detection, "
	"is a method to discriminate speech segments from input noisy speech. "
	"According to the article of @@Ma & Nishihara (2013)@, spectral flatness is a measure of the width, uniformity, "
	"and noisiness of the power spectrum. A high spectral flatness indicates that the spectrum has a similar amount "
	"of power in all spectral bands, and the graph of the spectrum would appear relatively flat and smooth; "
	"A low spectral flatness indicates that the spectral power is less uniform, and this would be more typical "
	"for speech-like sounds. In general speech is a highly non-stationary signal while background noise can be "
	"considered stationary over relatively longer periods of time.  ")
NORMAL (U"Because the spectral flatness measure is completely "
	"independent of the overall intensity of the sound we have added the possibility to also discriminate on "
	"intensity.")
ENTRY (U"Settings")
TERM (U"##Time step (s)#")
DEFINITION (U"determines the time interval between consecutive measurements of the spectral flatness measure. ")
TERM (U"##Long term window (s)#")
DEFINITION (U"determines the window duration for the calculation of the long term spectral flatness measure. "
	"According to ##Ma & Nishihara (2013)# a value of approximately 0.3 s performed best on average for a "
	"number of different noise conditions.")
TERM (U"##Short term window (s)#")
DEFINITION (U"determines the interval for averaging spectral estimates. "
	"According to ##Ma & Nishihara (2013)# a value of approximately 0.1 s performed best on average for a "
	"number of different noise conditions.")
TERM (U"##Frequency range (Hz)#")
DEFINITION (U"determines the frequency range used in the calculation of the spectral flatness measure. "
	"Ma & Nishihara (2013) used a range from 400 to 4000 Hz. Because fricatives tend to have strong components "
	"above 4000 Hz we increased the default value to 6000 Hz. In this way the fricative's intensity, which is "
	"calculated from this range, becomes higher and because of this a fricative is less likely to be skipped "
	"by a selection on the non-speech threshold. We also decreased the lower value from 400 to 70 Hz. "
	"In this way we increase chances that sounds at start or end positions with mainly low frequency components, "
	"like nasals, are detected.")
TERM (U"##Flatness threshold#")
DEFINITION (U"determines whether a frame is considered %%speech% or not, based on a spectral flatness measure. "
	"Values of the flatness below the threshold are considered speech.")
TERM (U"##Non-speech threshold (dB)#")
DEFINITION (U"also determines whether a frame is considered %%speech% or not, but based on intensity. "
	"Intervals with an intensity smaller than this value below the sound's maximum intensity value "
	"are considered as %%non-speech% intervals. The intensity is calculated from the frequency range defined above. ")
TERM (U"##Minimum non-speech interval duration (s)#")
DEFINITION (U"determines the minimum duration for an interval to be considered as non-speech. "
	"If you don't want the closure for a plosive to count as non-speech then use a large enough value.")
TERM (U"##Minimum speech interval (s)") \
DEFINITION (U"determines the minimum duration for an interval to be considered as speech. "
	"This offers the possibility to filter out small intense bursts of relatively short duration.")
TERM (U"##Speech / Non-speech interval label#")
DEFINITION (U"determine the labels for the corresponding intervals in the newly created TextGrid.")
ENTRY (U"Algorithm")
NORMAL (U"The speech activity algorithm is described in @@Ma & Nishihara (2013)@.")
NORMAL (U"The logarithm of the speech flatness at frame %m is defined as:")
EQUATION (U"L (m) = \\Si__k_ log (GM(%m, %f__%k_) / AM (%m, %f__%k_)), ")
NORMAL (U" where GM(%m, %f__%k_) and AM (%m, %f__%k_) are the geometric and arithmetic means for spectrum "
	"component %f__%k_, respectively. "
	"The geometric mean GM (%m, %f__%k_) is defined as ")
EQUATION (U"GM(%m, %f__%k_) = {\\Pi^^%m^__%n=%m-%R+1_ %S(%n, %f__%k_)}^^(1/%R)^")
NORMAL (U" where the number of frames %R is determined by the setting of the %%long term window% parameter. "
	"AM(%m, %f__%k_) is defined as ")
EQUATION (U"AM(%m, %f__%k_) = {\\Si^^m^__n=%m-%R+1_ %S(%n, %f__%k_)} / %R")
NORMAL (U"The %short term window comes into play in the definition of the %S(%n, %f__%k_), because this is "
	"itself the average of %M local spectral frames")
EQUATION (U"%S(%n, %f__%k_) = {\\Si^^M^__p=%m-%M+1_ |X(p, %f__%k_)|^^2^} / %M,")
NORMAL (U"where the number of frames %M is determined by the setting of the %%short term window% length. ")
NORMAL (U"The ratio between the geometric and arithmetic mean is always smaller than or equal to one. "
	"Only when all numbers are equal, this means a flat spectrum, the ratio becomes equal to one. ")
MAN_END

MAN_BEGIN (U"Intensity: To TextGrid (silences)...", U"djmw", 20061201)   // 2023
INTRO (U"A command that creates a @TextGrid in which the silent and sounding intervals of the selected @Intensity are marked.")
ENTRY (U"Settings")
xxx_to_TextGrid_detectSilences_COMMON_PARAMETERS_HELP
ENTRY (U"Algorithm")
NORMAL (U"First the intensity contour is evaluated and the intervals above and below the silence threshold are marked as "
	"%sounding and %silent. "
	"We then remove sounding intervals with a duration smaller than the %%Minimum sounding interval duration%. "
	"This step is followed by joining the neighbouring silent intervals that resulted because of this removal. "
	"Finally we remove silent intervals with a duration smaller than the %%Minimum silent interval duration%. "
	"This is followed by joining the neighbouring sounding intervals that resulted because of this removal.")
NORMAL (U"Experience showed that first removing short intensity bursts instead of short silences gave better results than doing it the other way around.")
ENTRY (U"Important")
NORMAL (U"The effectiveness of the %%Minimum silent interval duration% and %%Minimum sounding interval duration% "
	"depends on the effective window length that was used to determine the intensity contour. "
	"For example, if you have chosen 100 Hz for the “Pitch floor” parameter in the @@Sound: To Intensity...@ analysis, "
	"the effective window length was 32 ms. Don't expect to find sounding "
	"or silent intervals with a duration smaller than this effective window length.")
MAN_END

MAN_BEGIN (U"ConstantQLogFSpectrogram", U"djmw", 20211111)
INTRO (U"A ##ConstantQLogFSpectrogram# is a time-frequency representation of a sound that deviates from a traditional @Spectrogram in two important ways. ")
LIST_ITEM (U"1. The frequency scale is logarithmic which means that the distances between successive "
	"frequencies are equal on a logarithmic scale.")
LIST_ITEM (U"2. The frequency analysis is performed with filters that have a constant quality factor which essencially "
	"means that we are using the same number of periods in the analysis of each frequency bin and this results in longer "
	"analysis windows at low frequencies and shorter analysis windows at higher frequencies. As a result the sampling of each "
	"frequency bin is different, hence it is %%multi-sampled%.")
NORMAL (U"One of the characteristics of a ##ConstantQLogFSpectrogram#'s representation is that the number of "
	"frequency bins in an octave is constant. If, for example, the number of frequency bins per octave is 24 this "
	"means that the number "
	"of frequency bins in the interval from 100 to 200 Hz equals 24 and that the number of frequency bins in the intervals "
	"from 200\\--400 Hz, 400\\--800 Hz, 800\\--1600 Hz, 1600\\--3200 Hz, etc. also equal 24. This behaviour mimics the "
	"layout of the frequency sensitivity of the human basilar membrane much better than a traditional @@Spectrogram@.")
NORMAL (U"The frequency of the %k^^th^ frequency bin can be calculated as:")
EQUATION (U"%f__%k_ = %%lowestFrequency%\\.c2^^(%k-1)/%%numberOfFrequencyBinsPerOctave%). ")
NORMAL (U"A big advantage of the ##ConstantQLogFSpectrogram# over a traditional #Spectrogram is its invertability: we can create the original sound back from it. This provides a way to manipulate a sound in the frequency domain. ")
MAN_END

MAN_BEGIN (U"Sound: To ConstantQLogFSpectrogram...", U"djmw", 20211111)
INTRO (U"A command that creates a @@ConstantQLogFSpectrogram@ from a selected @@Sound@.")
ENTRY (U"Settings")
TERM (U"##Lowest frequency (Hz)")
DEFINITION (U"defines the lowest frequency about which information will be obtained. This frequency has to be chosen well above "
	"zero hertz (because of the logarithmic frequency scale there are an infinite number of octaves between any lowest frequency "
	"and zero hertz). ")
TERM (U"##Maximum frequency (Hz)")
DEFINITION (U"The maximum frequency you are interested in. The default will be the Nyquist frequency of the sound. ")
TERM (U"##Number of frequecy bins / octave")
DEFINITION (U"determines the frequency division as the number of frequency bins that will be used for each octave. "
	"For the analysis of music generally a multiple of 12 will be used.")
TERM (U"##Frequency resolution (bins)")
DEFINITION (U"determines, together with the number of frequency bins per octave, the %%quality factor% used in the analysis."
	"The quality factor Q of a filter is defined as its "
	"central frequency divided by its bandwidth. The bandwith of a filter, i.e its domain, will be be set as twice the "
	"frequency resolution in bins. Therefore, the corresponding quality factor will be: ")
EQUATION (U"%%qualityFactor% = 1 / (2^^(%%frequencyResolutionBins% / %%numberOfFrequencyBinsPerOctave%)^ - 2^^(\\--%%frequencyResolutionBins% / %%numberOfFrequencyBinsPerOctave%)^). ")
DEFINITION (U"For a frequency resolution of 1 bin and a frequency division of 24 frequency bins per octave we get a "
	"quality factor of 14.26, for a frequency division of 48 bins per octave we get a quality factor of 34.6. "
	"The larger the quality factor the more frequency selective the filters will be and consequently, in the time domain, "
	"the longer their analysis window has to be. (Strictly speaking, the bandwidth of a filter is defined as the "
	"width of the filter at "
	"an intensity -3 dB below its top. The actual bandwidth depends on the form of the used filter function.)")
TERM (U"##Time oversampling factor")
DEFINITION (U"determines the number of frames in each frequency bin. Any number larger than 1 will increase the number of frames in a bin by approximately this factor.  ")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm for constructing the invertable constant-Q transform is described in @@Velasco et al. (2011)@ and in @@Holighaus et al. (2013)@. ")
NORMAL (U"First the sound is transformed to the frequency domain with an FFT. The filtering is then performed in the frequency "
	"domain. For the %k^^th^ frequency bin the frequencies between %%lowestFrequency%\\.c2^^(%k-1)/%%numberOfFrequencyBinsPerOctave%)^ and  %%lowestFrequency%\\.c2^^(%k+1)/%%numberOfFrequencyBinsPerOctave%)^ are transformed back with an inverse @FFT. The resulting coefficients are copied to the frames of the frequency bin. Because of the logarithmic frequency scale, the number of coefficients in a frequency bin will increase with bin number. ")
NORMAL (U"")
MAN_END

MAN_BEGIN (U"Sound: To Sound (derivative)...", U"djmw", 20230801)
INTRO (U"Calculates the derivative of a @@Sound@.")
ENTRY (U"Settings")
TERM (U"##Low-pass frequency (Hz)")
DEFINITION (U"defines the highest frequency to keep in the derivative. Because taking a derivative is comparable to "
	"multiplying the strength of each frequency component by its frequency value it has the effect of high-pass filtering. "
	"E.g. a 10000 Hz component is amplified 100 times stronger than a 100 Hz component. "
	"Low-pass filtering then becomes essential for removing high-frequency noise.  ")
TERM (U"##Smoothing (Hz)")
DEFINITION (U"defines the width of the transition area between fully passed and fully suppressed "
	"frequencies. Frequencies below %%lowpassFrequency% will be fully passed, frequencies larger "
	"than %%lowpassFrequency%+%%smoothing% will be fully suppressed.")
TERM (U"##New absolute peak")
DEFINITION (U"the new absolute peak of the derivative. By specifying a value smaller than 1.0 the derivative can be made audible "
	"without distortion. If you want to listen to the derivative without distortion, it is absolutely necessary to scale the "
	"peak to a value somewhat smaller than 1.0, like 0.99. For example, for a pure sine tone with a frequency of 300 Hz "
	"and an amplitude of 1.0 whose formula is %%s(t) = sin(2\\pi300t)% the derivative with respect to time %t% is %%2\\pi300 cos(2\\pi300t)% ."
	"The result is a cosine of 300 Hz with a huge amplitude of %%2\\pi300%. You can prevent any scaling by supplying a value of 0.0.")
ENTRY (U"Algorithm")
NORMAL (U"The derivative of a wave form %%x%(%%t%) is most easily calculated in the spectral domain. According to "
	"Fourier theory, if %%x%(%%t%) = \\in%%X%(%%f%)exp(2\\pi%%ift%) %%dt%, then"
	" d%%x%(%%t%)/d%%t% = \\in%%X(%%f%)2\\pi%%if% exp(2\\pi%%ift%)d%%t%, where %%X%(%%f%) is the spectrum "
	"of the %%x%(%%t).")
NORMAL (U"Therefore, by taking the spectrum of the signal and from this spectrum calculate new real and "
	"imaginary components and then transform back to the time domain we get the derivative.")
NORMAL (U"The multiplication of the spectral components with the factor 2\\pi%%if% will result in a new "
	"%%X%\\'p(%%f%) whose components will be: Re(%%X\\'p%(%%f%)) = -2\\pi%%f% Im (%%X%(%%f%)) and Im(%%X\\'p%(%%f%)) =2\\pi%%f% Re(%%X%(%%f%)).")
MAN_END

MAN_BEGIN (U"Spectrum: To Sound (resampled)...", U"djmw", 20220105)
INTRO (U"A command that creates a @Sound from the selected @Spectrum by using a fast approximation of the inverse of the Discrete Fourier Transform (DFT).")
NORMAL (U"For more details see @@Sound: To Spectrum (resampled)...@.")
MAN_END

MAN_BEGIN (U"Sound: To Spectrum (resampled)...", U"djmw", 20220105)
INTRO (U"A command that creates a @Spectrum from the selected @Sound by using a fast approximation of the Discrete Fourier Transform (DFT).")
NORMAL (U"In general the amount of computation necessary to calculate the spectrum of a sound that consists "
	"of %N samples, is of the order of %O(%N log %N) multiplications. If the number of samples happens to be an "
	"exact power of 2, i.e. %N=2^^p^ and %p integer, a special algorithm called the FFT (Fast Fourier Transform) is "
	"available to calculate the spectrum very efficiently. In normal situations, however, the number of "
	"samples seldom happens to be an exact power of 2 and the calculation of the spectrum then proceeeds much slower, especially if "
	"%N happens to be a prime number a naive implementation of the DFT would calculate the spectrum in order O(%N^^2^) time. "
	"Extending the sound with zero sample values until the number of samples reaches a power of 2 enables us to use the fast "
	"FFT algorithm to calculate a fast approximation of the real spectrum. "
	"This is the traditional way to calculate the spectrum if you had chosen ##To Spectrum...# with the %%fast% option on. ")
NORMAL (U"However, there is another option to get a sound with a number of samples that equals a power of 2, "
	"namely by upsampling the sound with a suitably chosen sampling frequency. We have to calculate the new sampling "
	"frequency such that the number of samples in the upsampled sound is exactly a power of 2. Of the new upsampled "
	"sound we can use the FFT algorithm to calculate its spectrum %%without the need to add zero sample values%. "
	"Because the upsampling results in a spectrum that "
	"contains higher frequency components than the spectrum of the original sound we have to process the just calculated spectrum by leaving out these higher frequency components to obtain the desired spectrum.")
NORMAL (U"This resampled approximation generally performs better than the approximation by adding zero values.")
ENTRY (U"Settings")
TERM (U"##Precision#,")
DEFINITION (U"the depth of the interpolation in samples. This determines the quality of the interpolation used "
	"in resampling.")
ENTRY (U"Example")
NORMAL (U"The following script shows the three different ways to calculate a Spectrum from a given sound. We deliberately have chosen the number of samples to be prime.")
CODE (U"sound = Create Sound from formula: \"prime\", 1, 0.0, 3.9799, 10000, \"sin(2.0*pi*3333.0*x)\"")
CODE (U"stopwatch")
CODE (U"spectrum_dft = To Spectrum: \"no\"")
CODE (U"time_dft = stopwatch")
CODE (U"selectObject: sound")
CODE (U"spectrum_resampled = To Spectrum (resampled): 30")
CODE (U"time_resampled = stopwatch")
CODE (U"selectObject: sound")
CODE (U"spectrum_fft = To Spectrum: \"yes\"")
CODE (U"time_fft = stopwatch")
NORMAL (U"On my computer from 2019 the calculation of `spectrum_dft` happens to be very slow because of its naive %O(%N^2) algorithm. "
	"It takes 2.258 s while the resampled approximation only takes 0.031s and the approximation by adding zero values takes "
	"approximately 0.003 s. If the duration of the sound had been 10.0069 s, the number of samples again would be a prime number and "
	"the computing times are 14.127 s, 0.059 s and 0.005 s, respectively. This again shows that the naive implementation is "
	"very slow compared to the other two. It is also clear that resampling takes some extra time as compared to adding zero sample values. ")
NORMAL (U"The following picture shows the `spectrum_dft` in black colour, the `spectrum_fft` in silver/grey and the "
	"`spectrum_resampled` in red. "
	"From the two alternative approximations of the spectrum, the resampled one looks a better approximation to the DFT than the one with zeros added.")
PICTURE (5,3, drawSpectra)
NORMAL (U"This method was inspired by a script by Ton Wempe.")
MAN_END

MAN_BEGIN (U"Sound: Trim silences...", U"djmw", 20190914)
INTRO (U"A command that creates from the selected @Sound a new sound with silence durations not longer than a specified value.")
ENTRY (U"Settings")
TERM (U"##Trim duration (s)#")
DEFINITION (U"specifies the maximum allowed silence duration.")
TERM (U"##Pitch floor (Hz)#, and, ##Time step (s)#")
DEFINITION (U"determine how we measure the intensities on which the determination of silent intervals is based. See @@Sound: To Intensity...@ for more info.")
TERM (U"##Silence threshold (dB)#, ##Minimum silent interval duration (s)#, and ##Minimum sounding interval duration#")
DEFINITION (U"determine how the silent intervals will be determined. See @@Intensity: To TextGrid (silences)...@ for more info.")
TERM (U"##Save trimming info as TextGrid#")
DEFINITION (U"determines if a TextGrid with trimming information will also be created. The TextGrid will have one tier where intervals of the %%originating% sound that were trimmed have been labeled. ")
TERM (U"##Trim label#")
DEFINITION (U"determines the label that the trimmed intervals in the TextGrid will get.")
MAN_END

MAN_BEGIN (U"Sound & Pitch: To Spectrogram...", U"djmw", 20191008)
INTRO (U"A command that creates a @Spectrogram object from the selected "
	"@Sound and @Pitch objects by @@band filtering in the frequency domain@ with a "
	"bank of filters whose bandwidths depend on the local pitch.")
NORMAL (U"The filter functions used are:")
EQUATION (U"%#H(%f, %f__0_) = 1 / (((%f__%c_^^2^ - %f^2) /%f\\.c%B(%f__0_)))^2 + 1),")
NORMAL (U"where %f__%c_ is the central (resonance) frequency of the filter. "
	"%B(%f__0_) is the bandwidth in Hz and determined as")
EQUATION (U"%B(%f__0_) = %relativeBandwidth\\.c%f__0_, ")
NORMAL (U"where %f__0_ is the fundamental frequency as determined from the Pitch "
	"object. Whenever the value of %f__0_ is undefined, a value of 100 Hz is taken.")
MAN_END

MAN_BEGIN (U"Sound: To MelFilter...", U"djmw", 20141022)
INTRO (U"A deprecated command. Use @@Sound: To MelSpectrogram...@ instead.")
MAN_END

MAN_BEGIN (U"Sound: To MelSpectrogram...", U"djmw", 20141022)
INTRO (U"A command that creates a @MelSpectrogram object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"set of triangular filters.")
NORMAL (U"The filter functions used are all triangular in shape on a %mel "
	"frequency scale. Each filter function depends on three parameters, the "
	"lower frequency %f__%l_, the central frequency %f__%c_ and the higher "
	"frequency %f__%h_. "
	"On a %mel scale, the distances %f__%c_-%f__%l_ and %f__%h_-%f__%c_ "
	"are equal for each filter. The filter function is as follows:" )
EQUATION (U"%#H(%f) = 0 for %f \\<_ %f__%l_ and %f \\>_ %f__%h_")
EQUATION (U"%#H(%f) = (%f - %f__%l_) / (%f__%c_ - %f__%l_) for %f__%l_ \\<_ %f \\<_ %f__%c_")
EQUATION (U"%#H(%f) = (%f__%h_ - %f) / (%f__%h_ - %f__%c_) for %f__%c_ \\<_ %f \\<_ %f__%h_")
NORMAL (U"In general the number of filter values stored in each frame of the MelSpectrogram is an order of magnitude smaller than the number of sound samples in the corresponding analysis frame.")
MAN_END

MAN_BEGIN (U"Sound: To Pitch (shs)...", U"djmw", 19970402)   // 2023
INTRO (U"A command that creates a @Pitch object from every selected @Sound object.")
ENTRY (U"Purpose")
NORMAL (U"to perform a pitch analysis based on a spectral compression model. "
	"The concept of this model is that each spectral component not only activates "
	"those elements of the central pitch processor that are most sensitive to the "
	"component's frequency, but also elements that have a lower harmonic "
	"relation with this component. Therefore, when a specific element of the "
	"central pitch processor is most sensitive at a frequency %f__0_, it receives "
	"contributions from spectral components in the "
	"signal at integral multiples of %f__0_.")
ENTRY (U"Algorithm")
NORMAL (U"The spectral compression consists of the summation of a sequence of "
	"harmonically compressed spectra. "
	"The abscissa of these spectra is compressed by an integral factor, the rank "
	"of the compression. The maximum of the resulting sum spectrum is the "
	"estimate of the pitch. Details of the algorithm can be "
	"found in @@Hermes (1988)@")
ENTRY (U"Settings")
TERM (U"##Time step (s)# (standard value: 0.01 s)")
DEFINITION (U"the measurement interval (frame duration), in seconds.")
TERM (U"##Pitch floor (Hz)# (standard value: 50 Hz)")
DEFINITION (U"candidates below this frequency will not be recruited. This parameter "
	"determines the length of the analysis window.")
TERM (U"##Max. number of candidates# (standard value: 15)")
DEFINITION (U"The maximum number of candidates that will be recruited.")
TERM (U"##Maximum frequency (Hz)# (standard value: 1250 Hz)")
DEFINITION (U"higher frequencies will not be considered.")
TERM (U"##Max. number of subharmonics# (standard value: 15)")
DEFINITION (U"the maximum number of harmonics that add up to the pitch.")
TERM (U"##Compression factor# (standard value: 0.84)")
DEFINITION (U"the factor by which successive compressed spectra are multiplied before the summation.")
TERM (U"##Number of points per octave# (standard value: 48)")
DEFINITION (U"determines the sampling of the logarithmic frequency scale.")
TERM (U"##Pitch ceiling (Hz)# (standard value: 500 Hz)")
DEFINITION (U"candidates above this frequency will be ignored.")
MAN_END

MAN_BEGIN (U"Spectra: Multiply", U"djmw", 20100318)
INTRO (U"Returns a new Spectrum object that is the product of the two selected "
	"@Spectrum objects.")
MAN_END

MAN_BEGIN (U"Spectrum: Conjugate", U"djmw", 20031023)
INTRO (U"Reverses the sign of the complex part of the selected @Spectrum object(s).")
NORMAL (U"For real signals, conjugation in the spectral domain amounts to time-inversion in the time domain.")
MAN_END

MAN_BEGIN (U"Spectrum: Shift frequencies...", U"djmw", 20121028)
INTRO (U"Creates a new @Spectrum by shifting all frequencies of the selected Spectrum upwards or downwards.")
ENTRY (U"Settings")
TERM (U"##Shift by (Hz)")
DEFINITION (U"a positive value shifts the spectrum towards higher frequencies, a negative value shifts the spectrum "
	"towards lower frequencies.")
TERM (U"##New maximum frequency (Hz)")
DEFINITION (U"the maximum frequency in the new Spectrum.")
TERM (U"##Precision")
DEFINITION (U"the number of neighbouring frequency points that are used in the calculation of the new frequency points. "
	"The precision relates linearly to the amount of computing time needed to get the new shifted spectrum.")
MAN_END

MAN_BEGIN (U"SpeechSynthesizer", U"djmw", 20190811)
INTRO (U"The SpeechSynthesizer is one of the @@types of objects@ in Praat. It creates a speech sound from text. The actual text-to-speech synthesis is performed by the @@eSpeak|eSpeak NG@ speech synthesizer and therefore our SpeechSynthesizer is merely an interface to Espeak.")
ENTRY (U"Commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Create SpeechSynthesizer...@")
NORMAL (U"Playing:")
LIST_ITEM (U"\\bu @@SpeechSynthesizer: Play text...|Play text...@")
LIST_ITEM (U"\\bu @@SpeechSynthesizer: To Sound...|To Sound...@")
NORMAL (U"Modification:")
LIST_ITEM (U"\\bu @@SpeechSynthesizer: Set text input settings...|Set text input settings...@")
LIST_ITEM (U"\\bu @@SpeechSynthesizer: Speech output settings...|Speech output settings...@")
MAN_END

MAN_BEGIN (U"Create SpeechSynthesizer...", U"djmw", 20171101)
INTRO (U"Creates the @@eSpeak|eSpeak NG@ speech synthesizer.")
ENTRY (U"Settings")
TERM (U"##Language#")
DEFINITION (U"determines the language of the synthesizer.")
TERM (U"##Voice variant#")
DEFINITION (U"determines which voice type the synthesizer uses (male, female or whispered voices).")
MAN_END

MAN_BEGIN (U"SpeechSynthesizer: Play text...", U"djmw", 20171101)
INTRO (U"The selected @@SpeechSynthesizer@ plays a text.")
ENTRY (U"Settings")
TERM (U"##Text#")
DEFINITION (U"is the text to be played. Text within [[ ]] is treated as phonemes codes in @@Kirshenbaum phonetic encoding@. For example, besides a text like \"This is text\", you might also input \"This [[Iz]] text\".")
MAN_END

MAN_BEGIN (U"SpeechSynthesizer: To Sound...", U"djmw", 20171101)
INTRO (U"The selected @@SpeechSynthesizer@ converts a text to the corresponding speech sound.")
ENTRY (U"Settings")
TERM (U"##Text#")
DEFINITION (U"is the text to be played. Text within [[ ]] is treated as phonemes codes in @@Kirshenbaum phonetic encoding@. For example, besides a text like \"This is text\", you might also input \"This [[Iz]] text\".")
TERM (U"##Create TextGrid with annotations#")
DEFINITION (U"determines whether, besides the sound, a @@TextGrid@ with multiple-tier annotations will appear.")
MAN_END

MAN_BEGIN (U"SpeechSynthesizer: Set text input settings...", U"djmw", 20171101)
INTRO (U"A command available in the ##Modify# menu when you select a @@SpeechSynthesizer@.")
ENTRY (U"Settings")
TERM (U"##Input text format is#")
DEFINITION (U"determines how the input text will be synthesized.")
TERM (U"##Input phoneme codes are#")
DEFINITION (U"currently only @@Kirshenbaum phonetic encoding@ is available.")
MAN_END

MAN_BEGIN (U"SpeechSynthesizer: Speech output settings...", U"djmw", 20190811)
INTRO (U"A command available in the ##Modify# menu when you select a @@SpeechSynthesizer@.")
ENTRY (U"Settings")
TERM (U"##Sampling frequency#")
DEFINITION (U"determines how the sampling frequency of the sound.")
TERM (U"##Gap between words#")
DEFINITION (U"determines the amount of silence between words.")
TERM (U"##Pitch multiplier (0.5-2.0)#")
DEFINITION (U"determines how much the pitch will be changed. The extremes 0.5 and 2.0 represent, respectively, one octave "
	"below and one octave above the default pitch. ")
TERM (U"##Pitch range multiplier (0.0-2.0)#")
DEFINITION (U"determines how much the pitch range will be scaled. A value of 0.0 means monotonous pitch while a value of 2.0 means twice the default range.")
TERM (U"##Words per minute#")
DEFINITION (U"determines the speaking rate in words per minute.")
TERM (U"##Output phoneme codes are#")
MAN_END

MAN_BEGIN (U"SpeechSynthesizer: Set speech rate from speech...", U"djmw", 20171102)
INTRO (U"A command available in the ##Modify# menu when you select a @@SpeechSynthesizer@.")
ENTRY (U"Settings")
TERM (U"##Estimate speech rate from speech#")
DEFINITION (U"determines how speech rate is chosen. This is only used for the alignment of speech with text. If on, the speech rate is estimated from the part of speech that has to be aligned. ")
MAN_END

MAN_BEGIN (U"SSCP", U"djmw", 19981103)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of type SSCP represents the sums of squares and cross products of "
	"a multivariate data set.")
NORMAL (U"Besides the matrix part, an object of type SSCP also contains a "
	"vector with centroids.")
ENTRY (U"Inside a SSCP")
NORMAL (U"With @Inspect you will see that this type contains the same "
	"attributes as a @TableOfReal with the following extras:")
TERM (U"%numberOfObservations")
TERM (U"%centroid")
MAN_END

MAN_BEGIN (U"Sound: Extract Electroglottogram...", U"djmw", 20190929)
INTRO (U"Extract one of the channels of a @@Sound@ as an @@Electroglottogram@.")
ENTRY (U"Settings")
TERM (U"##Channel number#")
DEFINITION (U"defines the Electroglottogram channel in the sound.")
TERM (U"##Invert#")
DEFINITION (U"defines whether the wave in the Elecletroglottogram channel has to be inverted or not. "
	"The convention is that a positive direction in the Electroglottogram wave corresponds to "
	"an increase in contact area between the vocal folds which occurs if the vocal folds are closing. "
	"Since closing the vocal folds, in general, happens much faster than opening them, the steepest "
	"slope in each cycle of the wave should be the %%upward% slope. If this is not the case you "
	"should invert the wave.")
MAN_END

MAN_BEGIN (U"SSCP: Draw sigma ellipse...", U"djmw", 19990222)
INTRO (U"A command to draw for the selected @SSCP an ellipse that "
	"covers a part of the multivariate data.")
ENTRY (U"Setting")
TERM (U"##Number of sigmas")
DEFINITION (U"determines the @@concentration ellipse|data coverage@.")
MAN_END

MAN_BEGIN (U"SSCP: Get sigma ellipse area...", U"djmw", 20000525)
INTRO (U"A command to query the selected @SSCP object for the area of a "
	"sigma ellipse.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm proceeds as follows:")
LIST_ITEM (U"1. The four array elements in the SSCP-matrix that correspond to the chosen dimensions "
	"are copied into a two-dimensional matrix #%S (symmetric of course).")
LIST_ITEM (U"2. The eigenvalues of #%S are determined, call them %s__1_ and %s__2_.")
LIST_ITEM (U"3. The lengths %l__%i_  of the axes of the ellipse can be obtained as the  "
	"square root of the %s__i_ multiplied by a scale factor: %l__%i_ = %scaleFactor \\.c "
	"\\Vr (%s__%i_ ), "
	"where %scaleFactor = %numberOfSigmas / \\Vr(%numberOfObservations \\-- 1).")
LIST_ITEM (U"4. The area of the ellipse will be %\\pi\\.c%l__1_\\.c%l__2_.")
MAN_END

MAN_BEGIN (U"SSCP: Get confidence ellipse area...", U"djmw", 20000525)
INTRO (U"A command to query the selected @SSCP object for the area of a "
	"confidence ellipse.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm proceeds as follows:")
LIST_ITEM (U"1. The four array elements in the SSCP-matrix that correspond to the chosen dimensions "
	"are copied into a two-dimensional matrix #%S (symmetric of course).")
LIST_ITEM (U"2. The eigenvalues of #%S are determined, call them %s__1_ and %s__2_.")
LIST_ITEM (U"3. The lengths %l__1_ and %l__2_ of the two axes of the ellipse can be obtained as "
	"(see for example @@Johnson (1998)@, page 410: ")
EQUATION (U"    %l__%i_ = %scaleFactor \\.c \\Vr (%s__%i_ ),")
LIST_ITEM (U"    where")
EQUATION (U"%scaleFactor = \\Vr (%f \\.c %p \\.c (%n \\-- 1) / (%n \\.c (%n \\-- %p))),")
LIST_ITEM (U"    in which %f = @`invFisherQ` (1 \\-- %confidenceLevel, %p, %n \\-- %p), "
	"where %p is the numberOfRows from the SSCP object and %n the %numberOfObservations.")
LIST_ITEM (U"4. The area of the ellipse will be %\\pi\\.c%l__1_\\.c%l__2_.")
MAN_END

MAN_BEGIN (U"SSCP: Get diagonality (bartlett)...", U"djmw", 20011111)
INTRO (U"Tests the hypothesis that the selected @SSCP matrix object is "
	"diagonal.")
ENTRY (U"Setting")
TERM (U"##Number of constraints")
DEFINITION (U"modifies the number of independent observations. "
	"The default value is 1.")
ENTRY (U"Algorithm")
NORMAL (U"The test statistic is |#R|^^N/2^, the N/2-th power of the determinant"
	" of the correlation matrix. @@Bartlett (1954)@ developed the following "
	"approximation to the limiting distribution:")
EQUATION (U"\\ci^2 = -(%N - %numberOfConstraints - (2%p + 5) /6) ln |#R|")
NORMAL (U"In the formulas above, %p is the dimension of the correlation "
	"matrix, %N-%numberOfConstraints is the number of independent "
	"observations. Normally %numberOfConstraints would "
	"equal 1, however, if the matrix has been computed in some other way, "
	"e.g., from within-group sums of squares and cross-products of %k "
	"independent groups, %numberOfConstraints would equal %k.")
NORMAL (U"We return the probability %\\al as ")
EQUATION (U"%\\al = chiSquareQ (\\ci^2 , %p(%p-1)/2).")
NORMAL (U"A very low %\\al indicates that it is very improbable that the "
	"matrix is diagonal.")
MAN_END

MAN_BEGIN (U"SSCP: Get fraction variation...", U"djmw", 20040210)
INTRO (U"A command to ask the selected @SSCP object for the fraction "
	"of the total variation that is accounted for by the selected dimension(s).")
NORMAL (U"Further details can be found in @@Covariance: Get fraction variance...@.")
MAN_END

MAN_BEGIN (U"SSCP: To CCA...", U"djmw", 20031103)
INTRO (U"A command that creates a @@CCA|canonical correlation@ object from the "
	"selected @SSCP object.")
ENTRY (U"Setting")
TERM (U"##Dimension of dependent variate (ny)")
DEFINITION (U"defines a partition of the square %n x %n SSCP matrix S into the parts S__yy_ of "
	"dimension %ny x %ny, S__xx_ of dimension %nx x %nx, and the parts "
	"S__xy_ and S__yx_ of dimensions %nx x %ny and %ny x %nx, respectively.")
ENTRY (U"Algorithm")
NORMAL (U"The partition for the square SSCP-matrix is as follows:")
PICTURE (2.0, 2.0, drawPartionedMatrix)
NORMAL (U"The canonical correlation equations we have to solve are:")
EQUATION (U"(1)    (#S__%yx_ #S__%xx_^^-1^ #S__%yx_\\'p -\\la #S__%yy_)#y = #0")
EQUATION (U"(2)    (#S__%yx_\\'p #S__%yy_^^-1^ #S__%yx_ -\\la #S__%xx_)#x = #0")
NORMAL (U"where #S__%yy_ [%ny \\xx %ny] and #S__%xx_ [%nx \\xx %nx] are "
	"symmetric, positive definite matrices belonging to the dependent and the "
	"independent variables, respectively. ")
NORMAL (U"These two equations are not independent and we will show that both "
	"equations have the same eigenvalues and that the eigenvectors #x for "
	"equation (2) can be obtained from the eigenvectors #y of equation (1).")
NORMAL (U"We can solve equation (1) in several ways, however, the numerically "
	"stablest algorithm is probably by performing first a Cholesky decomposition "
	"of #S__xx_ and #S__yy_, followed by a @@generalized singular value "
	"decomposition@. The algorithm goes as follows:")
NORMAL (U"The Cholesky decompositions (\"square roots\") of #S__yy_ and #S__xx_ are:")
EQUATION (U"#S__yy_ = #U\\'p #U and #S__xx_ = #H\\'p #H,")
NORMAL (U"where #U and H are upper triangular matrices. From these decompositions, "
	"the inverse for #S__xx_^^-1^ is easily computed. Let #K be the inverse of #H, "
	"then we can write: ")
EQUATION (U"#S__xx_^^-1^ = #K #K\\'p.")
NORMAL (U"We next substitute in equation (1) and rewrite as:")
EQUATION (U"((#K\\'p#S__yx_\\'p)\\'p (#K\\'p#S__yx_\\'p) - \\la #U\\'p #U)#x = 0")
NORMAL (U"This equation can be solved for eigenvalues and eigenvectors by the "
	"generalized singular value decomposition because it is of the form "
	"#A\\'p#A -\\la #B\\'p#B.")
NORMAL (U"Now, given the solution for equation (1) we can find the solution "
	"for equation (2) by first multiplying (1) from the left with "
	"#S__yx_\\'p#S__yy_^^-1^, resulting in:")
EQUATION (U"(#S__yx_\\'p#S__yy_^^-1^#S__yx_#S__xx_^^-1^#S__yx_\\'p -\\la #S__yx_\\'p) #y = 0")
NORMAL (U"Now we split of the term #S__xx_^^-1^#S__yx_\\'p and obtain:")
EQUATION (U"(#S__yx_\\'p#S__yy_^^-1^#S__yx_ - \\la #S__xx_) #S__xx_^^-1^#S__yx_\\'p #y = 0")
NORMAL (U"This equation is like equation (2) and it has therefore the same eigenvalues "
	"and eigenvectors. (We also proved this fact in the algorithmic section of "
	"@@TableOfReal: To CCA...@.)")
NORMAL (U"The eigenvectors #x is now")
EQUATION (U"#x = #S__xx_^^-1^#S__yx_\\'p #y.")
MAN_END

MAN_BEGIN (U"SSCP: To Covariance...", U"djmw", 20090624)
INTRO (U"A command that creates a @Covariance object from each selected @SSCP object.")
ENTRY (U"Setting")
TERM (U"##Number of constraints")
DEFINITION (U"determines the factor by which each entry in the "
	"SSCP-matrix is scaled to obtain the Covariance matrix.")
ENTRY (U"Details")
NORMAL (U"The relation between the numbers %c__%ij_ in the covariance matrix and the numbers %s__%ij_ in "
	"the originating SSCP matrix is:")
EQUATION (U"%c__%ij_ = %s__%ij_ / (%numberOfObservations - %numberOfConstraints)")
NORMAL (U"Normally %numberOfConstraints will equal 1. However, when the SSCP was the "
	"result of summing %g SSCP objects, as is, for example, the case when you obtained the total "
	"within-groups SSCP from pooling the individual group SSCP's, %numberOfConstraints will equal the number of pooled SSCP's,  %g.")
MAN_END

MAN_BEGIN (U"SSCP & TableOfReal: Extract quantile range...", U"djmw", 20040225)
INTRO (U"Extract those rows from the selected @TableOfReal object whose @@Mahalanobis "
	"distance@, with respect to the selected @SSCP object, are within the "
	"quantile range.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
"Create Strings from tokens..."
© David Weenink 2017, Paul Boersma 2023

Create a new @Strings object as a list of tokens.

Settings
========
##Name
: the name of the new Strings object.

##Text
: the text to be tokenized.

##Separators
: determines the separator characters. If left empty, a space will be used as a separator.

Behaviour
=========
Multiple consecutive separators in the text will be treated as one.

Examples
========
Example 1
	\#{Create Strings from tokens:} "test", "a b c", " "
: will produce a Strings object with 3 strings in it: "a", "b" and "c".

Example 2
	\#{Create Strings from tokens:} "test", "a   b   c ", " "
: will also produce a Strings object with 3 strings in it: "a", "b" and "c".

Example 3
	\#{Create Strings from tokens:} "test", "a,b,c", ","
: will produce a Strings object with 3 strings in it: "a", "b" and "c".

Example 4
	\#{Create Strings from tokens:} "test", "a, b, c", ","
: will produce a Strings object with 3 strings in it: "a", " b" and " c" (note the spaces).

Example 5
	\#{Create Strings from tokens:} "test", "a, b, c", " ,"
: will produce a Strings object with 3 strings in it: "a", "b" and "c".

Example 6
	\#{Create Strings from tokens:} "test", "a,,b,c", " ,"
: will also produce a Strings object with 3 strings in it: "a", "b" and "c".

Example 7
	\#{Create Strings from tokens:} "test", "a, ,b,c", ","
: will produce a Strings with 4 strings in it: "a", " ", "b" and "c".

Example 8
	\#{Create Strings from tokens:} "test", "a,b,c,", ","
: will produce a Strings object with 3 strings in it: "a", "b" and "c".

Example 9
	\#{Create Strings from tokens:} "test", "a,b,c, ", ","
: will produce a Strings object with 4 strings in it: "a", "b", "c" and " ".

Example 10
	\#{Create Strings from tokens:} "test", "A string" + tab$ + "of ..tokens" + newline$ + "and some  more tokens", " .," + tab$ + newline$
: will produce a Strings object with 8 strings in it: "A", "string", "of", "tokens", "and", "some", "more" and "tokens".
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"T-test", U"djmw", 20020117)
INTRO (U"A test on the mean of a normal variate when the variance is unknown.")
NORMAL (U"In Praat, the %t-test is used to query a @Covariance object and:")
LIST_ITEM (U"1. get the significance of one mean. See @@Covariance: Get "
	"significance of one mean...@.")
LIST_ITEM (U"2. get the significance of the %difference between two means. "
	"See @@Covariance: Get significance of means difference...@.")
NORMAL (U"You should use a %t-test when you want to test a hypothesis about "
	"the mean of one column in a @TableOfReal object, or, if you want to test "
	"a hypothesis about the difference between the means of two columns in "
	"this object.")
NORMAL (U"You can perform these %t-tests in Praat by first transforming the "
	"TableOfReal object into a Covariance object (see @@TableOfReal: To "
	"Covariance@) and then choosing the appropriate query method on the "
	"latter object.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"BHEP multivariate normality test"
© David Weenink 2010-11-24

Given a sample of %n independent and identically distributed (i.i.d) observation vectors of dimension %d,
#%x_1 , #%x_2 , ..., #%x_%n, drawn from an unknown continuous distribution function %F(#%x),
the Baringhaus–Henze–Epps–Pulley multivariate normality test tests 
:   H_0: %F(#%x) = %F_0 (#%x) against H_1 : %F(#%x) ≠ %F_0(#%x), where 

%F_0(#%x) is the multivariate normal distribution function %N_%d(#%μ, #%Σ) with mean #%μ and covariance matrix #%Σ
whose derivative with respect to #%x is the multivariate probability density function
%f_0(#%x)= (2π)^^-%d/2^ |#%Σ|^½ exp{-½(#%x-#%μ)^T #%Σ^^-1^ (#%x-#%μ)}.
 
According to @@Henze & Wagner (1997)@ the test statistic in BHEP has a number of favourable characteristics:
. it is affine invariant
. it is consistent

Settings
========

The test depends on a smoothing parameter %h that can be chosen in various ways:
@@Henze & Wagner (1997)@ recommend %h = 1.41, while
@@Tenreiro (2009)@ recommends  %h__%s _= 0.448 + 0.026\\.c%d for short tailed alternatives and "
" %h__%l_ = 0.928 + 0.049\\.c%d for long tailed alternatives.
	
################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Table: Normal probability plot...", U"djmw", 20130619)
NORMAL (U"In a normal probability plot, the data in the selected column of the @Table are plotted "
	"against a normal distribution in such a way that the points should form approximately a straight line. "
	"Departures from a straight line indicate departures from normality.")
ENTRY (U"Settings")
TERM (U"##Number of quantiles#")
DEFINITION (U"the number of quantile points, %n, in the plot. From this number %n, the quantile points are "
	"determined as follows: the last quantile point is %q__%n_ = 0.5^^1/%n^ and the first quantile point is "
	"%q__1_=1\\--%q__%n_. The intermediate quantile points %q__%i_ are determined according to "
	"%q__%i_=(%i \\-- 0.3175)/(%n + 0.365), where %i runs from 2 to %n\\--1.")
TERM (U"##Number of sigmas#")
DEFINITION (U"determines the horizontal and vertical drawing ranges in units of standard deviations. ")
MAN_END

MAN_BEGIN (U"Table: Quantile-quantile plot...", U"djmw", 20130619)
NORMAL (U"In a quantile-quantile plot the quantiles of the data in the first selected column of the @Table are plotted against "
	"the quantiles of the data in the second selected column.  If the two sets come from a population with the "
	"same distribution, the points should fall approximately along the reference line.")
MAN_END

MAN_BEGIN (U"Table: Bar plot...", U"djmw", 20230901)  // ppgb 2023
INTRO (U"Draws a bar plot from data in one or more columns of the selected @Table. In a bar plot the horizontal axis has nominal values (labels). ")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (10.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Table: Bar plot", 10.6)   // 0.6 extra for the text
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Vertical column(s)", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= auto)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Column with labels", "")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Distance of first bar from border", "1.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Distance between bar groups", "1.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Distance between bars within group", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colours (0-1, name, {r,g,b})", "Grey")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Label text angle (degrees)", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 1)
)
TERM (U"##Vertical column(s)")
DEFINITION (U"you list the table columns that you want to represent in the bar plot. The number of selected columns is the group size.")
TERM (U"##Vertical range")
DEFINITION (U"determine the lower and upper limit of the display.")
TERM (U"##Column with labels")
DEFINITION (U"determines the column whose labels will be put at the bottom of the plot.")
TERM (U"##Distance of first bar from border")
DEFINITION (U"determines how far the first (and last) bar wil be positioned from the borders (in units of the width of one bar).")
TERM (U"##Distance between bar groups")
DEFINITION (U"determines how far groups of bars are from each other. ")
TERM (U"##Distance between bars within group")
DEFINITION (U"determines the distance between the bars within each group.")
TERM (U"##Colours")
DEFINITION (U"determines the colours of the bars in a group.")
TERM (U"##Label text angle (degrees)")
DEFINITION (U"determines the angle of the labels written below the plot. If you have very long label texts you can prevent the label texts from overlapping.")
ENTRY (U"Examples")
NORMAL (U"@@Keating & Esposito (2006)@ present a bar plot in their fig. 3 from which we estimate the following data table")
CODE (U"Language        Modal  Breathy")
CODE (U"Chong            -1.5    5")
CODE (U"Fuzhou            2     10")
CODE (U"Green Hmong       3     12")
CODE (U"White Hmong       2     11")
CODE (U"Mon              -1.5    0")
CODE (U"SADV Zapotec     -6     -4")
CODE (U"SLQ Zapotec       3.5   14")
CODE (U"Tlacolula Zapotec 3     13")
CODE (U"Tamang            1      1")
CODE (U"!Xoo              1     14")
NORMAL (U"Given that we have these data in a Table with the three columns labeled \"Language\", \"Modal\" and \"Breathy\", "
	"respectively, we can first try to reproduce their figure 3 (a bar plot with both Modal and Breathy columns displayed) ")
NORMAL (U"As you can see the labels in the first column are very long texts and they will surely overlap if "
	"plotted at the bottom of a plot. We therefore use a value of 15 degrees for the \"Label text angle\" " "parameter. This "
	"will make the label texts nonoverlapping. We cannot make this angle much larger, because then the label texts would run out of "
	"the viewport. ")
NORMAL (U"The following script line will produce the picture below.")
CODE (U"Bar plot: \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"0.9 0.5\", 15.0, \"yes\"")
SCRIPT (5, 3,  U"h1h2 = Create H1H2 table (Keating & Esposito 2006)\n"
	"Font size: 10\n"
	"Bar plot: \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"0.9 0.5\", 15.0, \"yes\"\n"
	"removeObject: h1h2\n")
NORMAL (U"The essentials of the bart plot in their paper are perfectly reproduced in the figure above. "
	"If you want the bars within a group to be placed somewhat more apart, say 0.2 times the bar width, "
	"you can set the \"Distance between bars in a group\" to a value of 0.2:")
CODE (U"Bar plot: \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.2, \"0.9 0.5\", 15.0, \"yes\"")
SCRIPT (5, 3,  U"h1h2 = Create H1H2 table (Keating & Esposito 2006)\n"
	"Font size: 10\n"
	"Bar plot: \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.2, \"0.9 0.5\", 15.0, \"yes\"\n"
	"removeObject: h1h2\n")
NORMAL (U"Of course we can also work with colours and we can add vertical marks as the following scriptlet shows")
CODE (U"Bar plot: \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"Green Red\", 15.0, \"yes\"")
CODE (U"Marks left every: 1, 5, 1, 1, 1")
CODE (U"Text left: 1, \"H__1_-H__2_ (dB)\"")
SCRIPT (5, 3,  U"h1h2 = Create H1H2 table (Keating & Esposito 2006)\n"
	"Font size: 10\n"
	"Bar plot: \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"Green Red\", 15.0, \"yes\"\n"
	"Marks left every: 1, 5, 1, 1, 1\n"
	"Text left: 1, \"H__1_-H__2_ (dB)\"\n"
	"removeObject: h1h2\n")
MAN_END

MAN_BEGIN (U"Table: Box plots...", U"djmw", 20230901)
INTRO (U"A command to draw a @@box plot@s for each factor from the data in other column(s) of the selected @Table object, "
	"where the factors will be sorted in @@natural sort order@.")
ENTRY (U"Example")
NORMAL (U"To draw separate box plots for the male, female and children F0 for the @@Peterson & Barney (1952)@ data: ")
CODE (U"Create formant table (Peterson & Barney 1952)")
CODE (U"Box plots: \"F0\", \"Type\", 70, 400")
CODE (U"Text left: \"yes\", \"F0 (Hz)\"")
SCRIPT (5,3, U"pb = Create formant table (Peterson & Barney 1952)\n"
	"Box plots: \"F0\", \"Type\", 70, 400, \"yes\"\n"
	"Text left: \"yes\", \"F0 (Hz)\"\n"
	"removeObject: pb\n"
)
MAN_END

MAN_BEGIN (U"Table: Line graph where...", U"djmw", 20230901)
INTRO (U"Draws a line graph from the data in a column of the selected @Table."
	"More info @@Table: Line graph...@.")
MAN_END

MAN_BEGIN (U"Table: Line graph...", U"djmw", 20231207)
INTRO (U"Draws a line graph from the data in a column of the selected @Table. "
	"In a line plot, the horizontal axis can have a nominal scale or a numeric scale. "
	"The data points are connected by line segments.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (8.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Table: Line graph", 8.6)   // 0.6 extra for the text
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Vertical column", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= auto)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Horizontal column", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Horizontal range", "0.0", "0.0 (= auto)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Text", "+")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Text font size", "12")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Label text angle (degrees)", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
)
TERM (U"##Vertical column")
DEFINITION (U"The column whose data points you want to plot.")
TERM (U"##Vertical range")
DEFINITION (U"determine the lower and upper limits of the plot.")
TERM (U"##Horizontal column")
DEFINITION (U"determines the horizontal scale. If you leave it empty, or if the (selected part of the) selected column "
	"contains nominal values, i.e. the values are not numeric but text, the horizontal "
	"distance between the data points will be constant (i.e. 1) and the nominal values (texts) will be put as labels at "
	"the bottom of the horizontal axis. On the other hand, if this column contains only numerical values, the horizontal "
	"position of the data points will be determined by the values in this column.")
TERM (U"##Horizontal range")
DEFINITION (U"determines the left and right limit of the plot.")
TERM (U"##Text")
DEFINITION (U"The text to put at the position of the data point in the plot.")
TERM (U"##Text font size")
DEFINITION (U"defines the size of the text.")
TERM (U"##Label text angle (degrees)")
DEFINITION (U"determines the angle of the labels written %%below% the plot. If you have very long label texts in the "
	"\"Horizontal column\", you can prevent the label texts from overlapping. This only has effect for a horizontal "
	"column with nominal values.")
ENTRY (U"Examples")
NORMAL (U"The following table was estimated from fig. 3 in @@Ganong (1980)@ and represents the fraction /d/ responses as a function of a "
	"voice onset time (VOT) continuum. The second column shows the responses in a word - nonword continuum, while the third column shows "
	"the responses to a nonword - word continuum.")
CODE (U"VOT dash-tash dask-task")
CODE (U"-17.5   0.98      0.92")
CODE (U" -7.5   0.95      0.83")
CODE (U" -2.5   0.71      0.33")
CODE (U"  2.5   0.29      0.10")
CODE (U"  7.5   0.12      0.02")
CODE (U" 17.5   0.10      0.02")
NORMAL (U"We can reproduce fig. 3 from Ganong (1980) with the following script, where we labeled the word - nonword curve with \"wn\" and the nonword - word curve with \"nw\". We deselect \"Garnish\" because we want to put special marks at the bottom.")
CODE (U"Font size: 10\n")
CODE (U"Dotted line\n")
CODE (U"Line graph: \"dash-tash\", 0, 1, \"VOT\", -20, 20, \"wn\", 12, 0, \"no\"")
CODE (U"Dashed line\n")
CODE (U"Line graph: \"dask-task\", 0, 1, \"VOT\", -20, 20, \"nw\", 12, 0, \"no\"")
CODE (U"Draw inner box")
CODE (U"One mark bottom: 2.5, \"no\", \"yes\", \"no\", \"+2.5\"")
CODE (U"One mark bottom: -2.5, \"yes\", \"yes\", \"no\", \"\"")
CODE (U"One mark bottom: -7.5, \"yes\", \"yes\", \"no\", \"\"")
CODE (U"One mark bottom: 7.5, \"no\", \"yes\", \"no\", \"+7.5\"")
CODE (U"One mark bottom: 2.5, \"no\", \"no\", \"no\", \"+2.5\"")
CODE (U"One mark bottom: -20, \"no\", \"no\", \"no\", \"Short VOT\"")
CODE (U"One mark bottom: 20, \"no\", \"no\", \"no\", \"Long VOT\"")
CODE (U"Text bottom: 1, \"VOT (ms)\"")
CODE (U"Marks left every: 1, 0.2, \"yes\", \"yes\", \"no\"")
CODE (U"Text left: 1, \"Prop. of voiced responses\"")

SCRIPT (5, 3, U"ganong = Create Table (Ganong 1980)\n"
	"Dotted line\n"
	"Font size: 10\n"
	"Line graph: \"dash-tash\", 0, 1, \"VOT\", -20, 20, \"wn\", 12, 0, \"no\"\n"
	"Dashed line\n"
	"Line graph: \"dask-task\", 0, 1, \"VOT\", -20, 20, \"nw\", 12, 0, \"no\"\n"
	"Draw inner box\n"
	"One mark bottom: 2.5, 0, \"yes\", \"no\", \"+2.5\"\n"
	"One mark bottom: -2.5, \"yes\", \"yes\", \"no\", \"\"\n"
	"One mark bottom: -7.5, \"yes\", \"yes\", \"no\", \"\"\n"
	"One mark bottom: 7.5, \"no\", \"yes\", \"no\", \"+7.5\"\n"
	"One mark bottom: 2.5, \"no\", \"no\", \"no\", \"+2.5\"\n"
	"One mark bottom: -20, \"no\", \"no\", \"no\", \"Short VOT\"\n"
	"One mark bottom: 20, \"no\", \"no\", \"no\", \"Long VOT\"\n"
	"Text bottom: 1, \"VOT (ms)\"\n"
	"Marks left every: 1, 0.2, \"yes\", \"yes\", \"no\"\n"
	"Text left: 1, \"Prop. of voiced responses\"\n"
	"removeObject: ganong\n"
)
NORMAL (U"As an example of what happens if you don't supply an argument for the \"Horizontal column\" we will use "
	"the same table as for the previous plot. However the resulting plot may not be as meaningful (note that the "
	"horizontal nominal scale makes all points equidistant in the horizontal direction.)")
CODE (U"Dotted line\n")
CODE (U"Line graph: \"dash-tash\", 0, 1, \"\", 0, 0, \"wn\", 12,  0, \"yes\"")
CODE (U"One mark bottom: 1, \"no\", \"yes\", \"no\", \"Short VOT\"")
SCRIPT (5, 3, U"ganong = Create Table (Ganong 1980)\n"
	"Dotted line\n"
	"Font size: 10\n"
	"Line graph: \"dash-tash\", 0, 1, \"\", 0, 0, \"wn\", 12, 0, \"yes\"\n"
	"One mark bottom: 1, \"no\", \"yes\", \"no\", \"Short VOT\"\n"
	"removeObject: ganong\n"
)
MAN_END

MAN_BEGIN (U"Table: Horizontal error bars plot...", U"djmw", 20230901)
INTRO (U"Draws horizontal lines that represent the error intervals of a data column from the selected @@Table@.")
NORMAL (U"This command behaves analogous to @@Table: Vertical error bars plot...@.")
MAN_END

MAN_BEGIN (U"Table: Vertical error bars plot...", U"djmw", 20230901)
INTRO (U"Draws vertical lines that represent the error intervals of a data column from the selected @@Table@.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (9.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Table: Vertical error bars plot", 9.6)   // 0.6 extra for the text
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Horizontal column", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Horizontal range", "0.0", "0.0 (= auto)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Vertical column", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= auto)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Lower error value column", "")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Upper error value column", "")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Bar size (mm)", "1.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
)
TERM (U"##Horizontal column#")
DEFINITION (U"determines the data along the horizontal axis.")
TERM (U"##Horizontal range#")
DEFINITION (U"determines the lower and upper limits of the plot.")
TERM (U"##Vertical column#")
DEFINITION (U"determines the data along the horizontal axis.")
TERM (U"##Vertical range#")
DEFINITION (U"determines the lower and upper limits of the plot.")
TERM (U"##Lower error value column#, ##Upper error value column#")
DEFINITION (U"determine the size of the vertical lines that will be drawn. These lines are drawn between the points "
	"(%x,%y-%low) and (%x, %y+%up), where %x and %y are the values from the %%horizontal column% and the %%vertical "
	"column%, respectively, and, %low and %up are the corresponding values "
	"in the %%lower error value column% and the %%upper error value column%, respectively. If either of these column "
	"names is not given the corresponding values (%low and/or %up) will taken as zero. This makes it possible to draw "
	"one-sided and two-sided error bars. If your errors are symmetric around the y-position, your table only needs "
	"one column and you can supply the name of this column in both fields.")
TERM (U"##Bar size (mm)#")
DEFINITION (U"determines the width of the horizontal bars or whishers at the lower end position of the drawn line. ")
TERM (U"##Garnish#")
DEFINITION (U"determines whether or not some decoration is drawn.")
MAN_END

MAN_BEGIN (U"Table: Get median absolute deviation...", U"djmw", 20120405)
INTRO (U"Get the median absolute deviation (MAD) of the column in the selected @@Table@ (adjusted by a scale factor).")
ENTRY (U"Algorithm")
NORMAL (U"From the %n numbers %x__1_, %x__2_, ..., %x__%n_ in the selected column we first calculate the @@quantile algorithm|median@ "
	"value %x__median_. Next we calculate the %n absolute deviations from this median: %d__1_, %d__2_, ..., %d__%n_, "
	"where %d__%j_=|%x__%j_ - %x__median_|. "
	"Then we calculate the MAD value, which is the median of the %n values %d__1_, %d__2_, ..., %d__%n_. Finally we multiply the MAD "
	"value by the scale factor 1.4826. This last multiplication makes the result comparable with the value of the standard deviation if "
	"the %x values are normally distributed.")
MAN_END

MAN_BEGIN (U"Table: Report one-way anova...", U"djmw", 20120617)
INTRO (U"Performs a one-way analysis of variance on the data in one column of a selected @@Table@ and reports the fixed-effects anova table results in the info window.")
ENTRY (U"Settings")
TERM (U"##Column with data#")
DEFINITION (U"the label of the column who's data will be analyzed.")
TERM (U"##Factor#")
DEFINITION (U"the label of the column with the names of the levels.")
TERM (U"##Table with means#")
DEFINITION (U"if checked, a Table with the mean values of the levels will be created.")
TERM (U"##Table with differences between means#")
DEFINITION (U"if checked, a Table with the differences between the mean values of the levels will be created.")
TERM (U"##Table with Tukey's post-hoc test#")
DEFINITION (U"if checked, a Table with Tukey's HSD tests will be created. Each value in this Table measures "
	"the probability that the corresponding difference between the level means happened by chance. The test "
	"compares all possible level means and is based on the studentized range distribution.")
MAN_END

MAN_BEGIN (U"Table: Report two-way anova...", U"djmw", 20140117)
INTRO (U"Performs a two-way analysis of variance on the data in one column of a selected %%fully factorial% "
	"@@Table@ and reports the fixed-effects anova table in the info window. ")
ENTRY (U"Settings")
TERM (U"##Column with data#")
DEFINITION (U"the label of the column who's data will be analyzed.")
TERM (U"##First factor")
DEFINITION (U"the label of the column with the names of the levels for the first factor.")
TERM (U"##Second factor")
DEFINITION (U"the label of the column with the names of the levels for the second factor.")
TERM (U"##Table with means")
DEFINITION (U"if checked, a Table with the mean values of all the levels will be created.")
ENTRY (U"Example")
NORMAL (U"Suppose you want to check if fundamental frequency depends on the type of vowel and speaker type. We will use the "
	"@@Create formant table (Peterson & Barney 1952)|Peterson & Barney@ vowel data set to illustrate this. "
	"The following script will first create the data set and then produce the two-way anova report." )
CODE (U"Create formant table (Peterson & Barney 1952)")
CODE (U"Report two-way anova: \"F0\", \"Vowel\", \"Type\"")
NORMAL (U"This will produce the following anova table in the info window:")
CODE (U"Two-way analysis of \"F0\" by \"Vowel\" and \"Type\".")
CODE (U"")
CODE (U"      Source             SS        Df             MS         F         P")
CODE (U"       Vowel        73719.4         9        8191.05    7.62537    5.25258e-11")
CODE (U"        Type    4.18943e+06         2    2.09471e+06    1950.05              0")
CODE (U"Vowel x Type        6714.34        18        373.019   0.347258       0.994969")
CODE (U"       Error    1.60053e+06      1490        1074.18")
CODE (U"       Total    5.87039e+06      1519")
NORMAL (U"The analysis shows that F0 strongly depends on the vowel and also on the speaker type and, luckily, we do not have any "
	"interaction between the vowel and the speaker type. Besides the anova table there is also shown a table with the mean F0 "
	"values for each Vowel-Type combination which looks like:")
CODE (U"                   c         m         w      Mean")
CODE (U"        aa       258       124       212       198")
CODE (U"        ae       248       125       208       194")
CODE (U"        ah       263       129       223       205")
CODE (U"        ao       259       127       217       201")
CODE (U"        eh       259       128       220       202")
CODE (U"        er       264       133       219       205")
CODE (U"        ih       270       136       232       213")
CODE (U"        iy       270       136       231       212")
CODE (U"        uh       273       136       234       214")
CODE (U"        uw       278       139       235       218")
CODE (U"      Mean       264       131       223       206")
NORMAL (U"The first column of this table shows the vowel codes while the first row shows the speaker types (child, man, woman). "
	"The last row and the last column of the table shows the averages for the factors Type and Vowel, respectively. The actual "
	"data are unbalanced because we have 300, 660 and 560 replications per column respectively (for each speaker we have two replcations of the data).")
ENTRY (U"Algorithm")
NORMAL (U"The formulas to handle unbalanced designs come from @@Khuri (1998)@.")
MAN_END

MAN_BEGIN (U"Table: Report one-way Kruskal-Wallis...", U"djmw", 20120617)
INTRO (U"Performs a one-way Kruskal-Wallis analysis on the data in one column of a selected @@Table@ and reports the results in the info window. This test is sometimes refered to as a one-way analysis of variance for %%non-normally distributed% data.")
ENTRY (U"Settings")
TERM (U"##Column with data#")
DEFINITION (U"the label of the column whose data will be analyzed.")
TERM (U"##Factor#")
DEFINITION (U"the label of the column with the names of the levels.")
ENTRY (U"Algorithm")
NORMAL (U"The analysis is done on the ranked data and consists of the following steps:")
LIST_ITEM (U"1. Rank all the %N data points together, i.e. rank the data from 1 to %N.")
LIST_ITEM (U"2. The test statistic is:")
EQUATION (U"%K = (12 / (%N(%N+1)) \\Si__%i=1_^^%g^ %n__%i_ (meanRank__%i_)^^2^ - 3(%N+1),")
DEFINITION (U"where %g is the number of levels, %n__%i_ the number of data in %i-th level and meanRank__%i_ "
	"the average rank of the %i-th level.")
LIST_ITEM (U"3. The %p value is %%approximated by the \\ci^^2^ (%K, %g - 1) distribution.")
MAN_END

MAN_BEGIN (U"TableOfReal: Report multivariate normality (BHEP)...", U"djmw", 20090701)
INTRO (U"Report about multivariate normality according to the @@BHEP multivariate normality test@.")
ENTRY (U"Settings")
TERM (U"##Smoothing parameter#")
DEFINITION (U"determines the smoothing parameter %h.")
MAN_END

MAN_BEGIN (U"TableOfReal: Change row labels...", U"djmw", 20010822)
INTRO (U"Changes the row labels of the selected @TableOfReal object according "
	"to the specification in the search and replace fields.")
NORMAL (U"Both search and replace fields may contain @@regular expressions|"
	"Regular expressions@. The ##Replace limit# parameter limits the number of "
	"replaces that may occur within each label.")
MAN_END

MAN_BEGIN (U"TableOfReal: Change column labels...", U"djmw", 20010822)
INTRO (U"Changes the column labels of the selected @TableOfReal object according "
	"to the specification in the search and replace fields.")
NORMAL (U"Both search and replace fields may contain @@regular expressions|"
	"Regular expressions@. The %%Replace limit% parameter limits the number of "
	"replaces that may occur within each label.")
MAN_END

MAN_BEGIN (U"TableOfReal: Draw biplot...", U"djmw", 20020603)
INTRO (U"A command to draw a biplot for each column in the selected "
	"@TableOfReal object.")
ENTRY (U"Settings")
TERM (U"##Xmin#, ##Xmax#, ##Ymin#, ##Ymax#")
DEFINITION (U"determine the drawing boundaries.")
TERM (U"##Split factor#")
DEFINITION (U"determines the weighing of the row and column structure "
	"(see below).")
ENTRY (U"Behaviour")
LIST_ITEM (U"1. Get the @@singular value decomposition@ #U #D #V\\'p of the "
	"table.")
LIST_ITEM (U"2. Calculate weighing factors %\\la for row and columns")
EQUATION (U"%\\la__r,1_ = %\\si__1_^^%splitFactor^")
EQUATION (U"%\\la__c,1_ = %\\si__1_^^1-%splitFactor^")
EQUATION (U"%\\la__r,2_ = %\\si__2_^^%splitFactor^")
EQUATION (U"%\\la__c,2_ = %\\si__2_^^1-%splitFactor^")
DEFINITION (U"where %\\si__1_ and %\\si__2_ are the first and the second singular values")
LIST_ITEM (U"3. For the rows (%i from 1..%numberOfRows) form:")
EQUATION (U"%xr__%i_ = %U__%i1_  %\\la__%r,1_")
EQUATION (U"%yr__%i_ = %U__%i2_  %\\la__%r,2_")
LIST_ITEM (U"4. For the columns (%i from 1..%numberOfColumns) form:")
EQUATION (U"%xc__%i_ = %V__%i1_  %\\la__%c,1_")
EQUATION (U"%yc__%i_ = %V__%i2_  %\\la__%c,2_")
LIST_ITEM (U"5. Plot the points (%xr__%i_, yr__%i_) and (%xc__%i_, yc__%i_) in the "
	"same figure with the corresponding row and column labels.")
MAN_END

MAN_BEGIN (U"TableOfReal: Draw box plots...", U"djmw", 20000523)
INTRO (U"A command to draw a @@box plot@ for each column in the selected "
	"@TableOfReal object.")
ENTRY (U"Settings")
TERM (U"##From row#, ##To row#, ##From column#, ##To column#")
DEFINITION (U"determine the part of the table that you want to analyse.")
TERM (U"%Ymin and %Ymax")
DEFINITION (U"determine the drawing boundaries.")
MAN_END

MAN_BEGIN (U"TableOfReal: Draw as scalable squares...", U"djmw", 20180304)
INTRO (U"A command to draw the cells of the table as squares whose areas conform to the cell's value. ")
ENTRY (U"Settings")
TERM (U"##From row#, ##To row#")
DEFINITION (U"determine the rows to be drawn.")
TERM (U"##From column#, ##To column#")
DEFINITION (U"determine the columns to be drawn.")
TERM (U"##Origin#")
DEFINITION (U"determines the drawing orientation. For a table with %%nrow% rows and %%ncol% columns:")
TERM1 (U"%%top-left%: cel [1] [1] will be at the top left position in the drawing, cell [%%nrow%] [%%ncol%] will be at bottom right position.")
TERM1 (U"%%top-right%: cel [1] [1] will be at the top right position in the drawing, cell [%%nrow%] [%%ncol%] will be at bottom left position.")
TERM1 (U"%%bottom-left%: cel [1] [1] will be at the bottom left position in the drawing, cell [%%nrow%] [%%ncol%] will be at top right position.")
TERM1 (U"%%bottom-right%: cel [1] [1] will be at the bottom right position in the drawing, cell [%%nrow%] [%%ncol%] will be at top left position.")
TERM (U"##Cell area scale factor#")
DEFINITION (U"multiplies the area of each cell's square. If this factor is larger than 1.0 some of the squares might overlap.")
TERM (U"##Filling order#")
DEFINITION (U"determines in what order the squares will be drawn. The order is only important if some of the squares overlap, "
	"i.e. if the cell area scale factor is larger than 1.0.")
TERM1 (U"%%rows%: start with the first row, cell [1] [1] to cell [1] [%%ncol%], next the second row, etc...")
TERM1 (U"%%columns% start with column 1, cell [1] [1] to cell [%%nrow%] [1], next column 2 etc...")
TERM1 (U"%%increasing values%: first sort the cell values in increasing order and then start drawing them, the cell with the smallest value first. ")
TERM1 (U"%%decreasing values%: first sort the cell values in decreasing order and then start drawing them, the cell with the largest value first.")
TERM1 (U"%%random%: draw cells in random order. If the cell area scale factor is larger than 1.0 this may result in a different graph of the same table for each successive call.")
MAN_END

MAN_BEGIN (U"TableOfReal: Draw rows as histogram...", U"djmw", 20030619)
INTRO (U"A command to draw a histogram from the rows in the selected "
	"@TableOfReal object.")
NORMAL (U"The histogram will consist of %groups of bars. The number of groups will "
	"be determined by the number of selected columns from the table, while the "
	"number of bars within each group will be determined from the number of "
	"selected rows.")
ENTRY (U"Settings")
TERM (U"##Row numbers# and ##Column range#")
DEFINITION (U"determine the part of the table that you want to draw. "
	"The column range determines the number of bars that you want to draw for "
	"each row selected by the %%Row numbers% argument.")
TERM (U"##Ymin# and ##Ymax#")
DEFINITION (U"the drawing boundaries.")
NORMAL (U"The following arguments are all relative to the width of a bar "
	"in the histogram. ")
TERM (U"##Horizontal offset")
DEFINITION (U"the offset from the left and right margin.")
TERM (U"##Distance between bar groups")
DEFINITION (U"the distance between each group, i.e., the distance "
	"between the right side of the last bar in a group to the left side of "
	"the first bar in the next group.")
TERM (U"##Distance between bars")
DEFINITION (U"the distance between the bars in a group.")
TERM (U"##Grey values")
DEFINITION (U"the grey values of the bars in a group.")
ENTRY (U"Bar positioning")
NORMAL (U"If you want to put the labels yourself you will need the following information.")
NORMAL (U"The width of a bar is determined as follows:")
EQUATION (U"%width = 1 / (%nc \\.c %nr + 2 \\.c %hoffset + (%nc - 1)\\.c %intergroup +"
		"%nc\\.c(%nr -1)\\.c %interbar),")
NORMAL (U"where %nc is the number of columns (groups) to draw, %nr is the number of "
	"rows to draw (the number of bars within a group), %hoffset is the horizontal "
	"offset, %intergroup the distance between each group and %interbar "
	"the distance between the bars within a group.")
NORMAL (U"The spacing between the bars drawn from a row:")
EQUATION (U"%dx = (%intergroup + %nr + (%nr -1) \\.c %interbar) *% width")
NORMAL (U"The first bar for the %k-th row starts at:")
EQUATION (U"%x1 = %hoffset \\.c %width + (%k - 1) \\.c (1 + %interbar) \\.c %width")
MAN_END

MAN_BEGIN (U"TableOfReal: Select columns where row...", U"djmw", 20140117)
INTRO (U"Copy columns from the selected @TableOfReal object to a new "
	"TableOfReal object.")
ENTRY (U"Settings")
TERM (U"##Columns#")
DEFINITION (U"defines the indices of the columns to be selected. Ranges can be "
	"defined with a colon \":\". Columns will be selected in the specified "
	"order.")
TERM (U"##Row condition#")
DEFINITION (U"specifies a condition for the selection of rows. If the "
	"condition evaluates as %true for a particular row, the selected elements "
	"in this row will be copied. See @@Matrix: Formula...@ for the kind of "
	"expressions that can be used here.")
ENTRY (U"Examples")
CODE (U"Select columns where row: \"1 2 3\", \"1\"")
CODE (U"Select columns where row: \"1 : 3\", \"1\"")
NORMAL (U"Two alternative expressions to copy the first three columns to a new table "
	"with the same number of rows.")
CODE (U"Select columns where row: \"3 : 1\", \"1\"")
NORMAL (U"Copy the first three columns to a new table with the same number of "
	"rows. The new table will have the 3 columns reversed.")
CODE (U"Select columns where row: \"1:6 9:11\", \"self[row,8]>0\"")
NORMAL (U"Copy the first six columns and columns 9, 10, and 11 to a new table. "
	"Copy only elements from rows where the element in column 8 is greater "
	"than zero.")
MAN_END

MAN_BEGIN (U"TableOfReal: Standardize columns", U"djmw", 19990428)
INTRO (U"Standardizes each column of the selected @TableOfReal.")
NORMAL (U"The entries %x__%ij_ in the TableOfReal will change to:")
EQUATION (U"(%x__%ij_ \\-- %\\mu__%j_) / %\\si__%j_, ")
NORMAL (U"where %\\mu__%j_ and %\\si__%j_ are the mean and the standard deviation as calculated "
	"from the %j^^th^ column, respectively. After standardization all column means will equal zero "
	"and all column standard deviations will equal one.")
ENTRY (U"Algorithm")
NORMAL (U"Standard deviations are calculated with the corrected two-pass algorithm as described in @@Chan, Golub & LeVeque (1983)@.")
MAN_END

MAN_BEGIN (U"TableOfReal: To Configuration (lda)...", U"djmw", 19981103)
INTRO (U"Calculates a @Configuration based on the @Discriminant scores obtained "
	"from the selected @TableOfReal. Row labels in the table indicate group membership.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"determines the number of dimensions of the resulting Configuration.")
ENTRY (U"Algorithm")
NORMAL (U"First we calculate the Discriminant from the data in the TableOfReal. "
	"See @@TableOfReal: To Discriminant@ for details.")
NORMAL (U"The eigenvectors of the Discriminant determine the directions that "
	"the data in the TableOfReal will be projected unto.")
MAN_END

MAN_BEGIN (U"TableOfReal: To Configuration (pca)...", U"djmw", 20220111)
INTRO (U"Calculates a @Configuration based on the principal components from the "
	"selected @TableOfReal.")
ENTRY (U"Setting")
TERM (U"##Number of dimensions")
DEFINITION (U"determines the number of dimensions of the resulting Configuration.")
ENTRY (U"Algorithm")
NORMAL (U"We form principal components without explicitly calculating the covariance matrix "
	"#C = #M\\'p #M, where #M is the matrix part of the TableOfReal. ")
LIST_ITEM (U"1. Calculate the singular value decomposition of #M. This results in "
	"#M = #U #D #V\\'p.")
LIST_ITEM (U"2. The principalComponent__%ij_ = \\su__%k=1..%numberOfColumns_ %M__%ik_ \\.c %V__%jk_.")
ENTRY (U"Remark")
NORMAL (U"The resulting configuration is unique up to reflections along the new principal directions.")
MAN_END

MAN_BEGIN (U"TableOfReal: To Correlation", U"djmw", 20151209)
INTRO (U"A command that creates a (%Pearson) @Correlation object from every "
	"selected @TableOfReal object. The correlations are calculated between "
	"columns.")
ENTRY (U"Algorithm")
NORMAL (U"The linear correlation coefficient %r__%ij_ (also called the %%product"
	" moment correlation coefficient% or %%Pearson's correlation coefficient%) "
	" between the elements of columns %i and %j is calculated as:")
EQUATION (U"%r__%ij_ = \\Si__%k_ (%x__%ki_ - %x\\-^__%i_)(%x__%kj_ - %x\\-^__%j_)/"
	"(\\Vr (\\Si__%k_(%x__%ki_ - %x\\-^__%i_)^2) \\Vr (\\Si__%k_(%x__%kj_ -"
	" %x\\-^__%j_)^2)),")
NORMAL (U"where %x__%mn_ is the element %m in column %n, and %x\\-^__%n_ "
	"is the mean of column %n.")
MAN_END

MAN_BEGIN (U"TableOfReal: To Correlation (rank)", U"djmw", 20151209)
INTRO (U"A command that creates a (%%Spearman rank-order%) @Correlation object "
	"from every selected @TableOfReal object. The correlations are calculated "
	"between columns.")
ENTRY (U"Algorithm")
NORMAL (U"The Spearman rank-order correlation coefficient %r__%ij_ between "
	"the elements of columns %i and %j is calculated as the linear correlation"
	" of the ranks:")
EQUATION (U"%r__%ij_ = \\Si__%k_ (%R__%ki_ - %R\\-^__%i_) "
	"(%R__%kj_ - %R\\-^__%j_) / (\\Vr (\\Si__%k_(%R__%ki_ - %R\\-^__%i_)^2) "
	"\\Vr (\\Si__%k_(%R__%kj_ - %R\\-^__%j_)^2)),")
NORMAL (U"where %R__%mn_ is the rank of element %m in column %n, "
	"and %R\\-^__%n_ is the mean of the ranks in column %n.")
MAN_END

MAN_BEGIN (U"TableOfReal: To Covariance", U"djmw", 20220111)
INTRO (U"A command that creates a @Covariance object from every "
	"selected @TableOfReal object. The covariances are calculated between "
	"columns.")
ENTRY (U"Algorithm")
NORMAL (U"The covariance coefficients %s__%ij_ "
	" between the elements of columns %i and %j are defined as:")
EQUATION (U"%s__%ij_ = \\Si__%k_ (%x__%ki_ - %x\\-^__%i_)(%x__%kj_ - %x\\-^__%j_)/"
	"(%numberOfObservations - %numberOfConstraints),")
NORMAL (U"where %x__%ki_ is the element %k in column %i, %x\\-^__%i_ "
	"is the mean of column %i, %numberOfObservations equals the number of rows in "
	"the table, and %numberOfConstraints equals 1.")
NORMAL (U"The actual calculation goes as follows")
LIST_ITEM (U"1. Centralize each column (subtract the mean).")
LIST_ITEM (U"2. Get its @@singular value decomposition@ #U #D #V\\'p.")
LIST_ITEM (U"3. Form #S = #V #D #V\\'p.")
LIST_ITEM (U"4. Divide all elements in #S by (%numberOfObservations - 1).")
MAN_END

MAN_BEGIN (U"TableOfReal: To Discriminant", U"djmw", 19990104)
INTRO (U"A command that creates a @Discriminant object from every selected "
	"@TableOfReal object. Row labels in the table indicate group membership.")
ENTRY (U"Algorithm")
NORMAL (U"We solve for directions #x that are eigenvectors of the generalized "
	"eigenvalue equation:")
EQUATION (U"#%B #x - %\\la #%W #x = 0,")
NORMAL (U"where #%B and #%W are the between-groups and the within-groups sums of "
	"squares and cross-products matrices, respectively. Both #%B and #%W are symmetric "
	"matrices. Standard formula show that both matrices can also "
	"be written as a matrix product. The formula above then transforms to:")
EQUATION (U"#%B__1_\\'p#%B__1_ #x - %\\la #%W__1_\\'p#%W__1_ #x = 0")
NORMAL (U"The equation can be solved with the @@generalized singular value decomposition@. "
	"This procedure is numerically very stable and can even cope with cases when both "
	"matrices are singular.")
NORMAL (U"The a priori probabilities in the Discriminant will be calculated from the number of "
	"%training vectors %n__%i_ in each group:")
EQUATION (U"%aprioriProbability__%i_ = %n__%i_ / \\Si__%k=1..%numberOfGroups_ %n__%k_")
MAN_END

MAN_BEGIN (U"TableOfReal: To PCA", U"djmw", 20160223)
INTRO (U"A command that creates a @PCA object from every selected "
	"@TableOfReal object, where the TableOfReal object is interpreted as row-oriented, i.e. %%numberOfRows% data vectors, each data vector has %%numberofColumns% elements.")
NORMAL (U"In @@Principal component analysis|the tutorial on PCA@ you will find more info on principal component analysis.")
MAN_END

MAN_BEGIN (U"TableOfReal: To SSCP...", U"djmw", 19990218)
INTRO (U"Calculates Sums of Squares and Cross Products (@SSCP) from the selected @TableOfReal.")
ENTRY (U"Algorithm")
NORMAL (U"The sums of squares and cross products %s__%ij_ "
	"between the elements of columns %i and %j are calculated as:")
EQUATION (U"%s__%ij_ = \\Si__%k_ (%x__%ki_ - %x\\-^__%i_)(%x__%kj_ - %x\\-^__%j_),")
NORMAL (U"where %x__%mn_ is the element %m in column %n, and %x\\-^__%n_ "
	"is the mean of column %n.")
MAN_END

MAN_BEGIN (U"TableOfReal: To PatternList and Categories...", U"djmw", 20040429)
INTRO (U"Extracts a @PatternList and a @Categories from the selected @TableOfReal.")
NORMAL (U"The selected rows and columns are copied into the PatternList and "
	"the corresponding row labels are copied into a Categories.")
MAN_END

MAN_BEGIN (U"TableOfReal: To CCA...", U"djmw", 20020424)
INTRO (U"A command that creates a @CCA object from the selected "
	"@TableOfReal object.")
ENTRY (U"Settings")
TERM (U"##Dimension of dependent variate (ny)#")
DEFINITION (U"defines the partition of the table into the two parts whose "
	"correlations will be determined. The first %ny columns should be the "
	"dependent part, the rest of the columns will be interpreted as the "
	"independent part (%nx columns). In general %nx should be larger than or "
	"equal to %ny.")
ENTRY (U"Behaviour")
NORMAL (U"Calculates canonical correlations between the %dependent and the "
	"%independent parts of the table. The corresponding "
	"canonical coefficients are also determined.")
ENTRY (U"Algorithm")
NORMAL (U"The canonical correlation equations for two data sets #T__%y_ "
	"[%n \\xx %p] and #T__%x_ [n \\xx %q] are:")
EQUATION (U"(1)    (#S__%yx_ #S__%xx_^^-1^ #S__%yx_\\'p -\\la #S__%yy_)#y = #0")
EQUATION (U"(2)    (#S__%yx_\\'p #S__%yy_^^-1^ #S__%yx_ -\\la #S__%xx_)#x = #0")
NORMAL (U"where #S__%yy_ [%p \\xx %p] and #S__%xx_ [%q \\xx %q] are the "
	"covariance matrices of data sets #T__%y_ and  #T__%x_, respectively, "
	"#S__%yx_ [%p \\xx %q] is the matrix of covariances between data sets "
	"#T__%y_ and #T__%x_, and the vectors #y and #x are the %%canonical "
	"weights% or the %%canonical function coefficients% for the dependent and "
	"the independent data, respectively. "
	"In terms of the (dependent) data set #T__%y_ and the (independent) data set "
	"#T__%x_, these covariances can be written as:")
EQUATION (U"#S__%yy_ =  #T__%y_\\'p #T__%y_,  #S__%yx_ = #T__%y_\\'p #T__%x_ and "
	"#S__%xx_ =  #T__%x_\\'p #T__%x_.")
NORMAL (U"The following @@singular value decomposition@s ")
EQUATION (U"#T__%y_ = #U__%y_ #D__%y_ #V__%y_\\'p and #T__%x_ = #U__%x_ #D__%x_ "
	"#V__%x_\\'p ")
NORMAL (U"transform equation (1) above into:")
EQUATION (U"(3)    (#V__%y_ #D__%y_ #U__%y_\\'p#U__%x_ #U__%x_\\'p #U__%y_ #D__%y_ "
	"#V__%y_\\'p - \\la #V__%y_ #D__%y_ #D__%y_ #V__%y_\\'p)#y = 0 ")
NORMAL (U"where we used the fact that:")
EQUATION (U"#S__%xx_^^-1^ = #V__%x_ #D__%x_^^-2^ #V__%x_\\'p.")
NORMAL (U"Equation (3) can be simplified by multiplication from the left by "
	"#D__%y_^^-1^ #V__%y_' to:")
EQUATION (U" (4)   ((#U__%x_\\'p #U__%y_)\\'p (#U__%x_\\'p #U__%y_) - \\la #I)#D__%y_ "
	"#V__%y_\\'p #y = #0")
NORMAL (U"This equation can, finally, be solved by a substitution of the s.v.d "
	"of  #U__%x_\\'p #U__%y_ = #U #D #V\\'p  into (4). This results in")
EQUATION (U"(5)  (#D^^2^ - \\la #I) #V\\'p #D__%y_ #V__%y_\\'p #y = #0")
NORMAL (U"In an analogous way we can reduce eigenequation (2) to:")
EQUATION (U"(6)  (#D^^2^ - \\la #I) #U\\'p #D__%x_ #V__%x_\\'p #x = #0")
NORMAL (U"From (5) and (6) we deduce that the eigenvalues in both equations "
	"are equal to the squared singular values of the product matrix "
	"#U__%x_\\'p#U__%y_. "
	"These singular values are also called %%canonical "
	"correlation coefficients%. The eigenvectors #y and #x can be obtained "
	"from the columns of the following matrices #Y and #X:")
EQUATION (U"#Y = #V__%y_ #D__%y_^^-1^ #V")
EQUATION (U"#X = #V__%x_ #D__%x_^^-1^ #U")
NORMAL (U"For example, when the vector #y equals the first column of #Y and "
	"the vector #x equals "
	"the first column of #X, then the vectors #u = #T__%y_#y and #v = #T__%x_#x "
	"are the linear combinations from #T__%y_ and #T__%x_ that have maximum "
	"correlation. Their correlation coefficient equals the first canonical "
	"correlation coefficient.")
MAN_END

MAN_BEGIN (U"TableOfReal: To TableOfReal (means by row labels)...", U"djmw", 20140117)
INTRO (U"A command that appears in the ##Multivariate statistics# menu if you select a @@TableOfReal@. "
	"It calculates the multivariate means for the different row labels from the selected TableOfReal.")
ENTRY (U"Setting")
TERM (U"##Expand#")
DEFINITION (U"when %off, then for a table with %n rows and %m different labels (%m\\<_%n), the resulting table will have %m rows. "
	"When %on, the dimensions of the resulting table will be the same as the originating, and corresponding means substituded "
	"in each row.")
ENTRY (U"Example")
NORMAL (U"The following commands")
CODE (U"@@Create TableOfReal (Pols 1973)...|Create TableOfReal (Pols 1973)@: \"no\"")
CODE (U"To TableOfReal (means by row labels): 0")
NORMAL (U"will result in a new TableOfReal that has 12 rows. Each row will contain the mean F1, F2 and F3 values "
	"for a particular vowel. These means were obtained from 50 representations of that vowel.")
NORMAL (U"If we had chosen the %expansion:")
CODE (U"To TableOfReal (means by row labels): \"yes\"")
NORMAL (U"the resulting TableOfReal would have had 600 rows. This representation  comes in handy when, for example, "
	"you have to calculate deviations from the mean.")
MAN_END

MAN_BEGIN (U"TextGrid: Extend time...", U"djmw", 20020702)
INTRO (U"Extends the domain of the selected @TextGrid object.")
ENTRY (U"Settings")
TERM (U"##Extend domain by#")
DEFINITION (U"defines the amount of time by which the domain will be extended.")
TERM (U"##At")
DEFINITION (U"defines whether starting times or finishing times will be modified.")
ENTRY (U"Behaviour")
NORMAL (U"We add an extra (empty) interval into each %%interval tier%. "
	"This is necessary to keep original intervals intact. According to the "
	"value of the second argument, the new interval will be added at the "
	"beginning or at the end of the tier.")
NORMAL (U"For %%point tiers% only the domain will be changed.")
MAN_END

MAN_BEGIN (U"TextGrid & DurationTier: To TextGrid (scale times)", U"djmw", 20170612)
INTRO (U"Scales the durations of the selected @@TextGrid@ intervals as specified by the selected @@DurationTier@.")
MAN_END

MAN_BEGIN (U"TextGrid: To DurationTier...", U"djmw", 20170617)
INTRO (U"Creates a @@DurationTier@ that could scale the durations of the specified intervals of the selected @@TextGrid@ with a specified factor.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (6), U""
	Manual_DRAW_SETTINGS_WINDOW ("TextGrid: To DurationTier", 6)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Tier number", "1")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Time scale factor", "2.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Left transition duration", "1e-10")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Right transition duration", "1e-10")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Scale intervals whose labels", "starts with")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("...the text", "hi")
)
TERM (U"##Tier number#")
DEFINITION (U"specifies the tier with the intervals.")
TERM (U"##Time scale factor#")
DEFINITION (U"specifies the scale factor by which the duration of a selected interval has to be multiplied.")
TERM (U"##Left transition duration#")
DEFINITION (U"specifies how long it takes to go from a time scale factor of 1.0 to the specified one. "
	"Default a very small duration is used. ")
TERM (U"##Right transition duration#")
DEFINITION (U"specifies the time it takes to go from the specified time scale factor to 1.0. "
	"Default a very small duration is used.")
TERM (U"##Scale intervals whose labels")
DEFINITION (U"specifies the interval selection criterion.")
TERM (U"##...the text")
DEFINITION (U"specifies the text used in the selection criterion.")
ENTRY (U"Algorithm")
SCRIPT (5, 3, U"ymin = 0.9\n"
	"Axes: 0, 1, ymin, 2.0\n" 
	"t1 = 0.2\n"
	"t4 = 0.9\n"
	"timeScaleFactor = 1.5\n"
	"leftTransitionDuration = 0.1\n"
	"rightTransitionDuration = 0.2\n"
	"t2 = t1 + leftTransitionDuration\n"
	"t3 = t4 - rightTransitionDuration\n"
	"Solid line\n"
	"Draw line: t1, 1, t2, timeScaleFactor\n"
	"Draw line: t2, timeScaleFactor, t3, timeScaleFactor\n"
	"Draw line: t3, timeScaleFactor, t4, 1.0\n"
	"Dotted line\n"
	"Draw line: 0, 1, t1, 1\n"
	"Draw line: t4, 1.0, 1.0, 1.0\n"
	"Draw line: t1, ymin, t1, timeScaleFactor+0.1\n"
	"Draw line: t2, ymin, t2, timeScaleFactor+0.1\n"
	"Draw two-way arrow: t1, timeScaleFactor+0.1, t2, timeScaleFactor+0.1\n"
	"Text: (t1+t2)/2, \"Centre\", timeScaleFactor+0.1, \"Bottom\", \"leftTransitionDuration\"\n"
	"Draw line: t3, ymin, t3, timeScaleFactor+0.1\n"
	"Draw line: t4, ymin, t4, timeScaleFactor+0.1\n"
	"Draw two-way arrow: t3, timeScaleFactor+0.1, t4, timeScaleFactor+0.1\n"
	"Text: (t3+t4)/2, \"Centre\", timeScaleFactor+0.1, \"Bottom\", \"rightTransitionDuration\"\n"
	"One mark bottom: t1, \"no\", \"yes\", \"no\", \"t__1_\"\n"
	"One mark bottom: t2, \"no\", \"yes\", \"no\", \"t__2_\"\n"
	"One mark bottom: t3, \"no\", \"yes\", \"no\", \"t__3_\"\n"
	"One mark bottom: t4, \"no\", \"yes\", \"no\", \"t__4_\"\n"
	"One mark left: 1.0, \"yes\", \"yes\", \"no\", \"\"\n"
	"Text bottom: \"yes\", \"Time (s) \\->\"\n"
	"Text left: \"yes\", \"Duration scale factor \\->\"\n"
	"Draw inner box\n"
)
NORMAL (U"For each selected interval its duration will be specified by four points in the duration tier as the figure above shows. "
	"Given that the start time "
	"and the end time of the interval are at %t__1_ and %t__4_, respectively, the times of these four points will be "
	"%t__1_, %t__2_=%t__1_+%%leftTransitionDuration%, %t__3_=%t__4_-%%rightTransitionDuration% and %t__4_. The associated duration scale factors "
	"will be 1.0, %%timeScalefactor%, %%timeScalefactor% and 1.0, respectively.")
NORMAL (U"Normally we would use very small values for the right and the left transition durations, and the curve in the figure above "
	"would look more like a rectangular block instead of the trapezium above. If, on the contrary, larger values for the durations are taken, such that the sum of "
	"the left and the right transition durations %%exceeds% the interval's width, then the ordering of the time points at %t__1_ to %t__4_ changes, "
	"which will have unexpected results on the duration tier.")
ENTRY (U"Examples")
NORMAL (U"Suppose you want to change the durations of some parts in a sound. The way to go is:")
LIST_ITEM (U"1. Create a TextGrid with at least one interval tier with the segments of interest labeled.")
LIST_ITEM (U"2. Select the TextGrid and choose ##To DurationTier...#.")
LIST_ITEM (U"3. Select the Sound and create a @@Manipulation@ object from it. Check and potentially correct the pitch measurements in this Manipulation "
	"(##View & Edit#), as the quality of the resynthesis depends critically on the quality of the pitch measurements.")
LIST_ITEM (U"4. Select the Manipulation and the DurationTier together and choose ##Replace duration tier#.")
LIST_ITEM (U"5. Select the Manipulation object and choose ##Get resynthesis (overlap-add)#. The resulting Sound will have the "
	"durations of its selected intervals changed.")
LIST_ITEM (U"6. Optionally you may also want to scale the TextGrid to line up with the newly created Sound. To do so, select the "
	"TextGrid and the DurationTier together and choose ##To TextGrid (scale times)#. You will get a new TextGrid that is nicely "
	"aligned with the new sound.")
MAN_END

MAN_BEGIN (U"NavigationContext", U"djmw", 20210723)
INTRO (U"One of the @@types of objects@ in Praat. The ##NavigationContext# contains the information that the "
	"@@TextGridNavigator@ needs to search for a match on one tier of a @@TextGrid@.")
ENTRY (U"What is inside a NavigationContext?")
TERM (U"##Topic label set#")
DEFINITION (U"defines the labels that we want to find. As an example consider the following "
	"set of vowel labels { \"u\", \"o\", \"a\", \"i\", \"e\" }. Our intention is to find only those intervals in "
	"the specified tier of the TextGrid that match with one of these labels. ")
TERM (U"##Topic match criterion#")
DEFINITION (U"defines the type of match you want. To name just a few of the possible options: "
	"##is equal to# or ##is not equal to# or ##contains# or ##does not contain# or "
	"##starts with# or ##does not start with#. Many more match criteria can be chosen and these criteria "
	"come in pairs where the second one of the pair is the negation of the first.")
TERM (U"##Combine topic matches with#")
DEFINITION (U"defines whether the match criteria of the labels in the topic set have to be combined by "
	"##OR# or by  ##AND#. Consider the topic set defined above. If the topic match criterion "
	"chosen was ##is equal to# then we would want the match for the current interval label in the tier to succeeed "
	"only if the expression (currentLabel\\$  = \"u\" OR currentLabel\\$  = \"o\" OR currentLabel\\$  = \"a\" "
	"OR currentLabel\\$  = \"i\" OR currentLabel\\$  = \"e\") evaluates as true, i.e. if one of the labels of the "
	"topic set matches the current label in the tier then we would have a match. "
	"On the other hand, if the chosen match criterium were the opposite, ##not equal to#, then we would want our match for "
	"the current interval label to succeed only if the expression (currentLabel\\$  \\=/ \"u\" AND currentLabel\\$  \\=/ \"o\" "
	"AND currentLabel\\$  \\=/ \"a\" AND currentLabel\\$  \\=/ \"i\" AND currentLabel\\$  \\=/ \"e\") evaluates as true. "
	"Therefore, in general, for labels that are single words, a positive match criterion, like ##is equal to#, "
	"we would choose ##OR# while for a negative match criterion, like ##is not equal to# we would choose ##AND#. "
	"However, for labels that are multi-word both ##AND# as well as ##OR# combinations can be useful.")
TERM (U"##Before label set#")
DEFINITION (U"defines the labels that the label of the preceding interval has to match if the current interval label "
	"matches the topic label set. This makes the search context-sensitive because it is not enough that the current "
	"interval label matches (in a specified way) the topic set but also the label of the preceding interval has to "
	"match the before set. If, for example, the before set contains "
	"\"p\", \"t\" and \"k\" then in combination with the topic labels defined above this would limit the search to only "
	"those vowels that are preceded by a plosive.")
TERM (U"##Combine before matches with#")

DEFINITION (U"defines, just like for the topic match criterion, the kind of match you are looking for only now for "
	"the labels of the %%before set%.")
TERM (U"##Before match boolean#")
DEFINITION (U"defines, also in analogy with the description of the topic match boolean, the relation between the matches "
	"of the labels in the %%before set%.")
TERM (U"##After label set#")
DEFINITION (U"defines the labels that the label of the following interval has to match if the current interval label "
	"matches the topic label set. An example set could consist of "
	"\"m\" and \"n\". In combination with the topic set we would search for vowels followed by a nasal.")
TERM (U"##After match criterion# and ##Combine after matches with#")
DEFINITION (U"have the same meaning as defined above for the topic and before sets.")
TERM (U"##Context combination criterion#")
DEFINITION (U"defines how the before and after sets have to be combined in the matching. The possible options are ##before#, or "
	"##after# or ##before and after# or ##before or after, not both# or ##before or after, or both# or finally "
	"##no before and no after#. Given the topic, before and after labels examples defined above, choosing ##before and after# "
	"would limit the search to vowels preceded by a plosive and followed by a nasal.")
TERM (U"##Exclude topic labels boolean#")
DEFINITION (U"when on, only the before and / or the after label set will be used in matching. Of course this effect "
	"could also be reached by leaving the topic set empty.")
MAN_END

MAN_BEGIN (U"TextGridNavigator", U"djmw", 20230801)
INTRO (U"One of the @@types of objects@ in Praat. A ##TextGridNavigator# is a multi-tier search machine.")
ENTRY (U"What is a multi-tier search machine?")
NORMAL (U"A multi-tier search machine enables you to find an interval (or a point) on a tier, based on criteria "
	"that can have a specified relation with intervals (or points) on the same tier or on other tiers of the TextGrid. "
	"Matches on each tier are based on the labels in a tier. Matches found on different tiers are combined based on time. "
	"The match criteria for each tier are specified in the tier's @@NavigationContext@. ")
ENTRY (U"Example 1: Single-tier topic search: ")
NORMAL (U"We create a TextGridNavigator that searches for the occurrence in tier 1 of one of the labels "
	"in a %topic set that consists of the labels { \"a\", \"e\", \"i\", \"o\", \"u\" }. If a label in tier 1 equals one of the labels in this topic set we have a match. "
	"The command to create the TextGridNavigator for the selected TextGrid is:")
CODE (U"To TextGridNavigator (topic only): 1,")
CODE (U"... { \"a\", \"e\", \"i\", \"o\", \"u\" }, \"is equal to\", \"OR\",")
CODE (U"... \"Match start to Match end\"")
NORMAL (U"In this case the tier's ##NavigationContext# is very simple as the searching / matching only involves "
	"labels of the topic set. After the TextGridNavigator has been created it doesn't need the TextGrid anymore because "
	"tier 1 has been copied into the navigator object. The following code would find all the intervals that match and "
	"get their start time, end time and label. ")
CODE (U"Find first")
CODE (U"index = Get index: tierNumber, \"topic\"")
CODE (U"while index > 0")
	CODE1 (U"selectObject: textgrid")
	CODE1 (U"startTime = Get start time: tierNumber, \"topic\"")
	CODE1 (U"endTime = Get end time: tierNumber, \"topic\"")
	CODE1 (U"label$ = Get label: tierNumber, \"topic\"")
	CODE1 (U"...")
	CODE1 (U"selectObject: navigator")
	CODE1 (U"Find next")
	CODE1 (U"index = Get index: tierNumber, \"topic\"")
CODE (U"endwhile")
NORMAL (U"The ##Find first# command finds the index of the first interval (or point) in the tier that matches. "
	"The ##Get index# command returns this index. Besides giving the index of the topic match it can also return "
	"indices of before or after matches. If no match was found it returns zero. ##Find next# finds the next interval "
	"that matches.")
NORMAL (U"Instead of finding the indices one at a time in a %while loop until we are done, we could use alternatives "
	"and query for a list of all indices or times where the labels match. We then know beforehand how many matches we have "
	"and therefore we can use a %for loop.")
CODE (U"tierNumber = 1")
CODE (U"navigator = To TextGridNavigator (topic only): tierNumber,")
CODE (U"... { \"a\", \"e\", \"i\", \"u\", \"o\" }, \"is equal to\", \"OR\",")
CODE (U"... \"Match start to Match end\"")
CODE (U"startTimes# = List start times: \"topic\"")
CODE (U"labels$# = List labels: \"Topic\"")
CODE (U"endTimes# = List end times: \"topic\"")
CODE (U"for index to size (startTimes#)")
	CODE1 (U"duration = endTimes# [index] - startTimes# [index]")
	CODE1 (U"<your code>")
CODE (U"endfor")
NORMAL (U"We could also combine the start and end times into one query list:")
CODE (U"domains## = List domains: \"Topic start to Topic end\"")
CODE (U"numberOfMatches = numberOfRows (domains##)")
NORMAL (U"and use it in a loop as, for example,")
CODE (U"for index to numberOfMatches")
	CODE1 (U"duration = domains##  [index, 2] - domains## [index, 1]")
	CODE1 (U"<your code>")
CODE (U"endfor")

ENTRY (U"Example 2: Single-tier before + topic + after search ")
NORMAL (U"A more complex example could query for a vowel from the same %topic set as we used above but only matches if "
	"additionally it is immediately preceded by an unvoiced plosive from a %Before set, like e.g. { \"p\", \"t\", \"k\" }, "
	"and also is immediately followed by a nasal from an %After set, like { \"m\", \"n\" }. "
	"The command to create this navigator once the TextGrid is selected is: ")
CODE (U"tierNumber = 1")
CODE (U"To TextGridNavigator: tierNumber,")
CODE (U"... { \"a\", \"e\", \"i\", \"o\", \"u\" }, \"is equal to\", \"OR\",")
CODE (U"... { \"p\", \"t\", \"k\" }, \"is equal to\", \"OR\",")
CODE (U"... { \"m\", \"n\" }, \"is equal to\", \"OR\",")
CODE (U"... \"before and after\", \"false\", \"Topic start to Topic end\"")
NORMAL (U"This example and the one above involve only searches on one tier and both follow the same scheme: they "
	"search for a %%topic label% which may be preceded by a %%before label% and/or followed by an %%after label%. "
	"The %topic, %before and %%after label% belong to different sets (the three sets may of course have labels in common). "
	"With a choice from a number of "
	"use criterions like ##Before or After, not both# or ##Before and After# you specify how the corresponding label "
	"sets will be used during the matching.")
NORMAL (U"For each tier in the TextGrid, we can define a tier search based on tier-specific sets of %%topic labels%, and/ or "
	"tier-specific %%before% and %after labels%. Besides these maximally three sets of labels, we also need to specify "
	"the kind of match that we want. This is all specified in a tier's @@NavigationContext@. ")

ENTRY (U"Example 3: Multi-tier search by combining single-tier searches")
NORMAL (U"A multi-tier search navigator can be created by successively adding one or more single tier searches to an "
	"already existing TextGridNavigator. Each tier added for searching should have a unique tier number. "
	"To combine the matches on different tiers we have to chose how to relate these matches on the basis of time because time is "
	"the only feature that all tiers have in common as they all have the same time domain. "
	"Suppose the TextGrid has two tiers: the first is a phoneme tier like we used in the previous example and the second "
	"is a syntactic tier where intervals may be labeled as \"Noun\", \"Determiner\", \"Verb\" etc. "
	"We want to restrict the vowel search on tier 1 to only those vowels that are within an interval at tier 2 that "
	"is labeled as \"Noun\" and is preceded by an interval labeled \"Determiner\". "
	"The only complexity of a multi-tier TextGrid navigator is within its creation process. The following script shows how "
	"to create the two-tier navigator.")	
CODE (U"phonemeTierNumber = 1")
CODE (U"syntaxTierNumber = 2")
CODE (U"selectObject: texgrid")
CODE (U"navigator = To TextGridNavigator: phonemeTierNumber,")
CODE (U"... { \"a\", \"e\", \"i\", \"o\", \"u\" }, \"is equal to\", \"OR\",")
CODE (U"... { \"p\", \"t\", \"k\" }, \"is equal to\", \"OR\",")
CODE (U"... { \"m\", \"n\" }, \"is equal to\", \"OR\",")
CODE (U"... \"before and after\", \"false\", \"Topic start to Topic end\"")
CODE (U"selectObject: texgrid, navigator")
CODE (U"Add search tier: syntaxTierNumber,")
CODE (U"... { \"Noun\" }, \"is equal to\", \"OR\",")
CODE (U"... { \"Determiner\" }, \"is equal to\", \"OR\",")
CODE (U"... { }, \"is equal to\", \"OR\",")
CODE (U"... \"before\", \"false\", \"Topic start to Topic end\",")
CODE (U"... overlaps before and after")
NORMAL (U"The script starts by creating the navigator for the %topic tier, tier number 1, that we already discussed in "
	"##Example 2# which searches for vowels in a plosive-nasal context. "
	"The %%match domain%, i.e. that time interval in the topic tier that will be used as the anchor for the comparisons with the match domains of other tiers, is chosen as ##Topic start to Topic end#. This means that, in case of a match on this tier, "
	"the start and end times of the match equal the start and end times of the matched vowel interval, respectively. "
	"More options exist for the choice of the match domain. "
	"The chosen match domain on the %topic tier will %always serve as the anchor for the comparisons with matches on other tiers. "
	"The ##Add search tier# command adds a tier to the navigator that will be searched for combinations of a \"Noun\" label "
	"preceded by a \"Determiner\" label. "
	"The @@NavigationContext@ for this tier therefore consists of only a %topic set and a %before set and both have "
	"only one member. "  
	"The matches on the added search tier will be used to limit the number of matches on the %topic tier by checking if a "
	"specified time relation between the match domains on both tiers exist. "
	"Suppose that on both tiers we have a match which means that on the %topic tier we found a vowel that is preceded "
	"by a plosive and followed by a nasal, and, on the syntax tier we found an interval labeled \"Noun\" that is preceded "
	"by an interval labeled \"Determiner\". The start and end time of the match domain on the topic tier equal the start and "
	"end time of the vowel interval; name them %tmin1 and %tmax1, respectively. The match domain on the syntax tier will be "
	"the interval [%tmin2, %tmax2], where %tmin2 and %tmax2 are the start and end time of the interval labeled \"Noun\" because we "
	"also have chosen the ##Topic start to Topic end# option as the match domain for the syntax tier. " 
	"How to combine these two separate matches on the two different tiers will be determined by the last option of the "
	"##Add search tier# command which was chosen as ##overlaps before and after#. "
	"Now, only if %tmin2 < %tmin1 and %tmax2 > %tmax1 the two intervals have the desired alignment and the match would succeed. ")
NORMAL (U"We can, of course, make our match stricter and demand that, for example, the complete three phoneme match of the topic "
	"tier is located within the match domain of the syntax tier by issuing the following command:")
CODE (U"selectObject: navigator")
CODE (U"Modify match domain: phonemeTierNumber, \"Before start to Topic end\"")
NORMAL (U"Even more stricter to exact alignment:")
CODE (U"Modify match domain alignment: syntaxTierNumber, \"touches before and after\"")
ENTRY (U"Using the TextGridNavigator with other TextGrids")
NORMAL (U"If the TextGrids in your corpus have identical structure, i.e. your search tiers all have the same tier number "
	"you can simply reuse your already defined navigator.")
CODE (U"selectObject: navigator, otherTextGrid")
CODE (U"Replace search tiers")
MAN_END

MAN_BEGIN (U"TextGrid: To TextGridNavigator...", U"djmw", 20210723)
INTRO (U"A command to create a new @@TextGridNavigator@ object for the selected @@TextGrid@.")
ENTRY (U"Settings")
TERM (U"##Tier number#")
DEFINITION (U"defines the tier of the TextGrid that will be used for the navigation. This tier will be copied "
	"into the TextGridNavigator. This tier will be called the %%topic tier%.")
TERM (U"Next follow the settings for the @@NavigationContext|navigation context@. You can define a full "
	"context-sensitive search on the selected tier. This means that whether a label in an interval matches may depend "
	"on whether the label of the preceeding interval matches a certain condition and / or whether the label in the succeeding interval matches a possibly different condition. ")
TERM (U"##Match domain#")
DEFINITION (U"defines the start and end times of the match domain on this tier. Various options exist, the default, "
	"##Topic start to Topic end# takes the start time and end time of the interval that matches the topic. "
	"As is explained in the @@TextGridNavigator@ matching in multi-tier search is based on time intervals,  "
	"therefore this option is irrelevant for single-tier searches.")
MAN_END

MAN_BEGIN (U"TextGridNavigator & TextGrid: Add search tier...", U"djmw", 20220117)
INTRO (U"A command to extend the search of the selected @@TextGridNavigator@ with another tier of the selected @@TextGrid@. ")
ENTRY (U"Settings")
TERM (U"##Tier number#")
DEFINITION (U"defines the tier of the TextGrid that will also be used to limit make the searches more specific. This tier has to be different from the tiers that are already in use.")
TERM (U"The @@NavigationContext|navigation context@ for this tier has to be defined.")
TERM (U"##Match domain#")
DEFINITION (U"defines the start and end times of the match domain on this tier. Various options exist, the default, "
	"##Topic start to Topic end# takes the start time and end time of the interval that matches the topic on this tier. ")
TERM (U"##Match domain alignment#")
DEFINITION (U"defines the alignment between the match domain in this tier to the match domain in the topic tier. "
	"For example, if the option ##overlaps before and after# were chosen and the match domain in the selected tier "
	"is [%%tmin2%, %%tmax2%] while on the topic tier it is [%%tmin1%, %%tmax1%] then a match would only succeed "
	"if %%tmin2% < %%tmin1% and %%tmax2% > %%tmax1%.")
MAN_END


MAN_BEGIN (U"TextGridNavigator & TextGrid: Replace search tiers", U"djmw", 20210719)
INTRO (U"A command to replace the current search tiers in the selected @@TextGridNavigator@ with the tiers from the "
	"selected @@TextGrid@.")
NORMAL (U"This is the easiest way to use the TextGridNavigator to navigate another TextGrid.")
MAN_END

MAN_BEGIN (U"TIMIT acoustic-phonetic speech corpus", U"djmw", 19970320)
INTRO (U"A large American-English speech corpus that resulted from the joint efforts "
	"of several American research sites.")
NORMAL (U"The TIMIT corpus contains a total of 6300 sentences, 10 sentences spoken by "
	"630 speakers selected from 8 major dialect regions of the USA. 70\\%  of "
	"the speakers are male, 30\\%  are female.")
NORMAL (U"The text corpus design was done by the Massachusetts Institute of "
	"Technology (MIT), Stanford Research Institute and Texas Instruments (TI). "
	"The speech was recorded at TI, transcribed at MIT, and has been maintained, "
	"verified and prepared for CDROM production by the American National Institute "
	"of Standards and Technology (NIST) (@@Lamel, Kassel & Seneff (1986)@).")
MAN_END

MAN_BEGIN (U"VowelEditor", U"djmw", 20200403)
INTRO (U"An Editor for generating vowel-like @@sound|Sound@s from mouse movements.")
ENTRY (U"How to get a sound")
NORMAL (U"With the mouse button down, you can move the mouse cursor around in the plane "
	"spanned by the first two formants. While you move the cursor around, the positions you trace will be "
	"indicated by blue dots. After you release the mouse button, the color of the trajectory will change "
	"to black. Next you will hear the vowel-like sound whose "
	"first two formants follow this trajectory. (The small bars on the trajectory are time markers. With "
	"default settings, time markers are at 50 milliseconds apart and they may give you an indication of the speed by which you traversed the trajectory.)")
ENTRY (U"The interface")
NORMAL (U"In the lower part of the editor a number of buttons and fields are displayed.")
TERM (U"##Play")
DEFINITION (U"will play the trajectory.")
TERM (U"##Reverse")
DEFINITION (U"will reverse the trajectory and play it.")
TERM (U"##Publish")
DEFINITION (U"will publish the sound in the list of objects.")
TERM (U"##Duration (s)")
DEFINITION (U"allows to modify the duration of the current trajectory. ")
TERM (U"##Extend (s)")
DEFINITION (U"determines the duration of the straight line trajectory that connects the endpoint of the current trajectory with the startpoint of a new trajectory. You may extend the current trajectory by starting a new trajectory with the shift button pressed. After you finished the new trajectory, three trajectories will be appended: the current one, the straight line one and the new one.")
TERM (U"##Start F0 (Hz)")
DEFINITION (U"determines the fundamental frequency at the start of the trajectory.")
TERM (U"##F0 slope (oct/s)")
DEFINITION (U"determines how many octaves the pitch will changes during the course of the trajectory.")
NORMAL (U"The bottom line in the Editor displays the first and second formant frequency and the fundamental frequency at the start point and the endpoint of the trajectory.")
ENTRY (U"Edit menu")
TERM (U"##Set F0...")
DEFINITION (U"Set pitch and slope.")
TERM (U"##Reverse trajectory")
DEFINITION (U"Reverses the trajectory (like editor button).")   // ??
TERM (U"##Modify trajectory duration...")
DEFINITION (U"Modifies trajectory duration (like editor field).")   // ??
TERM (U"##New trajectory...")
DEFINITION (U"Set startpoint, endpoint and duration of a new trajectory.")
TERM (U"##Extend trajectory...")
DEFINITION (U"Extend current trajectory to...")
TERM (U"##Shift trajectory...")
DEFINITION (U"Shift current trajectory.")
ENTRY (U"View menu")
TERM (U"##F1 & F2 range...#")
DEFINITION (U"Modify the horizontal and vertical scales.")
TERM (U"##Show vowel marks from fixed set...#")
DEFINITION (U"Show the vowel marks in the editor from a fixed set of vowel inventories.")
TERM (U"##Show vowel marks from Table file...#")
DEFINITION (U"Put your own marks in the editor. The Table needs to have at least three mandatory columns "
	"labeled \"Vowel\", \"F1\" and  \"F2\" and "
	"two optional column labeled \"Size\" and \"Colour\". The Vowel column contains the vowel marker labels, the F1 and "
	"F2 columns have the first and second formant frequencies in Hertz. The optional Size column contains "
	"the font size of the vowel markers, while the Colour column contains the @@Colour|colour@ specification of each vowel. "
	"The Table has to be in tab-separated format (or saved in Praat as a binary or text Table file)."
)
TERM (U"##Show trajectory time markers every...")
DEFINITION (U"Shows time markers as small bars orthogonal to the trajectory. ")
ENTRY (U"File menu")
TERM (U"##Preferences...#")
DEFINITION (U"Here you can modify the sharpness of the F1 and F2 peaks and also add a number of higher formants.")
TERM (U"##Publish Sound")
DEFINITION (U"Make the synthesized sound available in the object menu.")
TERM (U"##Extract FormantTier")
TERM (U"##Extract PitchTier")
DEFINITION (U"Publish the Sound, the PitchTier and the FormantTier from the trajectory.")
TERM (U"##Draw trajectory...")
DEFINITION (U"Draws the trajectory in the picture window.")
MAN_END

MAN_BEGIN (U"VowelEditor: Show vowel marks from Table file...", U"djmw", 20200403)  // ppgb 2024
INTRO (U"A command in the @@VowelEditor@ that lets you set your own vowel marks. ")
ENTRY (U"Layout of the Table")
NORMAL (U"The Table needs at least three mandatory columns labeled \"Vowel\", \"F1\" and  \"F2\" and "
	"two optional column labeled \"Size\" and \"Colour\". The Vowel column contains the vowel marker labels, the F1 and "
	"F2 columns have the first and second formant frequencies in Hertz. The optional Size column contains "
	"the font size of the vowel markers, while the Colour column contains the @@Colour|colour@ specification of each vowel.")
NORMAL (U"The Table has to be in tab-separated format (or saved in Praat as a binary or text Table file).")
MAN_END

/********************** GSL ********************************************/
MAN_BEGIN (U"incompleteBeta", U"djmw", 20071024)
TERM (U"##incompleteBeta (%a, %b, %x)")
DEFINITION (U"I__x_(%a,%b) = 1/beta(%a,%b)\\in__0_^%x %t^^%a-1^(1-%t)^^%b-1^ dt,")
NORMAL (U"for 0 \\<_ %x \\<_ 1 and %a and %b and %a+%b not equal to a negative integer.")
//double incompleteBeta (double a, double b, double x);
//Pre: 0<= x <= 1; a> 0, b>0
//Def: $I_x(a,b)=B_x(a,b)/B(a,b)=1/B(a,b) \int_0^x t^{a-1}(1-t)^{b-1)dt$
//Limiting values: $I_0(a,b)=0 I_1(a,b)=1$
//Symmetry: $I_x(a,b) = 1 - I_{1-x}(b,a)$
MAN_END

MAN_BEGIN (U"incompleteGammaP", U"djmw", 20170531)
TERM (U"##incompleteGammaP (%a, %x)")
DEFINITION (U"incompleteGammaP = 1/\\Ga(%a)\\in__0_^%x e^^-%t^%t^^%a-1^ dt,")
NORMAL (U"where %x and %a are real numbers that satisfy %x\\>_ 0 and %a not being a negative integer.")
MAN_END

MAN_BEGIN (U"lnBeta", U"djmw", 20071024)
TERM (U"##lnBeta (%a, %b)")
DEFINITION (U"Computes the logarithm of the #beta function, subject to %a and %b and %a+%b not being negative integers.")
MAN_END

/********************* References **************************************/

MAN_BEGIN (U"Alexandrescu (2017)", U"djmw", 20250120)
NORMAL (U"A. Alexandrescu (2017): \"Fast deterministic selection.\" %%Symposium on experimental algorithms (SEA 2017):% 1\\--18.")
MAN_END

MAN_BEGIN (U"Anderson et al. (1999)", U"djmw", 20200131)
NORMAL (U"E. Anderson, Z. Bai, C. Bischof, S. Blackford, J. Demmel, J. Dongarra, J. Du Croz, A. Greenbaum, "
	"S. Hammarling, A. McKenney & D. Sorensen (1999): %%LAPACK users' guide%. Third edition. Philadelphia, PA: Society for Industrial and Applied Mathematics.")
MAN_END

MAN_BEGIN (U"Bai & Demmel (1993)", U"djmw", 19981007)
NORMAL (U"Z. Bai & J. Demmel (1993): \"Computing the generalized singular value "
	"decomposition.\" %%SIAM J. Sci. Comput.% #14: 1464\\--1486.")
MAN_END

MAN_BEGIN (U"Bartlett (1954)", U"djmw", 20011111)
NORMAL (U"M.S. Bartlett (1954): \"A note on multiplying factors for various "
	"chi-squared approximations.\", %%Joural of the Royal Statistical Society, "
	"Series B% #16: 296\\--298.")
MAN_END

MAN_BEGIN (U"Berry et al. (2007)", U"djmw", 20190321)
NORMAL (U"M.W. Berry, M. Browne, A.N. Langville, V.P. Pauca & R.J. Plemmons (2007): "
	"\"Algorithms and applications for approximate nonnegative matrix factorization.\", "
	"Computational Statistics & Data Analysis ##52#: 155\\--173.")
MAN_END

MAN_BEGIN (U"Blumensath & Davies (2010)", U"djmw", 20190601)
NORMAL (U"T. Blumensath & M.E. Davies: \"Normalised iterative hard thresholding;"
	" guaranteed stability and performance\", %%IEEE Journal of Selected Topics in Signal Processing% #4: 298\\--309.")
MAN_END

MAN_BEGIN (U"Boll (1979)", U"djmw", 20121021)
NORMAL (U"S.F. Boll (1979): \"Suppression of acoustic noise in speech using spectral subtraction.\""
	"%%IEEE Transactions on ASSP% #27: 113\\--120.")
MAN_END

MAN_BEGIN (U"Boomsma (1977)", U"djmw", 20020524)
NORMAL (U"A. Boomsma (1977): \"Comparing approximations of confidence intervals "
	"for the product-moment correlation coefficient.\" %%Statistica Neerlandica% "
	"#31: 179-186.")
MAN_END

MAN_BEGIN (U"Chan, Golub & LeVeque (1983)", U"djmw", 20170802)
NORMAL (U"T.F. Chan, G.H. Golub & R.J. LeVeque (1983): \"Algorithms for computing the sample variance: Analysis and recommendations.\" %%The American Statistician% #37: 242\\--247.")
MAN_END

MAN_BEGIN (U"Chan, Golub & LeVeque (1979)", U"djmw", 20170802)
NORMAL (U"T.F. Chan, G.H. Golub & R.J. LeVeque (1979): \"Updating formulae and an pairwise algorithm for computing sample variances.\" %%Stanford working paper STAN-CS-79-773%, 1\\--22.")
MAN_END

MAN_BEGIN (U"Cooley & Lohnes (1971)", U"djmw", 20060322)
NORMAL (U"W.W. Colley & P.R. Lohnes (1971): %%Multivariate data analysis%. "
	"John Wiley & Sons.")
MAN_END

MAN_BEGIN (U"Davis & Mermelstein (1980)", U"djmw", 20010419)
NORMAL (U"S.B. Davis & P. Mermelstein (1980), \"Comparison of parametric "
	"representations for monosyllabic word recognition in continuously spoken sentences.\" "
	"%%IEEE Transactions on ASSP% #28: 357\\--366.")
MAN_END

MAN_BEGIN (U"Deng & Tang (2011)", U"djmw", 20170915)
NORMAL (U"X. Deng & Z. Tang (2011). \"Moving surface spline interpolation based on Green's function\": "
	"%%Mathematical Geosciences% #43: 663\\--680.")
MAN_END

MAN_BEGIN (U"Efron & Tibshirani (1993)", U"djmw", 20031103)
NORMAL (U"B. Efron & R.J. Tibshirani (1993): %%An introduction "
	"to the bootstrap%. Chapman & Hall.")
MAN_END

MAN_BEGIN (U"eSpeak", U"David Weenink & Paul Boersma", 20211217) // 2024
NORMAL (U"eSpeak is a free text-to-speech synthesizer. It was developed by Jonathan Duddington until 2015. "
	"In 2015, Reece Dunn cloned it, and together with a group of developers they maintain and actualize it under the name “eSpeak NG”. "
	"eSpeak NG uses formant synthesis. "
	"On 2024-08-24, version 1.52-dev supports 137 languages with 104 voices, and is available in Praat (see @Acknowledgments).")
MAN_END

MAN_BEGIN (U"Févotte, Bertin & Durrieu (2009)", U"djmw", 20190618)
NORMAL (U"C. Févotte, N. Bertin & J.-L. Durrieu (2009): \"Nonnegative matrix factorization with the Itakura-Saito divergene: "
	"with applications to music analysis\", %%Neural Computation% #21: 793\\--830.")
MAN_END

MAN_BEGIN (U"Flanagan (1960)", U"djmw", 19980713)
NORMAL (U"J.L. Flanagan (1960): \"Models for approximating basilar membrane "
	"displacement.\" %%Bell System Technical Journal% #39: 1163\\--1191.")
MAN_END

MAN_BEGIN (U"Friedl (1997)", U"djmw", 20010710)
NORMAL (U"J.E.F. Friedl (1997): %%Mastering Regular Expressions%. "
	"O'Reilly & Associates.")
MAN_END

MAN_BEGIN (U"Ganong (1980)", U"djmw", 20130622)
NORMAL (U"W.F. Ganong III (1980): \"Phonetic categorization in auditory word perception.\" %%Journal of Experimental Psychology: Human Perception and Performance% #6: 110\\--125.") 
MAN_END


MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Garofolo, Lamel, Fisher, Fiscus, Pallett & Dahlgren (1993)"
© Paul Boersma 2025-04-26

John S. Garofolo, Lori F. Lamel, William M. Fisher, Jonathan G. Fiscus, David S. Pallett & Nancy L. Dahlgren (1993):
“DARPA TIMIT: acoustic-phonetic continuous speech corpus CD-ROM, NIST Speech Disc CD1-1.1”,
%NISTIR (%%National Institute of Standards and Technology Interagency or Internal Report%) 4930, Gaithersburg MD.
[https://doi.org/10.6028/NIST.IR.4930]

################################################################################
"Greiner & Hormann (1998)"
© David Weenink 2011-06-17

G. Greiner & K. Hormann (1998): “Efficient clipping of arbitrary polygons”, %%ACM Transactions on Graphics% #17: 71\--83.

################################################################################
"Heath et al. (1986)"
© David Weenink 1998-10-07

M.T. Heath, J.A. Laub, C.C. Paige & R.C. Ward (1986): “Computing the
singular value decomposition of a product of two matrices”,
%%SIAM Journal on Scientific and Statistical Computing% #7: 1147\--1159.

################################################################################
"Hastie, Tibshirani & Friedman (2001)"
© David Weenink 2022-01-11

T. Hastie, R. Tibshirani & J. Friedman (2001): %%The elements of statistical learning%. Springer Series in Statistics.

################################################################################
"Henrich et al. (2004)"
© David Weenink 2019-09-03

N. Henrich, C. d'Alessandro, B. Doval & M. Castellengo (2004):
“On the use of the derivative of electroglottographic signals for characterization of nonpathological phonation”,
%%Journal of the Acoustical Society of America% #115: 1321\--1332.

################################################################################
"Hermes (1988)"
© David Weenink 1998-01-23

Dik J. Hermes (1988): “Measurement of pitch by subharmonic summation”,
%%Journal of the Acoustical Society of America% #83: 257\--264.

################################################################################
"Henze & Wagner (1997)"
© David Weenink 2021-08-03

N. Henze & T. Wagner (1997): “A new approach to the BHEP tests for multivariate normality”,
%%Journal of Multivariate Analysis% #62: 1\--23.

################################################################################
"Herbst et al. (2014)"
© David Weenink 2019-08-29

C. Herbst, J. Lohscheller, J. \S<vec, N. Henrich, G. Weissengruber & W. Tecumseh Fitch (2014):
“Glottal opening and closing events investigated by electroglottography and super-high-speed video recordings”,
%%The Journal of Experimental Biology% #217: 955\--963.

################################################################################
"Herbst (2019)"
© David Weenink 2019-08-26

C. Herbst (2019): “Electroglottography \-- an update”, %%Journal of Voice%: In press.

################################################################################
"Holighaus et al. (2013)"
© David Weenink 2021-04-26

N. Holighaus, M. Dörfler, G. A. Velasco & T. Grill (2013): “A framework for invertible, real-time constant-Q transforms”,
%%IEEE Transactions on Audio, Speech, and Language Processing% #21: 775\--785.

################################################################################
"Hormann & Agathos (2001)"
© David Weenink 2011-06-17

K. Hormann & A. Agathos (2001): “The point in polygon problem for arbitrary polygons”,
%%Computational Geometry% #20: 131\--144.

################################################################################
"Irino & Patterson (1997)"
© David Weenink 2010-05-17

T. Irino & R.D. Patterson (1997): “A time-domain, level-dependent auditory filter: The gammachirp”,
%%Journal of the Acoustical Society of America% #101: 412\--419.

################################################################################
"Itakura & Saito (1968)"
© David Weenink 2019-06-17

F. Itakura & S. Saito (1968): “Analysis synthesis telephony based on the maximum likelihood method”,
In %%Proc. 6th International Congress on Acoustics%, Los Alamitos, CA: IEEE: C-17\--20.

################################################################################
"Janecek et al. (2011)"
© David Weenink 2019-03-12

A. Janecek, S. Schulze Grotthoff & W.N. Gangsterer (2011):
“LIBNMF \-- a library for nonnegative matrix factorization”, %%Computing and Informatics% #30: 205\--224.

################################################################################
"Johannesma (1972)"
© David Weenink 1998-01-23

Peter I.M. Johannesma (1972): “The pre-response stimulus ensemble of neurons in the cochlear nucleus.”
In %%Symposium on Hearing Theory% (IPO, Eindhoven, Netherlands), 58\--69.

################################################################################
"Johnson (1998)"
© David Weenink 2000-05-25

D.E. Johnson (1998): %%Applied multivariate methods%.

################################################################################
"Keating & Esposito (2006)"
© David Weenink 2013-06-20

P.A. Keating & C. Esposito (2006): “Linguistic voice quality”, %%UCLA Working Papers in Phonetics% #105: 85\--91.

################################################################################
"Khuri (1998)"
© David Weenink 2012-07-02

A. Khuri (1998): “Unweighted sums of squares in unbalanced analysis of variance”,
%%Journal of Statistical Planning and Inference% #74: 135\--147.

################################################################################
"Kim & Kim (2006)"
© David Weenink 2011-06-17

D.H. Kim & M.-J. Kim (2006): “An extension of polygon clipping to resolve degenerate cases”,
%%Computer-Aided Design & Applications% #3: 447\--456.

################################################################################
"Kostlan & Gokhman (1987)"
© David Weenink 2017-05-30

E. Kostlan & D. Gokhman (1987): “A program for calculating the incomplete gamma function”,
%%Technical Report, Dept. of Mathematics, Univ. of California%, Berkeley.

################################################################################
"Krishnamoorthy & Yu (2004)"
© David Weenink 2009-08-13

K. Krishnamoorthy & J. Yu (2004): “Modified Nel and Van der Merwe test for multivariate
Behrens-Fisher problem”, %%Statistics & Probability Letters% #66: 161\--169.

################################################################################
"Lamel, Kassel & Seneff (1986)"
© David Weenink 1998-01-23, Paul Boersma 2025

Lori F. Lamel, Robert H. Kassel & Stephanie Seneff (1986):
“Speech database development: Design and analysis of the acoustic-phonetic corpus.”
%%Proc. DARPA Speech Recognition Workshop%, Report No. SAIC-86/1546, 100\--119.

Reprinted in @@Garofolo, Lamel, Fisher, Fiscus, Pallett & Dahlgren (1993)@.

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Lee & Seung (2001)", U"djmw", 20190312)
NORMAL (U"D.D. Lee & S.H. Seung (2001): \"Algorithms for non-negative matrix factorization.\" "
	"%%Advances in in neural information processing systems% #13: 556\\--562.")
MAN_END

MAN_BEGIN (U"Ma & Nishihara (2013)", U"djmw", 20210315)
NORMAL (U"Y. Ma & A. Nishihara (2013): \"Efficient voice activity detection algorithm using long-term "
	"spectral flatness measure.\", %%EURASIP Journal on Audio, Speech, and Music Processing%, ##2013#:21")
MAN_END

MAN_BEGIN (U"Magron & Virtanen (2018)", U"djmw", 20191024)
NORMAL (U"P. Magron & T. Virtanen (2018): \"Expectation-maximization algorithms for Itakura-Saito "
	"nonnegative matrix factorization.\" HAL-01632082v2.")
MAN_END

MAN_BEGIN (U"Marsaglia & Tsang (2000)", U"djmw", 20190620)
NORMAL (U"G. Marsaglia & W.W. Tsang (2000): \"A simple method for generating gamma variables.\""
	" %%ACM Transactions on Mathematical Software% #26: 363\\--372.")
MAN_END

MAN_BEGIN (U"Matou\\s<ek (1991)", U"djmw", 20250119)
NORMAL (U"J. Matou\\s<ek (1991): \"Randomized optimal algorithm for slope selection.\""
" %%Information Processing Letters #39: 183\\--187.")
MAN_END

MAN_BEGIN (U"Morrison (1990)", U"djmw", 19980123)
NORMAL (U"D.F. Morrison (1990): %%Multivariate statistical methods%. "
	"New York: McGraw-Hill.")
MAN_END

MAN_BEGIN (U"Peterson & Barney (1952)", U"djmw", 20020620)
NORMAL (U"G.E. Peterson & H.L. Barney (1952): \"Control methods used in a study "
	"of the vowels.\" %%Journal of the Acoustical Society of America% #24: 175\\--184")
MAN_END

MAN_BEGIN (U"Pols et al. (1973)", U"djmw", 19990426)
NORMAL (U"L.C.W. Pols, H.R.C. Tromp & R. Plomp (1973): "
	"\"Frequency analysis of Dutch vowels from 50 male speakers.\" "
	"%%Journal of the Acoustical Society of America% #53: 1093\\--1101.")
MAN_END

MAN_BEGIN (U"Press et al. (1992)", U"djmw", 19980114)
NORMAL (U"W.H. Press, S.A. Teukolsky, W.T. Vetterling & B.P. Flannery (1992): "
	"%%Numerical recipes in C: The art of scientific computing%. "
	"Second Edition. Cambridge University Press.")
MAN_END

MAN_BEGIN (U"Sakoe & Chiba (1978)", U"djmw", 20050302)
NORMAL (U"H. Sakoe & S. Chiba (1978): \"Dynamic programming algorithm optimization for spoken word recognition.\" "
	"%%Transactions on ASSP% #26: 43\\--49.")
MAN_END

MAN_BEGIN (U"Sanchez (2012)", U"djmw", 20260219)
	NORMAL (U"C.A. Sanchez (2012): \"mtpsd documentation.\" "
	"%%https://github.com/cantonios/mtpsd/.")
MAN_END

MAN_BEGIN (U"Sandwell (1987)", U"djmw", 20170915)
NORMAL (U"D.T. Sandwell (1987): \"Biharmonic spline interpolation of GEOS-3 and SEASAT altimeter data.\", "
		"%%Geophysica Research Letters% #14: 139\\--142.")
MAN_END

MAN_BEGIN (U"Sekey & Hanson (1984)", U"djmw", 20050302)
NORMAL (U"A. Sekey & B.A. Hanson (1984): \"Improved 1-Bark bandwidth auditory filter.\" "
	"%%Journal of the Acoustical Society of America% #75: 1902\\--1904.")
MAN_END

MAN_BEGIN (U"Schott (2001)", U"djmw", 20090629)
NORMAL (U"J. R. Schott (2001): \"Some tests for the equality of covariance matrices.\" "
	"%%Journal of Statistical Planning and Inference% #94: 25\\-–36.")
MAN_END

MAN_BEGIN (U"Shepard (1964)", U"djmw", 19980114)
NORMAL (U"R.N. Shepard (1964): \"Circularity in judgments of relative pitch.\" "
	"%%Journal of the Acoustical Society of America% #36: 2346\\--2353.")
MAN_END

MAN_BEGIN (U"Skala (2005)", U"djmw", 20260819)
NORMAL (U"V. Skala (2005): \"A new approach to line and line segment clipping in homogeneous coordinates.\" "
	"%%Visual Comput.% #21: 905\\--914.")
MAN_END

MAN_BEGIN (U"Slaney (1993)", U"djmw", 19980712)
NORMAL (U"M. Slaney (1993): \"An efficient implementation of the "
	"Patterson-Holdsworth auditory filterbank.\" "
	"%%Apple Computer Technical Report% #35, 41 pages.")
MAN_END

MAN_BEGIN (U"Slapian (1977)", U"djmw", 20260219)
NORMAL (U"D. Slapian (1977): \"Program to compute discrete prolate spheroidal wave functions, "
	"sequences, and eigenvalues.\" "
	"%%Technical Report% TM 77-1218-8, Bell Telephone Laboratories.")
MAN_END

MAN_BEGIN (U"Ten Berge (1991)", U"djmw", 20191221)
NORMAL (U"J. Ten Berge (1991): \"A general solution for a class of weakly constrained linear regression problems.\", %%Psychometrika% #56, 601\\--609.")
MAN_END

MAN_BEGIN (U"Tribolet et al. (1979)", U"djmw", 20010114)
NORMAL (U"J.M. Tribolet & T.F. Quatieri (1979): \"Computation of the Complex "
	"Cepstrum.\" In %%Programs for Digital Signal Processing%, "
	"Digital Signal Processing Committee (eds.), IEEE Press.")
MAN_END

MAN_BEGIN (U"Tukey (1977)", U"djmw", 20000524)
NORMAL (U"J.W. Tukey (1977): %%Exploratory data analysis%. Reading, MA: Addison-Wesley.")
MAN_END

MAN_BEGIN (U"Van Nierop et al. (1973)", U"djmw", 20020620)
NORMAL (U"D.J.P.J. Van Nierop, L.C.W. Pols & R. Plomp (1973): \"Frequency "
	"analysis of Dutch vowels from 25 female speakers.\" %%Acustica% #29: 110\\--118")
MAN_END

MAN_BEGIN (U"Velasco et al. (2011)", U"djmw", 20210422)
NORMAL (U"G.A. Velasco, N. Holighaus, M. Dörfler & T. Grill (2011):\"Constructing an invertable constant-Q transform with "
	"nonstationary Gabor frames.\" %%Proc. of the 14^^th^ Int. Conference on Digital Audio Effects%, Paris, France, September 2011.")
MAN_END

MAN_BEGIN (U"Weenink (1985)", U"djmw", 20111010)
NORMAL (U"D.J.M. Weenink (1985), \"Formant analysis of Dutch vowels from 10 children\", "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #9, 45\\--52.")
MAN_END

MAN_BEGIN (U"Watrous (1991)", U"djmw", 20080125)
NORMAL (U"R.L. Watrous (1991): \"Current status of Peterson-Barney vowel formant data.\" "
	"%%Journal of the Acoustical Society of America% #89: 2459\\--2460.")
MAN_END

MAN_BEGIN (U"Weenink (1999)", U"djmw", 20041217)
NORMAL (U"D.J.M. Weenink (1999): \"Accurate algorithms for performing "
 		"principal component analysis and discriminant analysis.\" "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #23: 77\\--89.")
MAN_END

MAN_BEGIN (U"Weenink (2003)", U"djmw", 20040225)
NORMAL (U"D.J.M. Weenink (2003): \"Canonical correlation analysis.\" "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #25: 81\\--99.")
MAN_END

MAN_BEGIN (U"Wessel & Bercovici (1989)", U"djmw", 20170917)
NORMAL (U"P. Wessel & D. Bercovici (1998): \"Interpolation with splines in tension: a Green's function approach.\" "
	"%%Mathematical Geology% #30: 77\\--93.")
MAN_END

MAN_BEGIN (U"Wu et al. (2018)", U"djmw", 20210803)
NORMAL (U"L. Wu, C. Weng, X. Wang, K. Wang & X. Liu (2018): \"Test of covariance and correlation matrices\", "
	"arXiv: 1812.01172 [stat.ME]: 32 pp.")
MAN_END
}

/* End of file manual_dwtools.cpp */
