/* TextGridArea.cpp
 *
 * Copyright (C) 1992-2026 Paul Boersma
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

#include "TextGridArea.h"
#include "TextGrid_Sound.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "LongSoundArea.h"   // for drawing TextGrid and Sound, or for aligning TextGrid to Sound
#include "SoundAnalysisArea.h"   // for drawing TextGrid and Pitch
#include "EditorM.h"
#include "SpeechRecognizer.h"

Thing_implement (TextGridArea, FunctionArea, 0);

#include "enums_getText.h"
#include "TextGridArea_enums.h"
#include "enums_getValue.h"
#include "TextGridArea_enums.h"

#include "Prefs_define.h"
#include "TextGridArea_prefs.h"
#include "Prefs_install.h"
#include "TextGridArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "TextGridArea_prefs.h"

/*
 * The main invariant of the TextGridArea is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */

static void checkTierSelection (constTextGridArea me, conststring32 verbPhrase) {
	if (my selectedTier < 1 || my selectedTier > my textGrid() -> tiers->size)
		Melder_throw (U"To ", verbPhrase, U", first select a tier by clicking anywhere inside it.");
}
static integer getSelectedInterval (constTextGridArea me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my textGrid() -> tiers->size);
	const IntervalTier tier = (IntervalTier) my textGrid() -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection());
}

static integer getSelectedLeftBoundary (constTextGridArea me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my textGrid() -> tiers->size);
	const IntervalTier tier = (IntervalTier) my textGrid() -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my startSelection());
}
static integer getSelectedPoint (constTextGridArea me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my textGrid() -> tiers->size);
	const TextTier tier = (TextTier) my textGrid() -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classTextTier);
	Melder_assert (isdefined (my startSelection()));
	return AnyTier_hasPoint (tier->asAnyTier(), my startSelection());
}

static void timeToInterval (TextGridArea me, double t, integer tierNumber,
	double *out_tmin, double *out_tmax)
{
	Melder_assert (isdefined (t));
	const Function tier = my textGrid() -> tiers->at [tierNumber];
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (tier, & intervalTier, & textTier);
	if (intervalTier) {
		integer iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval == 0) {
			if (t < my tmin()) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals.size;
			}
		}
		Melder_assert (iinterval >= 1);
		Melder_assert (iinterval <= intervalTier -> intervals.size);
		const TextInterval interval = intervalTier -> intervals.at [iinterval];
		*out_tmin = interval -> xmin;
		*out_tmax = interval -> xmax;
	} else {
		const integer n = textTier -> points.size;
		if (n == 0) {
			*out_tmin = my tmin();
			*out_tmax = my tmax();
		} else {
			integer ipointleft = AnyTier_timeToLowIndex (textTier->asAnyTier(), t);
			*out_tmin = ( ipointleft == 0 ? my tmin() : textTier -> points.at [ipointleft] -> number );
			*out_tmax = ( ipointleft == n ? my tmax() : textTier -> points.at [ipointleft + 1] -> number );
		}
	}
	Melder_clipLeft (my tmin(), out_tmin);
	Melder_clipRight (out_tmax, my tmax());
}

static void insertBoundaryOrPoint (TextGridArea me, integer itier, double t1, double t2, bool insertSecond) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	if (itier < 1 || itier > numberOfTiers)
		Melder_throw (U"No tier ", itier, U".");
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [itier], & intervalTier, & textTier);
	Melder_assert (t1 <= t2);

	if (intervalTier) {
		autoTextInterval rightNewInterval, midNewInterval;
		const bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
		const bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
		if (t1 == t2 && t1IsABoundary)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t1, 6), U" seconds, because there is already a boundary there.");
		if (t1IsABoundary && t2IsABoundary)
			Melder_throw (U"Cannot add boundaries at ", Melder_fixed (t1, 6), U" and ", Melder_fixed (t2, 6), U" seconds, because there are already boundaries there.");
		const integer iinterval = IntervalTier_timeToIndex (intervalTier, t1);
		const integer iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
		if (iinterval == 0 || iinterval2 == 0)
			Melder_throw (U"The selection is outside the time domain of the intervals.");
		const integer correctedIinterval2 = ( t2IsABoundary && iinterval2 == intervalTier -> intervals.size ? iinterval2 + 1 : iinterval2 );
		if (correctedIinterval2 > iinterval + 1 || (correctedIinterval2 > iinterval && ! t2IsABoundary))
			Melder_throw (U"The selection straddles a boundary.");
		const TextInterval interval = intervalTier -> intervals.at [iinterval];

		if (t1 == t2) {
			FunctionArea_save (me, U"Add boundary");
		} else {
			FunctionArea_save (me, U"Add interval");
		}

		if (itier == my selectedTier) {
			/*
				Divide up the label text into left, mid and right, depending on where the text selection is.
			*/
			integer left, right;
			autostring32 text = GuiText_getStringAndSelectionPosition (my functionEditor() -> textArea, & left, & right);
			const bool wholeTextIsSelected = ( right - left == Melder_length (text.get()) );
			rightNewInterval = TextInterval_create (t2, interval -> xmax, text.get() + right);
			text [right] = U'\0';
			midNewInterval = TextInterval_create (t1, t2, text.get() + left);
			if (! wholeTextIsSelected || t1 != t2)
				text [left] = U'\0';
			TextInterval_setText (interval, text.get());
		} else {
			/*
				Move the text to the left of the boundary.
			*/
			rightNewInterval = TextInterval_create (t2, interval -> xmax, U"");
			midNewInterval = TextInterval_create (t1, t2, U"");
		}
		if (t1IsABoundary) {
			/*
				Merge mid with left interval.
			*/
			if (interval -> xmin != t1)
				Melder_crash (U"Boundary unequal: ", interval -> xmin, U" versus ", t1, U".");
			interval -> xmax = t2;
			TextInterval_setText (interval, Melder_cat (interval -> text.get(), midNewInterval -> text.get()));
		} else if (t2IsABoundary) {
			/*
				Merge mid and right interval.
			*/
			if (interval -> xmax != t2)
				Melder_crash (U"Boundary unequal: ", interval -> xmax, U" versus ", t2, U".");
			interval -> xmax = t1;
			Melder_assert (rightNewInterval -> xmin == t2);
			Melder_assert (rightNewInterval -> xmax == t2);
			rightNewInterval -> xmin = t1;
			TextInterval_setText (rightNewInterval.get(), Melder_cat (midNewInterval -> text.get(), rightNewInterval -> text.get()));
		} else {
			interval -> xmax = t1;
			if (t1 != t2)
				intervalTier -> intervals. addItem_move (midNewInterval.move());
		}
		intervalTier -> intervals. addItem_move (rightNewInterval.move());
		if (insertSecond && numberOfTiers >= 2 && t1 == t2) {
			/*
				Find the last time before t on another tier.
			*/
			double tlast = interval -> xmin;
			for (integer jtier = 1; jtier <= numberOfTiers; jtier ++) {
				if (jtier != itier) {
					double startInterval, endInterval;
					timeToInterval (me, t1, jtier, & startInterval, & endInterval);
					if (startInterval > tlast)
						tlast = startInterval;
				}
			}
			if (tlast > interval -> xmin && tlast < t1) {
				autoTextInterval newInterval = TextInterval_create (tlast, t1, U"");
				interval -> xmax = tlast;
				intervalTier -> intervals. addItem_move (newInterval.move());
			}
		}
	} else {
		Melder_assert (isdefined (t1));
		if (AnyTier_hasPoint (textTier->asAnyTier(), t1))
			Melder_throw (U"Cannot add a point at ", Melder_fixed (t1, 6), U" seconds, because there is already a point there.");

		FunctionArea_save (me, U"Add point");

		autoTextPoint newPoint = TextPoint_create (t1, U"");
		textTier -> points. addItem_move (newPoint.move());
	}
	my setSelection (t1, t1);
}


#pragma mark - TextGridArea info

void structTextGridArea :: v1_info () {
	MelderInfo_writeLine (U"Selected tier: ", our selectedTier);
	MelderInfo_writeLine (U"TextGrid uses text styles: ", our instancePref_useTextStyles());
	MelderInfo_writeLine (U"TextGrid font size: ", our instancePref_fontSize());
	MelderInfo_writeLine (U"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (our instancePref_alignment()));
}


#pragma mark - TextGridArea drawing

void structTextGridArea :: v_specializedHighlightBackground () const {
	Melder_assert (our textGrid());
	const integer numberOfTiers = our textGrid() -> tiers->size;
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		const Function anyTier = our textGrid() -> tiers->at [itier];
		if (anyTier -> classInfo != classIntervalTier)
			continue;
		IntervalTier tier = (IntervalTier) anyTier;
		/*
			Highlight interval: yellow (selected) or green (matching label).
		*/
		const integer selectedInterval = ( itier == our selectedTier ? getSelectedInterval (this) : 0 );
		const integer numberOfIntervals = tier -> intervals.size;
		for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			/* mutable clip */ double startInterval = interval -> xmin, endInterval = interval -> xmax;
			if (endInterval > our startWindow() && startInterval < our endWindow()) {   // interval visible?
				const bool intervalIsSelected = ( iinterval == selectedInterval );
				const bool labelDoesMatch = Melder_stringMatchesCriterion (interval -> text.get(),
						our instancePref_greenMethod(), our instancePref_greenString(), true);
				Melder_clipLeft (our startWindow(), & startInterval);
				Melder_clipRight (& endInterval, our endWindow());
				const double bottom = 1.0 - double (itier) / numberOfTiers;
				const double top = 1.0 - double (itier - 1) / numberOfTiers;
				if (labelDoesMatch) {
					Graphics_setColour (our graphics(), Melder_LIME);
					Graphics_fillRectangle (our graphics(), startInterval, endInterval, bottom, top);
				}
				if (intervalIsSelected) {
					Graphics_setColour (our graphics(), Melder_YELLOW);
					Graphics_fillRectangle (our graphics(),
						labelDoesMatch ? 0.85 * startInterval + 0.15 * endInterval : startInterval,
						labelDoesMatch ? 0.15 * startInterval + 0.85 * endInterval : endInterval,
						labelDoesMatch ? 0.85 * bottom + 0.15 * top : bottom,
						labelDoesMatch ? 0.15 * bottom + 0.85 * top : top
					);
				}
			}
		}
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
}
static void do_drawIntervalTier (TextGridArea me, IntervalTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	integer x1DC, x2DC, yDC;
	Graphics_WCtoDC (my graphics(), my startWindow(), 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics(), my endWindow(), 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics(), my instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (my graphics(), my instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (my graphics(), my instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (my graphics(), my instancePref_useTextStyles());

	const integer selectedInterval = ( itier == my selectedTier ? getSelectedInterval (me) : 0 );
	const integer numberOfIntervals = tier -> intervals.size;

	/*
		Draw a grey bar and a selection button at the cursor position.
	*/
	if (my startSelection() == my endSelection() && my startSelection() >= my startWindow() && my startSelection() <= my endWindow()) {
		/* mutable search */ bool cursorAtBoundary = false;
		for (integer iinterval = 2; iinterval <= numberOfIntervals; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			if (interval -> xmin == my startSelection())
				cursorAtBoundary = true;
		}
		if (! cursorAtBoundary) {
			const double dy = Graphics_dyMMtoWC (my graphics(), 1.5);
			Graphics_setGrey (my graphics(), 0.8);
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), my startSelection(), 0.0, my startSelection(), 1.0);
			Graphics_setLineWidth (my graphics(), 1.0);
			if (my editable()) {
				Graphics_setColour (my graphics(), Melder_BLUE);
				Graphics_circle_mm (my graphics(), my startSelection(), 1.0 - dy, 3.0);
			}
		}
	}

	Graphics_setTextAlignment (my graphics(), my instancePref_alignment(), Graphics_HALF);
	for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
		const TextInterval interval = tier -> intervals.at [iinterval];
		/* mutable clip */ double startInterval = interval -> xmin, endInterval = interval -> xmax;
		Melder_clipLeft (my tmin(), & startInterval);
		Melder_clipRight (& endInterval, my tmax());
		if (startInterval >= endInterval)
			continue;
		const bool intervalIsSelected = ( selectedInterval == iinterval );

		/*
			Draw left boundary.
		*/
		if (startInterval >= my startWindow() && startInterval <= my endWindow() && iinterval > 1) {
			const bool boundaryIsSelected = ( my selectedTier == itier && startInterval == my startSelection() );
			Graphics_setColour (my graphics(), DataGui_defaultForegroundColour (me, boundaryIsSelected));
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), startInterval, 0.0, startInterval, 1.0);

			/*
				Show alignment with cursor.
			*/
			if (startInterval == my startSelection()) {
				Graphics_setColour (my graphics(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics(), startInterval, 0.0, startInterval, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics(), 1.0);

		/*
			Draw label text.
		*/
		if (interval -> text && endInterval >= my startWindow() && startInterval <= my endWindow()) {
			const double t1 = std::max (my startWindow(), startInterval);
			const double t2 = std::min (my endWindow(), endInterval);
			Graphics_setColour (my graphics(), DataGui_defaultForegroundColour (me, intervalIsSelected));
			Graphics_rectangleText_wrapAndTruncate (my graphics(), t1, t2, 0.0, 1.0, interval -> text.get());
			Graphics_setColour (my graphics(), Melder_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics(), true);
	Graphics_setNumberSignIsBold (my graphics(), true);
	Graphics_setCircumflexIsSuperscript (my graphics(), true);
	Graphics_setUnderscoreIsSubscript (my graphics(), true);
}

static void do_drawTextTier (TextGridArea me, TextTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	const integer numberOfPoints = tier -> points.size;
	Graphics_setPercentSignIsItalic (my graphics(), my instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (my graphics(), my instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (my graphics(), my instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (my graphics(), my instancePref_useTextStyles());

	/*
		Draw a grey bar and a selection button at the cursor position.
	*/
	if (my startSelection() == my endSelection() && my startSelection() >= my startWindow() && my startSelection() <= my endWindow()) {
		bool cursorAtPoint = false;
		for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
			const TextPoint point = tier -> points.at [ipoint];
			if (point -> number == my startSelection())
				cursorAtPoint = true;
		}
		if (! cursorAtPoint) {
			const double dy = Graphics_dyMMtoWC (my graphics(), 1.5);
			Graphics_setGrey (my graphics(), 0.8);
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), my startSelection(), 0.0, my startSelection(), 1.0);
			Graphics_setLineWidth (my graphics(), 1.0);
			if (my editable()) {
				Graphics_setColour (my graphics(), DataGuiColour_EDITABLE);
				Graphics_circle_mm (my graphics(), my startSelection(), 1.0 - dy, 3.0);
			}
		}
	}

	Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
	for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
		const TextPoint point = tier -> points.at [ipoint];
		const double t = point -> number;
		if (t >= my startWindow() && t <= my endWindow()) {
			const bool pointIsSelected = ( itier == my selectedTier && t == my startSelection() );
			Graphics_setColour (my graphics(), DataGui_defaultForegroundColour (me, pointIsSelected));
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), t, 0.0, t, 0.2);
			Graphics_line (my graphics(), t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics(), 1.0);

			/*
				Wipe out the cursor where the text is going to be.
			*/
			if (my startSelection() == t || my endSelection() == t) {
				Graphics_setLineWidth (my graphics(), 1.0);
				Graphics_setColour (my graphics(), DataGuiColour_AREA_BACKGROUND);
				Graphics_line (my graphics(), t, 0.2, t, 0.8);
				Graphics_setLineWidth (my graphics(), 1.0);
			}

			/*
				Show alignment with cursor.
			*/
			if (my startSelection() == my endSelection() && t == my startSelection()) {
				Graphics_setColour (my graphics(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics(), t, 0.0, t, 0.2);
				Graphics_line (my graphics(), t, 0.8, t, 1.0);
				Graphics_setLineWidth (my graphics(), 1.0);
			}
			Graphics_setColour (my graphics(), DataGui_defaultForegroundColour (me, pointIsSelected));
			if (point -> mark)
				Graphics_text (my graphics(), t, 0.5, point -> mark.get());
		}
	}
	Graphics_setPercentSignIsItalic (my graphics(), true);
	Graphics_setNumberSignIsBold (my graphics(), true);
	Graphics_setCircumflexIsSuperscript (my graphics(), true);
	Graphics_setUnderscoreIsSubscript (my graphics(), true);
}
void structTextGridArea :: v_drawInside () {
	Graphics_Viewport vp2;
	const integer numberOfTiers = our textGrid() -> tiers->size;
	const enum kGraphics_font oldFont = Graphics_inqFont (our graphics());
	const double oldFontSize = Graphics_inqFontSize (our graphics());
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, 1.0);
	const double xleft = our functionViewerLeft_WC();
	const double xright = our functionViewerRight_WC();
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		const Function anyTier = our textGrid() -> tiers->at [itier];
		const bool tierIsSelected = ( itier == our selectedTier );
		const bool isIntervalTier = ( anyTier -> classInfo == classIntervalTier );
		vp2 = Graphics_insetViewport (our graphics(), 0.0, 1.0,
			1.0 - (double) itier / (double) numberOfTiers,
			1.0 - (double) (itier - 1) / (double) numberOfTiers
		);
		Graphics_setColour (our graphics(), Melder_BLACK);
		if (itier != 1)
			Graphics_line (our graphics(), our startWindow(), 1.0, our endWindow(), 1.0);

		/*
			Show which tier is selected.
		*/
		if (tierIsSelected) {
			MelderColour oldColour = Graphics_inqColour (our graphics());
			Graphics_setColour (our graphics(), Melder_YELLOW);
			Graphics_fillRectangle (our graphics(), xleft, our startWindow(), 0.0, 1.0);
			Graphics_fillRectangle (our graphics(), our endWindow(), xright, 0.0, 1.0);
			Graphics_setColour (our graphics(), oldColour);
		}
		/*
			Show the name of the tier.
		*/
		Graphics_setColour (our graphics(), DataGui_defaultForegroundColour (this, tierIsSelected));
		Graphics_setFont (our graphics(), oldFont);
		Graphics_setFontSize (our graphics(), 14);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (our graphics(), Graphics_LEFT, Graphics_HALF);
			Graphics_rectangleText_maximalFit (our graphics(), our endWindow(), xright, 0.05, 0.5,
					0.0, 1.0, 0.01, 0.1, anyTier -> name.get());
		}
		/*
			Show the number of the tier.
		*/
		Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
		if (our instancePref_showNumberOf() == kTextGridArea_showNumberOf::NOTHING) {
			Graphics_rectangleText_maximalFit (our graphics(), xleft, our startWindow(), 0.05, 0.5,
					0.0, 1.0, 0.01, 0.1, Melder_cat (U"##", itier));
		} else {
			const conststring32 units = ( isIntervalTier ? U" int" : U" pts" );
			if (our instancePref_showNumberOf() == kTextGridArea_showNumberOf::INTERVALS_OR_POINTS) {
				const integer count = ( isIntervalTier ? ((IntervalTier) anyTier) -> intervals.size : ((TextTier) anyTier) -> points.size );
				const integer position = ( itier == our selectedTier ? ( isIntervalTier ? getSelectedInterval (this) : getSelectedPoint (this) ) : 0 );
				if (position)
					Graphics_rectangleText_maximalFit (our graphics(), xleft, our startWindow(), 0.05, 0.5,
							0.0, 1.0, 0.01, 0.1, Melder_cat (tierIsSelected ? U"\\s{(" : U"     \\s{(", position, U"/", count, units, U")}  ##", itier));
				else
					Graphics_rectangleText_maximalFit (our graphics(), xleft, our startWindow(), 0.05, 0.5,
							0.0, 1.0, 0.01, 0.1, Melder_cat (tierIsSelected ? U"\\s{(" : U"     \\s{(", count, units, U")}  ##", itier));
			} else {
				Melder_assert (our instancePref_showNumberOf() == kTextGridArea_showNumberOf::NONEMPTY_INTERVALS_OR_POINTS);
				integer count = 0;
				if (isIntervalTier) {
					const IntervalTier tier = (IntervalTier) anyTier;
					const integer numberOfIntervals = tier -> intervals.size;
					for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
						const TextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> text && interval -> text [0] != U'\0')
							count ++;
					}
				} else {
					const TextTier tier = (TextTier) anyTier;
					const integer numberOfPoints = tier -> points.size;
					for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
						const TextPoint point = tier -> points.at [ipoint];
						if (point -> mark && point -> mark [0] != U'\0')
							count ++;
					}
				}
				Graphics_rectangleText_maximalFit (our graphics(), xleft, our startWindow(), 0.05, 0.5,
						0.0, 1.0, 0.01, 0.1, Melder_cat (tierIsSelected ? U"\\s{(##" : U"     \\s{(##", count, units, U"#)}  ##", itier));
			}
		}

		Graphics_setColour (our graphics(), Melder_BLACK);
		Graphics_setFont (our graphics(), kGraphics_font::TIMES);
		Graphics_setFontSize (our graphics(), our instancePref_fontSize());
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (our graphics(), vp2);
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
	Graphics_setFont (our graphics(), oldFont);
	Graphics_setFontSize (our graphics(), oldFontSize);

	if (isdefined (our draggingTime) && hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
		Graphics_xorOn (our graphics(), Melder_MAROON);
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			if (our draggingTiers [itier]) {
				const double ymin = 1.0 - (double) itier / numberOfTiers;
				const double ymax = 1.0 - (double) (itier - 1) / numberOfTiers;
				Graphics_setLineWidth (our graphics(), 7.0);
				Graphics_line (our graphics(), our draggingTime, ymin, our draggingTime, ymax);
			}
		}
		our functionEditor() -> viewDataAsWorldByFraction ();
		Graphics_setLineWidth (our graphics(), 1.0);
		Graphics_line (our graphics(), our draggingTime, 0.0, our draggingTime, 1.0);
		Graphics_text (our graphics(), our draggingTime, 1.0, Melder_fixed (our draggingTime, 6));
		Graphics_xorOff (our graphics());
	}
}


#pragma mark - TextGridArea tracking

static integer localY_fraction_toTier (TextGridArea me, double localY_fraction) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	integer tierNumber = numberOfTiers - Melder_ifloor (localY_fraction * (double) numberOfTiers);
	Melder_clip (1_integer, & tierNumber, numberOfTiers);
	return tierNumber;
}

bool structTextGridArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	const integer numberOfTiers = our textGrid() -> tiers->size;
	const integer oldSelectedTier = our selectedTier;

	constexpr double clickingVicinityRadius_mm = 1.0;
	constexpr double draggingVicinityRadius_mm = clickingVicinityRadius_mm + 0.2;   // must be greater than `clickingVicinityRadius_mm`
	constexpr double droppingVicinityRadius_mm = 1.5;

	const integer mouseTier = localY_fraction_toTier (this, localY_fraction);

	our draggingTime = undefined;   // information to next expose event
	if (event -> isClick()) {
		if (isdefined (our anchorTime))   // sanity check for the fixed order click-drag-drop
			return false;
		Melder_assert (our clickedLeftBoundary == 0);
		Melder_assert (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce);
		our draggingTiers. reset();
		/*
			The user clicked in the grid part.
			We select the tier in which they clicked.
		*/
		our selectedTier = mouseTier;
		double startInterval, endInterval;
		timeToInterval (this, x_world, our selectedTier, & startInterval, & endInterval);

		if (event -> optionKeyPressed != event -> commandKeyPressed) {
			if (event -> isLeftBottomFunctionKeyPressed()) {
				our setSelection (x_world - startInterval < endInterval - x_world ? startInterval : endInterval, our endSelection());   // to nearest boundary
				return FunctionEditor_UPDATE_NEEDED;
			}
			if (event -> isRightBottomFunctionKeyPressed()) {
				our setSelection (our startSelection(), x_world - startInterval < endInterval - x_world ? startInterval : endInterval);
				return FunctionEditor_UPDATE_NEEDED;
			}
		}
		Melder_assert (! our functionEditor() -> clickWasModifiedByOptionKey && ! our functionEditor() -> clickWasModifiedByCommandKey ||
		                 our functionEditor() -> clickWasModifiedByOptionKey &&   our functionEditor() -> clickWasModifiedByCommandKey);   // either neither or both

		IntervalTier selectedIntervalTier;
		TextTier selectedTextTier;
		AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our selectedTier], & selectedIntervalTier, & selectedTextTier);

		if (x_world <= our startWindow() || x_world >= our endWindow())
			return FunctionEditor_UPDATE_NEEDED;

		/*
			Get the time of the nearest boundary or point.
		*/
		if (selectedIntervalTier) {
			const integer clickedIntervalNumber = IntervalTier_timeToIndex (selectedIntervalTier, x_world);
			const bool theyClickedOutsidetheTimeDomainOfTheIntervals = ( clickedIntervalNumber == 0 );
			if (theyClickedOutsidetheTimeDomainOfTheIntervals)
				return FunctionEditor_UPDATE_NEEDED;
			const TextInterval interval = selectedIntervalTier -> intervals.at [clickedIntervalNumber];
			if (x_world > 0.5 * (interval -> xmin + interval -> xmax)) {
				our anchorTime = interval -> xmax;
				our clickedLeftBoundary = clickedIntervalNumber + 1;
			} else {
				our anchorTime = interval -> xmin;
				our clickedLeftBoundary = clickedIntervalNumber;
			}
		} else {
			const integer clickedPointNumber = AnyTier_timeToNearestIndex (selectedTextTier->asAnyTier(), x_world);
			if (clickedPointNumber != 0) {
				const TextPoint point = selectedTextTier -> points.at [clickedPointNumber];
				our anchorTime = point -> number;
			}
		}
		Melder_assert (! (selectedIntervalTier && our clickedLeftBoundary == 0));

		const bool nearBoundaryOrPoint = ( isdefined (our anchorTime) &&
				fabs (Graphics_dxWCtoMM (our graphics(), x_world - our anchorTime)) < 1.5 );
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, 1.0);
		const double distanceToCursorCircle = ( our startSelection() != our endSelection() ? undefined :
			Graphics_distanceWCtoMM (our graphics(), x_world, localY_fraction,
				our startSelection(),
				(numberOfTiers + 1 - our selectedTier) / double (numberOfTiers) -
				Graphics_dyMMtoWC (our graphics(), 1.5))
		);
		trace (localY_fraction, U" ", distanceToCursorCircle);
		const bool nearCursorCircle = ( distanceToCursorCircle < 1.5 );

		if (nearBoundaryOrPoint) {
			/*
				Possibility 1: the user clicked near a boundary or point.
				Perhaps drag it.
			*/
			bool boundaryOrPointIsMovable = true;
			if (selectedIntervalTier) {
				const bool isLeftEdgeOfFirstInterval = ( our clickedLeftBoundary <= 1 );
				const bool isRightEdgeOfLastInterval = ( our clickedLeftBoundary > selectedIntervalTier -> intervals.size );
				boundaryOrPointIsMovable = ! isLeftEdgeOfFirstInterval && ! isRightEdgeOfLastInterval;
			}
			/*
				If the user clicked on an unselected boundary or point, we extend or shrink the selection to it.
			*/
			const bool rubberBanding = our functionEditor() -> clickWasModifiedByOptionKey &&
			                           our functionEditor() -> clickWasModifiedByCommandKey;
			if (event -> shiftKeyPressed && ! rubberBanding) {
				if (our anchorTime > 0.5 * (our startSelection() + our endSelection()))
					our setSelection (our startSelection(), our anchorTime);
				else
					our setSelection (our anchorTime, our endSelection());
			}
			if (! boundaryOrPointIsMovable) {
				our draggingTime = undefined;
				our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
				our anchorTime = undefined;
				our clickedLeftBoundary = 0;
				return FunctionEditor_UPDATE_NEEDED;
			}
			/*
				Determine the set of selected boundaries and points, and the dragging range.
			*/
			our draggingTiers = zero_BOOLVEC (numberOfTiers);
			if (rubberBanding) {
				our leftDraggingBoundary = our startSelection();
				our rightDraggingBoundary = our endSelection();
			} else {
				our leftDraggingBoundary = our tmin();
				our rightDraggingBoundary = our tmax();
			}
			for (int itier = 1; itier <= numberOfTiers; itier ++) {
				/*
					If the user has pressed the shift key, let her drag all the boundaries and points at this time.
					Otherwise, let her drag only the boundary or point on the clicked tier.
				*/
				if (itier == mouseTier || our functionEditor() -> clickWasModifiedByShiftKey == our instancePref_shiftDragMultiple()) {
					IntervalTier intervalTier;
					TextTier textTier;
					AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [itier], & intervalTier, & textTier);
					if (intervalTier) {
						const integer ibound = IntervalTier_hasBoundary (intervalTier, our anchorTime);
						if (ibound) {
							TextInterval leftInterval = intervalTier -> intervals.at [ibound - 1];
							TextInterval rightInterval = intervalTier -> intervals.at [ibound];
							our draggingTiers [itier] = true;
							if (! rubberBanding) {
								/*
									Prevent the user from dragging the boundary past its left or right neighbours on the same tier.
								*/
								Melder_clipLeft (leftInterval -> xmin, & our leftDraggingBoundary);
								Melder_clipRight (& our rightDraggingBoundary, rightInterval -> xmax);
							}
						}
					} else {
						Melder_assert (isdefined (our anchorTime));
						if (AnyTier_hasPoint (textTier->asAnyTier(), our anchorTime)) {
							/*
								Other than with boundaries on interval tiers,
								points on text tiers can be dragged past their neighbours.
							*/
							our draggingTiers [itier] = true;
						}
					}
				}
			}
		} else if (nearCursorCircle && our editable()) {
			/*
				Possibility 2: the user clicked near the cursor circle.
				Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
				because we are not 'nearBoundaryOrPoint'.
			*/
			Melder_assert (isdefined (our startSelection()));   // precondition of v_updateText()
			if (our selectedTier != oldSelectedTier)
				our v_updateText();   // this puts the text of the newly clicked tier into the text area
			insertBoundaryOrPoint (this, mouseTier, our startSelection(), our startSelection(), false);
			FunctionArea_broadcastDataChanged (this);
		} else {
			/*
				Possibility 3: the user clicked in empty space.
				Select the interval, if any.
			*/
			if (selectedIntervalTier)
				our setSelection (startInterval, endInterval);
		}
	} else if (event -> isDrag ()) {
		if (our editable() && isdefined (our anchorTime) && our draggingTiers.size > 0) {
			our draggingTime = x_world;
			if (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
				const double distanceToAnchor_mm = fabs (Graphics_dxWCtoMM (our graphics(), x_world - our anchorTime));
				if (distanceToAnchor_mm > draggingVicinityRadius_mm)
					our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = true;
			}
		}
	} else if (event -> isDrop ()) {
		if (isundef (our anchorTime) || our draggingTiers.size == 0) {   // TODO: figure out a circumstance under which anchorTime could be undefined
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			If the user shift-clicked, we extend the selection (this already happened during click()).
		*/
		if (event -> shiftKeyPressed && ! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			If the user dropped near an existing boundary in an unselected tier,
			we snap to that mark.
		*/
		const integer itierDrop = localY_fraction_toTier (this, localY_fraction);
		bool droppedOnABoundaryOrPointInsideAnUnselectedTier = false;
		if (x_world > 0.0 && ! our draggingTiers [itierDrop]) {   // dropped inside an unselected tier?
			const Function anyTierDrop = our textGrid() -> tiers->at [itierDrop];
			if (anyTierDrop -> classInfo == classIntervalTier) {
				const IntervalTier tierDrop = (IntervalTier) anyTierDrop;
				for (integer ibound = 1; ibound < tierDrop -> intervals.size; ibound ++) {
					const TextInterval left = tierDrop -> intervals.at [ibound];
					const double mouseDistanceToBoundary = fabs (Graphics_dxWCtoMM (our graphics(), x_world - left -> xmax));
					if (mouseDistanceToBoundary < droppingVicinityRadius_mm) {
						x_world = left -> xmax;   // snap to boundary
						droppedOnABoundaryOrPointInsideAnUnselectedTier = true;
					}
				}
			} else {
				const TextTier tierDrop = (TextTier) anyTierDrop;
				for (integer ipoint = 1; ipoint <= tierDrop -> points.size; ipoint ++) {
					const TextPoint point = tierDrop -> points.at [ipoint];
					const double mouseDistanceToPoint_mm = fabs (Graphics_dxWCtoMM (our graphics(), x_world - point -> number));
					if (mouseDistanceToPoint_mm < droppingVicinityRadius_mm) {
						x_world = point -> number;   // snap to point
						droppedOnABoundaryOrPointInsideAnUnselectedTier = true;
					}
				}
			}
		}
		/*
			If the user dropped near the cursor (outside the anchor),
			we snap to the cursor.
		*/
		if (our startSelection() == our endSelection() && our startSelection() != our anchorTime) {
			const double mouseDistanceToCursor = fabs (Graphics_dxWCtoMM (our graphics(), x_world - our startSelection()));
			if (mouseDistanceToCursor < droppingVicinityRadius_mm)
				x_world = our startSelection();
		}
		/*
			If the user wiggled near the anchor, we snap to the anchor and bail out
			(a boundary has been selected, but nothing has been dragged).
		*/
		if (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce && ! droppedOnABoundaryOrPointInsideAnUnselectedTier) {
			our setSelection (our anchorTime, our anchorTime);
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}

		if (! our editable()) {
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			We cannot move a boundary out of the dragging range.
		*/
		if (x_world <= our leftDraggingBoundary || x_world >= our rightDraggingBoundary) {
			Melder_beep ();
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}

		const bool rubberBanding = our functionEditor() -> clickWasModifiedByOptionKey &&
		                           our functionEditor() -> clickWasModifiedByCommandKey;
		FunctionArea_save (this, rubberBanding ? U"Rubber banding" : U"Drag");

		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			if (! our draggingTiers [itier])
				continue;
			IntervalTier intervalTier;
			TextTier textTier;
			AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [itier], & intervalTier, & textTier);
			if (rubberBanding) {
				if (intervalTier) {
					for (integer iinterval = 1; iinterval <= intervalTier -> intervals.size; iinterval ++) {
						const mutableTextInterval interval = intervalTier -> intervals.at [iinterval];
						if (interval -> xmin > leftDraggingBoundary && interval -> xmin < rightDraggingBoundary) {
							if (interval -> xmin < our anchorTime)
								NUMscale (& interval -> xmin, our leftDraggingBoundary, our anchorTime, our leftDraggingBoundary, x_world);
							else
								NUMscale (& interval -> xmin, our anchorTime, our rightDraggingBoundary, x_world, our rightDraggingBoundary);
						}
						if (interval -> xmax > leftDraggingBoundary && interval -> xmax < rightDraggingBoundary) {
							if (interval -> xmax < our anchorTime)
								NUMscale (& interval -> xmax, our leftDraggingBoundary, our anchorTime, our leftDraggingBoundary, x_world);
							else
								NUMscale (& interval -> xmax, our anchorTime, our rightDraggingBoundary, x_world, our rightDraggingBoundary);
						}
					}
				} else {
					for (integer ipoint = 1; ipoint <= textTier -> points.size; ipoint ++) {
						const mutableTextPoint point = textTier -> points.at [ipoint];
						if (point -> number > leftDraggingBoundary && point -> number < rightDraggingBoundary) {
							if (point -> number < our anchorTime)
								NUMscale (& point -> number, our leftDraggingBoundary, our anchorTime, our leftDraggingBoundary, x_world);
							else
								NUMscale (& point -> number, our anchorTime, our rightDraggingBoundary, x_world, our rightDraggingBoundary);
						}
					}
				}
			} else {
				if (intervalTier) {
					const integer numberOfIntervals = intervalTier -> intervals.size;
					for (integer ibound = 2; ibound <= numberOfIntervals; ibound ++) {
						TextInterval left = intervalTier -> intervals.at [ibound - 1], right = intervalTier -> intervals.at [ibound];
						if (left -> xmax == our anchorTime) {   // boundary dragged?
							left -> xmax = right -> xmin = x_world;   // move boundary to drop site
							break;
						}
					}
				} else {
					Melder_assert (isdefined (our anchorTime));
					const integer iDraggedPoint = AnyTier_hasPoint (textTier->asAnyTier(), our anchorTime);
					if (iDraggedPoint) {
						Melder_assert (isdefined (x_world));
						const integer dropSiteHasPoint = AnyTier_hasPoint (textTier->asAnyTier(), x_world);
						if (dropSiteHasPoint != 0) {
							Melder_warning (U"Cannot drop point on an existing point.");
						} else {
							const TextPoint point = textTier -> points.at [iDraggedPoint];
							/*
								Move point to drop site. May have passed another point.
							*/
							autoTextPoint newPoint = Data_copy (point);
							newPoint -> number = x_world;   // move point to drop site
							textTier -> points. removeItem (iDraggedPoint);
							textTier -> points. addItem_move (newPoint.move());
						}
					}
				}
			}
		}

		/*
			Select the drop site.
		*/
		our setSelection (x_world, x_world);
		our draggingTime = undefined;
		our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
		our anchorTime = undefined;
		our clickedLeftBoundary = 0;
		FunctionArea_broadcastDataChanged (this);
	}
	return FunctionEditor_UPDATE_NEEDED;
}


#pragma mark - TextGridArea File/Save

static void menu_cb_SaveWholeTextGridAsTextFile (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM_SAVE (U"Save whole TextGrid as text file", nullptr)
		Melder_sprint (defaultName,300, my textGrid() -> name.get(), U".TextGrid");
	EDITOR_DO_SAVE
		Data_writeToTextFile (my textGrid(), file);
	EDITOR_END
}
void structTextGridArea :: v_createMenuItems_save (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"- Save TextGrid to disk:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu,
		U"Save whole TextGrid as text file... || Save TextGrid as text file... || Write TextGrid to text file...",
		'S' | GuiMenu_DEPTH_1, menu_cb_SaveWholeTextGridAsTextFile, this
	);
}


#pragma mark - TextGridArea Edit menu

#ifndef macintosh
static void menu_cb_Cut (TextGridArea me, EDITOR_ARGS) {
	GuiText_cut (my functionEditor() -> textArea);
}
static void menu_cb_Copy (TextGridArea me, EDITOR_ARGS) {
	GuiText_copy (my functionEditor() -> textArea);
}
static void menu_cb_Paste (TextGridArea me, EDITOR_ARGS) {
	GuiText_paste (my functionEditor() -> textArea);
}
static void menu_cb_Erase (TextGridArea me, EDITOR_ARGS) {
	GuiText_remove (my functionEditor() -> textArea);
}
#endif

static void menu_cb_ConvertToBackslashTrigraphs (TextGridArea me, EDITOR_ARGS) {
	FunctionArea_save (me, U"Convert to Backslash Trigraphs");
	TextGrid_convertToBackslashTrigraphs (my textGrid());
	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_ConvertToUnicode (TextGridArea me, EDITOR_ARGS) {
	FunctionArea_save (me, U"Convert to Unicode");
	TextGrid_convertToUnicode (my textGrid());
	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
	FunctionArea_broadcastDataChanged (me);
}
static void findInTier (TextGridArea me) {
	checkTierSelection (me, U"find a text");
	Function anyTier = my textGrid() -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		integer iinterval = IntervalTier_timeToIndex (tier, my startSelection()) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			conststring32 text = interval -> text.get();
			if (text) {
				const char32 *position = str32str (text, my findString.get());
				if (position) {
					my setSelection (interval -> xmin, interval -> xmax);
					Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToNewSelection()
					FunctionEditor_scrollToNewSelection (my functionEditor(), my startSelection());
					if (my editable ())
						GuiText_setSelection (my functionEditor() -> textArea,
								position - text, position - text + Melder_length (my findString.get()));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		integer ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection()) + 1;
		while (ipoint <= tier -> points.size) {
			const TextPoint point = tier->points.at [ipoint];
			conststring32 text = point -> mark.get();
			if (text) {
				const char32 *const position = str32str (text, my findString.get());
				if (position) {
					my setSelection (point -> number, point -> number);
					Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToNewSelection()
					FunctionEditor_scrollToNewSelection (my functionEditor(), point -> number);
					if (my editable ())
						GuiText_setSelection (my functionEditor() -> textArea,
								position - text, position - text + Melder_length (my findString.get()));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}
static void do_find (TextGridArea me) {
	if (my findString) {
		if (my editable ()) {
			integer left, right;
			autostring32 label = GuiText_getStringAndSelectionPosition (my functionEditor() -> textArea, & left, & right);
			const char32 *const position = str32str (& label [right], my findString.get());   // CRLF BUG?
			if (position)
				GuiText_setSelection (my functionEditor() -> textArea,
						position - label.get(), position - label.get() + Melder_length (my findString.get()));
			else
				findInTier (me);
		} else
			findInTier (me);
	}
}

static void menu_cb_Find (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Find text", nullptr)
		TEXTFIELD (findString, U"Text", U"", 3)
	EDITOR_OK
	EDITOR_DO
		my findString = Melder_dup (findString);
		do_find (me);
	EDITOR_END
}
static void menu_cb_FindAgain (TextGridArea me, EDITOR_ARGS) {
	do_find (me);
}
void structTextGridArea :: v_createMenuItems_edit (EditorMenu menu) {
	#ifndef macintosh
		if (our editable()) {
			FunctionAreaMenu_addCommand (menu, U"-- cut copy paste --", 0, nullptr, this);
			FunctionAreaMenu_addCommand (menu, U"Cut text", 'X', menu_cb_Cut, this);
			FunctionAreaMenu_addCommand (menu, U"Cut", GuiMenu_HIDDEN, menu_cb_Cut, this);
			FunctionAreaMenu_addCommand (menu, U"Copy text", 'C', menu_cb_Copy, this);
			FunctionAreaMenu_addCommand (menu, U"Copy", GuiMenu_HIDDEN, menu_cb_Copy, this);
			FunctionAreaMenu_addCommand (menu, U"Paste text", 'V', menu_cb_Paste, this);
			FunctionAreaMenu_addCommand (menu, U"Paste", GuiMenu_HIDDEN, menu_cb_Paste, this);
			FunctionAreaMenu_addCommand (menu, U"Erase text", 0, menu_cb_Erase, this);
			FunctionAreaMenu_addCommand (menu, U"Erase", GuiMenu_HIDDEN, menu_cb_Erase, this);
		}
	#endif
	FunctionAreaMenu_addCommand (menu, U"-- search --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Find...", 'F', menu_cb_Find, this);
	FunctionAreaMenu_addCommand (menu, U"Find again", 'G', menu_cb_FindAgain, this);
}


#pragma mark - TextGridArea Query

static void QUERY_DATA_FOR_REAL__GetStartingPointOfInterval (TextGridArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		checkTierSelection (me, U"query the starting point of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection());
		const double result = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmin );
	QUERY_DATA_FOR_REAL_END (U" seconds")
}

static void QUERY_DATA_FOR_REAL__GetEndPointOfInterval (TextGridArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		checkTierSelection (me, U"query the end point of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection());
		const double result = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmax );
	QUERY_DATA_FOR_REAL_END (U" seconds")
}

static void QUERY_DATA_FOR_STRING__GetLabelOfInterval (TextGridArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_STRING
		checkTierSelection (me, U"query the label of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection());
		const conststring32 result = ( iinterval < 1 || iinterval > tier -> intervals.size ? U"" :
				tier -> intervals.at [iinterval] -> text.get() );
	QUERY_DATA_FOR_STRING_END
}


#pragma mark - TextGridArea changing

static void gui_text_cb_changed (TextGridArea me, GuiTextEvent /* event */) {
	trace (my suppressRedraw);
	if (my suppressRedraw)
		return;   // prevent infinite loop if 'draw' method or Editor_broadcastChange calls GuiText_setString
	if (my selectedTier) {
		autostring32 text = GuiText_getString (my functionEditor() -> textArea);
		IntervalTier intervalTier;
		TextTier textTier;
		AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer selectedInterval = getSelectedInterval (me);
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals.at [selectedInterval];
				TextInterval_setText (interval, text.get());
				my suppressTextCursorJump = true;
				FunctionArea_broadcastDataChanged (me);
				my suppressTextCursorJump = false;
			}
		} else {
			const integer selectedPoint = getSelectedPoint (me);
			if (selectedPoint) {
				TextPoint point = textTier -> points.at [selectedPoint];
				point -> mark. reset();
				if (Melder_findInk (text.get()))   // any visible characters?
					point -> mark = Melder_dup_f (text.get());
				my suppressTextCursorJump = true;
				FunctionArea_broadcastDataChanged (me);
				my suppressTextCursorJump = false;
			}
		}
	}
}


#pragma mark - TextGridArea TextGrid/Prefs

static void menu_cb_TextGridSettings (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"TextGrid settings", nullptr)
		POSITIVE (fontSize, U"Font size (points)", my default_fontSize())
		OPTIONMENU_ENUM (kGraphics_horizontalAlignment, textAlignmentInIntervals,
				U"Text alignment in intervals", kGraphics_horizontalAlignment::DEFAULT)
		OPTIONMENU (useTextStyles, U"The symbols %#_^ in labels", my default_useTextStyles() + 1)
			OPTION (U"are shown as typed")
			OPTION (U"mean italic/bold/sub/super")
		OPTIONMENU_VARIABLE (shiftDragMultiple)
		if (my editable()) {
			OPTIONMENU_FIELD (shiftDragMultiple, U"With the shift key, you drag", my default_shiftDragMultiple() + 1)
				OPTION (U"a single boundary")
				OPTION (U"multiple boundaries")
		}
		OPTIONMENU_ENUM (kTextGridArea_showNumberOf, showNumberOf,
				U"Show number of", kTextGridArea_showNumberOf::DEFAULT)
		OPTIONMENU_ENUM (kMelder_string, paintIntervalsGreenWhoseLabel,
				U"Paint intervals green whose label...", kMelder_string::DEFAULT)
		SENTENCE (theText, U"...the text", my default_greenString())
	EDITOR_OK
		SET_OPTION (useTextStyles, my instancePref_useTextStyles() + 1)
		SET_REAL (fontSize, my instancePref_fontSize())
		SET_ENUM (textAlignmentInIntervals, kGraphics_horizontalAlignment, my instancePref_alignment())
		if (my editable())
			SET_OPTION (shiftDragMultiple, my instancePref_shiftDragMultiple() + 1)
		SET_ENUM (showNumberOf, kTextGridArea_showNumberOf, my instancePref_showNumberOf())
		SET_ENUM (paintIntervalsGreenWhoseLabel, kMelder_string, my instancePref_greenMethod())
		SET_STRING (theText, my instancePref_greenString())
	EDITOR_DO
		my setInstancePref_useTextStyles (useTextStyles - 1);
		my setInstancePref_fontSize (fontSize);
		my setInstancePref_alignment (textAlignmentInIntervals);
		if (my editable())
			my setInstancePref_shiftDragMultiple (shiftDragMultiple - 1);
		my setInstancePref_showNumberOf (showNumberOf);
		my setInstancePref_greenMethod (paintIntervalsGreenWhoseLabel);
		my setInstancePref_greenString (theText);
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}


#pragma mark - TextGridArea TextGrid/Select

static void do_selectAdjacentTier (TextGridArea me, bool previous) {
	const integer n = my textGrid() -> tiers->size;
	if (n >= 2) {
		my selectedTier = ( previous ?
				my selectedTier > 1 ? my selectedTier - 1 : n :
				my selectedTier < n ? my selectedTier + 1 : 1 );
		double startInterval, endInterval;
		timeToInterval (me, my startSelection(), my selectedTier, & startInterval, & endInterval);
		my setSelection (startInterval, endInterval);
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (my functionEditor());
	}
}
static void menu_cb_SelectPreviousTier (TextGridArea me, EDITOR_ARGS) {
	do_selectAdjacentTier (me, true);
}
static void menu_cb_SelectNextTier (TextGridArea me, EDITOR_ARGS) {
	do_selectAdjacentTier (me, false);
}
static void do_selectAdjacentInterval (TextGridArea me, bool previous, bool shift) {
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > my textGrid() -> tiers->size)
		return;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		const integer n = intervalTier -> intervals.size;
		if (n >= 2) {
			integer iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection());
			if (shift) {
				const integer binterval = IntervalTier_timeToIndex (intervalTier, my startSelection());
				integer einterval = IntervalTier_timeToIndex (intervalTier, my endSelection());
				if (my endSelection() == intervalTier -> xmax)
					einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					const TextInterval interval = intervalTier -> intervals.at [iinterval];
					my setSelection (interval -> xmin, interval -> xmax);
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							const TextInterval interval = intervalTier -> intervals.at [einterval - 1];
							my setSelection (my startSelection(), interval -> xmin);
						}
					} else if (binterval > 1) {
						const TextInterval interval = intervalTier -> intervals.at [binterval - 1];
						my setSelection (interval -> xmin, my endSelection());
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							const TextInterval interval = intervalTier -> intervals.at [binterval];
							my setSelection (interval -> xmax, my endSelection());
						}
					} else if (einterval <= n) {
						const TextInterval interval = intervalTier -> intervals.at [einterval];
						my setSelection (my startSelection(), interval -> xmax);
					}
				}
			} else {
				iinterval = ( previous ?
						iinterval > 1 ? iinterval - 1 : n :
						iinterval < n ? iinterval + 1 : 1 );
				const TextInterval interval = intervalTier -> intervals.at [iinterval];
				my setSelection (interval -> xmin, interval -> xmax);
			}
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToNewSelection()
			FunctionEditor_scrollToNewSelection (my functionEditor(), iinterval == n ? my startSelection() : iinterval == 1 ? my endSelection() : 0.5 * (my startSelection() + my endSelection()));
		}
	} else {
		const integer n = textTier -> points.size;
		if (n >= 2) {
			integer ipoint = AnyTier_timeToHighIndex (textTier->asAnyTier(), my startSelection());
			ipoint = ( previous ?
					ipoint > 1 ? ipoint - 1 : n :
					ipoint < n ? ipoint + 1 : 1 );
			const TextPoint point = textTier -> points.at [ipoint];
			my setSelection (point -> number, point -> number);
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToNewSelection()
			FunctionEditor_scrollToNewSelection (my functionEditor(), my startSelection());
		}
	}
}
static void menu_cb_SelectPreviousInterval (TextGridArea me, EDITOR_ARGS) {
	do_selectAdjacentInterval (me, true, false);
}
static void menu_cb_SelectNextInterval (TextGridArea me, EDITOR_ARGS) {
	do_selectAdjacentInterval (me, false, false);
}
static void menu_cb_ExtendSelectPreviousInterval (TextGridArea me, EDITOR_ARGS) {
	do_selectAdjacentInterval (me, true, true);
}
static void menu_cb_ExtendSelectNextInterval (TextGridArea me, EDITOR_ARGS) {
	do_selectAdjacentInterval (me, false, true);
}


#pragma mark - TextGridArea Draw

static void menu_cb_DrawVisibleTextGrid (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		DataGui_openPraatPicture (me);
		TextGrid_Sound_draw (my textGrid(), nullptr, my pictureGraphics(),
				my startWindow(), my endWindow(), true, my instancePref_useTextStyles(), garnish);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}
static void menu_cb_DrawVisibleSoundAndTextGrid (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible sound and TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		DataGui_openPraatPicture (me);
		{// scope
			autoSound sound = (
				my borrowedSoundArea -> longSound() ?
					LongSound_extractPart (my borrowedSoundArea -> longSound(),
							my startWindow(), my endWindow(), true)
				:	Sound_extractPart (my borrowedSoundArea -> sound(),
							my startWindow(), my endWindow(), kSound_windowShape::RECTANGULAR, 1.0, true)
			);
			TextGrid_Sound_draw (my textGrid(), sound.get(), my pictureGraphics(),
					my startWindow(), my endWindow(), true, my instancePref_useTextStyles(), garnish);
		}
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}
static void menu_cb_DrawTextGridAndPitch (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw TextGrid and Pitch separately", nullptr)
		my v_form_pictureWindow (cmd);
		COMMENT (U"TextGrid:")
		BOOLEAN (showBoundariesAndPoints, U"Show boundaries and points", my default_picture_showBoundaries ());
		COMMENT (U"Pitch:")
		BOOLEAN (speckle, U"Speckle", my default_picture_pitch_speckle ());
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (showBoundariesAndPoints, my classPref_picture_showBoundaries())
		SET_BOOLEAN (speckle, my classPref_picture_pitch_speckle())
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my setClassPref_picture_showBoundaries (showBoundariesAndPoints);   // set prefs even if analyses are missing (it would be annoying not to)
		my setClassPref_picture_pitch_speckle (speckle);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		SoundAnalysisArea_haveVisiblePitch (my borrowedSoundAnalysisArea);
		DataGui_openPraatPicture (me);
		const kPitch_unit borrowedPitchUnit = my borrowedSoundAnalysisArea -> dynamic_instancePref_pitch_unit();
		const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my borrowedSoundAnalysisArea -> d_pitch.get(),
				my borrowedSoundAnalysisArea -> dynamic_instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) borrowedPitchUnit);
		const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my borrowedSoundAnalysisArea -> d_pitch.get(),
				my borrowedSoundAnalysisArea -> dynamic_instancePref_pitch_ceilingOrTop(), Pitch_LEVEL_FREQUENCY, (int) borrowedPitchUnit);
		const double pitchFloor_overt = Function_convertToNonlogarithmic (my borrowedSoundAnalysisArea -> d_pitch.get(),
				pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) borrowedPitchUnit);
		const double pitchCeiling_overt = Function_convertToNonlogarithmic (my borrowedSoundAnalysisArea -> d_pitch.get(),
				pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) borrowedPitchUnit);
		const double borrowedPitchViewFrom = my borrowedSoundAnalysisArea -> dynamic_instancePref_pitch_viewFrom();
		const double borrowedPitchViewTo = my borrowedSoundAnalysisArea -> dynamic_instancePref_pitch_viewTo();
		const double pitchViewFrom_overt = ( borrowedPitchViewFrom < borrowedPitchViewTo ? borrowedPitchViewFrom : pitchFloor_overt );
		const double pitchViewTo_overt = ( borrowedPitchViewFrom < borrowedPitchViewTo ? borrowedPitchViewTo : pitchCeiling_overt );
		TextGrid_Pitch_drawSeparately (my textGrid(), my borrowedSoundAnalysisArea -> d_pitch.get(), my pictureGraphics(), my startWindow(), my endWindow(),
			pitchViewFrom_overt, pitchViewTo_overt, showBoundariesAndPoints, my instancePref_useTextStyles(), garnish,
			speckle, borrowedPitchUnit
		);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}


#pragma mark - TextGridArea Extract

static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes (TextGridArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		if (my endSelection() <= my startSelection())
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection(), my endSelection(), true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero (TextGridArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		if (my endSelection() <= my startSelection())
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection(), my endSelection(), false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}


#pragma mark - TextGridArea Interval menu

static void menu_cb_AlignInterval (TextGridArea me, EDITOR_ARGS) {
	checkTierSelection (me, U"align words");
	const AnyTier tier = static_cast <AnyTier> (my textGrid() -> tiers->at [my selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Alignment works only for interval tiers, whereas tier ", my selectedTier, U" is a point tier.\nSelect an interval tier instead.");
	const integer intervalNumber = getSelectedInterval (me);
	if (! intervalNumber)
		Melder_throw (U"Select an interval first");
	if (! my instancePref_align_includeWords() && ! my instancePref_align_includePhonemes())
		Melder_throw (U"Nothing to be done.\nPlease switch on \"Include words\" and/or \"Include phonemes\" in the \"Alignment settings\".");
	{// scope
		const autoMelderProgressOff noprogress;
		FunctionArea_save (me, U"Align interval");
		TextGrid_anySound_alignInterval (my textGrid(), my borrowedSoundArea -> soundOrLongSound(), my selectedTier, intervalNumber,
				my instancePref_align_language(), my instancePref_align_includeWords(), my instancePref_align_includePhonemes());
	}
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_AlignmentSettings (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Alignment settings", nullptr)
		OPTIONMENU (language, U"Language", (int) NUMfindFirst (theSpeechSynthesizerLanguageNames, U"English (Great Britain)"))
		for (integer i = 1; i <= theSpeechSynthesizerLanguageNames.size; i ++) {
			OPTION (theSpeechSynthesizerLanguageNames [i]);
		}
		BOOLEAN (includeWords,    U"Include words",    my default_align_includeWords ())
		BOOLEAN (includePhonemes, U"Include phonemes", my default_align_includePhonemes ())
		BOOLEAN (allowSilences,   U"Allow silences",   my default_align_allowSilences ())
	EDITOR_OK
		int prefVoice = (int) NUMfindFirst (theSpeechSynthesizerLanguageNames, my instancePref_align_language());
		if (prefVoice == 0)
			prefVoice = (int) NUMfindFirst (theSpeechSynthesizerLanguageNames, U"English (Great Britain)");
		SET_OPTION (language, prefVoice)
		SET_BOOLEAN (includeWords, my instancePref_align_includeWords())
		SET_BOOLEAN (includePhonemes, my instancePref_align_includePhonemes())
		SET_BOOLEAN (allowSilences, my instancePref_align_allowSilences())
	EDITOR_DO
		my setInstancePref_align_language (theSpeechSynthesizerLanguageNames [language]);
		my setInstancePref_align_includeWords (includeWords);
		my setInstancePref_align_includePhonemes (includePhonemes);
		my setInstancePref_align_allowSilences (allowSilences);
	EDITOR_END
}

static void menu_cb_TranscribeInterval (TextGridArea me, EDITOR_ARGS) {
	checkTierSelection (me, U"transcribe interval");
	const AnyTier tier = static_cast <AnyTier> (my textGrid() -> tiers->at [my selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Transcription works only for interval tiers, whereas tier ", my selectedTier, U" is a point tier.\nSelect an interval tier instead.");
	const integer intervalNumber = getSelectedInterval (me);
	if (! intervalNumber)
		Melder_throw (U"Select an interval first");
	{// scope
		const autoMelderProgressOff noprogress;
		FunctionArea_save (me, U"Transcribe interval");
		TextGrid_Sound_transcribeInterval (my textGrid(), my borrowedSoundArea -> sound(), my selectedTier, intervalNumber,
				my instancePref_transcribe_model(), my instancePref_transcribe_language(),
				my instancePref_transcribe_includeWords(), my instancePref_transcribe_useVad(),
				my instancePref_vad_speechThreshold(), my instancePref_vad_minNonSpeech(),
				my instancePref_vad_minSpeech(),my instancePref_vad_speechPadding(),
				my instancePref_transcribe_diarize(),
				my instancePref_diarize_maxNumSpeakers(), my instancePref_diarize_allowOverlap(),
				my instancePref_diarize_clusterThreshold(), my instancePref_diarize_segmentationStep());
	}
	FunctionArea_broadcastDataChanged (me);
}

static void menu_cb_TranscriptionSettings (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Transcription settings", U"transcription with whisper.cpp")
		HEADING (U"Transcription...")
		LISTNUMSTR (modelIndex, modelName, U"Whisper model", constSTRVEC(), 1)
		OPTIONMENU (language, U"Language", (int) NUMfindFirst (theSpeechRecognizerLanguageNames(), TranscriptionDefaults::languageName))
		for (integer i = 1; i <= theSpeechRecognizerLanguageNames().size; i ++) {
			OPTION (theSpeechRecognizerLanguageNames() [i]);
		}
		BOOLEAN (includeWords, U"Include words", my default_transcribe_includeWords())
		HEADING (U"Non-speech detection...")
		BOOLEAN (useVad, U"Detect non-speech", my default_transcribe_useVad())
		NONNEGATIVE (speechProbabilityThreshold, U"Speech probability threshold (0-1)", my default_vad_speechThreshold())
		NONNEGATIVE (minNonSpeechDuration, U"Min. gap between speech segments (s)", my default_vad_minNonSpeech())
		NONNEGATIVE (minSpeechDuration, U"Min. speech segment (s)", my default_vad_minSpeech())
		NONNEGATIVE (speechPad, U"Padding around speech segments (s)", my default_vad_speechPadding())
		HEADING (U"Diarization...")
		BOOLEAN (includeDiarization, U"Include diarization", my default_transcribe_diarize())
		NATURAL (maxNumSpeakers, U"Max. number of speakers (≥ 2)", DiarizationDefaults::maxNumSpeakers)
		BOOLEAN (allowSpeakersOverlap, U"Allow speakers to overlap", DiarizationDefaults::allowOverlap)
		POSITIVE (clusterThreshold, U"Clustering threshold (0-2)", DiarizationDefaults::clusterThreshold)
		POSITIVE (segmentationStep, U"Segmentation step (0-1)", DiarizationDefaults::segmentationStep)
	EDITOR_OK
		static autoSTRVEC modelNames;
		modelNames = copy_STRVEC (theCurrentSpeechRecognizerModelNames());   // cannot be called twice in the same scope

		Melder_require (modelNames.size > 0,
			U"Found no Whisper-cpp models to do speech recognition with.\n"
			U"You can install them into the subfolders “whispercpp” of the folder “models” in the Praat preferences folder."
		);

		SET_BOOLEAN (includeWords, my instancePref_transcribe_includeWords())
		SET_BOOLEAN (includeDiarization, my instancePref_transcribe_diarize())
		SET_BOOLEAN (useVad, my instancePref_transcribe_useVad())
		SET_REAL (speechProbabilityThreshold, my instancePref_vad_speechThreshold())
		SET_REAL (minNonSpeechDuration, my instancePref_vad_minNonSpeech())
		SET_REAL (minSpeechDuration, my instancePref_vad_minSpeech())
		SET_REAL (speechPad, my instancePref_vad_speechPadding())
		SET_INTEGER (maxNumSpeakers, my instancePref_diarize_maxNumSpeakers())
		SET_BOOLEAN (allowSpeakersOverlap, my instancePref_diarize_allowOverlap())
		SET_REAL (clusterThreshold, my instancePref_diarize_clusterThreshold())
		SET_REAL (segmentationStep, my instancePref_diarize_segmentationStep())

		integer prefModel = NUMfindFirst (modelNames.get (), my instancePref_transcribe_model());
		if (prefModel == 0)
			prefModel = NUMfindFirst (modelNames.get (), TranscriptionDefaults::modelName);
		SET_INTEGER (modelIndex, prefModel)
		SET_LIST (modelIndex, modelName, modelNames.get (), prefModel)

		integer prefLanguage = NUMfindFirst (theSpeechRecognizerLanguageNames(), my instancePref_transcribe_language());
		if (prefLanguage == 0)
			prefLanguage = NUMfindFirst (theSpeechRecognizerLanguageNames(), TranscriptionDefaults::languageName);
		SET_OPTION (language, prefLanguage)
	EDITOR_DO
		my setInstancePref_transcribe_model (modelName);
		my setInstancePref_transcribe_language (theSpeechRecognizerLanguageNames() [language]);
		my setInstancePref_transcribe_includeWords (includeWords);
		my setInstancePref_transcribe_diarize (includeDiarization);
		my setInstancePref_transcribe_useVad (useVad);
		my setInstancePref_vad_speechThreshold (speechProbabilityThreshold);
		my setInstancePref_vad_minNonSpeech (minNonSpeechDuration);
		my setInstancePref_vad_minSpeech (minSpeechDuration);
		my setInstancePref_vad_speechPadding (speechPad);
		my setInstancePref_diarize_maxNumSpeakers (maxNumSpeakers);
		my setInstancePref_diarize_allowOverlap (allowSpeakersOverlap);
		my setInstancePref_diarize_clusterThreshold (clusterThreshold);
		my setInstancePref_diarize_segmentationStep (segmentationStep);
	EDITOR_END
}

static void menu_cb_DiarizeInterval (TextGridArea me, EDITOR_ARGS) {
	checkTierSelection (me, U"diarize interval");
	const AnyTier tier = static_cast <AnyTier> (my textGrid() -> tiers->at [my selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Diarization works only for interval tiers, whereas tier ", my selectedTier, U" is a point tier.\nSelect an interval tier instead.");
	const integer intervalNumber = getSelectedInterval (me);
	if (! intervalNumber)
		Melder_throw (U"Select an interval first");
	{// scope
		const autoMelderProgressOff noprogress;
		FunctionArea_save (me, U"Diarize interval");
		TextGrid_Sound_diarizeInterval (my textGrid(), my borrowedSoundArea -> sound(), my selectedTier, intervalNumber,
				my instancePref_diarize_maxNumSpeakers(), my instancePref_diarize_allowOverlap(),
				my instancePref_diarize_nonSpeechLabel(), my instancePref_diarize_speechLabel(),
				my instancePref_diarize_clusterThreshold(), my instancePref_diarize_segmentationStep()
		);
	}
	FunctionArea_broadcastDataChanged (me);
}

static void menu_cb_DiarizationSettings (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Diarization settings", U"speaker diarization with adapted pyannote.audio")
		NATURAL (maxNumSpeakers, U"Max. number of speakers (≥ 2)", DiarizationDefaults::maxNumSpeakers)
		BOOLEAN (allowSpeakersOverlap, U"Allow speakers to overlap", DiarizationDefaults::allowOverlap)
		WORD (nonSpeechLabel, U"Non-speech interval label", DiarizationDefaults::nonSpeechLabel)
		WORD (speechLabel, U"Speech interval label", DiarizationDefaults::speechLabel)
		POSITIVE (clusterThreshold, U"Clustering threshold (0-2)", DiarizationDefaults::clusterThreshold)
		POSITIVE (segmentationStep, U"Segmentation step (0-1)", DiarizationDefaults::segmentationStep)
	EDITOR_OK
		SET_INTEGER (maxNumSpeakers, my instancePref_diarize_maxNumSpeakers())
		SET_BOOLEAN (allowSpeakersOverlap, my instancePref_diarize_allowOverlap())
		SET_STRING (nonSpeechLabel, my instancePref_diarize_nonSpeechLabel())
		SET_STRING (speechLabel, my instancePref_diarize_speechLabel())
		SET_REAL (clusterThreshold, my instancePref_diarize_clusterThreshold())
		SET_REAL (segmentationStep, my instancePref_diarize_segmentationStep())
	EDITOR_DO
		my setInstancePref_diarize_maxNumSpeakers (maxNumSpeakers);
		my setInstancePref_diarize_allowOverlap (allowSpeakersOverlap);
		my setInstancePref_diarize_nonSpeechLabel (nonSpeechLabel);
		my setInstancePref_diarize_speechLabel (speechLabel);
		my setInstancePref_diarize_clusterThreshold (clusterThreshold);
		my setInstancePref_diarize_segmentationStep (segmentationStep);
	EDITOR_END
}

static void do_insertIntervalOnTier (TextGridArea me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my startSelection(),
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my endSelection(),
			true
		);
		my selectedTier = itier;
		FunctionArea_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Interval not inserted.");
	}
}
static void menu_cb_InsertIntervalOnTier1 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 1); }
static void menu_cb_InsertIntervalOnTier2 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 2); }
static void menu_cb_InsertIntervalOnTier3 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 3); }
static void menu_cb_InsertIntervalOnTier4 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 4); }
static void menu_cb_InsertIntervalOnTier5 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 5); }
static void menu_cb_InsertIntervalOnTier6 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 6); }
static void menu_cb_InsertIntervalOnTier7 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 7); }
static void menu_cb_InsertIntervalOnTier8 (TextGridArea me, EDITOR_ARGS) { do_insertIntervalOnTier (me, 8); }

#pragma mark - TextGridArea Boundary menu

static void menu_cb_RemovePointOrBoundary (TextGridArea me, EDITOR_ARGS) {
	checkTierSelection (me, U"remove a point or boundary");
	const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (selectedLeftBoundary == 0)
			Melder_throw (U"To remove a boundary, first click on it.");

		FunctionArea_save (me, U"Remove boundary");
		IntervalTier_removeLeftBoundary (tier, selectedLeftBoundary);
	} else {
		const TextTier tier = (TextTier) anyTier;
		const integer selectedPoint = getSelectedPoint (me);
		if (selectedPoint == 0)
			Melder_throw (U"To remove a point, first click on it.");

		FunctionArea_save (me, U"Remove point");
		tier -> points. removeItem (selectedPoint);
	}
	FunctionArea_broadcastDataChanged (me);
}
static void do_movePointOrBoundary (TextGridArea me, int where) {
	if (where == 0 && ! (my borrowedSoundArea && my borrowedSoundArea -> sound()))
		return;
	checkTierSelection (me, U"move a point or boundary");
	const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		static const conststring32 boundarySaveText [3] { U"Move boundary to zero crossing", U"Move boundary to B", U"Move boundary to E" };
		const integer selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (selectedLeftBoundary == 0)
			Melder_throw (U"To move a boundary, first click on it.");
		const TextInterval left = tier -> intervals.at [selectedLeftBoundary - 1];
		const TextInterval right = tier -> intervals.at [selectedLeftBoundary];
		const double position = ( where == 1 ? my startSelection() : where == 2 ? my endSelection() :
				Sound_getNearestZeroCrossing (my borrowedSoundArea -> sound(), left -> xmax, 1) );   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");
		if (position <= left -> xmin || position >= right -> xmax)
			Melder_throw (U"Cannot move a boundary past its neighbour.");

		FunctionArea_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = position;
		my setSelection (position, position);
	} else {
		TextTier tier = (TextTier) anyTier;
		static const conststring32 pointSaveText [3] { U"Move point to zero crossing", U"Move point to B", U"Move point to E" };
		const integer selectedPoint = getSelectedPoint (me);
		if (selectedPoint == 0)
			Melder_throw (U"To move a point, first click on it.");
		const TextPoint point = tier -> points.at [selectedPoint];
		const double position = ( where == 1 ? my startSelection() : where == 2 ? my endSelection() :
				Sound_getNearestZeroCrossing (my borrowedSoundArea -> sound(), point -> number, 1) );   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");

		FunctionArea_save (me, pointSaveText [where]);

		point -> number = position;
		my setSelection (position, position);
	}
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_MoveToB (TextGridArea me, EDITOR_ARGS) {
	do_movePointOrBoundary (me, 1);
}
static void menu_cb_MoveToE (TextGridArea me, EDITOR_ARGS) {
	do_movePointOrBoundary (me, 2);
}
static void menu_cb_MoveToZero (TextGridArea me, EDITOR_ARGS) {
	do_movePointOrBoundary (me, 0);
}
static void do_insertOnTier (TextGridArea me, integer itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my startSelection(),
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my endSelection(),
			false
		);
		my selectedTier = itier;
		FunctionArea_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Boundary or point not inserted.");
	}
}
static void menu_cb_InsertOnSelectedTier (TextGridArea me, EDITOR_ARGS) {
	do_insertOnTier (me, my selectedTier);
}
static void menu_cb_InsertOnTier1 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 1); }
static void menu_cb_InsertOnTier2 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 2); }
static void menu_cb_InsertOnTier3 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 3); }
static void menu_cb_InsertOnTier4 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 4); }
static void menu_cb_InsertOnTier5 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 5); }
static void menu_cb_InsertOnTier6 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 6); }
static void menu_cb_InsertOnTier7 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 7); }
static void menu_cb_InsertOnTier8 (TextGridArea me, EDITOR_ARGS) { do_insertOnTier (me, 8); }

static void menu_cb_InsertOnAllTiers (TextGridArea me, EDITOR_ARGS) {
	const integer saveTier = my selectedTier;
	for (integer itier = 1; itier <= my textGrid() -> tiers->size; itier ++)
		do_insertOnTier (me, itier);
	my selectedTier = saveTier;   // only if everything went right; otherwise, the tier where something went wrong will stand selected
}


#pragma mark - TextGridArea Tier menu

static void menu_cb_RenameTier (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Rename tier", nullptr)
		SENTENCE (newName, U"New name", U"");
	EDITOR_OK
		checkTierSelection (me, U"rename a tier");
		const Daata tier = my textGrid() -> tiers->at [my selectedTier];
		SET_STRING (newName, tier -> name ? tier -> name.get() : U"")
	EDITOR_DO
		checkTierSelection (me, U"rename a tier");
		const Function tier = my textGrid() -> tiers->at [my selectedTier];

		FunctionArea_save (me, U"Rename tier");

		Thing_setName (tier, newName);

		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}
static void CONVERT_DATA_TO_ONE__PublishTier (TextGridArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		checkTierSelection (me, U"publish a tier");
		const Function tier = my textGrid() -> tiers->at [my selectedTier];
		autoTextGrid result = TextGrid_createWithoutTiers (1e308, -1e308);
		TextGrid_addTier_copy (result.get(), tier);
	CONVERT_DATA_TO_ONE_END (tier -> name.get())
}
static void menu_cb_RemoveAllTextFromTier (TextGridArea me, EDITOR_ARGS) {
	checkTierSelection (me, U"remove all text from a tier");
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);

	FunctionArea_save (me, U"Remove text from tier");
	if (intervalTier)
		IntervalTier_removeText (intervalTier);
	else
		TextTier_removeText (textTier);

	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_RemoveTier (TextGridArea me, EDITOR_ARGS) {
	if (my textGrid() -> tiers->size <= 1)
		Melder_throw (U"Sorry, I refuse to remove the last tier.");
	checkTierSelection (me, U"remove a tier");

	FunctionArea_save (me, U"Remove tier");
	my textGrid() -> tiers -> removeItem (my selectedTier);

	//my textGridArea -> selectedTier = 1; TRY OUT 2022-07-23
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_AddIntervalTier (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add interval tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"")
	EDITOR_OK
		SET_INTEGER_AS_STRING (position, Melder_cat (my textGrid() -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (name, U"")
	EDITOR_DO
		{// scope
			autoIntervalTier tier = IntervalTier_create (my textGrid() -> xmin, my textGrid() -> xmax);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (tier.get(), name);

			FunctionArea_save (me, U"Add interval tier");
			my textGrid() -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my selectedTier = position;
		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}
static void menu_cb_AddPointTier (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add point tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"");
	EDITOR_OK
		SET_INTEGER_AS_STRING (position, Melder_cat (my textGrid() -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (name, U"")
	EDITOR_DO
		{// scope
			autoTextTier tier = TextTier_create (my textGrid() -> xmin, my textGrid() -> xmax);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (tier.get(), name);

			FunctionArea_save (me, U"Add point tier");
			my textGrid() -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my selectedTier = position;
		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}
static void menu_cb_DuplicateTier (TextGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Duplicate tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"")
	EDITOR_OK
		if (my selectedTier != 0) {
			SET_INTEGER (position, my selectedTier + 1)
			SET_STRING (name, my textGrid() -> tiers->at [my selectedTier] -> name.get())
		}
	EDITOR_DO
		checkTierSelection (me, U"duplicate a tier");
		const Function tier = my textGrid() -> tiers->at [my selectedTier];
		{// scope
			autoFunction newTier = Data_copy (tier);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (newTier.get(), name);

			FunctionArea_save (me, U"Duplicate tier");
			my textGrid() -> tiers -> addItemAtPosition_move (newTier.move(), position);
		}

		my selectedTier = position;
		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}


#pragma mark - TextGridArea Spell menu

static void checkSpellingInTier (TextGridArea me) {
	checkTierSelection (me, U"check spelling");
	const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		integer iinterval = IntervalTier_timeToIndex (tier, my startSelection()) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			conststring32 text = interval -> text.get();
			if (text) {
				integer position = 0;
				conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my setSelection (interval -> xmin, interval -> xmax);
					Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToNewSelection()
					FunctionEditor_scrollToNewSelection (my functionEditor(), my startSelection());
					GuiText_setSelection (my functionEditor() -> textArea,
							position, position + Melder_length (notAllowed));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		const TextTier tier = (TextTier) anyTier;
		integer ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection()) + 1;
		while (ipoint <= tier -> points.size) {
			TextPoint point = tier -> points.at [ipoint];
			conststring32 text = point -> mark.get();
			if (text) {
				integer position = 0;
				conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my setSelection (point -> number, point -> number);
					Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToNewSelection()
					FunctionEditor_scrollToNewSelection (my functionEditor(), point -> number);
					GuiText_setSelection (my functionEditor() -> textArea, position, position + Melder_length (notAllowed));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}
static void menu_cb_CheckSpelling (TextGridArea me, EDITOR_ARGS) {
	if (my spellingChecker) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my functionEditor() -> textArea, & left, & right);
		integer position = right;
		conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.get(), & position);
		if (notAllowed)
			GuiText_setSelection (my functionEditor() -> textArea,
					position, position + Melder_length (notAllowed));
		else
			checkSpellingInTier (me);
	}
}
static void menu_cb_CheckSpellingInInterval (TextGridArea me, EDITOR_ARGS) {
	if (my spellingChecker) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my functionEditor() -> textArea, & left, & right);
		integer position = right;
		conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.get(), & position);
		if (notAllowed)
			GuiText_setSelection (my functionEditor() -> textArea,
					position, position + Melder_length (notAllowed));
	}
}
static void menu_cb_AddToUserDictionary (TextGridArea me, EDITOR_ARGS) {
	if (my spellingChecker) {
		const autostring32 word = GuiText_getSelection (my functionEditor() -> textArea);
		SpellingChecker_addNewWord (my spellingChecker, word.get());
		FunctionArea_broadcastDataChanged (me);
	}
}


#pragma mark - TextGridArea all menus

void structTextGridArea :: v_createMenus () {
	EditorMenu textGridMenu = Editor_addMenu (our functionEditor(), U"TextGrid", 0);
	FunctionAreaMenu_addCommand (textGridMenu, U"TextGrid settings... || TextGrid preferences...", 0,
			menu_cb_TextGridSettings, this);   // alternative deprecated 2023
	if (editable()) {
		FunctionAreaMenu_addCommand (textGridMenu, U"-- encoding --", 0, nullptr, this);
		FunctionAreaMenu_addCommand (textGridMenu, U"Convert entire TextGrid to backslash trigraphs", 0,
				menu_cb_ConvertToBackslashTrigraphs, this);
		FunctionAreaMenu_addCommand (textGridMenu, U"Convert entire TextGrid to Unicode", 0,
				menu_cb_ConvertToUnicode, this);
	}

	FunctionAreaMenu_addCommand (textGridMenu, U"- Select by TextGrid:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW | GuiMenu_DEPTH_1,
			menu_cb_SelectPreviousTier, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW | GuiMenu_DEPTH_1,
			menu_cb_SelectNextTier, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW | GuiMenu_DEPTH_1,
			menu_cb_SelectPreviousInterval, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW | GuiMenu_DEPTH_1,
			menu_cb_SelectNextInterval, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW | GuiMenu_DEPTH_1,
			menu_cb_ExtendSelectPreviousInterval, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW | GuiMenu_DEPTH_1,
			menu_cb_ExtendSelectNextInterval, this);

	FunctionAreaMenu_addCommand (textGridMenu, U"- Draw TextGrid to picture window:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (textGridMenu, U"Draw visible TextGrid...", 1,
			menu_cb_DrawVisibleTextGrid, this);
	if (our borrowedSoundArea)
		FunctionAreaMenu_addCommand (textGridMenu, U"Draw visible sound and TextGrid...", 1,
				menu_cb_DrawVisibleSoundAndTextGrid, this);
	if (our borrowedSoundAnalysisArea)
		FunctionAreaMenu_addCommand (textGridMenu, U"Draw visible pitch contour and TextGrid...", 1,
				menu_cb_DrawTextGridAndPitch, this);

	FunctionAreaMenu_addCommand (textGridMenu, U"- Extract TextGrid to objects window:", 0, nullptr, this);
	our extractSelectedTextGridPreserveTimesButton = FunctionAreaMenu_addCommand (textGridMenu,
		U"Extract selected TextGrid (preserve times)",
		1, CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes, this
	);
	our extractSelectedTextGridTimeFromZeroButton = FunctionAreaMenu_addCommand (textGridMenu,
		U"Extract selected TextGrid (time from 0)",
		1, CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero, this
	);

	EditorMenu intervalMenu = Editor_addMenu (our functionEditor(), U"Interval", 0);
	if (our editable()) {
		if (our borrowedSoundArea) {
			FunctionAreaMenu_addCommand (intervalMenu, U"Align interval", 'E',
					menu_cb_AlignInterval, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"Alignment settings...", 0,
					menu_cb_AlignmentSettings, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"-- after align --", 0, nullptr, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"Transcribe interval", 'T',
					menu_cb_TranscribeInterval, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"Transcription settings...", 0,
					menu_cb_TranscriptionSettings, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"-- after transcribe --", 0, nullptr, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"Diarize interval", 'D',
					menu_cb_DiarizeInterval, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"Diarization settings...", 0,
					menu_cb_DiarizationSettings, this);
			FunctionAreaMenu_addCommand (intervalMenu, U"-- after diarize --", 0, nullptr, this);
		}
		FunctionAreaMenu_addCommand (intervalMenu, U"New interval:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 1", GuiMenu_COMMAND | '1' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier1, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 2", GuiMenu_COMMAND | '2' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier2, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 3", GuiMenu_COMMAND | '3' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier3, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 4", GuiMenu_COMMAND | '4' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier4, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 5", GuiMenu_COMMAND | '5' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier5, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 6", GuiMenu_COMMAND | '6' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier6, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 7", GuiMenu_COMMAND | '7' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier7, this);
		FunctionAreaMenu_addCommand (intervalMenu, U"Add interval on tier 8", GuiMenu_COMMAND | '8' | GuiMenu_DEPTH_1,
				menu_cb_InsertIntervalOnTier8, this);
	}

	FunctionAreaMenu_addCommand (intervalMenu, U"- Query interval:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (intervalMenu, U"Get starting point of interval", 1,
			QUERY_DATA_FOR_REAL__GetStartingPointOfInterval, this);
	FunctionAreaMenu_addCommand (intervalMenu, U"Get end point of interval", 1,
			QUERY_DATA_FOR_REAL__GetEndPointOfInterval, this);
	FunctionAreaMenu_addCommand (intervalMenu, U"Get label of interval", 1,
			QUERY_DATA_FOR_STRING__GetLabelOfInterval, this);

	EditorMenu boundaryMenu = Editor_addMenu (our functionEditor(), U"Boundary", 0);
	/*FunctionAreaMenu_addCommand (boundaryMenu, U"Move to B", 0, menu_cb_MoveToB, this);
	FunctionAreaMenu_addCommand (boundaryMenu, U"Move to E", 0, menu_cb_MoveToE, this);*/
	if (our editable()) {
		FunctionAreaMenu_addCommand (boundaryMenu, U"New boundary or point:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on selected tier", GuiMenu_ENTER | GuiMenu_DEPTH_1,
				menu_cb_InsertOnSelectedTier, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 1", GuiMenu_COMMAND_EXTRA | '1' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier1, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 2", GuiMenu_COMMAND_EXTRA | '2' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier2, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 3", GuiMenu_COMMAND_EXTRA | '3' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier3, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 4", GuiMenu_COMMAND_EXTRA | '4' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier4, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 5", GuiMenu_COMMAND_EXTRA | '5' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier5, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 6", GuiMenu_COMMAND_EXTRA | '6' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier6, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 7", GuiMenu_COMMAND_EXTRA | '7' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier7, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on tier 8", GuiMenu_COMMAND_EXTRA | '8' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnTier8, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"Add on all tiers", GuiMenu_COMMAND_EXTRA | '9' | GuiMenu_DEPTH_1,
				menu_cb_InsertOnAllTiers, this);
		FunctionAreaMenu_addCommand (boundaryMenu, U"- Modify boundary or point:", 0, nullptr, this);
		if (our borrowedSoundArea && ! Thing_isa (our borrowedSoundArea, classLongSoundArea)) {
			FunctionAreaMenu_addCommand (boundaryMenu, U"Move to nearest zero crossing", 1,
					menu_cb_MoveToZero, this);
		}
		FunctionAreaMenu_addCommand (boundaryMenu, U"Remove", GuiMenu_OPTION | GuiMenu_BACKSPACE | GuiMenu_DEPTH_1,
				menu_cb_RemovePointOrBoundary, this);
	}

	EditorMenu tierMenu = Editor_addMenu (our functionEditor(), U"Tier", 0);
	if (our editable()) {
		FunctionAreaMenu_addCommand (tierMenu, U"New tier:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (tierMenu, U"Add interval tier...", 1,
				menu_cb_AddIntervalTier, this);
		FunctionAreaMenu_addCommand (tierMenu, U"Add point tier...", 1,
				menu_cb_AddPointTier, this);
		FunctionAreaMenu_addCommand (tierMenu, U"Duplicate tier...", 1,
				menu_cb_DuplicateTier, this);
		FunctionAreaMenu_addCommand (tierMenu, U"- Modify tier:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (tierMenu, U"Rename tier...", 1,
				menu_cb_RenameTier, this);
		FunctionAreaMenu_addCommand (tierMenu, U"Remove all text from tier", 1,
				menu_cb_RemoveAllTextFromTier, this);
		FunctionAreaMenu_addCommand (tierMenu, U"Remove entire tier", 1,
				menu_cb_RemoveTier, this);
	}
	FunctionAreaMenu_addCommand (tierMenu, U"- Extract to list of objects:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (tierMenu, U"Extract entire selected tier", 1,
			CONVERT_DATA_TO_ONE__PublishTier, this);

	if (our functionEditor() -> textArea)
		GuiText_setChangedCallback (our functionEditor() -> textArea, gui_text_cb_changed, this);

	if (our spellingChecker) {
		EditorMenu spellMenu = Editor_addMenu (our functionEditor(), U"Spell", 0);
		FunctionAreaMenu_addCommand (spellMenu, U"Check spelling in tier", GuiMenu_COMMAND_EXTRA | 'L',
				menu_cb_CheckSpelling, this);
		FunctionAreaMenu_addCommand (spellMenu, U"Check spelling in interval", 0,
				menu_cb_CheckSpellingInInterval, this);
		FunctionAreaMenu_addCommand (spellMenu, U"-- edit lexicon --", 0, nullptr, this);
		FunctionAreaMenu_addCommand (spellMenu, U"Add selected word to user dictionary", 0,
				menu_cb_AddToUserDictionary, this);
	}
}
void structTextGridArea :: v_updateMenuItems () {
	GuiThing_setSensitive (extractSelectedTextGridPreserveTimesButton, our endSelection() > our startSelection());
	GuiThing_setSensitive (extractSelectedTextGridTimeFromZeroButton,  our endSelection() > our startSelection());
}

void structTextGridArea :: v_updateText () {
	if (! our textGrid())
		return;   // BUG: should not be needed
	if (our suppressTextCursorJump)
		return;
	conststring32 newText = U"";
	if (our selectedTier != 0) {
		IntervalTier intervalTier;
		TextTier textTier;
		trace (U"TextGrid ", Melder_pointer (our textGrid()));
		AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer iinterval = IntervalTier_timeToIndex (intervalTier, our startSelection());
			if (iinterval) {
				TextInterval interval = intervalTier -> intervals.at [iinterval];
				if (interval -> text)
					newText = interval -> text.get();
			}
		} else {
			Melder_assert (isdefined (our startSelection()));
			const integer ipoint = AnyTier_hasPoint (textTier->asAnyTier(), our startSelection());
			if (ipoint) {
				TextPoint point = textTier -> points.at [ipoint];
				if (point -> mark)
					newText = point -> mark.get();
			}
		}
	}
	if (our functionEditor() -> textArea) {
		our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
		trace (U"setting new text ", newText);
		GuiText_setString (our functionEditor() -> textArea, newText);
		const integer cursor = Melder_length (newText);   // at end
		GuiText_setSelection (our functionEditor() -> textArea, cursor, cursor);
		our suppressRedraw = false;
	}
}


#pragma mark - TextGridArea IPA chart

static const conststring32 characters [12] [10] = {
	{ U"ɑ", U"ɐ", U"ɒ", U"æ", U"ʙ", U"ɓ", U"β", U"ç", U"ɕ", U"ð" },
	{ U"ɗ", U"ɖ", U"ɛ", U"ɜ", U"ə", U"ɢ", U"ɠ", U"ʛ", U"ɣ", U"ɤ" },
	{ U"ˠ", U"ʜ", U"ɦ", U"ħ", U"ʰ", U"ɧ", U"ɪ", U"ɨ", U"ı", U"ɟ" },
	{ U"ʝ", U"ʄ", U"ᴊ", U"ʲ", U"ʟ", U"ɬ", U"ɭ", U"ɮ", U"ɫ", U"ˡ" },
	{ U"ɰ", U"ɯ", U"ɱ", U"ɴ", U"ŋ", U"ɲ", U"ɳ", U"ⁿ", U"ɔ", U"ɵ" },

	{ U"ø", U"œ", U"ɶ", U"ɸ", U"ɹ", U"ʀ", U"ʁ", U"ɾ", U"ɽ", U"ɺ" },
	{ U"ɻ", U"ʃ", U"ʂ", U"θ", U"ʈ", U"ʉ", U"ʊ", U"ʌ", U"ʋ", U"ʍ" },
	{ U"ʷ", U"χ", U"ʎ", U"ʏ", U"ɥ", U"ʒ", U"ʐ", U"ʑ", U"ˑ", U"ː" },
	{ U"ʔ", U"ʡ", U"ʕ", U"ʢ", U"ˤ", U"ǃ", U"ʘ", U"ǀ", U"ǁ", U"ǂ" },
	{ U"ʤ", U"ɘ", U"ɚ", U"ɝ", U"ʱ", U"ˢ", U"ʧ", U"ɞ", U"ʦ", U"ʣ" },

	{ U"ʨ", U"ʥ", U"z̊", U"z̥", U"z̤", U"z̰", U"z̪", U"z̩", U"z̝", U"z̞" },
	{ U"ý", U"ȳ", U"ỳ", U"ÿ", U"ỹ", U"o̟", U"o̱", U"o̹", U"o̜", U"t̚" },
};

void TextGridArea_drawSelectionViewer (TextGridArea me) {
	Graphics_setWindow (my graphics(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (my graphics(), Melder_WHITE);
	Graphics_fillRectangle (my graphics(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (my graphics(), Melder_BLACK);
	Graphics_setFont (my graphics(), kGraphics_font::TIMES);
	const double pointsPerMillimetre = 72.0 / 25.4;
	const double cellWidth_points = Graphics_dxWCtoMM (my graphics(), 1.0) * pointsPerMillimetre;
	const double cellHeight_points = Graphics_dyWCtoMM (my graphics(), 1.0) * pointsPerMillimetre;
	const double fontSize = std::min (0.8 * cellHeight_points, 1.2 * cellWidth_points);
	Graphics_setFontSize (my graphics(), fontSize);
	Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
	for (integer irow = 1; irow <= 12; irow ++)
		for (integer icol = 1; icol <= 10; icol ++)
			Graphics_text (my graphics(), 0.0 + 1.0 * icol, 13.0 - 1.0 * irow, characters [irow-1] [icol-1]);
}
void TextGridArea_clickSelectionViewer (TextGridArea me, double x_fraction, double y_fraction) {
	const integer rowNumber = Melder_iceiling ((1.0 - y_fraction) * 12.0);
	const integer columnNumber = Melder_iceiling (x_fraction * 10.0);
	if (rowNumber < 1 || rowNumber > 12)
		return;
	if (columnNumber < 1 || columnNumber > 10)
		return;
	conststring32 character = characters [rowNumber-1] [columnNumber-1];
	character += Melder_length (character) - 1;
	if (my functionEditor() -> textArea) {
		integer first = 0, last = 0;
		autostring32 oldText = GuiText_getStringAndSelectionPosition (my functionEditor() -> textArea, & first, & last);
		static MelderString newText;
		MelderString_empty (& newText);
		MelderString_ncopy (& newText, oldText.get(), first);
		MelderString_append (& newText, character);
		MelderString_append (& newText, oldText.get() + last);
		if (my selectedTier != 0) {
			IntervalTier intervalTier;
			TextTier textTier;
			AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);
			if (intervalTier) {
				const integer selectedInterval = getSelectedInterval (me);
				if (selectedInterval != 0) {
					TextInterval interval = intervalTier -> intervals.at [selectedInterval];
					TextInterval_setText (interval, newText.string);

					my suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (my functionEditor() -> textArea, newText.string);
					GuiText_setSelection (my functionEditor() -> textArea, first + 1, first + 1);
					my suppressRedraw = false;

					my suppressTextCursorJump = true;
					FunctionArea_broadcastDataChanged (me);
					my suppressTextCursorJump = false;
				}
			} else {
				const integer selectedPoint = getSelectedPoint (me);
				if (selectedPoint != 0) {
					TextPoint point = textTier -> points.at [selectedPoint];
					point -> mark. reset();
					if (Melder_findInk (newText.string))   // any visible characters?
						point -> mark = Melder_dup_f (newText.string);

					my suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (my functionEditor() -> textArea, newText.string);
					GuiText_setSelection (my functionEditor() -> textArea, first + 1, first + 1);
					my suppressRedraw = false;

					my suppressTextCursorJump = true;
					FunctionArea_broadcastDataChanged (me);
					my suppressTextCursorJump = false;
				}
			}
		}
	}
}

/* End of file TextGridArea.cpp */
