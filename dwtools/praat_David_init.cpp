/* praat_David_init.cpp
 *
 * Copyright (C) 1993-2026 David Weenink, 2015-2026 Paul Boersma
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
 djmw 20030701 Added Strings_setString.
 djmw 20031020 Changed Matrix_solveEquation.
 djmw 20031023 Added Spectra_multiply, Spectrum_conjugate and modified interface for CCA_TableOfReal_scores.
 djmw 20031030 Added TableOfReal_appendColumns.
 djmw 20031107 Added TablesOfReal_to_GSVD.
 djmw 20040303 Latest modification
 djmw 20040305 Added hints for PCA.
 djmw 20040323 Added hint for Discriminant.
 djmw 20040324 Added PCA_TableOfReal_getFractionVariance.
 djmw 20040331 Modified Eigen_drawEigenvalues interface.
 djmw 20040406 Extensive checks for creation of Sounds.
 djmw 20040414 Forms texts.
 djmw 20040523 Discriminant_TableOfReal_to_ClassificationTable: give new object a name.
 djmw 20040623 Added ClassificationTable_to_Strings_maximumProbability.
 djmw 20040704 BarkFilter... in Thing_recognizeClassesByName.
 djmw 20041020 MelderFile -> structMelderFile.
 djmw 20041105 TableOfReal_createFromVanNieropData_25females.
 djmw 20041108 FormantFilter_drawSpectrum bug corrected (wrong field name).
 djmw 20050308 Find path (slopes), Find path (band)... and others.
 djmw 20050404 TableOfReal_appendColumns -> TableOfReal_appendColumnsMany
 djmw 20050406 Procrustus -> Prorustes
 djmw 20050407 MelFilter_drawFilterFunctions error in field names crashed praat
 djmw 20050706 Eigen_getSumOfEigenvalues
 djmw 20051012 Robust LPC analysis test
 djmw 20051116 TableOfReal_drawScatterPlot horizontal and vertical axes indices should be positive numbers
 djmw SVD extract lef/right singular vectors
 djmw 20060111 TextGrid: Extend time moved from depth 1 to depth 2.
 djmw 20060308 Thing_recognizeClassesByName: StringsIndex, CCA
 djmw 20070206 Sound_changeGender: pitch range factor should be >= 0
 djmw 20070304 Latest modification.
 djmw 20070903 Melder_new<1...>
 djmw 20071011 REQUIRE requires U"".
 djmw 20071202 Melder_warning<n>
 djmw 20080521 Confusion_drawAsnumbers
 djmw 20090109 KlattGrid formulas for formant
 djmw 20090708 KlattTable <-> Table
 djmw 20090822 Thing_recognizeClassesByName: added classCepstrum, classIndex, classKlattTable
 djmw 20090914 Excitation to Excitations crashed because of nullptr reference
 djmw 20090927 TableOfReal_drawRow(s)asHistogram
 djmw 20091023 Sound_draw_selectedIntervals
 djmw 20091230 Covariance_TableOfReal_mahalanobis
 djmw 20100212 Standardize on Window length
 djmw 20100511 Categories_getNumberOfCategories
 djmw 20120813 Latest modification.
*/

#include "NUMcomplex.h"
#include "NUMmachar.h"

#include "ActivationList.h"
#include "AmplitudeTier.h"
#include "Categories.h"
#include "CategoriesEditor.h"
#include "ChebyshevSeries.h"
#include "ClassificationTable.h"
#include "Collection_extensions.h"
#include "ComplexSpectrogram.h"
#include "Confusion.h"
#include "Covariance.h"
#include "DataModeler.h"
#include "Discriminant.h"
#include "EditDistanceTable.h"
#include "Editor.h"
#include "EditDistanceTable.h"
#include "Eigen_and_Matrix.h"
#include "Eigen_and_Procrustes.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "Excitations.h"
#include "FileInMemory.h"
#include "FilterBank.h"
#include "Formula.h"
#include "FormantGridEditor.h"
#include "FormantGrid_extensions.h"
#include "Intensity_extensions.h"
#include "IntensityTierEditor.h"
#include "Matrix_Categories.h"
#include "Matrix_extensions.h"
#include "LongSound_extensions.h"
#include "KlattGridEditors.h"
#include "KlattTable.h"
#include "LegendreSeries.h"
#include "Ltas_extensions.h"
#include "Minimizers.h"
#include "NavigationContext.h"
#include "PatternList.h"
#include "PCA.h"
#include "PitchTierEditor.h"
#include "Polygon_extensions.h"
#include "Polynomial_to_Spectrum.h"
#include "Roots_to_Spectrum.h"
#include "Sound_and_Spectrum_dft.h"
#include "Sound_extensions.h"
#include "Sound_and_TextGrid_extensions.h"
#include "Sounds_to_DTW.h"
#include "Spectrum_extensions.h"
#include "Spectrogram.h"
#include "SpeechSynthesizer.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "Spline.h"
#include "Strings_extensions.h"
#include "SVD.h"
#include "Table_extensions.h"
#include "TableOfReal_and_Discriminant.h"
#include "TableOfReal_and_Permutation.h"
#include "TextGrid_extensions.h"
#include "TextGridTierNavigator.h"
#include "TextGridNavigator.h"
#include "Vector_extensions.h"

#include "Categories_and_Strings.h"
#include "CCA_and_Correlation.h"
#include "Cepstrum_and_Spectrum.h"
#include "CCs_to_DTW.h"
#include "Discriminant_PatternList_Categories.h"
#include "DTW_and_TextGrid.h"
#include "Matrix_and_NMF.h"
#include "Permutation_and_Index.h"
#include "Pitch_extensions.h"
#include "Sound_and_Spectrogram_extensions.h"
#include "Sound_to_Pitch2.h"
#include "Sound_to_SPINET.h"
#include "TableOfReal_and_SVD.h"
#include "TextGrid_and_DurationTier.h"
#include "TextGrid_and_PitchTier.h"
#include "VowelEditor.h"

#include "praat_TimeFrameSampled.h"
#include "praat_Matrix.h"
#include "praat_TableOfReal.h"
#include "praat_uvafon_init.h"

void praat_TableOfReal_init2 (ClassInfo klas);
void praat_SSCP_as_TableOfReal_init (ClassInfo klas);

void praat_CC_init (ClassInfo klas);
void praat_BandFilterSpectrogram_query_init (ClassInfo klas);
void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas);

#define DTW_constraints_addCommonFields(matchStart,matchEnd,slopeConstraint) \
	COMMENT (U"Boundary conditions") \
	BOOLEAN (matchStart, U"Match begin positions", false) \
	BOOLEAN (matchEnd, U"Match end positions", false) \
	CHOICE (slopeConstraint, U"Slope constraint", 1) \
		OPTION (U"no restriction") \
		OPTION (U"1/3 < slope < 3") \
		OPTION (U"1/2 < slope < 2") \
		OPTION (U"2/3 < slope < 3/2")

#undef INCLUDE_DTW_SLOPES

/********************** Activation *******************************************/

FORM (MODIFY_ActivationList_formula, U"ActivationList: Formula", nullptr) {
	COMMENT (U"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }}")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (ActivationList)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

DIRECT (CONVERT_EACH_TO_ONE__ActivationList_to_Matrix) {
	CONVERT_EACH_TO_ONE (ActivationList)
		autoMatrix result = ActivationList_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__ActivationList_to_PatternList) {
	CONVERT_EACH_TO_ONE (ActivationList)
		autoPatternList result = ActivationList_to_PatternList (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (QUERY_ONE_FOR_REAL__AmplitudeTier_getValueAtTime, U"AmplitudeTier: Get value at time", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	QUERY_ONE_FOR_REAL (AmplitudeTier)
		const double result = RealTier_getValueAtTime (me, time);
	QUERY_ONE_FOR_REAL_END (U" Hz")
}
	
FORM (QUERY_ONE_FOR_REAL__AmplitudeTier_getValueAtIndex, U"AmplitudeTier: Get value at index", nullptr) {
	INTEGER (pointNumber, U"Point number", U"10")
	OK
DO
	QUERY_ONE_FOR_REAL (AmplitudeTier)
		const double result = RealTier_getValueAtIndex (me, pointNumber);
	QUERY_ONE_FOR_REAL_END (U" Hz")
}


/********************** BarkFilter *******************************************/

DIRECT (HELP__BarkFilter_help) {
	HELP (U"BarkFilter")
}

DIRECT (HELP__BarkSpectrogram_help) {
	HELP (U"BarkSpectrogram")
}

FORM (GRAPHICS_EACH__BarkFilter_drawSpectrum, U"BarkFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (Bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Barks", garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__BarkFilter_drawSekeyHansonFilterFunctions, U"BarkFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	CHOICE (frequencyScale, U"Frequency scale", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (amplitudeScale_dB, U"Amplitude scale in dB", true)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		BarkFilter_drawSekeyHansonFilterFunctions (me, GRAPHICS, frequencyScale, fromFilter, toFilter, 
			fromFrequency, toFrequency, amplitudeScale_dB, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__BarkSpectrogram_drawSekeyHansonAuditoryFilters, U"BarkSpectrogram: Draw Sekey-Hanson auditory filters", U"BarkSpectrogram: Draw Sekey-Hanson auditory filters...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	CHOICE (frequencyScale, U"Frequency scale", 2)
		OPTION (U"Hertz")
		OPTION (U"Bark")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (amplitudeScale_dB, U"Amplitude scale in dB", true)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BarkSpectrogram)
		bool xIsHertz = (frequencyScale == 1);
		BarkSpectrogram_drawSekeyHansonFilterFunctions (me, GRAPHICS, xIsHertz, fromFilter, toFilter, 
			fromFrequency, toFrequency, amplitudeScale_dB, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__BarkFilter_paint, U"FilterBank: Paint", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (BarkFilter)
		FilterBank_paint (me, GRAPHICS, 
			fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__BarkFilter_to_BarkSpectrogram) {
	CONVERT_EACH_TO_ONE (BarkFilter)
		autoBarkSpectrogram result = BarkFilter_to_BarkSpectrogram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__MelFilter_to_MelSpectrogram) {
	CONVERT_EACH_TO_ONE (MelFilter)
		autoMelSpectrogram result = MelFilter_to_MelSpectrogram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__FormantFilter_to_Spectrogram) {
	CONVERT_EACH_TO_ONE (FormantFilter);
		autoSpectrogram result = FormantFilter_to_Spectrogram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************** Categories  ****************************************/

FORM (MODIFY_Categories_appendCategory, U"Categories: Append 1 category", U"Categories: Append 1 category...") {
	SENTENCE (category, U"Category", U"")
	OK
DO
	MODIFY_EACH (Categories)
		OrderedOfString_append (me, category);
	MODIFY_EACH_END
}

DIRECT (EDITOR_ONE_Categories_edit) {
	EDITOR_ONE (a,Categories)
		autoCategoriesEditor editor = CategoriesEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__Categories_getNumberOfCategories) {
	QUERY_ONE_FOR_INTEGER (Categories)
		const integer result = my size;
	QUERY_ONE_FOR_INTEGER_END (U" categories")
}

DIRECT (QUERY_TWO_FOR_REAL__Categories_getNumberOfDifferences) {
	QUERY_TWO_FOR_REAL (Categories)
		const integer result = OrderedOfString_getNumberOfDifferences (me, you);
	QUERY_TWO_FOR_REAL_END (U" differences")
}

DIRECT (QUERY_TWO_FOR_REAL__Categories_getFractionDifferent) {
	QUERY_TWO_FOR_REAL (Categories)
		const double result = OrderedOfString_getFractionDifferent (me, you);
	QUERY_TWO_FOR_REAL_END (U" (fraction different)")
}

DIRECT (CONVERT_EACH_TO_ONE__Categories_selectUniqueItems) {
	CONVERT_EACH_TO_ONE (Categories)
		autoCategories result = Categories_selectUniqueItems (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_uniq")
}

DIRECT (CONVERT_TWO_TO_ONE__Categories_to_Confusion) {
	CONVERT_TWO_TO_ONE (Categories)
		autoConfusion result = Categories_to_Confusion (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Categories_to_Strings) {
	CONVERT_EACH_TO_ONE (Categories)
		autoStrings result = Categories_to_Strings (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_TWO_TO_ONE__Categories_join) {
	CONVERT_TWO_TO_ONE (Categories)
		autoStringList result = OrderedOfString_joinItems (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Categories_permuteItems) {
	CONVERT_EACH_TO_ONE (Categories)
		autoCollection result = Collection_permuteItems (reinterpret_cast <Collection> (me));
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_perm")
}

DIRECT (MODIFY_EACH__Categories_permuteItems_inplace) {
	MODIFY_EACH (Categories)
		Collection_permuteItems_inplace (reinterpret_cast <Collection> (me));
	MODIFY_EACH_END
}

/***************** CC ****************************************/

FORM (QUERY_ONE_FOR_INTEGER__CC_getNumberOfCoefficients, U"Get number of coefficients", nullptr) {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (CC)
		const integer result = CC_getNumberOfCoefficients (me, frameNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of coefficients)")
}

FORM (QUERY_ONE_FOR_REAL__CC_getValue, U"CC: Get value", U"CC: Get value...") {
	REAL (time, U"Time (s)", U"0.1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (CC)
		const double result = CC_getValue (me, time, index);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__CC_getValueInFrame, U"CC: Get value in frame", U"CC: Get value in frame...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (CC)
		const double result = CC_getValueInFrame (me, frameNumber, index);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__CC_getC0ValueInFrame, U"CC: Get c0 value in frame", U"CC: Get c0 value in frame...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (CC)
		const double result = CC_getC0ValueInFrame (me, frameNumber);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (GRAPHICS_EACH__CC_paint, U"CC: Paint", U"CC: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	INTEGER (fromCoefficient, U"From coefficient", U"0")
	INTEGER (toCoefficient, U"To coefficient", U"0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CC)
		CC_paint (me, GRAPHICS, fromTime, toTime, fromCoefficient, toCoefficient, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__CC_drawC0, U"CC: Draw c0", U"CC: Draw c0...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CC)
		CC_drawC0 (me, GRAPHICS, fromTime, toTime,fromAmplitude, toAmplitude, garnish);
	GRAPHICS_EACH_END
}

FORM (CONVERT_TWO_TO_ONE__CCs_to_DTW, U"CC: To DTW", U"CC: To DTW...") {
	COMMENT (U"Distance  between cepstral coefficients")
	REAL (cepstralWeight, U"Cepstral weight", U"1.0")
	REAL (logEnergyWeight, U"Log energy weight", U"0.0")
	REAL (regressionWeight, U"Regression weight", U"0.0")
	REAL (regressionLogEnergyWeight, U"Regression log energy weight", U"0.0")
	REAL (regressionWindowLength, U"Regression window length (s)", U"0.056")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_TWO_TO_ONE (CC)
		autoDTW result = CCs_to_DTW (me, you, cepstralWeight, logEnergyWeight, regressionWeight, 
			regressionLogEnergyWeight, regressionWindowLength
		);
		DTW_findPath (result.get(), matchStart, matchEnd, slopeConstraint);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get());
}

DIRECT (CONVERT_EACH_TO_ONE__CC_to_Matrix) {
	CONVERT_EACH_TO_ONE (CC)
		autoMatrix result = CC_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/******************* class CCA ********************************/

DIRECT (HELP__CCA_help) {
	HELP (U"CCA")
}

FORM (GRAPHICS_EACH__CCA_drawEigenvector, U"CCA: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	INTEGER (eigenVectorNumber, U"Eigenvector number", U"1")
	COMMENT (U"Multiply by eigenvalue?")
	BOOLEAN (useComponentLoadings, U"Component loadings", false)
	COMMENT (U"Select part of the eigenvector:")
	INTEGER (fromElement, U"left Element range", U"0")
	INTEGER (toElement, U"right Element range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"-1.0")
	REAL (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (connectPoints, U"Connect points", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (CCA)
		CCA_drawEigenvector (me, GRAPHICS, xOrY, eigenVectorNumber, fromElement, toElement, 
			fromAmplitude, toAmplitude, useComponentLoadings, markSize_mm, mark_string, connectPoints, garnish
		);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__CCA_getNumberOfCorrelations) {
	QUERY_ONE_FOR_INTEGER (CCA)
		const integer result = my numberOfCoefficients;
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__CCA_getCorrelation, U"CCA: Get canonical correlation coefficient", U"CCA: Get canonical correlation coefficient") {
	NATURAL (coefficientNuber, U"Coefficient number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (CCA)
		const double result = CCA_getCorrelationCoefficient (me, coefficientNuber);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__CCA_getEigenvectorElement, U"CCA: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURAL (elementNumber, U"Element number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (CCA)
		const double result = CCA_getEigenvectorElement (me, xOrY, eigenvectorNumber, elementNumber);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__CCA_getZeroCorrelationProbability, U"CCA: Get zero correlation probability", U"CCA: Get zero correlation probability...") {
	NATURAL (coefficientNumber, U"Coefficient number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (CCA)
		double result, chisq, ndof;
		CCA_getZeroCorrelationProbability (me, coefficientNumber, & result, & chisq, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability for chisq = ", chisq, U" and ndf = ", ndof, U")");
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__CCA_Correlation_to_TableOfReal_loadings) {
	CONVERT_ONE_AND_ONE_TO_ONE (CCA, Correlation)
		autoTableOfReal result = CCA_Correlation_factorLoadings (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_loadings")
}

FORM (QUERY_ONE_AND_ONE_FOR_REAL__CCA_Correlation_getVarianceFraction, U"CCA & Correlation: Get variance fraction", U"CCA & Correlation: Get variance fraction...") {
	COMMENT (U"Get the fraction of variance from the data in set...")
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	COMMENT (U"extracted by...")
	NATURAL (fromCanonicalVariate, U"left Canonical variate range", U"1")
	NATURAL (toCanonicalVariate, U"right Canonical variate range", U"1")
	OK
DO
	QUERY_ONE_AND_ONE_FOR_REAL (CCA, Correlation)
		const double result = CCA_Correlation_getVarianceFraction (me, you, xOrY, fromCanonicalVariate, toCanonicalVariate);
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" (fraction variance from ", (xOrY == 1 ? U"y" : U"x"), 
		U", extracted by canonical variates ", fromCanonicalVariate, U" to ", toCanonicalVariate, U")")
}

FORM (QUERY_ONE_AND_ONE_FOR_REAL__CCA_Correlation_getRedundancy_sl, U"CCA & Correlation: Get Stewart-Love redundancy", U"CCA & Correlation: Get redundancy (sl)...") {
	COMMENT (U"Get the redundancy of the data in set...")
	OPTIONMENU (xOrY, U"X or Y", 1)
		OPTION (U"y")
		OPTION (U"x")
	COMMENT (U"extracted by...")
	NATURAL (fromCanonicalVariate, U"left Canonical variate range", U"1")
	NATURAL (toCanonicalVariate, U"right Canonical variate range", U"1")
	COMMENT (U"...given the availability of the data in the other set.")
	OK
DO
	QUERY_ONE_AND_ONE_FOR_REAL (CCA, Correlation)
		const double result = CCA_Correlation_getRedundancy_sl (me, you, xOrY, fromCanonicalVariate, toCanonicalVariate);
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" (redundancy from ", (xOrY == 1 ? U"y" : U"x"), 
		U" extracted by canonical variates ", fromCanonicalVariate, U" to ", toCanonicalVariate, U")")
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__CCA_TableOfReal_to_TableOfReal_loadings) {
	CONVERT_ONE_AND_ONE_TO_ONE (CCA, TableOfReal)
		autoTableOfReal result = CCA_TableOfReal_factorLoadings (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_loadings")
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__CCA_TableOfReal_to_TableOfReal_scores, U"CCA & TableOfReal: To TableOfReal (scores)", U"CCA & TableOfReal: To TableOfReal (scores)...") {
	INTEGER (numberOfCanonicalVariates, U"Number of canonical correlations", U"0 (= all)")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (CCA, TableOfReal)
		autoTableOfReal result = CCA_TableOfReal_scores (me, you, numberOfCanonicalVariates);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_scores");
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__CCA_TableOfReal_predict, U"CCA & TableOfReal: Predict", U"CCA & TableOfReal: Predict...") {
	COMMENT (U"The data set from which to predict starts at...")
	INTEGER (columnNumber, U"Column number", U"1")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (CCA, TableOfReal)
		autoTableOfReal result = CCA_TableOfReal_predict (me, you, columnNumber);
	CONVERT_ONE_AND_ONE_TO_ONE_END (your name.get(), U"_", my name.get())
}

FORM (CONVERT_EACH_TO_ONE__CCA_extractEigen, U"CCA: Extract Eigen", nullptr) {
	OPTIONMENU (choice, U"variablesType", 1)
		OPTION (U"Dependent")
		OPTION (U"Independent")
	OK
DO
	CONVERT_EACH_TO_ONE (CCA)
		autoEigen result = choice == 1 ? Data_copy (my y.get()) : Data_copy (my x.get());
	CONVERT_EACH_TO_ONE_END (my name.get(), ( choice == 1 ? U"_y" : U"_x" ))
}

/***************** ChebyshevSeries ****************************************/

DIRECT (HELP__ChebyshevSeries_help) {
	HELP (U"ChebyshevSeries")
}

FORM (CREATE_ONE__ChebyshevSeries_create, U"Create ChebyshevSeries", U"Create ChebyshevSeries...") {
	WORD (name, U"Name", U"cs")
	COMMENT (U"Domain")
	REAL (xmin, U"Xmin", U"-1")
	REAL (xmax, U"Xmax", U"1")
	COMMENT (U"ChebyshevSeries(x) = c[1] T[0](x) + c[2] T[1](x) + ... c[n+1] T[n](x)")
	COMMENT (U"T[k] is a Chebyshev polynomial of degree k")
	REALVECTOR (coefficients, U"Coefficients (c[k])", WHITESPACE_SEPARATED_, U"0 0 1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be smaller than Xmax.");
	CREATE_ONE
		autoChebyshevSeries result = ChebyshevSeries_createWithCoefficients (xmin, xmax, coefficients);
	CREATE_ONE_END (name)
}

DIRECT (CONVERT_EACH_TO_ONE__ChebyshevSeries_to_Polynomial) {
	CONVERT_EACH_TO_ONE (ChebyshevSeries)
		autoPolynomial result = ChebyshevSeries_to_Polynomial (me);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

/***************** ClassificationTable ****************************************/

DIRECT (HELP__ClassificationTable_help) {
	HELP (U"ClassificationTable")
}

FORM (QUERY_ONE_FOR_INTEGER__ClassificationTable_getClassIndexAtMaximumInRow, U"ClassificationTable: Get class index at maximum in row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (ClassificationTable)
		const integer result = TableOfReal_getColumnIndexAtMaximumInRow (me, rowNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (class index at maximum in row)")
}

FORM (QUERY_ONE_FOR_STRING__ClassificationTable_getClassLabelAtMaximumInRow, U"ClassificationTable: Get class label at maximum in row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (ClassificationTable)
		conststring32 result = TableOfReal_getColumnLabelAtMaximumInRow (me, rowNumber);
	QUERY_ONE_FOR_STRING_END
}

// deprecated 2014
DIRECT (CONVERT_EACH_TO_ONE__ClassificationTable_to_Confusion_old) {
	CONVERT_EACH_TO_ONE (ClassificationTable)
		autoConfusion result = ClassificationTable_to_Confusion (me, false);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__ClassificationTable_to_Confusion, U"ClassificationTable: To Confusion", U"ClassificationTable: To Confusion...") {
	BOOLEAN (onlyClassLabels, U"Only class labels", true)
	OK
DO
	CONVERT_EACH_TO_ONE (ClassificationTable)
		autoConfusion result = ClassificationTable_to_Confusion (me, onlyClassLabels);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__ClassificationTable_to_Correlation_columns) {
	CONVERT_EACH_TO_ONE (ClassificationTable)
		autoCorrelation result = ClassificationTable_to_Correlation_columns (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_col")
}

DIRECT (CONVERT_EACH_TO_ONE__ClassificationTable_to_Strings_maximumProbability) {
	CONVERT_EACH_TO_ONE (ClassificationTable)
		autoStrings result = ClassificationTable_to_Strings_maximumProbability (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************** Confusion *******************************************/

DIRECT (HELP__Confusion_help) {
	HELP (U"Confusion")
}

FORM (CREATE_ONE__Confusion_createSimple, U"Create simple Confusion", U"Create simple Confusion...") {
	WORD (name, U"Name", U"simple")
	SENTENCE (labels, U"Labels", U"u i a")
	OK
DO
	CREATE_ONE
		autoConfusion result = Confusion_createSimple (labels);
	CREATE_ONE_END (name)
}

FORM (MODIFY_Confusion_increase, U"Confusion: Increase", U"Confusion: Increase...") {
	WORD (stimulus, U"Stimulus", U"u")
	WORD (response, U"Response", U"i")
	OK
DO
	MODIFY_EACH (Confusion)
		Confusion_increase (me, stimulus, response);
	MODIFY_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__Confusion_getValue_labels, U"Confusion: Get value", nullptr) {
	WORD (stimulus, U"Stimulus", U"u")
	WORD (response, U"Response", U"i")
	OK
DO
	QUERY_ONE_FOR_REAL (Confusion)
		const double result = Confusion_getValue (me, stimulus, response);
	QUERY_ONE_FOR_REAL_END (U" ([\"", stimulus, U"\", \"",  response, U"\"])")
}

FORM (QUERY_ONE_FOR_REAL__Confusion_getResponseSum, U"Confusion: Get response sum", U"Confusion: Get response sum...") {
	WORD (response, U"Response", U"u")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const double result = TableOfReal_getColumnSumByLabel (me, response);
	QUERY_ONE_FOR_REAL_END (U" (response sum)")
}

FORM (QUERY_ONE_FOR_REAL__Confusion_getStimulusSum, U"Confusion: Get stimulus sum", U"Confusion: Get stimulus sum...") {
	WORD (stimulus, U"Stimulus", U"u")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const double result = TableOfReal_getRowSumByLabel (me, stimulus);
	QUERY_ONE_FOR_REAL_END (U" (stimulus sum)")
}

DIRECT (CONVERT_EACH_TO_ONE__Confusion_to_TableOfReal_marginals) {
	CONVERT_EACH_TO_ONE (Confusion)
		autoTableOfReal result = Confusion_to_TableOfReal_marginals (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_TWO_TO_ONE__Confusion_difference) {
	CONVERT_TWO_TO_ONE (Confusion)
		autoMatrix result = Confusion_difference (me, you);
	CONVERT_TWO_TO_ONE_END (U"diffs")
}

FORM (CONVERT_EACH_TO_ONE__Confusion_condense, U"Confusion: Condense", U"Confusion: Condense...") {
	SENTENCE (search_string, U"Search", U"^(u|i)$")
	SENTENCE (replace_string, U"Replace", U"high")
	INTEGER (replaceLimit, U"Replace limit", U"0 (= unlimited)")
	CHOICEx (matchType, U"Search and replace are", 2, 0)
		OPTION (U"literals")
		OPTION (U"Regular Expressions")
	OK
DO
	CONVERT_EACH_TO_ONE (Confusion)
		autoConfusion result = Confusion_condense (me, search_string, replace_string, replaceLimit, matchType == 2);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cnd")
}

FORM (CONVERT_EACH_TO_ONE__Confusion_group, U"Confusion: Group stimuli & responses", U"Confusion: Group...") {
	SENTENCE (labels, U"Stimuli & Responses", U"u i")
	SENTENCE (newLabel, U"New label", U"high")
	INTEGER (newPosition, U"New label position", U"0 (= at start)")
	OK
DO
	CONVERT_EACH_TO_ONE (Confusion)
		autoConfusion result = Confusion_group (me, labels, newLabel, newPosition);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_sr", newLabel)
}

FORM (CONVERT_EACH_TO_ONE__Confusion_groupStimuli, U"Confusion: Group stimuli", U"Confusion: Group stimuli...") {
	SENTENCE (stimuli_string, U"Stimuli", U"u i")
	SENTENCE (newLabel, U"New label", U"high")
	INTEGER (newPosition, U"New label position", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Confusion)
		autoConfusion result = Confusion_groupStimuli (me,stimuli_string, newLabel, newPosition);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_s", newLabel);
}

FORM (CONVERT_EACH_TO_ONE__Confusion_groupResponses, U"Confusion: Group responses", U"Confusion: Group responses...") {
	SENTENCE (responses_string, U"Responses", U"a i")
	SENTENCE (newLabel, U"New label", U"front")
	INTEGER (newPosition, U"New label position", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Confusion)
		autoConfusion result = Confusion_groupResponses (me, responses_string, newLabel, newPosition);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_s", newLabel);
}

FORM (GRAPHICS_EACH__Confusion_drawAsNumbers, U"Confusion: Draw as numbers", nullptr) {
	BOOLEAN (drawMarginals, U"Draw marginals", true)
	CHOICE (format, U"Format", 3)
		OPTION (U"decimal")
		OPTION (U"exponential")
		OPTION (U"free")
		OPTION (U"rational")
	NATURAL (precision, U"Precision", U"5")
	OK
DO
	GRAPHICS_EACH (Confusion)
		Confusion_drawAsNumbers (me, GRAPHICS, drawMarginals, format, precision);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_REAL__Confusion_getFractionCorrect) {
	QUERY_ONE_FOR_REAL (Confusion)
		double result;
		Confusion_getFractionCorrect (me, & result, nullptr);
	QUERY_ONE_FOR_REAL_END (U" (fraction correct)")
}

DIRECT (MODIFY_Confusion_ClassificationTable_increaseConfusionCount) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Confusion, ClassificationTable)
		Confusion_ClassificationTable_increase (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

/******************* Confusion & Matrix *************************************/

FORM (GRAPHICS_ONE_AND_ONE__Confusion_Matrix_draw, U"Confusion & Matrix: Draw confusions with arrows", nullptr) {
	INTEGER (categoryPosition, U"Category position", U"0 (= all)")
	REAL (lowerLevel, U"Lower level (%)", U"0")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	Melder_require (categoryPosition >= 0, U"Your category position should be at least 0.");
	GRAPHICS_ONE_AND_ONE (Confusion, Matrix)
		Confusion_Matrix_draw (me, you, GRAPHICS, categoryPosition, lowerLevel, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_ONE_AND_ONE_END
}

/********************** ComplexSpectrogram *******************************************/

DIRECT (HELP__ComplexSpectrogram_help) {
	HELP (U"ComplexSpectrogram_help")
}

FORM (CONVERT_EACH_TO_ONE__ComplexSpectrogram_to_Sound, U"ComplexSpectrogram: To Sound", nullptr) {
	POSITIVE (durationFactor, U"Duration factor", U"1.0")
	OK
DO
	CONVERT_EACH_TO_ONE (ComplexSpectrogram)
		autoSound result = ComplexSpectrogram_to_Sound (me, durationFactor);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__ComplexSpectrogram_downto_Spectrogram) {
	CONVERT_EACH_TO_ONE (ComplexSpectrogram)
		autoSpectrogram result = ComplexSpectrogram_to_Spectrogram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__ComplexSpectrogram_to_Spectrum_slice, U"ComplexSpectrogram: To Spectrum (slice)", nullptr) {
	REAL (time, U"Time (s)", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (ComplexSpectrogram)
		autoSpectrum result = ComplexSpectrogram_to_Spectrum (me, time);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (MODIFY_ComplexSpectrogram_Spectrogram_replaceAmplitudes) {
	MODIFY_FIRST_OF_ONE_AND_ONE (ComplexSpectrogram, Spectrogram)
		ComplexSpectrogram_Spectrogram_replaceAmplitudes (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

/********************** Correlation *******************************************/

FORM (CREATE_ONE__Correlation_createSimple, U"Create simple Correlation", U"Create simple Correlation...") {
	WORD (name, U"Name", U"correlation")
	REALVECTOR (correlations, U"Correlations", WHITESPACE_SEPARATED_, U"1.0 0.5 1.0")
	REALVECTOR (centroid, U"Centroid", WHITESPACE_SEPARATED_, U"0.0 0.0")
	NATURAL (numberOfObservations, U"Number of observations", U"100")
	OK
DO
	CREATE_ONE
		autoCorrelation result = Correlation_createSimple (correlations, centroid, numberOfObservations);
	CREATE_ONE_END (name)
}

DIRECT (HELP__Correlation_help) {
	HELP (U"Correlation")
}

FORM (CONVERT_EACH_TO_ONE__Correlation_confidenceIntervals, U"Correlation: Confidence intervals...", U"Correlation: Confidence intervals...") {
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	INTEGER (numberOfTests, U"Number of tests (Bonferroni correction)", U"0")
	CHOICE (approximation, U"Approximation", 1)
		OPTION (U"Ruben")
		OPTION (U"Fisher")
	OK
DO
	CONVERT_EACH_TO_ONE (Correlation)
		autoTableOfReal result = Correlation_confidenceIntervals (me, confidenceLevel, numberOfTests, approximation);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_conf_intervals")
}

FORM (QUERY_ONE_FOR_REAL__Correlation_getDiagonality_bartlett, U"Correlation: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...") {
	NATURAL (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Correlation)
		double chisq, result, ndof;
		Correlation_testDiagonality_bartlett (me, numberOfConstraints, & chisq, & result, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability, based on chisq = ", chisq, U" and ndf = ", ndof, U")")
}

DIRECT (CONVERT_EACH_TO_ONE__Correlation_to_PCA) {
	CONVERT_EACH_TO_ONE (Correlation)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************** Covariance *******************************************/

DIRECT (HELP__Covariance_help) {
	HELP (U"Covariance")
}

FORM (CREATE_ONE__Covariance_createSimple, U"Create simple Covariance", U"Create simple Covariance...") {
	WORD (name, U"Name", U"c")
	REALVECTOR (covariances, U"Covariances", WHITESPACE_SEPARATED_, U"1.0 0.0 1.0")
	REALVECTOR (centroid, U"Centroid", WHITESPACE_SEPARATED_, U"0.0 0.0")
	NATURAL (numberOfObservations, U"Number of observations", U"100")
	OK
DO
	CREATE_ONE
		autoCovariance result = Covariance_createSimple (covariances, centroid, numberOfObservations);
	CREATE_ONE_END (name)
}

FORM (QUERY_ONE_FOR_REAL__Covariance_getProbabilityAtPosition, U"Covariance: Get probability at position", nullptr) {
	SENTENCE (position_string, U"Position", U"10.0 20.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Covariance)
		const double result = Covariance_getProbabilityAtPosition_string (me, position_string);
	QUERY_ONE_FOR_REAL_END (U" (probability at position ", position_string, U")")
}

FORM (QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfOneMean, U"Covariance: Get significance of one mean", U"Covariance: Get significance of one mean...") {
	COMMENT (U"Get probability that the estimated mean for")
	NATURAL (index, U"Index", U"1")
	COMMENT (U"(or an estimated mean even further away)")
	COMMENT (U"could arise if the true mean were")
	REAL (value, U"Value", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Covariance)
		double result, t, ndof;
		Covariance_getSignificanceOfOneMean (me, index, value, & result, & t, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability, based on t = ", t, U" and ndf = ", ndof)
}

FORM (QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfMeansDifference, U"Covariance: Get significance of means difference", U"Covariance: Get significance of means difference...") {
	COMMENT (U"Get probability that the estimated difference between the means for")
	NATURAL (index1, U"Index1", U"1")
	NATURAL (index2, U"Index2", U"2")
	COMMENT (U"could arise if the true mean were")
	REAL (value, U"Value", U"0.0")
	COMMENT (U"Assume the means are ")
	BOOLEAN (paired, U"Paired", true)
	COMMENT (U"and have")
	BOOLEAN (equalVariances, U"Equal variances", true)
	OK
DO
	QUERY_ONE_FOR_REAL (Covariance)
		double result, t, ndof;
		Covariance_getSignificanceOfMeansDifference (me, index1, index2, value, paired, equalVariances, & result, & t, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability, based on t = ", t, U" and ndf = ", ndof, U")")
}

FORM (QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfOneVariance, U"Covariance: Get significance of one variance", U"Covariance: Get significance of one variance...") {
	COMMENT (U"Get the probability that the estimated variance for")
	NATURAL (index, U"Index", U"1")
	COMMENT (U"(or an even larger estimated variance)")
	COMMENT (U"could arise if the true variance were")
	REAL (value, U"Value", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Covariance)
		double result, chisq, ndof;
		Covariance_getSignificanceOfOneVariance (me, index, value, & result, & chisq, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability, based on chisq = ", chisq, U" and ndf = ", ndof, U")")
}

FORM (QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfVariancesRatio, U"Covariance: Get significance of variances ratio", nullptr) {
	COMMENT (U"Get the probability that the estimated variance ratio observed for")
	NATURAL (index1, U"Index1", U"1")
	NATURAL (index2, U"Index2", U"2")
	COMMENT (U"(or an estimated ratio even further away)")
	COMMENT (U"could arise if the true ratio were")
	REAL (value, U"Value", U"1.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Covariance)
		double result, f, ndof;
		Covariance_getSignificanceOfVariancesRatio (me, index1, index2, value, & result, & f , & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability, based on F = ", f, U" and ndf1 = ", ndof, U" and ndf2 = ", ndof, U")")
}

FORM (QUERY_ONE_FOR_REAL__Covariance_getFractionVariance, U"Covariance: Get fraction variance", U"Covariance: Get fraction variance...") {
	NATURAL (fromDimension, U"From dimension", U"1")
	NATURAL (toDimension, U"To dimension", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Covariance)
		const double result = SSCP_getFractionVariation (me, fromDimension, toDimension);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (INFO_TWO__Covariances_reportMultivariateMeanDifference, U"Covariances: Report multivariate mean difference", U"Covariances: Report multivariate mean difference...") {
	COMMENT (U"Get probability that the estimated multivariate means difference could arise ")
	COMMENT (U"if the actual means were equal.")
	COMMENT (U"")
	COMMENT (U"Assume for both means we have")
	BOOLEAN (covariancesAreEqual, U"Equal covariances", true)
	OK
DO
	INFO_TWO (Covariance)
		double prob, fisher, ndof1, ndof2, difference;
		MelderInfo_open ();
		difference = Covariances_getMultivariateCentroidDifference (me, you, covariancesAreEqual, & prob, & fisher, & ndof1, & ndof2);
		MelderInfo_writeLine (U"Under the assumption that the two covariances are", (covariancesAreEqual ? U" " : U" not "), U"equal:");
		MelderInfo_writeLine (U"Difference between multivariate means: ", difference);
		MelderInfo_writeLine (U"Fisher's F: ", fisher);
		MelderInfo_writeLine (U"Significance from zero: ", prob);
		MelderInfo_writeLine (U"Degrees of freedom 1: ", ndof1);
		MelderInfo_writeLine (U"Degrees of freedom 2: ", ndof2);
		MelderInfo_writeLine (U"Number of observations 1: ", me -> numberOfObservations);
		MelderInfo_writeLine (U"Number of observations 2: ", you -> numberOfObservations);
		MelderInfo_writeLine (U"Number of variables: ", me -> numberOfRows);
		MelderInfo_close ();
	INFO_TWO_END
}

FORM (CONVERT_EACH_TO_ONE__Covariance_to_TableOfReal_randomSampling, U"Covariance: To TableOfReal (random sampling)", U"Covariance: To TableOfReal (random sampling)...") {
	INTEGER (numberOfDataPoints, U"Number of data points", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Covariance)
		autoTableOfReal result = Covariance_to_TableOfReal_randomSampling (me, numberOfDataPoints);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (INFO_NONE__Covariances_reportEquality) { //TODO 20210510 is this ok
	INFO_NONE
		autoCovarianceList covariances = CovarianceList_create ();
		LOOP {
			iam_LOOP (Covariance);
			covariances -> addItem_ref (me);
		}
		MelderInfo_open ();
		double p, chisq, ndof;
		Covariances_equality (covariances.get(), 1, & p, & chisq, & ndof);
		MelderInfo_writeLine (U"Difference between covariance matrices:");
		MelderInfo_writeLine (U"Significance of difference (bartlett) = ", p);
		MelderInfo_writeLine (U"Chi-squared (bartlett) = ", chisq);
		MelderInfo_writeLine (U"Degrees of freedom (bartlett) = ", ndof);

		Covariances_equality (covariances.get(), 2, & p, & chisq, & ndof);
		MelderInfo_writeLine (U"Significance of difference (wald) = ", p);
		MelderInfo_writeLine (U"Chi-squared (wald) = ", chisq);
		MelderInfo_writeLine (U"Degrees of freedom (wald) = ", ndof);
		MelderInfo_close ();
	INFO_NONE_END
}

DIRECT (CONVERT_EACH_TO_ONE__Covariance_to_Correlation) {
	CONVERT_EACH_TO_ONE (Covariance)
		autoCorrelation result = SSCP_to_Correlation (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Covariance_to_PCA) {
	CONVERT_EACH_TO_ONE (Covariance)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (COMBINE_ALL_LISTED_TO_ONE__Covariances_pool) {
	COMBINE_ALL_LISTED_TO_ONE (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_pool (list.get());
	COMBINE_ALL_LISTED_TO_ONE_END (U"pool")
}

DIRECT (COMBINE_ALL_LISTED_TO_ONE__Covariances_to_Covariance_between) {
	COMBINE_ALL_LISTED_TO_ONE (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_between (list.get());
	COMBINE_ALL_LISTED_TO_ONE_END (U"between")
}

DIRECT (COMBINE_ALL_LISTED_TO_ONE__Covariances_to_Covariance_within) {
	COMBINE_ALL_LISTED_TO_ONE (Covariance, CovarianceList)
		autoCovariance result = CovarianceList_to_Covariance_within (list.get());
	COMBINE_ALL_LISTED_TO_ONE_END (U"within")
}

DIRECT (CREATE_ONE__Covariances_to_CovarianceList) {
	autoCovarianceList result = CovarianceList_create ();
	CREATE_ONE
		LOOP {
			iam_LOOP (Covariance);
			autoCovariance cov = Data_copy (me);
			result -> addItem_move (cov.move());
		}
	CREATE_ONE_END (U"List_of_", Melder_integer (result->size))
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Covariance_TableOfReal_mahalanobis, U"Covariance & TableOfReal: To TableOfReal (mahalanobis)", U"Covariance & TableOfReal: To TableOfReal (mahalanobis)...") {
	BOOLEAN (centroidFromTable, U"Centroid from table", false)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Covariance, TableOfReal)
		autoTableOfReal result = Covariance_TableOfReal_mahalanobis (me, you, centroidFromTable);
	CONVERT_ONE_AND_ONE_TO_ONE_END (U"mahalanobis")
}

/********************** Discriminant **********************************/

DIRECT (HELP__Discriminant_help) {
	HELP (U"Discriminant")
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__Discriminant_setGroupLabels) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Discriminant, Strings)
		Discriminant_setGroupLabels (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_PatternList_to_Categories, U"Discriminant & PatternList: To Categories", U"Discriminant & PatternList: To Categories...") {
	BOOLEAN (poolCovariances, U"Pool covariance matrices", true)
	BOOLEAN (useAPrioriProbabilities, U"Use apriori probabilities", true)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Discriminant, PatternList)
		autoCategories result = Discriminant_PatternList_to_Categories (me, you, poolCovariances, useAPrioriProbabilities);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_to_Configuration, U"Discriminant & TableOfReal: To Configuration", U"Discriminant & TableOfReal: To Configuration...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (=all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, 
		U"\"Number of dimensions\" should not be less than zero.");
	CONVERT_ONE_AND_ONE_TO_ONE (Discriminant, TableOfReal)
		autoConfiguration result = Discriminant_TableOfReal_to_Configuration (me, you, numberOfDimensions);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (HINT__Discriminant_TableOfReal_to_ClassificationTable) {
	INFO_NONE
		Melder_information (U"You can use the Discriminant as a classifier by \nselecting a Discriminant and a TableOfReal object together.");
	INFO_NONE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_to_ClassificationTable, U"Discriminant & TableOfReal: To ClassificationTable", U"Discriminant & TableOfReal: To ClassificationTable...") {
	BOOLEAN (poolCovariances, U"Pool covariance matrices", true)
	BOOLEAN (useAPrioriProbabilities, U"Use apriori probabilities", true)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Discriminant, TableOfReal)
		autoClassificationTable result = Discriminant_TableOfReal_to_ClassificationTable (me, you, 
			poolCovariances, useAPrioriProbabilities
		);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_mahalanobis, U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)", U"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...") {
	SENTENCE (groupLabel, U"Group label", U"")
	BOOLEAN (poolCovariances, U"Pool covariance matrices", false)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Discriminant, TableOfReal)
		const integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, 
			U"Your group label \"", groupLabel, U"\" does not exist.");
		autoTableOfReal result = Discriminant_TableOfReal_mahalanobis (me, you, group, poolCovariances);
	CONVERT_ONE_AND_ONE_TO_ONE_END (U"mahalanobis")
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_mahalanobis_all, U"Discriminant & TableOfReal: Mahalanobis all", nullptr) {
	BOOLEAN (poolCovariances, U"Pool covariance matrices", false)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Discriminant, TableOfReal)
		autoTableOfReal result = Discriminant_TableOfReal_mahalanobis_all (me, you, poolCovariances);
	CONVERT_ONE_AND_ONE_TO_ONE_END (U"mahalanobis")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfEigenvalues) {
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = my eigen -> numberOfEigenvalues;
	QUERY_ONE_FOR_INTEGER_END (U" (number of eigenvalues)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfEigenvectors) {
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = my eigen -> numberOfEigenvalues;
	QUERY_ONE_FOR_INTEGER_END (U" (number of eigenvectors)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Discriminant_getEigenvectorDimension) {
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = my eigen -> dimension;
	QUERY_ONE_FOR_INTEGER_END (U" (dimension)")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getEigenvalue, U"Discriminant: Get eigenvalue", U"Eigen: Get eigenvalue...") {
	NATURAL (eigenvalueNumber, U"Eigenvalue number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		Melder_require (eigenvalueNumber <= my eigen -> numberOfEigenvalues, 
			U"Eigenvalue number should be smaller than ", my eigen -> numberOfEigenvalues + 1);
		const double result = my eigen -> eigenvalues [eigenvalueNumber];
	QUERY_ONE_FOR_REAL_END (U" (eigenvalue [", eigenvalueNumber, U"])")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getSumOfEigenvalues, U"Discriminant:Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range",  U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const double result = Eigen_getSumOfEigenvalues (my eigen.get(), fromEigenvalue, toEigenvalue);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getEigenvectorElement, U"Discriminant: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURAL (elementNumber, U"Element number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const double result = Eigen_getEigenvectorElement (my eigen.get(), eigenvectorNumber, elementNumber);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getWilksLambda, U"Discriminant: Get Wilks' lambda", U"Discriminant: Get Wilks' lambda...") {
	COMMENT (U"Product (i=from..numberOfEigenvalues, 1 / (1 + eigenvalue[i]))")
	NATURAL (from, U"From", U"1") //TODO better name
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const double result = Discriminant_getWilksLambda (me, from);
	QUERY_ONE_FOR_REAL_END (U" (wilks lambda)")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getCumulativeContributionOfComponents, U"Discriminant: Get cumulative contribution of components", U"Eigen: Get cumulative contribution of components...") {
	NATURAL (fromComponent, U"From component", U"1")
	NATURAL (toComponent, U"To component", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const double result = Eigen_getCumulativeContributionOfComponents (my eigen.get(), fromComponent, toComponent);
	QUERY_ONE_FOR_REAL_END (U" (cumulative contribution)")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getPartialDiscriminationProbability, U"Discriminant: Get partial discrimination probability", U"Discriminant: Get partial discrimination probability...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"1")
	OK
DO
	Melder_require (numberOfDimensions >= 0, 
		U"The number of dimensions should be at least zero.");
	QUERY_ONE_FOR_REAL (Discriminant)
		double result, chisq, ndof;
		Discriminant_getPartialDiscriminationProbability (me, numberOfDimensions, & result, & chisq, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", ndof, U")");
}

DIRECT (QUERY_ONE_FOR_REAL__Discriminant_getHomegeneityOfCovariances_box) {
	QUERY_ONE_FOR_REAL (Discriminant)
		double chisq, result, ndof;
		SSCPList_getHomegeneityOfCovariances_box (my groups.get(), & result, & chisq, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (= probability, based on chisq = ", chisq, U" and ndf = ", ndof, U")")
}

DIRECT (INFO_ONE__Discriminant_reportEqualityOfCovarianceMatrices) {
	INFO_ONE (Discriminant)
		MelderInfo_open ();
		structCovarianceList list;
		for (integer i = 1; i <= my groups->size; i ++) {
			SSCP sscp = my groups->at [i];
			autoCovariance covariance = SSCP_to_Covariance (sscp, 1);   // FIXME numberOfConstraints shouldn't be 1, should it?
			list. addItem_move (covariance.move());
		}
		double chisq, prob, ndof;
		Covariances_equality (& list, 2, & prob, & chisq, & ndof);
		MelderInfo_writeLine (U"Wald test for equality of covariance matrices:");
		MelderInfo_writeLine (U"Chi squared: ", chisq);
		MelderInfo_writeLine (U"Significance: ", prob);
		MelderInfo_writeLine (U"Degrees of freedom: ", ndof);
		MelderInfo_writeLine (U"Number of matrices: ", list.size);
		MelderInfo_close ();
	INFO_NONE_END
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getSigmaEllipseArea, U"Discriminant: Get concentration ellipse area", U"Discriminant: Get concentration ellipse area...") {
	SENTENCE (groupLabel, U"Group label", U"")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, 
			U"The group label \"", groupLabel, U"\" does not exist.");
		const double result = Discriminant_getConcentrationEllipseArea (me, group, numberOfSigmas, false, 
			discriminatPlane, xDimension, yDimension
		);
	QUERY_ONE_FOR_REAL_END (U" (concentration ellipse area)")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getConfidenceEllipseArea, U"Discriminant: Get confidence ellipse area", U"Discriminant: Get confidence ellipse area...") {
	SENTENCE (groupLabel, U"Group label", U"")
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEAN (discriminatPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0,
			U"The group label \"", groupLabel, U"\" does not exist.");
		const double result = Discriminant_getConcentrationEllipseArea (me, group, confidenceLevel, true,
			discriminatPlane, xDimension, yDimension
		);
	QUERY_ONE_FOR_REAL_END (U" (confidence ellipse area)")
}

FORM (QUERY_ONE_FOR_REAL__Discriminant_getLnDeterminant_group, U"Discriminant: Get determinant (group)", U"Discriminant: Get determinant (group)...")
	SENTENCE (groupLabel, U"Group label", U"") {
	OK
DO
	QUERY_ONE_FOR_REAL (Discriminant)
		const integer group = Discriminant_groupLabelToIndex (me, groupLabel);
		Melder_require (group > 0, 
			U"The group label \"", groupLabel, U"\" does not exist.");
		const double result = Discriminant_getLnDeterminant_group (me, group);
	QUERY_ONE_FOR_REAL_END (U" (ln(determinant) group")
}

DIRECT (QUERY_ONE_FOR_REAL__Discriminant_getLnDeterminant_total) {
	QUERY_ONE_FOR_REAL (Discriminant)
		const double result = Discriminant_getLnDeterminant_total (me);
	QUERY_ONE_FOR_REAL_END (U" (ln(determinant) total")
}

FORM (MODIFY_Discriminant_invertEigenvector, U"Discriminant: Invert eigenvector", nullptr) {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Discriminant)
		Eigen_invertEigenvector (my eigen.get(), eigenvectorNumber);
	MODIFY_EACH_END
}

FORM (GRAPHICS_EACH__Discriminant_drawEigenvalues, U"Discriminant: Draw eigenvalues", U"Eigen: Draw eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (showFractions, U"Fraction of eigenvalues summed", false)
	BOOLEAN (showCumulativeValues, U"Cumulative", false)
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Eigen_drawEigenvalues (my eigen.get(), GRAPHICS, fromEigenvalue, toEigenvalue, fromAmplitude, toAmplitude, 
			showFractions, showCumulativeValues, markSize_mm, mark_string, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Discriminant_drawEigenvector, U"Discriminant: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	INTEGER (eigenvectorNumber, U"Eigenvector number", U"1")
	BOOLEAN (componentLoadings, U"Component loadings", false)
	INTEGER (fromElement, U"left Element range", U"0")
	INTEGER (toElement, U"right Element range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"-1.0")
	REAL (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (connectPoints, U"Connect points", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Eigen_drawEigenvector (my eigen.get(), GRAPHICS, eigenvectorNumber, fromElement, toElement, 
			fromAmplitude, toAmplitude, componentLoadings, markSize_mm, mark_string, connectPoints,
			nullptr,  garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Discriminant_drawSigmaEllipses, U"Discriminant: Draw sigma ellipses", U"Discriminant: Draw sigma ellipses...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (discriminantPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false, nullptr, discriminantPlane, 
			xDimension, yDimension,xmin, xmax, ymin, ymax, labelSize, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Discriminant_drawOneSigmaEllipse, U"Discriminant: Draw one sigma ellipse", U"Discriminant: Draw one sigma ellipse...") {
	SENTENCE (label, U"Label", U"")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	BOOLEAN (discriminantPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false, label, discriminantPlane,
			xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Discriminant_drawConfidenceEllipses, U"Discriminant: Draw confidence ellipses", nullptr) {
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEAN (discriminantPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, nullptr, discriminantPlane,
			xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Discriminant_drawOneConfidenceEllipse, U"Discriminant: Draw one confidence ellipse", nullptr) {
	SENTENCE (label, U"Label", U"")
	POSITIVE (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	BOOLEAN (discriminantPlane, U"Discriminant plane", true)
	INTEGER (xDimension, U"X-dimension", U"1")
	INTEGER (yDimension, U"Y-dimension", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (labelSize, U"Label size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Discriminant)
		Discriminant_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, label, discriminantPlane, 
			xDimension, yDimension, xmin, xmax, ymin, ymax, labelSize, garnish
		);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__Discriminant_extractBetweenGroupsSSCP) {
	CONVERT_EACH_TO_ONE (Discriminant)
		autoSSCP result = Discriminant_extractBetweenGroupsSSCP (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_between")
}

DIRECT (CONVERT_EACH_TO_ONE__Discriminant_extractGroupCentroids) {
	CONVERT_EACH_TO_ONE (Discriminant)
		autoTableOfReal result = Discriminant_extractGroupCentroids (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_centroids")
}

DIRECT (CONVERT_EACH_TO_ONE__Discriminant_extractGroupStandardDeviations) {
	CONVERT_EACH_TO_ONE (Discriminant)
		autoTableOfReal result = Discriminant_extractGroupStandardDeviations (me);
	CONVERT_EACH_TO_ONE_END (U"group_stddevs")
}

DIRECT (CONVERT_EACH_TO_ONE__Discriminant_extractGroupLabels) {
	CONVERT_EACH_TO_ONE (Discriminant)
		autoStrings result = Discriminant_extractGroupLabels (me);
	CONVERT_EACH_TO_ONE_END (U"group_labels")
}

DIRECT (CONVERT_EACH_TO_ONE__Discriminant_extractEigen) {
	CONVERT_EACH_TO_ONE (Discriminant)
		autoEigen result = Data_copy (my eigen.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Discriminant_extractPooledWithinGroupsSSCP) {
	CONVERT_EACH_TO_ONE (Discriminant)
		autoSSCP result = Discriminant_extractPooledWithinGroupsSSCP (me);
	CONVERT_EACH_TO_ONE_END (U"pooled_within")
}

FORM (CONVERT_EACH_TO_ONE__Discriminant_extractWithinGroupSSCP, U"Discriminant: Extract within-group SSCP", U"Discriminant: Extract within-group SSCP...") {
	NATURAL (groupIndex, U"Group index", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Discriminant)
		autoSSCP result = Discriminant_extractWithinGroupSSCP (me, groupIndex);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_g", groupIndex)
}

DIRECT (QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfFunctions) {
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = Discriminant_getNumberOfFunctions (me);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Discriminant_getDimensionOfFunctions) {
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = Eigen_getDimensionOfComponents (my eigen.get());
	QUERY_ONE_FOR_INTEGER_END (U"")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfGroups) {
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = Discriminant_getNumberOfGroups (me);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfObservations, U"Discriminant: Get number of observations", U"Discriminant: Get number of observations...") {
	INTEGER (group, U"Group", U"0 (= total)")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Discriminant)
		const integer result = Discriminant_getNumberOfObservations (me, group);
	QUERY_ONE_FOR_INTEGER_END (U"")
}


/********************** DTW *******************************************/

FORM (MODIFY_DTW_Polygon_findPathInside, U"DTW & Polygon: Find path inside", nullptr) {
    CHOICE (slopeConstraint, U"Slope constraint", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
    OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (DTW, Polygon)
		DTW_Polygon_findPathInside (me, you, slopeConstraint, nullptr);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__DTW_Polygon_to_Matrix_cumulativeDistances, U"DTW & Polygon: To Matrix (cum. distances)", nullptr) {
    CHOICE (slopeConstraint, U"Slope constraint", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_ONE_AND_ONE_TO_ONE (DTW, Polygon)
		autoMatrix result = DTW_Polygon_to_Matrix_cumulativeDistances (me, you, slopeConstraint);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", slopeConstraint);
}

FORM (GRAPHICS_TWO_AND_ONE__DTW_Sounds_draw, U"DTW & Sounds: Draw", U"DTW & Sounds: Draw...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO_AND_ONE (Sound, DTW)
		DTW_Sounds_draw (him, you, me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_TWO_AND_ONE_END
}

FORM (GRAPHICS_TWO_AND_ONE__DTW_Sounds_drawWarp_x, U"DTW & Sounds: Draw warp (x)", U"DTW & Sounds: Draw warp (x)...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (time, U"Time (s)", U"0.1")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO_AND_ONE (Sound, DTW)
		DTW_Sounds_drawWarpX (him, you, me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_TWO_AND_ONE_END
}

DIRECT (HELP__DTW_help) {
	HELP (U"DTW")
}

FORM (GRAPHICS_EACH__DTW_drawPath, U"DTW: Draw path", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawPath (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__DTW_drawDistancesAlongPath, U"DTW: Draw distances along path", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawDistancesAlongPath (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__DTW_paintDistances, U"DTW: Paint distances", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_paintDistances (me, GRAPHICS, xmin, xmax, ymin, ymax, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__DTW_drawWarp_x, U"DTW: Draw warp (x)", U"DTW: Draw warp (x)...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (time, U"Time (s)", U"0.1")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawWarpX (me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_EACH__DTW_drawWarp_y, U"DTW: Draw warp (y)", U"DTW: Draw warp (y)...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (time, U"Time (s)", U"0.1")
	BOOLEAN (garnish, U"Garnish", false);
	OK
DO
	GRAPHICS_EACH (DTW)
		DTW_drawWarpY (me, GRAPHICS, xmin, xmax, ymin, ymax, time, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getStartTime_x) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my xmin;
	QUERY_ONE_FOR_REAL_END (U" seconds (start time along x)")
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getEndTime_x) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my xmax;
	QUERY_ONE_FOR_REAL_END (U" seconds (end time along x)");
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getTotalDuration_x) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my xmax - my xmin;
	QUERY_ONE_FOR_REAL_END (U" seconds (total duration along x)");
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getStartTime_y) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my ymin;
	QUERY_ONE_FOR_REAL_END (U" seconds (start time along y)");
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getEndTime_y) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my ymax;
	QUERY_ONE_FOR_REAL_END (U" seconds (end time along y)");
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getTotalDuration_y) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my ymax - my ymin;
	QUERY_ONE_FOR_REAL_END (U" seconds (total duration along y)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__DTW_getNumberOfFrames_x) {
	QUERY_ONE_FOR_INTEGER (DTW)
		const integer result = my nx;
	QUERY_ONE_FOR_INTEGER_END (U" frames along x")
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getTimeStep_x) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" seconds (time step along x)")
}

FORM (QUERY_ONE_FOR_REAL__DTW_getTimeFromFrameNumber_x, U"DTW: Get time from frame number (x)", nullptr) {
	NATURAL (frameNumber, U"Frame number (x)", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DTW)
		const double result = Matrix_columnToX (me, frameNumber);
	QUERY_ONE_FOR_REAL_END (U" seconds (y time at x frame ", frameNumber, U")")
}

FORM (QUERY_ONE_FOR_INTEGER__DTW_getFrameNumberFromTime_x, U"DTW: Get frame number from time (x)", nullptr) {
	REAL (xTime, U"Time along x (s)", U"0.1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (DTW)
		Melder_require (xTime >= my xmin && xTime <= my xmax, 
			U"Time outside x domain.");
		const integer result = Melder_iround (Matrix_xToColumn (me, xTime));
	QUERY_ONE_FOR_INTEGER_END (U" (x frame at y time ", xTime, U")")
}

DIRECT (QUERY_ONE_FOR_INTEGER__DTW_getNumberOfFrames_y) {
	QUERY_ONE_FOR_INTEGER (DTW)
		const integer result = my ny;
	QUERY_ONE_FOR_INTEGER_END (U" (number of frames along y)")
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getTimeStep_y) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my dy;
	QUERY_ONE_FOR_REAL_END (U" seconds (time step along y)")
}


FORM (QUERY_ONE_FOR_REAL__DTW_getTimeFromFrameNumber_y, U"DTW: Get time from frame number (y)", nullptr) {
	NATURAL (frameNumber, U"Frame number (y)", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (DTW)
		const double result = Matrix_rowToY (me, frameNumber);
	QUERY_ONE_FOR_REAL_END (U" seconds (x time at y frame ", frameNumber, U")")
}

FORM (QUERY_ONE_FOR_INTEGER__DTW_getFrameNumberFromTime_y, U"DTW: Get frame number from time (y)", nullptr) {
	REAL (yTime, U"Time along y (s)", U"0.1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (DTW)
		Melder_require (yTime >= my ymin && yTime <= my ymax, 
			U"Time outside y domain.");
		const integer result = Melder_iround (Matrix_yToRow (me, yTime));
	QUERY_ONE_FOR_INTEGER_END (U" (y frame at x time ", yTime, U")")
}

FORM (QUERY_ONE_FOR_REAL__DTW_getPathY, U"DTW: Get time along path", U"DTW: Get time along path...") {
	REAL (xTime, U"Time (s)", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (DTW)
		const double result = DTW_getYTimeFromXTime (me, xTime);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__DTW_getYTimeFromXTime, U"DTW: Get y time from x time", U"DTW: Get y time from x time...") {
	REAL (xTime, U"Time at x (s)", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (DTW)
		const double result = DTW_getYTimeFromXTime (me, xTime);
	QUERY_ONE_FOR_REAL_END (U" seconds (= y time at x time ", xTime, U")")
}

FORM (QUERY_ONE_FOR_REAL__DTW_getXTimeFromYTime, U"DTW: Get x time from y time", U"DTW: Get x time from y time...") {
	REAL (yTime, U"Time at y (s)", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (DTW)
		const double result = DTW_getXTimeFromYTime (me, yTime);
	QUERY_ONE_FOR_REAL_END (U" seconds (= x time at y time ", yTime, U")")
}

FORM (QUERY_ONE_FOR_INTEGER__DTW_getMaximumConsecutiveSteps, U"DTW: Get maximum consecutive steps", U"DTW: Get maximum consecutive steps...") {
	OPTIONMENU (direction, U"Direction", 1)
		OPTION (U"X")
		OPTION (U"Y")
		OPTION (U"Diagonal")
	OK
DO
	int direction_code [] = { DTW_START, DTW_X, DTW_Y, DTW_XANDY };
	conststring32 direction_string [] = { U"", U"x", U"y", U"diagonal" };
	QUERY_ONE_FOR_INTEGER (DTW)
		integer result = DTW_getMaximumConsecutiveSteps (me, direction_code [direction]);
	QUERY_ONE_FOR_INTEGER_END (U" (maximum number of consecutive steps in ", direction_string [direction], U" direction)")
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getDistance_weighted) {
	QUERY_ONE_FOR_REAL (DTW)
		const double result = my weightedDistance;
	QUERY_ONE_FOR_REAL_END (U" (weighted distance)")
}

FORM (QUERY_ONE_FOR_REAL__DTW_getDistanceValue, U"DTW: Get distance value", nullptr) {
	REAL (xTime, U"Time at x (s)", U"0.1")
	REAL (yTime, U"Time at y (s)", U"0.1")
	OK
DO
	QUERY_ONE_FOR_REAL (DTW)
		double result = undefined;
		if ((xTime >= my xmin && xTime <= my xmax) && (yTime >= my ymin && yTime <= my ymax)) {
			integer irow = Matrix_yToNearestRow (me, yTime);
			integer icol = Matrix_xToNearestColumn (me, xTime);
			result = my z [irow] [icol];
		}
	QUERY_ONE_FOR_REAL_END (U" (distance at (", xTime, U", ", yTime, U"))")
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getMinimumDistance) {
	QUERY_ONE_FOR_REAL (DTW)
		double result, maximum;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & result, & maximum);
	QUERY_ONE_FOR_REAL_END (U" (minimum)")
}

DIRECT (QUERY_ONE_FOR_REAL__DTW_getMaximumDistance) {
	QUERY_ONE_FOR_REAL (DTW)
		double minimum, result;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & result);
	QUERY_ONE_FOR_REAL_END (U" (maximum)")
}

FORM (MODIFY_DTW_formula_distances, U"DTW: Formula (distances)", nullptr) {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (DTW)
		Matrix_formula (me, formula, interpreter, 0);
		double minimum, maximum;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		if (minimum < 0.0)
			Melder_throw (U"Execution of the formula has made some distance(s) negative.");
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_DTW_setDistanceValue, U"DTW: Set distance value", nullptr) {
	REAL (xTime, U"Time at x (s)", U"0.1")
	REAL (yTime, U"Time at y (s)", U"0.1")
	REAL (newDistance, U"New value", U"0.0")
	OK
DO
	if (newDistance < 0)
		Melder_throw (U"Distances cannot be negative.");
	MODIFY_EACH (DTW)
		if (xTime < my xmin || xTime > my xmax)
			Melder_throw (U"Time at x outside domain.");
		if (yTime < my ymin || yTime > my ymax)
			Melder_throw (U"Time at y outside domain.");
		const integer irow = Matrix_yToNearestRow (me, yTime);
		const integer icol = Matrix_xToNearestColumn (me, xTime);
		my z [irow] [icol] = newDistance;
	MODIFY_EACH_END
}

FORM (MODIFY_DTW_findPath, U"DTW: Find path", nullptr) {
	DTW_constraints_addCommonFields(matchStart,matchEnd,slopeConstraint)
	OK
DO
	MODIFY_EACH (DTW)
		DTW_findPath (me, matchStart, matchEnd, slopeConstraint);
	MODIFY_EACH_END
}

FORM (MODIFY_DTW_findPath_bandAndSlope, U"DTW: find path (band & slope)", nullptr) {
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.05")
    CHOICE (slopeConstraint, U"Slope constraint", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
    OK
DO
    MODIFY_EACH (DTW)
        DTW_findPath_bandAndSlope (me, sakoeChibaBand, slopeConstraint, nullptr);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__DTW_to_Matrix_cumulativeDistances, U"DTW: To Matrix", nullptr) {
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.05")
    CHOICE (slopeConstraint, U"Slope constraint", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_EACH_TO_ONE (DTW)
        autoMatrix result = DTW_to_Matrix_cumulativeDistances (me, sakoeChibaBand, slopeConstraint);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cd")
}

FORM (CONVERT_EACH_TO_ONE__DTW_to_Polygon, U"DTW: To Polygon...", nullptr) {
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.1")
    CHOICE (slopeConstraint, U"Slope constraint", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_EACH_TO_ONE (DTW)
        autoPolygon result = DTW_to_Polygon (me, sakoeChibaBand, slopeConstraint);
    CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__DTW_to_Matrix_distances) {
	CONVERT_EACH_TO_ONE (DTW)
		autoMatrix result = DTW_to_Matrix_distances (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__DTW_swapAxes) {
	CONVERT_EACH_TO_ONE (DTW)
		autoDTW result = DTW_swapAxes (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_axesSwapped")
}

DIRECT (MODIFY_DTW_Matrix_replace) {
	MODIFY_FIRST_OF_ONE_AND_ONE (DTW, Matrix)
		DTW_Matrix_replace (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__DTW_TextGrid_to_TextGrid) {
	CONVERT_ONE_AND_ONE_TO_ONE (DTW, TextGrid)
		autoTextGrid result = DTW_TextGrid_to_TextGrid (me, you, 0);
		// in result use the name of the textgrid and append the name of the DTW object
	CONVERT_ONE_AND_ONE_TO_ONE_END (your name.get(), U"_", my name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__DTW_IntervalTier_to_Table) {
	CONVERT_ONE_AND_ONE_TO_ONE (DTW, IntervalTier)
		autoTable result = DTW_IntervalTier_to_Table (me, you, 1.0/44100);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

/******************** EditDistanceTable & EditCostsTable ********************************************/

DIRECT (HELP__EditDistanceTable_help) {
	HELP (U"EditDistanceTable")
}

DIRECT (CONVERT_EACH_TO_ONE__EditDistanceTable_to_TableOfReal_directions) {
	CONVERT_EACH_TO_ONE (EditDistanceTable)
		autoTableOfReal result = EditDistanceTable_to_TableOfReal_directions (me);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

DIRECT (MODIFY_EditDistanceTable_setEditCosts) {
	MODIFY_FIRST_OF_ONE_AND_ONE (EditDistanceTable, EditCostsTable)
		EditDistanceTable_setEditCosts (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_EditDistanceTable_setDefaultCosts, U"", nullptr) {
	POSITIVE (insertionCosts, U"Insertion costs", U"1.0")
	POSITIVE (deletionCosts, U"Deletion costs", U"1.0")
	POSITIVE (substitutionCosts, U"Substitution costs", U"2.0")
	OK
DO
	MODIFY_EACH (EditDistanceTable)
		EditDistanceTable_setDefaultCosts (me, insertionCosts, deletionCosts, substitutionCosts);
	MODIFY_EACH_END
}

FORM (GRAPHICS_EACH__EditDistanceTable_draw, U"EditDistanceTable_draw", nullptr) {
	CHOICE (format, U"Format", 3)
		OPTION (U"decimal")
		OPTION (U"exponential")
		OPTION (U"free")
		OPTION (U"rational")
	NATURAL (precision, U"Precision", U"1")
	REAL (angle, U"Rotate source labels by (degrees)", U"0.0")
	OK
DO
	GRAPHICS_EACH (EditDistanceTable)
		EditDistanceTable_draw (me, GRAPHICS, format, precision, angle);
	GRAPHICS_EACH_END
}

DIRECT (GRAPHICS_EACH__EditDistanceTable_drawEditOperations) {
	GRAPHICS_EACH (EditDistanceTable)
		EditDistanceTable_drawEditOperations (me, GRAPHICS);
	GRAPHICS_EACH_END
}

DIRECT (HELP__EditCostsTable_help) {
	HELP (U"EditCostsTable")
}

FORM (QUERY_ONE_FOR_INTEGER__EditCostsTable_getTargetIndex, U"EditCostsTable: Get target index", nullptr) {
	SENTENCE (target, U"Target", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (EditCostsTable)
		const integer result = EditCostsTable_getTargetIndex (me, target);
	QUERY_ONE_FOR_INTEGER_END (U" (target index)")
}

FORM (QUERY_ONE_FOR_INTEGER__EditCostsTable_getSourceIndex, U"EditCostsTable: Get source index", nullptr) {
	SENTENCE (source, U"Source", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (EditCostsTable)
		const integer result = EditCostsTable_getSourceIndex (me, source);
	QUERY_ONE_FOR_INTEGER_END (U" (source index)")
}

FORM (QUERY_ONE_FOR_REAL__EditCostsTable_getInsertionCosts, U"EditCostsTable: Get insertion cost", nullptr) {
	SENTENCE (target, U"Target", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (EditCostsTable)
		const double result = EditCostsTable_getInsertionCost (me, target);
	QUERY_ONE_FOR_REAL_END (U" (insertion cost)")
}

FORM (QUERY_ONE_FOR_REAL__EditCostsTable_getDeletionCost, U"EditCostsTable: Get deletion cost", nullptr) {
	SENTENCE (source, U"Source", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (EditCostsTable)
		const double result = EditCostsTable_getDeletionCost (me, source);
	QUERY_ONE_FOR_REAL_END (U" (deletion cost)")
}

FORM (QUERY_ONE_FOR_REAL__EditCostsTable_getSubstitutionCost, U"EditCostsTable: Get substitution cost", nullptr) {
	SENTENCE (target, U"Target", U"")
	SENTENCE (source, U"Source", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (EditCostsTable)
		const double result = EditCostsTable_getSubstitutionCost (me, target, source);
	QUERY_ONE_FOR_REAL_END (U" (substitution cost)")
}

FORM (QUERY_ONE_FOR_REAL__EditCostsTable_getCosts_others, U"EditCostsTable: Get cost (others)", nullptr) {
	CHOICE (costTypes, U"Others cost type", 1)
		OPTION (U"insertion")
		OPTION (U"deletion")
		OPTION (U"equality")
		OPTION (U"inequality")
	OK
DO
	QUERY_ONE_FOR_REAL (EditCostsTable)
		const double result = EditCostsTable_getOthersCost (me,costTypes);
	QUERY_ONE_FOR_REAL_END (U" (cost)")
}

FORM (MODIFY_EditCostsTable_setTargetSymbol_index, U"EditCostsTable: Set target symbol (index)", nullptr) {
	NATURAL (index, U"Index", U"1")
	SENTENCE (target, U"Target", U"a")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		TableOfReal_setRowLabel (me, index, target);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setSourceSymbol_index, U"EditCostsTable: Set source symbol (index)", nullptr) {
	NATURAL (index, U"Index", U"1")
	SENTENCE (source, U"Source", U"a")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		TableOfReal_setColumnLabel (me, index, source);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setInsertionCosts, U"EditCostsTable: Set insertion costs", nullptr) {
	SENTENCE (targets, U"Targets", U"")
	REAL (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setInsertionCosts (me, targets, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setDeletionCosts, U"EditCostsTable: Set deletion costs", nullptr) {
	SENTENCE (sources, U"Sources", U"")
	REAL (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setDeletionCosts (me, sources, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setSubstitutionCosts, U"EditCostsTable: Set substitution costs", nullptr) {
	SENTENCE (targets, U"Targets", U"a i u")
	SENTENCE (sources, U"Sources", U"a i u")
	REAL (cost, U"Cost", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setSubstitutionCosts (me, targets, sources, cost);
	MODIFY_EACH_END
}

FORM (MODIFY_EditCostsTable_setCosts_others, U"EditCostsTable: Set costs (others)", nullptr) {
	COMMENT (U"Others costs")
	REAL (insertionCosts, U"Insertion", U"1.0")
	REAL (deletionCosts, U"Deletion", U"1.0")
	COMMENT (U"Substitution costs")
	REAL (equalityCosts, U"Equality", U"0.0")
	REAL (inequalityCosts, U"Inequality", U"2.0")
	OK
DO
	MODIFY_EACH (EditCostsTable)
		EditCostsTable_setOthersCosts (me, insertionCosts, deletionCosts, equalityCosts, inequalityCosts);
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__EditCostsTable_to_TableOfReal) {
	CONVERT_EACH_TO_ONE (EditCostsTable)
		autoTableOfReal result = EditCostsTable_to_TableOfReal (me);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

FORM (CREATE_ONE__EditCostsTable_createEmpty, U"Create empty EditCostsTable", U"Create empty EditCostsTable...") {
	SENTENCE (name, U"Name", U"editCosts")
	INTEGER (numberOfTargetSymbols, U"Number of target symbols", U"0")
	INTEGER (numberOfSourceSymbols, U"Number of source symbols", U"0")
	OK
DO
	CREATE_ONE
		numberOfTargetSymbols = std::max (0_integer, numberOfTargetSymbols);
		numberOfSourceSymbols = std::max (0_integer, numberOfSourceSymbols );
		autoEditCostsTable result = EditCostsTable_create (numberOfTargetSymbols, numberOfSourceSymbols);
	CREATE_ONE_END (name)
}

/******************** Eigen ********************************************/

DIRECT (HELP__Eigen_help) {
	HELP (U"Eigen")
}

DIRECT (WARNING__Eigen_drawEigenvalues_scree) {
	WARNING
		Melder_warning (U"The command \"Draw eigenvalues (scree)...\" has been "
			"removed.\n To get a scree plot, use \"Draw eigenvalues...\" with the "
			"arguments\n 'Fraction of eigenvalues summed' and 'Cumulative' unchecked."
		);
	WARNING_END
}

FORM (GRAPHICS_EACH__Eigen_drawEigenvalues, U"Eigen: Draw eigenvalues", U"Eigen: Draw eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (fractionSummed, U"Fraction of eigenvalues summed", false)
	BOOLEAN (cumulative, U"Cumulative", false)
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Eigen)
		Eigen_drawEigenvalues (me, GRAPHICS, fromEigenvalue, toEigenvalue, fromAmplitude, toAmplitude, 
			fractionSummed, cumulative, markSize_mm, mark_string, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Eigen_drawEigenvector, U"Eigen: Draw eigenvector", U"Eigen: Draw eigenvector...") {
	INTEGER (eigenvectorNumber, U"Eigenvector number", U"1")
	BOOLEAN (loadings, U"Component loadings", false)
	INTEGER (fromElement, U"left Element range", U"0")
	INTEGER (toElement, U"right Element range", U"0")
	REAL (fromAmplitude, U"left Amplitude range", U"-1.0")
	REAL (toAmplitude, U"right Amplitude range", U"1.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (connectPoints, U"Connect points", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Eigen)
		Eigen_drawEigenvector (me, GRAPHICS, eigenvectorNumber, fromElement, toElement, fromAmplitude, 
			toAmplitude, loadings, markSize_mm, mark_string, connectPoints, nullptr,  garnish
		);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__Eigen_getNumberOfEigenvalues) {
	QUERY_ONE_FOR_INTEGER (Eigen)
		const integer result = my numberOfEigenvalues;
	QUERY_ONE_FOR_INTEGER_END (U" (number of eigenvalues)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Eigen_getNumberOfEigenvectors) {
	QUERY_ONE_FOR_INTEGER (Eigen)
		const integer result = my numberOfEigenvalues;
	QUERY_ONE_FOR_INTEGER_END (U" (number of eigenvectors)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Eigen_getEigenvectorDimension) {
	QUERY_ONE_FOR_INTEGER (Eigen)
		const integer result = my dimension;
	QUERY_ONE_FOR_INTEGER_END (U" (dimension)")
}

FORM (QUERY_ONE_FOR_REAL__Eigen_getEigenvalue, U"Eigen: Get eigenvalue", U"Eigen: Get eigenvalue...") {
	NATURAL (eigenvalueNumber, U"Eigenvalue number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Eigen)
		double result = undefined;
		if (eigenvalueNumber > 0 && eigenvalueNumber <= my numberOfEigenvalues) {
			result = my eigenvalues [eigenvalueNumber];
		}
	QUERY_ONE_FOR_REAL_END (U" (eigenvalue [", eigenvalueNumber, U"])")
}

DIRECT (QUERY_ONE_FOR_REAL_VECTOR__Eigen_listEigenvalues) {
	QUERY_ONE_FOR_REAL_VECTOR (Eigen)
		autoVEC result = Eigen_listEigenvalues (me);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

DIRECT (QUERY_ONE_FOR_REAL_VECTOR__Eigen_listEigenvalues_imag) {
	QUERY_ONE_FOR_REAL_VECTOR (Eigen)
		autoVEC result = Eigen_listEigenvalues_imag (me);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL__Eigen_getSumOfEigenvalues, U"Eigen: Get sum of eigenvalues", U"Eigen: Get sum of eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range",  U"1")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0 (=all)")
	OK
DO
	QUERY_ONE_FOR_REAL (Eigen)
		if (toEigenvalue == 0)
			toEigenvalue = my numberOfEigenvalues;
		const double result = Eigen_getSumOfEigenvalues (me, fromEigenvalue, toEigenvalue);
	QUERY_ONE_FOR_REAL_END (U" (sum of eigenvalues [", fromEigenvalue, U"..", toEigenvalue, U"])")
}

FORM (QUERY_ONE_FOR_REAL__Eigen_getEigenvectorElement, U"Eigen: Get eigenvector element", U"Eigen: Get eigenvector element...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	NATURAL (elementNumber, U"Element number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Eigen)
		const double result = Eigen_getEigenvectorElement (me, eigenvectorNumber, elementNumber);
	QUERY_ONE_FOR_REAL_END (U" (eigenvector [", eigenvectorNumber, U"] element [", elementNumber, U"])")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__Eigen_getEigenvector, U"Eigen: Get eigenvector", U"Eigen: Get eigenvector...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Eigen)
		autoVEC result = Eigen_getEigenvector (me, eigenvectorNumber);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__Eigen_getEigenvector_imag, U"Eigen: Get eigenvector (imag)", U"Eigen: Get eigenvector...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Eigen)
		autoVEC result = Eigen_getEigenvector_imag (me, eigenvectorNumber);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

DIRECT (QUERY_ONE_FOR_BOOLEAN__Eigen_areAllEigenvaluesReal) {
	QUERY_ONE_FOR_BOOLEAN (Eigen)
		const integer result = Eigen_areAllEigenvaluesReal (me);
	QUERY_ONE_FOR_BOOLEAN_END (result ? U" (all eigenvalues are real)" : U" (not all eigenvalues are real)")
}

FORM (MODIFY__Eigen_sort, U"Eigen: Sort...", U"Eigen: Sort...") {
	COMMENT (U"Sort eigenvalues and corresponding eigenvectors")
	BOOLEAN (sortAscending, U"Sort ascending", true)
	OK
DO
	MODIFY_EACH (Eigen)
		Eigen_sort_special (me, sortAscending);
	MODIFY_EACH_END
}

FORM (MODIFY__Eigen_invertEigenvector, U"Eigen: Invert eigenvector", nullptr) {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Eigen)
		Eigen_invertEigenvector (me, eigenvectorNumber);
	MODIFY_EACH_END
}

DIRECT (MODIFY_ALL__Eigens_alignEigenvectors) {
	MODIFY_ALL (Eigen)
		Eigens_alignEigenvectors (& list);
	MODIFY_ALL_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Matrix_projectColumns, U"Eigen & Matrix: Project columns", U"Eigen & Matrix: Project...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Eigen, Matrix)
		autoMatrix result = Eigen_Matrix_to_Matrix_projectColumns (me, you, numberOfDimensions);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__Eigen_SSCP_project) {
	CONVERT_ONE_AND_ONE_TO_ONE (Eigen, SSCP)
		autoSSCP result = Eigen_SSCP_project (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Covariance_project) {
	CONVERT_ONE_AND_ONE_TO_ONE (Eigen, Covariance)
		autoCovariance result = Eigen_Covariance_project (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

/******************** Index ********************************************/

DIRECT (HELP__Index_help) {
	HELP (U"Index")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Index_getNumberOfClasses) {
	QUERY_ONE_FOR_INTEGER (Index)
		const integer result = my classes->size;
	QUERY_ONE_FOR_INTEGER_END (U" (number of classes)")
}

FORM (QUERY_ONE_FOR_STRING__StringsIndex_getClassLabelFromClassIndex, U"StringsIndex: Get class label", U"StringsIndex: Get class label...") {
	NATURAL (index, U"Class index", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (StringsIndex)
		conststring32 result = StringsIndex_getClassLabelFromClassIndex (me, index);
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_STRING__StringsIndex_getItemLabelFromItemIndex, U"StringsIndex: Get item label", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (StringsIndex)
		conststring32 result = StringsIndex_getItemLabelFromItemIndex (me, itemIndex);
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_INTEGER__StringsIndex_getClassIndexFromItemIndex, U"StringsIndex: Get class index from item index", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (StringsIndex)
		const integer result = Index_getClassIndexFromItemIndex (me, itemIndex);
	QUERY_ONE_FOR_INTEGER_END (U" (class index)")
}

FORM (QUERY_ONE_FOR_INTEGER__Index_getIndex, U"StringsIndex: Get item index", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (StringsIndex)
		const integer result = Index_getClassIndexFromItemIndex (me, itemIndex);
	QUERY_ONE_FOR_INTEGER_END (U" (class index)")
}

FORM (QUERY_ONE_FOR_INTEGER__StringsIndex_getClassIndex, U"StringsIndex: Get class index from class label", nullptr) {
	WORD (klasLabel, U"Class label", U"label")
	OK
DO
	QUERY_ONE_FOR_INTEGER (StringsIndex)
		const integer result = StringsIndex_getClassIndexFromClassLabel (me, klasLabel);
	QUERY_ONE_FOR_INTEGER_END (U" (class index)")
}

DIRECT (QUERY_ONE_FOR_STRING_ARRAY__StringsIndex_listAllClasses) {
	QUERY_ONE_FOR_STRING_ARRAY (StringsIndex)
		autoSTRVEC result = StringsIndex_listAllClasses (me);
	QUERY_ONE_FOR_STRING_ARRAY_END
}

FORM (CONVERT_EACH_TO_ONE__Index_extractPart, U"Index: Extract part", U"Index: Extract part...") {
	INTEGER (fromItem, U"left Item range", U"0")
	INTEGER (toItem, U"right Item range", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Index)
		autoIndex result = Index_extractPart (me, fromItem, toItem);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_part")
}

FORM (CONVERT_EACH_TO_ONE__Index_to_Permutation, U"Index: To Permutation", U"Index: To Permutation...") {
	BOOLEAN (permuteWithinClasses, U"Permute within classes", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Index)
		autoPermutation result = Index_to_Permutation_permuteRandomly (me, permuteWithinClasses);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__StringsIndex_to_Strings) {
	CONVERT_EACH_TO_ONE (StringsIndex)
		autoStrings result = StringsIndex_to_Strings (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/******************** Excitation ********************************************/

DIRECT (COMBINE_ALL_TO_ONE__Excitations_to_ExcitationList) {
	COMBINE_ALL_TO_ONE (Excitation)
		autoExcitationList result = Excitations_to_ExcitationList (& list);
	COMBINE_ALL_TO_ONE_END (U"appended")
}

/******************** ExcitationList ********************************************/

FORM (MODIFY_ExcitationList_formula, U"ExcitationList: Formula", nullptr) {
	COMMENT (U"for all objects in ExcitationList do { for col := 1 to ncol do { self [col] := `formula' ; x := x + dx } }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (ExcitationList)
		for (integer j = 1; j <= my size; j ++)
			Matrix_formula (my at [j], formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

DIRECT (MODIFY_ExcitationList_addItem) {
	MODIFY_FIRST_OF_ONE_AND_ALL (ExcitationList, Excitation)
		ExcitationList_addItems (me, & list);
	MODIFY_FIRST_OF_ONE_AND_ALL_END
}

FORM (CONVERT_EACH_TO_ONE__ExcitationList_extractItem, U"ExcitationList: Extract item", nullptr) {
	NATURAL (itemIndex, U"Item index", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (ExcitationList)
		autoExcitation result = ExcitationList_extractItem (me, itemIndex);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", itemIndex)
}

DIRECT (CONVERT_TWO_TO_ONE__ExcitationList_append) {
	CONVERT_TWO_TO_ONE (ExcitationList)
		autoExcitationList result = Data_copy (me);
		result -> merge (you);
	CONVERT_TWO_TO_ONE_END (U"appended")
}

FORM (CONVERT_EACH_TO_ONE__ExcitationList_to_PatternList, U"Excitations: To PatternList", nullptr) {
	NATURAL (join, U"Join", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (ExcitationList)
		autoPatternList result = ExcitationList_to_PatternList (me, join);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__ExcitationList_to_TableOfReal) {
	CONVERT_EACH_TO_ONE (ExcitationList)
		autoTableOfReal result = ExcitationList_to_TableOfReal (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}


/************************* FileInMemory ***********************************/


FORM_READ (READ_ONE__FileInMemory_create, U"Create file in memory", nullptr, true) {
	READ_ONE
		autoFileInMemory result = FileInMemory_create (file);
	READ_ONE_END
}

FORM (INFO_ONE__FileInMemory_showAsCode, U"FileInMemory: Show as code", nullptr) {
	WORD (name, U"Name", U"example")
	INTEGER (numberOfByterPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemory)
		MelderInfo_open ();
		FileInMemory_showAsCode (me, name, numberOfByterPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

/************************* FileInMemorySet ***********************************/

DIRECT (QUERY_ONE_FOR_INTEGER__FileInMemorySet_getNumberOfFiles) {
	QUERY_ONE_FOR_INTEGER (FileInMemorySet)
		const integer result = my size;
	QUERY_ONE_FOR_INTEGER_END (U" (number of files)")
}

FORM (QUERY_ONE_FOR_BOOLEAN__FileInMemorySet_hasDirectory, U"FileInMemorySet: Has directory?", nullptr) {
	WORD (name, U"Name", U"aav")
	OK
DO
	QUERY_ONE_FOR_BOOLEAN (FileInMemorySet)
		const bool result = FileInMemorySet_hasDirectory (me, name);
	QUERY_ONE_FOR_BOOLEAN_END (U" (has directory?)")
}

FORM (CONVERT_EACH_TO_ONE__FileInMemorySet_downto_Table, U"FileInMemorySet: Down to Table", nullptr) {
	BOOLEAN (openFilesOnly, U"Open files only?", false)
	OK
DO
	CONVERT_EACH_TO_ONE (FileInMemorySet)
		autoTable result = FileInMemorySet_downto_Table (me, openFilesOnly);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CREATE_ONE__FileInMemorySet_createFromDirectoryContents, U"Create files in memory from directory contents", nullptr) {
	SENTENCE (name, U"Name", U"list")
	FOLDER (directory, U"Directory", U"/home/david/projects/espeak-ng/espeak-ng-data/voices/!v")
	WORD (fileGlobber, U"Only files that match pattern", U"*")
	OK
DO
	CREATE_ONE
		autoFileInMemorySet result = FileInMemorySet_createFromDirectoryContents (directory, fileGlobber);
	CREATE_ONE_END (name)
}

FORM (CONVERT_EACH_TO_ONE__FileInMemorySet_extractFiles, U"FileInMemorySet: Extract files", nullptr) {
	COMMENT (U"Extract all files where the file name ")
	OPTIONMENU_ENUM (kMelder_string, which, U"...", kMelder_string::CONTAINS)
	SENTENCE (criterion, U"...the text", U"/voices/")
	OK
DO
	CONVERT_EACH_TO_ONE (FileInMemorySet)
		autoFileInMemorySet result = FileInMemorySet_extractFiles (me, which, criterion);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__FileInMemorySet_removeFiles, U"FileInMemorySet: Remove files", nullptr) {
	COMMENT (U"Remove all files where the file name ")
	OPTIONMENU_ENUM (kMelder_string, which, U"...", kMelder_string::CONTAINS)
	SENTENCE (criterion, U"...the text", U"/voices/")
	OK
DO
	CONVERT_EACH_TO_ONE (FileInMemorySet)
		autoFileInMemorySet result = FileInMemorySet_removeFiles (me, which, criterion);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (INFO_ONE__FileInMemorySet_showAsCode, U"FileInMemorySet: Show as code", nullptr) {
	WORD (functionName, U"Function name", U"example_FileInMemorySet_add_examples")
	INTEGER (numberOfBytesPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemorySet)
		MelderInfo_open ();
		FileInMemorySet_showAsCode (me, functionName, numberOfBytesPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__FileInMemorySet_showOneFileAsCode, U"FileInMemorySet: Show one file as code", nullptr) {
	NATURAL (index, U"Index", U"1")
	WORD (name, U"Name", U"example")
	INTEGER (numberOfBytesPerLine, U"Number of bytes per line", U"20")
	OK
DO
	INFO_ONE (FileInMemorySet)
		MelderInfo_open ();
		FileInMemorySet_showOneFileAsCode (me, index, name, numberOfBytesPerLine);
		MelderInfo_close ();
	INFO_ONE_END
}

DIRECT (COMBINE_ALL_TO_ONE__FilesInMemory_to_FileInMemorySet) {
	COMBINE_ALL_TO_ONE (FileInMemory)
		autoFileInMemorySet result = FilesInMemory_to_FileInMemorySet (list);
	COMBINE_ALL_TO_ONE_END (U"merged");
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__FileInMemorySet_addItemsToSet) {
	MODIFY_FIRST_OF_ONE_AND_ONE (FileInMemorySet, FileInMemory)
		autoFileInMemory him = Data_copy (you);
		my addItem_move (him.move());
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (COMBINE_ALL_TO_ONE__FileInMemorySets_merge) {
	COMBINE_ALL_TO_ONE (FileInMemorySet)
		autoFileInMemorySet result = FileInMemorySets_merge (list);
	COMBINE_ALL_TO_ONE_END (U"merge");
}

DIRECT (CONVERT_EACH_TO_ONE__FileInMemorySet_to_Strings_path) {
	CONVERT_EACH_TO_ONE (FileInMemorySet)
		autoStrings result = FileInMemorySet_to_Strings_path (me);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

/************************* FilterBank ***********************************/

FORM (GRAPHICS_EACH__FilterBank_drawFilters, U"FilterBank: Draw filters", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawRows (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FilterBank_drawOneContour, U"FilterBank: Draw one contour", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (height, U"Height (dB)", U"40.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawOneContour (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, height);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FilterBank_drawContours, U"FilterBank: Draw contours", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawContours (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FilterBank_drawFrequencyScales, U"FilterBank: Draw frequency scales", U"FilterBank: Draw frequency scales...") {
	CHOICE (xFrequencyScale, U"Horizontal frequency scale", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	REAL (xFromFrequency, U"left Horizontal frequency range", U"0.0")
	REAL (xToFrequency, U"right Horizontal frequency range", U"0.0")
	CHOICE (yFrequencyScale, U"Vertical frequency scale", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	REAL (yFromFrequency, U"left Vertical frequency range", U"0.0")
	REAL (yToFrequency, U"right Vertical frequency range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawFrequencyScales (me, GRAPHICS, xFrequencyScale, xFromFrequency, xToFrequency, yFrequencyScale,
			yFromFrequency, yToFrequency, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__MelSpectrogram_drawFrequencyScale, U"", U"") {
	REAL (fromFrequency, U"left Horizontal frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Horizontal frequency range (Hz)", U"0.0")
	REAL (yFromFrequency, U"left Vertical frequency range (mel)", U"0.0")
	REAL (yToFrequency, U"right Vertical frequency range (mel)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawFrequencyScale (me, GRAPHICS, fromFrequency, toFrequency, yFromFrequency, yToFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__BarkSpectrogram_drawFrequencyScale, U"", U"") {
	REAL (fromFrequency, U"left Horizontal frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Horizontal frequency range (Hz)", U"0.0")
	REAL (yFromFrequency, U"left Vertical frequency range (bark)", U"0.0")
	REAL (yToFrequency, U"right Vertical frequency range (bark)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawFrequencyScale (me, GRAPHICS, fromFrequency, toFrequency, yFromFrequency, yToFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__MelSpectrogram_paintImage, U"MelSpectrogram: Paint image", U"MelSpectrogram: Paint image...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, 
			fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__BarkSpectrogram_paintImage, U"BarkSpectrogram: Paint image", U"BarkSpectrogram: Paint image...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, 
			fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FilterBank_paintImage, U"FilterBank: Paint image", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintImage (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FilterBank_paintContours, U"FilterBank: Paint contours", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintContours (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}


FORM (GRAPHICS_EACH__FilterBank_paintCells, U"FilterBank: Paint cells", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintCells (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FilterBank_paintSurface, U"FilterBank: Paint surface", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintSurface (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, 30, 45);
	GRAPHICS_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__FilterBank_getFrequencyInHertz, U"FilterBank: Get frequency in Hertz", U"FilterBank: Get frequency in Hertz...") {
	REAL (frequency, U"Frequency", U"10.0")
	CHOICE (frequencyUnit, U"Unit", 2)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	OK
DO
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = FilterBank_getFrequencyInHertz (me, frequency, frequencyUnit);
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

FORM (QUERY_ONE_FOR_REAL__FilterBank_getFrequencyInBark, U"FilterBank: Get frequency in Bark", U"FilterBank: Get frequency in Bark...") {
	REAL (frequency, U"Frequency", U"93.17")
	CHOICE (frequencyUnit, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	OK
DO
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = FilterBank_getFrequencyInBark (me, frequency, frequencyUnit);
	QUERY_ONE_FOR_REAL_END (U" bark")
}

FORM (QUERY_ONE_FOR_REAL__FilterBank_getFrequencyInMel, U"FilterBank: Get frequency in mel", U"FilterBank: Get frequency in mel...") {
	REAL (frequency, U"Frequency", U"1000.0")
	CHOICE (frequencyUnit, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	OK
DO
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = FilterBank_getFrequencyInMel (me, frequency, frequencyUnit);
	QUERY_ONE_FOR_REAL_END (U" mel")
}

FORM (MODIFY_EACH__FilterBank_equalizeIntensities, U"FilterBank: Equalize intensities", nullptr) {
	REAL (intensity_dB, U"Intensity (dB)", U"80.0")
	OK
DO
	MODIFY_EACH (FilterBank)
		FilterBank_equalizeIntensities (me, intensity_dB);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__BandFilterSpectrogram_equalizeIntensities, U"BandFilterSpectrogram: Equalize intensities", nullptr) {
	REAL (intensity_dB, U"Intensity (dB)", U"80.0")
	OK
DO
	MODIFY_EACH (BandFilterSpectrogram)
		BandFilterSpectrogram_equalizeIntensities (me, intensity_dB);
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__FilterBank_to_Matrix) {
	CONVERT_EACH_TO_ONE (FilterBank)
		autoMatrix result = FilterBank_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__BandFilterSpectrogram_to_Matrix, U"BandFilterSpectrogram: To Matrix", nullptr) {
	BOOLEAN (convertToDB, U"Convert to dB values", 1)
	OK
DO
	CONVERT_EACH_TO_ONE (BandFilterSpectrogram)
		autoMatrix result = BandFilterSpectrogram_to_Matrix (me, convertToDB);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_TWO_TO_ONE__FilterBanks_crossCorrelate, U"FilterBanks: Cross-correlate", nullptr) {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (FilterBank)
		autoSound result = FilterBanks_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_TWO_TO_ONE__BandFilterSpectrograms_crossCorrelate, U"BandFilterSpectrograms: Cross-correlate", nullptr) {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (BandFilterSpectrogram)
		autoSound result = BandFilterSpectrograms_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_TWO_TO_ONE__FilterBanks_convolve, U"FilterBanks: Convolve", nullptr) {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (FilterBank)
		autoSound result = FilterBanks_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_TWO_TO_ONE__BandFilterSpectrograms_convolve, U"BandFilterSpectrograms: Convolve", nullptr) {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (BandFilterSpectrogram)
		autoSound result = BandFilterSpectrograms_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__FilterBank_to_Intensity) {
	CONVERT_EACH_TO_ONE (FilterBank)
		autoIntensity result = FilterBank_to_Intensity (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__BandFilterSpectrogram_to_Intensity) {
	CONVERT_EACH_TO_ONE (BandFilterSpectrogram)
		autoIntensity result = BandFilterSpectrogram_to_Intensity (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/*********** FormantFilter *******************************************/

DIRECT (HELP__FormantFilter_help) {
	HELP (U"FormantFilter")
}

FORM (GRAPHICS_EACH__FormantFilter_drawFilterFunctions, U"FormantFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	CHOICE (frequencyScale, U"Frequency scale", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (dBScale, U"Amplitude scale in dB", 1)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantFilter)
		FormantFilter_drawFilterFunctions (me, GRAPHICS, bandwidth, frequencyScale, fromFilter, toFilter, 
			fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantFilter_drawSpectrum_slice, U"FormantFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Hz", garnish);
	GRAPHICS_EACH_END
}

/****************** FormantGrid  *********************************/

FORM (GRAPHICS_EACH__old_FormantGrid_draw, U"FormantGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= auto)")
	BOOLEAN (bandwidths, U"Bandwidths", false)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantGrid)
		FormantGrid_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, bandwidths, garnish, U"lines and speckles");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FormantGrid_draw, U"FormantGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= auto)")
	BOOLEAN (bandwidths, U"Bandwidths", false)
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_EACH__old_FormantGrid_draw)
	GRAPHICS_EACH (FormantGrid)
		FormantGrid_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, bandwidths, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

/****************** FunctionSeries  *********************************/

FORM (GRAPHICS_EACH__FunctionSeries_draw, U"FunctionSeries: Draw", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (extrapolate, U"Extrapolate", false)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FunctionSeries)
		FunctionSeries_draw (me, GRAPHICS, xmin, xmax, ymin, ymax,extrapolate, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FunctionSeries_drawBasisFunction, U"FunctionSeries: Draw basis function", nullptr) {
	NATURAL (index, U"Index", U"1")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (extrapolate, U"Extrapolate", false)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FunctionSeries)
		FunctionSeries_drawBasisFunction (me, GRAPHICS, index, xmin, xmax, ymin, ymax, extrapolate, garnish);
	GRAPHICS_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__FunctionSeries_getValue, U"FunctionSeries: Evaluate", nullptr) {
	REAL (x, U"X", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FunctionSeries)
		const double result = FunctionSeries_evaluate (me, x);
	QUERY_ONE_FOR_REAL_END (U"")
}

DIRECT (QUERY_ONE_FOR_INTEGER__FunctionSeries_getNumberOfCoefficients) {
	QUERY_ONE_FOR_INTEGER (FunctionSeries)
		const integer result = my numberOfCoefficients;
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__FunctionSeries_getCoefficient, U"FunctionSeries: Get coefficient", nullptr) {
	COMMENT (U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FunctionSeries)
		const double result = ( index > 0 && index <= my numberOfCoefficients ? my coefficients [index] : undefined );
	QUERY_ONE_FOR_REAL_END (U"")
}

DIRECT (QUERY_ONE_FOR_INTEGER__FunctionSeries_getDegree) {
	QUERY_ONE_FOR_INTEGER (FunctionSeries)
		const integer result = FunctionSeries_getDegree (me);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__FunctionSeries_getMaximum, U"FunctionSeries: Get maximum", U"Polynomial: Get maximum...") {
	COMMENT (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FunctionSeries)
		const double result = FunctionSeries_getMaximum (me, xmin, xmax);
	QUERY_ONE_FOR_REAL_END (U" (maximum)")
}

FORM (QUERY_ONE_FOR_REAL__FunctionSeries_getMinimum, U"FunctionSeries: Get minimum", U"Polynomial: Get minimum...") {
	COMMENT (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FunctionSeries)
		const double result = FunctionSeries_getMinimum (me, xmin, xmax);
	QUERY_ONE_FOR_REAL_END (U" (minimum)")
}

FORM (QUERY_ONE_FOR_REAL__FunctionSeries_getXOfMaximum, U"FunctionSeries: Get x of maximum", U"Polynomial: Get x of maximum...") {
	COMMENT (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FunctionSeries)
		const double result = FunctionSeries_getXOfMaximum (me, xmin, xmax);
	QUERY_ONE_FOR_REAL_END (U" (x of maximum)")
}

FORM (QUERY_ONE_FOR_REAL__FunctionSeries_getXOfMinimum, U"FunctionSeries: Get x of minimum", U"Polynomial: Get x of minimum...") {
	COMMENT (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FunctionSeries)
		const double result = FunctionSeries_getXOfMinimum (me, xmin, xmax);
	QUERY_ONE_FOR_REAL_END (U" (x of minimum)")
}

FORM (MODIFY_EACH__FunctionSeries_setCoefficient, U"FunctionSeries: Set coefficient", nullptr) {
	COMMENT (U"p(x) = c[1]F[0] + c[2]F[1] + ... c[n+1]F[n]")
	COMMENT (U"F[k] is of degree k")
	NATURAL (index, U"Index", U"1")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FunctionSeries)
		FunctionSeries_setCoefficient (me, index, value);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FunctionSeries_setDomain, U"FunctionSeries: Set domain", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"2.0")
	OK
DO
	MODIFY_EACH (FunctionSeries)
		Melder_require (xmin < xmax,
			U"\"Xmin\" should be smaller than \"Xmax\"");
		FunctionSeries_setDomain (me, xmin, xmax);
	MODIFY_EACH_END
}

/***************** Intensity ***************************************************/

FORM (CONVERT_EACH_TO_ONE__Intensity_to_TextGrid_silences, U"Intensity: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...") {
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	WORD (silenceLabel, U"Silent interval label", U"silent")
	WORD (soundingLabel, U"Sounding interval label", U"sounding")
	OK
DO
	CONVERT_EACH_TO_ONE (Intensity)
		autoTextGrid result = Intensity_to_TextGrid_detectSilences (me, silenceThreshold, minimumSilenceDuration,
			minimumSoundingDuration, silenceLabel, soundingLabel
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (QUERY_ONE_FOR_REAL__Intensity_getNearestLevelCrossing, U"Intensity: Get nearest level crossing", U"") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (level, U"Level (dB)", U"60")
	OPTIONMENU_ENUM (kVectorSearchDirection, searchDirection, U"Search direction", kVectorSearchDirection::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getNearestLevelCrossing (me, 1, time, level, searchDirection);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

/***************** IntensityTier ***************************************************/

FORM (CONVERT_EACH_TO_ONE__IntensityTier_to_TextGrid_silences, U"IntensityTier: To TextGrid (silences)", U"Intensity: To TextGrid (silences)...") {
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	WORD (silenceLabel, U"Silent interval label", U"silent")
	WORD (soundingLabel, U"Sounding interval label", U"sounding")
	POSITIVE (timeStep, U"Time step (s)", U"0.001")
	OK
DO
	CONVERT_EACH_TO_ONE (IntensityTier)
		autoTextGrid result = IntensityTier_to_TextGrid_detectSilences (me, timeStep, silenceThreshold,
			minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__IntensityTier_to_Intensity, U"", nullptr) {
	POSITIVE (timeStep, U"Time step (s)", U"0.001")
	OK
DO
	CONVERT_EACH_TO_ONE (IntensityTier)
		autoIntensity result = IntensityTier_to_Intensity (me, timeStep);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/***************** ISpline ***************************************************/

DIRECT (HELP__ISpline_help) {
	HELP (U"ISpline")
}

FORM (CREATE_ONE__ISpline_create, U"Create ISpline", U"Create ISpline...") {
	WORD (name, U"Name", U"ispline")
	COMMENT (U"Domain")
	REAL (xmin, U"Xmin", U"0")
	REAL (xmax, U"Xmax", U"1")
	COMMENT (U"ISpline(x) = c[1] I[1](x) + c[2] I[1](x) + ... c[n] I[n](x)")
	COMMENT (U"all I[k] are polynomials of degree \"Degree\"")
	COMMENT (U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree")
	INTEGER (degree, U"Degree", U"3")
	REALVECTOR (coefficients, U"Coefficients (c[k])", WHITESPACE_SEPARATED_, U"1.2 2.0 1.2 1.2 3.0 0.0")
	REALVECTOR (interiorKnots, U"Interior knots", WHITESPACE_SEPARATED_, U"0.3 0.5 0.6")
	OK
DO
	CREATE_ONE
		Melder_require (xmin < xmax,
			U"Xmin should be smaller than Xmax.");
		autoISpline result = ISpline_createWithCoefficients (xmin, xmax, degree, coefficients, interiorKnots);
	CREATE_ONE_END (name)
}

/******************* KlattTable  *********************************/

DIRECT (HELP__KlattTable_help) {
	HELP (U"KlattTable")
}

DIRECT (CREATE_ONE__KlattTable_createExample) {
	CREATE_ONE
		autoKlattTable result = KlattTable_createExample ();
	CREATE_ONE_END (U"example")
}

FORM (CONVERT_EACH_TO_ONE__KlattTable_to_Sound, U"KlattTable: To Sound", U"KlattTable: To Sound...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"16000")
	CHOICE (synthesisModel, U"Synthesis model", 1)
		OPTION (U"cascade")
		OPTION (U"parallel")
	NATURAL (numberOfFormants, U"Number of formants", U"5")
	POSITIVE (frameDuration, U"Frame duration (s)", U"0.005")
	REAL (flutter_percentage, U"Flutter percentage (%)", U"0.0")   // ppgb: foutgevoelig
	OPTIONMENU (voicingSource, U"Voicing source", 1)
		OPTION (U"impulsive")
		OPTION (U"natural")
	OPTIONMENU (soundOutputType, U"Output type", 1)
		OPTION (U"sound")
		OPTION (U"voicing")
		OPTION (U"aspiration")
		OPTION (U"frication")
		OPTION (U"cascade-glottal-output")
		OPTION (U"parallel-glottal-output")
		OPTION (U"bypass-output")
		OPTION (U"all-excitations")
	OK
DO
	Melder_require (flutter_percentage >= 0.0 && flutter_percentage <= 0.0,
		U"\"Flutter percentage\" should be between 0 and 100%.");
	CONVERT_EACH_TO_ONE (KlattTable)
		autoSound result = KlattTable_to_Sound (me, samplingFrequency, synthesisModel, numberOfFormants, 
			frameDuration, voicingSource, flutter_percentage, soundOutputType
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__KlattTable_to_KlattGrid, U"KlattTable: To KlattGrid", nullptr) {
	POSITIVE (frameDuration, U"Frame duration (s)", U"0.002")
	OK
DO
	CONVERT_EACH_TO_ONE (KlattTable)
		autoKlattGrid result = KlattTable_to_KlattGrid (me, frameDuration);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__KlattTable_to_Table) {
	CONVERT_EACH_TO_ONE (KlattTable)
		autoTable result = KlattTable_to_Table (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Table_to_KlattTable) {
	CONVERT_EACH_TO_ONE (Table)
		autoKlattTable result = Table_to_KlattTable (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (QUERY_ONE_FOR_REAL__Table_getMedianAbsoluteDeviation, U"Table: Get median absolute deviation", U"Table: Get median absolute deviation...") {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnLabel);
		const double result = Table_getMedianAbsoluteDeviation (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (INFO_ONE__Table_reportRobustStatistics, U"Table: Report robust statistics", U"Table: Report robust statistics...") {
	SENTENCE (columnLabel, U"Column label", U"F1")
	POSITIVE (k_stdev, U"Number of standard deviations", U"1.5")
	POSITIVE (tolerance, U"Tolerance", U"1e-6")
	NATURAL (maximumNumberOfiterations, U"Maximum number of iterations", U"30")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnLabel);
		double location, scale;
		Table_reportHuberMStatistics (me, columnNumber, k_stdev, tolerance, & location, & scale, maximumNumberOfiterations);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Location: ", location);
			MelderInfo_writeLine (U"Scale: ", scale);
		MelderInfo_close();
	INFO_ONE_END
}

static void print_means (Table me);
static void print_means (Table me) {
	Table_numericize_a (me, 2);
	Table_numericize_a (me, 3);
	if (my numberOfColumns < 3) {
		MelderInfo_writeLine (U"Table does not have the right format.");
		return;
	}
	MelderInfo_writeLine (
		Melder_padOrTruncateLeft (my columnHeaders [1]. label.get(), 15), U"\t",
		Melder_padOrTruncateLeft (my columnHeaders [2]. label.get(), 15), U"\t",
		Melder_padOrTruncateLeft (my columnHeaders [3]. label.get(), 15)
	);
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		const TableRow row = my rows.at [irow];
		MelderInfo_writeLine (
			Melder_padOrTruncateLeft (row -> cells [1]. string.get(), 15), U"\t",
			Melder_padOrTruncateLeft (Melder_double (row -> cells [2]. number), 15), U"\t",
			Melder_padOrTruncateLeft (Melder_double (row -> cells [3]. number), 15)
		);
	}
}

FORM (QUERY_ONE_FOR_INTEGER__Table_getNumberOfRowsWhere, U"", nullptr) {
	FORMULA (formula, U"Count only rows where the following condition holds", U"1; self$[\"gender\"]=\"M\"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Table)
		const integer result = Table_getNumberOfRowsWhere (me, formula, interpreter);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (INFO_ONE__Table_reportOneWayAnova, U"Table: Report one-way anova",  U"Table: Report one-way anova...") {
	SENTENCE (dataColumn_string, U"Column with data", U"F0")
	SENTENCE (factor_string, U"Factor", U"Vowel")
	BOOLEAN (wantMeans, U"Table with means", false);
	BOOLEAN (wantDifferencesBetweenMeans, U"Table with differences between means", false)
	BOOLEAN (wantTukeyPostHoc, U"Table with Tukey's post-hoc HSD test", false)
	OK
DO
	INFO_ONE (Table)
		const integer factorColumn = Table_columnNameToNumber_e (me, factor_string);
		const integer dataColumn = Table_columnNameToNumber_e (me, dataColumn_string);
		autoTable means, meansDiff, meansDiffProbabilities;
		autoTable anova = Table_getOneWayAnalysisOfVarianceF (me, dataColumn, factorColumn, & means,
			& meansDiff, & meansDiffProbabilities);
		MelderInfo_open ();
		MelderInfo_writeLine (U"One-way analysis of \"", dataColumn_string, U"\" by \"", factor_string, U"\".\n");
		Table_printAsAnovaTable (anova.get());
		MelderInfo_writeLine (U"\nMeans:\n");
		print_means (means.get());
		MelderInfo_close ();
		if (wantMeans)
			praat_new (means.move(), my name.get(), U"_groupMeans");
		if (wantDifferencesBetweenMeans)
			praat_new (meansDiff.move(), my name.get(), U"_meansDiff");
		if (wantTukeyPostHoc)
			praat_new (meansDiffProbabilities.move(), my name.get(), U"_meansDiffP");
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportTwoWayAnova, U"Table: Report two-way anova", U"Table: Report two-way anova...") {
	SENTENCE (dataColumn_string, U"Column with data", U"Data")
	SENTENCE (firstFactor_string, U"First factor", U"A")
	SENTENCE (secondFactor_string, U"Second factor", U"B")
	BOOLEAN (wantMeans, U"Table with means", false);
	OK
DO
	INFO_ONE (Table)
		const integer firstFactorColumn = Table_columnNameToNumber_e (me, firstFactor_string);
		const integer secondFactorColumn = Table_columnNameToNumber_e (me, secondFactor_string);
		const integer dataColumn = Table_columnNameToNumber_e (me, dataColumn_string);
		autoTable means, sizes;
		autoTable anova = Table_getTwoWayAnalysisOfVarianceF (me, dataColumn, firstFactorColumn, secondFactorColumn, &means, &sizes);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Two-way analysis of \"", dataColumn_string, U"\" by \"", firstFactor_string, U"\" and \"", secondFactor_string, U"\".\n");
			Table_printAsAnovaTable (anova.get());
			MelderInfo_writeLine (U"\nMeans:\n");
			Table_printAsMeansTable (means.get());
			MelderInfo_writeLine (U"\nCell sizes:\n");
			Table_printAsMeansTable (sizes.get());
		MelderInfo_close ();
		if (wantMeans)
			praat_new (means.move(), my name.get(), U"_groupMeans");
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportOneWayKruskalWallis, U"Table: Report one-way Kruskal-Wallis", U"Table: Report one-way Kruskal-Wallis...") {
	SENTENCE (dataColumn_string, U"Column with data", U"Data")
	SENTENCE (factor_string, U"Factor", U"Group")
	OK
DO
	INFO_ONE (Table)
		const integer factorColumn = Table_columnNameToNumber_e (me, factor_string);
		const integer dataColumn = Table_columnNameToNumber_e (me, dataColumn_string);
		double ndof, kruskalWallis, prob;
		autoTable result = Table_getOneWayKruskalWallis (me, dataColumn, factorColumn, & prob, & kruskalWallis, & ndof);
		MelderInfo_open ();
			MelderInfo_writeLine (U"One-way Kruskal-Wallis of \"", dataColumn_string, U"\" by \"", factor_string, U"\".\n");
			MelderInfo_writeLine (U"Chi squared: ", kruskalWallis);
			MelderInfo_writeLine (U"Degrees of freedom: ", ndof);
			MelderInfo_writeLine (U"Probability: ", prob);
			MelderInfo_writeLine (U"\nMeans:\n");
			print_means (result.get());
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (CONVERT_EACH_TO_ONE__Table_to_StringsIndex_column, U"Table: To StringsIndex (column)", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		const integer icol = Table_columnNameToNumber_e (me, columnLabel);
		autoStringsIndex result = Table_to_StringsIndex_column (me, icol, kStrings_sorting::NUMBER_AWARE);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", columnLabel)
}

/******************* LegendreSeries *********************************/

FORM (CREATE_ONE__LegendreSeries_create, U"Create LegendreSeries", U"Create LegendreSeries...") {
	WORD (name, U"Name", U"ls")
	COMMENT (U"Domain")
	REAL (xmin, U"Xmin", U"-1")
	REAL (xmax, U"Xmax", U"1")
	COMMENT (U"LegendreSeries(x) = c[1] P[0](x) + c[2] P[1](x) + ... c[n+1] P[n](x)")
	COMMENT (U"P[k] is a Legendre polynomial of degree k")
	REALVECTOR (coefficients, U"Coefficients", WHITESPACE_SEPARATED_, U"0 0 1.0")
	OK
DO
	Melder_require (xmin < xmax, 
		U"Xmin should be smaller than Xmax.");
	CREATE_ONE
		autoLegendreSeries result = LegendreSeries_createWithCoefficients (xmin, xmax, coefficients);
	CREATE_ONE_END (name)
}

DIRECT (HELP__LegendreSeries_help) {
	HELP (U"LegendreSeries")
}

DIRECT (CONVERT_EACH_TO_ONE__LegendreSeries_to_Polynomial) {
	CONVERT_EACH_TO_ONE (LegendreSeries)
		autoPolynomial result = LegendreSeries_to_Polynomial (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************* LongSound **************************************/

FORM_READ (APPEND_ALL__LongSounds_appendToExistingSoundFile, U"LongSound: Append to existing sound file", 0, false) {
	APPEND_ALL (Sampled, U"append the selected LongSound object(s) to the existing sound file")
		LongSounds_appendToExistingSoundFile (& list, file);
	APPEND_ALL_END
}

FORM_SAVE (SAVE_TWO__LongSounds_saveAsStereoAiffFile, U"LongSound: Save as AIFF file", 0, U"aiff") {
	SAVE_TWO (LongSound, U"save the selected LongSound objects to the AIFF file")
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_AIFF, file);
	SAVE_TWO_END
}

FORM_SAVE (SAVE_TWO__LongSounds_saveAsStereoAifcFile, U"LongSound: Save as AIFC file", 0, U"aifc") {
	SAVE_TWO (LongSound, U"save the selected LongSound objects to the AIFC file")
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_AIFC, file);
	SAVE_TWO_END
}

FORM_SAVE (SAVE_TWO__LongSounds_saveAsStereoWavFile, U"LongSound: Save as WAV file", 0, U"wav") {
	SAVE_TWO (LongSound, U"save the selected LongSound objects to the WAV file")
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_WAV, file);
	SAVE_TWO_END
}

FORM_SAVE (SAVE_TWO__LongSounds_saveAsStereoNextSunFile, U"LongSound: Save as NeXT/Sun file", 0, U"au") {
	SAVE_TWO (LongSound, U"save the selected LongSound objects to the NeXT/Sun file")
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_NEXT_SUN, file);
	SAVE_TWO_END
}

FORM_SAVE (SAVE_TWO__LongSounds_saveAsStereoNistFile, U"LongSound: Save as NIST file", 0, U"nist") {
	SAVE_TWO (LongSound, U"save the selected LongSound objects to the NIST file")
		LongSounds_writeToStereoAudioFile16 (me, you, Melder_NIST, file);
	SAVE_TWO_END
}

/******************* Matrix **************************************************/

FORM (GRAPHICS_EACH__Matrix_drawAsSquares, U"Matrix: Draw as squares", U"Matrix: Draw as squares...") {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawAsSquares (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Matrix_drawDistribution, U"Matrix: Draw distribution", U"Matrix: Draw distribution...") {
	COMMENT (U"Selection of (part of) Matrix")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	COMMENT (U"Selection of Matrix values")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	COMMENT (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimumFrequency, U"Minimum frequency", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawDistribution (me, GRAPHICS, xmin, xmax, ymin, ymax, minimumValue, maximumValue, 
			numberOfBins, minimumFrequency, maximumFrequency, false, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Matrix_drawCumulativeDistribution, U"Matrix: Draw cumulative distribution", nullptr) {
	COMMENT (U"Selection of (part of) Matrix")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	COMMENT (U"Selection of Matrix values")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	COMMENT (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawDistribution (me, GRAPHICS, xmin, xmax, ymin, ymax, minimumValue, maximumValue, 
			numberOfBins, minimum, maximum, true, garnish
		);
	GRAPHICS_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__Matrix_getMean, U"Matrix: Get mean", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		const double result = Matrix_getMean (me, xmin, xmax, ymin, ymax);
	QUERY_ONE_FOR_REAL_END (U" (mean)")
}

FORM (QUERY_ONE_FOR_REAL__Matrix_getStandardDeviation, U"Matrix: Get standard deviation", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		const double result = Matrix_getStandardDeviation (me, xmin, xmax, ymin, ymax);
	QUERY_ONE_FOR_REAL_END (U" (std dev)")
}

FORM (QUERY_ONE_FOR_REAL__Matrix_getNorm, U"Matrix: Get norm", U"Matrix: Get norm...") {
	REAL (power, U"Power", U"2.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		const double result = NUMnorm (my z.all(), power);
	QUERY_ONE_FOR_REAL_END (U" (norm with power = ", power, U")")
}

FORM (MODIFY_Matrix_scale, U"Matrix: Scale", nullptr) {
	COMMENT (U"self[row, col] := self[row, col] / `Scale factor'")
	CHOICE (scaleMethod, U"Scale factor", 1)
		OPTION (U"extremum in matrix")
		OPTION (U"extremum in each row")
		OPTION (U"extremum in each column")
	OK
DO
	MODIFY_EACH (Matrix)
		Matrix_scale (me, scaleMethod);
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__Matrix_transpose) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoMatrix result = Matrix_transpose (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_transposed")
}

DIRECT (CONVERT_EACH_TO_ONE__Matrix_to_PCA_byColumns) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoPCA result = Matrix_to_PCA_byColumns (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_columns");
}

DIRECT (CONVERT_EACH_TO_ONE__Matrix_to_PCA_byRows) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoPCA result = Matrix_to_PCA_byRows (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rows")
}

FORM (CONVERT_EACH_TO_ONE__Matrix_solveEquation, U"Matrix: Solve equation", U"Matrix: Solve equation...") {
	REAL (tolerance, U"Tolerance", U"1.0e-7")
	OK
DO
	CONVERT_EACH_TO_ONE (Matrix)
		autoMatrix result = Matrix_solveEquation (me, tolerance);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_solution")
}

FORM (CONVERT_TWO_TO_ONE__Matrix_solveMatrixEquation, U"Matrix: Solve matrix equation", U"Matrix: Solve matrix equation...") {
	REAL (tolerance, U"Tolerance", U"1.0e-7")
	OK
DO
	CONVERT_TWO_TO_ONE (Matrix)
		autoMatrix result = Matrix_solveEquation (me, you, tolerance);
	CONVERT_TWO_TO_ONE_END (U"solution")
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__Matrix_Categories_to_TableOfReal) {
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE (Categories, Matrix)
		autoTableOfReal result = Matrix_Categories_to_TableOfReal (you, me);
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__ActivationList_Categories_to_TableOfReal) {
	CONVERT_ONE_AND_ONE_TO_ONE (ActivationList, Categories)
		autoTableOfReal result = Matrix_Categories_to_TableOfReal (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (GRAPHICS_EACH__Matrix_scatterPlot, U"Matrix: Scatter plot", nullptr) {
	NATURAL (xColumn, U"Column for X-axis", U"1")
	NATURAL (yColumn, U"Column for Y-axis", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO

	GRAPHICS_EACH (Matrix)
		Matrix_scatterPlot (me, GRAPHICS, xColumn, yColumn, xmin, xmax, ymin, ymax, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__Matrix_to_ActivationList) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoActivationList result = Matrix_to_ActivationList (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Matrix_to_Eigen) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoEigen result = Matrix_to_Eigen (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Matrix_to_Eigen_special, U"Matrix: To Eigen", U"Matrix: To Eigen") {
	OPTIONMENU_ENUM (kMAT_TYPE, matType, U"Matrix type", kMAT_TYPE::SYMMETRIC)
	INTEGER (numberOfEigenvalues, U"Number of eigenvalues", U"0 (=all)")
	BOOLEAN (sortAscending, U"Sort ascending", 0)
	OK
DO
	CONVERT_EACH_TO_ONE (Matrix)
		autoEigen result = Matrix_to_Eigen_special (me, matType, numberOfEigenvalues, sortAscending);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Matrix_to_SVD) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoSVD result = SVD_createFromGeneralMatrix (my z.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_MULTIPLE_Matrix_eigen_complex) {
	CONVERT_EACH_TO_MULTIPLE (Matrix)
		autoMatrix vectors, values;
		Matrix_Eigen_complex (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	CONVERT_EACH_TO_MULTIPLE_END
}

FORM (CONVERT_EACH_TO_ONE__Matrix_to_NMF_mu, U"Matrix: To NMF (m.u.)", U"Matrix: To NMF (m.u.)...") {
	NATURAL (numberOfFeatures, U"Number of features", U"2")
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"400")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	OPTIONMENU_ENUM (kNMF_Initialization, initializationMethod, U"Initialisation method", kNMF_Initialization::RANDOM_UNIFORM)
	BOOLEAN (info, U"Info", 0)
	OK
DO
	Melder_require (maximumNumberOfIterations >= 0,
		U"The maximum number of iterations should not be negative.");
	CONVERT_EACH_TO_ONE (Matrix)
		autoNMF result = Matrix_to_NMF_mu (me, numberOfFeatures, maximumNumberOfIterations, tolx, told, initializationMethod, info);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_mu")
}

FORM (CONVERT_EACH_TO_ONE__Matrix_to_NMF_als, U"Matrix: To NMF (ALS)", U"Matrix: To NMF (ALS)...") {
	NATURAL (numberOfFeatures, U"Number of features", U"2")
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"20")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	OPTIONMENU_ENUM (kNMF_Initialization, initializationMethod, U"Initialisation method", kNMF_Initialization::RANDOM_UNIFORM)
	BOOLEAN (info, U"Info", 0)
	OK
DO
	Melder_require (maximumNumberOfIterations >= 0,
		U"The maximum number of iterations should not be negative.");
	CONVERT_EACH_TO_ONE (Matrix)
		autoNMF result = Matrix_to_NMF_als (me, numberOfFeatures, maximumNumberOfIterations, tolx, told, initializationMethod, info);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_als")
}

FORM (CONVERT_EACH_TO_ONE__Matrix_to_NMF_is, U"Matrix: To NMF (IS)", U"Matrix: To NMF (IS)...") {
	NATURAL (numberOfFeatures, U"Number of features", U"2")
	INTEGER (maximumNumberOfIterations, U"Maximum number of iterations", U"20")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	OPTIONMENU_ENUM (kNMF_Initialization, initializationMethod, U"Initialisation method", kNMF_Initialization::RANDOM_UNIFORM)
	BOOLEAN (info, U"Info", 0)
	OK
DO
	Melder_require (maximumNumberOfIterations >= 0,
		U"The maximum number of iterations should not be negative.");
	CONVERT_EACH_TO_ONE (Matrix)
		autoNMF result = Matrix_to_NMF_is (me, numberOfFeatures, maximumNumberOfIterations, tolx, told, initializationMethod, info);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_als")
}

DIRECT (QUERY_ONE_AND_ONE_FOR_REAL__NMF_Matrix_getEuclideanDistance) {
	QUERY_ONE_AND_ONE_FOR_REAL (NMF, Matrix)
		const double result = NMF_getEuclideanDistance (me, your z.get());
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" (= ", result / (your ny * your nx), U" * nrow * ncol)")
}

DIRECT (QUERY_ONE_AND_ONE_FOR_REAL__NMF_Matrix_getItakuraSaitoDivergence) {
	QUERY_ONE_AND_ONE_FOR_REAL (NMF, Matrix)
		const double result = NMF_getItakuraSaitoDivergence (me, your z.get());
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" (= ", result / (your ny * your nx), U" * nrow * ncol)")
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__NMF_Matrix_improveFactorization_mu, U"NMF & Matrix: Improve factorization (m.u.)", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	BOOLEAN (info, U"Info", 0)
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (NMF, Matrix)
		NMF_improveFactorization_mu (me, your z.get(), maximumNumberOfIterations, tolx, told, info);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__NMF_Matrix_improveFactorization_als, U"NMF & Matrix: Improve factorization (ALS)", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"10")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	BOOLEAN (info, U"Info", 0)
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (NMF, Matrix)
		NMF_improveFactorization_als (me, your z.get(), maximumNumberOfIterations, tolx, told, info);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__NMF_Matrix_improveFactorization_is, U"NMF & Matrix: Improve factorization (IS)", nullptr) {
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"10")
	REAL (tolx, U"Change tolerance", U"1e-9")
	REAL (told, U"Approximation tolerance", U"1e-9")
	BOOLEAN (info, U"Info", 0)
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (NMF, Matrix)
		NMF_improveFactorization_is (me, your z.get(), maximumNumberOfIterations, tolx, told, info);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (CONVERT_TWO_TO_ONE__Matrices_to_DTW, U"Matrices: To DTW", U"Matrix: To DTW...") {
	COMMENT (U"Distance  between cepstral coefficients")
	REAL (distanceMetric, U"Distance metric", U"2.0")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_TWO_TO_ONE (Matrix)
		autoDTW result = Matrices_to_DTW (me, you, matchStart, matchEnd, slopeConstraint, distanceMetric);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__Matrix_to_PatternList, U"Matrix: To PatternList", nullptr) {
	NATURAL (join, U"Join", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Matrix)
		autoPatternList result = Matrix_to_PatternList (me, join);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/**** Filterbank (deprecated) *******/

DIRECT (QUERY_ONE_FOR_REAL__FilterBank_getHighestFrequency) {
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = my ymax;
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (QUERY_ONE_FOR_REAL__FilterBank_getLowestFrequency) {
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = my ymin;
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (QUERY_ONE_FOR_INTEGER__FilterBank_getNumberOfFrequencies) {
	QUERY_ONE_FOR_INTEGER (FilterBank)
		const integer result = my ny;
	QUERY_ONE_FOR_INTEGER_END (U"")
}

DIRECT (QUERY_ONE_FOR_REAL__FilterBank_getFrequencyDistance) {
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = my dy;
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

FORM (QUERY_ONE_FOR_REAL__FilterBank_getTimeFromColumn, U"Get time of column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = Matrix_columnToX (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__FilterBank_getFrequencyFromRow, U"Get frequency of row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = Matrix_rowToY (me, rowNumber);
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

static double Matrix_getValueAtNearestColRow (Matrix me, double x, double y) {
	/*
		Compatibility with old behaviour. Having real values as arguments would suggest interpolation which is not done.
	*/
	if ((y >= my ymin && y <= my ymax) && (x >= my xmin && x <= my xmax)) {
		integer icol = Matrix_xToNearestColumn (me, x);
		Melder_clip (1_integer, & icol, my nx);
		integer irow = Matrix_yToNearestRow (me, y);
		Melder_clip (1_integer, & irow, my ny);
		return my z [irow] [icol];
	} else
		return undefined;	
}

FORM (QUERY_ONE_FOR_REAL__FilterBank_getValueInCell, U"Get value in cell", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	POSITIVE (frequency, U"Frequency", U"1.0")
	OK
DO
	QUERY_ONE_FOR_REAL (FilterBank)
		const double result = Matrix_getValueAtNearestColRow (me, time, frequency);
	QUERY_ONE_FOR_REAL_END (U"")
}

/***** MATRIXFT *************/

DIRECT (QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getHighestFrequency) {
	QUERY_ONE_FOR_REAL (BandFilterSpectrogram)
		const double result = my ymax;
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getLowestFrequency) {
	QUERY_ONE_FOR_REAL (BandFilterSpectrogram)
		const double result = my ymin;
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

DIRECT (QUERY_ONE_FOR_INTEGER__BandFilterSpectrogram_getNumberOfFrequencies) {
	QUERY_ONE_FOR_INTEGER (BandFilterSpectrogram)
		const integer result = my ny;
	QUERY_ONE_FOR_INTEGER_END (U"")
}

DIRECT (QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getFrequencyDistance) {
	QUERY_ONE_FOR_REAL (BandFilterSpectrogram)
		const double result = my dy;
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

FORM (QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getFrequencyFromRow, U"Get frequency of row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (BandFilterSpectrogram)
		const double result = Matrix_rowToY (me, rowNumber);
	QUERY_ONE_FOR_REAL_END (U" ", my v_getFrequencyUnit ())
}

FORM (QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getTimeFromColumn, U"Get time of column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (BandFilterSpectrogram)
		const double result = Matrix_columnToX (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" s")
}

FORM (QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getValueInCell, U"Get value in cell", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	POSITIVE (frequency, U"Frequency", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (BandFilterSpectrogram)
		const double result = Matrix_getValueAtNearestColRow (me, time, frequency);
	QUERY_ONE_FOR_REAL_END (U"")
}

/**************** MelFilter *******************************************/

DIRECT (HELP__MelFilter_help) {
	HELP (U"MelFilter")
}

DIRECT (HELP__MelSpectrogram_help) {
	HELP (U"MelSpectrogram")
}

FORM (GRAPHICS_EACH__MelFilter_drawFilterFunctions, U"MelFilter: Draw filter functions", U"FilterBank: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	CHOICE (frequencyScale, U"Frequency scale", 1)
		OPTION (U"Hertz")
		OPTION (U"Bark")
		OPTION (U"mel")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (dBScale, U"Amplitude scale in dB", false)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelFilter)
		MelFilter_drawFilterFunctions (me, GRAPHICS, frequencyScale, fromFilter, toFilter, 
			fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__MelSpectrogram_drawTriangularFilterFunctions, U"MelSpectrogram: Draw triangulat filter functions", U"MelSpectrogram: Draw filter functions...") {
	INTEGER (fromFilter, U"left Filter range", U"0")
	INTEGER (toFilter, U"right Filter range", U"0")
	CHOICE (frequencyScale, U"Frequency scale", 1)
		OPTION (U"mel")
		OPTION (U"Hertz")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right Frequency range", U"0.0")
	BOOLEAN (dBScale, U"Amplitude scale in dB", false)
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		MelSpectrogram_drawTriangularFilterFunctions (me, GRAPHICS, frequencyScale - 1, fromFilter, toFilter, 
			fromFrequency, toFrequency, dBScale, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__MelFilter_drawSpectrum, U"MelFilter: Draw spectrum (slice)", U"FilterBank: Draw spectrum (slice)...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FilterBank)
		FilterBank_drawTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, fromAmplitude, toAmplitude, U"Mels", garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__MelSpectrogram_drawSpectrumAtNearestTimeSlice, U"MelSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, time, fromFrequency, toFrequency, 
			fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__BarkSpectrogram_drawSpectrumAtNearestTimeSlice, U"BarkSpectrogram: Draw spectrum at nearest time slice", U"BandFilterSpectrogram: Draw spectrum at nearest time slice...") {
	REAL (time, U"Time (s)", U"0.1")
	REAL (fromFrequency, U"left Frequency range (bark)", U"0.0")
	REAL (toFrequency, U"right Frequency range (bark)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (MelSpectrogram)
		BandFilterSpectrogram_drawSpectrumAtNearestTimeSlice (me, GRAPHICS, time, 
			fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__MelFilter_paint, U"FilterBank: Paint", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"0.0")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (MelFilter)
		FilterBank_paint (me, GRAPHICS, 
			fromTime, toTime, fromFrequency, toFrequency, fromAmplitude, toAmplitude, garnish
		);
	GRAPHICS_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__MelFilter_to_MFCC, U"MelFilter: To MFCC", U"MelSpectrogram: To MFCC...") {
	NATURAL (numberOfCoefficients, U"Number of coefficients", U"12")
	OK
DO
	CONVERT_EACH_TO_ONE (MelFilter)
		autoMFCC result = MelFilter_to_MFCC (me, numberOfCoefficients);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__MelSpectrogram_to_MFCC, U"MelSpectrogram: To MFCC", U"MelSpectrogram: To MFCC...") {
	NATURAL (numberOfCoefficients, U"Number of coefficients", U"12")
	OK
DO
	CONVERT_EACH_TO_ONE (MelSpectrogram)
		autoMFCC result = MelSpectrogram_to_MFCC (me, numberOfCoefficients);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/**************** Ltas *******************************************/

#include "../kar/UnicodeData.h"
FORM (INFO_ONE__Ltas_reportSpectralTrend, U"Ltas: Report spectral trend", nullptr) {
	POSITIVE (fromFrequency, U"left Frequency range (Hz)", U"100.0")
	POSITIVE (toFrequency, U"right Frequency range (Hz)", U"5000.0")
	OPTIONMENU (frequencyScale, U"Frequency scale", 1)
		OPTION (U"linear")
		OPTION (U"logarithmic")
	OPTIONMENU (fitMethod, U"Fit method", 2)
		OPTION (U"least squares")
		OPTION (U"robust")
	OK
DO
	bool logScale = frequencyScale == 2;
	INFO_ONE (Ltas)
		double a, b;
		Ltas_fitTrendLine (me, fromFrequency, toFrequency, logScale, fitMethod, &a, &b);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Spectral model: amplitude_dB(frequency_Hz) " UNITEXT_ALMOST_EQUAL_TO " ", logScale ? U"offset + slope * log (frequency_Hz)" : U"offset + slope * frequency_Hz");
			MelderInfo_writeLine (U"Slope: ", a, logScale ? U" dB/decade" : U" dB/Hz");
			MelderInfo_writeLine (U"Offset: ", b, U" dB");
		MelderInfo_close ();
	INFO_ONE_END
}


/**************** MFCC *******************************************/

DIRECT (HELP__MFCC_help) {
	HELP (U"MFCC")
}

FORM (CONVERT_EACH_TO_ONE__MFCC_to_MelFilter, U"MFCC: To MelFilter", nullptr) {
	INTEGER (fromCoefficient, U"From coefficient", U"0")
	INTEGER (toCoefficient, U"To coefficient", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (MFCC)
		autoMelFilter result = MFCC_to_MelFilter (me, fromCoefficient, toCoefficient);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__MFCC_to_MelSpectrogram, U"MFCC: MelSpectrogram", U"MFCC: To MelSpectrogram...") {
	INTEGER (fromCoefficient, U"From coefficient", U"0")
	INTEGER (toCoefficient, U"To coefficient", U"0")
	BOOLEAN (includeConstant, U"Include constant term", true)
	OK
DO
	CONVERT_EACH_TO_ONE (MFCC)
		autoMelSpectrogram result = MFCC_to_MelSpectrogram (me, fromCoefficient, toCoefficient, includeConstant);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__MFCC_to_TableOfReal, U"MFCC: To TableOfReal", U"MFCC: To TableOfReal...") {
	BOOLEAN (includeEnergy, U"Include energy", false)
	OK
DO
	CONVERT_EACH_TO_ONE (MFCC)
		autoTableOfReal result = MFCC_to_TableOfReal (me, includeEnergy);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__MFCC_to_Matrix_features, U"MFCC: To Matrix (features)", nullptr) {
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	BOOLEAN (includeEnergy, U"Include energy", false)
	OK
DO
	CONVERT_EACH_TO_ONE (MFCC)
		autoMatrix result = MFCC_to_Matrix_features (me, windowLength, includeEnergy);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_TWO_TO_ONE__MFCCs_crossCorrelate, U"MFCC & MFCC: Cross-correlate", nullptr) {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (MFCC)
		autoSound result = MFCCs_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_",your name.get())
}

FORM (CONVERT_TWO_TO_ONE__MFCCs_convolve, U"MFCC & MFCC: Convolve", nullptr) {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (MFCC)
		autoSound result = MFCCs_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_",your name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__MFCC_to_Sound) {
	CONVERT_EACH_TO_ONE (MFCC)
		autoSound result = MFCC_to_Sound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/**************** MSpline *******************************************/

FORM (CREATE_ONE__MSpline_create, U"Create MSpline", U"Create MSpline...") {
	WORD (name, U"Name", U"mspline")
	COMMENT (U"Domain")
	REAL (xmin, U"Xmin", U"0")
	REAL (xmax, U"Xmax", U"1")
	COMMENT (U"MSpline(x) = c[1] M[1](x) + c[2] M[1](x) + ... c[n] M[n](x)")
	COMMENT (U"all M[k] are polynomials of degree \"Degree\"")
	COMMENT (U"Relation: numberOfCoefficients == numberOfInteriorKnots + degree + 1")
	INTEGER (degree, U"Degree", U"2")
	REALVECTOR (coefficients, U"Coefficients (c[k])", WHITESPACE_SEPARATED_, U"1.2 2.0 1.2 1.2 3.0 0.0")
	REALVECTOR (interiorKnots, U"Interior knots", WHITESPACE_SEPARATED_, U"0.3 0.5 0.6")
	OK
DO
	Melder_require (xmin < xmax, 
		U"\"Xmin\" should be smaller than \"Xmax\".");
	CREATE_ONE
		autoMSpline result = MSpline_createWithCoefficients (xmin, xmax, degree, coefficients, interiorKnots);
	CREATE_ONE_END (name)
}

DIRECT (HELP__MSpline_help) {
	HELP (U"MSpline")
}

FORM (MODIFY_EACH__NavigationContext_modifyUseCriterion, U"NavigationContext: Modify use criterion", nullptr) {
	OPTIONMENU_ENUM (kContext_combination, combinationCriterion, U"Use criterion", kContext_combination::DEFAULT)
	BOOLEAN (excludeTopic, U"Exclude topic", false)
	OK
DO
	MODIFY_EACH (NavigationContext)
		NavigationContext_modifyUseCriterion (me, combinationCriterion, excludeTopic);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__NavigationContext_modifyTopicCriterion, U"NavigationContext: Modify Topic match criterion", nullptr) {
	OPTIONMENU_ENUM (kMelder_string, criterion, U"Match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, topicMatchBoolean, U"Combine topic matches with", kMatchBoolean::OR_)
	OK
DO
	MODIFY_EACH (NavigationContext)
		NavigationContext_modifyTopicCriterion (me, criterion, topicMatchBoolean);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__NavigationContext_modifyBeforeCriterion, U"NavigationContext: Modify Before match criterion", nullptr) {
	OPTIONMENU_ENUM (kMelder_string, criterion, U"Match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, beforeMatchBoolean, U"Combine before matches with", kMatchBoolean::OR_)
	OK
DO
	MODIFY_EACH (NavigationContext)
		NavigationContext_modifyBeforeCriterion (me, criterion, beforeMatchBoolean);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__NavigationContext_modifyAfterCriterion, U"NavigationContext: Modify After match criterion", nullptr) {
	OPTIONMENU_ENUM (kMelder_string, criterion, U"Match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, afterMatchBoolean, U"Combine after matches with", kMatchBoolean::OR_)
	OK
DO
	MODIFY_EACH (NavigationContext)
		NavigationContext_modifyAfterCriterion (me, criterion, afterMatchBoolean);
	MODIFY_EACH_END
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__NavigationContext_replaceTopicLabels) {
	MODIFY_FIRST_OF_ONE_AND_ONE (NavigationContext, Strings)
		NavigationContext_replaceTopicLabels (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__NavigationContext_replaceBeforeLabels) {
	MODIFY_FIRST_OF_ONE_AND_ONE (NavigationContext, Strings)
		NavigationContext_replaceBeforeLabels (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__NavigationContext_replaceAfterLabels) {
	MODIFY_FIRST_OF_ONE_AND_ONE (NavigationContext, Strings)
		NavigationContext_replaceAfterLabels (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}


FORM (CONVERT_ONE_AND_ONE_TO_ONE__TextGrid_and_NavigationContext_to_TextGridNavigator, U"TextGrid & NavigationContext: To TextGridNavigator", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (TextGrid, NavigationContext)
		autoTextGridNavigator result = TextGrid_and_NavigationContext_to_TextGridNavigator (me, you, tierNumber, matchDomain);
	CONVERT_ONE_AND_ONE_TO_ONE_END (U"tgn")
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__TextGrid_and_NavigationContext_to_TextGridTierNavigator, U"TextGrid & NavigationContext: To TextGridTierNavigator", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (TextGrid, NavigationContext)
		autoTextGridTierNavigator result = TextGrid_and_NavigationContext_to_TextGridTierNavigator (me, you, tierNumber, matchDomain);
	CONVERT_ONE_AND_ONE_TO_ONE_END (U"ttgn_", tierNumber)
}

DIRECT (HELP__NMF_help) {
	HELP (U"NMF")
}

FORM (GRAPHICS_EACH__NMF_paintFeatures, U"NMF: Paint features", U"") {
	NATURAL (fromFeature, U"From feature", U"1")
	INTEGER (toFeature, U"To feature", U"0 (= all)")
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (NMF)
		NMF_paintFeatures (me, GRAPHICS, fromFeature, toFeature, fromRow, toRow, minimum,  maximum, 0, 0, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__NMF_paintWeights, U"NMF: Paint weights", U"") {
	NATURAL (fromWeight, U"From weight", U"1")
	INTEGER (toWeight, U"To weight", U"0 (= all)")
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"maximum", U"0.0")	
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (NMF)
		NMF_paintWeights (me, GRAPHICS, fromWeight, toWeight, fromRow, toRow, minimum,  maximum, 0, 0, garnish);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__NMF_to_Matrix) {
	CONVERT_EACH_TO_ONE (NMF)
		autoMatrix result = NMF_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************** PatternList *******************************************/

DIRECT (CONVERT_ONE_AND_ONE__TO_ONE_PatternList_Categories_to_Discriminant) {
	CONVERT_ONE_AND_ONE_TO_ONE (PatternList, Categories)
		autoDiscriminant result = PatternList_Categories_to_Discriminant (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (GRAPHICS_EACH__PatternList_draw, U"PatternList: Draw", nullptr) {
	NATURAL (patternNumber, U"Pattern number", U"1")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PatternList)
		PatternList_draw (me, GRAPHICS, patternNumber, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__PatternList_getNumberOfPatterns) {
	QUERY_ONE_FOR_INTEGER (PatternList)
		const integer result = my ny;
	QUERY_ONE_FOR_INTEGER_END (U" (number of patterns)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__PatternList_getPatternSize) {
	QUERY_ONE_FOR_INTEGER (PatternList)
		const integer result = my nx;
	QUERY_ONE_FOR_INTEGER_END (U" (pattern size)")
}

FORM (QUERY_ONE_FOR_REAL__PatternList_getValue, U"", nullptr) {
	NATURAL (patternNumber, U"Pattern number", U"1")
	NATURAL (nodeNumber, U"Node number", U"2")
	OK
DO
	QUERY_ONE_FOR_REAL (PatternList)
		const double result = ( patternNumber <= my ny && nodeNumber <= my nx ? my z [patternNumber] [nodeNumber] : undefined );
	QUERY_ONE_FOR_REAL_END (U"")
}

DIRECT (NUMMAT_PatternList_getAllValues) {
	QUERY_ONE_FOR_MATRIX (PatternList)
		autoMAT result = copy_MAT (my z.all());
	QUERY_ONE_FOR_MATRIX_END
}

FORM (MODIFY_PatternList_formula, U"PatternList: Formula", nullptr) {
	COMMENT (U"# `col` is the node number, `row` is the pattern number")
	COMMENT (U"for row from 1 to nrow   ; for all patterns")
	COMMENT (U"   for col from 1 to ncol   ; for all nodes")
	COMMENT (U"      self [row, col] =")
	FORMULA (formula, U"Formula", U"5 * exp (-0.5 * ((col - 10 - row/100) / 1.5) ^ 2) - 0.5   ; sliding peak")
	COMMENT (U"   endfor")
	COMMENT (U"endfor")
	OK
DO
	MODIFY_EACH_WEAK (PatternList)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_PatternList_setValue, U"PatternList: Set value", U"PatternList: Set value...") {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (newValue, U"New value", U"0.0")
	OK
DO
	MODIFY_EACH (PatternList)
		Melder_require (rowNumber <= my ny, 
			U"The row number should not be greater than the number of rows.");
		Melder_require (columnNumber <= my nx, 
			U"The column number should not be greater than the number of columns.");
		my z [rowNumber] [columnNumber] = newValue;
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__PatternList_to_Matrix) {
	CONVERT_EACH_TO_ONE (PatternList)
		autoMatrix result = PatternList_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/******************* PCA ******************************/

DIRECT (HELP__PCA_help) {
	HELP (U"PCA")
}

DIRECT (HINT__hint_PCA_TableOfReal_to_Configuration) {
	INFO_NONE
		Melder_information (U"You can get principal components by selecting a PCA and a TableOfReal\n"
			"together and choosing \"To Configuration...\".");
	INFO_NONE_END
}

DIRECT (HINT_hint__PCA_Covariance_Project) {
	INFO_NONE
		Melder_information (U"You can get a new Covariance object rotated to the directions of the direction vectors\n"
			" in the PCA object by selecting a PCA and a Covariance object together.");
	INFO_NONE_END
}

DIRECT (HINT_hint__PCA_Configuration_to_TableOfReal_reconstruct) {
	INFO_NONE
		Melder_information (U"You can reconstruct the original TableOfReal as well as possible from\n"
			" the principal components in the Configuration and the direction vectors in the PCA object.");
	INFO_NONE_END
}

FORM (QUERY_ONE_AND_ONE_FOR_REAL__PCA_TableOfReal_getFractionVariance, U"PCA & TableOfReal: Get fraction variance", U"PCA & TableOfReal: Get fraction variance...") {
	NATURAL (fromPrincipalComponent, U"left Principal component range", U"1")
	NATURAL (toPrincipalComponent, U"right Principal component range", U"1")
	OK
DO
	QUERY_ONE_AND_ONE_FOR_REAL (PCA, TableOfReal)
		const double result = PCA_TableOfReal_getFractionVariance (me, you, fromPrincipalComponent, toPrincipalComponent);
	QUERY_ONE_AND_ONE_FOR_REAL_END (U"")
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__PCA_Configuration_to_TableOfReal_reconstruct) {
	CONVERT_ONE_AND_ONE_TO_ONE (PCA, Configuration)
		autoTableOfReal result = PCA_Configuration_to_TableOfReal_reconstruct (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__PCA_TableOfReal_to_TableOfReal_projectRows, U"PCA & TableOfReal: To TableOfReal (project rows)", U"PCA & TableOfReal: To Configuration...") {
	INTEGER (numberOfDimensionsToKeep, U"Number of dimensions to keep", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensionsToKeep >= 0, U"The number of dimensions to keep should be at least zero.");
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE (PCA, TableOfReal)
		autoTableOfReal result = PCA_TableOfReal_to_TableOfReal_projectRows (me, you, numberOfDimensionsToKeep);
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_GENERIC_TO_ONE__PCA_TableOfReal_to_Configuration, U"PCA & TableOfReal: To Configuration", U"PCA & TableOfReal: To Configuration...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, 
		U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE (PCA, TableOfReal)
		autoConfiguration result = PCA_TableOfReal_to_Configuration (me, you, numberOfDimensions);
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_GENERIC_TO_ONE__PCA_TableOfReal_to_TableOfReal_zscores, U"PCA & TableOfReal: To TableOfReal (z-scores)", U"PCA & TableOfReal: To TableOfReal (z-scores)...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, 
		U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE (PCA, TableOfReal)
		autoTableOfReal result = PCA_TableOfReal_to_TableOfReal_zscores (me, you, numberOfDimensions);
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE_END (my name.get(), U"_", your name.get(), U"_zscores")
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__PCA_Matrix_to_Matrix_projectRows, U"PCA & Matrix: To Matrix (project rows)", nullptr) {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0,
		U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_ONE_TO_ONE (PCA, Matrix)
		autoMatrix result = Eigen_Matrix_to_Matrix_projectRows (me, you, numberOfDimensions);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_GENERIC_TO_ONE__Matrix_to_Matrix_projectColumns, U"PCA & Matrix: To Matrix (project columns)", nullptr) {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, 
		U"The number of dimensions should be at least zero.");
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE (PCA, Matrix)
		autoMatrix result = Eigen_Matrix_to_Matrix_projectColumns (me, you, numberOfDimensions);
	CONVERT_ONE_AND_ONE_GENERIC_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (QUERY_ONE_FOR_REAL__PCA_getCentroidElement, U"PCA: Get centroid element...", nullptr) {
	NATURAL (number, U"Number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (PCA)
		Melder_require (number <= my dimension,
			U"Number should not be greater than ", my dimension, U".");
		const double result = my centroid [number];
	QUERY_ONE_FOR_REAL_END (U" (element ", number, U")")
}

FORM (QUERY_ONE_FOR_REAL__PCA_getEqualityOfEigenvalues, U"PCA: Get equality of eigenvalues", U"PCA: Get equality of eigenvalues...") {
	INTEGER (fromEigenvalue, U"left Eigenvalue range", U"0")
	INTEGER (toEigenvalue, U"right Eigenvalue range", U"0")
	BOOLEAN (conservativeTest, U"Conservative test", false)
	OK
DO
	QUERY_ONE_FOR_REAL (PCA)
		double result, chisq, df;
		PCA_getEqualityOfEigenvalues (me, fromEigenvalue, toEigenvalue,
			conservativeTest, & result, & chisq, & df
		);
	QUERY_ONE_FOR_REAL_END (U" (probability, based on chisq = ", chisq, U" and df = ", df, U")")
}

FORM (QUERY_ONE_FOR_INTEGER__PCA_getNumberOfComponents_VAF, U"PCA: Get number of components (VAF)", U"PCA: Get number of components (VAF)...") {
	POSITIVE (varianceFraction, U"Variance fraction (0-1)", U"0.95")
	OK
DO
	Melder_require (varianceFraction >= 0.0 && varianceFraction <= 1.0, 
		U"The variance fraction should be in the interval [0, 1].");
	QUERY_ONE_FOR_INTEGER (PCA)
		const integer result = Eigen_getDimensionOfFraction (me, varianceFraction);
	QUERY_ONE_FOR_INTEGER_END (U" (for variance fraction)")
}

FORM (QUERY_ONE_FOR_REAL__PCA_getFractionVAF, U"PCA: Get fraction variance accounted for", U"PCA: Get fraction variance accounted for...") {
	NATURAL (fromPrincipalComponent, U"left Principal component range", U"1")
	NATURAL (toPrincipalComponent, U"right Principal component range", U"1")
	OK
DO
	Melder_require (fromPrincipalComponent <= toPrincipalComponent, 
		U"The second component should be greater than or equal to the first component.");
	QUERY_ONE_FOR_REAL (PCA)
		const double result = Eigen_getCumulativeContributionOfComponents (me, fromPrincipalComponent, toPrincipalComponent);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (MODIFY_PCA_invertEigenvector, U"PCA: Invert eigenvector", nullptr) {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	OK
DO
	MODIFY_EACH (Eigen)
		Eigen_invertEigenvector (me, eigenvectorNumber);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__PCA_extractEigenvector, U"PCA: Extract eigenvector", U"Eigen: Extract eigenvector...") {
	NATURAL (eigenvectorNumber, U"Eigenvector number", U"1")
	COMMENT (U"Reshape as")
	INTEGER (numberOfRows, U"Number of rows", U"0")
	INTEGER (numberOfColumns, U"Number of columns", U"0")
	OK
DO
	Melder_require (numberOfRows >= 0, 
		U"The number of rows should be at least 0.");
	Melder_require (numberOfColumns >= 0, 
		U"The number of columns should be at least 0.");
	CONVERT_EACH_TO_ONE (PCA);
		autoMatrix result = Eigen_extractEigenvector (me, eigenvectorNumber, numberOfRows, numberOfColumns);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_ev", eigenvectorNumber)
}

DIRECT (CONVERT_EACH_TO_ONE__PCA_extractEigen) {
	CONVERT_EACH_TO_ONE (PCA)
		autoEigen result = PCA_to_Eigen (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__PCA_to_TableOfReal_reconstruct1, U"PCA: To TableOfReal (reconstruct)", U"PCA: To TableOfReal (reconstruct 1)...") {
	REALVECTOR (coefficients, U"Coefficients", WHITESPACE_SEPARATED_, U"1.0 1.0")
	OK
DO
	CONVERT_EACH_TO_ONE (PCA)
		autoTableOfReal result = PCA_to_TableOfReal_reconstruct1 (me, coefficients);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_reconstructed")
}

FORM (CONVERT_TWO_TO_ONE__PCAs_to_Procrustes, U"PCA & PCA: To Procrustes", U"PCA & PCA: To Procrustes...") {
	NATURAL (fromEigenvector, U"left Eigenvector range", U"1")
	NATURAL (toEigenvector, U"right Eigenvector range", U"2")
	OK
DO
	CONVERT_TWO_TO_ONE (PCA)
		autoProcrustes result = Eigens_to_Procrustes (me, you, fromEigenvector, toEigenvector);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (QUERY_TWO_FOR_REAL__PCAs_getAngleBetweenPc1Pc2Plane_degrees) {
	QUERY_TWO_FOR_REAL (PCA)
		Melder_require (my numberOfEigenvalues > 1, 
			U"There must be at least two eigenvectors in the first PCA.");
		Melder_require (your numberOfEigenvalues > 1, 
			U"There must be at least two eigenvectors in the second PCA.");
		const double result = Eigens_getAngleBetweenEigenplanes_degrees (me, you);
	QUERY_TWO_FOR_REAL_END (U" degrees (angle of intersection between the two pc1-pc2 eigenplanes)")
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__PCA_SSCP_project) {
	CONVERT_ONE_AND_ONE_TO_ONE (PCA, SSCP)
		autoSSCP result = Eigen_SSCP_project (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

/******************* Permutation **************************************/

DIRECT (HELP__Permutation_help) {
	HELP (U"Permutation")
}

FORM (GRAPHICS_EACH__Permutation_drawAsLine, U"", nullptr) {
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Permutation)
		Permutation_drawAsLine (me, GRAPHICS, garnish);
	GRAPHICS_EACH_END
}

FORM (CREATE_ONE__Permutation_create, U"Create Permutation", U"Create Permutation...") {
	WORD (name, U"Name", U"p")
	NATURAL (numberOfElements, U"Number of elements", U"10")
	BOOLEAN (identity, U"Identity Permutation", false)
	OK
DO
	CREATE_ONE
		autoPermutation result = Permutation_create (numberOfElements, identity);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Permutation_createSimplePermutation, U"Permutation: Create simple Permutation", U"") {
	WORD (name, U"Name", U"p")
	INTEGERVECTOR (numbers, U"The Permutation", WHITESPACE_SEPARATED_, U"3 1 2")
	OK
DO
	CREATE_ONE
		autoPermutation result = Permutation_createSimplePermutation (numbers);
	CREATE_ONE_END (name)
}

DIRECT (QUERY_ONE_FOR_INTEGER__Permutation_getNumberOfElements) {
	QUERY_ONE_FOR_INTEGER (Permutation)
		const integer result = my numberOfElements;
	QUERY_ONE_FOR_INTEGER_END (U" (number of elements)")
}

FORM (QUERY_ONE_FOR_INTEGER__Permutation_getValueAtIndex, U"Permutation: Get value", U"Permutation: Get value...") {
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Permutation)
		const integer result = Permutation_getValueAtIndex (me, index);
	QUERY_ONE_FOR_INTEGER_END (U" (value, at index = ", index, U")")
}

DIRECT (QUERY_ONE_FOR_REAL_VECTOR__Permutation_listValues) {
	QUERY_ONE_FOR_REAL_VECTOR (Permutation)
		autoVEC result = raw_VEC (my numberOfElements);
		for (integer i = 1; i <= my numberOfElements; i++)
			result [i] = my p [i];
	QUERY_ONE_FOR_REAL_VECTOR_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__Permutation_getNumberOfInversions) {
	QUERY_ONE_FOR_INTEGER (Permutation)
		const integer result = Permutation_getNumberOfInversions (me);
	QUERY_ONE_FOR_INTEGER_END (U" (inversions)")
}

FORM (QUERY_ONE_FOR_MATRIX__Permutation_listRandomInversions, U"Permutation: Get random inversions", U"Permutation: Get random inversions...") {
	NATURAL (maxNumberOfInversions, U"max. number of inversions", U"1")
	OK
DO
	QUERY_ONE_FOR_MATRIX (Permutation)
		autoMAT result = Permutation_getRandomInversions (me, maxNumberOfInversions);
	QUERY_ONE_FOR_MATRIX_END
}

DIRECT (QUERY_ONE_FOR_MATRIX__Permutation_listAllInversions) {
	QUERY_ONE_FOR_MATRIX (Permutation)
		autoMAT result = Permutation_getAllInversions (me);
	QUERY_ONE_FOR_MATRIX_END
}

FORM (QUERY_ONE_FOR_INTEGER__Permutation_getIndexAtValue, U"Permutation: Get index", U"Permutation: Get index...") {
	NATURAL (value, U"Value", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Permutation)
		const integer result = Permutation_getIndexAtValue (me, value);
	QUERY_ONE_FOR_INTEGER_END (U" (index, at value = ", value, U")")
}

FORM (MODIFY_Permutation_tableJump, U"Permutation: Table jump", U"Permutation: Table jump...") {
	NATURAL (jumpSize, U"Jump size", U"4")
	NATURAL (first, U"First", U"1")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_tableJump_inline (me, jumpSize, first);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Permutation_sort) {
	MODIFY_EACH (Permutation)
		Permutation_sort (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapBlocks, U"Permutation: Swap blocks", U"Permutation: Swap blocks...") {
	NATURAL (fromIndex, U"From index", U"1")
	NATURAL (toIndex, U"To index", U"2")
	NATURAL (blockSize, U"Block size", U"1")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapBlocks (me, fromIndex, toIndex, blockSize);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapPositions, U"Permutation: Swap positions", U"Permutation: Swap positions...") {
	NATURAL (firstIndex, U"First index", U"1")
	NATURAL (secondIndex, U"Second index", U"2")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapPositions (me, firstIndex, secondIndex);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapNumbers, U"Permutation: Swap numbers", U"Permutation: Swap numbers...") {
	NATURAL (firstNumber, U"First number", U"1")
	NATURAL (secondNumber, U"Second number", U"2")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapNumbers (me, firstNumber, secondNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_Permutation_swapOneFromRange, U"Permutation: Swap one from range", U"Permutation: Swap one from range...") {
	COMMENT (U"A randomly chosen element from ")
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	COMMENT (U"is swapped with the element at")
	NATURAL (index, U"Index", U"1")
	BOOLEAN (forbidSame, U"Forbid same", true)
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_swapOneFromRange (me, fromIndex, toIndex, index, forbidSame);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Permutation_permuteRandomly, U"Permutation: Permute randomly", U"Permutation: Permute randomly...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Permutation)
		autoPermutation result = Permutation_permuteRandomly (me, fromIndex, toIndex);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rdm")
}

FORM (CONVERT_EACH_TO_ONE__Permutation_rotate, U"Permutation: Rotate", U"Permutation: Rotate...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	INTEGER (step, U"Step size", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Permutation)
		autoPermutation result = Permutation_rotate (me, fromIndex, toIndex, step);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rot", step)
}

FORM (CONVERT_EACH_TO_ONE__Permutation_reverse, U"Permutation: Reverse", U"Permutation: Reverse...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Permutation)
		autoPermutation result = Permutation_reverse (me, fromIndex, toIndex);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rev")
}

FORM (CONVERT_EACH_TO_ONE__Permutation_permuteBlocksRandomly, U"Permutation: Permute blocks randomly", U"Permutation: Permute randomly (blocks)...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	NATURAL (blockSize, U"Block size", U"12")
	BOOLEAN (permuteWithinBlocks, U"Permute within blocks", true)
	BOOLEAN (noDoublets, U"No doublets", false)
	OK
DO
	CONVERT_EACH_TO_ONE (Permutation)
		autoPermutation result = Permutation_permuteBlocksRandomly (me, fromIndex, toIndex, 
			blockSize, permuteWithinBlocks, noDoublets
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_pbr", blockSize)
}

FORM (CONVERT_EACH_TO_ONE__Permutation_interleave, U"Permutation: Interleave", U"Permutation: Interleave...") {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	NATURAL (blockSize, U"Block size", U"12")
	INTEGER (offset, U"Offset", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (Permutation)
		autoPermutation result = Permutation_interleave (me, fromIndex, toIndex, blockSize, offset);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_itl")
}

DIRECT (CONVERT_EACH_TO_ONE__Permutation_invert) {
	CONVERT_EACH_TO_ONE (Permutation)
		autoPermutation result = Permutation_invert (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_inv")
}

DIRECT (COMBINE_ALL_TO_ONE__Permutations_multiply) {
	COMBINE_ALL_TO_ONE (Permutation)
		autoPermutation result = Permutations_multiply (& list);
	COMBINE_ALL_TO_ONE_END (U"mul_", list.size);
}

FORM (CONVERT_TWO_TO_ONE__Permutation_permutePartByOther, U"Permutation: Permute part by other", U"Permutation: Permute part...") {
	NATURAL (startPos, U"Start index", U"1")
	OK
DO
	CONVERT_TWO_TO_ONE (Permutation)
		autoPermutation result = Permutation_permutePartByOther (me, startPos, you);
	CONVERT_TWO_TO_ONE_END (U"")
}
DIRECT (MODIFY_Permutations_next) {
	MODIFY_EACH (Permutation)
		Permutation_next_inplace (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Permutations_previous) {
	MODIFY_EACH (Permutation)
		Permutation_previous_inplace (me);
	MODIFY_EACH_END
}

FORM (MODIFY__Permutation_permuteRandomlyInplace, U"Permutation: Permute randomly (in-place)", nullptr) {
	INTEGER (fromIndex, U"left Index range", U"0")
	INTEGER (toIndex, U"right Index range", U"0")
	OK
DO
	MODIFY_EACH (Permutation)
		Permutation_permuteRandomly_inplace (me, fromIndex, toIndex);
	MODIFY_EACH_END
}

FORM (CONVERT_TWO_TO_ONE__Pitches_to_DTW, U"Pitches: To DTW", U"Pitches: To DTW...") {
	REAL (vuvCosts, U"Voiced-unvoiced costs", U"24.0")
	REAL (weight, U"Time costs weight", U"10.0")
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_TWO_TO_ONE (Pitch)
		autoDTW result = Pitches_to_DTW (me, you, vuvCosts, weight, matchStart, matchEnd, slopeConstraint);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__PitchTier_to_Pitch, U"PitchTier: To Pitch", U"PitchTier: To Pitch...") {
	POSITIVE (stepSize, U"Step size", U"0.02")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"400.0")
	OK
DO
	CONVERT_EACH_TO_ONE (PitchTier)
		autoPitch result = PitchTier_to_Pitch (me, stepSize, pitchFloor, pitchCeiling);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_PitchTier_modifyInterval, U"PitchTier: Modify interval", U"PitchTier: Modify interval...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	COMMENT (U"")
	REALVECTOR (relativeTimes, U"Relative times", WHITESPACE_SEPARATED_, U"0.0 0.5 1.0")
	OPTIONMENU (timeOffset, U"...are...", 1)
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"independent")
	COMMENT (U"...of the interval duration which will be added...")
	COMMENT (U"...to the start time of the interval.")
	SENTENCE (pitches_string, U"The \"pitch\" values", U"100 200 100")
	OPTIONMENU (pitch_as, U"...are...", 1)
		OPTION (U"frequencies")
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"start and slopes")
		OPTION (U"slopes and end")
		OPTION (U"music notes")
//		OPTION (U"semitones")
	COMMENT (U"...to be added to the anchor value (if used)...")
	OPTIONMENU (pitch_is, U"...which is the...", 1)
		OPTION (U"not used")
		OPTION (U"current")
		OPTION (U"start")
		OPTION (U"end")
		OPTION (U"mean of the curve")
		OPTION (U"mean of the points")
		OPTION (U"maximum")
		OPTION (U"minimum")
	COMMENT (U"...frequency value in the interval.")
	COMMENT (U"")
	OPTIONMENU (pitch_unit, U"Pitch frequency unit", 1)
		OPTION (U"Hertz")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_modifyInterval (me, fromTime, toTime, relativeTimes, timeOffset, pitches_string, pitch_unit, pitch_as, pitch_is);
	MODIFY_EACH_END
}

FORM (MODIFY_PitchTier_modifyInterval_toneLevels, U"PitchTier: Modify interval (tone levels)", U"PitchTier: Modify interval (tone levels)...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	REAL (fmin, U"left Pitch range (Hz)", U"80.0")
	REAL (fmax, U"right Pitch range", U"200.0")
	NATURAL (numberOfToneLevels, U"Number of tone levels", U"5")
	COMMENT (U"")
	REALVECTOR (relativeTimes, U"Relative times", WHITESPACE_SEPARATED_, U"0.0 0.5 1.0")
	OPTIONMENU (time_offset, U"...are...", 1)
		OPTION (U"fractions")
		OPTION (U"percentages")
		OPTION (U"independent")
	COMMENT (U"...of the interval duration which will be added...")
	COMMENT (U"...to the start time of the interval.")
	REALVECTOR (pitchesAsToneLevels, U"Pitches as tone levels", WHITESPACE_SEPARATED_, U"2.1 2.1 5.0")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_modifyInterval_toneLevels (me, fromTime, toTime, fmin, fmax, numberOfToneLevels,
			relativeTimes, time_offset, pitchesAsToneLevels
		);
	MODIFY_EACH_END
}

/******************* Polygon & Categories *************************************/

FORM (CREATE_ONE__Polygon_createSimple, U"Create simple Polygon", U"Create simple Polygon...") {
	WORD (name, U"Name", U"p")
	REALVECTOR (vertices_asXYPairs, U"Vertices as X-Y pairs", WHITESPACE_SEPARATED_, U"0.0 0.0  0.0 1.0  1.0 0.0")
	OK
DO
	CREATE_ONE
		autoPolygon result = Polygon_createSimple (vertices_asXYPairs);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Polygon_createFromRandomPoints, U"Polygon: Create from random points", nullptr) {
	WORD (name, U"Name", U"p")
	NATURAL (numberOfPoints, U"Number of points", U"10")
	REAL (xmin, U"left X range", U"0.0")
	REAL (xmax, U"right X range", U"1.0")
	REAL (ymin, U"left Y range", U"0.0")
	REAL (ymax, U"right Y range", U"1.0")
	OK
DO
	CREATE_ONE
		autoPolygon result = Polygon_createFromRandomPoints (numberOfPoints, xmin, xmax, ymin, ymax);
	CREATE_ONE_END (name)
}

DIRECT (QUERY_ONE_FOR_INTEGER__Polygon_getNumberOfPoints) {
	QUERY_ONE_FOR_INTEGER (Polygon)
		const integer result = my numberOfPoints;
	QUERY_ONE_FOR_INTEGER_END (U" (number of points)")
}

FORM (QUERY_ONE_FOR_REAL__Polygon_getPointX, U"Polygon: Get point (x)", nullptr) {
	NATURAL (pointNumber, U"Point number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Polygon)
		const double result = ( pointNumber <= my numberOfPoints ? my x [pointNumber] : undefined );
	QUERY_ONE_FOR_REAL_END (U" (x [", pointNumber, U"])")
}

FORM (QUERY_ONE_FOR_REAL__Polygon_getPointY, U"Polygon: Get point (y)", nullptr) {
	NATURAL (pointNumber, U"Point number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Polygon)
		const double result = ( pointNumber <= my numberOfPoints ? my y [pointNumber] : undefined );
	QUERY_ONE_FOR_REAL_END (U" (y [", pointNumber, U"])")
}

FORM (QUERY_ONE_FOR_STRING__Polygon_getLocationOfPoint, U"Get location of point", U"Polygon: Get location of point...") {
	COMMENT (U"Point is (I)n, (O)ut, (E)dge or (V)ertex?")
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	REAL (eps, U"Precision", U"1.64e-15")
	OK
DO
	Melder_require (eps >= 0.0, 
		U"The precision cannot be negative.");
	QUERY_ONE_FOR_STRING (Polygon)
		const int loc = Polygon_getLocationOfPoint (me, x, y, eps);
		conststring32 result = ( loc == Polygon_INSIDE ? U"I" : loc == Polygon_OUTSIDE ? U"O" :
				loc == Polygon_EDGE ? U"E" : U"V" );
	QUERY_ONE_FOR_STRING_END
}

DIRECT (QUERY_ONE_FOR_REAL__Polygon_getAreaOfConvexHull) {
	QUERY_ONE_FOR_REAL (Polygon)
		const double result = Polygon_getAreaOfConvexHull (me);
	QUERY_ONE_FOR_REAL_END (U" (area convex hull)")
}

FORM (CONVERT_EACH_TO_ONE__Polygon_circularPermutation, U"Polygon: Circular permutation", nullptr) {
	INTEGER (shift, U"Shift", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Polygon)
		autoPolygon result = Polygon_circularPermutation (me, shift);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", shift)
}


DIRECT (CONVERT_EACH_TO_ONE__Polygon_simplify) {
	CONVERT_EACH_TO_ONE (Polygon)
		autoPolygon result = Polygon_simplify (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_s")
}

DIRECT (CONVERT_EACH_TO_ONE__Polygon_convexHull) {
	CONVERT_EACH_TO_ONE (Polygon)
		autoPolygon result = Polygon_convexHull (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_hull")
}

FORM (MODIFY_EACH__Polygon_translate, U"Polygon: Translate", U"Polygon: Translate...") {
	REAL (xDistance, U"X distance", U"0.0")
	REAL (yDistance, U"Y distance", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_translate (me, xDistance, yDistance);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Polygon_rotate, U"Polygon: Rotate", U"Polygon: Rotate...") {
	COMMENT (U"Rotate counterclockwise over the")
	REAL (angle_degrees, U"Angle (degrees)", U"0.0")
	COMMENT (U"With respect to the point")
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_rotate (me, angle_degrees, x, y);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Polygon_scale, U"Polygon: Scale polygon", nullptr) {
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_scale (me, x, y);
	MODIFY_EACH_END
}

FORM (GRAPHICS_ONE_AND_ONE__Polygon_Categories_draw, U"Polygon & Categories: Draw", nullptr) {
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_ONE_AND_ONE (Polygon, Categories)
		Polygon_Categories_draw (me, you, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_ONE_AND_ONE_END
}

DIRECT (MODIFY_EACH__Polygon_reverseX) {
	MODIFY_EACH (Polygon)
		Polygon_reverseX (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Polygon_reverseY) {
	MODIFY_EACH (Polygon)
		Polygon_reverseY (me);
	MODIFY_EACH_END
}

/***************** Polynomial *******************/

DIRECT (HELP__Polynomial_help) {
	HELP (U"Polynomial")
}

FORM (CREATE_ONE__Polynomial_create, U"Create Polynomial from coefficients", U"Create Polynomial...") {
	WORD (name, U"Name", U"p")
	COMMENT (U"Domain of polynomial")
	REAL (xmin, U"Xmin", U"-3.0")
	REAL (xmax, U"Xmax", U"4.0")
	COMMENT (U"p(x) = c[1] + c[2] x + ... c[n+1] x^n")
	REALVECTOR (coefficients, U"Coefficients", WHITESPACE_SEPARATED_, U"2.0 -1.0 -2.0 1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CREATE_ONE
		autoPolynomial result = Polynomial_createWithCoefficients (xmin, xmax, coefficients);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Polynomial_createFromProductTerms, U"Create Polynomial from second order products", nullptr) {
	WORD (name, U"Name", U"p")
	COMMENT (U"Domain of polynomial")
	REAL (xmin, U"Xmin", U"-2.0")
	REAL (xmax, U"Xmax", U"2.0")
	COMMENT (U"(1+a[1]*x+x^2)*(1+a[2]*x+x^2)*...*(1+a[n]*x+x^2)")
	REALVECTOR (coefficients, U"The a's", WHITESPACE_SEPARATED_, U"1.0 2.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromProductOfSecondOrderTerms (xmin, xmax, coefficients);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Polynomial_createFromRealZeros, U"Create Polynomial from first order products", nullptr) {
	WORD (name, U"Name", U"p")
	COMMENT (U"Domain of polynomial")
	REAL (xmin, U"Xmin", U"-3.0")
	REAL (xmax, U"Xmax", U"3.0")
	COMMENT (U"(P(x) = (x-zero[1])*(1-zero[2])*...*(x-zero[n])")
	REALVECTOR (zeroes, U"The zeroes", WHITESPACE_SEPARATED_, U"1.0 2.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CREATE_ONE
		autoPolynomial result = Polynomial_createFromRealRoots (xmin, xmax, zeroes);
	CREATE_ONE_END (name)
}

FORM (MODIFY_EACH__Polynomial_divide_secondOrderFactor, U"Polynomial: Divide second order factor", nullptr) {
	COMMENT (U"P(x) / (x^2 - factor)")
	REAL (factor, U"Factor", U"1.0")
	OK
DO
	MODIFY_EACH (Polynomial)
		Polynomial_divide_secondOrderFactor (me, factor);
	MODIFY_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__Polynomial_getArea, U"Polynomial: Get area", U"Polynomial: Get area...") {
	COMMENT (U"Interval")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Polynomial)
		const double result = Polynomial_getArea (me, xmin, xmax);
	QUERY_ONE_FOR_REAL_END (U" (area)")
}

FORM (QUERY_ONE_FOR_REAL__Polynomial_getRemainderAfterDivision, U"Polynomial: Get remainder after division", nullptr) {
	COMMENT (U"P(x) / (x - factor)")
	REAL (factor, U"Monomial factor", U"1.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Polynomial)
		double result;
		autoPolynomial p = Data_copy (me);
		Polynomial_divide_firstOrderFactor (p.get(), factor, & result);
	QUERY_ONE_FOR_REAL_END (U" (remainder)")
}

FORM (INFO_ONE__Polynomial_getDerivativesAtX, U"Polynomial: Get derivatives at X", nullptr) {
	REAL (x, U"X", U"0.5")
	INTEGER (numberOfDerivatives, U"Number of derivatives", U"2")
	OK
DO
	INFO_ONE (Polynomial)
		autoVEC derivatives = Polynomial_evaluateDerivatives (me, x, numberOfDerivatives);
		MelderInfo_open ();
			MelderInfo_writeLine (U"Function value: ", derivatives [1]);
			for (integer i = 2; i <= numberOfDerivatives + 1; i ++) {
				MelderInfo_writeLine (U"Derivative ", i - 1, U": ", i < my numberOfCoefficients ? derivatives [i] : undefined, U"");
			}
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (QUERY_ONE_FOR_REAL__Polynomial_getOneRealRoot, U"Polynomial: Get one real root", nullptr) {
	COMMENT (U"Interval: ")
	REAL (xmin, U"left X Range", U"-1.0")
	REAL (xmax, U"right X Range", U"1.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Polynomial)
		const double result = Polynomial_findOneSimpleRealRoot_nr (me, xmin, xmax);
	QUERY_ONE_FOR_REAL_END (U" (root)")
}

DIRECT (CONVERT_EACH_TO_ONE__Polynomial_getDerivative) {
	CONVERT_EACH_TO_ONE (Polynomial)
		autoPolynomial result = Polynomial_getDerivative (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_derivative")
}

FORM (CONVERT_EACH_TO_ONE__Polynomial_getPrimitive, U"Polynomial: Get primitive", nullptr) {
	REAL (constant, U"Constant", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Polynomial)
		autoPolynomial result = Polynomial_getPrimitive (me,constant);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_primitive")
}

FORM (CONVERT_EACH_TO_ONE__Polynomial_scaleX, U"Polynomial: Scale x", U"Polynomial: Scale x...") {
	COMMENT (U"New domain")
	REAL (xmin, U"Xmin", U"-1.0")
	REAL (xmax, U"Xmax", U"1.0")
	OK
DO
	Melder_require (xmin < xmax, U"Xmin should be less than Xmax.");
	CONVERT_EACH_TO_ONE (Polynomial)
		autoPolynomial result = Polynomial_scaleX (me, xmin, xmax);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_scaleX")
}

DIRECT (MODIFY_Polynomial_scaleCoefficients_monic) {
	MODIFY_EACH (Polynomial)
		Polynomial_scaleCoefficients_monic (me);
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__Polynomial_to_Roots) {
	CONVERT_EACH_TO_ONE (Polynomial)
		autoRoots result = Polynomial_to_Roots (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (INFO_ONE__Polynomial_evaluate_z, U"Polynomial: Get value (complex)", U"Polynomial: Get value (complex)...") {
	REAL (x, U"Real part", U"0.0")
	REAL (y, U"Imaginary part", U"0.0")
	OK
DO
	dcomplex z { x, y };
	INFO_ONE (Polynomial)
		const dcomplex result = Polynomial_evaluate_z (me, z);
		MelderInfo_open ();
		MelderInfo_writeLine (result);
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (CONVERT_EACH_TO_ONE__Polynomial_to_Spectrum, U"Polynomial: To Spectrum", U"Polynomial: To Spectrum...") {
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	NATURAL (numberOfFrequencies, U"Number of frequencies (>1)", U"1025")
	OK
DO
	CONVERT_EACH_TO_ONE (Polynomial)
		autoSpectrum result = Polynomial_to_Spectrum (me, maximumFrequency, numberOfFrequencies, 1.0);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Polynomials_multiply) {
	CONVERT_TWO_TO_ONE (Polynomial)
		autoPolynomial result = Polynomials_multiply (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_x_", your name.get())
}

FORM (CONVERT_TWO_TO_MULTIPLE__Polynomials_divide, U"Polynomials: Divide", U"Polynomials: Divide...") {
	BOOLEAN (wantQuotient, U"Want quotient", true)
	BOOLEAN (wantRemainder, U"Want remainder", true)
	OK
DO
	Melder_require (wantQuotient || wantRemainder,
		U"You should select \"Want quotient\", \"Want remainder\", or both.");
	CONVERT_TWO_TO_MULTIPLE (Polynomial)
		autoPolynomial quotient, remainder;
		Polynomials_divide (me, you, wantQuotient ? & quotient : nullptr, wantRemainder ? & remainder : nullptr);
		if (wantQuotient)
			praat_new (quotient.move(), my name.get(), U"_q");
		if (wantRemainder)
			praat_new (remainder.move(), my name.get(), U"_r");
	CONVERT_TWO_TO_MULTIPLE_END
}

/********************* Roots ******************************/

DIRECT (HELP__Roots_help) {
	HELP (U"Roots")
}

FORM (GRAPHICS_EACH__Roots_draw, U"Roots: Draw", nullptr) {
	REAL (xmin, U"Minimum of real axis", U"0.0")
	REAL (xmax, U"Maximum of real axis", U"0.0")
	REAL (ymin, U"Minimum of imaginary axis", U"0.0")
	REAL (ymax, U"Maximum of imaginary axis", U"0.0")
	SENTENCE (mark_string, U"Mark string (+x0...)", U"o")
	POSITIVE (markSize, U"Mark size", U"12")
	BOOLEAN (garnish, U"Garnish", false)
	OK
DO
	GRAPHICS_EACH (Roots)
		Roots_draw (me, GRAPHICS, xmin, xmax, ymin, ymax, mark_string, markSize, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__Roots_getNumberOfRoots) {
	QUERY_ONE_FOR_INTEGER (Roots)
		const integer result = Roots_getNumberOfRoots (me);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (COMPLEX_Roots_getRoot, U"Roots: Get root", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	OK
DO
	QUERY_ONE_FOR_COMPLEX (Roots)
		const dcomplex result = Roots_getRoot (me, rootNumber);
	QUERY_ONE_FOR_COMPLEX_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__Roots_getRealPartOfRoot, U"Roots: Get real part", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Roots)
		const dcomplex z = Roots_getRoot (me, rootNumber);
		const double result = z.real();
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__Roots_getImaginaryPartOfRoot, U"Roots: Get imaginary part", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Roots)
		const dcomplex z = Roots_getRoot (me, rootNumber);
		const double result = z.imag();
	QUERY_ONE_FOR_REAL_END (U" i")
}

FORM (MODIFY_EACH__Roots_setRoot, U"Roots: Set root", nullptr) {
	NATURAL (rootNumber, U"Root number", U"1")
	REAL (x, U"Real part", U"1.0/sqrt(2)")
	REAL (y, U"Imaginary part", U"1.0/sqrt(2)")
	OK
DO
	MODIFY_EACH (Roots)
		Roots_setRoot (me, rootNumber, x, y);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Roots_to_Spectrum, U"Roots: To Spectrum", U"Roots: To Spectrum...") {
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	NATURAL (numberOfFrequencies, U"Number of frequencies (>1)", U"1025")
	OK
DO
	CONVERT_EACH_TO_ONE (Roots)
		autoSpectrum result = Roots_to_Spectrum (me, maximumFrequency, numberOfFrequencies, 1.0);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__Roots_Polynomial_polish) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Roots, Polynomial)
		Roots_Polynomial_polish (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

/*****************************************************************************/

FORM (QUERY_NONE_FOR_REAL__Praat_getTukeyQ, U"Get TukeyQ", nullptr) {
	POSITIVE (criticalValue, U"Critical value", U"2.0")
	NATURAL (numberOfMeans, U"Number of means", U"3")
	POSITIVE (degreesOfFreedon, U"Degrees of freedom", U"10.0")
	NATURAL (numberOfRows, U"Number of rows", U"1")
	OK
DO
	QUERY_NONE_FOR_REAL
		const double result = NUMtukeyQ (criticalValue, numberOfMeans, degreesOfFreedon, numberOfRows);
	QUERY_NONE_FOR_REAL_END (U" (tukeyQ)")
}

FORM (QUERY_NONE_FOR_REAL__Praat_getInvTukeyQ, U"Get invTukeyQ", nullptr) {
	REAL (probability, U"Probability", U"0.05")
	NATURAL (numberOfMeans, U"Number of means", U"3")
	POSITIVE (degreesOfFreedon, U"Degrees of freedom", U"10.0")
	NATURAL (numberOfRows, U"Number of rows", U"1")
	OK
DO
	QUERY_NONE_FOR_REAL
		Melder_require (probability >= 0.0 && probability <= 1.0, 
			U"The probability should be in the interval [0, 1].");
		const double result = NUMinvTukeyQ (probability, numberOfMeans, degreesOfFreedon, numberOfRows);
	QUERY_NONE_FOR_REAL_END (U" (inv tukeyQ)")
}

FORM (QUERY_NONE_FOR_COMPLEX__Praat_getIncompleteGamma, U"Get incomplete gamma", U"Get incomplete gamma...") {
	POSITIVE (reAlpha, U"Real part of alpha", U"4.0")
	REAL (imAlpha, U"Imaginary part of alpha", U"0.0")
	REAL (reX, U"Real part of X", U"4.0")
	REAL (imX, U"Imaginary part of X", U"0.0")
	OK
DO
	QUERY_NONE_FOR_COMPLEX
		const dcomplex result = NUMincompleteGammaFunction (dcomplex {reAlpha, imAlpha}, dcomplex {reX, imX});
	QUERY_NONE_FOR_COMPLEX_END (U"")
}

/******************** Sound ****************************************/

#define Sound_create_addCommonFields(startTime,endTime,samplingFrequency) \
	REAL (startTime, U"Start time (s)", U"0.0") \
	REAL (endTime, U"End time (s)", U"1.0") \
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")


static void Sound_create_checkCommonFields (double startTime, double endTime, double samplingFrequency) {
	double numberOfSamples_real;
	numberOfSamples_real = round ((endTime - startTime) * samplingFrequency);
	if (endTime <= startTime) {
		if (endTime == startTime)
			Melder_throw (U"A Sound cannot have a duration of zero.");
		else
			Melder_throw (U"A Sound cannot have a duration less than zero.");
		if (startTime == 0.0)
			Melder_throw (U"Please set the finishing time to something greater than 0 seconds.");
		else
			Melder_throw (U"Please lower the starting time or raise the finishing time.");
	}
	if (samplingFrequency <= 0.0)
		Melder_throw (U"A Sound cannot have a negative sampling frequency.\n"
		U"Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");

	if (numberOfSamples_real < 1.0) {
		Melder_appendError (U"A Sound cannot have zero samples.\n");
		if (startTime == 0.0) {
			Melder_throw (U"Please raise the finishing time.");
		} else {
			Melder_throw (U"Please lower the starting time or raise the finishing time.");
		}
	}
	if (numberOfSamples_real > INT54_MAX)
		Melder_throw (U"A Sound cannot have ", numberOfSamples_real,
				U" samples; the maximum is ", Melder_bigInteger (INT54_MAX), U" samples.");
	if (sizeof (integer) == 4 && numberOfSamples_real > INT32_MAX)
		Melder_throw (U"A Sound cannot have ", Melder_bigInteger (int64 (numberOfSamples_real)),
				U" samples; the maximum is ", Melder_bigInteger (INT32_MAX), U" samples.");
}

FORM_SAVE (SAVE__Sound_saveAsHighestQualityMP3File, U"Sound: Save as MP3 file", nullptr, U"mp3") {
	SAVE_ONE (Sound, U"save the selected Sound object to the highest-quality MP3 file")
		Sound_saveAsMP3File_VBR (me, file, 0.0);
	SAVE_ONE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_to_FormantFilter, U"Sound & Pitch: To FormantFilter", U"Sound & Pitch: To Spectrogram...") {
	POSITIVE (windowLength, U"Analysis window duration (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoFormantFilter result = Sound_Pitch_to_FormantFilter (me, you, windowLength, timeStep, 
			firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth
		);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_to_Spectrogram, U"Sound & Pitch: To Spectrogram", U"Sound & Pitch: To Spectrogram...") {
	POSITIVE (windowLength, U"Analysis window duration (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoSpectrogram result = Sound_Pitch_to_Spectrogram (me, you, windowLength, timeStep, 
			firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth
		);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_changeGender, U"Sound & Pitch: Change gender", U"Sound & Pitch: Change gender...") {
	POSITIVE (formantShiftRatio, U"Formant shift ratio", U"1.2")
	REAL (newPitchMedian, U"New pitch median (Hz)", U"0.0 (= no change)")
	POSITIVE (pitchRangeFactor, U"Pitch range factor", U"1.0 (= no change)")
	POSITIVE (durationFactor, U"Duration factor", U"1.0")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoSound result = Sound_Pitch_changeGender_old (me, you, formantShiftRatio, 
			newPitchMedian, pitchRangeFactor, durationFactor
		);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_changeSpeaker, U"Sound & Pitch: Change speaker", U"Sound & Pitch: Change speaker...") {
	POSITIVE (formantFrequencyMultiplicationFactor, U"Multiply formants by", U"1.1 (male->female)")
	POSITIVE (pitchMultiplicationFactor, U"Multiply pitch by", U"1.8 (male->female")
	REAL (pitchRangeMultiplicationFactor, U"Multiply pitch range by", U"1.0 (= no change)")
	POSITIVE (durationMultiplicationFactor, U"Multiply duration by", U"1.0")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoSound result = Sound_Pitch_changeSpeaker (me, you, formantFrequencyMultiplicationFactor,
			pitchMultiplicationFactor, pitchRangeMultiplicationFactor, durationMultiplicationFactor
		);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Sound_IntervalTier_cutPartsMatchingLabel, U"Sound & IntervalTier: Cut parts matching label", nullptr) {
	SENTENCE (label, U"Label", U"cut")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, IntervalTier)
		autoSound result = Sound_IntervalTier_cutPartsMatchingLabel (me, you, label);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_cut")
}

FORM (CREATE_ONE__Sound_createAsGammaTone, U"Create a gammatone", U"Create Sound as gammatone...") {
	WORD (name, U"Name", U"gammatone")
	Sound_create_addCommonFields (startTime, endTime, samplingFrequency)
	INTEGER (gamma, U"Gamma", U"4")
	POSITIVE (frequency, U"Frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"150.0")
	REAL (initialPhase, U"Initial phase (radians)", U"0.0")
	REAL (additionFactor, U"Addition factor", U"0.0")
	BOOLEAN (scaleAmplitudes, U"Scale amplitudes", true)
	OK
DO
	CREATE_ONE
		Sound_create_checkCommonFields (startTime, endTime, samplingFrequency);
		Melder_require (frequency < 0.5 * samplingFrequency,
			U"Your frequency should not be greater than half the sampling frequency. Use a frequency less than ", 0.5 * samplingFrequency, U".");
		Melder_require (gamma >= 0, 
			U"Gamma should not be negative.");
		autoSound result = Sound_createGammaTone (startTime, endTime, samplingFrequency, gamma, 
			frequency, bandwidth, initialPhase, additionFactor, scaleAmplitudes
		);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Sound_createAsShepardTone, U"Create a Shepard tone", U"Create Sound as Shepard tone...") {
	WORD (name, U"Name", U"shepardTone")
	Sound_create_addCommonFields (startTime, endTime, samplingFrequency)
	POSITIVE (minimumFrequency, U"Lowest frequency (Hz)", U"4.863")
	NATURAL (numberOfComponents, U"Number of components", U"10")
	REAL (frequencyChange, U"Frequency change (semitones/s)", U"4.0")
	POSITIVE (amplitudeRange_dB, U"Amplitude range (dB)", U"30.0")
	REAL (octaveShiftFraction, U"Octave shift fraction ([0,1))", U"0.0")
	OK
DO
	CREATE_ONE
		Sound_create_checkCommonFields (startTime, endTime, samplingFrequency);
		autoSound result = Sound_createShepardToneComplex (startTime, endTime, samplingFrequency, minimumFrequency,
			numberOfComponents, frequencyChange, amplitudeRange_dB, octaveShiftFraction
		);
	CREATE_ONE_END (name)
}

FORM (GRAPHICS_EACH__Sound_drawWhere, U"Sound: Draw where", U"Sound: Draw where...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
		OPTION (U"curve")
		OPTION (U"bars")
		OPTION (U"poles")
		OPTION (U"speckles")
	FORMULA (formula, U"Draw only those parts where the following condition holds", U"x < xmin + (xmax - xmin) / 2; first half")
	OK
DO
	const integer numberOfBisections = 10;
	GRAPHICS_EACH (Sound)
		Sound_drawWhere (me, GRAPHICS, fromTime, toTime, ymin, ymax, garnish, drawingMethod, 
			numberOfBisections, formula, interpreter
		);
	GRAPHICS_EACH_END
}

FORM (PLAY_EACH__Sound_playOneChannel, U"Sound: Play one channel", nullptr) {
    NATURAL (channel, U"Channel", U"1")
    OK
DO
    PLAY_EACH (Sound)
		Melder_require (channel <= my ny,
			me, U": there is no channel ", channel, U". Sound has only ", my ny, U" channel",
				(my ny > 1 ? U"s." : U"."));
        autoSound thee = Sound_extractChannel (me, channel);
        Sound_play (thee.get(), nullptr, nullptr);
    PLAY_EACH_END
}

FORM (PLAY_EACH__Sound_playAsFrequencyShifted, U"Sound: Play as frequency shifted", U"Sound: Play as frequency shifted...") {
	REAL (frequencyShift, U"Shift by (Hz)", U"1000.0")
	POSITIVE (samplingFrequency, U"New sampling frequency (Hz)", U"44100.0")
	NATURAL (samplePrecision, U"Precision (samples)", U"50")
	OK
DO
    PLAY_EACH (Sound)
		Sound_playAsFrequencyShifted (me, frequencyShift, samplingFrequency, samplePrecision);
    PLAY_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__Sound_getNearestLevelCrossing, U"Sound: Get nearest level crossing", U"Sound: Get nearest level crossing...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	REAL (time, U"Time (s)", U"0.1")
	REAL (level, U"Level", U"0.1")
	OPTIONMENU_ENUM (kVectorSearchDirection, searchDirection, U"Search direction", kVectorSearchDirection::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		Melder_require (channel > 0 && channel <= my ny,
			U"The channel number should be between 1 and ", my ny, U".");
		const double result = Vector_getNearestLevelCrossing (me, channel, time, level, searchDirection);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (CONVERT_TWO_TO_ONE__Sounds_to_DTW, U"Sounds: To DTW", nullptr) {
    POSITIVE (windowLength, U"Window length (s)", U"0.015")
    POSITIVE (timeStep, U"Time step (s)", U"0.005")
    COMMENT (U"")
    REAL (sakoeChibaBand, U"Sakoe-Chiba band (s)", U"0.1")
    CHOICE (slopeConstraint, U"Slope constraint", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
    OK
DO
    CONVERT_TWO_TO_ONE (Sound)
		autoDTW result = Sounds_to_DTW (me, you, windowLength, timeStep, sakoeChibaBand, slopeConstraint);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_TextGrid_detectSilences, U"Sound: To TextGrid (silences)", U"Sound: To TextGrid (silences)...") {
	COMMENT (U"Parameters for the intensity analysis")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"100")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	COMMENT (U"Silent intervals detection")
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-25.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval (s)", U"0.1")
	WORD (silenceLabel, U"Silent interval label", U"silent")
	WORD (soundingLabel, U"Sounding interval label", U"sounding")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoTextGrid result = Sound_to_TextGrid_detectSilences (me, pitchFloor, timeStep, silenceThreshold,
			minimumSilenceDuration, minimumSoundingDuration, silenceLabel, soundingLabel
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_TextGrid_speechActivity, U"Sound: To TextGrid (speech activity, LTSF)", U"Sound: To TextGrid (speech activity, LTSF)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (longtermWindow, U"Long term window (s)", U"0.3")
	POSITIVE (shorttermWindow, U"Short term window (s)", U"0.1")
	POSITIVE (fmin, U"left Frequency range (Hz)", U"70.0")
	POSITIVE (fmax,  U"right Frequency range (Hz)", U"6000.0")
	REAL (flatnessThreshold, U"Flatness threshold", U"-10.0")
	REAL (nonspeechThreshold_dB, U"Non-speech threshold (dB)", U"-35.0")
	POSITIVE (minimumNonspeechDuration, U"Min. non-speech interval (s)", U"0.1")
	POSITIVE (minimumSpeechDuration, U"Min. speech interval (s)", U"0.1")
	WORD (nonspeechLabel, U"Non-speech interval label", U"non-speech")
	WORD (speechLabel, U"Speech interval label", U"speech")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoTextGrid result = Sound_to_TextGrid_speechActivity_ltsf (me, timeStep, longtermWindow, shorttermWindow,
			fmin, fmax, flatnessThreshold, nonspeechThreshold_dB, minimumNonspeechDuration, minimumSpeechDuration,
			nonspeechLabel, speechLabel
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_TextGrid_highMidLowIntervals, U"Sound: To TextGrid (high, mid, low)", nullptr) {
	REAL (maxLevel, U"Maximum level", U"0.5")
	REAL (minLevel, U"Minimum level", U"-0.5")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoTextGrid result = Sound_to_TextGrid_highMidLowIntervals (me, minLevel, maxLevel);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_copyChannelRanges, U"Sound: Copy channel ranges", nullptr) {
	NATURALVECTOR (channels, U"Create a new Sound from the following channels", RANGES_, U"1:64")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractChannels (me, sortedSet_INTVEC (channels).get());   // TODO: why sorting? (ppgb 20210711)
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_channels")
}

FORM (CONVERT_EACH_TO_ONE__Sound_trimSilences, U"Sound: Trim silences", U"Sound: Trim silences...") {
	REAL (trimDuration, U"Trim duration (s)", U"0.08")
	BOOLEAN (onlyAtStartAndEnd, U"Only at start and end", true);
	COMMENT (U"Parameters for the intensity analysis")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"100")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	COMMENT (U"Silent intervals detection")
	REAL (silenceThreshold, U"Silence threshold (dB)", U"-35.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.05")
	BOOLEAN (saveTextGrid, U"Save trimming info as TextGrid", false)
	WORD (trim_string, U"Trim label", U"trimmed")
	OK
DO
	Melder_clipLeft (0.0, & trimDuration);
	CONVERT_EACH_TO_ONE (Sound)
		autoTextGrid tg;
		autoSound result = Sound_trimSilences (me, trimDuration, onlyAtStartAndEnd, pitchFloor, timeStep,
			silenceThreshold, minimumSilenceDuration, minimumSoundingDuration, 
			( saveTextGrid ? & tg : nullptr ), trim_string
		);
		if (saveTextGrid)
			praat_new (tg.move(), my name.get(), U"_trimmed");
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_trimmed")
}

// deprecated
FORM (CONVERT_EACH_TO_ONE__Sound_to_BarkFilter, U"Sound: To BarkFilter", U"Sound: To BarkSpectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (bark)", U"1.0")
	POSITIVE (deltaFrequency, U"Distance between filters (bark)", U"1.0")
	REAL (maximumFrequency, U"Maximum frequency (bark)", U"0.0");
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoBarkFilter result = Sound_to_BarkFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_BarkSpectrogram, U"Sound: To BarkSpectrogram", U"Sound: To BarkSpectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (bark)", U"1.0")
	POSITIVE (deltaFrequency, U"Distance between filters (bark)", U"1.0")
	REAL (maximumFrequency, U"Maximum frequency (bark)", U"0.0");
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoBarkSpectrogram result = Sound_to_BarkSpectrogram (me, windowLength, timeStep, 
			firstFrequency, maximumFrequency, deltaFrequency
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Spectrum_resampled, U"Sound To Spectrum (resampled)", 
	U"Sound: To Spectrum (resampled)...")
{
	NATURAL (precision, U"Precision (samples)", U"50")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSpectrum result = Sound_to_Spectrum_resampled (me, precision);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// deprecated
FORM (CONVERT_EACH_TO_ONE__Sound_to_FormantFilter, U"Sound: To FormantFilter", U"Sound: To FormantFilter...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	COMMENT (U"Pitch analysis")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormantFilter result = Sound_to_FormantFilter (me, windowLength, timeStep, 
			firstFrequency, maximumFrequency, deltaFrequency, relativeBandwidth, pitchFloor, pitchCeiling
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Spectrogram_pitchDependent, U"Sound: To Spectrogram (pitch-dependent)", U"Sound: To Spectrogram (pitch-dependent)...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (Hz)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (Hz)", U"50.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0");
	POSITIVE (relativeBandwidth, U"Relative bandwidth", U"1.1")
	COMMENT (U"Pitch analysis")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSpectrogram result = Sound_to_Spectrogram_pitchDependent (me, windowLength, timeStep, firstFrequency, 
			maximumFrequency, deltaFrequency, relativeBandwidth, pitchFloor, pitchCeiling
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// deprecated
FORM (CONVERT_EACH_TO_ONE__Sound_to_MelFilter, U"Sound: To MelFilter", U"Sound: To MelFilter...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (mel)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (mel)", U"100.0")
	REAL (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoMelFilter result = Sound_to_MelFilter (me, windowLength, timeStep, firstFrequency, maximumFrequency, deltaFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_MelSpectrogram, U"Sound: To MelSpectrogram", U"Sound: To MelSpectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFrequency, U"Position of first filter (mel)", U"100.0")
	POSITIVE (deltaFrequency, U"Distance between filters (mel)", U"100.0")
	REAL (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoMelSpectrogram result = Sound_to_MelSpectrogram (me, windowLength, timeStep,
			firstFrequency, maximumFrequency, deltaFrequency
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_ComplexSpectrogram, U"Sound: To ComplexSpectrogram", nullptr) {
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"8000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoComplexSpectrogram result = Sound_to_ComplexSpectrogram (me, windowLength, maximumFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_shs, U"Sound: To Pitch (shs)", U"Sound: To Pitch (shs)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"50.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates (Hz)", U"15")
	COMMENT (U"Algorithm parameters")
	POSITIVE (maximumFrequency, U"Maximum frequency component (Hz)", U"1250.0")
	NATURAL (maximumNumberOfSubharmonics, U"Max. number of subharmonics", U"15")
	POSITIVE (compressionFactor, U"Compression factor (<=1)", U"0.84")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	NATURAL (numberOfPointsPerOctave, U"Number of points per octave", U"48");
	OK
DO
	Melder_require (pitchFloor < pitchCeiling,
		U"The pitch floor should be less than the pitch ceiling.");
	Melder_require (pitchCeiling < maximumFrequency, 
		U"The maximum frequency should be greater than or equal to the pitch ceiling.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_shs (me, timeStep, pitchFloor, maximumFrequency, pitchCeiling, 
			maximumNumberOfSubharmonics, maximumNumberOfCandidates, compressionFactor, numberOfPointsPerOctave
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_EACH__Sound_fadeIn, U"Sound: Fade in", U"Sound: Fade in...") {
	CHANNEL (channel, U"Channel (number; 0 = all)", U"1")
	REAL (time, U"Time (s)", U"-10000.0")
	REAL (fadeTime, U"Fade time (s)", U"0.005")
	BOOLEAN (silentFromStart, U"Silent from start", false)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_fade (me, channel, time, fadeTime, false, silentFromStart);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_fadeOut, U"Sound: Fade out", U"Sound: Fade out...") {
	CHANNEL (channel, U"Channel (number; 0 = all)", U"1")
	REAL (time, U"Time (s)", U"10000.0")
	REAL (fadeTime, U"Fade time (s)", U"-0.005")
	BOOLEAN (silentToEnd, U"Silent to end", false)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_fade (me, channel, time, fadeTime, true, silentToEnd);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_KlattGrid_simple, U"Sound: To KlattGrid (simple)", U"Sound: To KlattGrid (simple)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Formant determination")
	NATURAL (numberOfFormants, U"Max. number of formants", U"5")
	POSITIVE (formantCeiling, U"Formant ceiling (Hz)", U"5500 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	COMMENT (U"Pitch determination")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	COMMENT (U"Intensity determination")
	POSITIVE (pitchFloorForIntensity, U"Pitch floor for intensity (Hz)", U"100.0")
	BOOLEAN (subtractMean, U"Subtract mean", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoKlattGrid result = Sound_to_KlattGrid_simple (me, timeStep, numberOfFormants, formantCeiling, windowLength,
			preEmphasisFrequency, pitchFloor, pitchCeiling, pitchFloorForIntensity, subtractMean
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_SPINET, U"Sound: To SPINET", U"Sound: To SPINET...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (windowLength, U"Window length (s)", U"0.040")
	COMMENT (U"Gammatone filter bank")
	POSITIVE (minimumFrequency, U"Minimum filter frequency (Hz)", U"70.0")
	POSITIVE (maximumFrequency, U"Maximum filter frequency (Hz)", U"5000.0")
	NATURAL (numberOfFilters, U"Number of filters", U"250");
	POSITIVE (pitchCeiling, U"Ceiling (Hz)", U"500.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	OK
DO
	Melder_require (minimumFrequency < maximumFrequency, 
		U"The maximum frequency should be greater than the minimum frequency.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_SPINET (me, timeStep, windowLength, minimumFrequency, maximumFrequency,
			numberOfFilters, pitchCeiling, maximumNumberOfCandidates
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}
	
FORM (CONVERT_EACH_TO_ONE__Sound_to_Polygon, U"Sound: To Polygon", U"Sound: To Polygon...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (connectionY, U"Connection y-value", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		Melder_require (channel > 0 && channel <= my ny,
			U"The channel number should be between 1 and ", my ny, U".");
		autoPolygon result = Sound_to_Polygon (me, channel, fromTime, toTime, ymin, ymax, connectionY);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_TWO_TO_ONE__Sounds_to_Polygon_enclosed, U"Sounds: To Polygon (enclosed)", U"Sounds: To Polygon (enclosed)...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	CONVERT_TWO_TO_ONE (Sound)
		autoPolygon result = Sounds_to_Polygon_enclosed (me, you, channel, fromTime, toTime, ymin, ymax);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_filterByGammaToneFilter4, U"Sound: Filter (gammatone)", U"Sound: Filter (gammatone)...") {
	POSITIVE (centreFrequency, U"Centre frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"150.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filterByGammaToneFilter4 (me, centreFrequency, bandwidth);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_filtered")
}

FORM (CONVERT_EACH_TO_ONE__Sound_reduceNoise, U"Sound: Reduce noise", U"Sound: Reduce noise...") {
	REAL (fromTime, U"left Noise time range (s)", U"0.0")
	REAL (toTime, U"right Noise time range (s)", U"0.0")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	COMMENT (U"Filter")
	REAL (fromFrequency, U"left Filter frequency range (Hz)", U"80.0")
	REAL (toFrequency, U"right Filter frequency range (Hz)", U"10000.0")
	POSITIVE (smoothingBandwidth, U"Smoothing bandwidth, (Hz)", U"40.0")
	REAL (noiseReduction_dB, U"Noise reduction (dB)", U"-20.0")
	OPTIONMENU_ENUM (kSoundNoiseReductionMethod, noiseReductionMethod, U"Noise reduction method", kSoundNoiseReductionMethod::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_reduceNoise (me, fromTime, toTime, windowLength, fromFrequency, toFrequency,
			smoothingBandwidth, noiseReduction_dB, noiseReductionMethod
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_denoised")
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Sound_derivative, U"Sound: To Sound (derivative)", U"Sound: To Sound (derivative)...") {
	POSITIVE (lowPassFrequency, U"Low-pass frequency (Hz)", U"5000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	REAL (newAbsolutePeak, U"New absolute peak", U"0 (= don't scale)")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_derivative (me, lowPassFrequency, smoothing, newAbsolutePeak);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_derivative")
}

FORM (CONVERT_EACH_TO_ONE__Sound_removeNoise, U"Sound: Remove noise", U"Sound: Reduce noise...") {
	REAL (fromTime, U"left Noise time range (s)", U"0.0")
	REAL (toTime, U"right Noise time range (s)", U"0.0")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	COMMENT (U"Filter")
	REAL (fromFrequency, U"left Filter frequency range (Hz)", U"80.0")
	REAL (toFrequency, U"right Filter frequency range (Hz)", U"10000.0")
	POSITIVE (smoothingBandwidth, U"Smoothing bandwidth, (Hz)", U"40.0")
	OPTIONMENU_ENUM (kSoundNoiseReductionMethod, noiseReductionMethod, U"Noise reduction method", kSoundNoiseReductionMethod::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_removeNoise (me, fromTime, toTime, windowLength, fromFrequency, toFrequency, 
			smoothingBandwidth, noiseReductionMethod
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_denoised")
}

FORM (CONVERT_EACH_TO_ONE__Sound_changeSpeaker, U"Sound: Change speaker", U"Sound: Change speaker...") {
	COMMENT (U"Pitch measurement parameters")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	COMMENT (U"Modification parameters")
	POSITIVE (formantMultiplicationFactor, U"Multiply formants by", U"1.2")
	POSITIVE (pitchMultiplicationFactor, U"Multiply pitch by", U"1.0")
	REAL (pitchRangeMultiplicationFactor, U"Multiply pitch range by", U"1.0 (= no change)")
	POSITIVE (durationMultiplicationFactor, U"Multiply duration by", U"1.0")
	OK
DO
	Melder_require (pitchFloor < pitchCeiling, 
		U"The \"Pitch floor\" should be smaller than the \"Pitch ceiling\".");
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_changeSpeaker (me, pitchFloor, pitchCeiling, formantMultiplicationFactor,
			pitchMultiplicationFactor, pitchRangeMultiplicationFactor, durationMultiplicationFactor
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_changeSpeaker")
}

FORM (CONVERT_EACH_TO_ONE__Sound_changeGender, U"Sound: Change gender", U"Sound: Change gender...") {
	COMMENT (U"Pitch measurement parameters")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	COMMENT (U"Modification parameters")
	POSITIVE (formantShiftRatio, U"Formant shift ratio", U"1.2")
	REAL (pitchMedian, U"New pitch median (Hz)", U"0.0 (= no change)")
	REAL (pitchRangeMultiplicationFactor, U"Pitch range factor", U"1.0 (= no change)")
	POSITIVE (durationMultiplicationFactor, U"Duration factor", U"1.0")
	OK
DO
	Melder_require (pitchFloor < pitchCeiling, 
		U"The \"Pitch floor\" should be smaller than the \"Pitch ceiling\".");
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_changeGender_old (me, pitchFloor, pitchCeiling, formantShiftRatio, pitchMedian,
			pitchRangeMultiplicationFactor, durationMultiplicationFactor
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_changeGender");
}

FORM (GRAPHICS_EACH__Sound_paintWhere, U"Sound paint where", U"Sound: Paint where...") {
	COLOUR (colour, U"Colour (0-1, name, or {r,g,b})", U"0.5")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (level, U"Fill from level", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	FORMULA (formula, U"Paint only those parts where the following condition holds", U"1; always")
	OK
DO
	const integer numberOfBisections = 10;
	GRAPHICS_EACH (Sound)
		Sound_paintWhere (me, GRAPHICS, colour, fromTime, toTime, ymin, ymax, level, garnish, 
			numberOfBisections, formula, interpreter
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TWO__Sounds_paintEnclosed, U"Sounds paint enclosed", U"Sounds: Paint enclosed...") {
	COLOUR (colour, U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (xFromTime, U"left Horizontal time range (s)", U"0.0")
	REAL (xToTime, U"right Horizontal time range (s)", U"0.0")
	REAL (yFromTime, U"left Vertical range", U"0.0")
	REAL (yToTime, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (Sound)
		Sounds_paintEnclosed (me, you, GRAPHICS, colour, xFromTime, xToTime, yFromTime, yToTime, garnish);
	GRAPHICS_TWO_END
}

FORM_READ (READ_ONE__Sound_readFromRawFileLE, U"Read Sound from raw Little Endian file", nullptr, true) {
	READ_ONE
		autoSound result = Sound_readFromRawFile (file, nullptr, 16, 1, 0, 0, 16000.0);
	READ_ONE_END
}

FORM_READ (READ_ONE__Sound_readFromRawFileBE, U"Read Sound from raw 16-bit Little Endian file", nullptr, true) {
	READ_ONE
		autoSound result = Sound_readFromRawFile (file, nullptr, 16, 0, 0, 0, 16000.0);
	READ_ONE_END
}

FORM_READ (READ_ONE__KlattTable_readFromRawTextFile, U"KlattTable_readFromRawTextFile", nullptr, true) {
	READ_ONE
		autoKlattTable result = KlattTable_readFromRawTextFile (file);
	READ_ONE_END
}

/************ Spectrograms *********************************************/

FORM (CONVERT_TWO_TO_ONE__Spectrograms_to_DTW, U"Spectrograms: To DTW", nullptr) {
	DTW_constraints_addCommonFields (matchStart, matchEnd, slopeConstraint)
	OK
DO
	CONVERT_TWO_TO_ONE (Spectrogram)
		autoDTW result = Spectrograms_to_DTW (me, you, matchStart, matchEnd, slopeConstraint, 1.0);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (GRAPHICS_EACH__Spectrogram_drawLongtermSpectralFlatness, U"Spectrogram: Draw long-term spectral flatness", U"") {
//double tmin, double tmax, double minimumFlatness_db,
//	double longtermWindow, double shorttermWindow, double fmin, double fmax, bool garnish
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	REAL (minimumFlatness_db, U"Minimum flatness (dB)", U"-30.0")
	POSITIVE (longtimeWindow, U"Long time window", U"0.3")
	POSITIVE (shorttimeWindow, U"Short time window", U"0.1")
	POSITIVE (fmin, U"left Frequency range_(Hz)", U"400.0")
	POSITIVE (fmax, U"right Frequency range_(Hz)", U"4000.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrogram)
		Spectrogram_drawLongTermFlatness (me, GRAPHICS, fromTime, toTime, minimumFlatness_db,
			longtimeWindow, shorttimeWindow, fmin, fmax, garnish);
	GRAPHICS_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Spectrogram_getLongtermSpectralFlatness, U"Spectrogram: Get long-term spectral flatness", nullptr) {
	POSITIVE (longtimeWindow, U"Long time window", U"0.3")
	POSITIVE (shorttimeWindow, U"Short time window", U"0.1")
	POSITIVE (fmin, U"left Frequency range_(Hz)", U"400.0")
	POSITIVE (fmax, U"right Frequency range_(Hz)", U"6000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrogram)
		autoMatrix result = Spectrogram_getLongtermSpectralFlatness (me, longtimeWindow, shorttimeWindow, fmin, fmax);
	CONVERT_EACH_TO_ONE_END (my name.get())
}
/**************** Spectrum *******************************************/

FORM (CONVERT_EACH_TO_ONE__Spectrum_to_Sound_resampled, U"Spectrum: To Sound (resampled)", U"Spectrum: To Sound (resampled)...") {
	INTEGER (interpolationDepth, U"Precision (samples)", U"50")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSound result = Spectrum_to_Sound_resampled (me, interpolationDepth);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (GRAPHICS_EACH__Spectrum_drawPhases, U"Spectrum: Draw phases", U"Spectrum: Draw phases...") {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0")
	REAL (fromPhase, U"Minimum phase (dB/Hz)", U"0.0 (= auto)")
	REAL (toPhase, U"Maximum phase (dB/Hz)", U"0.0 (= auto)")
	BOOLEAN (unwrap, U"Unwrap", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_drawPhases (me, GRAPHICS, fromFrequency, toFrequency, fromPhase, toPhase, unwrap, garnish);
	GRAPHICS_EACH_END
}

FORM (MODIFY_EACH__Spectrum_setRealValueInBin, U"Spectrum: Set real value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		Melder_require (binNumber <= my nx, 
			U"Your bin number should not exceed the number of bins (", my nx, U").");
		my z [1] [binNumber] = value;
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Spectrum_setImaginaryValueInBin, U"Spectrum: Set imaginary value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		Melder_require (binNumber <= my nx, 
			U"Your bin number should not exceed the number of bins (", my nx, U").");
		my z [2] [binNumber] = value;
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Spectrum_conjugate) {
	MODIFY_EACH (Spectrum)
		Spectrum_conjugate (me);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Spectrum_shiftFrequencies, U"Spectrum: Shift frequencies", U"Spectrum: Shift frequencies...") {
	REAL (frequencyShift, U"Shift by (Hz)", U"1000.0")
	POSITIVE (maximumFrequency, U"New maximum frequency (Hz)", U"22050")
	NATURAL (interpolationDepth, U"Interpolation depth", U"50")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrum result = Spectrum_shiftFrequencies (me, frequencyShift, maximumFrequency, interpolationDepth);
	CONVERT_EACH_TO_ONE_END (my name.get(), ( frequencyShift < 0.0 ? U"_m" : U"_" ), Melder_ifloor (fabs (frequencyShift)))
}

DIRECT (CONVERT_TWO_TO_ONE__Spectra_multiply) {
	CONVERT_TWO_TO_ONE (Spectrum)
		autoSpectrum result = Spectra_multiply (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_x_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__Spectrum_resample, U"Spectrum: Resample", nullptr) {
	NATURAL (numberOfFrequencies, U"New number of frequencies", U"256")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrum result = Spectrum_resample (me, numberOfFrequencies);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", numberOfFrequencies)
}

FORM (CONVERT_EACH_TO_ONE__Spectrum_compressFrequencyDomain, U"Spectrum: Compress frequency domain", nullptr) {
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	INTEGER (interpolationDepth, U"Interpolation depth", U"50")
	CHOICE (scale, U"Interpolation scale", 1)
		OPTION (U"linear")
		OPTION (U"logarithmic")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrum result = Spectrum_compressFrequencyDomain (me, maximumFrequency, interpolationDepth, scale, 1);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_ifloor (maximumFrequency))
}

DIRECT (CONVERT_EACH_TO_ONE__Spectrum_unwrap) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoMatrix result = Spectrum_unwrap (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Spectrum_to_PowerCepstrum) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoPowerCepstrum result = Spectrum_to_PowerCepstrum (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Spectrum_to_Cepstrum) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoCepstrum result = Spectrum_to_Cepstrum (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/************* SpeechSynthesizer *************************************************/

DIRECT (HELP__SpeechSynthesizer_help) {
	HELP (U"SpeechSynthesizer")
}

DIRECT (CREATE_ONE__TabulateSpeechSynthesizerLanguageProperties) {
	CREATE_ONE
		autoTable result = Data_copy (theSpeechSynthesizerLanguagePropertiesTable);
	CREATE_ONE_END (U"theSpeechSynthesizerLanguageProperties")
}

DIRECT (CREATE_ONE__TabulateSpeechSynthesizerVoiceProperties) {
	CREATE_ONE
		autoTable result = Data_copy (theSpeechSynthesizerVoicePropertiesTable);
	CREATE_ONE_END (U"theSpeechSynthesizerVoiceProperties")
}

FORM (CREATE_ONE__SpeechSynthesizer_create, U"Create SpeechSynthesizer", U"Create SpeechSynthesizer...") {
	LISTNUMSTR (languageIndex, languageName, U"Language", theSpeechSynthesizerLanguageNames, NUMfindFirst (theSpeechSynthesizerLanguageNames, U"English (Great Britain)"))
	LISTNUMSTR (voiceIndex, voiceName, U"Voice", theSpeechSynthesizerVoiceNames, (int) NUMfindFirst (theSpeechSynthesizerVoiceNames, U"Female1"))
	OK
DO
	CREATE_ONE
		autoSpeechSynthesizer result = SpeechSynthesizer_create (languageName, voiceName);
		languageName = result -> d_languageName.get();   // save before moving
		voiceName = result -> d_voiceName.get();   // save before moving
	CREATE_ONE_END (languageName, U"_", voiceName)   // the names may have been "repaired"
}

FORM (WINDOW_SpeechSynthesizer_viewAndEdit, U"View & Edit SpeechSynthesizer", nullptr) {
	LIST (languageIndex, U"Language", theSpeechSynthesizerLanguageNames, NUMfindFirst (theSpeechSynthesizerLanguageNames, U"English (Great Britain)"))
	LIST (voiceIndex, U"Voice", theSpeechSynthesizerVoiceNames, NUMfindFirst (theSpeechSynthesizerVoiceNames, U"Female1"))
	LIST (phonemeSetIndex, U"Phoneme set", theSpeechSynthesizerLanguageNames, (int) NUMfindFirst (theSpeechSynthesizerLanguageNames, U"English (Great Britain)"))
OK
	FIND_ONE (SpeechSynthesizer)
		SET_INTEGER (languageIndex, (int) NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_languageName.get()))
		SET_INTEGER (voiceIndex, (int) NUMfindFirst (theSpeechSynthesizerVoiceNames, my d_voiceName.get()))
		SET_INTEGER (phonemeSetIndex, (int) NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_phonemeSetName.get()))
DO
	FIND_ONE (SpeechSynthesizer)
		my d_languageName = Melder_dup (theSpeechSynthesizerLanguageNames [languageIndex]);
		my d_voiceName = Melder_dup (theSpeechSynthesizerVoiceNames [voiceIndex]);
		my d_phonemeSetName = Melder_dup (theSpeechSynthesizerLanguageNames [phonemeSetIndex]);
		SpeechSynthesizer_checkAndRepairLanguageAndVoiceNames (me);
	END_NO_NEW_DATA
}

FORM (PLAY_EACH__SpeechSynthesizer_playText, U"SpeechSynthesizer: Play text", U"SpeechSynthesizer: Play text...") {
	TEXTFIELD (text, U"Text", U"This is some text.", 10)
	OK
DO
	PLAY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_playText (me, text);
	PLAY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__SpeechSynthesizer_to_Sound, U"SpeechSynthesizer: To Sound", U"SpeechSynthesizer: To Sound...") {
	TEXTFIELD (text, U"Text", U"This is some text.", 10)
	BOOLEAN (wantTextGrid, U"Create TextGrid with annotations", false);
	OK
DO
	CONVERT_EACH_TO_MULTIPLE (SpeechSynthesizer)
		autoTextGrid tg;
		autoTable t;
		autoSound result = SpeechSynthesizer_to_Sound (
			me, text, (wantTextGrid ? & tg : nullptr), (Melder_debug == -2 ? & t : nullptr )
		);
		if (wantTextGrid)
			praat_new (tg.move(), my name.get());
		if (Melder_debug == -2)
			praat_new (t.move(), my name.get());
		praat_new (result.move(), my name.get());
	CONVERT_EACH_TO_MULTIPLE_END
}

DIRECT (QUERY_ONE_FOR_STRING__SpeechSynthesizer_getLanguageName) {
	QUERY_ONE_FOR_STRING (SpeechSynthesizer)
		conststring32 result = my d_languageName.get();
	QUERY_ONE_FOR_STRING_END
}

DIRECT (QUERY_ONE_FOR_STRING__SpeechSynthesizer_getVoiceName) {
	QUERY_ONE_FOR_STRING (SpeechSynthesizer)
		conststring32 result = my d_voiceName.get();
	QUERY_ONE_FOR_STRING_END
}

DIRECT (QUERY_ONE_FOR_STRING__SpeechSynthesizer_getPhonemeSetName) {
	QUERY_ONE_FOR_STRING (SpeechSynthesizer)
		conststring32 result = my d_phonemeSetName.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_AUTOSTRING__SpeechSynthesizer_getPhonemesFromText, U"SpeechSynthesizer: Get phonemes from text", nullptr) {
	TEXTFIELD (text, U"Text", U"This is some text.", 10)
	OK
DO
	QUERY_ONE_FOR_AUTOSTRING (SpeechSynthesizer)
		autostring32 result = SpeechSynthesizer_getPhonemesFromText (me, text, false);
	QUERY_ONE_FOR_AUTOSTRING_END
}

FORM (QUERY_ONE_FOR_AUTOSTRING__SpeechSynthesizer_getPhonemesFromTextSpaceSeparated, U"SpeechSynthesizer: Get phonemes from text", nullptr) {
	TEXTFIELD (text, U"Text", U"This is some text.", 10)
	OK
DO
	QUERY_ONE_FOR_AUTOSTRING (SpeechSynthesizer)
		autostring32 result = SpeechSynthesizer_getPhonemesFromText (me, text, true);
	QUERY_ONE_FOR_AUTOSTRING_END
}

FORM (MODIFY_EACH__SpeechSynthesizer_setTextInputSettings, U"SpeechSynthesizer: Set text input settings", U"SpeechSynthesizer: Set text input settings...") {
	OPTIONMENU (inputTextFormat, U"Input text format is", 3)
		OPTION (U"text only")
		OPTION (U"phoneme codes only")
		OPTION (U"mixed with tags")
	OPTIONMENU (inputPhonemeCoding, U"Input phoneme codes are", 1)
		OPTION (U"Kirshenbaum_espeak")
	OK
DO
	const int inputPhonemeCoding_always = SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM;
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setTextInputSettings (me, inputTextFormat, inputPhonemeCoding_always);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__SpeechSynthesizer_estimateSpeechRateFromSpeech, U"SpeechSynthesizer: Estimate speech rate from speech", U"SpeechSynthesizer: Estimate speech rate from speech...") {
	BOOLEAN (estimate, U"Estimate speech rate from speech", true);
	OK
DO
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setEstimateSpeechRateFromSpeech (me, estimate);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__SpeechSynthesizer_speechOutputSettings, U"SpeechSynthesizer: Speech output settings", U"SpeechSynthesizer: Speech output settings...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	REAL (wordGap, U"Gap between words (s)", U"0.01")
	POSITIVE (pitchMultiplier, U"Pitch multiplier (0.5-2.0)", U"1.0")
	REAL (pitchRangeMultiplier, U"Pitch range multiplier (0-2.0)", U"1.0");
	POSITIVE (wordsPerMinute, U"Words per minute (80-450)", U"175.0");
	OPTIONMENU (outputPhonemeCodes, U"Output phoneme codes are", 2)
		OPTION (U"Kirshenbaum_espeak")
		OPTION (U"IPA")
	OK
DO
	Melder_clipLeft (0.0, & wordGap);
	Melder_require (pitchMultiplier >= 0.5 && pitchMultiplier <= 2.0,
		U"The pitch multiplier should be between 0.5 and 2.0.");
	Melder_require (pitchRangeMultiplier >= 0.0 && pitchRangeMultiplier <= 2.0,
		U"The pitch range multiplier should be between 0.0 and 2.0.");
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setSpeechOutputSettings (
			me, samplingFrequency, wordGap, pitchMultiplier, pitchRangeMultiplier, wordsPerMinute, outputPhonemeCodes
		);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__SpeechSynthesizer_setSpeechOutputSettings, U"SpeechSynthesizer: Set speech output settings", U"SpeechSynthesizer: Speech output settings...") {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	REAL (wordGap, U"Gap between words (s)", U"0.01")
	INTEGER (pitchAdjustment_0_99, U"Pitch adjustment (0-99)", U"50")
	INTEGER (pitchRange_0_99, U"Pitch range (0-99)", U"50");
	NATURAL (wordsPerMinute, U"Words per minute (80-450)", U"175");
	BOOLEAN (estimateWordsPerMinute, U"Estimate rate from data", true);
	OPTIONMENU (outputPhonemeCodes, U"Output phoneme codes are", 2)
		OPTION (U"Kirshenbaum_espeak")
		OPTION (U"IPA")
	OK
DO
	Melder_clipLeft (0.0, & wordGap);
	Melder_clip (0_integer, & pitchAdjustment_0_99, 99_integer);
	Melder_clip (0_integer, & pitchRange_0_99, 99_integer);
	const double pitchAdjustment = (1.5 / 99.0 * pitchAdjustment_0_99 + 0.5);
	const double pitchRange = pitchRange_0_99 / 49.5;
	MODIFY_EACH (SpeechSynthesizer)
		SpeechSynthesizer_setSpeechOutputSettings (
			me, samplingFrequency, wordGap, pitchAdjustment, pitchRange, wordsPerMinute, outputPhonemeCodes
		);
		SpeechSynthesizer_setEstimateSpeechRateFromSpeech (me, estimateWordsPerMinute);
	MODIFY_EACH_END
}

/************* SpeechSynthesizer and TextGrid ************************/

FORM (CONVERT_ONE_AND_ONE_TO_ONE__SpeechSynthesizer_TextGrid_to_Sound, U"SpeechSynthesizer & TextGrid: To Sound", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	NATURAL (intervalNumber, U"Interval number", U"1")
	BOOLEAN (createAnnotations, U"Create TextGrid with annotations", false);
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (SpeechSynthesizer, TextGrid)
		autoTextGrid annotations;
		autoSound result = SpeechSynthesizer_TextGrid_to_Sound (
			me, you, tierNumber, intervalNumber, (createAnnotations ? & annotations : nullptr)
		);
		if (createAnnotations)
			praat_new (annotations.move(), my name.get());
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

FORM (CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE__SpeechSynthesizer_Sound_TextGrid_align, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align)", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	NATURAL (fromInterval, U"From interval number", U"1")
	NATURAL (toInterval, U"To interval number", U"1")
	REAL (silenceThreshold_dB, U"Silence threshold (dB)", U"-35.0")
	POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
	POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
	OK
DO
	CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE (SpeechSynthesizer, Sound, TextGrid)
		autoTextGrid result = SpeechSynthesizer_Sound_TextGrid_align (
			me, you, him, tierNumber, fromInterval, toInterval, silenceThreshold_dB, 
			minimumSilenceDuration, minimumSoundingDuration
		);
	CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE_END (your name.get(), U"_aligned")
}

FORM (CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE__SpeechSynthesizer_Sound_TextGrid_align2, U"SpeechSynthesizer & Sound & TextGrid: To TextGrid (align, trim)", nullptr) {
    NATURAL (tierNumber, U"Tier number", U"1")
    NATURAL (fromInterval, U"From interval number", U"1")
    NATURAL (toInterval, U"To interval number", U"1")
    REAL (silenceThreshold_dB, U"Silence threshold (dB)", U"-35.0")
    POSITIVE (minimumSilenceDuration, U"Minimum silent interval duration (s)", U"0.1")
    POSITIVE (minimumSoundingDuration, U"Minimum sounding interval duration (s)", U"0.1")
    REAL (trimDuration, U"Silence trim duration (s)", U"0.08")
    OK
DO
	Melder_require (trimDuration >= 0.0,
		U"The \"Silence trim duration\" should not be negative."); 
    CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE (SpeechSynthesizer, Sound, TextGrid)
		autoTextGrid result = SpeechSynthesizer_Sound_TextGrid_align2 (
			me, you, him, tierNumber, fromInterval, toInterval, silenceThreshold_dB, minimumSilenceDuration,
			minimumSoundingDuration, trimDuration
		);
    CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE_END (his name.get(), U"_aligned")
}

/************* Spline *************************************************/

FORM (GRAPHICS_EACH__Spline_drawKnots, U"Spline: Draw knots", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spline)
		Spline_drawKnots (me, GRAPHICS, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__Spline_getOrder) {
	QUERY_ONE_FOR_INTEGER (Spline)
		const integer result = Spline_getOrder (me);
	QUERY_ONE_FOR_INTEGER_END (U" (order)")
}

FORM (CONVERT_EACH_TO_ONE__Spline_scaleX, U"Spline: Scale x", U"Spline: Scale x...") {
	COMMENT (U"New domain")
	REAL (xmin, U"Xmin", U"-1.0")
	REAL (xmax, U"Xmax", U"1.0")
	OK
DO
	Melder_require (xmin < xmax, 
		U"\"Xmin\" should be smaller than \"Xmax\".");
	CONVERT_EACH_TO_ONE (Spline)
		autoSpline result = Spline_scaleX (me, xmin, xmax);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_scaleX")
}

/************ SSCP ***************************************************/

DIRECT (HELP__SSCP_help) {
	HELP (U"SSCP")
}

FORM (GRAPHICS_EACH__SSCP_drawConfidenceEllipse, U"SSCP: Draw confidence ellipse", nullptr) {
	POSITIVE (confidenceLevel, U"Confidence level", U"0.95")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SSCP)
		SSCP_drawConcentrationEllipse (me, GRAPHICS, confidenceLevel, 1, xIndex, yIndex, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__SSCP_drawSigmaEllipse, U"SSCP: Draw sigma ellipse", U"SSCP: Draw sigma ellipse...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SSCP)
		SSCP_drawConcentrationEllipse (me, GRAPHICS, numberOfSigmas, 0, xIndex, yIndex, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__SSCP_extractCentroid) {
	CONVERT_EACH_TO_ONE (SSCP)
		autoTableOfReal result = SSCP_extractCentroid (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_centroid")
}

FORM (QUERY_ONE_FOR_REAL__SSCP_getConfidenceEllipseArea, U"SSCP: Get confidence ellipse area", U"SSCP: Get confidence ellipse area...") {
	POSITIVE (confidenceLevel, U"Confidence level", U"0.95")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	OK
DO
	QUERY_ONE_FOR_REAL (SSCP)
		const double result = SSCP_getConcentrationEllipseArea (me, confidenceLevel, 1, xIndex, yIndex);
	QUERY_ONE_FOR_REAL_END (U" (confidence ellipse area)")
}

FORM (QUERY_ONE_FOR_REAL__SSCP_getFractionVariation, U"SSCP: Get fraction variation", U"SSCP: Get fraction variation...") {
	NATURAL (fromDimension, U"From dimension", U"1")
	NATURAL (toDimension, U"To dimension", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (SSCP)
		const double result = SSCP_getFractionVariation (me, fromDimension, toDimension);
	QUERY_ONE_FOR_REAL_END (U" (fraction)")
}


FORM (QUERY_ONE_FOR_REAL__SSCP_getConcentrationEllipseArea, U"SSCP: Get sigma ellipse area", U"SSCP: Get sigma ellipse area...") {
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	NATURAL (xIndex, U"Index for X-axis", U"1")
	NATURAL (yIndex, U"Index for Y-axis", U"2")
	OK
DO
	QUERY_ONE_FOR_REAL (SSCP)
		const double result = SSCP_getConcentrationEllipseArea (me, numberOfSigmas, 0, xIndex, yIndex);
	QUERY_ONE_FOR_REAL_END (U" (concentration ellipse area)")
}

DIRECT (QUERY_ONE_FOR_REAL__SSCP_getDegreesOfFreedom) {
	QUERY_ONE_FOR_REAL (SSCP)
		const double result = SSCP_getDegreesOfFreedom (me);
	QUERY_ONE_FOR_REAL_END (U" (degrees of freedom)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__SSCP_getNumberOfObservations) {
	QUERY_ONE_FOR_INTEGER (SSCP)
		const integer result = Melder_ifloor (my numberOfObservations);   // ppgb: blijf ik raar vinden
	QUERY_ONE_FOR_INTEGER_END (U" (number of observations)")
}

DIRECT (QUERY_ONE_FOR_REAL__SSCP_getTotalVariance) {
	QUERY_ONE_FOR_REAL (SSCP)
		const double result = SSCP_getTotalVariance (me);
	QUERY_ONE_FOR_REAL_END (U" (total variance)")
}

FORM (QUERY_ONE_FOR_REAL__SSCP_getCentroidElement, U"SSCP: Get centroid element", U"SSCP: Get centroid element") {
	NATURAL (number, U"Number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (SSCP)
		double result = undefined;
		if (number > 0 && number <= my numberOfColumns) {
			result = my centroid [number];
		}
	QUERY_ONE_FOR_REAL_END (U" (centroid [", number, U"])")
}

DIRECT (QUERY_ONE_FOR_REAL__SSCP_getLnDeterminant) {
	QUERY_ONE_FOR_REAL (SSCP)
		const double result = SSCP_getLnDeterminant (me);
	QUERY_ONE_FOR_REAL_END (U" (ln (determinant))")
}

FORM (QUERY_ONE_FOR_REAL__SSCP_getDiagonality_bartlett, U"SSCP: Get diagonality (bartlett)", U"SSCP: Get diagonality (bartlett)...") {
	NATURAL (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (SSCP)
		double chisq, result, ndof;
		SSCP_getDiagonality_bartlett (me, numberOfConstraints, & chisq, & result, & ndof);
	QUERY_ONE_FOR_REAL_END (U" (probability for chisq = ", chisq, U" and ndf = ", ndof, U")")
}

DIRECT (CONVERT_EACH_TO_ONE__SSCP_to_Correlation) {
	CONVERT_EACH_TO_ONE (SSCP)
		autoCorrelation result = SSCP_to_Correlation (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__SSCP_to_Covariance, U"SSCP: To Covariance", U"SSCP: To Covariance...") {
	NATURAL (numberOfConstraints, U"Number of constraints", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (SSCP)
		autoCovariance result = SSCP_to_Covariance (me, numberOfConstraints);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__SSCP_to_PCA) {
	CONVERT_EACH_TO_ONE (SSCP)
		autoPCA result = SSCP_to_PCA (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/******************* Strings ****************************/

FORM (CREATE_ONE__Strings_createAsCharacters, U"Strings: Create as characters", nullptr) {
	SENTENCE (text, U"Text", U"intention")
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsCharacters (text);
	CREATE_ONE_END (U"chars")
}

FORM (CREATE_ONE__old_Strings_createAsTokens, U"Strings: Create as tokens", nullptr) {
	TEXTFIELD (text, U"Text", U"There are seven tokens in this text", 10)
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsTokens (text, U" ");
	CREATE_ONE_END (U"tokens")
}

FORM (CREATE_ONE__Strings_createFromTokens, U"Create Strings from tokens", U"Create Strings from tokens...") {
	WORD (name, U"Name", U"tokens")
	TEXTFIELD (text, U"Text", U"There are seven tokens in this text", 10)
	SENTENCE (separators, U"Separators", U" ,")
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsTokens (text, separators);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Strings_createAsTokens, U"Strings: Create as tokens", U"Create Strings as tokens...") {
	TEXTFIELD (text, U"Text", U"There are seven tokens in this text", 10)
	SENTENCE (separators, U"Separators", U" ,")
	OK
DO_ALTERNATIVE (CREATE_ONE__old_Strings_createAsTokens)
	CREATE_ONE
		autoStrings result = Strings_createAsTokens (text, separators);
	CREATE_ONE_END (U"tokens")
}

DIRECT (COMBINE_ALL_TO_ONE__Strings_append) {
	COMBINE_ALL_TO_ONE (Strings)
		autoStrings result = Strings_append (& list);
	COMBINE_ALL_TO_ONE_END (U"appended")
}

DIRECT (CONVERT_EACH_TO_ONE__Strings_to_Categories) {
	CONVERT_EACH_TO_ONE (Strings)
		autoCategories result = Strings_to_Categories (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Strings_change, U"Strings: Change", U"Strings: Change") {
	SENTENCE (search_string, U"Search", U"a")
	SENTENCE (replace_string, U"Replace", U"a")
	INTEGER (replaceLimit, U"Replace limit", U"0 (= unlimited)")
	CHOICE (stringType, U"Search and replace are:", 1)
		OPTION (U"literals")
		OPTION (U"Regular Expressions")
	OK
DO
	CONVERT_EACH_TO_ONE (Strings)
		integer nmatches, nstringmatches;
		autoStrings result = Strings_change (
			me, search_string, replace_string, replaceLimit, &nmatches, &nstringmatches, stringType - 1
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Strings_extractPart, U"Strings: Extract part", nullptr) {
	NATURAL (fromIndex, U"From index", U"1")
	NATURAL (toIndex, U"To index", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Strings)
		autoStrings result = Strings_extractPart (me, fromIndex, toIndex);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_part");
}

DIRECT (CONVERT_TWO_TO_ONE__Strings_to_EditDistanceTable) {
	CONVERT_TWO_TO_ONE (Strings)
		autoEditDistanceTable result = EditDistanceTable_create (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__Strings_to_StringsIndex, U"Strings: To StringsIndex", nullptr) {
	OPTIONMENU_ENUM (kStrings_sorting, sorting, U"Sorting method", kStrings_sorting::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Strings)
		autoStringsIndex result = Strings_to_StringsIndex (me, sorting);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Strings_to_Permutation_old, U"Strings: To Permutation", U"Strings: To Permutation...") {
	BOOLEAN (sort, U"Sort", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Strings)
		autoPermutation result = Strings_to_Permutation (me, (sort ? kStrings_sorting::ALPHABETICAL : kStrings_sorting::NONE));
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Strings_to_Permutation, U"Strings: To Permutation", U"Strings: To Permutation...") {
	OPTIONMENU_ENUM (kStrings_sorting, sortingMethod, U"Sorting", kStrings_sorting::DEFAULT)
	OK
DO_ALTERNATIVE (CONVERT_EACH_TO_ONE__Strings_to_Permutation_old)	
	CONVERT_EACH_TO_ONE (Strings)
		autoPermutation result = Strings_to_Permutation (me, sortingMethod);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__Strings_Permutation_permuteStrings) {
	CONVERT_ONE_AND_ONE_TO_ONE (Strings, Permutation)
		autoStrings result = Strings_Permutation_permuteStrings (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

/****************************** SVD *****************************************/

DIRECT (HELP__SVD_help) {
	HELP (U"singular value decomposition")
}

DIRECT (QUERY_ONE_FOR_INTEGER__SVD_getNumberOfRows) {
	QUERY_ONE_FOR_INTEGER (SVD)
		const integer result = ( my isTransposed ? my numberOfColumns : my numberOfRows );
	QUERY_ONE_FOR_INTEGER_END (U" (number of rows)")	
}

DIRECT (QUERY_ONE_FOR_INTEGER__SVD_getNumberOfColumns) {
	QUERY_ONE_FOR_INTEGER (SVD)
		const integer result = ( my isTransposed ? my numberOfRows : my numberOfColumns );
	QUERY_ONE_FOR_INTEGER_END (U" (number of columns)")
}

DIRECT (QUERY_ONE_FOR_REAL__SVD_getRank) {
	QUERY_ONE_FOR_REAL (SVD)
		const double result = SVD_getRank (me);
	QUERY_ONE_FOR_REAL_END (U" (rank)")
}

DIRECT (QUERY_ONE_FOR_REAL__SVD_getConditionNumber) {
	QUERY_ONE_FOR_REAL (SVD)
		const double result = SVD_getConditionNumber (me);
	QUERY_ONE_FOR_REAL_END (U" (condition number)")
}

FORM (QUERY_ONE_FOR_REAL__SVD_getSingularValue, U"SVD: Get singular values", nullptr) {
	NATURAL (index, U"Index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (SVD)
		Melder_require (index <= my numberOfColumns, 
			U"Index must be in the range [1,", my numberOfColumns, U"].");
		const double result = my d [index];
	QUERY_ONE_FOR_REAL_END (U" (singular value ", index, U")")
}

FORM (QUERY_ONE_FOR_REAL__SVD_getSumOfSingularValues, U"SVD: Get sum of singular values", nullptr) {
	NATURAL (from, U"From", U"1")
	INTEGER (to, U"To", U"0 (= last)")
	OK
DO
	QUERY_ONE_FOR_REAL (SVD)
		const double result = SVD_getSumOfSingularValues (me, from, to);
	QUERY_ONE_FOR_REAL_END (U" (sum of singular values)")
}

FORM (QUERY_ONE_FOR_REAL__SVD_getSumOfSingularValuesAsFractionOfTotal, U"SVD: Get sum of singular values as fraction of total", nullptr) {
	NATURAL (from, U"From", U"1")
	INTEGER (to, U"To", U"0 (= last)")
	OK
DO
	QUERY_ONE_FOR_REAL (SVD)
		const double result = SVD_getSumOfSingularValuesAsFractionOfTotal (me, from, to);
	QUERY_ONE_FOR_REAL_END (U" (fraction of total sum of singular values)")
}

FORM (QUERY_ONE_FOR_INTEGER__SVD_getMinimumNumberOfSingularValues, U"SVD: Get minimum number of singular values", U"SVD: Get minimum number of singular values...") {
	POSITIVE (fraction, U"Fraction of total sum", U"0.95")
	OK
DO
	Melder_require (fraction <= 1.0, 
		U"Fraction must be a number in (0,1).");
	QUERY_ONE_FOR_INTEGER (SVD)
		const integer result = SVD_getMinimumNumberOfSingularValues (me, fraction);
	QUERY_ONE_FOR_INTEGER_END (U" (number of singular values needed)")
}

FORM (QUERY_ONE_FOR_REAL__SVD_getShrinkageParameter, U"SVD: Get shrinkage parameter", nullptr) {
	POSITIVE (edf, U"Effective degrees of freedom", U"3.0")
	OK
DO
	QUERY_ONE_FOR_REAL (SVD)
		const double result = SVD_getShrinkageParameter (me, edf);
	QUERY_ONE_FOR_REAL_END (U" (shrinkage parameter for ridge regression with ", edf, U" effective degrees of freedom)")
}

FORM (QUERY_ONE_FOR_REAL__SVD_getEffectiveDegreesOfFreedom, U"SVD: Get effective degrees of freedom", nullptr) {
	POSITIVE (lambda, U"Shrinkage parameter", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (SVD)
		const double result = SVD_getEffectiveDegreesOfFreedom (me, lambda);
	QUERY_ONE_FOR_REAL_END (U" (effective degrees of freedom for ridge regression with ", lambda, U" shrinkage factor)")
}

FORM (CONVERT_EACH_TO_ONE__SVD_to_Matrix, U"SVD: To Matrix", U"SVD: To Matrix...") {
	NATURAL (fromComponent, U"First component", U"1")
	INTEGER (toComponent, U"Last component", U"0 (= all)")
	OK
DO
	CONVERT_EACH_TO_ONE (SVD)
		autoMatrix result = SVD_to_Matrix (me, fromComponent, toComponent);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__SVD_to_TableOfReal, U"SVD: To TableOfReal", U"SVD: To TableOfReal...") {
	NATURAL (fromComponent, U"First component", U"1")
	INTEGER (toComponent, U"Last component", U"0 (= all)")
	OK
DO
	CONVERT_EACH_TO_ONE (SVD)
		autoTableOfReal result = SVD_to_TableOfReal (me, fromComponent, toComponent);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__SVD_extractLeftSingularVectors) {
	CONVERT_EACH_TO_ONE (SVD)
		autoTableOfReal result = SVD_extractLeftSingularVectors (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_lsv")
}

DIRECT (CONVERT_EACH_TO_ONE__SVD_extractRightSingularVectors) {
	CONVERT_EACH_TO_ONE (SVD)
		autoTableOfReal result = SVD_extractRightSingularVectors (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rsv")
}

DIRECT (CONVERT_EACH_TO_ONE__SVD_extractSingularValues) {
	CONVERT_EACH_TO_ONE (SVD)
		autoTableOfReal result = SVD_extractSingularValues (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_sv");
}

/******************* Table ****************************/

DIRECT (CREATE_ONE__Table_create_petersonBarney1952) {
	CREATE_ONE
		autoTable result = Table_create_petersonBarney1952 ();
	CREATE_ONE_END (U"pb")
}

DIRECT (CREATE_ONE__Table_create_hillenbrandEtAl1995) {
	CREATE_ONE
		autoTable result = Table_create_hillenbrandEtAl1995 ();
	CREATE_ONE_END (U"h95")
}

DIRECT (CREATE_ONE__Table_create_polsVanNierop1973) {
	CREATE_ONE
		autoTable result = Table_create_polsVanNierop1973 ();
	CREATE_ONE_END (U"pvn")
}

DIRECT (CREATE_ONE__Table_create_weenink1983) {
	CREATE_ONE
		autoTable result = Table_create_weenink1983 ();
	CREATE_ONE_END (U"m10w10c10")
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_scatterPlotWhere, U"Table: Scatter plot where", nullptr) {
	WORD (xColumnName, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	WORD (markColumnName, U"Column with marks", U"")
	POSITIVE (fontSize, U"Font size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"Use data only from rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber    = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber    = Table_columnNameToNumber_e (me, yColumnName);
		const integer markColumnNumber = Table_columnNameToNumber_e (me, markColumnName);
		autoTable part = Table_extractRowsWhere (me, condition, interpreter);
		Table_scatterPlot (part.get(), GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax, markColumnNumber, fontSize, garnish);
	GRAPHICS_EACH_END
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_scatterPlotMarkWhere, U"Scatter plot where (marks)", nullptr) {
	WORD (xColumnName, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (markSize_mm, U"Mark size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true)
	SENTENCE (mark_string, U"Mark string (+xo.)", U"+")
	COMMENT (U"Use data only from rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		autoTable part = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_scatterPlot_mark (part.get(), GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax, markSize_mm, mark_string, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_barPlotWhere, U"Table: Bar plot where", U"Table: Bar plot...") { // deprecated 2023
	STRINGARRAY_LINES (2, columnNames, U"Vertical column(s)", { U"speaker", U"age" })
	///SENTENCE (yColumns_string, U"Vertical column(s)", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (labelColumnName, U"Column with labels", U"")
	COMMENT (U"Distances are in units of 'bar width'")
	REAL (distanceFromBorder, U"Distance of first bar from border", U"1.0")
	REAL (distanceBetweenGroups, U"Distance between bar groups", U"1.0")
	REAL (distanceWithinGroup, U"Distance between bars within group", U"0.0")
	STRINGARRAY_LINES (2, colours, U"Colours", { U"Grey", U"Black" })
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"Use data only from rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"row >= 1 and row <= 8")
	OK
DO
	GRAPHICS_EACH (Table)
		autoINTVEC columnNumbers = Table_columnNamesToNumbers (me, columnNames);
		const integer labelColumnNumber = Table_columnNameToNumber_0 (me, labelColumnName); // can be empty
		autoTable part = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_barPlot (part.get(), GRAPHICS, columnNumbers.get(), ymin, ymax, labelColumnNumber, distanceFromBorder,
				distanceWithinGroup, distanceBetweenGroups, colours, angle, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_barPlot, U"Table: Bar plot", U"Table: Bar plot...") {
	STRINGARRAY_LINES (2, columnNames, U"Vertical column(s)", { U"speaker", U"age" })
	///SENTENCE (yColumns_string, U"Vertical column(s)", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (labelColumnName, U"Column with labels", U"")
	COMMENT (U"Distances are in units of 'bar width'")
	REAL (distanceFromBorder, U"Distance of first bar from border", U"1.0")
	REAL (distanceBetweenGroups, U"Distance between bar groups", U"1.0")
	REAL (distanceWithinGroup, U"Distance between bars within group", U"0.0")
	STRINGARRAY_LINES (2, colours, U"Colours", { U"Grey", U"Black" })
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		autoINTVEC columnNumbers = Table_columnNamesToNumbers (me, columnNames);
		const integer labelColumnNumber = Table_columnNameToNumber_0 (me, labelColumnName); // can be empty
		Table_barPlot (me, GRAPHICS, columnNumbers.get(), ymin, ymax, labelColumnNumber, distanceFromBorder,
				distanceWithinGroup, distanceBetweenGroups, colours, angle, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_LineGraphWhere, U"Table: Line graph where", U"Table: Line graph...") {
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (xColumnName, U"Horizontal column (optional)", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (text, U"Text", U"+")
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"Use data only from rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; (= everything)")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer xColumnNumber = str32equ (xColumnName, U"") ? 0 : Table_columnNameToNumber_e (me, xColumnName);
		autoTable part = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_lineGraph_old (part.get(), GRAPHICS, xColumnNumber, xmin, xmax, yColumnNumber, ymin, ymax, text,
				angle, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_LineGraph_old, U"Table: Line graph", U"Table: Line graph...") {
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (xColumnName, U"Horizontal column (optional)", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (text, U"Text", U"+")
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer xColumnNumber = str32equ (xColumnName, U"") ? 0 : Table_columnNameToNumber_e (me, xColumnName);
		Table_lineGraph_old (me, GRAPHICS, xColumnNumber, xmin, xmax, yColumnNumber, ymin, ymax, text,
				angle, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_LineGraph, U"Table: Line graph", U"Table: Line graph...") {
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (xColumnName, U"Horizontal column (optional)", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	WORD (text, U"Text", U"+")
	REAL (textFontSize, U"Text font size", U"12")
	REAL (angle, U"Label text angle (degrees)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO_ALTERNATIVE (GRAPHICS_EACH__Table_LineGraph_old)
	GRAPHICS_EACH (Table)
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer xColumnNumber = str32equ (xColumnName, U"") ? 0 : Table_columnNameToNumber_e (me, xColumnName);
		Table_lineGraph (me, GRAPHICS, xColumnNumber, xmin, xmax, yColumnNumber, ymin, ymax, text,
				textFontSize, angle, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_boxPlots, U"Table: Box plots", U"Table: Box plots...") {
	STRINGARRAY_LINES (2, dataColumnNames, U"Data columns", { U"F1", U"F2" })
	WORD (factorColumnName, U"Factor column", U"Sex")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer factorColumnNumber = Table_columnNameToNumber_e (me, factorColumnName);
		autoINTVEC dataColumnNumbers = Table_columnNamesToNumbers (me, dataColumnNames);
		Table_boxPlots (me, GRAPHICS, dataColumnNumbers.get(), factorColumnNumber, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_boxPlotsWhere, U"Table: Box plots where", U"Table: Box plots...") { // deprecated 2023
	STRINGARRAY_LINES (2, dataColumnNames, U"Data columns", { U"F1", U"F2" })
	WORD (factorColumnName, U"Factor column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true);
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer factorColumnNumber = Table_columnNameToNumber_e (me, factorColumnName);
		autoINTVEC dataColumnNumbers = Table_columnNamesToNumbers (me, dataColumnNames);
		autoTable part = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_boxPlots (part.get(), GRAPHICS, dataColumnNumbers.get(), factorColumnNumber, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_drawEllipseWhere, U"Draw ellipse (standard deviation)", nullptr) { // deprecated 2023
	SENTENCE (xColumnName, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"2.0")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		autoTable thee = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_drawEllipse_e (thee.get(), GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax, numberOfSigmas, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_drawEllipses, U"Table: Draw ellipses", nullptr) {
	SENTENCE (xColumnName, U"Horizontal column", U"F2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (yColumnName, U"Vertical column", U"F1")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (factorColumnName, U"Factor column", U"Vowel")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (fontSize, U"Label font size", U"12.0 ; (0 = no label)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber      = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber      = Table_columnNameToNumber_e (me, yColumnName);
		const integer factorColumnNumber = Table_columnNameToNumber_e (me, factorColumnName);
		Table_drawEllipses (me, GRAPHICS, xColumnNumber, yColumnNumber, factorColumnNumber,
				xmin, xmax, ymin, ymax, numberOfSigmas, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_drawEllipsesWhere, U"Table: Draw ellipses where", nullptr) { // deprecated 2023
	SENTENCE (xColumnName, U"Horizontal column", U"F2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (yColumnName, U"Vertical column", U"F1")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (factorColumnName, U"Factor column", U"Vowel")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"1.0")
	REAL (fontSize, U"Label font size", U"12.0 ; (0 = no label)")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber      = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber      = Table_columnNameToNumber_e (me, yColumnName);
		const integer factorColumnNumber = Table_columnNameToNumber_e (me, factorColumnName);
		Table_extractRowsWhere_e (me, condition, interpreter);
		Table_drawEllipses (me, GRAPHICS, xColumnNumber, yColumnNumber, factorColumnNumber,
			xmin, xmax, ymin, ymax, numberOfSigmas, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_normalProbabilityPlot, U"Table: Normal probability plot", U"Table: Normal probability plot...") {
	SENTENCE (columnName, U"Column", U"F1")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	SENTENCE (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		Table_normalProbabilityPlot (me, GRAPHICS, columnNumber, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_normalProbabilityPlotWhere, U"Table: Normal probability plot where", U"Table: Normal probability plot...") {
	SENTENCE (columnName, U"Column", U"F0")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (numberOfSigmas, U"Number of sigmas", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	SENTENCE (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const autoTable thee = Table_extractRowsWhere (me, condition, interpreter);
		Table_normalProbabilityPlot (thee.get(), GRAPHICS, columnNumber, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_quantileQuantilePlot, U"Table: Quantile-quantile plot", U"Table: Quantile-quantile plot...") {
	SENTENCE (xColumnName, U"Horizontal axis column", U"")
	SENTENCE (yColumnName, U"Vertical axis column", U"")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	SENTENCE (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		Table_quantileQuantilePlot (me, GRAPHICS, xColumnNumber, yColumnNumber,
				numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_quantileQuantilePlot_betweenLevels, U"Table: Quantile-quantile plot (between levels)", U"Table: Quantile-quantile plot...") {
	SENTENCE (dataColumnName, U"Data column", U"F0")
	SENTENCE (factorColumnName, U"Factor column", U"Sex")
	SENTENCE (xLevel_string, U"Horizontal factor level", U"")
	SENTENCE (yLevelString, U"Vertical factor level", U"")
	NATURAL (numberOfQuantiles, U"Number of quantiles", U"100")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	SENTENCE (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer dataColumnNumber = Table_columnNameToNumber_e (me, dataColumnName);
		const integer factorColumnNumber = Table_columnNameToNumber_e (me, factorColumnName);
		Table_quantileQuantilePlot_betweenLevels (me, GRAPHICS, dataColumnNumber, factorColumnNumber, xLevel_string, yLevelString,
				numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, label, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_lagPlot, U"Table: lag plot", nullptr) {
	SENTENCE (dataColumnName, U"Data column", U"errors")
	NATURAL (lag, U"Lag", U"1")
	REAL (fromXY, U"left Horizontal and vertical range", U"0.0")
	REAL (toXY, U"right Horizontal and vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	SENTENCE (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer dataColumnNumber = Table_columnNameToNumber_e (me, dataColumnName);
		Table_lagPlot (me, GRAPHICS, dataColumnNumber, lag, fromXY, toXY, label, labelSize, garnish);
	GRAPHICS_EACH_END
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_lagPlotWhere, U"Table: lag plot where", nullptr) {
	SENTENCE (dataColumnName, U"Data column", U"errors")
	NATURAL (lag, U"Lag", U"1")
	REAL (fromXY, U"left Horizontal and vertical range", U"0.0")
	REAL (toXY, U"right Horizontal and vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	SENTENCE (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true);
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer dataColumnNumber = Table_columnNameToNumber_e (me, dataColumnName);
		autoTable thee = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_lagPlot (me, GRAPHICS, dataColumnNumber, lag, fromXY, toXY, label, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_distributionPlot, U"Table: Distribution plot", nullptr) {
	SENTENCE (dataColumnName, U"Data column", U"data")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	COMMENT (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimumFrequency, U"Minimum frequency", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer dataColumnNumber = Table_columnNameToNumber_e (me, dataColumnName);
		Table_distributionPlot (me, GRAPHICS, dataColumnNumber, minimumValue, maximumValue, 
			numberOfBins, minimumFrequency, maximumFrequency, garnish
		);
	GRAPHICS_EACH_END
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_distributionPlotWhere, U"Table: Distribution plot where", nullptr) {
	SENTENCE (dataColumnName, U"Data column", U"data")
	REAL (minimumValue, U"Minimum value", U"0.0")
	REAL (maximumValue, U"Maximum value", U"0.0")
	COMMENT (U"Display of the distribution")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	REAL (minimumFrequency, U"Minimum frequency", U"0.0")
	REAL (maximumFrequency, U"Maximum frequency", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer dataColumnNumber = Table_columnNameToNumber_e (me, dataColumnName);
		autoTable thee = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_distributionPlot (thee.get(), GRAPHICS, dataColumnNumber, minimumValue, maximumValue, numberOfBins, 
			minimumFrequency, maximumFrequency, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_horizontalErrorBarsPlot, U"Table: Horizontal error bars plot", U"Table: Horizontal error bars plot...") {
	SENTENCE (xColumnName, U"Horizontal column", U"x")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	SENTENCE (yColumnName, U"Vertical column", U"y")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	SENTENCE (lowerErrorColumnName, U"Lower error value column", U"error1")
	SENTENCE (upperErrorColumnName, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer xlColumnNumber = Table_columnNameToNumber_0 (me, lowerErrorColumnName);
		const integer xuColumnNumber = Table_columnNameToNumber_0 (me, upperErrorColumnName);
		Table_horizontalErrorBarsPlot (me, GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax, xlColumnNumber, xuColumnNumber, barSize_mm, garnish);
	GRAPHICS_EACH_END
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_horizontalErrorBarsPlotWhere, U"Table: Horizontal error bars plot where", U"Table: Horizontal error bars plot where...") {
	SENTENCE (xColumnName, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	SENTENCE (lowerErrorColumnName, U"Lower error value column", U"error1")
	SENTENCE (upperErrorColumnName, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer xlColumnNumber = Table_columnNameToNumber_0 (me, lowerErrorColumnName);
		const integer xuColumnNumber = Table_columnNameToNumber_0 (me, upperErrorColumnName);
		autoTable thee = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_horizontalErrorBarsPlot (me, GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax, xlColumnNumber, xuColumnNumber, barSize_mm, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_verticalErrorBarsPlot, U"Table: Vertical error bars plot", U"Table: Vertical error bars plot...") {
	SENTENCE (xColumnName, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	SENTENCE (lowerErrorColumnName, U"Lower error value column", U"error1")
	SENTENCE (upperErrorColumnName, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer ylColumnNumber = Table_columnNameToNumber_0 (me, lowerErrorColumnName);
		const integer yuColumnNumber = Table_columnNameToNumber_0 (me, upperErrorColumnName);
		Table_verticalErrorBarsPlot (me, GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax, ylColumnNumber, yuColumnNumber, barSize_mm, garnish);
	GRAPHICS_EACH_END
}

// deprecated 2023
FORM (GRAPHICS_EACH__Table_verticalErrorBarsPlotWhere, U"Table: Vertical error bars plot where", U"Table: Vertical error bars plot where...") {
	SENTENCE (xColumnName, U"Horizontal column", U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	SENTENCE (yColumnName, U"Vertical column", U"")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	SENTENCE (lowerErrorColumnName, U"Lower error value column", U"error1")
	SENTENCE (upperErrorColumnName, U"Upper error value column", U"error2")
	REAL (barSize_mm, U"Bar size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true);
	COMMENT (U"Use data only in rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xColumnNumber = Table_columnNameToNumber_e (me, xColumnName);
		const integer yColumnNumber = Table_columnNameToNumber_e (me, yColumnName);
		const integer ylColumnNumber = Table_columnNameToNumber_0 (me, lowerErrorColumnName);
		const integer yuColumnNumber = Table_columnNameToNumber_0 (me, upperErrorColumnName);
		autoTable thee = Table_extractRowsWhere_e (me, condition, interpreter);
		Table_verticalErrorBarsPlot (thee.get(), GRAPHICS, xColumnNumber, yColumnNumber,
				xmin, xmax, ymin, ymax,	ylColumnNumber, yuColumnNumber, barSize_mm, garnish);
	GRAPHICS_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Table_extractRowsWhere, U"Table: Extract rows where", nullptr) {
	COMMENT (U"Extract rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		autoTable result = Table_extractRowsWhere (me, condition, interpreter);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_formula")
}

// deprecated 2023
FORM (CONVERT_EACH_TO_ONE__Table_extractRowsMahalanobisWhere, U"Table: Extract rows where (mahalanobis)", nullptr) {
	STRINGARRAY_LINES (2, dataColumnNames, U"Extract all rows where columns...", { U"F1", U"F2", U"F3" })
	CHOICE_ENUM (kMelder_number, which,
			U"...have a mahalanobis distance...", kMelder_number::GREATER_THAN)
	REAL (numberOfSigmas, U"...the number", U"2.0")
	SENTENCE (factorColumnName, U"Factor column", U"")
	COMMENT (U"Process only rows where the following condition holds.")
	FORMULA (condition, U"Condition", U"1; self$[\"gender\"]=\"male\"")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		autoINTVEC columnNumbers = Table_columnNamesToNumbers (me, dataColumnNames);
		const integer factorColumnNumber = Table_columnNameToNumber_0 (me, factorColumnName);
		autoTable thee = Table_extractRowsWhere_e (me, condition, interpreter);
		autoTable result = Table_extractMahalanobis (thee.get(), columnNumbers.get(), which, numberOfSigmas, factorColumnNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_mahalanobis")
}

FORM (CONVERT_EACH_TO_ONE__Table_extractRowsWhereMahalanobis, U"Table: Extract rows where (mahalanobis)", nullptr) {
	STRINGARRAY_LINES (2, dataColumnNames, U"Extract all rows where columns...", { U"F1", U"F2", U"F3" })
	CHOICE_ENUM (kMelder_number, which,
			U"...have a mahalanobis distance...", kMelder_number::GREATER_THAN)
	REAL (numberOfSigmas, U"...the number", U"2.0")
	SENTENCE (factorColumnName, U"Factor column", U"")
	OK
DO_ALTERNATIVE (CONVERT_EACH_TO_ONE__Table_extractRowsMahalanobisWhere)
	CONVERT_EACH_TO_ONE (Table)
		autoINTVEC columnNumbers = Table_columnNamesToNumbers (me, dataColumnNames);
		const integer factorColumnNumber = Table_columnNameToNumber_0 (me, factorColumnName);
		autoTable result = Table_extractMahalanobis (me, columnNumbers.get(), which, numberOfSigmas, factorColumnNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_mahalanobis")
}

FORM (CONVERT_EACH_TO_ONE__Table_extractColumnsByNumber, U"Table: Extract columns by number", nullptr) {
	COMMENT (U"Create a new Table from the columns with the following numbers.")
	NATURALVECTOR (columnNumbers, U"Column numbers", RANGES_, U"1 2")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		autoTable result = Table_extractColumnsByNumber (me, columnNumbers);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_columns")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__Table_listRowNumbersWhere, U"Table: List rows where", U"") {
	COMMENT (U"List rows where the following condition holds true.")
	FORMULA (condition, U"Condition", U"self [row, \"F1\"] > 800.0")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Table)
		autoVEC result = cast_VEC (Table_listRowNumbersWhere (me, condition, interpreter).get());
	QUERY_ONE_FOR_REAL_VECTOR_END
}

/******************* TableOfReal ****************************/

DIRECT (CREATE_ONE__CreateIrisDataset) {
	CREATE_ONE
		autoTableOfReal result = TableOfReal_createIrisDataset ();
	CREATE_ONE_END (U"iris")
}

FORM (INFO_ONE__TableOfReal_reportMultivariateNormality, U"TableOfReal: Report multivariate normality (BHEP)", U"TableOfReal: Report multivariate normality (BHEP)...") {
	REAL (smoothing, U"Smoothing parameter", U"0.0")
	OK
DO
	INFO_ONE (TableOfReal)
		bool singular;
		double tnb, lnmu, lnvar;
		const double prob = TableOfReal_normalityTest_BHEP (me, & smoothing, & tnb, & lnmu, & lnvar, & singular);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Baringhaus–Henze–Epps–Pulley normality test:");
		MelderInfo_writeLine (U"Significance of normality: ", prob);
		MelderInfo_writeLine (U"BHEP statistic: ", tnb);
		MelderInfo_writeLine (U"Lognormal mean: ", lnmu);
		MelderInfo_writeLine (U"Lognormal variance: ", lnvar);
		MelderInfo_writeLine (U"Smoothing: ", smoothing);
		MelderInfo_writeLine (U"Sample size: ", my numberOfRows);
		MelderInfo_writeLine (U"Number of variables: ", my numberOfColumns);
		if (singular)
			MelderInfo_writeLine (U"(Attention: the covariance matrix was singular!)");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__TableOfReal_reportSphericityOfCovariance, U"TableOfReal: Report covariance sphericity", nullptr) {
	NATURAL (numberOfPermutations, U"Number of permutations", U"100");
	BOOLEAN (useCorrelation, U"Use correlation instead", false);
	OK
DO
	INFO_ONE (TableOfReal)
		const double p = TableOfReal_testSphericityOfCovariance (me, numberOfPermutations, useCorrelation);
		MelderInfo_open ();
		MelderInfo_writeLine (p, U" (probability from ", numberOfPermutations, U" permutations).");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__TableOfReal_reportCompoundSymmetryOfCovariance, U"TableOfReal: Report covariance compound symmetry", nullptr) {
	NATURAL (numberOfPermutations, U"Number of permutations", U"100");
	BOOLEAN (useCorrelation, U"Use correlation instead", false);
	OK
DO
	INFO_ONE (TableOfReal)
		const double p = TableOfReal_testCovarianceCompoundSymmetry (me, numberOfPermutations, useCorrelation);
		MelderInfo_open ();
		MelderInfo_writeLine (p, U" (probability from ", numberOfPermutations, U" permutations).");
		MelderInfo_close ();
	INFO_ONE_END	
}

FORM (INFO_ONE__TableOfReal_reportCovarianceIdentity, U"", nullptr) {
	NATURAL (numberOfPermutations, U"Number of permutations", U"100");
	BOOLEAN (useCorrelation, U"Use correlation instead", false);
	OK
DO
	INFO_ONE (TableOfReal)
		const double p = TableOfReal_testCovarianceEqualsIdentityMatrix (me, numberOfPermutations, useCorrelation);
		MelderInfo_open ();
		MelderInfo_writeLine (p, U" (probability from ", numberOfPermutations, U" permutations).");
		MelderInfo_close ();
	INFO_ONE_END	
}

FORM (INFO_TWO__TableOfReal_reportEqualityOfCovariances, U"TableOfReal: Report equality of covariances", nullptr) {
	NATURAL (numberOfPermutations, U"Number of permutations", U"100");
	BOOLEAN (useCorrelation, U"Use correlation instead", false);
	OK
DO
	INFO_TWO (TableOfReal)
		const double p = TableOfReal_testEqualityOfCovariances (me, you, numberOfPermutations, useCorrelation);
		MelderInfo_open ();
		MelderInfo_writeLine (p, U" (probability from ", numberOfPermutations, U" permutations).");
		MelderInfo_close ();
	INFO_ONE_END	
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__TableOfReal_Permutation_permuteRows) {
	CONVERT_ONE_AND_ONE_TO_ONE (TableOfReal, Permutation)
		autoTableOfReal result = TableOfReal_Permutation_permuteRows (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__TableOfReal_Permutation_permuteColumns) {
	CONVERT_ONE_AND_ONE_TO_ONE (TableOfReal, Permutation)
		autoTableOfReal result = TableOfReal_Permutation_permuteColumns (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_Permutation_sortRowlabels) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoPermutation result = TableOfReal_to_Permutation_sortRowLabels (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (COMBINE_ALL_LISTED_TO_ONE__TableOfReal_appendColumns) {
	COMBINE_ALL_LISTED_TO_ONE (TableOfReal, TableOfRealList)
		autoTableOfReal result = TableOfRealList_appendColumnsMany (list.get());
	COMBINE_ALL_LISTED_TO_ONE_END (U"columns_appended");
}

FORM (CREATE_ONE__TableOfReal_create_pols1973, U"Create TableOfReal (Pols 1973)", U"Create TableOfReal (Pols 1973)...") {
	BOOLEAN (includeFormantAmplitudeLevels, U"Include formant levels", false)
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_pols1973 (includeFormantAmplitudeLevels);
	CREATE_ONE_END (U"pols1973")
}

DIRECT (CREATE_ONE__Table_create_keatingEsposito2006) {
	CREATE_ONE
		autoTable result = Table_create_keatingEsposito2006 ();
	CREATE_ONE_END (U"keatingEsposito2006")
}

DIRECT (CREATE_ONE__Table_create_ganong1980) {
	CREATE_ONE
		autoTable result = Table_create_ganong1980 ();
	CREATE_ONE_END (U"ganong")
}

FORM (CREATE_ONE__TableOfReal_create_vanNierop1973, U"Create TableOfReal (Van Nierop 1973)...", U"Create TableOfReal (Van Nierop 1973)...") {
	BOOLEAN (includeFormantAmplitudeLevels, U"Include formant levels", false)
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_vanNierop1973 (includeFormantAmplitudeLevels);
	CREATE_ONE_END (U"vanNierop1973")
}

FORM (CREATE_ONE__TableOfReal_create_weenink1983, U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Weenink 1985)...") {
	CHOICE (speakerGroup, U"Speakers group", 1)
		OPTION (U"men")
		OPTION (U"women")
		OPTION (U"children")
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_weenink1983 (speakerGroup);
	CREATE_ONE_END ((speakerGroup == 1 ? U"m10" : speakerGroup == 2 ? U"w10" : U"c10"));
}

DIRECT (CREATE_ONE__Table_create_sandwell1987) {
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create_sandwell1987 ();
	CREATE_ONE_END (U"Sandwell1987");
}

FORM (GRAPHICS_EACH__TableOfReal_drawAsScalableSquares, U"TableOfReal: Draw as scalable squares", U"TableOfReal: Draw as scalable squares...") {
	NATURAL (rowmin, U"From row", U"1");
	INTEGER (rowmax, U"To row", U"0 (= all)");
	NATURAL (colmin, U"From column", U"1");
	INTEGER (colmax, U"To column", U"0 (= all)");
	OPTIONMENU_ENUM (kGraphicsMatrixOrigin, origin,
			U"Origin", kGraphicsMatrixOrigin::DEFAULT)
	POSITIVE (scaleFactor, U"Cell area scale factor", U"0.95")
	OPTIONMENU_ENUM (kGraphicsMatrixCellDrawingOrder, drawingOrder,
			U"Filling order", kGraphicsMatrixCellDrawingOrder::DEFAULT)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawAsScalableSquares (me, GRAPHICS, rowmin, rowmax, colmin, colmax,
			origin, scaleFactor, drawingOrder, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawScatterPlot, U"TableOfReal: Draw scatter plot", U"TableOfReal: Draw scatter plot...") {
	COMMENT (U"Select the part of the table")
	NATURAL (xColumn, U"Horizontal axis column number", U"1")
	NATURAL (yColumn, U"Vertical axis column number", U"2")
	INTEGER (fromRow, U"left Row number range", U"0")
	INTEGER (toRow, U"right Row number range", U"0")
	COMMENT (U"Select the drawing area limits")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	NATURAL (labelSize, U"Label size", U"12")
	BOOLEAN (useRowLabels, U"Use row labels", false)
	WORD (label, U"Label", U"+")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawScatterPlot (me, GRAPHICS, xColumn, yColumn, fromRow, toRow, xmin, xmax, 
			ymin, ymax, labelSize, useRowLabels, label, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawScatterPlotMatrix, U"TableOfReal: Draw scatter plots matrix", nullptr) {
	INTEGER (fromColumn, U"From column", U"0")
	INTEGER (toColumn, U"To column", U"0")
	POSITIVE (fractionWhite, U"Fraction white", U"0.1")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawScatterPlotMatrix (me, GRAPHICS, fromColumn, toColumn, fractionWhite);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawBiplot, U"TableOfReal: Draw biplot", U"TableOfReal: Draw biplot...") {
	COMMENT (U"")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	REAL (splitFactor, U"Split factor", U"0.5")
	INTEGER (labelSize, U"Label size", U"10")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawBiplot (me, GRAPHICS, xmin, xmax, ymin, ymax, splitFactor, labelSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawVectors, U"Draw vectors", U"TableOfReal: Draw vectors...") {
	COMMENT (U"From (x1, y1) to (x2, y2)")
	NATURAL (x1Column, U"X1 column", U"1")
	NATURAL (y1Column, U"Y1 column", U"2")
	NATURAL (x2Column, U"X2 column", U"3")
	NATURAL (y2Column, U"Y2 column", U"4")
	COMMENT (U"Select the drawing area")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	CHOICE (vectorType, U"Vector type", 1)
		OPTION (U"arrow")
		OPTION (U"double arrow")
		OPTION (U"line")
	INTEGER (labelSize, U"Label size", U"10")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawVectors (me, GRAPHICS, x1Column, y1Column, x2Column, y2Column, xmin, xmax, 
			ymin, ymax, vectorType, labelSize, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawRowAsHistogram, U"Draw row as histogram", U"TableOfReal: Draw rows as histogram...") {
	COMMENT (U"Select from the table")
	NATURAL (rowNumber, U"Row number", U"1")
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0")
	COMMENT (U"Vertical drawing range")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	COMMENT (U"Offset and distance in units of 'bar width'")
	REAL (xOffset, U"Horizontal offset", U"0.5")
	REAL (distanceBetweenBars, U"Distance between bars", U"1.0")
	REAL (greyValue, U"Grey value (1.0=white)", U"0.7")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, autoINTVEC ({ rowNumber }).get(), fromColumn, toColumn, ymin, ymax,
			xOffset, 0, distanceBetweenBars, autoVEC ({ greyValue }).get(), garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawRowsAsHistogram, U"Draw rows as histogram", U"TableOfReal: Draw rows as histogram...") {
	COMMENT (U"Select from the table")
	NATURALVECTOR (rowNumbers, U"Row numbers", WHITESPACE_SEPARATED_, U"1 2")
	INTEGER (fromColumn, U"left Column range", U"0 (= all)")
	INTEGER (toColumn, U"right Column range", U"0 (= all)")
	COMMENT (U"Vertical drawing range")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	COMMENT (U"Offset and distance in units of 'bar width'")
	REAL (xOffset, U"Horizontal offset", U"1.0")
	REAL (distanceBetweenBarGroups, U"Distance between bar groups", U"1.0")
	REAL (distanceBetweenBars, U"Distance between bars", U"0.0")
	REALVECTOR (greys, U"Grey values (1.0=white)", WHITESPACE_SEPARATED_, U"1.0 1.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawRowsAsHistogram (me, GRAPHICS, rowNumbers, fromColumn, toColumn, ymin, ymax,
			xOffset, distanceBetweenBars, distanceBetweenBarGroups, greys, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawBoxPlots, U"TableOfReal: Draw box plots", U"TableOfReal: Draw box plots...") {
	INTEGER (fromRow, U"From row", U"0")
	INTEGER (toRow, U"To row", U"0")
	INTEGER (fromColumn, U"From column", U"0")
	INTEGER (toColumn, U"To column", U"0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawBoxPlots (me, GRAPHICS, fromRow, toRow, fromColumn, toColumn, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__TableOfReal_drawColumnAsDistribution, U"TableOfReal: Draw column as distribution", U"TableOfReal: Draw column as distribution...") {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (fromValue, U"left Value range", U"0.0")
	REAL (toValue, U"right Value range", U"0.0")
	REAL (fromFrequency, U"left Frequency range", U"0.0")
	REAL (toFrequency, U"right frequency range", U"0.0")
	NATURAL (numberOfBins, U"Number of bins", U"10")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawColumnAsDistribution (me, GRAPHICS, columnNumber, fromValue, toValue, numberOfBins, 
			fromFrequency, toFrequency, false, garnish
		);
	GRAPHICS_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_Configuration_lda, U"TableOfReal: To Configuration (lda)", U"TableOfReal: To Configuration (lda)...") {
	INTEGER (numberOfDimensions, U"Number of dimensions", U"0 (= all)")
	OK
DO
	Melder_require (numberOfDimensions >= 0, 
		U"The number of dimensions should be at least zero.");
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoConfiguration result = TableOfReal_to_Configuration_lda (me, numberOfDimensions);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_lda")
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_CCA, U"TableOfReal: To CCA", U"TableOfReal: To CCA...") {
	NATURAL (dimensionOfDependentVariate, U"Dimension of dependent variate", U"2")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoCCA result = TableOfReal_to_CCA (me, dimensionOfDependentVariate);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_Configuration_pca, U"TableOfReal: To Configuration (pca)", U"TableOfReal: To Configuration (pca)...") {
	NATURAL (numberOfDimensions, U"Number of dimensions", U"2")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoConfiguration result = TableOfReal_to_Configuration_pca (me, numberOfDimensions);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_pca")
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_Discriminant) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoDiscriminant result = TableOfReal_to_Discriminant (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_PCA_byRows) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoPCA result = TableOfReal_to_PCA_byRows (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_SSCP, U"TableOfReal: To SSCP", U"TableOfReal: To SSCP...") {
	INTEGER (fromRow, U"Begin row", U"0")
	INTEGER (toRow, U"End row", U"0")
	INTEGER (fromColumn, U"Begin column", U"0")
	INTEGER (toColumn, U"End column", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoSSCP result = TableOfReal_to_SSCP (me, fromRow, toRow, fromColumn, toColumn);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_SSCP_rowWeights, U"TableOfReal: To SSCP (row weights)", U"TableOfReal: To SSCP (row weights)...") {
	INTEGER (fromRow, U"Begin row", U"0")
	INTEGER (toRow, U"End row", U"0")
	INTEGER (fromColumn, U"Begin column", U"0")
	INTEGER (toColumn, U"End column", U"0")
	INTEGER (rowWeights, U"Weights column number", U"0")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoSSCP result = TableOfReal_to_SSCP_rowWeights (me, fromRow, toRow, fromColumn, toColumn, rowWeights);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/* For the inheritors */
DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_TableOfReal) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_to_TableOfReal (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_Correlation) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoCorrelation result = TableOfReal_to_Correlation (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_Correlation_rank) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoCorrelation result = TableOfReal_to_Correlation_rank (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_Covariance) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoCovariance result = TableOfReal_to_Covariance (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__TableOfReal_to_SVD) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoSVD result = TableOfReal_to_SVD (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_TWO_TO_ONE__TablesOfReal_to_Eigen_gsvd) {
	CONVERT_TWO_TO_ONE (TableOfReal)
		autoEigen result = TablesOfReal_to_Eigen_gsvd (me, you);
	CONVERT_TWO_TO_ONE_END (U"gsvd");
}

FORM (CONVERT_TWO_TO_ONE__TableOfReal_TableOfReal_crossCorrelations, U"TableOfReal & TableOfReal: Cross-correlations", nullptr) {
	OPTIONMENU (between, U"Correlations between", 1)
		OPTION (U"rows")
		OPTION (U"columns")
	BOOLEAN (center, U"Center", false)
	BOOLEAN (normalize, U"Normalize", false)
	OK
DO
	const bool by_columns = ( between == 2 );
	CONVERT_TWO_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_TableOfReal_crossCorrelations (me, you, by_columns, center, normalize);
	CONVERT_TWO_TO_ONE_END (U"cc_", by_columns ? U"by_cols" : U"by_rows")
}

void praat_TableOfReal_init3 (ClassInfo klas);
void praat_TableOfReal_init3 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 2, U"To TableOfReal (cross-correlations)...", 0, 0, 
			CONVERT_TWO_TO_ONE__TableOfReal_TableOfReal_crossCorrelations);
}

DIRECT (CONVERT_TWO_TO_ONE__TablesOfReal_to_GSVD) {
	CONVERT_TWO_TO_ONE (TableOfReal)
		autoGSVD result = TablesOfReal_to_GSVD (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_choleskyDecomposition, U"TableOfReal: Cholesky decomposition", nullptr) {
	BOOLEAN (wantUpper, U"Upper (else L)", false)
	BOOLEAN (wantInverse, U"Inverse", false)
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_choleskyDecomposition (me, wantUpper, wantInverse);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_PatternList_and_Categories, U"TableOfReal: To PatternList and Categories", U"TableOfReal: To PatternList and Categories...") {
	INTEGER (fromRow, U"left Row range", U"0")
	INTEGER (toRow, U"right Row range", U"0 (= all)")
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0 (= all)")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoPatternList ap;
		autoCategories result;
		TableOfReal_to_PatternList_and_Categories (me, fromRow, toRow, fromColumn, toColumn, & ap, & result);
		praat_new (ap.move(), my name.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (QUERY_ONE_FOR_REAL__TableOfReal_getColumnSum, U"TableOfReal: Get column sum", U"") {
	INTEGER (columnNumber, U"Column", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const double result = TableOfReal_getColumnSum (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U"(column sum)")
}

FORM (QUERY_ONE_FOR_REAL__TableOfReal_getRowSum, U"TableOfReal: Get row sum", U"") {
	INTEGER (rowNumber, U"Row", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const double result = TableOfReal_getRowSum (me, rowNumber);
	QUERY_ONE_FOR_REAL_END (U"(row sum)")
}

DIRECT (QUERY_ONE_FOR_REAL__TableOfReal_getGrandSum) {
	QUERY_ONE_FOR_REAL (TableOfReal)
		const double result = TableOfReal_getGrandSum (me);
	QUERY_ONE_FOR_REAL_END (U"(grand sum)")
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_meansByRowLabels, U"TableOfReal: Means by row labels", U"TableOfReal: To TableOfReal (means by row labels)...") {
	BOOLEAN (expand, U"Expand", false)
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_meansByRowLabels (me, expand, 0);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_byrowlabels")
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_mediansByRowLabels, U"TableOfReal: Medians by row labels", U"TableOfReal: To TableOfReal (medians by row labels)...") {
	BOOLEAN (expand, U"Expand", false)
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_meansByRowLabels (me, expand, 1);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_byrowlabels")
}

FORM (CONVERT_EACH_TO_ONE__TableOfReal_to_TableOfReal_rankColumns, U"TableOfReal: Rank columns", U"TableOfReal: To TableOfReal (rank columns)...") {
	INTEGER (fromColumn, U"left Column range", U"0")
	INTEGER (toColumn, U"right Column range", U"0 (=all)")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_rankColumns (me, fromColumn, toColumn);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_byrowlabels")
}

/***** TableOfReal and FilterBank  *****/

FORM (QUERY_ONE_FOR_REAL__TextGrid_getTotalDurationOfIntervalsWhere, U"Total duration of intervals where", nullptr) {
	INTEGER (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMelder_string, countIntervalsWhoseLabel___,
			U"Intervals whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	QUERY_ONE_FOR_REAL (TextGrid)
		const double result = TextGrid_getTotalDurationOfIntervalsWhere (me, tierNumber, countIntervalsWhoseLabel___, ___theText);
	QUERY_ONE_FOR_REAL_END (U" s (duration of intervals containing \"", ___theText, U"\")");
}

FORM (MODIFY_TextGrid_extendTime, U"TextGrid: Extend time", U"TextGrid: Extend time...") {
	POSITIVE (extendDomainBy, U"Extend domain by (s)", U"1.0")
	CHOICE (position, U"At", 1)
		OPTION (U"end")
		OPTION (U"start")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_extendTime (me, extendDomainBy, position - 1);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_replaceIntervalTexts, U"TextGrid: Replace interval texts", U"TextGrid: Replace interval texts...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	INTEGER (fromInterval, U"left Interval range", U"1")
	INTEGER (toInterval, U"right Interval range", U"0 (= all)")
	SENTENCE (search_string, U"Search", U"a")
	SENTENCE (replace_string, U"Replace", U"b")
	CHOICE (searchType, U"Search and replace strings are", 1)
		OPTION (U"literals")
		OPTION (U"Regular Expressions")
	OK
DO
	MODIFY_EACH (TextGrid)
		integer nmatches, nstringmatches;
		TextGrid_changeLabels (me, tierNumber, fromInterval, toInterval, search_string, replace_string, 
			searchType - 1, &nmatches, &nstringmatches
		);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_replacePointTexts, U"TextGrid: Replace point texts", U"TextGrid: Replace point texts...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	INTEGER (fromInterval, U"left Interval range", U"1")
	INTEGER (toInterval, U"right Interval range", U"0 (= all)")
	SENTENCE (search_string, U"Search", U"a")
	SENTENCE (replace_string, U"Replace", U"b")
	CHOICE (searchType, U"Search and replace strings are", 1)
		OPTION (U"literals")
		OPTION (U"Regular Expressions")
	OK
DO
	MODIFY_EACH (TextGrid)
		integer nmatches, nstringmatches;
		TextGrid_changeLabels (me, tierNumber, fromInterval, toInterval, search_string, replace_string,
			searchType - 1, &nmatches, &nstringmatches
		);
	MODIFY_EACH_END
}

FORM (CONVERT_TWO_TO_ONE__TextGrids_to_Table_textAlignment, U"TextGrids: To Table (text alignment)", nullptr) {
	NATURAL (targetTierNumber, U"Target tier", U"1")
	NATURAL (sourceTierNumber, U"Source tier", U"1")
	OK
DO
	CONVERT_TWO_TO_ONE (TextGrid)
 		autoTable result = TextGrids_to_Table_textAlignment (me, targetTierNumber, you, sourceTierNumber, nullptr);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get());
}

FORM (CONVERT_EACH_TO_ONE__TextGrid_to_DurationTier, U"TextGrid: To DurationTier", U"TextGrid: To DurationTier...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	POSITIVE (timeScaleFactor, U"Time scale factor", U"2.0")
	POSITIVE (leftTransitionDuration, U"Left transition duration (s)", U"1e-10")
	POSITIVE (rightTransitionDuration, U"Right transition duration (s)", U"1e-10")
	OPTIONMENU_ENUM (kMelder_string, scaleIntervalsWhoseLabel___,
			U"Scale intervals whose label... ", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH_TO_ONE (TextGrid)
		autoDurationTier result = TextGrid_to_DurationTier (me,tierNumber, timeScaleFactor,
			leftTransitionDuration, rightTransitionDuration, scaleIntervalsWhoseLabel___, ___theText
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__TextGrid_to_TextGridNavigator_topicSearch, U"TextGrid: To TextGridNavigator (topic only)", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	STRINGARRAY_LINES (4, topicLabels, U"Topic labels", { U"i", U"u", U"e", U"o", U"\\as" })
	OPTIONMENU_ENUM (kMelder_string, topicCriterion, U"Topic match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, topicMatchBoolean, U"Combine topic matches with", kMatchBoolean::OR_)
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (TextGrid)
		autoTextGridNavigator result = TextGrid_to_TextGridNavigator_topicSearch (me, tierNumber, 
			topicLabels, topicCriterion, topicMatchBoolean, matchDomain
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__TextGrid_to_TextGridNavigator, U"TextGrid: To TextGridNavigator", U"TextGrid: To TextGridNavigator...") {
	NATURAL (tierNumber, U"Tier number", U"1")
	STRINGARRAY_LINES (4, topicLabels, U"Topic labels", { U"i", U"u", U"e", U"o", U"\\as" })
	OPTIONMENU_ENUM (kMelder_string, topicCriterion, U"Topic match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, topicMatchBoolean, U"Combine topic matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	STRINGARRAY_LINES (4, beforeLabels, U"Before labels", { U"p", U"b", U"t", U"d", U"k", U"g" })
	OPTIONMENU_ENUM (kMelder_string, beforeCriterion, U"Before match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, beforeMatchBoolean, U"Combine before matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	STRINGARRAY_LINES (4, afterLabels, U"After labels", { U"m", U"n" })
	OPTIONMENU_ENUM (kMelder_string, afterCriterion, U"After match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, afterMatchBoolean, U"Combine after matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	OPTIONMENU_ENUM (kContext_combination, combinationCriterion, U"Combination criterion", kContext_combination::BEFORE_AND_AFTER)
	BOOLEAN (excludeTopic, U"Exclude topic labels", false)
	COMMENT (U"")
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (TextGrid)
		autoTextGridNavigator result = TextGrid_to_TextGridNavigator (me, tierNumber, 
			topicLabels, topicCriterion, topicMatchBoolean,
			beforeLabels, beforeCriterion, beforeMatchBoolean,
			afterLabels, afterCriterion, afterMatchBoolean,
			combinationCriterion, excludeTopic, matchDomain
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__TextGrid_DurationTier_to_TextGrid) {
	CONVERT_ONE_AND_ONE_TO_ONE (TextGrid, DurationTier)
		autoTextGrid result = TextGrid_DurationTier_scaleTimes (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_TWO_AND_ONE_TO_ONE__TextGrids_EditCostsTable_to_Table_textAlignment, U"TextGrids & EditCostsTable: To Table (text alignment)", nullptr) {
	NATURAL (targetTierNumber, U"Target tier", U"1")
	NATURAL (sourceTierNumber, U"Source tier", U"1")
	OK
DO
	CONVERT_TWO_AND_ONE_TO_ONE (TextGrid, EditCostsTable)
		autoTable result = TextGrids_to_Table_textAlignment (me, targetTierNumber, you, sourceTierNumber, him);
	CONVERT_TWO_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}


DIRECT (HELP__TextGridNavigator_help) {
	HELP (U"TextGridNavigator")
}

FORM (CREATE_ONE__TextGridNavigator_createSimple, U"TextGridNavigator: Create simple", U"") {
	WORD (name, U"Name", U"navigator")
	COMMENT (U"Domain")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"5.0")
	NATURAL (tierNumber, U"Tier number", U"1")
	STRINGARRAY_LINES (4, topicLabels, U"Topic labels", { U"i", U"u", U"e", U"o", U"\\as" })
	OK
DO
	CREATE_ONE
		autoTextGridNavigator result = TextGridNavigator_createSimple (xmin, xmax, tierNumber, topicLabels);
	CREATE_ONE_END (name)
}

FORM (MODIFY_EACH__TextGridNavigator_findNextAfterTime, U"TextGridNavigator: Find next after time", nullptr) {
	REAL (time, U"Time (s)", U"-1.0")
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		(void) TextGridNavigator_findNextAfterTime (me, time);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_findPreviousBeforeTime, U"TextGridNavigator: Find previous before time", nullptr) {
	REAL (time, U"Time (s)", U"10.0")
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		(void) TextGridNavigator_findPreviousBeforeTime (me, time);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__TextGridNavigator_findFirst) {
	MODIFY_EACH (TextGridNavigator)
		(void) TextGridNavigator_findFirst (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__TextGridNavigator_findLast) {
	MODIFY_EACH (TextGridNavigator)
		(void) TextGridNavigator_findLast (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__TextGridNavigator_findNext) {
	MODIFY_EACH (TextGridNavigator)
		(void) TextGridNavigator_findNext (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__TextGridNavigator_findPrevious) {
	MODIFY_EACH (TextGridNavigator)
		(void) TextGridNavigator_findPrevious (me);
	MODIFY_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__TextGridNavigator_getStartTime, U"TextGridNavigator: Get start time", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (TextGridNavigator)
		const double result = TextGridNavigator_getStartTime (me, tierNumber, where);
	QUERY_ONE_FOR_REAL_END (U" s (start time)")
}

FORM (QUERY_ONE_FOR_REAL__TextGridNavigator_getEndTime, U"TextGridNavigator: Get end time", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (TextGridNavigator)
		const double result = TextGridNavigator_getEndTime (me, tierNumber, where);
	QUERY_ONE_FOR_REAL_END (U" s (end time)")
}

FORM (QUERY_ONE_FOR_INTEGER__TextGridNavigator_getIndex, U"TextGridNavigator: Get index", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_INTEGER (TextGridNavigator)
		const integer result = TextGridNavigator_getIndex (me, tierNumber, where);
	QUERY_ONE_FOR_INTEGER_END (U"")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__TextGridNavigator_listIndices, U"TextGridNavigator: List indices", nullptr) {
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (TextGridNavigator)
		autoINTVEC resulti = TextGridNavigator_listIndices (me, where);
		autoVEC result = raw_VEC (resulti.size);
		for (integer i = 1; i <= result.size; i++)
			result [i] = resulti [i];
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__TextGridNavigator_listStartTimes, U"TextGridNavigator: List start times", nullptr) {
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (TextGridNavigator)
		autoVEC result = TextGridNavigator_listStartTimes (me, where);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__TextGridNavigator_listEndTimes, U"TextGridNavigator: List end times", nullptr) {
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (TextGridNavigator)
		autoVEC result = TextGridNavigator_listEndTimes (me, where);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_STRING_ARRAY__TextGridNavigator_listLabels, U"TextGridNavigator: List labels", nullptr) {
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_STRING_ARRAY (TextGridNavigator)
		autoSTRVEC result = TextGridNavigator_listLabels (me, where);
	QUERY_ONE_FOR_STRING_ARRAY_END
}

FORM (QUERY_ONE_FOR_MATRIX__TextGridNavigator_listDomains, U"", nullptr) {
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_MATRIX (TextGridNavigator)
		autoMAT result = TextGridNavigator_listDomains (me, matchDomain);
	QUERY_ONE_FOR_MATRIX_END
}

FORM (QUERY_ONE_FOR_STRING__TextGridNavigator_getLabel, U"TextGridNavigator: Get label", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kContext_where, where, U"Where", kContext_where::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_STRING (TextGridNavigator)
		conststring32 result = TextGridNavigator_getLabel (me, tierNumber, where);
	QUERY_ONE_FOR_STRING_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfMatches) {
	QUERY_ONE_FOR_INTEGER (TextGridNavigator)
		const integer result = TextGridNavigator_getNumberOfMatches (me);
	QUERY_ONE_FOR_INTEGER_END (U" (number of matches)")	
}

FORM (QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfTopicMatches, U"TextGridNavigator: Get number of Topic only matches", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (TextGridNavigator)
		const integer result = TextGridNavigator_getNumberOfTopicMatches (me, tierNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of Topic only matches in tier number ", tierNumber, U")")	
}

FORM (QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfBeforeMatches, U"TextGridNavigator: Get number of Before only matches", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (TextGridNavigator)
		const integer result = TextGridNavigator_getNumberOfBeforeMatches (me, tierNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of Before only matches in tier number ", tierNumber, U")")	
}

FORM (QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfAfterMatches, U"TextGridNavigator: Get number of After only matches", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (TextGridNavigator)
		const integer result = TextGridNavigator_getNumberOfAfterMatches (me, tierNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (number of After only matches in tier number ", tierNumber, U")")	
}

FORM (MODIFY_EACH__TextGridNavigator_modifyCombinationCriterion, U"TextGridNavigator: Modify combination criterion", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kContext_combination, combinationCriterion, U"Use criterion", kContext_combination::DEFAULT)
	BOOLEAN (excludeTopic, U"Exclude topic", false)
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyCombinationCriterion (me, tierNumber, combinationCriterion, excludeTopic);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyTopicCriterion, U"TextGridNavigator: Modify Topic match criterion", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMelder_string, topicCriterion, U"Topic match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, topicMatchBoolean, U"Combine topic matches with", kMatchBoolean::OR_)
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyTopicCriterion (me, tierNumber, topicCriterion, topicMatchBoolean);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyBeforeCriterion, U"TextGridNavigator: Modify Before match criterion", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMelder_string, beforeCriterion, U"Before match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, beforeMatchBoolean, U"Combine before matches with", kMatchBoolean::OR_)
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyBeforeCriterion (me, tierNumber, beforeCriterion, beforeMatchBoolean);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyAfterCriterion, U"TextGridNavigator: Modify After match criterion", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMelder_string, afterCriterion, U"After match criterion", kMelder_string::DEFAULT)
	OPTIONMENU_ENUM (kMatchBoolean, afterMatchBoolean, U"Combine after matches with", kMatchBoolean::OR_)
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyAfterCriterion (me, tierNumber, afterCriterion, afterMatchBoolean);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyMatchDomainAlignment, U"TextGridNavigator: Modify match domain alignment", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMatchDomainAlignment, matchDomainAlignment, U"Match domain alignment", kMatchDomainAlignment::DEFAULT)
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyMatchDomainAlignment (me, tierNumber, matchDomainAlignment);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyMatchDomain, U"TextGridNavigator: Modify match domain", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyMatchDomain (me, tierNumber, matchDomain);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyBeforeRange, U"TextGridNavigator: Modify Before range", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	NATURAL (from, U"From", U"1")
	NATURAL (to, U"To", U"1")
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyBeforeRange (me, tierNumber, from, to);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__TextGridNavigator_modifyAfterRange, U"TextGridNavigator: Modify After range", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	NATURAL (from, U"From", U"1")
	NATURAL (to, U"To", U"1")
	OK
DO
	MODIFY_EACH (TextGridNavigator)
		TextGridNavigator_modifyAfterRange (me, tierNumber, from, to);
	MODIFY_EACH_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_replaceNavigationContext, U"TextGridNavigator: Replace NavigationContext", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (TextGridNavigator, NavigationContext)
		TextGridNavigator_replaceNavigationContext (me, you, tierNumber);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_addTextGridTierNavigator, U"TextGridNavigator: Add TextGridTierNavigator", nullptr) {
	OPTIONMENU_ENUM (kMatchDomainAlignment, matchDomainAlignment, U"Match domain alignment", kMatchDomainAlignment::DEFAULT)
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (TextGridNavigator, TextGridTierNavigator)
		TextGridNavigator_addTextGridTierNavigator (me, you, matchDomainAlignment);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_replaceSearchTiers) {
	MODIFY_FIRST_OF_ONE_AND_ONE (TextGridNavigator, TextGrid)
		TextGridNavigator_replaceTiers (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_addSearchTier_topicOnly, 
	U"TextGridNavigator & TextGrid: Add search tier (topic only)", U"TextGridNavigator & TextGrid: Add search tier...")
{
	NATURAL (tierNumber, U"Tier number", U"1")
	STRINGARRAY_LINES (4, topicLabels, U"Topic labels", { U"i", U"u", U"e", U"o", U"\\as" })
	OPTIONMENU_ENUM (kMelder_string, topicCriterion, U"Topic match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, topicMatchBoolean, U"Combine topic matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OPTIONMENU_ENUM (kMatchDomainAlignment, matchDomainAlignment, U"Match domain alignment", kMatchDomainAlignment::DEFAULT)
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (TextGridNavigator, TextGrid)
		TextGridNavigator_and_TextGrid_addSearchTier_topicOnly (
			me, you, tierNumber, topicLabels, topicCriterion, topicMatchBoolean, matchDomain, matchDomainAlignment
		);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_addSearchTier, U"TextGridNavigator & TextGrid: Add search tier",
	U"TextGridNavigator & TextGrid: Add search tier...")
{
	NATURAL (tierNumber, U"Tier number", U"1")
	STRINGARRAY_LINES (4, topicLabels, U"Topic labels", { U"i", U"u", U"e", U"o", U"\\as" })
	OPTIONMENU_ENUM (kMelder_string, topicCriterion, U"Topic match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, topicMatchBoolean, U"Combine topic matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	STRINGARRAY_LINES (4, beforeLabels, U"Before labels", { U"p", U"b", U"t", U"d", U"k", U"g" })
	OPTIONMENU_ENUM (kMelder_string, beforeCriterion, U"Before match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, beforeMatchBoolean, U"Combine before matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	STRINGARRAY_LINES (4, afterLabels, U"After labels", { U"m", U"n" })
	OPTIONMENU_ENUM (kMelder_string, afterCriterion, U"After match criterion", kMelder_string::EQUAL_TO)
	OPTIONMENU_ENUM (kMatchBoolean, afterMatchBoolean, U"Combine after matches with", kMatchBoolean::OR_)
	COMMENT (U"")
	OPTIONMENU_ENUM (kContext_combination, combinationCriterion, U"Combination criterion", kContext_combination::BEFORE_AND_AFTER)
	BOOLEAN (excludeTopic, U"Exclude topic labels", false)
	COMMENT (U"")
	OPTIONMENU_ENUM (kMatchDomain, matchDomain, U"Match domain", kMatchDomain::DEFAULT)
	OPTIONMENU_ENUM (kMatchDomainAlignment, matchDomainAlignment, U"Match domain alignment", kMatchDomainAlignment::DEFAULT)
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (TextGridNavigator, TextGrid)
		TextGridNavigator_and_TextGrid_addSearchTier (me, you, tierNumber, 
			topicLabels, topicCriterion, topicMatchBoolean,
			beforeLabels, beforeCriterion, beforeMatchBoolean,
			afterLabels, afterCriterion, afterMatchBoolean,
			combinationCriterion, excludeTopic, matchDomain, matchDomainAlignment
		);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (CONVERT_EACH_TO_ONE__TextGridTierNavigator_to_TextGridNavigator) {
	CONVERT_EACH_TO_ONE (TextGridTierNavigator)
		autoTextGridNavigator result = TextGridTierNavigator_to_TextGridNavigator (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_EACH__TextGrid_setTierName, U"TextGrid: Set tier name", U"TextGrid: Set tier name...") {
	NATURAL (tierNUmber, U"Tier number", U"1")
	SENTENCE (name, U"Name", U"");
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_setTierName (me, tierNUmber, name);
	MODIFY_EACH_END
}

static void cb_publish (Editor /*editor*/, autoDaata publish) {
	try {
		praat_new (publish.move(), U"");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}

DIRECT (CREATION_WINDOW__VowelEditor_create) {
	CREATION_WINDOW (a, VowelEditor)
		autoVowelEditor creationWindow = VowelEditor_create (U"VowelEditor");
		Editor_setPublicationCallback (creationWindow.get(), cb_publish);
	CREATION_WINDOW_END
}

static autoDaata cmuAudioFileRecognizer (integer nread, const char *header, MelderFile file) {
	return nread < 12 || header [0] != 6 || header [1] != 0 ?
	       autoSound () : Sound_readFromCmuAudioFile (file);
}

static autoDaata oggFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread < 27 ) // each page header is 27 bytes
		return autoDaata ();
	if (! strnequ (header, "OggS", 4)) // Capture pattern 32 bits
		return autoDaata ();
	unsigned char version = header [4];
	if (version != 0) // Currently mandated to be zero
		return autoDaata ();
	unsigned int headerType = header [5];
	if (headerType != 0x02) // Beginning of stream
		return autoDaata ();
	if (header [6] == '\0' && header [7] == '\0' && header [8] == '\0' && header [9] == '\0' &&
		header [10] == '\0' && header [11] == '\0' && header [12] == '\0' && header [13] == '\0' &&
		strnequ (& header [28], "OpusHead", 8))
	{
		return Sound_readFromOggOpusFile (file);
	}
	/*
		Leave rest of checking to libVorbis
	*/
	return Sound_readFromOggVorbisFile (file);
}

void praat_CC_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Paint...", nullptr, 1,
			GRAPHICS_EACH__CC_paint);
	praat_addAction1 (klas, 1, U"Draw...", nullptr, 1, 
			GRAPHICS_EACH__CC_drawC0);
	praat_addAction1 (klas, 1, U"Query -", nullptr, 0, nullptr);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get number of coefficients...", nullptr, 1, 
			QUERY_ONE_FOR_INTEGER__CC_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, U"Get value in frame...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__CC_getValueInFrame);
	praat_addAction1 (klas, 1, U"Get c0 value in frame...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__CC_getC0ValueInFrame);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__CC_getValue);
	praat_addAction1 (klas, 0, U"To Matrix", nullptr, 0, 
			CONVERT_EACH_TO_ONE__CC_to_Matrix);
	praat_addAction1 (klas, 2, U"To DTW...", nullptr, 0, 
			CONVERT_TWO_TO_ONE__CCs_to_DTW);
}

static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm); // deprecated 2014
static void praat_Eigen_Matrix_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, GuiMenu_HIDDEN,
			CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, GuiMenu_HIDDEN,
			CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Matrix_projectColumns);
}

static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm);
static void praat_Eigen_Spectrogram_project (ClassInfo klase, ClassInfo klasm) {
	praat_addAction2 (klase, 1, klasm, 1, U"Project...", nullptr, GuiMenu_HIDDEN,
			CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Matrix_projectColumns);
	praat_addAction2 (klase, 1, klasm, 1, U"To Matrix (project columns)...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Matrix_projectColumns);
}

static void praat_Eigen_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Get eigenvalue...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__Eigen_getEigenvalue);
	praat_addAction1 (klas, 1, U"Get sum of eigenvalues...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__Eigen_getSumOfEigenvalues);
	praat_addAction1 (klas, 1, U"Get number of eigenvectors", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__Eigen_getNumberOfEigenvectors);
	praat_addAction1 (klas, 1, U"Get eigenvector dimension", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__Eigen_getEigenvectorDimension);
	praat_addAction1 (klas, 1, U"Get eigenvector element...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__Eigen_getEigenvectorElement);
}

static void praat_Eigen_draw_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Draw eigenvalues...", nullptr, 1,
			GRAPHICS_EACH__Eigen_drawEigenvalues);
	praat_addAction1 (klas, 0, U"Draw eigenvalues (scree)...", nullptr, GuiMenu_DEPRECATED_2010 | GuiMenu_DEPTH_1,
			WARNING__Eigen_drawEigenvalues_scree);
	praat_addAction1 (klas, 0, U"Draw eigenvector...", nullptr, 1,
			GRAPHICS_EACH__Eigen_drawEigenvector);
}

static void praat_FilterBank_query_init (ClassInfo klas);
static void praat_FilterBank_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Query -", nullptr, 0, nullptr);
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getTimeFromColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_INTEGER__FilterBank_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getFrequencyFromRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getValueInCell);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, GuiMenu_DEPTH_1, 0);
	praat_addAction1 (klas, 1, U"Get frequency in Hertz...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getFrequencyInHertz);
	praat_addAction1 (klas, 1, U"Get frequency in Bark...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getFrequencyInBark);
	praat_addAction1 (klas, 1, U"Get frequency in mel...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__FilterBank_getFrequencyInMel);
}

static void praat_FilterBank_modify_init (ClassInfo klas);
static void praat_FilterBank_modify_init (ClassInfo klas) {
	// praat_addAction1 (klas, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (klas, 0, U"Equalize intensities...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1, MODIFY_EACH__FilterBank_equalizeIntensities);
}

static void praat_FilterBank_draw_init (ClassInfo klas);
static void praat_FilterBank_draw_init (ClassInfo klas) {
	// praat_addAction1 (klas, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (klas, 0, U"Draw filters...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_drawFilters);
	praat_addAction1 (klas, 0, U"Draw one contour...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_drawOneContour);
	praat_addAction1 (klas, 0, U"Draw contours...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_drawContours);
	praat_addAction1 (klas, 0, U"Paint image...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_paintImage);
	praat_addAction1 (klas, 0, U"Paint contours...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_paintContours);
	praat_addAction1 (klas, 0, U"Paint cells...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_paintCells);
	praat_addAction1 (klas, 0, U"Paint surface...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_paintSurface);
	praat_addAction1 (klas, 0, U"-- frequency scales --", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1, 0);
	praat_addAction1 (klas, 0, U"Draw frequency scales...", nullptr, GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__FilterBank_drawFrequencyScales);
}

static void praat_FilterBank_all_init (ClassInfo klas);
static void praat_FilterBank_all_init (ClassInfo klas) {
	praat_FilterBank_draw_init (klas);
	praat_FilterBank_query_init (klas);
	praat_FilterBank_modify_init (klas);
	praat_addAction1 (klas, 0, U"To Intensity", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_EACH_TO_ONE__FilterBank_to_Intensity);
	praat_addAction1 (klas, 0, U"To Matrix", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_EACH_TO_ONE__FilterBank_to_Matrix);
	praat_addAction1 (klas, 2, U"Cross-correlate...", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_TWO_TO_ONE__FilterBanks_crossCorrelate);
	praat_addAction1 (klas, 2, U"Convolve...", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_TWO_TO_ONE__FilterBanks_convolve);
}

static void praat_FunctionSeries_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (klas, 0, U"Draw...", nullptr, 1, 
			GRAPHICS_EACH__FunctionSeries_draw);
	praat_addAction1 (klas, 0, U"Draw basis function...", nullptr, 1, 
			GRAPHICS_EACH__FunctionSeries_drawBasisFunction);
	praat_addAction1 (klas, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (klas, 1, U"Get number of coefficients", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__FunctionSeries_getNumberOfCoefficients);
	praat_addAction1 (klas, 1, U"Get coefficient...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FunctionSeries_getCoefficient);
	praat_addAction1 (klas, 1, U"Get degree", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__FunctionSeries_getDegree);
	praat_addAction1 (klas, 0, U"-- function specifics --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FunctionSeries_getValue);
	praat_addAction1 (klas, 1, U"Get minimum...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FunctionSeries_getMinimum);
	praat_addAction1 (klas, 1, U"Get x of minimum...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FunctionSeries_getXOfMinimum);
	praat_addAction1 (klas, 1, U"Get maximum...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FunctionSeries_getMaximum);
	praat_addAction1 (klas, 1, U"Get x of maximum...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FunctionSeries_getXOfMaximum);
	praat_addAction1 (klas, 0, U"Modify -", nullptr, 0, 0);
	praat_addAction1 (klas, 1, U"Set domain...", nullptr, 1, 
			MODIFY_EACH__FunctionSeries_setDomain);
	praat_addAction1 (klas, 1, U"Set coefficient...", nullptr, 1,
			MODIFY_EACH__FunctionSeries_setCoefficient);
	praat_addAction1 (klas, 0, U"Analyse", nullptr, 0, 0);
}

/* Query buttons for frame-based frequency x time subclasses of matrix. */

void praat_BandFilterSpectrogram_query_init (ClassInfo klas) {
	praat_TimeFrameSampled_query_init (klas);
	praat_addAction1 (klas, 1, U"Get time from column...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getTimeFromColumn);
	praat_addAction1 (klas, 1, U"-- frequencies --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get lowest frequency", nullptr, 1, 
			QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getLowestFrequency);
	praat_addAction1 (klas, 1, U"Get highest frequency", nullptr, 1,
			QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getHighestFrequency);
	praat_addAction1 (klas, 1, U"Get number of frequencies", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__BandFilterSpectrogram_getNumberOfFrequencies);
	praat_addAction1 (klas, 1, U"Get frequency distance", nullptr, 1, 
			QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getFrequencyDistance);
	praat_addAction1 (klas, 1, U"Get frequency from row...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getFrequencyFromRow);
	praat_addAction1 (klas, 1, U"-- get value --", nullptr, 1, 0);
	praat_addAction1 (klas, 1, U"Get value in cell...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__BandFilterSpectrogram_getValueInCell);
}

static void praat_PatternList_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (klas, 1, U"Get number of patterns", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__PatternList_getNumberOfPatterns);
	praat_addAction1 (klas, 1, U"Get pattern size", nullptr, 1, 
		QUERY_ONE_FOR_INTEGER__PatternList_getPatternSize);
	praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__PatternList_getValue);

}

static void praat_Spline_init (ClassInfo klas) {
	praat_FunctionSeries_init (klas);
	praat_addAction1 (klas, 0, U"Draw knots...", U"Draw basis function...", 1, 
			GRAPHICS_EACH__Spline_drawKnots);
	praat_addAction1 (klas, 1, U"Get order", U"Get degree", 1, 
		QUERY_ONE_FOR_INTEGER__Spline_getOrder);
	praat_addAction1 (klas, 1, U"Scale x...", U"Analyse", 0, 
			CONVERT_EACH_TO_ONE__Spline_scaleX);
}

static void praat_SSCP_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"-- statistics --", U"Get value...", 1, 0);
	praat_addAction1 (klas, 1, U"Get number of observations", U"-- statistics --", 1, 
		QUERY_ONE_FOR_INTEGER__SSCP_getNumberOfObservations);
	praat_addAction1 (klas, 1, U"Get degrees of freedom", U"Get number of observations", 1, 
			QUERY_ONE_FOR_REAL__SSCP_getDegreesOfFreedom);
	praat_addAction1 (klas, 1, U"Get centroid element...", U"Get degrees of freedom", 1, 
			QUERY_ONE_FOR_REAL__SSCP_getCentroidElement);
	praat_addAction1 (klas, 1, U"Get ln(determinant)", U"Get centroid element...", 1, 
			QUERY_ONE_FOR_REAL__SSCP_getLnDeterminant);
}

static void praat_SSCP_extract_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Extract centroid", U"Extract -", 1, 
			CONVERT_EACH_TO_ONE__SSCP_extractCentroid);
}

FORM (MODIFY_SSCP_setValue, U"Covariance: Set value", U"Covariance: Set value...") {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (value, U"New value", U"1.0")
	OK
DO
	MODIFY_EACH (SSCP)
		SSCP_setValue (me, rowNumber, columnNumber, value);
	MODIFY_EACH_END
}

FORM (MODIFY_SSCP_setCentroid, U"", nullptr) {
	NATURAL (elementNumber, U"Element number", U"1")
	REAL (value, U"New value", U"1.0")
	OK
DO
	MODIFY_EACH (SSCP)
		SSCP_setCentroid (me, elementNumber, value);
	MODIFY_EACH_END
}

void praat_SSCP_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_removeAction (klas, nullptr, nullptr, U"Set value...");
	praat_addAction1 (klas, 1, U"Set centroid...", U"Formula...", 1, 
			MODIFY_SSCP_setCentroid);
	praat_addAction1 (klas, 1, U"Set value...", U"Formula...", 1, 
			MODIFY_SSCP_setValue);
	praat_addAction1 (klas, 0, U"To TableOfReal", U"To Matrix", 1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_TableOfReal);
}

void praat_TableOfReal_init2 (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, U"To TableOfReal", U"To Matrix", 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_TableOfReal);
}

void praat_EditDistanceTable_as_TableOfReal_init (ClassInfo klas) {
	praat_TableOfReal_init (klas);
	praat_addAction1 (klas, 0, U"Set default costs...", U"Formula...", 1, 
			MODIFY_EditDistanceTable_setDefaultCosts);
	praat_removeAction (klas, nullptr, nullptr, U"Draw as numbers...");
	praat_addAction1 (klas, 0, U"Draw...", U"Draw -", 1, 
			GRAPHICS_EACH__EditDistanceTable_draw);
	praat_addAction1 (klas, 0, U"Draw edit operations", U"Draw...", 1, 
			GRAPHICS_EACH__EditDistanceTable_drawEditOperations);
	praat_removeAction (klas, nullptr, nullptr, U"Draw as numbers if...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw as squares...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw vertical lines...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw horizontal lines...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw left and right lines...");
	praat_removeAction (klas, nullptr, nullptr, U"Draw top and bottom lines...");
	praat_removeAction (klas, nullptr, nullptr, U"-- draw lines --");
}

void praat_David_generics_new_init ();
void praat_David_generics_new_init () {

	Thing_recognizeClassesByName (
		classPermutation,
		classPolynomial, classLegendreSeries, classChebyshevSeries, classMSpline, classISpline
	);

	praat_addMenuCommand (U"Objects", U"New", U"Create Permutation...", nullptr, 1,
						  CREATE_ONE__Permutation_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Permutation...", nullptr, 1,
						  CREATE_ONE__Permutation_createSimplePermutation);
	praat_addMenuCommand (U"Objects", U"New", U"Polynomial", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial...", nullptr, 2,
			CREATE_ONE__Polynomial_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial from product terms...", nullptr, 2,
			CREATE_ONE__Polynomial_createFromProductTerms);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polynomial from real zeros...", nullptr, 2,
			CREATE_ONE__Polynomial_createFromRealZeros);
	praat_addMenuCommand (U"Objects", U"New", U"Create LegendreSeries...", nullptr, 2,
			CREATE_ONE__LegendreSeries_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create ChebyshevSeries...", nullptr, 2,
			CREATE_ONE__ChebyshevSeries_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create MSpline...", nullptr, 2,
			CREATE_ONE__MSpline_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create ISpline...", nullptr, 2,
			CREATE_ONE__ISpline_create);
}

void praat_David_init ();
void praat_David_init () {

	Data_recognizeFileType (cmuAudioFileRecognizer);
	Data_recognizeFileType (oggFileRecognizer);

	Thing_recognizeClassesByName (classActivationList, classBarkFilter, classBarkSpectrogram,
		classCategories, classCepstrum, classCCA,
		classClassificationTable, classComplexSpectrogram, classConfusion,
		classCorrelation, classCovariance, classDiscriminant, classDTW,
		classEigen, classExcitationList, classEditCostsTable, classEditDistanceTable,
		classFileInMemory, classFileInMemorySet,
		classFormantFilter,
		classIndex, classKlattTable, classNMF,
		classMelFilter, classMelSpectrogram, classNavigationContext,
		classPatternList, classPCA, classRoots,
		classSimpleString, classStringsIndex, classSpeechSynthesizer, classSPINET, classSSCP,
		classSVD, classTextGridNavigator, classTextGridTierNavigator
	);

	Thing_recognizeClassByOtherName (classExcitationList, U"Excitations");
	Thing_recognizeClassByOtherName (classActivationList, U"Activation");
	Thing_recognizeClassByOtherName (classPatternList, U"Pattern");
	Thing_recognizeClassByOtherName (classFileInMemorySet, U"FilesInMemory");

	structVowelEditor :: f_preferences ();

	{// scope
		//TRACE
		double t = Melder_clock ();
		classSpeechSynthesizer_initClass ();
		trace (U"initializing eSpeak data took ", Melder_fixed (1000 * (Melder_clock () - t), 3), U" milliseconds");
	}
	
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get TukeyQ...", 0, GuiMenu_HIDDEN,
			QUERY_NONE_FOR_REAL__Praat_getTukeyQ);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get invTukeyQ...", 0, GuiMenu_HIDDEN,
			QUERY_NONE_FOR_REAL__Praat_getInvTukeyQ);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Get incomplete gamma...", 0, GuiMenu_HIDDEN,
			QUERY_NONE_FOR_COMPLEX__Praat_getIncompleteGamma);

	praat_addMenuCommand (U"Objects", U"New", U"Create Sound as gammatone... || Create Sound from gammatone...", U"Create Sound as tone complex...", 1,
			CREATE_ONE__Sound_createAsGammaTone);   // alternative COMPATIBILITY <= 2016
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound as Shepard tone... || Create Sound from Shepard tone...", U"Create Sound as gammatone...", GuiMenu_DEPTH_1,
			CREATE_ONE__Sound_createAsShepardTone);   // alternative COMPATIBILITY <= 2016
	praat_addMenuCommand (U"Objects", U"New", U"Create Sound from VowelEditor...", U"Create Sound as Shepard tone...", GuiMenu_DEPTH_1 | GuiMenu_NO_API,
			CREATION_WINDOW__VowelEditor_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create TextGridNavigator...", U"Create Corpus...", GuiMenu_HIDDEN,
			CREATE_ONE__TextGridNavigator_createSimple);

	/*
		The `New/SpeechSynthesizer` commands don't require us to supply the `after` argument,
		because `praat_David_init()` is called between `praat_uvafon_Artsynth_init()` and `praat_uvafon_gram_init()`.
		(last checked 2024-09-07)
	*/
	praat_addMenuCommand (U"Objects", U"New", U"Text-to-speech synthesis", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"SpeechSynthesizer help", nullptr, 1,
			HELP__SpeechSynthesizer_help);
	praat_addMenuCommand (U"Objects", U"New", U"    (more info)", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Tabulate SpeechSynthesizer language properties", nullptr, 2,
			CREATE_ONE__TabulateSpeechSynthesizerLanguageProperties);
	praat_addMenuCommand (U"Objects", U"New", U"Tabulate SpeechSynthesizer voice properties", nullptr, 2,
			CREATE_ONE__TabulateSpeechSynthesizerVoiceProperties);
	praat_addMenuCommand (U"Objects", U"New", U"-- new SpeechSynthesizer --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create SpeechSynthesizer...", nullptr, 1,
			CREATE_ONE__SpeechSynthesizer_create);

	praat_addMenuCommand (U"Objects", U"New", U"Data sets from the literature", U"Create Table without column names...", 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Peterson & Barney 1952)", U"Data sets from the literature", 2,
			CREATE_ONE__Table_create_petersonBarney1952);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Hillenbrand et al. 1995)", U"Data sets from the literature",
		GuiMenu_HIDDEN + GuiMenu_DEPTH_2,
			CREATE_ONE__Table_create_hillenbrandEtAl1995);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Pols & Van Nierop 1973)", U"Create formant table (Peterson & Barney 1952)", 2,
			CREATE_ONE__Table_create_polsVanNierop1973);
	praat_addMenuCommand (U"Objects", U"New", U"Create formant table (Weenink 1985)", U"Create formant table (Pols & Van Nierop 1973)", 2,
			CREATE_ONE__Table_create_weenink1983);
	praat_addMenuCommand (U"Objects", U"New", U"Create H1H2 table (Keating & Esposito 2006)", U"Create formant table (Weenink 1985)", 2,
			CREATE_ONE__Table_create_keatingEsposito2006);
	praat_addMenuCommand (U"Objects", U"New", U"Create Table (Ganong 1980)", U"Create H1H2 table (Keating & Esposito 2006)", 2,
			CREATE_ONE__Table_create_ganong1980);
	praat_addMenuCommand (U"Objects", U"New", U"-- new TableOfReal --", U"Create Table (Ganong 1980)", 2, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create iris data set", U"-- new TableOfReal --" , 2,
			CREATE_ONE__CreateIrisDataset);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Pols 1973)...", U"Create iris data set", 2, 
			CREATE_ONE__TableOfReal_create_pols1973);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Van Nierop 1973)...", U"Create TableOfReal (Pols 1973)...", 2, 
			CREATE_ONE__TableOfReal_create_vanNierop1973);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Weenink 1985)...", U"Create TableOfReal (Van Nierop 1973)...", 2,
			CREATE_ONE__TableOfReal_create_weenink1983);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal (Sandwell 1987)", U"Create TableOfReal (Weenink 1985)...", 2,
			CREATE_ONE__Table_create_sandwell1987);
	praat_addMenuCommand (U"Objects", U"New", U"-- new TableOfReal children --", U"Create TableOfReal (Weenink 1985)...", 1,
			CREATE_ONE__Confusion_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Confusion...", U"-- new TableOfReal children --", 1,
			CREATE_ONE__Confusion_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Covariance...", U"Create simple Confusion...", 1,
			CREATE_ONE__Covariance_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create simple Correlation...", U"Create simple Covariance...", 1, 
			CREATE_ONE__Correlation_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create empty EditCostsTable...", U"Create simple Covariance...", 1, 
			CREATE_ONE__EditCostsTable_createEmpty);
	praat_addMenuCommand (U"Objects", U"New", U"Create KlattTable example", U"Create TableOfReal (Weenink 1985)...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CREATE_ONE__KlattTable_createExample);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings from tokens...", U"Create Strings as folder list...", 2,
			CREATE_ONE__Strings_createFromTokens);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as tokens...", U"Create Strings from tokens...", GuiMenu_DEPTH_2 | GuiMenu_HIDDEN,
			CREATE_ONE__Strings_createAsTokens);
	praat_addMenuCommand (U"Objects", U"New", U"Create Strings as characters...", U"Create Strings from tokens...", GuiMenu_DEPTH_2 | GuiMenu_HIDDEN,
			CREATE_ONE__Strings_createAsCharacters);

	praat_addMenuCommand (U"Objects", U"New", U"Create simple Polygon...", nullptr, GuiMenu_HIDDEN,
			CREATE_ONE__Polygon_createSimple);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polygon (random vertices)...", nullptr, GuiMenu_DEPRECATED_2016,
			CREATE_ONE__Polygon_createFromRandomPoints);
	praat_addMenuCommand (U"Objects", U"New", U"Create Polygon (random points)...", nullptr, GuiMenu_HIDDEN,
			CREATE_ONE__Polygon_createFromRandomPoints);
	praat_addMenuCommand (U"Objects", U"New", U"FileInMemory", nullptr, GuiMenu_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemory...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			READ_ONE__FileInMemory_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create FileInMemorySet from directory contents...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			CREATE_ONE__FileInMemorySet_createFromDirectoryContents);

	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Little Endian file...", U"Read from special sound file", 1, 
			READ_ONE__Sound_readFromRawFileLE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw 16-bit Big Endian file...", U"Read Sound from raw 16-bit Little Endian file...", 1,
			READ_ONE__Sound_readFromRawFileBE);
	praat_addMenuCommand (U"Objects", U"Open", U"Read KlattTable from raw text file...", U"Read Matrix from raw text file...", GuiMenu_HIDDEN,
			READ_ONE__KlattTable_readFromRawTextFile);

	praat_addAction1 (classActivationList, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classActivationList, 0, U"Formula...", nullptr, 0, 
			MODIFY_ActivationList_formula);
	praat_addAction1 (classActivationList, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classActivationList, 0, U"To Matrix", nullptr, 0, 
			CONVERT_EACH_TO_ONE__ActivationList_to_Matrix);
	praat_addAction1 (classActivationList, 0, U"To PatternList", nullptr, 0, 
			CONVERT_EACH_TO_ONE__ActivationList_to_PatternList);

	praat_addAction2 (classActivationList, 1, classCategories, 1, U"To TableOfReal", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__ActivationList_Categories_to_TableOfReal);
	
	praat_addAction1 (classAmplitudeTier, 0, U"Get value at time...", U"Get time from index...", 1, 
			QUERY_ONE_FOR_REAL__AmplitudeTier_getValueAtTime);
	praat_addAction1 (classAmplitudeTier, 0, U"Get value at index...", U"Get value at time...", 1, 
			QUERY_ONE_FOR_REAL__AmplitudeTier_getValueAtIndex);

	praat_addAction1 (classBarkFilter, 0, U"BarkFilter help", nullptr, 0, HELP__BarkFilter_help);
	praat_FilterBank_all_init (classBarkFilter);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__BarkFilter_drawSpectrum);
	praat_addAction1 (classBarkFilter, 1, U"Draw filter functions...", U"Draw filters...", GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__BarkFilter_drawSekeyHansonFilterFunctions);
	praat_addAction1 (classBarkFilter, 0, U"Paint...", U"Draw filters...", GuiMenu_DEPTH_1,
			GRAPHICS_EACH__BarkFilter_paint);	// deprecated 2014
	praat_addAction1 (classBarkFilter, 0, U"To BarkSpectrogram", nullptr, 0,
			CONVERT_EACH_TO_ONE__BarkFilter_to_BarkSpectrogram);

	praat_addAction1 (classBarkSpectrogram, 0, U"BarkSpectrogram help", nullptr, 0,
			HELP__BarkSpectrogram_help);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw frequency scale...", nullptr, 1,
			GRAPHICS_EACH__BarkSpectrogram_drawFrequencyScale);
	praat_addAction1 (classBarkSpectrogram, 0, U"Paint image...", nullptr, 1,
			GRAPHICS_EACH__BarkSpectrogram_paintImage);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw Sekey-Hanson auditory filters...", nullptr, 1,
			GRAPHICS_EACH__BarkSpectrogram_drawSekeyHansonAuditoryFilters);
	praat_addAction1 (classBarkSpectrogram, 0, U"Draw spectrum at nearest time slice...", nullptr, 1,
			GRAPHICS_EACH__BarkSpectrogram_drawSpectrumAtNearestTimeSlice);
	praat_addAction1 (classBarkSpectrogram, 0, U"Query -", nullptr, 0, nullptr);
	praat_BandFilterSpectrogram_query_init (classBarkSpectrogram);
	praat_addAction1 (classBarkSpectrogram, 0, U"Equalize intensities...", nullptr, 0, 
			MODIFY_EACH__BandFilterSpectrogram_equalizeIntensities);
	praat_addAction1 (classBarkSpectrogram, 0, U"To Intensity", nullptr, 0,
			CONVERT_EACH_TO_ONE__BandFilterSpectrogram_to_Intensity);
	praat_addAction1 (classBarkSpectrogram, 0, U"To Matrix...", nullptr, 0,
			CONVERT_EACH_TO_ONE__BandFilterSpectrogram_to_Matrix);
	praat_addAction1 (classBarkSpectrogram, 2, U"Cross-correlate...", nullptr, 0,
			CONVERT_TWO_TO_ONE__BandFilterSpectrograms_crossCorrelate);
	praat_addAction1 (classBarkSpectrogram, 2, U"Convolve...", nullptr, 0,
			CONVERT_TWO_TO_ONE__BandFilterSpectrograms_convolve);

	praat_addAction1 (classCategories, 0, U"View & Edit || Edit", nullptr, GuiMenu_NO_API,
			EDITOR_ONE_Categories_edit);
	praat_addAction1 (classCategories, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classCategories, 1, U"Get number of categories", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__Categories_getNumberOfCategories);
		praat_addAction1 (classCategories, 2, U"Get number of differences", nullptr, 1,
			QUERY_TWO_FOR_REAL__Categories_getNumberOfDifferences);
		praat_addAction1 (classCategories, 2, U"Get fraction different", nullptr, 1,
			QUERY_TWO_FOR_REAL__Categories_getFractionDifferent);
	praat_addAction1 (classCategories, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classCategories, 1, U"Append category... || Append 1 category...",
			nullptr, 1, MODIFY_Categories_appendCategory);
		praat_addAction1 (classCategories, 0, U"Permute items (in-place)", nullptr, 1,
			MODIFY_EACH__Categories_permuteItems_inplace);	
	praat_addAction1 (classCategories, 0, U"Extract", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 0, U"To unique Categories", nullptr, 0,
			CONVERT_EACH_TO_ONE__Categories_selectUniqueItems);
	praat_addAction1 (classCategories, 0, U"Analyse", nullptr, 0, nullptr); // TODO name
	praat_addAction1 (classCategories, 2, U"To Confusion", nullptr, 0,
			CONVERT_TWO_TO_ONE__Categories_to_Confusion);
	praat_addAction1 (classCategories, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classCategories, 2, U"Join", nullptr, 0, 
			CONVERT_TWO_TO_ONE__Categories_join);
	praat_addAction1 (classCategories, 0, U"Permute items", nullptr, 0,
			CONVERT_EACH_TO_ONE__Categories_permuteItems);
	praat_addAction1 (classCategories, 0, U"To Strings", nullptr, 0,
			CONVERT_EACH_TO_ONE__Categories_to_Strings);

	praat_addAction1 (classChebyshevSeries, 0, U"ChebyshevSeries help", nullptr, 0,
			HELP__ChebyshevSeries_help);
	praat_FunctionSeries_init (classChebyshevSeries);
	praat_addAction1 (classChebyshevSeries, 0, U"To Polynomial", U"Analyse", 0,
			CONVERT_EACH_TO_ONE__ChebyshevSeries_to_Polynomial);

	praat_addAction1 (classCCA, 1, U"CCA help", nullptr, 0,
			HELP__CCA_help);
	praat_addAction1 (classCCA, 1, U"Draw eigenvector...", nullptr, 0,
			GRAPHICS_EACH__CCA_drawEigenvector);
	praat_addAction1 (classCCA, 1, U"Get number of correlations", nullptr, 0,
		QUERY_ONE_FOR_INTEGER__CCA_getNumberOfCorrelations);
	praat_addAction1 (classCCA, 1, U"Get correlation...", nullptr, 0,
			QUERY_ONE_FOR_REAL__CCA_getCorrelation);
	praat_addAction1 (classCCA, 1, U"Get eigenvector element...", nullptr, 0,
			QUERY_ONE_FOR_REAL__CCA_getEigenvectorElement);
	praat_addAction1 (classCCA, 1, U"Get zero correlation probability...", nullptr, 0,
			QUERY_ONE_FOR_REAL__CCA_getZeroCorrelationProbability);
	praat_addAction1 (classCCA, 1, U"Extract Eigen...", nullptr, 0,
			CONVERT_EACH_TO_ONE__CCA_extractEigen);

	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (scores)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__CCA_TableOfReal_to_TableOfReal_scores);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"To TableOfReal (loadings)", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__CCA_TableOfReal_to_TableOfReal_loadings);
	praat_addAction2 (classCCA, 1, classTableOfReal, 1, U"Predict...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__CCA_TableOfReal_predict);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"To TableOfReal (loadings)", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__CCA_Correlation_to_TableOfReal_loadings);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get variance fraction...", nullptr, 0,
			QUERY_ONE_AND_ONE_FOR_REAL__CCA_Correlation_getVarianceFraction);
	praat_addAction2 (classCCA, 1, classCorrelation, 1, U"Get redundancy (sl)...", nullptr, 0,
			QUERY_ONE_AND_ONE_FOR_REAL__CCA_Correlation_getRedundancy_sl);

	praat_addAction1 (classComplexSpectrogram, 0, U"ComplexSpectrogram help", nullptr, 0,
			HELP__ComplexSpectrogram_help);
	praat_addAction1 (classComplexSpectrogram, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Sound...", nullptr, 0,
			CONVERT_EACH_TO_ONE__ComplexSpectrogram_to_Sound);
	praat_addAction1 (classComplexSpectrogram, 0, U"Down to Spectrogram", nullptr, 0,
			CONVERT_EACH_TO_ONE__ComplexSpectrogram_downto_Spectrogram);
	praat_addAction1 (classComplexSpectrogram, 0, U"To Spectrum (slice)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__ComplexSpectrogram_to_Spectrum_slice);
	praat_addAction2 (classComplexSpectrogram, 1, classSpectrogram, 1, U"Replace amplitudes", nullptr, 0,
			MODIFY_ComplexSpectrogram_Spectrogram_replaceAmplitudes);

	praat_addAction1 (classConfusion, 0, U"Confusion help", nullptr, 0,
			HELP__Confusion_help);
	praat_TableOfReal_init2 (classConfusion);
	praat_removeAction (classConfusion, nullptr, nullptr, U"Draw as numbers...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by label...");
	praat_removeAction (classConfusion, nullptr, nullptr, U"Sort by column...");
	praat_addAction1 (classConfusion, 0, U"Draw as numbers...", U"Draw -", 1, 
			GRAPHICS_EACH__Confusion_drawAsNumbers);
	praat_addAction1 (classConfusion, 1, U"Get value (labels)...", U"Get value...", 1, 
			QUERY_ONE_FOR_REAL__Confusion_getValue_labels);
	praat_addAction1 (classConfusion, 0, U"-- confusion statistics --", U"Get value (labels)...", 1, nullptr);
	praat_addAction1 (classConfusion, 1, U"Get fraction correct", U"-- confusion statistics --", 1,
			QUERY_ONE_FOR_REAL__Confusion_getFractionCorrect);
	praat_addAction1 (classConfusion, 1, U"Get stimulus sum...", U"Get fraction correct", 1,
			QUERY_ONE_FOR_REAL__Confusion_getStimulusSum);
	praat_addAction1 (classConfusion, 1, U"Get row sum...", U"Get fraction correct", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL__TableOfReal_getRowSum);
	praat_addAction1 (classConfusion, 1, U"Get response sum...", U"Get stimulus sum...", 1,
			QUERY_ONE_FOR_REAL__Confusion_getResponseSum);
	praat_addAction1 (classConfusion, 1, U"Get column sum...", U"Get row sum...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL__TableOfReal_getColumnSum);
	praat_addAction1 (classConfusion, 1, U"Get grand sum", U"Get response sum...", 1,
			QUERY_ONE_FOR_REAL__TableOfReal_getGrandSum);
	praat_addAction1 (classConfusion, 0, U"Increase...", U"Formula...", 1,
			MODIFY_Confusion_increase);
	praat_addAction1 (classConfusion, 0, U"To TableOfReal (marginals)", U"To TableOfReal", 0,
			CONVERT_EACH_TO_ONE__Confusion_to_TableOfReal_marginals);
	praat_addAction1 (classConfusion, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classConfusion, 0, U"Condense...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Confusion_condense);
	praat_addAction1 (classConfusion, 0, U"Group...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Confusion_group);
	praat_addAction1 (classConfusion, 0, U"Group stimuli...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Confusion_groupStimuli);
	praat_addAction1 (classConfusion, 0, U"Group responses...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Confusion_groupResponses);
	praat_addAction1 (classConfusion, 2, U"To difference matrix", nullptr, 0,
			CONVERT_TWO_TO_ONE__Confusion_difference); // TODO to_Matrix_difference ?

	praat_addAction2 (classConfusion, 1, classClassificationTable, 1, U"Increase confusion count", nullptr, 0,
			MODIFY_Confusion_ClassificationTable_increaseConfusionCount);

	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classConfusion, 1, classMatrix, 1, U"Draw confusion...", nullptr, 0,
			GRAPHICS_ONE_AND_ONE__Confusion_Matrix_draw);

	praat_addAction1 (classCovariance, 0, U"Covariance help", nullptr, 0,
			HELP__Covariance_help);
	praat_SSCP_as_TableOfReal_init (classCovariance);
	praat_SSCP_query_init (classCovariance);
	praat_SSCP_extract_init (classCovariance);
	praat_addAction1 (classCovariance, 1, U"Get probability at position...", U"Get value...", 1,
			QUERY_ONE_FOR_REAL__Covariance_getProbabilityAtPosition);
	praat_addAction1 (classCovariance, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1,
			QUERY_ONE_FOR_REAL__SSCP_getDiagonality_bartlett);
	praat_addAction1 (classCovariance, 1, U"Get significance of one mean...", U"Get diagonality (bartlett)...", 1,
			QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfOneMean);
	praat_addAction1 (classCovariance, 1, U"Get significance of means difference...", U"Get significance of one mean...", 1,
			QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfMeansDifference);
	praat_addAction1 (classCovariance, 1, U"Get significance of one variance...", U"Get significance of means difference...", 1,
			QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfOneVariance);
	praat_addAction1 (classCovariance, 1, U"Get significance of variances ratio...", U"Get significance of one variance...", 1,
			QUERY_ONE_FOR_REAL__Covariance_getSignificanceOfVariancesRatio);
	praat_addAction1 (classCovariance, 1, U"Get fraction variance...", U"Get significance of variances ratio...", 1,
			QUERY_ONE_FOR_REAL__Covariance_getFractionVariance);
	praat_addAction1 (classCovariance, 2, U"Report multivariate mean difference...", U"Get fraction variance...", 1,
			INFO_TWO__Covariances_reportMultivariateMeanDifference);
	praat_addAction1 (classCovariance, 2, U"Difference", U"Report multivariate mean difference...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_NONE__Covariances_reportEquality);
	praat_addAction1 (classCovariance, 0, U"Report equality of covariances", U"Report multivariate mean difference...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_NONE__Covariances_reportEquality);
	praat_addAction1 (classCovariance, 0, U"To TableOfReal (random sampling)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Covariance_to_TableOfReal_randomSampling);
	praat_addAction1 (classCovariance, 0, U"To Correlation", nullptr, 0,
			CONVERT_EACH_TO_ONE__Covariance_to_Correlation);
	praat_addAction1 (classCovariance, 0, U"To PCA", nullptr, 0,
			CONVERT_EACH_TO_ONE__Covariance_to_PCA);
	praat_addAction1 (classCovariance, 0, U"Pool", nullptr, 0,
			COMBINE_ALL_LISTED_TO_ONE__Covariances_pool);
	praat_addAction1 (classCovariance, 0, U"To Covariance (between)", nullptr, 0,
			COMBINE_ALL_LISTED_TO_ONE__Covariances_to_Covariance_between);
	praat_addAction1 (classCovariance, 0, U"To Covariance (within)", nullptr, 0,
			COMBINE_ALL_LISTED_TO_ONE__Covariances_to_Covariance_within);

	praat_addAction2 (classCovariance, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Covariance_TableOfReal_mahalanobis);
	
	praat_addAction1 (classClassificationTable, 0, U"ClassificationTable help", nullptr, 0,
			HELP__ClassificationTable_help);
	praat_TableOfReal_init (classClassificationTable);
	praat_addAction1 (classClassificationTable, 0, U"Get class index at maximum in row...", U"Get column index...", 1,
		QUERY_ONE_FOR_INTEGER__ClassificationTable_getClassIndexAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"Get class label at maximum in row...", U"Get class index at maximum in row...", 1,
		QUERY_ONE_FOR_STRING__ClassificationTable_getClassLabelAtMaximumInRow);
	praat_addAction1 (classClassificationTable, 0, U"To Confusion...", nullptr, 0,
			CONVERT_EACH_TO_ONE__ClassificationTable_to_Confusion);
	praat_addAction1 (classClassificationTable, 0, U"To Confusion", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_EACH_TO_ONE__ClassificationTable_to_Confusion_old);   // replace with To Confusion: 0
	praat_addAction1 (classClassificationTable, 0, U"To Correlation (columns)", nullptr, 0,
			CONVERT_EACH_TO_ONE__ClassificationTable_to_Correlation_columns);
	praat_addAction1 (classClassificationTable, 0, U"To Strings (max. prob.)", nullptr, 0,
			CONVERT_EACH_TO_ONE__ClassificationTable_to_Strings_maximumProbability); // TODO name?

	praat_addAction1 (classCorrelation, 0, U"Correlation help", nullptr, 0, 
			HELP__Correlation_help);
	praat_TableOfReal_init2 (classCorrelation);
	praat_SSCP_query_init (classCorrelation);
	praat_SSCP_extract_init (classCorrelation);
	praat_addAction1 (classCorrelation, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1, 
			QUERY_ONE_FOR_REAL__Correlation_getDiagonality_bartlett);
	praat_addAction1 (classCorrelation, 0, U"Confidence intervals...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Correlation_confidenceIntervals);
	praat_addAction1 (classCorrelation, 0, U"To PCA", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Correlation_to_PCA);

	praat_addAction1 (classDiscriminant, 0, U"Discriminant help", 0, 0,
			HELP__Discriminant_help);
	praat_addAction1 (classDiscriminant, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvalues...", nullptr, 1,
			GRAPHICS_EACH__Discriminant_drawEigenvalues);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvalues (scree)...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			WARNING__Eigen_drawEigenvalues_scree);
	praat_addAction1 (classDiscriminant, 0, U"Draw eigenvector...", nullptr, 1,
			GRAPHICS_EACH__Discriminant_drawEigenvector);

	praat_addAction1 (classDiscriminant, 0, U"-- sscps --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 0, U"Draw sigma ellipses...", 0, 1,
			GRAPHICS_EACH__Discriminant_drawSigmaEllipses);
	praat_addAction1 (classDiscriminant, 0, U"Draw one sigma ellipse...", 0, 1,
			GRAPHICS_EACH__Discriminant_drawOneSigmaEllipse);
	praat_addAction1 (classDiscriminant, 0, U"Draw confidence ellipses...", 0, 1,
			GRAPHICS_EACH__Discriminant_drawConfidenceEllipses);

	praat_addAction1 (classDiscriminant, 1, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classDiscriminant, 1, U"-- eigen structure --", nullptr, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvalue...", nullptr, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getEigenvalue);
	praat_addAction1 (classDiscriminant, 1, U"Get sum of eigenvalues...", nullptr, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getSumOfEigenvalues);
	praat_addAction1 (classDiscriminant, 1, U"Get number of eigenvectors", nullptr, 1,
		QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfEigenvectors);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector dimension", nullptr, 1,
		QUERY_ONE_FOR_INTEGER__Discriminant_getEigenvectorDimension);
	praat_addAction1 (classDiscriminant, 1, U"Get eigenvector element...", nullptr, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getEigenvectorElement);

	praat_addAction1 (classDiscriminant, 1, U"-- discriminant --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get number of functions", 0, 1,
		QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfFunctions);
	praat_addAction1 (classDiscriminant, 1, U"Get dimension of functions", 0, 1,
		QUERY_ONE_FOR_INTEGER__Discriminant_getDimensionOfFunctions);
	praat_addAction1 (classDiscriminant, 1, U"Get number of groups", 0, 1,
		QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfGroups);
	praat_addAction1 (classDiscriminant, 1, U"Get number of observations...", 0, 1,
		QUERY_ONE_FOR_INTEGER__Discriminant_getNumberOfObservations);
	praat_addAction1 (classDiscriminant, 1, U"-- tests --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get Wilks lambda...", 0, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getWilksLambda);
	praat_addAction1 (classDiscriminant, 1, U"Get cumulative contribution of components...", 0, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getCumulativeContributionOfComponents);
	praat_addAction1 (classDiscriminant, 1, U"Get partial discrimination probability...", 0, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getPartialDiscriminationProbability);
	praat_addAction1 (classDiscriminant, 1, U"Get homogeneity of covariances (box)", 0, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL__Discriminant_getHomegeneityOfCovariances_box);
	praat_addAction1 (classDiscriminant, 1, U"Report equality of covariance matrices", 0, 1, 
			INFO_ONE__Discriminant_reportEqualityOfCovarianceMatrices);
	praat_addAction1 (classDiscriminant, 1, U"-- ellipses --", 0, 1, 0);
	praat_addAction1 (classDiscriminant, 1, U"Get sigma ellipse area...", 0, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getSigmaEllipseArea);
	praat_addAction1 (classDiscriminant, 1, U"Get confidence ellipse area...", 0, 1, 
			QUERY_ONE_FOR_REAL__Discriminant_getConfidenceEllipseArea);
	praat_addAction1 (classDiscriminant, 1, U"Get ln(determinant_group)...", 0, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getLnDeterminant_group);
	praat_addAction1 (classDiscriminant, 1, U"Get ln(determinant_total)", 0, 1,
			QUERY_ONE_FOR_REAL__Discriminant_getLnDeterminant_total);

	praat_addAction1 (classDiscriminant, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classDiscriminant, 1, U"Invert eigenvector...", nullptr, 1,
			MODIFY_Discriminant_invertEigenvector);
	praat_addAction1 (classDiscriminant, 0, U"Align eigenvectors", nullptr, 1,
			MODIFY_ALL__Eigens_alignEigenvectors);

	praat_addAction1 (classDiscriminant, 0, U"Extract -", nullptr, 0, 0);
		praat_addAction1 (classDiscriminant, 0, U"Extract pooled within-groups SSCP", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractPooledWithinGroupsSSCP);
		praat_addAction1 (classDiscriminant, 0, U"Extract within-group SSCP...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractWithinGroupSSCP);
		praat_addAction1 (classDiscriminant, 0, U"Extract between-groups SSCP", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractBetweenGroupsSSCP);
		praat_addAction1 (classDiscriminant, 0, U"Extract group centroids", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractGroupCentroids);
		praat_addAction1 (classDiscriminant, 0, U"Extract group standard deviations", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractGroupStandardDeviations);
		praat_addAction1 (classDiscriminant, 0, U"Extract group labels", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractGroupLabels);
		praat_addAction1 (classDiscriminant, 0, U"Extract Eigen", nullptr, 1,
				CONVERT_EACH_TO_ONE__Discriminant_extractEigen);

	praat_addAction1 (classDiscriminant , 0, U"& TableOfReal: To ClassificationTable?", nullptr, 0,
				HINT__Discriminant_TableOfReal_to_ClassificationTable);

	praat_Eigen_Spectrogram_project (classDiscriminant, classSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classDiscriminant, classMelSpectrogram);

	praat_Eigen_Matrix_project (classDiscriminant, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classDiscriminant, classMelFilter); // deprecated 2014

	praat_addAction2 (classDiscriminant, 1, classPatternList, 1, U"To Categories...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_PatternList_to_Categories);
	praat_addAction2 (classDiscriminant, 1, classSSCP, 1, U"Project", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Eigen_SSCP_project);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Modify Discriminant", nullptr, 0, 0);
	praat_addAction2 (classDiscriminant, 1, classStrings, 1, U"Set group labels", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Discriminant_setGroupLabels);

	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To Configuration...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_to_Configuration);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To ClassificationTable...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_to_ClassificationTable);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_mahalanobis);
	praat_addAction2 (classDiscriminant, 1, classTableOfReal, 1, U"To TableOfReal (mahalanobis, all)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Discriminant_TableOfReal_mahalanobis_all);

	praat_addAction1 (classDTW, 0, U"DTW help", nullptr, 0,
			HELP__DTW_help);
	praat_addAction1 (classDTW, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classDTW, 0, U"Draw path...", nullptr, 1,
			GRAPHICS_EACH__DTW_drawPath);
	praat_addAction1 (classDTW, 0, U"Paint distances...", nullptr, 1,
			GRAPHICS_EACH__DTW_paintDistances);
	praat_addAction1 (classDTW, 0, U"Draw warp (x)...", nullptr, 1,
			GRAPHICS_EACH__DTW_drawWarp_x);
	praat_addAction1 (classDTW, 0, U"Draw warp (y)...", nullptr, 1,
			GRAPHICS_EACH__DTW_drawWarp_y);
	praat_addAction1 (classDTW, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classDTW, 1, U"Query time domains", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (x)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getStartTime_x);
	praat_addAction1 (classDTW, 1, U"Get end time (x)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getEndTime_x);
	praat_addAction1 (classDTW, 1, U"Get total duration (x)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getTotalDuration_x);
	praat_addAction1 (classDTW, 1, U"-- time domain x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get start time (y)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getStartTime_y);
	praat_addAction1 (classDTW, 1, U"Get end time (y)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getEndTime_y);
	praat_addAction1 (classDTW, 1, U"Get total duration (y)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getTotalDuration_y);
	praat_addAction1 (classDTW, 1, U"Query time samplings", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (x)", nullptr, 2,
		QUERY_ONE_FOR_INTEGER__DTW_getNumberOfFrames_x);
	praat_addAction1 (classDTW, 1, U"Get time step (x)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getTimeStep_x);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (x)...", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getTimeFromFrameNumber_x);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (x)...", nullptr, 2,
		QUERY_ONE_FOR_INTEGER__DTW_getFrameNumberFromTime_x);
	praat_addAction1 (classDTW, 1, U"-- time sampling x from y separator --", nullptr, 2, 0);
	praat_addAction1 (classDTW, 1, U"Get number of frames (y)", nullptr, 2,
		QUERY_ONE_FOR_INTEGER__DTW_getNumberOfFrames_y);
	praat_addAction1 (classDTW, 1, U"Get time step (y)", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getTimeStep_y);
	praat_addAction1 (classDTW, 1, U"Get time from frame number (y)...", nullptr, 2,
			QUERY_ONE_FOR_REAL__DTW_getTimeFromFrameNumber_y);
	praat_addAction1 (classDTW, 1, U"Get frame number from time (y)...", nullptr, 2,
		QUERY_ONE_FOR_INTEGER__DTW_getFrameNumberFromTime_y);

	praat_addAction1 (classDTW, 1, U"Get y time from x time...", nullptr, 1,
			QUERY_ONE_FOR_REAL__DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, U"Get x time from y time...", nullptr, 1,
			QUERY_ONE_FOR_REAL__DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, U"Get y time...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__DTW_getYTimeFromXTime);
	praat_addAction1 (classDTW, 1, U"Get x time...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL__DTW_getXTimeFromYTime);
	praat_addAction1 (classDTW, 1, U"Get maximum consecutive steps...", nullptr, 1,
		QUERY_ONE_FOR_INTEGER__DTW_getMaximumConsecutiveSteps);
	praat_addAction1 (classDTW, 1, U"Get time along path...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL__DTW_getPathY);
	praat_addAction1 (classDTW, 1, U"-- distance queries --", nullptr, 1, 0);
	praat_addAction1 (classDTW, 1, U"Get distance value...", nullptr, 1,
			QUERY_ONE_FOR_REAL__DTW_getDistanceValue);
	praat_addAction1 (classDTW, 1, U"Get minimum distance", nullptr, 1,
			QUERY_ONE_FOR_REAL__DTW_getMinimumDistance);
	praat_addAction1 (classDTW, 1, U"Get maximum distance", nullptr, 1,
			QUERY_ONE_FOR_REAL__DTW_getMaximumDistance);
	praat_addAction1 (classDTW, 1, U"Get distance (weighted)", nullptr, 1,
			QUERY_ONE_FOR_REAL__DTW_getDistance_weighted);
	praat_addAction1 (classDTW, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classDTW, 0, U"Formula (distances)...", nullptr, 1,
			MODIFY_DTW_formula_distances);
	praat_addAction1 (classDTW, 0, U"Set distance value...", nullptr, 1, 
			MODIFY_DTW_setDistanceValue);

	praat_addAction1 (classDTW, 0, U"Analyse", nullptr, 0, 0);
    praat_addAction1 (classDTW, 0, U"Find path...", nullptr, GuiMenu_HIDDEN,
			MODIFY_DTW_findPath);
    praat_addAction1 (classDTW, 0, U"Find path (band & slope)...", nullptr, 0,
			MODIFY_DTW_findPath_bandAndSlope);
    praat_addAction1 (classDTW, 0, U"To Polygon...", nullptr, 1,
			CONVERT_EACH_TO_ONE__DTW_to_Polygon);
	praat_addAction1 (classDTW, 0, U"To Matrix (distances)", nullptr, 0,
			CONVERT_EACH_TO_ONE__DTW_to_Matrix_distances);
    praat_addAction1 (classDTW, 0, U"To Matrix (cum. distances)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__DTW_to_Matrix_cumulativeDistances);
	praat_addAction1 (classDTW, 0, U"Swap axes", nullptr, 0,
			CONVERT_EACH_TO_ONE__DTW_swapAxes);

	praat_addAction2 (classDTW, 1, classMatrix, 1, U"Replace matrix", nullptr, 0,
			MODIFY_DTW_Matrix_replace);
	praat_addAction2 (classDTW, 1, classTextGrid, 1, U"To TextGrid (warp times)", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__DTW_TextGrid_to_TextGrid);
	praat_addAction2 (classDTW, 1, classIntervalTier, 1, U"To Table (distances)", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__DTW_IntervalTier_to_Table);

    praat_addAction2 (classDTW, 1, classPolygon, 1, U"Find path inside...", nullptr, 0,
			MODIFY_DTW_Polygon_findPathInside);
    praat_addAction2 (classDTW, 1, classPolygon, 1, U"To Matrix (cum. distances)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__DTW_Polygon_to_Matrix_cumulativeDistances);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw...", nullptr, 0,
			GRAPHICS_TWO_AND_ONE__DTW_Sounds_draw);
	praat_addAction2 (classDTW, 1, classSound, 2, U"Draw warp (x)...", nullptr, 0,
			GRAPHICS_TWO_AND_ONE__DTW_Sounds_drawWarp_x);

	praat_addAction1 (classEditDistanceTable, 1, U"EditDistanceTable help", nullptr, 0, 
			HELP__EditDistanceTable_help);
	praat_EditDistanceTable_as_TableOfReal_init (classEditDistanceTable);
	praat_addAction1 (classEditDistanceTable, 1, U"To TableOfReal (directions)...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__EditDistanceTable_to_TableOfReal_directions);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, U"Set edit costs", nullptr, 0, 
			MODIFY_EditDistanceTable_setEditCosts);
	praat_addAction2 (classEditDistanceTable, 1, classEditCostsTable, 1, U"Set new edit costs", nullptr, GuiMenu_DEPRECATED_2017,
			MODIFY_EditDistanceTable_setEditCosts);

	praat_addAction1 (classEditCostsTable, 1, U"EditCostsTable help", nullptr, 0, 
			HELP__EditCostsTable_help);
	praat_addAction1 (classEditCostsTable, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classEditCostsTable, 1, U"Get target index...", nullptr, 1, 
			QUERY_ONE_FOR_INTEGER__EditCostsTable_getTargetIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get source index...", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__EditCostsTable_getSourceIndex);
	praat_addAction1 (classEditCostsTable, 1, U"Get insertion costs...", nullptr, 1,
			QUERY_ONE_FOR_REAL__EditCostsTable_getInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Get insertion cost...", nullptr, GuiMenu_DEPRECATED_2017,
			QUERY_ONE_FOR_REAL__EditCostsTable_getInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Get deletion costs...", nullptr, 1,
			QUERY_ONE_FOR_REAL__EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get deletion cost...", nullptr, GuiMenu_DEPRECATED_2017,
			QUERY_ONE_FOR_REAL__EditCostsTable_getDeletionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get substitution costs...", nullptr, 1,
			QUERY_ONE_FOR_REAL__EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get substitution cost...", nullptr, GuiMenu_DEPRECATED_2017,
			QUERY_ONE_FOR_REAL__EditCostsTable_getSubstitutionCost);
	praat_addAction1 (classEditCostsTable, 1, U"Get costs (others)...", nullptr, 1,
			QUERY_ONE_FOR_REAL__EditCostsTable_getCosts_others);
	praat_addAction1 (classEditCostsTable, 1, U"Get cost (others)...", nullptr, GuiMenu_DEPRECATED_2017,
			QUERY_ONE_FOR_REAL__EditCostsTable_getCosts_others);
	praat_addAction1 (classEditCostsTable, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classEditCostsTable, 1, U"Set target symbol (index)...", nullptr, 1,
			MODIFY_EditCostsTable_setTargetSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set source symbol (index)...", nullptr, 1,
			MODIFY_EditCostsTable_setSourceSymbol_index);
	praat_addAction1 (classEditCostsTable, 1, U"Set insertion costs...", nullptr, 1,
			MODIFY_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set insertion cost...", nullptr, GuiMenu_DEPRECATED_2017,
			MODIFY_EditCostsTable_setInsertionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set deletion costs...", nullptr, 1,
			MODIFY_EditCostsTable_setDeletionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set substitution costs...", nullptr, 1,
			MODIFY_EditCostsTable_setSubstitutionCosts);
	praat_addAction1 (classEditCostsTable, 1, U"Set costs (others)...", nullptr, 1,
			MODIFY_EditCostsTable_setCosts_others);
	praat_addAction1 (classEditCostsTable, 1, U"To TableOfReal", nullptr, 0,
			CONVERT_EACH_TO_ONE__EditCostsTable_to_TableOfReal);

	praat_addAction1 (classIndex, 0, U"Index help", nullptr, 0, HELP__Index_help);
	praat_addAction1 (classStringsIndex, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classStringsIndex, 1, U"Get number of classes", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__Index_getNumberOfClasses);
		praat_addAction1 (classStringsIndex, 1, U"Get class label...", nullptr, 1,
				QUERY_ONE_FOR_STRING__StringsIndex_getClassLabelFromClassIndex);
		praat_addAction1 (classStringsIndex, 1, U"Get class index...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__StringsIndex_getClassIndex);
		praat_addAction1 (classStringsIndex, 1, U"Get item label...", nullptr, 1,
				QUERY_ONE_FOR_STRING__StringsIndex_getItemLabelFromItemIndex);
		praat_addAction1 (classStringsIndex, 1, U"Get class index from item index...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__StringsIndex_getClassIndexFromItemIndex);
		praat_addAction1 (classStringsIndex, 1, U"Get index...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__Index_getIndex);
		praat_addAction1 (classStringsIndex, 1, U"List all classes", nullptr, 1,
				QUERY_ONE_FOR_STRING_ARRAY__StringsIndex_listAllClasses);
	praat_addAction1 (classStringsIndex, 1, U"To Strings", nullptr, 0,
			CONVERT_EACH_TO_ONE__StringsIndex_to_Strings);
	praat_addAction1 (classStringsIndex, 1, U"To Permutation...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Index_to_Permutation);
	praat_addAction1 (classStringsIndex, 1, U"Extract part...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Index_extractPart);

	praat_addAction1 (classEigen, 0, U"Eigen help", nullptr, 0,
			HELP__Eigen_help);
	praat_addAction1 (classEigen, 0, U"Draw -", nullptr, 0, nullptr);
		praat_Eigen_draw_init (classEigen);
	praat_addAction1 (classEigen, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 1, U"Get number of eigenvalues", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__Eigen_getNumberOfEigenvalues);
		praat_addAction1 (classEigen, 1, U"Get eigenvalue...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__Eigen_getEigenvalue);
		praat_addAction1 (classEigen, 1, U"List eigenvalues", nullptr, 1, 
				QUERY_ONE_FOR_REAL_VECTOR__Eigen_listEigenvalues);
		praat_addAction1 (classEigen, 1, U"Get sum of eigenvalues...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Eigen_getSumOfEigenvalues);
	praat_addAction1 (classEigen, 1, U"-- eigenvectors --", nullptr, 1, 0);
		praat_addAction1 (classEigen, 1, U"Get number of eigenvectors", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__Eigen_getNumberOfEigenvectors);
		praat_addAction1 (classEigen, 1, U"Get eigenvector dimension", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__Eigen_getEigenvectorDimension);
		praat_addAction1 (classEigen, 1, U"Get eigenvector element...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Eigen_getEigenvectorElement);
		praat_addAction1 (classEigen, 1, U"Get eigenvector...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__Eigen_getEigenvector);
		praat_addAction1 (classEigen, 1, U"-- imaginary part --", nullptr, 1, nullptr);
		praat_addAction1 (classEigen, 1, U"Are all eigenvalues real?", nullptr, 1,
				QUERY_ONE_FOR_BOOLEAN__Eigen_areAllEigenvaluesReal);
		praat_addAction1 (classEigen, 1, U"Get eigenvector (imag)...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__Eigen_getEigenvector_imag);
		praat_addAction1 (classEigen, 1, U"List eigenvalues (imag)", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__Eigen_listEigenvalues_imag);
	praat_addAction1 (classEigen, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classEigen, 1, U"Sort...", nullptr, 1,
				MODIFY__Eigen_sort);
		praat_addAction1 (classEigen, 1, U"Invert eigenvector...", nullptr, 1,
				MODIFY__Eigen_invertEigenvector);
	praat_addAction1 (classExcitation, 0, U"Synthesize", U"To Formant...", 0, 0);
	praat_addAction1 (classExcitation, 0, U"To ExcitationList", U"Synthesize", 0,
			COMBINE_ALL_TO_ONE__Excitations_to_ExcitationList);
	praat_addAction1 (classExcitation, 0, U"To Excitations", U"Synthesize", GuiMenu_DEPRECATED_2015,
			COMBINE_ALL_TO_ONE__Excitations_to_ExcitationList);

	praat_addAction1 (classExcitationList, 0, U"Modify", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"Formula...", nullptr, 0,
			MODIFY_ExcitationList_formula);
	praat_addAction1 (classExcitationList, 0, U"Extract", nullptr, 0, 0);
		praat_addAction1 (classExcitationList, 0, U"Extract Excitation...", nullptr, 0,
				CONVERT_EACH_TO_ONE__ExcitationList_extractItem);
	praat_addAction1 (classExcitationList, 0, U"Synthesize", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 2, U"Append", nullptr, 0,
			CONVERT_TWO_TO_ONE__ExcitationList_append);
	praat_addAction1 (classExcitationList, 0, U"Convert", nullptr, 0, 0);
	praat_addAction1 (classExcitationList, 0, U"To PatternList...", nullptr, 0,
			CONVERT_EACH_TO_ONE__ExcitationList_to_PatternList);
	praat_addAction1 (classExcitationList, 0, U"To Pattern...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__ExcitationList_to_PatternList);
	praat_addAction1 (classExcitationList, 0, U"To TableOfReal", nullptr, 0,
			CONVERT_EACH_TO_ONE__ExcitationList_to_TableOfReal);

	praat_addAction2 (classExcitationList, 1, classExcitation, 0, U"Add to ExcitationList", nullptr, 0,
			MODIFY_ExcitationList_addItem);
	praat_addAction2 (classExcitationList, 1, classExcitation, 0, U"Add to Excitations", nullptr, GuiMenu_HIDDEN,
			MODIFY_ExcitationList_addItem);

	praat_addAction1 (classFileInMemory, 1, U"Show as code...", nullptr, 0,
			INFO_ONE__FileInMemory_showAsCode);
	praat_addAction1 (classFileInMemory, 0, U"To FileInMemorySet", nullptr, 0,
			COMBINE_ALL_TO_ONE__FilesInMemory_to_FileInMemorySet);
	praat_addAction1 (classFileInMemory, 0, U"To FilesInMemory", nullptr, GuiMenu_DEPRECATED_2015,
			COMBINE_ALL_TO_ONE__FilesInMemory_to_FileInMemorySet);

	praat_addAction1 (classFileInMemorySet, 1, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classFileInMemorySet, 1, U"Get number of files", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__FileInMemorySet_getNumberOfFiles);
	//praat_addAction1 (classFileInMemorySet, 1, U"Get number of open files", nullptr, 1,
	//		QUERY_ONE_FOR_INTEGER__classFileInMemorySet_getNumberOfOpenFiles);
	praat_addAction1 (classFileInMemorySet, 1, U"Has directory?", nullptr, 1,
			QUERY_ONE_FOR_BOOLEAN__FileInMemorySet_hasDirectory);
	praat_addAction1 (classFileInMemorySet, 1, U"Show as code...", nullptr, 0,
			INFO_ONE__FileInMemorySet_showAsCode);
	praat_addAction1 (classFileInMemorySet, 1, U"Show one file as code...", nullptr, 0,
			INFO_ONE__FileInMemorySet_showOneFileAsCode);
	praat_addAction1 (classFileInMemorySet, 0, U"Merge", nullptr, 0,
			COMBINE_ALL_TO_ONE__FileInMemorySets_merge);
	praat_addAction1 (classFileInMemorySet, 0, U"To Strings (path)", nullptr, 0,
			CONVERT_EACH_TO_ONE__FileInMemorySet_to_Strings_path);
	praat_addAction1 (classFileInMemorySet, 0, U"Extract files...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FileInMemorySet_extractFiles);
	praat_addAction1 (classFileInMemorySet, 0, U"Remove files...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FileInMemorySet_removeFiles);
	praat_addAction2 (classFileInMemorySet, 1, classFileInMemory, 0, U"Add items to set", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__FileInMemorySet_addItemsToSet);
	praat_addAction1 (classFileInMemorySet, 0, U"Down to Table...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FileInMemorySet_downto_Table);
	
	praat_addAction1 (classFormantFilter, 0, U"FormantFilter help", nullptr, GuiMenu_DEPRECATED_2015,
			HELP__FormantFilter_help);
	praat_FilterBank_all_init (classFormantFilter);
	praat_addAction1 (classFormantFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__FormantFilter_drawSpectrum_slice);
	praat_addAction1 (classFormantFilter, 0, U"Draw filter functions...", U"Draw filters...", GuiMenu_DEPTH_1  | GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__FormantFilter_drawFilterFunctions);
	praat_addAction1 (classFormantFilter, 0, U"To Spectrogram", nullptr, 0, 
			CONVERT_EACH_TO_ONE__FormantFilter_to_Spectrogram);

	praat_addAction1 (classFormantGrid, 0, U"Draw...", U"Edit", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			GRAPHICS_EACH__FormantGrid_draw);

	praat_addAction1 (classIntensity, 0, U"To TextGrid (silences)...", U"To IntensityTier (valleys)", 0,
			CONVERT_EACH_TO_ONE__Intensity_to_TextGrid_silences);
	praat_addAction1 (classIntensity, 0, U"Get nearest level crossing...", U"Get time of maximum...", 1,
			QUERY_ONE_FOR_REAL__Intensity_getNearestLevelCrossing);
	praat_addAction1 (classIntensityTier, 0, U"To TextGrid (silences)...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__IntensityTier_to_TextGrid_silences);
	praat_addAction1 (classIntensityTier, 0, U"To Intensity...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__IntensityTier_to_Intensity);

	praat_addAction1 (classISpline, 0, U"ISpline help", nullptr, 0, 
			HELP__ISpline_help);
	praat_Spline_init (classISpline);

	praat_addAction1 (classKlattTable, 0, U"KlattTable help", nullptr, 0,
			HELP__KlattTable_help);
	praat_addAction1 (classKlattTable, 0, U"To Sound...", nullptr, 0,
			CONVERT_EACH_TO_ONE__KlattTable_to_Sound);
	praat_addAction1 (classKlattTable, 0, U"To KlattGrid...", nullptr, 0,
			CONVERT_EACH_TO_ONE__KlattTable_to_KlattGrid);
	praat_addAction1 (classKlattTable, 0, U"To Table", nullptr, 0,
			CONVERT_EACH_TO_ONE__KlattTable_to_Table);

	praat_addAction1 (classLegendreSeries, 0, U"LegendreSeries help", nullptr, 0,
			HELP__LegendreSeries_help);
	praat_FunctionSeries_init (classLegendreSeries);
	praat_addAction1 (classLegendreSeries, 0, U"To Polynomial", U"Analyse", 0,
			CONVERT_EACH_TO_ONE__LegendreSeries_to_Polynomial);
	
	praat_addAction1 (classLongSound, 0, U"Append to existing sound file...", nullptr, 0,
			APPEND_ALL__LongSounds_appendToExistingSoundFile);
	praat_addAction1 (classSound, 0, U"Append to existing sound file...", nullptr, 0,
			APPEND_ALL__LongSounds_appendToExistingSoundFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Append to existing sound file...", nullptr, 0,
			APPEND_ALL__LongSounds_appendToExistingSoundFile);

	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFF file...", U"Save as NIST file...", 1,
			SAVE_TWO__LongSounds_saveAsStereoAiffFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFF file...", U"Write to NIST file...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			SAVE_TWO__LongSounds_saveAsStereoAiffFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo AIFC file...", U"Save as stereo AIFF file...", 1,
			SAVE_TWO__LongSounds_saveAsStereoAifcFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo AIFC file...", U"Write to stereo AIFF file...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			SAVE_TWO__LongSounds_saveAsStereoAifcFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo WAV file...", U"Save as stereo AIFC file...", 1,
			SAVE_TWO__LongSounds_saveAsStereoWavFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo WAV file...", U"Write to stereo AIFC file...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			SAVE_TWO__LongSounds_saveAsStereoWavFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NeXT/Sun file...", U"Save as stereo WAV file...", 1,
			SAVE_TWO__LongSounds_saveAsStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NeXT/Sun file...", U"Write to stereo WAV file...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			SAVE_TWO__LongSounds_saveAsStereoNextSunFile);
	praat_addAction1 (classLongSound, 2, U"Save as stereo NIST file...", U"Save as stereo NeXT/Sun file...", 1,
			SAVE_TWO__LongSounds_saveAsStereoNistFile);
	praat_addAction1 (classLongSound, 2, U"Write to stereo NIST file...", U"Write to stereo NeXT/Sun file...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			SAVE_TWO__LongSounds_saveAsStereoNistFile);

	praat_addAction1 (classLtas, 0, U"Report spectral trend...", U"Get slope...", 1, INFO_ONE__Ltas_reportSpectralTrend);
	praat_addAction1 (classLtas, 0, U"Report spectral tilt...", U"Get slope...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_ONE__Ltas_reportSpectralTrend);

	praat_addAction1 (classMatrix, 0, U"Scatter plot...", U"Paint cells...", 1,
			GRAPHICS_EACH__Matrix_scatterPlot);
	praat_addAction1 (classMatrix, 0, U"Draw as squares...", U"Scatter plot...", 1,
			GRAPHICS_EACH__Matrix_drawAsSquares);
	praat_addAction1 (classMatrix, 0, U"Draw distribution...", U"Draw as squares...", 1,
			GRAPHICS_EACH__Matrix_drawDistribution);
	praat_addAction1 (classMatrix, 0, U"Draw cumulative distribution...", U"Draw distribution...", 1,
			GRAPHICS_EACH__Matrix_drawCumulativeDistribution);
	praat_addAction1 (classMatrix, 0, U"Get mean...", U"Get sum", 1,
			QUERY_ONE_FOR_REAL__Matrix_getMean);
	praat_addAction1 (classMatrix, 0, U"Get standard deviation...", U"Get mean...", 1, 
			QUERY_ONE_FOR_REAL__Matrix_getStandardDeviation);
	praat_addAction1 (classMatrix, 0, U"Mathematical", U"Get standard deviation...", 1, nullptr);
	praat_addAction1 (classMatrix, 0, U"Get norm...", U"Mathematical", 2,
			QUERY_ONE_FOR_REAL__Matrix_getNorm);

	praat_addAction1 (classMatrix, 0, U"Transpose", U"Synthesize", 0,
			CONVERT_EACH_TO_ONE__Matrix_transpose);
	praat_addAction1 (classMatrix, 0, U"Solve equation...", U"Analyse", 0,
			CONVERT_EACH_TO_ONE__Matrix_solveEquation);
	praat_addAction1 (classMatrix, 2, U"Solve matrix equation...", U"Solve equation...", 0,
			CONVERT_TWO_TO_ONE__Matrix_solveMatrixEquation);
	praat_addAction1 (classMatrix, 0, U"To PCA (by rows)", U"Solve matrix equation...", 0,
			CONVERT_EACH_TO_ONE__Matrix_to_PCA_byRows);
	praat_addAction1 (classMatrix, 0, U"To PCA (by columns)", U"To PCA (by rows)", 0,
			CONVERT_EACH_TO_ONE__Matrix_to_PCA_byColumns);
	praat_addAction1 (classMatrix, 0, U"To PatternList... || To Pattern...",
			U"To VocalTract", 1, CONVERT_EACH_TO_ONE__Matrix_to_PatternList);
	praat_addAction1 (classMatrix, 0, U"To ActivationList || To Activation",
			U"To PatternList...", 1, CONVERT_EACH_TO_ONE__Matrix_to_ActivationList);
	praat_addAction1 (classMatrix, 0, U"To Eigen", U"Eigen", GuiMenu_DEPRECATED_2026,
			CONVERT_EACH_TO_ONE__Matrix_to_Eigen);
	praat_addAction1 (classMatrix, 0, U"To Eigen (special)...", U"Eigen", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Matrix_to_Eigen_special);
	praat_addAction1 (classMatrix, 0, U"To SVD", U"To Eigen", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Matrix_to_SVD);
	praat_addAction1 (classMatrix, 0, U"To NMF (m.u.)...", U"To SVD", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Matrix_to_NMF_mu);
	praat_addAction1 (classMatrix, 0, U"To NMF (ALS)...", U"To SVD", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Matrix_to_NMF_als);
	praat_addAction1 (classMatrix, 0, U"To NMF (IS)...", U"To SVD", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Matrix_to_NMF_is);
	praat_addAction1 (classMatrix, 0, U"Eigen (complex)", U"Eigen", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_MULTIPLE_Matrix_eigen_complex);
	praat_addAction1 (classMatrix, 2, U"To DTW...", U"To ParamCurve", 1,
			CONVERT_TWO_TO_ONE__Matrices_to_DTW);

	praat_addAction2 (classMatrix, 1, classCategories, 1, U"To TableOfReal", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Matrix_Categories_to_TableOfReal);

	praat_addAction1 (classMelSpectrogram, 0, U"MelSpectrogram help", nullptr, 0,
			HELP__MelSpectrogram_help);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw frequency scale...", nullptr, 1,
			GRAPHICS_EACH__MelSpectrogram_drawFrequencyScale);
	praat_addAction1 (classMelSpectrogram, 0, U"Paint image...", nullptr, 1,
			GRAPHICS_EACH__MelSpectrogram_paintImage);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw triangular filter functions...", nullptr, 1,
			GRAPHICS_EACH__MelSpectrogram_drawTriangularFilterFunctions);
	praat_addAction1 (classMelSpectrogram, 0, U"Draw spectrum at nearest time slice...", nullptr, 1,
			GRAPHICS_EACH__MelSpectrogram_drawSpectrumAtNearestTimeSlice);
	praat_addAction1 (classMelSpectrogram, 0, U"Query -", nullptr, 0, nullptr);
	praat_BandFilterSpectrogram_query_init (classMelSpectrogram);

	praat_addAction1 (classMelSpectrogram, 0, U"Equalize intensities...", nullptr, 0,
			MODIFY_EACH__BandFilterSpectrogram_equalizeIntensities);
	praat_addAction1 (classMelSpectrogram, 0, U"To MFCC...", nullptr, 0,
			CONVERT_EACH_TO_ONE__MelSpectrogram_to_MFCC);
	praat_addAction1 (classMelSpectrogram, 0, U"To Intensity", nullptr, 0,
			CONVERT_EACH_TO_ONE__BandFilterSpectrogram_to_Intensity);
	praat_addAction1 (classMelSpectrogram, 0, U"To Matrix...", nullptr, 0,
			CONVERT_EACH_TO_ONE__BandFilterSpectrogram_to_Matrix);
	praat_addAction1 (classMelSpectrogram, 2, U"Cross-correlate...", nullptr, 0,
			CONVERT_TWO_TO_ONE__BandFilterSpectrograms_crossCorrelate);
	praat_addAction1 (classMelSpectrogram, 2, U"Convolve...", nullptr, 0,
			CONVERT_TWO_TO_ONE__BandFilterSpectrograms_convolve);

	praat_addAction1 (classMelFilter, 0, U"MelFilter help", nullptr, 0,
			HELP__MelFilter_help);   //
	praat_FilterBank_all_init (classMelFilter); // deprecated 2014
	praat_addAction1 (classMelFilter, 0, U"Draw spectrum (slice)...", U"Draw filters...", GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__MelFilter_drawSpectrum);
	praat_addAction1 (classMelFilter, 0, U"Draw filter functions...", U"Draw filters...", GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__MelFilter_drawFilterFunctions);
	praat_addAction1 (classMelFilter, 0, U"Paint...", U"Draw filter functions...", GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2014,
			GRAPHICS_EACH__MelFilter_paint);
	praat_addAction1 (classMelFilter, 0, U"To MFCC...", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_EACH_TO_ONE__MelFilter_to_MFCC);
	praat_addAction1 (classMelFilter, 0, U"To MelSpectrogram", nullptr, 0, 
			CONVERT_EACH_TO_ONE__MelFilter_to_MelSpectrogram);

	praat_addAction1 (classMFCC, 0, U"MFCC help", nullptr, 0, HELP__MFCC_help);
	praat_CC_init (classMFCC);
	praat_addAction1 (classMFCC, 0, U"To MelFilter...", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_EACH_TO_ONE__MFCC_to_MelFilter);
	praat_addAction1 (classMFCC, 0, U"To MelSpectrogram...", nullptr, 0,
			CONVERT_EACH_TO_ONE__MFCC_to_MelSpectrogram);
	praat_addAction1 (classMFCC, 0, U"To TableOfReal...", nullptr, 0,
			CONVERT_EACH_TO_ONE__MFCC_to_TableOfReal);
	praat_addAction1 (classMFCC, 0, U"To Matrix (features)...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__MFCC_to_Matrix_features);
	praat_addAction1 (classMFCC, 0, U"To Sound", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__MFCC_to_Sound);
	praat_addAction1 (classMFCC, 2, U"Cross-correlate...", nullptr, 0,
			CONVERT_TWO_TO_ONE__MFCCs_crossCorrelate);
	praat_addAction1 (classMFCC, 2, U"Convolve...", nullptr, 0, 
			CONVERT_TWO_TO_ONE__MFCCs_convolve);

	praat_addAction1 (classMSpline, 0, U"MSpline help", nullptr, 0,
			HELP__MSpline_help);
	praat_Spline_init (classMSpline);

	praat_addAction1 (classNavigationContext, 0, U"Modify use criterion...", nullptr, 0,
			MODIFY_EACH__NavigationContext_modifyUseCriterion);
	praat_addAction1 (classNavigationContext, 0, U"Modify Topic match criterion...", nullptr, 0,
			MODIFY_EACH__NavigationContext_modifyTopicCriterion);
	praat_addAction1 (classNavigationContext, 0, U"Modify Before match criterion...", nullptr, 0,
			MODIFY_EACH__NavigationContext_modifyBeforeCriterion);
	praat_addAction1 (classNavigationContext, 0, U"Modify After match criterion...", nullptr, 0,
			MODIFY_EACH__NavigationContext_modifyAfterCriterion);
	praat_addAction2 (classNavigationContext, 1, classStrings, 1, U"Replace Topic labels", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__NavigationContext_replaceTopicLabels);
	praat_addAction2 (classNavigationContext, 1, classStrings, 1, U"Replace Before labels", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__NavigationContext_replaceBeforeLabels);
	praat_addAction2 (classNavigationContext, 1, classStrings, 1, U"Replace After labels", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__NavigationContext_replaceAfterLabels);

	praat_addAction1 (classNMF, 0, U"NMF help", nullptr, 0,
			HELP__NMF_help);
	praat_addAction1 (classNMF, 0, U"Paint features...", nullptr, 0,
			GRAPHICS_EACH__NMF_paintFeatures);
	praat_addAction1 (classNMF, 0, U"Paint weights...", nullptr, 0,
			GRAPHICS_EACH__NMF_paintWeights);
	praat_addAction1 (classNMF, 0, U"To Matrix", nullptr, 0,
			CONVERT_EACH_TO_ONE__NMF_to_Matrix);

	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Get Euclidean distance", nullptr, 0,
			QUERY_ONE_AND_ONE_FOR_REAL__NMF_Matrix_getEuclideanDistance);
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Get Itakura-Saito distance", nullptr, 0,
			QUERY_ONE_AND_ONE_FOR_REAL__NMF_Matrix_getItakuraSaitoDivergence);
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Improve factorization (ALS)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__NMF_Matrix_improveFactorization_als);
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Improve factorization (m.u.)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__NMF_Matrix_improveFactorization_mu);
	praat_addAction2 (classNMF, 1, classMatrix, 1, U"Improve factorization (IS)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__NMF_Matrix_improveFactorization_is);

	praat_addAction1 (classPatternList, 0, U"Draw", nullptr, 0, 0);
	praat_addAction1 (classPatternList, 0, U"Draw...", nullptr, 0,
			GRAPHICS_EACH__PatternList_draw);
	praat_PatternList_query_init (classPatternList);
	praat_addAction1 (classPatternList, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classPatternList, 0, U"Formula...", nullptr, 1,
			MODIFY_PatternList_formula);
	praat_addAction1 (classPatternList, 0, U"Set value...", nullptr, 1,
			MODIFY_PatternList_setValue);
	praat_addAction1 (classPatternList, 0, U"Get all values", nullptr, 0,
			NUMMAT_PatternList_getAllValues);
	praat_addAction1 (classPatternList, 0, U"To Matrix", nullptr, 0,
			CONVERT_EACH_TO_ONE__PatternList_to_Matrix);

	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To TableOfReal", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Matrix_Categories_to_TableOfReal);
	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To Discriminant", nullptr, 0,
			CONVERT_ONE_AND_ONE__TO_ONE_PatternList_Categories_to_Discriminant);

	praat_addAction1 (classPCA, 0, U"PCA help", nullptr, 0, 
			HELP__PCA_help);
	praat_addAction1 (classPCA, 0, U"Draw -", nullptr, 0, nullptr);
	praat_Eigen_draw_init (classPCA);
	praat_addAction1 (classPCA, 0, U"Query -", nullptr, 0, nullptr);
	praat_Eigen_query_init (classPCA);
	praat_addAction1 (classPCA, 1, U"-- pca --", nullptr, 1, 0);
	praat_addAction1 (classPCA, 1, U"Get centroid element...", nullptr, 1,
			QUERY_ONE_FOR_REAL__PCA_getCentroidElement);
	praat_addAction1 (classPCA, 1, U"Get equality of eigenvalues...", nullptr, 1,
			QUERY_ONE_FOR_REAL__PCA_getEqualityOfEigenvalues);
	praat_addAction1 (classPCA, 1, U"Get fraction variance accounted for...", nullptr, 1,
			QUERY_ONE_FOR_REAL__PCA_getFractionVAF);
	praat_addAction1 (classPCA, 1, U"Get number of components (VAF)...", nullptr, 1, 
			QUERY_ONE_FOR_INTEGER__PCA_getNumberOfComponents_VAF);
	praat_addAction1 (classPCA, 2, U"Get angle between pc1-pc2 planes", nullptr, 1,
			QUERY_TWO_FOR_REAL__PCAs_getAngleBetweenPc1Pc2Plane_degrees);
	praat_addAction1 (classPCA, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classPCA, 1, U"Invert eigenvector...", nullptr, 1,
			MODIFY_PCA_invertEigenvector);
	praat_addAction1 (classPCA, 0, U"Align eigenvectors", nullptr, 1,
			MODIFY_ALL__Eigens_alignEigenvectors);
	praat_addAction1 (classPCA, 0, U"Extract -", nullptr, 0, 0);
		praat_addAction1 (classPCA, 0, U"Extract eigenvector...", nullptr, 1,
				CONVERT_EACH_TO_ONE__PCA_extractEigenvector);
		praat_addAction1 (classPCA, 0, U"Extract Eigen", nullptr, 1,
				CONVERT_EACH_TO_ONE__PCA_extractEigen);
	praat_addAction1 (classPCA, 2, U"To Procrustes...", nullptr, 0,
			CONVERT_TWO_TO_ONE__PCAs_to_Procrustes);
	praat_addAction1 (classPCA, 0, U"To TableOfReal (reconstruct 1)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__PCA_to_TableOfReal_reconstruct1);

	praat_addAction1 (classPCA, 0, U"& TableOfReal: To Configuration?",
			nullptr, GuiMenu_NO_API, HINT__hint_PCA_TableOfReal_to_Configuration);
	praat_addAction1 (classPCA, 0, U"& Configuration (reconstruct)?",
			nullptr, GuiMenu_NO_API, HINT_hint__PCA_Configuration_to_TableOfReal_reconstruct);
	praat_addAction1 (classPCA, 0, U"& Covariance: Project?",
			nullptr, GuiMenu_NO_API, HINT_hint__PCA_Covariance_Project);
	praat_addAction2 (classPCA, 1, classConfiguration, 1, U"To TableOfReal (reconstruct)",
			nullptr, 0, CONVERT_ONE_AND_ONE_TO_ONE__PCA_Configuration_to_TableOfReal_reconstruct);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project rows)...",
			nullptr, 0, CONVERT_ONE_AND_ONE_TO_ONE__PCA_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classMatrix, 1, U"To Matrix (project columns)... || To Matrix (pc)...",
			nullptr, 0, CONVERT_ONE_AND_ONE_GENERIC_TO_ONE__Matrix_to_Matrix_projectColumns);
	praat_addAction2 (classPCA, 1, classPatternList, 1, U"To Matrix (project rows)...",
			nullptr, 0, CONVERT_ONE_AND_ONE_TO_ONE__PCA_Matrix_to_Matrix_projectRows);
	praat_addAction2 (classPCA, 1, classSSCP, 1, U"Project",
			nullptr, 0, CONVERT_ONE_AND_ONE_TO_ONE__PCA_SSCP_project);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (project rows)... || To TableOfReal...",
			nullptr, 0, CONVERT_ONE_AND_ONE_TO_ONE__PCA_TableOfReal_to_TableOfReal_projectRows);   // alternative GuiMenu_DEPRECATED_2016
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To Configuration...",
			nullptr, 0, CONVERT_ONE_AND_ONE_GENERIC_TO_ONE__PCA_TableOfReal_to_Configuration);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"To TableOfReal (z-scores)...",
			nullptr, 0, CONVERT_ONE_AND_ONE_GENERIC_TO_ONE__PCA_TableOfReal_to_TableOfReal_zscores);
	praat_addAction2 (classPCA, 1, classTableOfReal, 1, U"Get fraction variance...",
			nullptr, 0, QUERY_ONE_AND_ONE_FOR_REAL__PCA_TableOfReal_getFractionVariance);
	praat_addAction2 (classPCA, 1, classCovariance, 1, U"Project",
			nullptr, 0, CONVERT_ONE_AND_ONE_TO_ONE__Eigen_Covariance_project);

	praat_Eigen_Spectrogram_project (classPCA, classSpectrogram);
	praat_Eigen_Spectrogram_project (classPCA, classBarkSpectrogram);
	praat_Eigen_Spectrogram_project (classPCA, classMelSpectrogram);

	praat_Eigen_Matrix_project (classPCA, classFormantFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classPCA, classBarkFilter); // deprecated 2014
	praat_Eigen_Matrix_project (classPCA, classMelFilter); // deprecated 2014

	praat_addAction1 (classPermutation, 0, U"Permutation help",
			nullptr, 0, HELP__Permutation_help);
	praat_addAction1 (classPermutation, 0, U"Draw as line...",
					  nullptr, 0, GRAPHICS_EACH__Permutation_drawAsLine);
	praat_addAction1 (classPermutation, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classPermutation, 1, U"Get number of elements",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Permutation_getNumberOfElements);
		praat_addAction1 (classPermutation, 1, U"Get value...",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Permutation_getValueAtIndex);
		praat_addAction1 (classPermutation, 1, U"List values",
				nullptr, 1, QUERY_ONE_FOR_REAL_VECTOR__Permutation_listValues);
		praat_addAction1 (classPermutation, 1, U"Get index...",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Permutation_getIndexAtValue);
		praat_addAction1 (classPermutation, 1, U"Get number of inversions",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Permutation_getNumberOfInversions);
		praat_addAction1 (classPermutation, 1, U"List random inversions...",
						  nullptr, 1, QUERY_ONE_FOR_MATRIX__Permutation_listRandomInversions);
		praat_addAction1 (classPermutation, 1, U"List all inversions",
						  nullptr, 1, QUERY_ONE_FOR_MATRIX__Permutation_listAllInversions);
		praat_addAction1 (classPermutation, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classPermutation, 0, U"Permute randomly (in-place)...",
				nullptr, 1, MODIFY__Permutation_permuteRandomlyInplace);
		praat_addAction1 (classPermutation, 1, U"Sort",
				nullptr, 1, MODIFY_Permutation_sort);
		praat_addAction1 (classPermutation, 1, U"Swap blocks...",
				nullptr, 1, MODIFY_Permutation_swapBlocks);
		praat_addAction1 (classPermutation, 1, U"Swap numbers...",
				nullptr, 1, MODIFY_Permutation_swapNumbers);
		praat_addAction1 (classPermutation, 1, U"Swap positions...",
				nullptr, 1, MODIFY_Permutation_swapPositions);
		praat_addAction1 (classPermutation, 1, U"Swap one from range...",
				nullptr, 1, MODIFY_Permutation_swapOneFromRange);
		praat_addAction1 (classPermutation, 1, U"Jump...",
				nullptr, 1, MODIFY_Permutation_tableJump);
		praat_addAction1 (classPermutation, 0, U"-- sequential permutations --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPermutation, 0, U"Next",
				nullptr, 1, MODIFY_Permutations_next);
		praat_addAction1 (classPermutation, 0, U"Previous",
				nullptr, 1, MODIFY_Permutations_previous);
	
	praat_addAction1 (classPermutation, 1, U"Permute randomly...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Permutation_permuteRandomly);
	praat_addAction1 (classPermutation, 1, U"Permute randomly (blocks)...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Permutation_permuteBlocksRandomly);
	praat_addAction1 (classPermutation, 1, U"Interleave...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Permutation_interleave);
	praat_addAction1 (classPermutation, 1, U"Rotate...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Permutation_rotate);
	praat_addAction1 (classPermutation, 1, U"Reverse...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Permutation_reverse);
	praat_addAction1 (classPermutation, 1, U"Invert",
			nullptr, 0, CONVERT_EACH_TO_ONE__Permutation_invert);
	praat_addAction1 (classPermutation, 0, U"Multiply",
			nullptr, 0, COMBINE_ALL_TO_ONE__Permutations_multiply);
	praat_addAction1 (classPermutation, 2, U"Permute part...",
			nullptr, 0, CONVERT_TWO_TO_ONE__Permutation_permutePartByOther);

	praat_addAction1 (classPitch, 2, U"To DTW...", U"To PointProcess",
			GuiMenu_HIDDEN, CONVERT_TWO_TO_ONE__Pitches_to_DTW);

	praat_addAction1 (classPitchTier, 0, U"To Pitch...",
			U"To Sound (sine)...", 1, CONVERT_EACH_TO_ONE__PitchTier_to_Pitch);
	praat_addAction1 (classPitchTier, 0, U"Modify interval...",
			U"Add point...", 1, MODIFY_PitchTier_modifyInterval);
	praat_addAction1 (classPitchTier, 0, U"Modify interval (tone levels)...",
			U"Modify interval...", 1, MODIFY_PitchTier_modifyInterval_toneLevels);
	praat_addAction1 (classPolygon, 0, U"Query -", U"Paint circles...", 0, nullptr);
	praat_addAction1 (classPolygon, 0, U"Get number of points",
			U"Query -", 1, QUERY_ONE_FOR_INTEGER__Polygon_getNumberOfPoints);
	praat_addAction1 (classPolygon, 0, U"Get point (x)...",
			U"Get number of points", 1, QUERY_ONE_FOR_REAL__Polygon_getPointX);
	praat_addAction1 (classPolygon, 0, U"Get point (y)...",  U"Get point (x)...", 1, 
			QUERY_ONE_FOR_REAL__Polygon_getPointY);
	praat_addAction1 (classPolygon, 0, U"-- other queries --",  U"Get point (y)...", 1, 0);
	praat_addAction1 (classPolygon, 0, U"Get location of point...", U"-- other queries --", 1, 
			QUERY_ONE_FOR_STRING__Polygon_getLocationOfPoint);
	praat_addAction1 (classPolygon, 0, U"Get area of convex hull...", U"Get location of point...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL__Polygon_getAreaOfConvexHull);

	praat_addAction1 (classPolygon, 0, U"Translate...", U"Modify -", 1,
			MODIFY_EACH__Polygon_translate);
	praat_addAction1 (classPolygon, 0, U"Rotate...", U"Translate...", 1,
			MODIFY_EACH__Polygon_rotate);
	praat_addAction1 (classPolygon, 0, U"Scale...", U"Rotate...", 1,
			MODIFY_EACH__Polygon_scale);
	praat_addAction1 (classPolygon, 0, U"Reverse X", U"Scale...", 1,
			MODIFY_EACH__Polygon_reverseX);
	praat_addAction1 (classPolygon, 0, U"Reverse Y", U"Reverse X", 1,
			MODIFY_EACH__Polygon_reverseY);
	praat_addAction1 (classPolygon, 0, U"Simplify", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Polygon_simplify);
	praat_addAction1 (classPolygon, 0, U"Convex hull", nullptr, 0,
			CONVERT_EACH_TO_ONE__Polygon_convexHull);
	praat_addAction1 (classPolygon, 0, U"Circular permutation...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Polygon_circularPermutation);

	praat_addAction2 (classPolygon, 1, classCategories, 1, U"Draw...", nullptr, 0,
			GRAPHICS_ONE_AND_ONE__Polygon_Categories_draw);

	praat_addAction1 (classPolynomial, 0, U"Polynomial help", nullptr, 0,
			HELP__Polynomial_help);
	praat_FunctionSeries_init (classPolynomial);
	praat_addAction1 (classPolynomial, 0, U"-- area --", U"Get x of maximum...", 1, 0);
	praat_addAction1 (classPolynomial, 1, U"Get area...", U"-- area --", 1,
			QUERY_ONE_FOR_REAL__Polynomial_getArea);
	praat_addAction1 (classPolynomial, 1, U"Get remainder after division... || Get remainder...", U"Get area...", 1,
			QUERY_ONE_FOR_REAL__Polynomial_getRemainderAfterDivision);   // alternative COMPATIBILITY <= 2016
	praat_addAction1 (classPolynomial, 0, U"-- monic --", U"Set coefficient...", 1, 0);
	praat_addAction1 (classPolynomial, 0, U"Scale coefficients (monic)", U"-- monic --", 1, 
			MODIFY_Polynomial_scaleCoefficients_monic);
	praat_addAction1 (classPolynomial, 1, U"Divide (second order factor)...", U"Scale coefficients (monic)", 1,
			MODIFY_EACH__Polynomial_divide_secondOrderFactor);

	praat_addAction1 (classPolynomial, 1, U"Get value (complex)...", U"Get value...", 1,
			INFO_ONE__Polynomial_evaluate_z);
	praat_addAction1 (classPolynomial, 1, U"Get derivatives at X...", U"Get value (complex)...", 1,
			INFO_ONE__Polynomial_getDerivativesAtX);
	praat_addAction1 (classPolynomial, 1, U"Get one real root...", U"Get derivatives at X...", 1,
			QUERY_ONE_FOR_REAL__Polynomial_getOneRealRoot);
	praat_addAction1 (classPolynomial, 0, U"To Spectrum...", U"Analyse", 0,
			CONVERT_EACH_TO_ONE__Polynomial_to_Spectrum);
	praat_addAction1 (classPolynomial, 0, U"To Roots", nullptr, 0,
			CONVERT_EACH_TO_ONE__Polynomial_to_Roots);
	praat_addAction1 (classPolynomial, 0, U"To Polynomial (derivative)", nullptr, 0,
			CONVERT_EACH_TO_ONE__Polynomial_getDerivative);
	praat_addAction1 (classPolynomial, 0, U"To Polynomial (primitive)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Polynomial_getPrimitive);
	praat_addAction1 (classPolynomial, 0, U"Scale x...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Polynomial_scaleX);
	praat_addAction1 (classPolynomial, 2, U"Multiply", nullptr, 0,
			CONVERT_EACH_TO_ONE__Polynomials_multiply);
	praat_addAction1 (classPolynomial, 2, U"Divide...", nullptr, 0,
			CONVERT_TWO_TO_MULTIPLE__Polynomials_divide);

	praat_addAction1 (classRoots, 1, U"Roots help", nullptr, 0,
			HELP__Roots_help);
	praat_addAction1 (classRoots, 1, U"Draw...", nullptr, 0,
			GRAPHICS_EACH__Roots_draw);
	praat_addAction1 (classRoots, 1, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classRoots, 1, U"Get number of roots", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__Roots_getNumberOfRoots);
	praat_addAction1 (classRoots, 1, U"-- roots --", nullptr, 1, 0);
	praat_addAction1 (classRoots, 1, U"Get root...", nullptr, 1,
			COMPLEX_Roots_getRoot);
	praat_addAction1 (classRoots, 1, U"Get real part of root...", nullptr, 1,
			QUERY_ONE_FOR_REAL__Roots_getRealPartOfRoot);
	praat_addAction1 (classRoots, 1, U"Get imaginary part of root...", nullptr, 1,
			QUERY_ONE_FOR_REAL__Roots_getImaginaryPartOfRoot);
	praat_addAction1 (classRoots, 1, U"Modify -", nullptr, 0, 0);
	praat_addAction1 (classRoots, 1, U"Set root...", nullptr, 1,
			MODIFY_EACH__Roots_setRoot);
	praat_addAction1 (classRoots, 0, U"Analyse", nullptr, 0, 0);
	praat_addAction1 (classRoots, 0, U"To Spectrum...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Roots_to_Spectrum);

	praat_addAction2 (classRoots, 1, classPolynomial, 1, U"Polish roots", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Roots_Polynomial_polish);

	praat_addAction1 (classSound, 0, U"To TextGrid (silences)...", U"To IntervalTier", 1,
			CONVERT_EACH_TO_ONE__Sound_to_TextGrid_detectSilences);
	praat_addAction1 (classSound, 0, U"To TextGrid (speech activity, LTSF)... || To TextGrid (speech activity)... || To TextGrid (voice activity)...", U"To IntervalTier", 1,
			CONVERT_EACH_TO_ONE__Sound_to_TextGrid_speechActivity);
	praat_addAction1 (classSound, 0, U"To TextGrid (high, mid, low)...", U"To IntervalTier", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_TextGrid_highMidLowIntervals);
    praat_addAction1 (classSound, 0, U"Play one channel...", U"Play", GuiMenu_HIDDEN,
			PLAY_EACH__Sound_playOneChannel);
    praat_addAction1 (classSound, 0, U"Play as frequency shifted...", U"Play", GuiMenu_HIDDEN,
			PLAY_EACH__Sound_playAsFrequencyShifted);
	praat_addAction1 (classSound, 0, U"Draw where...", U"Draw...", 1,
			GRAPHICS_EACH__Sound_drawWhere);
	praat_addAction1 (classSound, 0, U"Paint where...", U"Draw where...", 1,
			GRAPHICS_EACH__Sound_paintWhere);
	praat_addAction1 (classSound, 2, U"Paint enclosed...", U"Paint where...", 1,
			GRAPHICS_TWO__Sounds_paintEnclosed);

	praat_addAction1 (classSound, 1, U"Get nearest level crossing...", U"Get nearest zero crossing...", 1,
			QUERY_ONE_FOR_REAL__Sound_getNearestLevelCrossing);

	praat_addAction1 (classSound, 0, U"To Pitch (shs)...", U"To Pitch (filtered cc)...", 1,
			CONVERT_EACH_TO_ONE__Sound_to_Pitch_shs);
	praat_addAction1 (classSound, 0, U"Fade in...", U"Multiply by window...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			MODIFY_EACH__Sound_fadeIn);
	praat_addAction1 (classSound, 0, U"Fade out...", U"Fade in...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			MODIFY_Sound_fadeOut);
	praat_addAction1 (classSound, 0, U"To Pitch (SPINET)...", U"To Pitch (shs)...", 1,
			CONVERT_EACH_TO_ONE__Sound_to_Pitch_SPINET);

	praat_addAction1 (classSound, 1, U"Save as highest quality MP3 file...", U"Save as 32-bit WAV file...", 1,
			SAVE__Sound_saveAsHighestQualityMP3File);
	praat_addAction1 (classSound, 0, U"To Spectrum (resampled)...", U"To Spectrum...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_Spectrum_resampled);
	praat_addAction1 (classSound, 0, U"To FormantFilter...", U"To Cochleagram (edb)...", GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_FormantFilter);
	praat_addAction1 (classSound, 0, U"To Spectrogram (pitch-dependent)...", U"To Cochleagram (edb)...", 1,
			CONVERT_EACH_TO_ONE__Sound_to_Spectrogram_pitchDependent);

	praat_addAction1 (classSound, 0, U"To BarkFilter...", U"To FormantFilter...", GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_BarkFilter);
	praat_addAction1 (classSound, 0, U"To BarkSpectrogram...", U"To FormantFilter...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_BarkSpectrogram);

	praat_addAction1 (classSound, 0, U"To MelFilter...", U"To BarkFilter...", GuiMenu_DEPRECATED_2014 | GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_MelFilter);
	praat_addAction1 (classSound, 0, U"To MelSpectrogram...", U"To BarkSpectrogram...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_MelSpectrogram);
	praat_addAction1 (classSound, 0, U"To ComplexSpectrogram...", U"To MelSpectrogram...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_ComplexSpectrogram);

	praat_addAction1 (classSound, 0, U"To Polygon...", U"Down to Matrix", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_Polygon);
    praat_addAction1 (classSound, 2, U"To Polygon (enclosed)...", U"Cross-correlate...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_TWO_TO_ONE__Sounds_to_Polygon_enclosed);
    praat_addAction1 (classSound, 2, U"To DTW...", U"Cross-correlate...", GuiMenu_DEPTH_1,
			CONVERT_TWO_TO_ONE__Sounds_to_DTW);

	praat_addAction1 (classSound, 1, U"Filter (gammatone)...", U"Filter (de-emphasis)...", 1,
			CONVERT_EACH_TO_ONE__Sound_filterByGammaToneFilter4);
	praat_addAction1 (classSound, 0, U"Remove noise...", U"Filter (formula)...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_removeNoise);
	praat_addAction1 (classSound, 0, U"Reduce noise...", U"Filter (formula)...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_reduceNoise);
	praat_addAction1 (classSound, 0, U"To Sound (derivative)...", U"Filter (formula)...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Sound_to_Sound_derivative);

	praat_addAction1 (classSound, 0, U"Change gender...", U"Deepen band modulation...", 1,
			CONVERT_EACH_TO_ONE__Sound_changeGender);

	praat_addAction1 (classSound, 0, U"Change speaker...", U"Deepen band modulation...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_changeSpeaker);
	praat_addAction1 (classSound, 0, U"Copy channel ranges...", U"Extract all channels", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_copyChannelRanges);
	praat_addAction1 (classSound, 0 /* TODO produces 2N objects */, U"Trim silences...", U"Resample...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_trimSilences);
	praat_addAction1 (classSound, 0, U"To KlattGrid (simple)...", U"To Manipulation...", 1,
			CONVERT_EACH_TO_ONE__Sound_to_KlattGrid_simple);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To FormantFilter...", nullptr, GuiMenu_DEPRECATED_2014,
			CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_to_FormantFilter);
	praat_addAction2 (classSound, 1, classPitch, 1, U"To Spectrogram (pitch-dependent)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_to_Spectrogram);

	praat_addAction2 (classSound, 1, classPitch, 1, U"Change gender...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_changeGender);
	praat_addAction2 (classSound, 1, classPitch, 1, U"Change speaker...", nullptr, GuiMenu_HIDDEN,
			CONVERT_ONE_AND_ONE_TO_ONE__Sound_Pitch_changeSpeaker);
	praat_addAction2 (classSound, 1, classIntervalTier, 1, U"Cut parts matching label...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Sound_IntervalTier_cutPartsMatchingLabel);

	praat_addAction1 (classSpectrogram, 2, U"To DTW...", U"To Spectrum (slice)...", 1,
			CONVERT_TWO_TO_ONE__Spectrograms_to_DTW);
	praat_addAction1 (classSpectrogram, 0, U"Draw long-term spectral flatness...", U"Paint...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			GRAPHICS_EACH__Spectrogram_drawLongtermSpectralFlatness);
	praat_addAction1 (classSpectrogram, 0, U"Get long-term spectral flatness...", U"To DTW...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Spectrogram_getLongtermSpectralFlatness);

	praat_addAction1 (classSpectrum, 0, U"To Sound (resampled)...", U"To Sound", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Spectrum_to_Sound_resampled);
	praat_addAction1 (classSpectrum, 0, U"Draw phases...", U"Draw (log freq)...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			GRAPHICS_EACH__Spectrum_drawPhases);
	praat_addAction1 (classSpectrum, 0, U"Set real value in bin...", U"Formula...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			MODIFY_EACH__Spectrum_setRealValueInBin);
	praat_addAction1 (classSpectrum, 0, U"Set imaginary value in bin...", U"Formula...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			MODIFY_EACH__Spectrum_setImaginaryValueInBin);
	praat_addAction1 (classSpectrum, 0, U"Conjugate", U"Formula...", GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			MODIFY_EACH__Spectrum_conjugate);
	praat_addAction1 (classSpectrum, 2, U"Multiply", U"To Sound (fft)", GuiMenu_HIDDEN,
			CONVERT_TWO_TO_ONE__Spectra_multiply);
	praat_addAction1 (classSpectrum, 0, U"To Matrix (unwrap)", U"To Matrix", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Spectrum_unwrap);
	praat_addAction1 (classSpectrum, 0, U"Shift frequencies...", U"To Matrix", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Spectrum_shiftFrequencies);
	praat_addAction1 (classSpectrum, 0, U"Compress frequency domain...", U"Shift frequencies...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Spectrum_compressFrequencyDomain);
	praat_addAction1 (classSpectrum, 0, U"Resample...", U"Compress frequency domain...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Spectrum_resample);
	praat_addAction1 (classSpectrum, 0, U"To Cepstrum", U"To Spectrogram", 1,
			CONVERT_EACH_TO_ONE__Spectrum_to_Cepstrum);
	praat_addAction1 (classSpectrum, 0, U"To PowerCepstrum", U"To Cepstrum", 1,
			CONVERT_EACH_TO_ONE__Spectrum_to_PowerCepstrum);

	praat_addAction1 (classSpeechSynthesizer, 0, U"SpeechSynthesizer help", nullptr, 0,
			HELP__SpeechSynthesizer_help);
	praat_addAction1 (classSpeechSynthesizer, 0, U"Play text...", nullptr, 0,
			PLAY_EACH__SpeechSynthesizer_playText);
	praat_addAction1 (classSpeechSynthesizer, 0, U"To Sound...", nullptr, 0,
			CONVERT_EACH_TO_ONE__SpeechSynthesizer_to_Sound);
	praat_addAction1 (classSpeechSynthesizer, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get language name", nullptr, 1,
				QUERY_ONE_FOR_STRING__SpeechSynthesizer_getLanguageName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice name", nullptr, 1,
				QUERY_ONE_FOR_STRING__SpeechSynthesizer_getVoiceName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get phoneme set name", nullptr, 1,
				QUERY_ONE_FOR_STRING__SpeechSynthesizer_getPhonemeSetName);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get phonemes from text...", nullptr, 1,
				QUERY_ONE_FOR_AUTOSTRING__SpeechSynthesizer_getPhonemesFromText);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get phonemes from text (space-separated)...", nullptr, 1,
				QUERY_ONE_FOR_AUTOSTRING__SpeechSynthesizer_getPhonemesFromTextSpaceSeparated);
		praat_addAction1 (classSpeechSynthesizer, 1, U"Get voice variant", nullptr, GuiMenu_DEPRECATED_2017,
				QUERY_ONE_FOR_STRING__SpeechSynthesizer_getVoiceName);

	praat_addAction1 (classSpeechSynthesizer, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classSpeechSynthesizer, 1, U"View & Edit...", nullptr, 1,
				WINDOW_SpeechSynthesizer_viewAndEdit);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set text input settings...", nullptr, 1,
				MODIFY_EACH__SpeechSynthesizer_setTextInputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Speech output settings...", nullptr, 1,
				MODIFY_EACH__SpeechSynthesizer_speechOutputSettings);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Estimate speech rate from speech...", nullptr, 1,
				MODIFY_EACH__SpeechSynthesizer_estimateSpeechRateFromSpeech);
		praat_addAction1 (classSpeechSynthesizer, 0, U"Set speech output settings...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2017,
				MODIFY_EACH__SpeechSynthesizer_setSpeechOutputSettings);

	praat_addAction2 (classSpeechSynthesizer, 1, classTextGrid, 1, U"To Sound...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__SpeechSynthesizer_TextGrid_to_Sound);
	praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE__SpeechSynthesizer_Sound_TextGrid_align);
    praat_addAction3 (classSpeechSynthesizer, 1, classSound, 1, classTextGrid, 1, U"To TextGrid (align,trim)...", nullptr, 0,
			CONVERT_ONE_AND_ONE_AND_ONE_TO_ONE__SpeechSynthesizer_Sound_TextGrid_align2);

	praat_addAction1 (classSSCP, 0, U"SSCP help", nullptr, 0,
			HELP__SSCP_help);
	praat_TableOfReal_init2 (classSSCP);
	praat_removeAction (classSSCP, nullptr, nullptr, U"Append");
	praat_addAction1 (classSSCP, 0, U"Draw sigma ellipse...", U"Draw -", 1,
			GRAPHICS_EACH__SSCP_drawSigmaEllipse);
	praat_addAction1 (classSSCP, 0, U"Draw confidence ellipse...", U"Draw -", 1,
			GRAPHICS_EACH__SSCP_drawConfidenceEllipse);
	praat_SSCP_query_init (classSSCP);
	praat_addAction1 (classSSCP, 1, U"Get diagonality (bartlett)...", U"Get ln(determinant)", 1,
			QUERY_ONE_FOR_REAL__SSCP_getDiagonality_bartlett);
	praat_addAction1 (classSSCP, 1, U"Get total variance", U"Get diagonality (bartlett)...", 1,
			QUERY_ONE_FOR_REAL__SSCP_getTotalVariance);
	praat_addAction1 (classSSCP, 1, U"Get sigma ellipse area...", U"Get total variance", 1,
			QUERY_ONE_FOR_REAL__SSCP_getConcentrationEllipseArea);
	praat_addAction1 (classSSCP, 1, U"Get confidence ellipse area...", U"Get sigma ellipse area...", 1,
			QUERY_ONE_FOR_REAL__SSCP_getConfidenceEllipseArea);
	praat_addAction1 (classSSCP, 1, U"Get fraction variation...", U"Get confidence ellipse area...", 1,
			QUERY_ONE_FOR_REAL__SSCP_getFractionVariation);
	praat_SSCP_extract_init (classSSCP);
	praat_addAction1 (classSSCP, 0, U"To PCA", nullptr, 0,
			CONVERT_EACH_TO_ONE__SSCP_to_PCA);
	praat_addAction1 (classSSCP, 0, U"To Correlation", nullptr, 0,
			CONVERT_EACH_TO_ONE__SSCP_to_Correlation);
	praat_addAction1 (classSSCP, 0, U"To Covariance...", nullptr, 0,
			CONVERT_EACH_TO_ONE__SSCP_to_Covariance);

	praat_addAction1 (classStrings, 0, U"To Categories", nullptr, 0,
			CONVERT_EACH_TO_ONE__Strings_to_Categories);
	praat_addAction1 (classStrings, 0, U"Append", nullptr, 0,
			COMBINE_ALL_TO_ONE__Strings_append);
	praat_addAction1 (classStrings, 0, U"Change...", U"Replace all...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Strings_change);
	praat_addAction1 (classStrings, 0, U"Extract part...", U"Replace all...", 0,
			CONVERT_EACH_TO_ONE__Strings_extractPart);
	praat_addAction1 (classStrings, 0, U"To Permutation...", U"To Distributions", 0,
			CONVERT_EACH_TO_ONE__Strings_to_Permutation);
	praat_addAction1 (classStrings, 2, U"To EditDistanceTable", U"To Distributions", 0,
			CONVERT_TWO_TO_ONE__Strings_to_EditDistanceTable);
	praat_addAction1 (classStrings, 0, U"To StringsIndex...", U"To Permutation...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Strings_to_StringsIndex);
	praat_addAction1 (classSVD, 0, U"SVD help", nullptr, 0,
			HELP__SVD_help);
	praat_addAction1 (classSVD, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classSVD, 1, U"Get number of rows", nullptr, 1, 
			QUERY_ONE_FOR_INTEGER__SVD_getNumberOfRows);
	praat_addAction1 (classSVD, 1, U"Get number of columns", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__SVD_getNumberOfColumns);
	praat_addAction1 (classSVD, 1, U"Get rank", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getRank);
	praat_addAction1 (classSVD, 1, U"Get condition number", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getConditionNumber);
	praat_addAction1 (classSVD, 0, U"-- singular values ---", nullptr, 1, nullptr);
	praat_addAction1 (classSVD, 1, U"Get singular value...", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getSingularValue);
	praat_addAction1 (classSVD, 1, U"Get sum of singular values...", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getSumOfSingularValues);
	praat_addAction1 (classSVD, 1, U"Get sum of singular values (fraction)...", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getSumOfSingularValuesAsFractionOfTotal);
	praat_addAction1 (classSVD, 1, U"Get minimum number of singular values...", nullptr, 1,
			QUERY_ONE_FOR_INTEGER__SVD_getMinimumNumberOfSingularValues);
	praat_addAction1 (classSVD, 1, U"Get shrinkage parameter...", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getShrinkageParameter);	
	praat_addAction1 (classSVD, 1, U"Get effective degrees of freedom...", nullptr, 1,
			QUERY_ONE_FOR_REAL__SVD_getEffectiveDegreesOfFreedom);

	praat_addAction1 (classSVD, 0, U"To TableOfReal...", nullptr, 0,
			CONVERT_EACH_TO_ONE__SVD_to_TableOfReal);
	praat_addAction1 (classSVD, 0, U"To Matrix...", nullptr, 0,
			CONVERT_EACH_TO_ONE__SVD_to_Matrix);
	praat_addAction1 (classSVD, 0, U"Extract left singular vectors", nullptr, 0,
			CONVERT_EACH_TO_ONE__SVD_extractLeftSingularVectors);
	praat_addAction1 (classSVD, 0, U"Extract right singular vectors", nullptr, 0,
			CONVERT_EACH_TO_ONE__SVD_extractRightSingularVectors);
	praat_addAction1 (classSVD, 0, U"Extract singular values", nullptr, 0,
			CONVERT_EACH_TO_ONE__SVD_extractSingularValues);
		praat_addAction1 (classTable, 0, U"Draw ellipses...", U"Draw ellipse (standard deviation)...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_drawEllipses);
		praat_addAction1 (classTable, 0, U"Box plots...", U"Draw ellipses...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_boxPlots);
		praat_addAction1 (classTable, 0, U"Normal probability plot...", U"Box plots...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_normalProbabilityPlot);
		praat_addAction1 (classTable, 0, U"Quantile-quantile plot...", U"Normal probability plot...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_quantileQuantilePlot);
		praat_addAction1 (classTable, 0, U"Quantile-quantile plot (between levels)...", U"Quantile-quantile plot...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_quantileQuantilePlot_betweenLevels);
		praat_addAction1 (classTable, 0, U"Lag plot...", U"Quantile-quantile plot (between levels)...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_lagPlot);
		praat_addAction1 (classTable, 0, U"Bar plot...", U"Lag plot...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_barPlot);
		praat_addAction1 (classTable, 0, U"Line graph...", U"Bar plot...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_LineGraph);
		praat_addAction1 (classTable, 0, U"Distribution plot...", U"Line graph...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_distributionPlot);
		praat_addAction1 (classTable, 0, U"Horizontal error bars plot...", U"Distribution plot...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_horizontalErrorBarsPlot);
		praat_addAction1 (classTable, 0, U"Vertical error bars plot...", U"Horizontal error bars plot...", GuiMenu_DEPTH_1,
				GRAPHICS_EACH__Table_verticalErrorBarsPlot);

		// The Draw_where's are deprecated 2023
			praat_addAction1 (classTable, 0, U"Draw where", U"Vertical error bars plot...", GuiMenu_DEPRECATED_2023, nullptr);
			praat_addAction1 (classTable, 0, U"Scatter plot where...", U"Draw where", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_scatterPlotWhere);
			praat_addAction1 (classTable, 0, U"Scatter plot where (mark)...", U"Scatter plot where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_scatterPlotMarkWhere);
			praat_addAction1 (classTable, 0, U"Draw ellipses where...", U"Scatter plot where (mark)...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_drawEllipsesWhere);
			praat_addAction1 (classTable, 0, U"Box plots where...", U"Draw ellipses where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_boxPlotsWhere);
			praat_addAction1 (classTable, 0, U"Normal probability plot where...", U"Box plots where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_normalProbabilityPlotWhere);
			praat_addAction1 (classTable, 0, U"Distribution plot where...", U"Scatter plot where (mark)...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_distributionPlotWhere);
			praat_addAction1 (classTable, 0, U"Draw ellipse where (standard deviation)...", U"Distribution plot where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_drawEllipseWhere);
			praat_addAction1 (classTable, 0, U"Bar plot where...", U"Draw ellipse where (standard deviation)...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_barPlotWhere);
			praat_addAction1 (classTable, 0, U"Line graph where...", U"Bar plot where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_LineGraphWhere);
			praat_addAction1 (classTable, 0, U"Lag plot where...", U"Line graph where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_lagPlotWhere);
			praat_addAction1 (classTable, 0, U"Horizontal error bars plot where...", U"Lag plot where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_horizontalErrorBarsPlotWhere);
			praat_addAction1 (classTable, 0, U"Vertical error bars plot where...", U"Horizontal error bars plot where...", GuiMenu_DEPRECATED_2023,
					GRAPHICS_EACH__Table_verticalErrorBarsPlotWhere);
	// End of deprecated 2023

	praat_addAction1 (classTable, 1, U"List row numbers where...", U"Get number of rows", GuiMenu_DEPTH_1,
			QUERY_ONE_FOR_REAL_VECTOR__Table_listRowNumbersWhere);
	praat_addAction1 (classTable, 1, U"Get number of rows where...", U"Get number of rows", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_INTEGER__Table_getNumberOfRowsWhere);
	praat_addAction1 (classTable, 1, U"Report one-way anova...", U"Report group difference (Wilcoxon rank sum)...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_ONE__Table_reportOneWayAnova);
	praat_addAction1 (classTable, 1, U"Report one-way Kruskal-Wallis...", U"Report one-way anova...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_ONE__Table_reportOneWayKruskalWallis);
	praat_addAction1 (classTable, 1, U"Report two-way anova...", U"Report one-way Kruskal-Wallis...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_ONE__Table_reportTwoWayAnova);
	praat_addAction1 (classTable, 1, U"Report robust statistics...", U"Report two-way anova...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			INFO_ONE__Table_reportRobustStatistics);
	praat_addAction1 (classTable, 0, U"Extract rows where...", U"Extract rows where column (text)...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Table_extractRowsWhere);
	praat_addAction1 (classTable, 0, U"Extract rows where (mahalanobis)...", U"Extract rows where...", GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__Table_extractRowsWhereMahalanobis);
	praat_addAction1 (classTable, 0, U"-- Extract columns ----", U"Extract rows where (mahalanobis)...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN, nullptr);
	praat_addAction1 (classTable, 0, U"Extract columns by number... || Extract column ranges...", U"-- Extract columns ----", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Table_extractColumnsByNumber);

	praat_addAction1 (classTable, 0, U"To KlattTable", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Table_to_KlattTable);
	praat_addAction1 (classTable, 1, U"Get median absolute deviation...", U"Get standard deviation...", 1,
			QUERY_ONE_FOR_REAL__Table_getMedianAbsoluteDeviation);
	praat_addAction1 (classTable, 0, U"To StringsIndex (column)...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Table_to_StringsIndex_column);
	praat_addAction1 (classTableOfReal, 0, U"Multivariate tests -", U"Get column stdev (label)...", 1, nullptr);
		praat_addAction1 (classTableOfReal, 1, U"Report multivariate normality...", U"Multivariate tests -",
				GuiMenu_DEPTH_2, INFO_ONE__TableOfReal_reportMultivariateNormality);
		praat_addAction1 (classTableOfReal, 1, U"Report covariance sphericity...", U"Report multivariate normality...",
				GuiMenu_DEPTH_2, INFO_ONE__TableOfReal_reportSphericityOfCovariance);
		praat_addAction1 (classTableOfReal, 1, U"Report covariance compound symmetry...",  U"Report covariance sphericity...",
				GuiMenu_DEPTH_2, INFO_ONE__TableOfReal_reportCompoundSymmetryOfCovariance);
		praat_addAction1 (classTableOfReal, 1, U"Report covariance identity...",  U"Report covariance compound symmetry...",
				GuiMenu_DEPTH_2, INFO_ONE__TableOfReal_reportCovarianceIdentity);
		praat_addAction1 (classTableOfReal, 2, U"Report equality of covariances...",  U"Report covariance identity...",
				GuiMenu_DEPTH_2, INFO_TWO__TableOfReal_reportEqualityOfCovariances);
	praat_addAction1 (classTableOfReal, 0, U"Append columns", U"Append", 1, 
			COMBINE_ALL_LISTED_TO_ONE__TableOfReal_appendColumns);
	praat_addAction1 (classTableOfReal, 0, U"Multivariate statistics -", nullptr, 0, nullptr);
	praat_addAction1 (classTableOfReal, 0, U"To Discriminant", nullptr, 1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_Discriminant);
	praat_addAction1 (classTableOfReal, 0, U"To PCA", nullptr, 1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_PCA_byRows);
	praat_addAction1 (classTableOfReal, 0, U"To SSCP...", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_SSCP);
	praat_addAction1 (classTableOfReal, 0, U"To SSCP (row weights)...", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_SSCP_rowWeights);
	praat_addAction1 (classTableOfReal, 0, U"To Covariance", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_Covariance);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_Correlation);
	praat_addAction1 (classTableOfReal, 0, U"To Correlation (rank)", nullptr, 1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_Correlation_rank);
	praat_addAction1 (classTableOfReal, 0, U"To CCA...", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_CCA);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (means by row labels)...", nullptr, 1,
			CONVERT_EACH_TO_ONE__TableOfReal_meansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (medians by row labels)...", nullptr, 1,
			CONVERT_EACH_TO_ONE__TableOfReal_mediansByRowLabels);
	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (rank columns)...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_TableOfReal_rankColumns);
	praat_addAction1 (classTableOfReal, 0, U"-- configurations --", nullptr, 1, 0);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (pca)...", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_Configuration_pca);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration (lda)...", nullptr, 1, 
			CONVERT_EACH_TO_ONE__TableOfReal_to_Configuration_lda);
	praat_addAction1 (classTableOfReal, 2, U"-- between tables --", nullptr, 1, 0);
	praat_addAction1 (classTableOfReal, 2, U"To TableOfReal (cross-correlations)...", nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1,
			CONVERT_TWO_TO_ONE__TableOfReal_TableOfReal_crossCorrelations);

	praat_addAction1 (classTableOfReal, 1, U"To PatternList and Categories... || To Pattern and Categories... || Split into Pattern and Categories...", U"To Matrix", 1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_PatternList_and_Categories);   // alternatives COMPATIBILITY <= 2015
	praat_addAction1 (classTableOfReal, 0, U"To Permutation (sort row labels)", U"To Matrix", 1,
			CONVERT_EACH_TO_ONE__TableOfReal_to_Permutation_sortRowlabels);

	praat_addAction1 (classTableOfReal, 1, U"To SVD", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__TableOfReal_to_SVD);
	praat_addAction1 (classTableOfReal, 2, U"To GSVD", nullptr, GuiMenu_HIDDEN,
			CONVERT_TWO_TO_ONE__TablesOfReal_to_GSVD);
	praat_addAction1 (classTableOfReal, 2, U"To Eigen (gsvd)", nullptr, GuiMenu_HIDDEN,
			CONVERT_TWO_TO_ONE__TablesOfReal_to_Eigen_gsvd);

	praat_addAction1 (classTableOfReal, 0, U"To TableOfReal (cholesky)...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__TableOfReal_choleskyDecomposition);

	praat_addAction1 (classTableOfReal, 0, U"Draw as scalable squares...", U"Draw as squares...", 1,
			GRAPHICS_EACH__TableOfReal_drawAsScalableSquares);
	praat_addAction1 (classTableOfReal, 0, U"-- scatter plots --", U"Draw top and bottom lines...", 1, nullptr);
	praat_addAction1 (classTableOfReal, 0, U"Draw scatter plot...", U"-- scatter plots --", 1,
			GRAPHICS_EACH__TableOfReal_drawScatterPlot);
	praat_addAction1 (classTableOfReal, 0, U"Draw scatter plot matrix...", U"Draw scatter plot...", 1,
			GRAPHICS_EACH__TableOfReal_drawScatterPlotMatrix);
	praat_addAction1 (classTableOfReal, 0, U"Draw box plots...", U"Draw scatter plot matrix...", 1,
			GRAPHICS_EACH__TableOfReal_drawBoxPlots);
	praat_addAction1 (classTableOfReal, 0, U"Draw biplot...", U"Draw box plots...", 1,
			GRAPHICS_EACH__TableOfReal_drawBiplot);
	praat_addAction1 (classTableOfReal, 0, U"Draw vectors...", U"Draw box plots...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			GRAPHICS_EACH__TableOfReal_drawVectors);
	praat_addAction1 (classTableOfReal, 1, U"Draw row as histogram...", U"Draw biplot...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			GRAPHICS_EACH__TableOfReal_drawRowAsHistogram);
	praat_addAction1 (classTableOfReal, 1, U"Draw rows as histogram...", U"Draw row as histogram...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			GRAPHICS_EACH__TableOfReal_drawRowsAsHistogram);
	praat_addAction1 (classTableOfReal, 1, U"Draw column as distribution...", U"Draw rows as histogram...", GuiMenu_DEPTH_1,
			GRAPHICS_EACH__TableOfReal_drawColumnAsDistribution);

	praat_addAction2 (classStrings, 1, classPermutation, 1, U"Permute strings", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Strings_Permutation_permuteStrings);

	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, U"Permute rows", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__TableOfReal_Permutation_permuteRows);
	praat_addAction2 (classTableOfReal, 1, classPermutation, 1, U"Permute columns", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__TableOfReal_Permutation_permuteColumns);

	praat_addAction1 (classTextGrid, 1, U"Get total duration of intervals where...", U"Count intervals where...", 2,
			QUERY_ONE_FOR_REAL__TextGrid_getTotalDurationOfIntervalsWhere);
	praat_addAction1 (classTextGrid, 0, U"Extend time...", U"Scale times...", 2,
			MODIFY_TextGrid_extendTime);
	praat_addAction1 (classTextGrid, 1, U"Set tier name...", U"Remove tier...", 1,
			MODIFY_EACH__TextGrid_setTierName);
	praat_addAction1 (classTextGrid, 0, U"Replace interval texts... || Replace interval text...", U"Set interval text...", 2,
			MODIFY_TextGrid_replaceIntervalTexts);   // alternatives COMPATIBILITY <= 2018
	praat_addAction1 (classTextGrid, 0, U"Replace point texts... || Replace point text...", U"Set point text...", 2,
			MODIFY_TextGrid_replacePointTexts);   // alternatives COMPATIBILITY <= 2018
	praat_addAction1 (classTextGrid, 2, U"To Table (text alignment)...", U"Extract part...", 0,
			CONVERT_TWO_TO_ONE__TextGrids_to_Table_textAlignment);
	praat_addAction1 (classTextGrid, 0, U"To DurationTier...", U"Concatenate", 0, 
			CONVERT_EACH_TO_ONE__TextGrid_to_DurationTier);
	praat_addAction1 (classTextGrid, 0, U"To TextGridNavigator (topic only)...", U"To DurationTier...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__TextGrid_to_TextGridNavigator_topicSearch);
	praat_addAction1 (classTextGrid, 0, U"To TextGridNavigator...", U"To DurationTier...", GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__TextGrid_to_TextGridNavigator);
	praat_addAction2 (classTextGrid, 1, classDurationTier, 1, U"To TextGrid (scale times)", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__TextGrid_DurationTier_to_TextGrid);

	praat_addAction2 (classTextGrid, 2, classEditCostsTable, 1, U"To Table (text alignment)...", nullptr, 0,
			CONVERT_TWO_AND_ONE_TO_ONE__TextGrids_EditCostsTable_to_Table_textAlignment);
	praat_addAction2 (classTextGrid, 1, classNavigationContext, 1, U"To TextGridNavigator...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__TextGrid_and_NavigationContext_to_TextGridNavigator);
	praat_addAction2 (classTextGrid, 1, classNavigationContext, 1, U"To TextGridTierNavigator...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__TextGrid_and_NavigationContext_to_TextGridTierNavigator);

	praat_addAction1 (classTextGridNavigator, 0, U"TextGridNavigator help", nullptr, 0,
			HELP__TextGridNavigator_help);
	praat_addAction1 (classTextGridNavigator, 0, U"Find first", nullptr, 0,
			MODIFY_EACH__TextGridNavigator_findFirst);
	praat_addAction1 (classTextGridNavigator, 0, U"Find last", nullptr, 0,
			MODIFY_EACH__TextGridNavigator_findLast);
	praat_addAction1 (classTextGridNavigator, 0, U"Find next", nullptr, 0,
			MODIFY_EACH__TextGridNavigator_findNext);
	praat_addAction1 (classTextGridNavigator, 0, U"Find next after time...", nullptr, 0,
			MODIFY_EACH__TextGridNavigator_findNextAfterTime);
	praat_addAction1 (classTextGridNavigator, 0, U"Find previous", nullptr, 0,
			MODIFY_EACH__TextGridNavigator_findPrevious);
	praat_addAction1 (classTextGridNavigator, 0, U"Find previous before time...", nullptr, 0,
			MODIFY_EACH__TextGridNavigator_findPreviousBeforeTime);

	praat_addAction1 (classTextGridNavigator, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classTextGridNavigator); 
		praat_addAction1 (classTextGridNavigator, 1, U"Get start time...", nullptr, 1,
				QUERY_ONE_FOR_REAL__TextGridNavigator_getStartTime);
		praat_addAction1 (classTextGridNavigator, 1, U"Get label...", nullptr, 1,
				QUERY_ONE_FOR_STRING__TextGridNavigator_getLabel);
		praat_addAction1 (classTextGridNavigator, 1, U"Get end time...", nullptr, 1,
				QUERY_ONE_FOR_REAL__TextGridNavigator_getEndTime);
		praat_addAction1 (classTextGridNavigator, 1, U"Get index...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__TextGridNavigator_getIndex);
		praat_addAction1 (classTextGridNavigator, 1, U"-- number of matches --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGridNavigator, 1, U"List indices...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__TextGridNavigator_listIndices);
		praat_addAction1 (classTextGridNavigator, 1, U"List start times...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__TextGridNavigator_listStartTimes);
		praat_addAction1 (classTextGridNavigator, 1, U"List labels...", nullptr, 1,
				QUERY_ONE_FOR_STRING_ARRAY__TextGridNavigator_listLabels);
		praat_addAction1 (classTextGridNavigator, 1, U"List end times...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__TextGridNavigator_listEndTimes);
		praat_addAction1 (classTextGridNavigator, 1, U"List domains...", nullptr, 1,
				QUERY_ONE_FOR_MATRIX__TextGridNavigator_listDomains);
		praat_addAction1 (classTextGridNavigator, 1, U"Get number of matches", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfMatches);
		praat_addAction1 (classTextGridNavigator, 1, U"Get number of Topic matches...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfTopicMatches);
		praat_addAction1 (classTextGridNavigator, 1, U"Get number of Before matches...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfBeforeMatches);
		praat_addAction1 (classTextGridNavigator, 1, U"Get number of After matches...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__TextGridNavigator_getNumberOfAfterMatches);
	praat_addAction1 (classTextGridNavigator, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify Topic match criterion...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyTopicCriterion);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify Before match criterion...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyBeforeCriterion);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify After match criterion...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyAfterCriterion);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify combination criterion...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyCombinationCriterion);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify match domain...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyMatchDomain);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify match domain alignment...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyMatchDomainAlignment);
		praat_addAction1 (classTextGridNavigator, 0, U"-- search range extensions --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify Before range...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyBeforeRange);
		praat_addAction1 (classTextGridNavigator, 0, U"Modify After range...", nullptr, 1,
				MODIFY_EACH__TextGridNavigator_modifyAfterRange);

	praat_addAction2 (classTextGridNavigator, 1, classNavigationContext, 1, U"Replace navigation context...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_replaceNavigationContext);
	praat_addAction2 (classTextGridNavigator, 1, classTextGridTierNavigator, 1, U"Add TextGridTierNavigator...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_addTextGridTierNavigator);
	praat_addAction2 (classTextGridNavigator, 1, classTextGrid, 1, U"Replace search tiers", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_replaceSearchTiers);
	praat_addAction2 (classTextGridNavigator, 1, classTextGrid, 1, U"Add search tier (topic only)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_addSearchTier_topicOnly);
	praat_addAction2 (classTextGridNavigator, 1, classTextGrid, 1, U"Add search tier...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__TextGridNavigator_addSearchTier);

	praat_addAction1 (classTextGridTierNavigator, 0, U"To TextGridNavigator", nullptr, 1,
			CONVERT_EACH_TO_ONE__TextGridTierNavigator_to_TextGridNavigator);

	INCLUDE_MANPAGES (manual_dwtools_init)
	INCLUDE_MANPAGES (manual_Permutation_init)

	INCLUDE_LIBRARY (praat_MultiSampledSpectrogram_init)
	INCLUDE_LIBRARY (praat_MDS_actions_init)
	INCLUDE_LIBRARY (praat_HMM_init)
	INCLUDE_LIBRARY (praat_BSS_init)
}

/* End of file praat_David_init.cpp */
