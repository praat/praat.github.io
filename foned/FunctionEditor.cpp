/* FunctionEditor.cpp
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

#include "FunctionEditor.h"
#include "machine.h"
#include "EditorM.h"
#include "GuiP.h"
#include "FunctionArea.h"

Thing_implement_pureVirtual (FunctionEditor, Editor, 0);

#include "Prefs_define.h"
#include "FunctionEditor_prefs.h"
#include "Prefs_install.h"
#include "FunctionEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FunctionEditor_prefs.h"
#include "GraphicsP.h"

namespace {
	constexpr double maximumScrollBarValue = 2'000'000'000.0;
	constexpr double RELATIVE_PAGE_INCREMENT = 0.8;
	constexpr double SCROLL_INCREMENT_FRACTION = 20.0;
	constexpr int TEXT_HEIGHT = 50;
	constexpr int BUTTON_X = 3;
	constexpr int BUTTON_WIDTH = 40;
	constexpr int BUTTON_SPACING = 8;

	constexpr integer THE_MAXIMUM_GROUP_SIZE = 100;
	integer theGroupSize = 0;
	FunctionEditor theGroupMembers [1 + THE_MAXIMUM_GROUP_SIZE];
}

static bool group_equalDomain (double tmin, double tmax) {
	if (theGroupSize == 0)
		return true;
	for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++)
		if (theGroupMembers [i])
			return ( tmin == theGroupMembers [i] -> tmin && tmax == theGroupMembers [i] -> tmax );
	return false;   // should not occur
}

static void updateScrollBar (FunctionEditor me) {
/* We cannot call this immediately after creation. */
	if (my endWindow - my startWindow > my tmax - my tmin)
		Melder_crash (U"updateScrollBar: the window runs from ", my startWindow, U" to ", my endWindow, U" ", my v_format_units_long (),
			 	U", but the whole domain runs only from ", my tmin, U" to ", my tmax, U" ", my v_format_units_long (), U".");
	const double slider_size = Melder_clippedLeft (1.0, (my endWindow - my startWindow) / (my tmax - my tmin) * maximumScrollBarValue - 1.0);
	Melder_assert (maximumScrollBarValue - slider_size >= 1.0);
	const double value = Melder_clipped (1.0, (my startWindow - my tmin) / (my tmax - my tmin) * maximumScrollBarValue + 1.0, maximumScrollBarValue - slider_size);
	const double increment = slider_size / SCROLL_INCREMENT_FRACTION + 1.0;
	const double page_increment = RELATIVE_PAGE_INCREMENT * slider_size + 1.0;
	GuiScrollBar_set (my scrollBar, undefined, maximumScrollBarValue, value, slider_size, increment, page_increment);
}

static void updateGroup (FunctionEditor me, const bool windowMarkersChanged, const bool selectionMarkersChanged) {
	if (! my group)
		return;
	for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++) {
		if (theGroupMembers [i] && theGroupMembers [i] != me) {
			FunctionEditor thee = theGroupMembers [i];
			if (my classPref_synchronizedZoomAndScroll() && windowMarkersChanged) {
				thy startWindow = my startWindow;
				thy endWindow = my endWindow;
				thy v_windowChanged ();
			}
			if (selectionMarkersChanged) {
				thy startSelection = my startSelection;
				thy endSelection = my endSelection;
			}
			Melder_assert (isdefined (thy startSelection));   // precondition of FunctionEditor_updateText()
			FunctionEditor_updateText (thee);
			updateScrollBar (thee);
			FunctionEditor_redraw (thee);   // BUG: does this do *two* updates if thou containst the same data as me?
		}
	}
}

static void drawBackgroundAndData (FunctionEditor me) {
	if (Melder_debug == 55)
		Melder_casual (Thing_messageNameAndAddress (me), U" draw");
	const bool leftFromWindow = ( my startWindow > my tmin );
	const bool rightFromWindow = ( my endWindow < my tmax );
	const bool cursorIsVisible = ( my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow );
	const bool selectionIsNonempty = ( my endSelection > my startSelection );

	/*
		Update selection.
	*/
	const bool startIsVisible = ( my startSelection > my startWindow && my startSelection < my endWindow );
	const bool endIsVisible = ( my endSelection > my startWindow && my endSelection < my endWindow );

	/*
		Update markers.
	*/
	my numberOfMarkers = 0;
	if (startIsVisible)
		my marker [++ my numberOfMarkers] = my startSelection;
	if (endIsVisible && my endSelection != my startSelection)
		my marker [++ my numberOfMarkers] = my endSelection;
	my marker [++ my numberOfMarkers] = my endWindow;
	sort_e_VEC_inout (VEC (& my marker [1], my numberOfMarkers));

	/*
		Update rectangles.
	*/

	/*
		Initialize to 0; rectangles that stay at zero will not be drawn.
	*/
	for (integer i = 0; i < 8; i++)
		my rect [i]. left = my rect [i]. right = 0;

	/*
		0: rectangle for total.
	*/
	my rect [0]. left = my _functionViewerLeft + ( leftFromWindow ? 0 : my MARGIN );
	my rect [0]. right = my _functionViewerRight - ( rightFromWindow ? 0 : my MARGIN );
	my rect [0]. bottom = my BOTTOM_MARGIN;
	my rect [0]. top = my BOTTOM_MARGIN + my space;

	/*
		1: rectangle for visible part.
	*/
	my rect [1]. left = my _functionViewerLeft + my MARGIN;
	my rect [1]. right = my _functionViewerRight - my MARGIN;
	my rect [1]. bottom = my BOTTOM_MARGIN + my space;
	my rect [1]. top = my BOTTOM_MARGIN + my space * ( my numberOfMarkers > 1 ? 2 : 3 );

	/*
		2: rectangle for left from visible part.
	*/
	if (leftFromWindow) {
		my rect [2]. left = my _functionViewerLeft;
		my rect [2]. right = my _functionViewerLeft + my MARGIN;
		my rect [2]. bottom = my BOTTOM_MARGIN + my space;
		my rect [2]. top = my BOTTOM_MARGIN + my space * 2;
	}

	/*
		3: rectangle for right from visible part.
	*/
	if (rightFromWindow) {
		my rect [3]. left = my _functionViewerRight - my MARGIN;
		my rect [3]. right = my _functionViewerRight;
		my rect [3]. bottom = my BOTTOM_MARGIN + my space;
		my rect [3]. top = my BOTTOM_MARGIN + my space * 2;
	}

	/*
		4, 5, 6: rectangles between markers visible in visible part.
	*/
	if (my numberOfMarkers > 1) {
		const double window = my endWindow - my startWindow;
		for (integer i = 1; i <= my numberOfMarkers; i ++) {
			my rect [3 + i]. left = i == 1 ? my _functionViewerLeft + my MARGIN : my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) *
				(my marker [i - 1] - my startWindow) / window;
			my rect [3 + i]. right = my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) *
				(my marker [i] - my startWindow) / window;
			my rect [3 + i]. bottom = my BOTTOM_MARGIN + my space * 2;
			my rect [3 + i]. top = my BOTTOM_MARGIN + my space * 3;
		}
	}

	if (selectionIsNonempty) {
		const double window = my endWindow - my startWindow;
		const double left =
			my startSelection == my startWindow ?
				my _functionViewerLeft + my MARGIN
			: my startSelection == my tmin ?
				my _functionViewerLeft
			: my startSelection < my startWindow ?
				my _functionViewerLeft + my MARGIN * 0.3
			: my startSelection < my endWindow ?
				my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) * (my startSelection - my startWindow) / window
			: my startSelection == my endWindow ?
				my _functionViewerRight - my MARGIN
			:
				my _functionViewerRight - my MARGIN * 0.7
		;
		const double right =
			my endSelection < my startWindow ?
				my _functionViewerLeft + my MARGIN * 0.7
			: my endSelection == my startWindow ?
				my _functionViewerLeft + my MARGIN
			: my endSelection < my endWindow ?
				my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) * (my endSelection - my startWindow) / window
			: my endSelection == my endWindow ?
				my _functionViewerRight - my MARGIN
			: my endSelection < my tmax ?
				my _functionViewerRight - my MARGIN * 0.3
			: my _functionViewerRight
		;
		my rect [7]. left = left;
		my rect [7]. right = right;
		my rect [7]. bottom = my height_pxlt - my space - my TOP_MARGIN;
		my rect [7]. top = my height_pxlt - my TOP_MARGIN;
	}

	/*
		Window background.
	*/
	my viewAllAsPixelettes ();
	Graphics_setColour (my graphics.get(), DataGuiColour_WINDOW_BACKGROUND);
	Graphics_fillRectangle (my graphics.get(), my _functionViewerLeft, my _selectionViewerRight, my BOTTOM_MARGIN, my height_pxlt);
	Graphics_setColour (my graphics.get(), Melder_BLACK);

	/*
		Buttons (still without text).
	*/
	my viewFunctionViewerAsPixelettes ();
	Graphics_setFont (my graphics.get(), kGraphics_font :: HELVETICA);
	Graphics_setFontSize (my graphics.get(), 12.0);
	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer i = 0; i < 8; i ++) {
		const double left = my rect [i]. left, right = my rect [i]. right;
		if (left < right)
			Graphics_button (my graphics.get(), left, right, my rect [i]. bottom, my rect [i]. top);
	}

	/*
		Opening triangle (sometimes over button).
	*/
	if (my v_hasSelectionViewer() && ! my instancePref_showSelectionViewer()) {
		const bool weHaveToDrawOverSelectionRectangleWithText = ( selectionIsNonempty && my endSelection == my tmax && my endWindow != my tmax );
		Graphics_setLineWidth (my graphics.get(), 1.0);
		const double left = my _functionViewerRight - my space + 9.0, right = my _functionViewerRight - 3.0;
		const double bottom = my height_pxlt - my space - my TOP_MARGIN + 3.0, top = my height_pxlt - my TOP_MARGIN - 3.0;
		Graphics_setColour (my graphics.get(), Melder_PINK);
		const double x [] = { left, right, left }, y [] = { bottom, 0.5 * (bottom + top), top };
		Graphics_fillArea (my graphics.get(), 3, x, y);
		if (! weHaveToDrawOverSelectionRectangleWithText) {
			Graphics_setColour (my graphics.get(), Melder_GREY);
			Graphics_polyline_closed (my graphics.get(), 3, x, y);
		}
		Graphics_setColour (my graphics.get(), Melder_BLACK);
	}

	/*
		Button texts (sometimes over opening triangle).
	*/
	const double verticalCorrection = my height_pxlt / (my height_pxlt - 111.0 + 11.0)
		#ifdef _WIN32
			* 1.5
		#endif
	;
	for (integer i = 0; i < 8; i ++) {
		const double left = my rect [i]. left, right = my rect [i]. right;
		const double bottom = my rect [i]. bottom, top = my rect [i]. top;
		if (left < right) {
			conststring8 format = my v_format_long ();
			double value = undefined, inverseValue = 0.0;
			switch (i) {
				case 0: {
					format = my v_format_totalDuration ();
					value = my tmax - my tmin;
				} break; case 1: {
					format = my v_format_window ();
					value = my endWindow - my startWindow;
					/*
						Window domain text.
					*/
					Graphics_setColour (my graphics.get(), Melder_BLUE);
					Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
					Graphics_text (my graphics.get(), left, 0.5 * (bottom + top) - verticalCorrection,
							Melder_fixed (my startWindow, my v_fixedPrecision_long ()));
					Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
					Graphics_text (my graphics.get(), right, 0.5 * (bottom + top) - verticalCorrection,
							Melder_fixed (my endWindow, my v_fixedPrecision_long ()));
					Graphics_setColour (my graphics.get(), Melder_BLACK);
					Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
				} break; case 2: {
					value = my startWindow - my tmin;
				} break; case 3: {
					value = my tmax - my endWindow;
				} break; case 4: {
					value = my marker [1] - my startWindow;
				} break; case 5: {
					value = my marker [2] - my marker [1];
				} break; case 6: {
					value = my marker [3] - my marker [2];
				} break; case 7: {
					format = my v_format_selection ();
					value = my endSelection - my startSelection;
					inverseValue = 1.0 / value;
				}
			}
			char text8 [100];
			snprintf (text8, 100, format, value, inverseValue);
			autostring32 text = Melder_8to32_e (text8);
			if (Graphics_textWidth (my graphics.get(), text.get()) < right - left) {
				Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
			} else if (format == my v_format_long()) {
				snprintf (text8, 100, my v_format_short(), value);
				text = Melder_8to32_e (text8);
				if (Graphics_textWidth (my graphics.get(), text.get()) < right - left)
					Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
			} else {
				snprintf (text8, 100, my v_format_long(), value);
				text = Melder_8to32_e (text8);
				if (Graphics_textWidth (my graphics.get(), text.get()) < right - left) {
					Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
				} else {
					snprintf (text8, 100, my v_format_short(), my endSelection - my startSelection);
					text = Melder_8to32_e (text8);
					if (Graphics_textWidth (my graphics.get(), text.get()) < right - left)
						Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
				}
			}
		}
	}

	my viewTallDataAsWorldByPixelettes ();

	/*
		Red marker text.
	*/
	Graphics_setColour (my graphics.get(), Melder_RED);
	if (cursorIsVisible) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my graphics.get(), my startSelection, my height_pxlt - (my TOP_MARGIN + my space*0.9) - verticalCorrection * 7,
				Melder_fixed (my startSelection, my v_fixedPrecision_long()));
	}
	if (startIsVisible && selectionIsNonempty) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (my graphics.get(), my startSelection, my height_pxlt - (my TOP_MARGIN + my space/2) - verticalCorrection,
				Melder_fixed (my startSelection, my v_fixedPrecision_long()));
	}
	if (endIsVisible && selectionIsNonempty) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (my graphics.get(), my endSelection, my height_pxlt - (my TOP_MARGIN + my space/2) - verticalCorrection,
				Melder_fixed (my endSelection, my v_fixedPrecision_long()));
	}
	Graphics_setColour (my graphics.get(), Melder_BLACK);

	/*
		Start of inner drawing.
	*/
	my v_distributeAreas ();
	//my viewDataAsWorldByFraction ();
	my v_draw ();
	my v_drawLegends ();

	/*
		Red dotted marker lines.
	*/
	my viewDataAsWorldByFraction ();
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
	if (cursorIsVisible)
		Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
	if (startIsVisible)
		Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
	if (endIsVisible)
		Graphics_line (my graphics.get(), my endSelection, 0.0, my endSelection, 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
}

/********** METHODS **********/

void structFunctionEditor :: v9_destroy () noexcept {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	if (our group) {   // undangle
		integer i = 1;
		while (theGroupMembers [i] != this) {
			Melder_assert (i < THE_MAXIMUM_GROUP_SIZE);
			i ++;
		}
		theGroupMembers [i] = nullptr;
		theGroupSize --;
	}
	if (Melder_debug == 55)
		Melder_casual (Thing_messageNameAndAddress (this), U" v9_destroy");
	FunctionEditor_Parent :: v9_destroy ();
}

void structFunctionEditor :: v1_info () {
	FunctionEditor_Parent :: v1_info ();
	MelderInfo_writeLine (U"Editor start: ", our tmin, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Editor end: ", our tmax, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Window start: ", our startWindow, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Window end: ", our endWindow, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Selection start: ", our startSelection, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Selection end: ", our endSelection, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Arrow scroll step: ", our instancePref_arrowScrollStep(), U" ", v_format_units_long());
	MelderInfo_writeLine (U"Group: ", group ? U"yes" : U"no");
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v1_info ();
	}
}

void structFunctionEditor :: v_windowChanged () {
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_windowChanged ();
	}
}


/********** FILE MENU **********/

static void gui_drawingarea_cb_resize (FunctionEditor me, GuiDrawingArea_ResizeEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	my updateGeometry (event -> width, event -> height);
	FunctionEditor_redraw (me);
	/*
		Save the current shell size as the user's preference for a new FunctionEditor.
	*/
	my setClassPref_shellWidth  (GuiShell_getShellWidth  (my windowForm));
	my setClassPref_shellHeight (GuiShell_getShellHeight (my windowForm));
}

static void zoom_fromTo (FunctionEditor me, double from, double to) {
	if (from < my tmin + 1e-12)
		from = my tmin;
	if (to > my tmax - 1e-12)
		to = my tmax;
	Melder_require (to > from,
		U"“to” should be greater than “from”.");
	my startWindow = from;
	my endWindow = to;
	if (my endWindow <= my startWindow) {   // floating-point underflow
		my startWindow = my tmin;
		my endWindow = my tmax;
	}
	my v_windowChanged ();
	FunctionEditor_windowMarksChanged (me, false);
}
static void shift_by (FunctionEditor me, const double shift, const bool selectionChanged) {
	const double windowLength = my endWindow - my startWindow;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // quickly, before window changes
	trace (U"shifting by ", shift);
	if (shift < 0.0) {
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
		my endWindow = my startWindow + windowLength;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
	} else {
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
		my startWindow = my endWindow - windowLength;
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
	}
	my v_windowChanged ();
	FunctionEditor_windowMarksChanged (me, selectionChanged);
}
static void zoom_by (FunctionEditor me, double factor) {
	const double currentSize = my endWindow - my startWindow;
	const double newSize = currentSize * factor;
	const double increase = newSize - currentSize;
	const double shift = 0.5 * increase;
	zoom_fromTo (me, my startWindow - shift, my endWindow + shift);
}

static void gui_drawingarea_cb_zoom (FunctionEditor me, GuiDrawingArea_ZoomEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	const double enlargement = exp (-0.02 * (event -> delta>0.0?+1:-1) * sqrt (fabs (event -> delta)));   // 2 percent per step
	zoom_by (me, enlargement);
}

void structFunctionEditor :: v_prefs_addFields (EditorCommand cmd) {
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_prefs_addFields (cmd);
	}
}
void structFunctionEditor :: v_prefs_setValues (EditorCommand cmd) {
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_prefs_setValues (cmd);
	}
}
void structFunctionEditor :: v_prefs_getValues (EditorCommand cmd) {
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_prefs_getValues (cmd);
	}
}
static void menu_cb_editorSettings (FunctionEditor me, EDITOR_ARGS) {   // TODO: make visible
	EDITOR_FORM (U"Editor settings", nullptr)
		BOOLEAN (showSelectionViewer, Melder_cat (U"Show ", my v_selectionViewerName()), my default_showSelectionViewer())
		my v_prefs_addFields (cmd);
	EDITOR_OK
		SET_BOOLEAN (showSelectionViewer, my instancePref_showSelectionViewer())
		my v_prefs_setValues (cmd);
	EDITOR_DO
		const bool oldShowSelectionViewer = my instancePref_showSelectionViewer();
		my setInstancePref_showSelectionViewer (showSelectionViewer);
		if (my instancePref_showSelectionViewer() != oldShowSelectionViewer)
			my updateGeometry (GuiControl_getWidth (my drawingArea), GuiControl_getHeight (my drawingArea));
		my v_prefs_getValues (cmd);
		FunctionEditor_redraw (me);
	EDITOR_END
}


#pragma mark - FuncEd Time query selection

static void QUERY_EDITOR_FOR_REAL__getStartOfSelection (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my startSelection;
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}
static void QUERY_EDITOR_FOR_REAL__getCursor (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = 0.5 * (my startSelection + my endSelection);
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}
static void QUERY_EDITOR_FOR_REAL__getEndOfSelection (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my endSelection;
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}
static void QUERY_EDITOR_FOR_REAL__getLengthOfSelection (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my endSelection - my startSelection;
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}


#pragma mark - FuncEd Time set selection

static void menu_cb_select (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Select", nullptr)
		REAL (startOfSelection, Melder_cat (U"Start of selection (", my v_format_units_short(), U")"), U"0.0")
		REAL (endOfSelection,   Melder_cat (U"End of selection (",   my v_format_units_short(), U")"), U"1.0")
	EDITOR_OK
		SET_REAL (startOfSelection, my startSelection)
		SET_REAL (endOfSelection,   my endSelection)
	EDITOR_DO
		my startSelection = startOfSelection;
		if (my startSelection < my tmin + 1e-12)
			my startSelection = my tmin;
		my endSelection = endOfSelection;
		if (my endSelection > my tmax - 1e-12)
			my endSelection = my tmax;
		if (my startSelection > my endSelection)
			std::swap (my startSelection, my endSelection);   // this can invalidate the above logic
		Melder_clip (my tmin, & my startSelection, my tmax);
		Melder_clip (my tmin, & my endSelection, my tmax);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	EDITOR_END
}
static void menu_cb_widenOrShrinkSelection (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Widen or shrink selection", nullptr)
		POSITIVE (newWidthOfSelection, Melder_cat (U"New width of selection (", my v_format_units_short(), U")"), U"0.3")
		CHOICE_ENUM (kGraphics_horizontalAlignment, alignmentWithCurrentSelection, U"Alignment with current selection", kGraphics_horizontalAlignment::CENTRE)
	EDITOR_OK
	EDITOR_DO
		const double currentWidthOfSelection = my endSelection - my startSelection;
		const double degreeOfWidening = newWidthOfSelection - currentWidthOfSelection;
		double newStartOfSelection;
		switch (alignmentWithCurrentSelection) {
			case kGraphics_horizontalAlignment::LEFT: {
				newStartOfSelection = my startSelection;
			} break; case kGraphics_horizontalAlignment::CENTRE: {
				newStartOfSelection = my startSelection - 0.5 * degreeOfWidening;
			} break; case kGraphics_horizontalAlignment::RIGHT: {
				newStartOfSelection = my startSelection - degreeOfWidening;
			} break; case kGraphics_horizontalAlignment::UNDEFINED: {
				Melder_throw (U"Undefined alignment.");
			}
		}
		const double newEndOfSelection = newStartOfSelection + newWidthOfSelection;
		Melder_require (newStartOfSelection >= my tmin,
			U"Widening the selection to ", newWidthOfSelection, U" ", my v_format_units_long(),
			U" would move the start of the selection to ", newStartOfSelection, U" ", my v_format_units_long(),
			U", which lies before the start of the editor’s ", my v_domainName(),
			U" domain, which is at ", my tmin, U" ", my v_format_units_long(), U"."
		);
		Melder_require (newEndOfSelection <= my tmax,
			U"Widening the selection to ", newWidthOfSelection, U" ", my v_format_units_long(),
			U" would move the end of the selection to ", newEndOfSelection, U" ", my v_format_units_long(),
			U", which lies past the end of the editor’s ", my v_domainName(),
			U" domain, which is at ", my tmax, U" ", my v_format_units_long(), U"."
		);
		my startSelection = newStartOfSelection;
		my endSelection = newEndOfSelection;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	EDITOR_END
}
static void menu_cb_moveCursorToStartOfSelection (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my endSelection = my startSelection;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	VOID_EDITOR_END
}
static void menu_cb_moveCursorToEndOfSelection (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my startSelection = my endSelection;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	VOID_EDITOR_END
}
static void menu_cb_moveCursorTo (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Move cursor to", nullptr)
		REAL (position, Melder_cat (U"Position (", my v_format_units_short(), U")"), U"0.0")
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		if (position < my tmin + 1e-12)
			position = my tmin;
		if (position > my tmax - 1e-12)
			position = my tmax;
		my startSelection = my endSelection = position;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	EDITOR_END
}
static void menu_cb_moveCursorBy (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Move cursor by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		Melder_assert (my tmax >= my tmin);
		const double position = Melder_clipped (my tmin, 0.5 * (my startSelection + my endSelection) + distance, my tmax);
		my startSelection = my endSelection = position;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	EDITOR_END
}
static void menu_cb_moveStartOfSelectionBy (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Move start of selection by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		Melder_assert (my tmax >= my tmin);
		my startSelection = Melder_clipped (my tmin, my startSelection + distance, my tmax);
		Melder_sort (& my startSelection, & my endSelection);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	EDITOR_END
}
static void menu_cb_moveEndOfSelectionBy (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Move end of selection by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		Melder_assert (my tmax >= my tmin);
		my endSelection = Melder_clipped (my tmin, my endSelection + distance, my tmax);
		Melder_sort (& my startSelection, & my endSelection);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	EDITOR_END
}
void FunctionEditor_scrollToNewSelection (FunctionEditor me, double t) {
	if (t <= my startWindow) {
		Melder_assert (isdefined (my startSelection));   // precondition of shift_by()
		shift_by (me, t - my startWindow - 0.618 * (my endWindow - my startWindow), true);
	} else if (t >= my endWindow) {
		Melder_assert (isdefined (my startSelection));   // precondition of shift_by()
		shift_by (me, t - my endWindow + 0.618 * (my endWindow - my startWindow), true);
	} else {
		FunctionEditor_selectionMarksChanged (me);
	}
}
static void menu_cb_selectionStepSettings (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Selection step settings", nullptr)
		POSITIVE (arrowScrollStep, Melder_cat (U"Arrow scroll step (", my v_format_units_short(), U")"), my default_arrowScrollStep())
	EDITOR_OK
		SET_REAL (arrowScrollStep, my instancePref_arrowScrollStep())
	EDITOR_DO
		my setInstancePref_arrowScrollStep (arrowScrollStep);
	EDITOR_END
}
static void menu_cb_selectEarlier (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my startSelection -= my instancePref_arrowScrollStep();
		if (my startSelection < my tmin + 1e-12)
			my startSelection = my tmin;
		my endSelection -= my instancePref_arrowScrollStep();
		if (my endSelection < my tmin + 1e-12)
			my endSelection = my tmin;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToNewSelection()
		FunctionEditor_scrollToNewSelection (me, 0.5 * (my startSelection + my endSelection));
	VOID_EDITOR_END
}
static void menu_cb_selectLater (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my startSelection += my instancePref_arrowScrollStep();
		if (my startSelection > my tmax - 1e-12)
			my startSelection = my tmax;
		my endSelection += my instancePref_arrowScrollStep();
		if (my endSelection > my tmax - 1e-12)
			my endSelection = my tmax;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToNewSelection()
		FunctionEditor_scrollToNewSelection (me, 0.5 * (my startSelection + my endSelection));
	VOID_EDITOR_END
}
static void menu_cb_moveStartOfSelectionLeft (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my startSelection -= my instancePref_arrowScrollStep();
		if (my startSelection < my tmin + 1e-12)
			my startSelection = my tmin;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToNewSelection()
		FunctionEditor_scrollToNewSelection (me, 0.5 * (my startSelection + my endSelection));
	VOID_EDITOR_END
}
static void menu_cb_moveStartOfSelectionRight (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my startSelection += my instancePref_arrowScrollStep();
		if (my startSelection > my tmax - 1e-12)
			my startSelection = my tmax;
		Melder_sort (& my startSelection, & my endSelection);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToNewSelection()
		FunctionEditor_scrollToNewSelection (me, 0.5 * (my startSelection + my endSelection));
	VOID_EDITOR_END
}
static void menu_cb_moveEndOfSelectionLeft (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my endSelection -= my instancePref_arrowScrollStep();
		if (my endSelection < my tmin + 1e-12)
			my endSelection = my tmin;
		Melder_sort (& my startSelection, & my endSelection);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToNewSelection()
		FunctionEditor_scrollToNewSelection (me, 0.5 * (my startSelection + my endSelection));
	VOID_EDITOR_END
}
static void menu_cb_moveEndOfSelectionRight (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		my endSelection += my instancePref_arrowScrollStep();
		if (my endSelection > my tmax - 1e-12)
			my endSelection = my tmax;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToNewSelection()
		FunctionEditor_scrollToNewSelection (me, 0.5 * (my startSelection + my endSelection));
	VOID_EDITOR_END
}


#pragma mark - FuncEd Time query visible part

static void QUERY_EDITOR_FOR_REAL__getStartOfVisiblePart (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my startWindow;
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}
static void QUERY_EDITOR_FOR_REAL__getEndOfVisiblePart (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my endWindow;
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}
static void QUERY_EDITOR_FOR_REAL__getLengthOfVisiblePart (FunctionEditor me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my endWindow - my startWindow;
	QUERY_EDITOR_FOR_REAL_END (U" ", my v_format_units_long())
}


#pragma mark - FuncEd Time set visible part

static void menu_cb_zoomAndScrollSettings (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Zoom and scroll settings", nullptr)
		BOOLEAN (synchronizeZoomAndScroll, U"Synchronize zoom and scroll", my default_synchronizedZoomAndScroll())
	EDITOR_OK
		SET_BOOLEAN (synchronizeZoomAndScroll, my classPref_synchronizedZoomAndScroll())
	EDITOR_DO
		const bool oldSynchronizedZoomAndScroll = my classPref_synchronizedZoomAndScroll();
		my setClassPref_synchronizedZoomAndScroll (synchronizeZoomAndScroll);
		if (! oldSynchronizedZoomAndScroll && my classPref_synchronizedZoomAndScroll())
			updateGroup (me, true, false);
		FunctionEditor_redraw (me);
	EDITOR_END
}
static void menu_cb_zoom (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Zoom", nullptr)
		REAL (from, Melder_cat (U"From (", my v_format_units_short(), U")"), U"0.0")
		REAL (to,   Melder_cat (U"To (", my v_format_units_short(), U")"),   U"1.0")
	EDITOR_OK
		SET_REAL (from, my startWindow)
		SET_REAL (to,   my endWindow)
	EDITOR_DO
		Melder_require (to > from,
			U"“to” should be greater than “from”.");
		zoom_fromTo (me, from, to);
	EDITOR_END
}
static void do_showAll (FunctionEditor me) {
	zoom_fromTo (me, my tmin, my tmax);
}
static void gui_button_cb_showAll (FunctionEditor me, GuiButtonEvent /* event */) {
	do_showAll (me);
}
static void do_zoomIn (FunctionEditor me) {
	zoom_by (me, 0.5);
}
static void gui_button_cb_zoomIn (FunctionEditor me, GuiButtonEvent /* event */) {
	do_zoomIn (me);
}
static void do_zoomOut (FunctionEditor me) {
	//MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // quickly, before window changes; ppgb 2022-06-25: why was this here?
	zoom_by (me, 2.0);
}
static void gui_button_cb_zoomOut (FunctionEditor me, GuiButtonEvent /*event*/) {
	do_zoomOut (me);
}
static void do_zoomToSelection (FunctionEditor me) {
	if (my endSelection > my startSelection) {
		my startZoomHistory = my startWindow;   // remember for Zoom Back
		my endZoomHistory = my endWindow;   // remember for Zoom Back
		zoom_fromTo (me, my startSelection, my endSelection);
	}
}
static void gui_button_cb_zoomToSelection (FunctionEditor me, GuiButtonEvent /* event */) {
	do_zoomToSelection (me);
}
static void do_zoomBack (FunctionEditor me) {
	Melder_clip (my tmin, & my startZoomHistory, my tmax);
	Melder_clip (my tmin, & my endZoomHistory, my tmax);
	if (my endZoomHistory > my startZoomHistory)
		zoom_fromTo (me, my startZoomHistory, my endZoomHistory);
}
static void gui_button_cb_zoomBack (FunctionEditor me, GuiButtonEvent /* event */) {
	do_zoomBack (me);
}
static void menu_cb_showAll (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		do_showAll (me);
	VOID_EDITOR_END
}
static void menu_cb_zoomIn (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		do_zoomIn (me);
	VOID_EDITOR_END
}
static void menu_cb_zoomOut (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		do_zoomOut (me);
	VOID_EDITOR_END
}
static void menu_cb_zoomToSelection (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		do_zoomToSelection (me);
	VOID_EDITOR_END
}
static void menu_cb_zoomBack (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		do_zoomBack (me);
	VOID_EDITOR_END
}
static void menu_cb_pageUp (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		Melder_assert (isdefined (my startSelection));   // precondition of shift_by()
		shift_by (me, -RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow), false);
	VOID_EDITOR_END
}
static void menu_cb_pageDown (FunctionEditor me, EDITOR_ARGS) {
	VOID_EDITOR
		Melder_assert (isdefined (my startSelection));   // precondition of shift_by()
		shift_by (me, +RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow), false);
	VOID_EDITOR_END
}


#pragma mark - FunctionEditor Play menu

static void PLAY_DATA__play (FunctionEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Play", nullptr)
		REAL (from, Melder_cat (U"From (", my v_format_units_short(), U")"), U"0.0")
		REAL (to,   Melder_cat (U"To (", my v_format_units_short(), U")"),   U"1.0")
	EDITOR_OK
		SET_REAL (from, my startWindow)
		SET_REAL (to,   my endWindow)
	EDITOR_DO
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
		my v_play (from, to);
	EDITOR_END
}
static void PLAY_DATA__playOrStop (FunctionEditor me, EDITOR_ARGS) {
	PLAY_DATA
		if (MelderAudio_isPlaying) {
			MelderAudio_stopPlaying (MelderAudio_EXPLICIT);
		} else if (my startSelection < my endSelection) {
			my v_play (my startSelection, my endSelection);
		} else {
			if (my startSelection == my endSelection && my startSelection > my startWindow && my startSelection < my endWindow)
				my v_play (my startSelection, my endWindow);
			else
				my v_play (my startWindow, my endWindow);
		}
	PLAY_DATA_END
}
static void PLAY_DATA__playWindow (FunctionEditor me, EDITOR_ARGS) {
	PLAY_DATA
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
		my v_play (my startWindow, my endWindow);
	PLAY_DATA_END
}
static void PLAY_DATA__interruptPlaying (FunctionEditor me, EDITOR_ARGS) {
	PLAY_DATA
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	PLAY_DATA_END
}

void structFunctionEditor :: v_createMenuItems_play (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Audio:", 0, nullptr);
	EditorMenu_addCommand (menu, U"Play...",
			1, PLAY_DATA__play);
	EditorMenu_addCommand (menu, U"Play or stop",
			GuiMenu_DEPTH_1 | GuiMenu_TAB, PLAY_DATA__playOrStop);
	EditorMenu_addCommand (menu, U"Play window",
			GuiMenu_DEPTH_1 | GuiMenu_SHIFT | GuiMenu_TAB, PLAY_DATA__playWindow);
	EditorMenu_addCommand (menu, U"Interrupt playing",
			GuiMenu_DEPTH_1 | GuiMenu_ESCAPE, PLAY_DATA__interruptPlaying);
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_createMenuItems_play (menu);
	}
}


#pragma mark - FunctionEditor GUI callbacks

static void gui_cb_scroll (FunctionEditor me, GuiScrollBarEvent event) {
	if (! my graphics)
		return;   // ignore events during creation
	const double value = GuiScrollBar_getValue (event -> scrollBar);
	const double shift = my tmin + (value - 1) * (my tmax - my tmin) / maximumScrollBarValue - my startWindow;
	const bool shifted = ( shift != 0.0 );
	const double oldSliderSize = (my endWindow - my startWindow) / (my tmax - my tmin) * maximumScrollBarValue - 1.0;
	const double newSliderSize = GuiScrollBar_getSliderSize (event -> scrollBar);
	bool zoomed = ( newSliderSize != oldSliderSize );
	#if ! cocoa
		zoomed = false;
	#endif
	if (shifted) {
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
	}
	if (zoomed) {
		const double zoom = (newSliderSize + 1) * (my tmax - my tmin) / maximumScrollBarValue;
		my endWindow = my startWindow + zoom;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
	}
	if (shifted || zoomed) {
		my v_windowChanged ();
		Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
		my v_updateText ();   // BUG: this could be superfluous, because the selection hasn't changed?
		//updateScrollBar (me);   // commented out, becaus it would be circular
		FunctionEditor_redraw (me);
		if (! my group || ! my classPref_synchronizedZoomAndScroll())
			return;
		for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++) {
			if (theGroupMembers [i] && theGroupMembers [i] != me) {
				theGroupMembers [i] -> startWindow = my startWindow;
				theGroupMembers [i] -> endWindow = my endWindow;
				theGroupMembers [i] -> v_windowChanged ();
				Melder_assert (isdefined (theGroupMembers [i] -> startSelection));   // precondition of FunctionEditor_updateText()
				FunctionEditor_updateText (theGroupMembers [i]);
				updateScrollBar (theGroupMembers [i]);
				FunctionEditor_redraw (theGroupMembers [i]);
			}
		}
	}
}

static integer findEmptySpotInGroup () {
	integer emptySpot = 1;
	while (theGroupMembers [emptySpot])
		emptySpot ++;
	return emptySpot;
}

static integer findMeInGroup (FunctionEditor me) {
	integer positionInGroup = 1;
	while (theGroupMembers [positionInGroup] != me)
		positionInGroup ++;
	return positionInGroup;
}

static integer findOtherGroupMember (FunctionEditor me) {
	integer otherGroupMember = 1;
	while (! theGroupMembers [otherGroupMember] || theGroupMembers [otherGroupMember] == me)
		otherGroupMember ++;
	return otherGroupMember;
}

static void gui_checkbutton_cb_group (FunctionEditor me, GuiCheckButtonEvent /* event */) {
	my group = ! my group;   // toggle
	if (my group) {
		const integer emptySpot = findEmptySpotInGroup ();
		theGroupMembers [emptySpot] = me;
		if (++ theGroupSize == 1) {
			FunctionEditor_redraw (me);
			return;
		}
		const integer otherGroupMember = findOtherGroupMember (me);
		const FunctionEditor she = theGroupMembers [otherGroupMember];
		/*
			Adapt my domain.
		*/
		Melder_clipRight (& my tmin, her tmin);
		Melder_clipLeft (her tmax, & my tmax);
		/*
			Adapt my window.
		*/
		if (my classPref_synchronizedZoomAndScroll()) {
			my startWindow = her startWindow;
			my endWindow = her endWindow;
			my v_windowChanged ();
		}
		/*
			Adapt my selection.
			Or should I impose my selection on the others? Votes?
		*/
		my startSelection = her startSelection;
		my endSelection = her endSelection;
		/*
			Update me.
		*/
		Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
		my v_updateText ();
		updateScrollBar (me);
		FunctionEditor_redraw (me);
		/*
			Adapt all the other windows.
		*/
		if (my tmin < her tmin || my tmax > her tmax) {
			for (integer imember = 1; imember <= THE_MAXIMUM_GROUP_SIZE; imember ++) {
				FunctionEditor you = theGroupMembers [imember];
				if (you && you != me) {
					/*
						Adapt your domain.
					*/
					Melder_clipRight (& your tmin, my tmin);
					Melder_clipLeft (my tmax, & your tmax);
					/*
						Adapt your window.
					 */
					if (my classPref_synchronizedZoomAndScroll()) {
						your startWindow = my startWindow;
						your endWindow = my endWindow;
						your v_windowChanged ();
					}
					/*
						Adapt your selection.
						Should already have been set equal earlier, but why not
						(to keep it compatible with the imposure suggested above).
					*/
					your startSelection = my startSelection;   // probably void
					your endSelection = my endSelection;   // probably void
					/*
						Update you.
					*/
					Melder_assert (isdefined (your startSelection));   // precondition of FunctionEditor_updateText()
					FunctionEditor_updateText (you);
					updateScrollBar (you);
					FunctionEditor_redraw (you);
				}
			}
		}
	} else {
		const integer myLocationInGroup = findMeInGroup (me);
		theGroupMembers [myLocationInGroup] = nullptr;
		theGroupSize --;
		Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
		my v_updateText ();
		FunctionEditor_redraw (me);   // for setting buttons in draw method
		updateGroup (me, true, true);
	}
}

static void HELP__intro (FunctionEditor /* me */, EDITOR_ARGS) {
	HELP (U"Intro")
}

void structFunctionEditor :: v_updateText () {
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_updateText ();
	}
}

void structFunctionEditor :: v_createMenuItems_prefs (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_prefs (menu);
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_createMenuItems_prefs (menu);
	}
}

void structFunctionEditor :: v_createMenuItems_save (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_save (menu);
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_createMenuItems_save (menu);
	}
}

void structFunctionEditor :: v_createMenuItems_edit (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_edit (menu);
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_createMenuItems_edit (menu);
	}
}

void structFunctionEditor :: v_createMenus () {
	FunctionEditor_Parent :: v_createMenus ();

	EditorMenu domainMenu = Editor_addMenu (this, v_format_domain (), 0);   // Time or Frequency

	EditorMenu_addCommand (domainMenu, U"Query visible part:", 0, nullptr);
	EditorMenu_addCommand (domainMenu, U"Get start of visible part", 1, QUERY_EDITOR_FOR_REAL__getStartOfVisiblePart);
	EditorMenu_addCommand (domainMenu, U"Get end of visible part", 1, QUERY_EDITOR_FOR_REAL__getEndOfVisiblePart);
	EditorMenu_addCommand (domainMenu, U"Get length of visible part", 1, QUERY_EDITOR_FOR_REAL__getLengthOfVisiblePart);

	EditorMenu_addCommand (domainMenu, U"- Set visible part:", 0, nullptr);
	EditorMenu_addCommand (domainMenu, U"Zoom and scroll settings...", 1, menu_cb_zoomAndScrollSettings);
	EditorMenu_addCommand (domainMenu, U"Zoom...", 1, menu_cb_zoom);
	EditorMenu_addCommand (domainMenu, U"Show all", 'A' | GuiMenu_DEPTH_1, menu_cb_showAll);
	EditorMenu_addCommand (domainMenu, U"Zoom in", 'I' | GuiMenu_DEPTH_1, menu_cb_zoomIn);
	EditorMenu_addCommand (domainMenu, U"Zoom out", 'O' | GuiMenu_DEPTH_1, menu_cb_zoomOut);
	EditorMenu_addCommand (domainMenu, U"Zoom to selection", 'N' | GuiMenu_DEPTH_1, menu_cb_zoomToSelection);
	EditorMenu_addCommand (domainMenu, U"Zoom back", 'B' | GuiMenu_DEPTH_1, menu_cb_zoomBack);
	EditorMenu_addCommand (domainMenu, U"Scroll page back", GuiMenu_PAGE_UP | GuiMenu_DEPTH_1, menu_cb_pageUp);
	EditorMenu_addCommand (domainMenu, U"Scroll page forward", GuiMenu_PAGE_DOWN | GuiMenu_DEPTH_1, menu_cb_pageDown);

	EditorMenu_addCommand (domainMenu, U"- Query selection:", 0, nullptr);
	EditorMenu_addCommand (domainMenu, U"Get start of selection || Get begin of selection",
			1, QUERY_EDITOR_FOR_REAL__getStartOfSelection);
	EditorMenu_addCommand (domainMenu, U"Get cursor",
			GuiMenu_F6 | GuiMenu_DEPTH_1, QUERY_EDITOR_FOR_REAL__getCursor);
	EditorMenu_addCommand (domainMenu, U"Get end of selection",
			1, QUERY_EDITOR_FOR_REAL__getEndOfSelection);
	EditorMenu_addCommand (domainMenu, U"Get length of selection || Get selection length",
			1, QUERY_EDITOR_FOR_REAL__getLengthOfSelection);

	EditorMenu_addCommand (domainMenu, U"- Set selection:", 0, nullptr);
	EditorMenu_addCommand (domainMenu, U"Select...",
			1, menu_cb_select);
	EditorMenu_addCommand (domainMenu, U"Widen or shrink selection...",
			1, menu_cb_widenOrShrinkSelection);
	EditorMenu_addCommand (domainMenu, U"Move cursor to start of selection || Move cursor to begin of selection",
			1, menu_cb_moveCursorToStartOfSelection);
	EditorMenu_addCommand (domainMenu, U"Move cursor to end of selection",
			1, menu_cb_moveCursorToEndOfSelection);
	EditorMenu_addCommand (domainMenu, U"Move cursor to...",
			1, menu_cb_moveCursorTo);
	EditorMenu_addCommand (domainMenu, U"Move cursor by...",
			1, menu_cb_moveCursorBy);
	EditorMenu_addCommand (domainMenu, U"Move start of selection by... || Move begin of selection by...",
			1, menu_cb_moveStartOfSelectionBy);
	EditorMenu_addCommand (domainMenu, U"Move end of selection by...",
			1, menu_cb_moveEndOfSelectionBy);
	EditorMenu_addCommand (domainMenu, U"Selection step settings...",
			1, menu_cb_selectionStepSettings);
	EditorMenu_addCommand (domainMenu, U"Select earlier",
			GuiMenu_DEPTH_1 | GuiMenu_UP_ARROW, menu_cb_selectEarlier);
	EditorMenu_addCommand (domainMenu, U"Select later",
			GuiMenu_DEPTH_1 | GuiMenu_DOWN_ARROW, menu_cb_selectLater);
	EditorMenu_addCommand (domainMenu, U"Move start of selection left || Move begin of selection left",
			GuiMenu_DEPTH_1 | GuiMenu_SHIFT | GuiMenu_UP_ARROW, menu_cb_moveStartOfSelectionLeft);
	EditorMenu_addCommand (domainMenu, U"Move start of selection right || Move begin of selection right",
			GuiMenu_DEPTH_1 | GuiMenu_SHIFT | GuiMenu_DOWN_ARROW, menu_cb_moveStartOfSelectionRight);
	EditorMenu_addCommand (domainMenu, U"Move end of selection left",
			GuiMenu_DEPTH_1 | GuiMenu_COMMAND | GuiMenu_UP_ARROW, menu_cb_moveEndOfSelectionLeft);
	EditorMenu_addCommand (domainMenu, U"Move end of selection right",
			GuiMenu_DEPTH_1 | GuiMenu_COMMAND | GuiMenu_DOWN_ARROW, menu_cb_moveEndOfSelectionRight);

	if (our v_hasPlayMenu ()) {
		our playMenu = Editor_addMenu (this, U"Play", 0);
		v_createMenuItems_play (our playMenu);
	}

	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_createMenus ();
	}
}

void structFunctionEditor :: v_updateMenuItems () {
	FunctionEditor_Parent :: v_updateMenuItems ();
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			area -> v_updateMenuItems ();
	}
}

void structFunctionEditor :: v_createMenuItems_help (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"Praat Intro", 0, HELP__intro);
}

void structFunctionEditor :: v_draw () {
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
		if (area)
			FunctionArea_drawOne (area);
	}
}

static void gui_drawingarea_cb_expose (FunctionEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	if (! my enableUpdates)
		return;
	#ifdef WIN32
		if (my duringPlay) {
			GraphicsScreen graphics = (GraphicsScreen) my graphics.get();
			HDC foregroundContext = graphics -> d_gdiGraphicsContext;
			static HDC backgroundContext;
			if (! my backgroundIsUpToDate) {
				/*
					Draw into background.
				*/
				static HBITMAP backgroundBitmap;
				if (backgroundBitmap)
					DeleteObject (backgroundBitmap);
				if (backgroundContext)
					DeleteDC (backgroundContext);
				backgroundContext = CreateCompatibleDC (foregroundContext);
				backgroundBitmap = CreateCompatibleBitmap (foregroundContext, my drawingArea -> d_widget -> width, my drawingArea -> d_widget -> height);
				SelectObject (backgroundContext, backgroundBitmap);
				SetBkMode (backgroundContext, TRANSPARENT);   // not the default!
				SelectPen (backgroundContext, GetStockPen (BLACK_PEN));
				SelectBrush (backgroundContext, GetStockBrush (BLACK_BRUSH));
				SetTextAlign (backgroundContext, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
				graphics -> d_gdiGraphicsContext = backgroundContext;
				drawBackgroundAndData (me);
				graphics -> d_gdiGraphicsContext = foregroundContext;
				my backgroundIsUpToDate = true;
			}
			/*
				Copy to foreground.
			*/
			BitBlt (foregroundContext, 0, 0, my drawingArea -> d_widget -> width, my drawingArea -> d_widget -> height, backgroundContext, 0, 0, SRCCOPY);
		} else {
			my backgroundIsUpToDate = false;
			drawBackgroundAndData (me);
		}
	#else
		drawBackgroundAndData (me);
	#endif

	/*
		Draw the running cursor.
	*/
	if (my duringPlay) {
		if (Melder_debug == 53) {
			static integer numberOfRunningCursorsDrawn = 0;
			numberOfRunningCursorsDrawn += 1;
			Melder_casual (U"playing cursor ", numberOfRunningCursorsDrawn);
		}
		my viewDataAsWorldByFraction ();
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		Graphics_setLineWidth (my graphics.get(), 3.0);
		Graphics_xorOn (my graphics.get(), Melder_BLACK);
		Graphics_line (my graphics.get(), my playCursor, 0.0, my playCursor, 1.0);
		Graphics_xorOff (my graphics.get());
		Graphics_setLineWidth (my graphics.get(), 1.0);
	}

	/*
		Draw the selection part.
	*/
	if (my instancePref_showSelectionViewer()) {
		/*
			Draw closing box.
		*/
		my viewAllAsPixelettes ();
		Graphics_setLineWidth (my graphics.get(), 1.0);
		const double left = my width_pxlt - my space + 9.0, right = my width_pxlt - 3.0;
		const double bottom = my height_pxlt - my space + 5.0, top = my height_pxlt - 5.0;
		Graphics_setColour (my graphics.get(), Melder_PINK);
		Graphics_fillRectangle (my graphics.get(), left, right, bottom, top);
		Graphics_setColour (my graphics.get(), Melder_GREY);
		Graphics_line (my graphics.get(), left + 2.0, bottom + 2.0, right - 2.0, top - 2.0);
		Graphics_line (my graphics.get(), left + 2.0, top - 2.0, right - 2.0, bottom + 2.0);
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		/*
			Draw content.
		*/
		my viewInnerSelectionViewerAsFractionByFraction ();
		if (my duringPlay)
			my v_drawRealTimeSelectionViewer (my playCursor);
		else
			my v_drawSelectionViewer ();
	}

	/*
		Not really drawing, but until further notice we believe that updating the menu items
		(i.e. making them sensitive or insensitive, or checking or unchecking them) could be done as often as redrawing.
	*/
	my v_updateMenuItems ();
}

bool FunctionEditor_defaultMouseInWideDataView (FunctionEditor me, GuiDrawingArea_MouseEvent event, double mouseTime) {
	my viewDataAsWorldByFraction ();
	Melder_assert (isdefined (mouseTime));
	Melder_assert (my startSelection <= my endSelection);
	Melder_clip (my startWindow, & mouseTime, my endWindow);   // WYSIWYG
	if (event -> isClick()) {
		/*
			Ignore any click that occurs during a drag,
			such as might occur when the user has both a mouse and a trackpad.
		*/
		if (isdefined (my anchorTime))
			return false;
		const double selectedMiddleTime = 0.5 * (my startSelection + my endSelection);
		const bool theyWantToExtendTheCurrentSelectionAtTheLeft =
				(event -> shiftKeyPressed && mouseTime < selectedMiddleTime) || event -> isLeftBottomFunctionKeyPressed();
		const bool theyWantToExtendTheCurrentSelectionAtTheRight =
				(event -> shiftKeyPressed && mouseTime >= selectedMiddleTime) || event -> isRightBottomFunctionKeyPressed();
		if (theyWantToExtendTheCurrentSelectionAtTheLeft) {
			my startSelection = mouseTime;
			my anchorTime = my endSelection;
		} else if (theyWantToExtendTheCurrentSelectionAtTheRight) {
			my endSelection = mouseTime;
			my anchorTime = my startSelection;
		} else {
			my startSelection = mouseTime;
			my endSelection = mouseTime;
			my anchorTime = mouseTime;
		}
		Melder_sort (& my startSelection, & my endSelection);
		Melder_assert (isdefined (my anchorTime));
	} else if (event -> isDrag() || event -> isDrop()) {
		/*
			Ignore any drag or drop that happens after a descendant preempted the above click handling.
		*/
		if (isundef (my anchorTime))
			return false;
		if (! my hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			const double distanceToAnchor_mm = fabs (Graphics_dxWCtoMM (my graphics.get(), mouseTime - my anchorTime));
			constexpr double vicinityRadius_mm = 1.0;
			if (distanceToAnchor_mm > vicinityRadius_mm)
				my hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = true;
		}
		if (my hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			my startSelection = std::min (my anchorTime, mouseTime);
			my endSelection = std::max (my anchorTime, mouseTime);
		}
		if (event -> isDrop()) {
			my anchorTime = undefined;
			my hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
		}
	}
	return true;
}

bool structFunctionEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	bool result = false;
	if (event -> isClick ())
		for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
			FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
			if (area)
				area -> isClickAnchor = area -> y_fraction_globalIsInside (globalY_fraction);
		}
	{// scope
		integer iarea = 1;
		for (; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
			FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
			if (area && area -> isClickAnchor) {
				const double localY_fraction = area -> y_fraction_globalToLocal (globalY_fraction);
				FunctionArea_setViewport (area);   // for Graphics_dxWCtoMM and the like
				result = area -> v_mouse (event, x_world, localY_fraction);
				break;
			}
		}
		if (iarea > FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS)
			result = FunctionEditor_defaultMouseInWideDataView (this, event, x_world);
	}
	if (event -> isDrop()) {
		for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
			FunctionArea area = static_cast <FunctionArea> (our functionAreas [iarea].get());
			if (area)
				area -> isClickAnchor = false;
		}
	}
	return result;
}

void structFunctionEditor :: v_clickSelectionViewer (double /* x_fraction */, double /* y_fraction */) {
}

static void gui_drawingarea_cb_mouse (FunctionEditor me, GuiDrawingArea_MouseEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	my viewAllAsPixelettes ();
	double x_pxlt, y_pxlt;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & x_pxlt, & y_pxlt);
	if (event -> isClick()) {
		if (my v_hasSelectionViewer() || my instancePref_showSelectionViewer()) {
			const double left = my width_pxlt - my space + 9.0, right = my width_pxlt - 3.0;
			const double bottom = my height_pxlt - my space + 5.0, top = my height_pxlt - 5.0;
			if (x_pxlt > left && x_pxlt < right && y_pxlt > bottom && y_pxlt < top) {
				my setInstancePref_showSelectionViewer (! my instancePref_showSelectionViewer());   // toggle
				my updateGeometry (GuiControl_getWidth (my drawingArea), GuiControl_getHeight (my drawingArea));
				FunctionEditor_redraw (me);
				return;
			}
		}
		my clickWasModifiedByShiftKey = event -> shiftKeyPressed;
		my clickWasModifiedByOptionKey = event -> optionKeyPressed;
		my clickWasModifiedByCommandKey = event -> commandKeyPressed;
		my anchorIsInSelectionViewer = my isInSelectionViewer (x_pxlt);
		my anchorIsInWideDataView = ( y_pxlt > my dataBottom_pxlt() && y_pxlt < my dataTop_pxlt() );
	}
	if (my anchorIsInSelectionViewer) {
		my viewInnerSelectionViewerAsFractionByFraction ();
		double x_fraction, y_fraction;
		Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & x_fraction, & y_fraction);
		if (event -> isClick()) {
			my v_clickSelectionViewer (x_fraction, y_fraction);
			//Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
			//my v_updateText ();
			FunctionEditor_redraw (me);
			updateGroup (me, false, false);   // TODO: why needed?
		} else;   // no dragging (yet?) in any selection viewer
	} else if (my anchorIsInWideDataView) {
		my viewDataAsWorldByFraction ();
		double x_world, y_fraction;
		Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & x_world, & y_fraction);
		my v_distributeAreas ();
		my v_mouseInWideDataView (event, x_world, y_fraction);   // this can change the selection, but not (yet?) the window (last checked 2023-02-13)
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_selectionMarksChanged()
		FunctionEditor_selectionMarksChanged (me);
	} else {   // clicked outside signal region? Let us hear it
		try {
			if (event -> isClick()) {
				for (integer i = 0; i < 8; i ++) {
					if (x_pxlt > my rect [i]. left && x_pxlt < my rect [i]. right && y_pxlt > my rect [i]. bottom && y_pxlt < my rect [i]. top) {
						switch (i) {
							case 0: my v_play (my tmin, my tmax); break;
							case 1: my v_play (my startWindow, my endWindow); break;
							case 2: my v_play (my tmin, my startWindow); break;
							case 3: my v_play (my endWindow, my tmax); break;
							case 4: my v_play (my startWindow, my marker [1]); break;
							case 5: my v_play (my marker [1], my marker [2]); break;
							case 6: my v_play (my marker [2], my marker [3]); break;
							case 7: my v_play (my startSelection, my endSelection); break;
						}
					}
				}
			} else;   // no dragging in the play rectangles
		} catch (MelderError) {
			Melder_flushError ();
		}
	}
}

void structFunctionEditor :: v_createChildren () {
	int x = BUTTON_X;

	/*
		Create zoom buttons.
	*/
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"all", gui_button_cb_showAll, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"in", gui_button_cb_zoomIn, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"out", gui_button_cb_zoomOut, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"sel", gui_button_cb_zoomToSelection, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"bak", gui_button_cb_zoomBack, this, 0);

	/*
		Create scroll bar.
	*/
	our scrollBar = GuiScrollBar_createShown (our windowForm,
		x += BUTTON_WIDTH + BUTTON_SPACING, -80 - BUTTON_SPACING, -4 - Gui_PUSHBUTTON_HEIGHT, 0,
		1, maximumScrollBarValue, 1, maximumScrollBarValue - 1, 1, 1,
		gui_cb_scroll, this, GuiScrollBar_HORIZONTAL);

	/*
		Create Group button.
	*/
	our groupButton = GuiCheckButton_createShown (our windowForm, -80, 0, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"Group", gui_checkbutton_cb_group, this, group_equalDomain (our tmin, our tmax) ? GuiCheckButton_SET : 0);

	/*
		Create optional text field.
	*/
	if (our v_hasText ()) {
		our textArea = GuiText_createShown (our windowForm, 0, 0,
			Machine_getMenuBarBottom (),
			Machine_getMenuBarBottom () + TEXT_HEIGHT,
			GuiText_INKWRAP | GuiText_SCROLLED
		);
		#if gtk
			Melder_assert (our textArea -> d_widget);
			gtk_widget_grab_focus (GTK_WIDGET (our textArea -> d_widget));   // BUG: can hardly be correct (the text should grab the focus of the window, not the global focus)
		#elif cocoa
			Melder_assert ([(NSView *) our textArea -> d_widget window]);
			//[[(NSView *) our textArea -> d_widget window] setInitialFirstResponder: (NSView *) our textArea -> d_widget];
			[[(NSView *) our textArea -> d_widget window] makeFirstResponder: (NSView *) our textArea -> d_widget];
		#endif
	}

	/*
		Create drawing area.
	*/
	#if cocoa
		int marginBetweenTextAndDrawingAreaToEnsureCorrectUnhighlighting = 3;
	#else
		int marginBetweenTextAndDrawingAreaToEnsureCorrectUnhighlighting = 0;
	#endif
	our drawingArea = GuiDrawingArea_createShown (our windowForm,
		0, 0,
		Machine_getMenuBarBottom () + ( our v_hasText () ? TEXT_HEIGHT + marginBetweenTextAndDrawingAreaToEnsureCorrectUnhighlighting : 0), -8 - Gui_PUSHBUTTON_HEIGHT,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_mouse,
		nullptr, gui_drawingarea_cb_resize, gui_drawingarea_cb_zoom, this, 0
	);
	GuiDrawingArea_setSwipable (our drawingArea, our scrollBar, nullptr);
}

void structFunctionEditor :: v1_dataChanged (Editor sender) {
	Melder_assert (our function());
	Melder_assert (Thing_isa (our function(), classFunction));
	if (! our group) {
		our tmin = our function() -> xmin;
 		our tmax = our function() -> xmax;
	}
 	if (our startWindow < our tmin || our startWindow > our tmax) {
 		our startWindow = our tmin;
 		our v_windowChanged ();
	}
 	if (our endWindow < our tmin || our endWindow > our tmax) {
 		our endWindow = our tmax;
 		our v_windowChanged ();
	}
 	if (our startWindow >= our endWindow) {
 		our startWindow = our tmin;
 		our endWindow = our tmax;
 		our v_windowChanged ();
	}
	Melder_clip (our tmin, & our startSelection, our tmax);
	Melder_clip (our tmin, & our endSelection, our tmax);
	Melder_assert (isdefined (our startSelection));   // precondition of v_updateText()
	our v_updateText ();
	updateScrollBar (this);
	FunctionEditor_redraw (this);
	//updateGroup (this, true, true);
}

int structFunctionEditor :: v_playCallback (int phase, double /* startTime */, double endTime, double currentTime) {
	Melder_assert (isdefined (currentTime));
	our playCursor = currentTime;
	if (phase == 1) {
		our duringPlay = true;
		return 1;
	}
	if (phase == 3) {
		our duringPlay = false;
		if (currentTime < endTime && MelderAudio_stopWasExplicit ()) {
			if (currentTime > our startSelection && currentTime < our endSelection)
				our startSelection = currentTime;
			else
				our startSelection = our endSelection = currentTime;
			Melder_assert (isdefined (our startSelection));   // precondition of v_updateText()
			our v_updateText ();
			updateGroup (this, false, true);
		}
	}
	if (Melder_debug == 53)
		Melder_casual (U"draining");
	Graphics_updateWs (our graphics.get());   // note: without setting my backgroundIsUpToDate to false
	GuiShell_drain (our windowForm, false, true);   // this may not be needed on all platforms (but on Windows it is, at least on 2020-09-21)
	return 1;
}

int theFunctionEditor_playCallback (FunctionEditor me, int phase, double startTime, double endTime, double currentTime) {
	return my v_playCallback (phase, startTime, endTime, currentTime);
}

void FunctionEditor_init (FunctionEditor me, conststring32 title, Function data) {
	Melder_assert (data);
	Thing_cast (Function, function, data);

	if (Melder_debug == 55)
		Melder_casual (Thing_messageNameAndAddress (me), U" init");
	my tmin = function -> xmin;   // set before adding children (see group button)
	my tmax = function -> xmax;
	Editor_init (me, 0, 0, my classPref_shellWidth(), my classPref_shellHeight(), title, function);

	my startWindow = my tmin;
	my endWindow = my tmax;
	my startSelection = my endSelection = 0.5 * (my tmin + my tmax);
	double maximumInitialLengthOfWindow = undefined;
	for (integer iarea = 1; iarea <= FunctionEditor_MAXIMUM_NUMBER_OF_FUNCTION_AREAS; iarea ++) {
		FunctionArea area = static_cast <FunctionArea> (my functionAreas [iarea].get());
		if (area) {
			const double areaMaximumInitialLengthOfWindow = area -> maximumInitialLengthOfWindow();
			if (isdefined (areaMaximumInitialLengthOfWindow) && ! (areaMaximumInitialLengthOfWindow >= maximumInitialLengthOfWindow))   // NaN-safe
				maximumInitialLengthOfWindow = areaMaximumInitialLengthOfWindow;
		}
	}
	if (isdefined (maximumInitialLengthOfWindow)) {
		if (my endWindow - my startWindow > maximumInitialLengthOfWindow) {
			my endWindow = my startWindow + maximumInitialLengthOfWindow;
			if (my startWindow == my tmin)
				my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
		}
	}

	#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
	#endif
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setFontSize (my graphics.get(), 12);

	my updateGeometry (GuiControl_getWidth (my drawingArea), GuiControl_getHeight (my drawingArea));

	Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
	//my v_updateText ();   // BUG: superfluous because of Editor_dataChanged below? and too early if function pointers not yet set?
	if (group_equalDomain (my tmin, my tmax))
		gui_checkbutton_cb_group (me, nullptr);   // BUG: nullptr
	my enableUpdates = true;   // BUG: explain why still needed

	Editor_dataChanged (me, me);   // only to self, not to the other editors; BUG: should be in Editor_init?
}

void FunctionEditor_windowMarksChanged (FunctionEditor me, const bool selectionChanged) {
	if (selectionChanged) {
		Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
		my v_updateText ();
	}
	updateScrollBar (me);
	FunctionEditor_redraw (me);
	updateGroup (me, true, selectionChanged);
}
void FunctionEditor_selectionMarksChanged (FunctionEditor me) {
	Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
	my v_updateText ();
	FunctionEditor_redraw (me);
	updateGroup (me, false, true);
}

void FunctionEditor_updateText (FunctionEditor me) {
	Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
	my v_updateText ();
}

void FunctionEditor_redraw (FunctionEditor me) {
	my backgroundIsUpToDate = false;
	Graphics_updateWs (my graphics.get());
}

void FunctionEditor_enableUpdates (FunctionEditor me, bool enable) {
	my enableUpdates = enable;
}

void FunctionEditor_ungroup (Daata data) {
	for (integer ieditor = 1; ieditor <= THE_MAXIMUM_GROUP_SIZE; ieditor ++) {
		const FunctionEditor me = theGroupMembers [ieditor];
		if (me && my group && my data() == data) {   // BUG: this may not be precise enough, in case an editor is editing multiple objects
			my group = false;
			GuiCheckButton_setValue (my groupButton, false);
			theGroupMembers [ieditor] = nullptr;
			theGroupSize --;
			Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
			my v_updateText ();
			FunctionEditor_redraw (me);   // for setting buttons in v_draw() method
		}
	}
}

void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment) {
	static MelderString text;
	MelderString_copy (& text, yWC_string, units);
	double textWidth = Graphics_textWidth (my graphics.get(), text.string) + Graphics_dxMMtoWC (my graphics.get(), 0.5);
	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_line (my graphics.get(), my endWindow, yWC, my endWindow + textWidth, yWC);
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, verticalAlignment);
	if (verticalAlignment == Graphics_BOTTOM)
		yWC -= Graphics_dyMMtoWC (my graphics.get(), 0.5);
	Graphics_text (my graphics.get(), my endWindow, yWC, text.string);
}

void FunctionEditor_insertCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, double minimum, double maximum) {
	double textX = my endWindow, textY = yWC;
	const bool tooHigh = ( Graphics_dyWCtoMM (my graphics.get(), maximum - textY) < 5.0 );
	const bool tooLow = ( Graphics_dyWCtoMM (my graphics.get(), textY - minimum) < 5.0 );
	if (yWC < minimum || yWC > maximum)
		return;
	Graphics_setColour (my graphics.get(), Melder_CYAN);
	Graphics_line (my graphics.get(), 0.99 * my endWindow + 0.01 * my startWindow, yWC, my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics.get(), 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	if (tooHigh) {
		if (tooLow)
			textY = 0.5 * (minimum + maximum);
		else
			textY = maximum - Graphics_dyMMtoWC (my graphics.get(), 5.0);
	} else if (tooLow) {
		textY = minimum + Graphics_dyMMtoWC (my graphics.get(), 5.0);
	}
	static MelderString text;
	MelderString_copy (& text, yWC_string, units);
	double textWidth = Graphics_textWidth (my graphics.get(), text.string);
	Graphics_fillCircle_mm (my graphics.get(), my endWindow + textWidth + Graphics_dxMMtoWC (my graphics.get(), 1.5), textY, 1.5);
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics.get(), textX, textY, text.string);
}

void FunctionEditor_drawHorizontalHair (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units) {
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_line (my graphics.get(), my startWindow, yWC, my endWindow, yWC);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics.get(), my startWindow, yWC,   yWC_string, units);
}

void FunctionEditor_drawGridLine (FunctionEditor me, double yWC) {
	Graphics_setColour (my graphics.get(), Melder_CYAN);
	Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
	Graphics_line (my graphics.get(), my startWindow, yWC, my endWindow, yWC);
	Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
}

/* End of file FunctionEditor.cpp */
