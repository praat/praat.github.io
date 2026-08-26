#ifndef _FunctionArea_h_
#define _FunctionArea_h_
/* FunctionArea.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015-2018,2020-2022,2026 Paul Boersma
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

#include "DataGui.h"
#include "FunctionEditor.h"

Thing_define (FunctionArea, DataGui) {
	/*
		Accessors.
	*/
public:   // all readonly
	Function function() const { Thing_cast (Function, function, our data()); return function; }
	FunctionEditor functionEditor() const { Thing_cast (FunctionEditor, functionEditor, our boss()); return functionEditor; }
	double startWindow() const { return our functionEditor() -> startWindow; }
	double endWindow() const { return our functionEditor() -> endWindow; }
	double startSelection() const { return our functionEditor() -> startSelection; }
	double endSelection() const { return our functionEditor() -> endSelection; }
	double tmin() const { return our functionEditor() -> tmin; }
	double tmax() const { return our functionEditor() -> tmax; }
	Graphics graphics() const { return our functionEditor() -> graphics.get(); }

	/*
		Initialization.
	*/
public:
	friend void FunctionArea_init (FunctionArea me, bool editable, Function optionalFunctionToCopy, FunctionEditor boss) {
		DataGui_init (me, nullptr, editable, boss);
		my _optionalFunctionCopy = Data_copy (optionalFunctionToCopy);
	}
	bool functionHasBeenCopied () {
		return !! our _optionalFunctionCopy;
	}
private:
	autoFunction _optionalFunctionCopy;

public:
	void functionChanged (Function newFunction) {
		our setData (our _optionalFunctionCopy ? our _optionalFunctionCopy.get() : newFunction);
		our v_invalidateAllDerivedDataCaches ();
		our v_computeAuxiliaryData ();
	}
	void invalidateAllDerivedDataCaches () {
		our v_invalidateAllDerivedDataCaches ();
	}
	virtual void v_windowChanged () { }

protected:
	virtual void v_invalidateAllDerivedDataCaches () { }   // derived classes can call inherited at end
	virtual void v_computeAuxiliaryData () { }   // derived classes can call inherited at start

public:
	/*
		To be called just before drawing or tracking:
	*/
	void setGlobalYRange_fraction (double ymin_fraction, double ymax_fraction) {
		our _ymin_fraction = ymin_fraction;
		our _ymax_fraction = ymax_fraction;
	}
	/*
		FunctionArea_setViewport() is used both for drawing and for mousing.
	*/
	friend void FunctionArea_setViewport (constFunctionArea me) {
		Graphics_setViewport (my graphics(), my left_pxlt(), my right_pxlt(), my bottom_pxlt(), my top_pxlt());
	}
	friend void FunctionArea_drawOne (FunctionArea me) {
		FunctionArea_prepareCanvas (me);
		FunctionArea_drawInside (me);
	}
	friend void FunctionArea_drawTwo (FunctionArea me, FunctionArea you) {
		FunctionArea_prepareCanvas (me);
		FunctionArea_drawInside (me);
		FunctionArea_drawInside (you);
	}
	friend void FunctionArea_prepareCanvas (FunctionArea me) {
		FunctionArea_setViewport (me);
		FunctionArea_drawBackground (me);
	}
	friend void FunctionArea_drawInside (FunctionArea me) {
		my v_drawInside ();
	}
	friend void FunctionArea_drawBackground (constFunctionArea me) {
		FunctionArea_eraseBackground (me);
		FunctionArea_highlightSelectionBackground (me);
		FunctionArea_drawFrame (me);
	}
	friend void FunctionArea_eraseBackground (constFunctionArea me) {
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics(), DataGuiColour_AREA_BACKGROUND);
		Graphics_fillRectangle (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics(), DataGuiColour_NONEDITABLE);
	}
	friend void FunctionArea_highlightSelectionBackground (constFunctionArea me) {
		Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), 0.0, 1.0);
		my v_specializedHighlightBackground ();
		double left = my startSelection(), right = my endSelection();
		if (left < right && left < my endWindow() && right > my startWindow()) {
			Melder_clipLeft (my startWindow(), & left);
			Melder_clipRight (& right, my endWindow());
			Graphics_highlight (my graphics(), left, right, 0.0, 1.0);
		}
	}
	friend void FunctionArea_drawFrame (constFunctionArea me) {
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		if (my editable()) {
			Graphics_setLineWidth (my graphics(), 2.0);
			Graphics_setColour (my graphics(), DataGuiColour_EDITABLE_FRAME);
		} else {
			Graphics_setLineWidth (my graphics(), 1.0);
			Graphics_setColour (my graphics(), DataGuiColour_NONEDITABLE_FRAME);
		}
		Graphics_rectangle (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics(), DataGuiColour_NONEDITABLE);
		Graphics_setLineWidth (my graphics(), 1.0);
	}
	friend void FunctionArea_drawRightMark (FunctionArea me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment);
	friend void FunctionArea_drawLegend_ (FunctionArea me,
		conststring32 cattableText1, MelderColour colour1,
		conststring32 cattableText2, MelderColour colour2,
		conststring32 cattableText3, MelderColour colour3,
		conststring32 cattableText4, MelderColour colour4
	);

private:
	virtual void v_specializedHighlightBackground () const { }
protected:
	virtual void v_drawInside () { }
public:
	void setSelection (double startSelection, double endSelection) {
		Melder_sort (& startSelection, & endSelection);
		our functionEditor() -> startSelection = startSelection;
		our functionEditor() -> endSelection = endSelection;
	}
	bool isClickAnchor = false;
	bool y_fraction_globalIsInside (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return y_pxlt >= our bottom_pxlt() && y_pxlt < our top_pxlt();
	}
	double y_fraction_globalToLocal (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return (y_pxlt - our bottom_pxlt()) / (our top_pxlt() - our bottom_pxlt());
	}
	double functionViewerLeft_WC () const {
		return our functionEditor() -> functionViewerLeft_WC();
	}
	double functionViewerRight_WC () const {
		return our functionEditor() -> functionViewerRight_WC();
	}
private:
	double _ymin_fraction, _ymax_fraction;
	double globalY_fraction_to_pxlt (double globalY_fraction) const {
		return our functionEditor() -> dataBottom_pxlt() +
				globalY_fraction * (our functionEditor() -> dataTop_pxlt() - our functionEditor() -> dataBottom_pxlt());
	}
	double left_pxlt() const { return our functionEditor() -> dataLeft_pxlt(); }
	double right_pxlt() const { return our functionEditor() -> dataRight_pxlt(); }
	double bottom_pxlt() const {
		return globalY_fraction_to_pxlt (_ymin_fraction);
	}
	double top_pxlt() const {
		constexpr double legendMargin = 23.0;
		return globalY_fraction_to_pxlt (our _ymax_fraction) - legendMargin;
	}
public:
	virtual void v_form_pictureSelection (EditorCommand);
	virtual void v_ok_pictureSelection (EditorCommand);
	virtual void v_do_pictureSelection (EditorCommand);

	virtual bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
		(void) localY_fraction;
		return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world);
	}

public:
	/*
		The data handling functions assume that editable functions
		are identical to or part of the data of the editor,
		hence that if the function changes, the data has changed.
	*/
	friend void FunctionArea_save (FunctionArea me, conststring32 undoText) {
		Melder_assert (my editable());
		Editor_save (my functionEditor(), undoText);
	}
	friend void FunctionArea_broadcastDataChanged (FunctionArea me) {
		Melder_assert (my editable());
		Editor_broadcastDataChanged (my boss());
	}

	virtual double maximumInitialLengthOfWindow() { return undefined; }

	virtual void v_updateText () { }

	#include "FunctionArea_prefs.h"
};

#define DEFINE_FunctionArea(which, FunctionAreaType, variableName) \
	auto##FunctionAreaType & variableName() { \
		return * reinterpret_cast <auto##FunctionAreaType *> (& our functionAreas [which]); \
	}

#define DEFINE_FunctionArea_create(FunctionAreaType, FunctionType) \
inline auto##FunctionAreaType FunctionAreaType##_create (bool editable, FunctionType functionToCopy, FunctionEditor boss) { \
	auto##FunctionAreaType me = Thing_new (FunctionAreaType); \
	FunctionArea_init (me.get(), editable, functionToCopy, boss); \
	return me; \
}

typedef MelderCallback <void, structFunctionArea, EditorCommand, UiForm, integer /*narg*/, Stackel /*args*/, conststring32,
		Interpreter> FunctionAreaCommandCallback;

GuiMenuItem FunctionAreaMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
		FunctionAreaCommandCallback commandCallback, FunctionArea commandBoss);

#if defined (macintosh)
	#define FunctionArea_legend_LINES  U"—"
	#define FunctionArea_legend_SPECKLES  U"• •"
	#define FunctionArea_legend_LINES_SPECKLES  U"–\\s{●}–"
	#define FunctionArea_legend_POLES  U" ⃒ ⃒ ⃒"
	#define FunctionArea_legend_WAVEFORM  U"\u301C"
	#define FunctionArea_legend_GREYS  U"\\s{▩}"
	#define FunctionArea_legend_TEXTGRID  U"☵"
#elif defined (_WIN32)
	#define FunctionArea_legend_LINES  U"—"
	#define FunctionArea_legend_SPECKLES  U"\\s{• •}"
	#define FunctionArea_legend_LINES_SPECKLES  U"–●–"
	#define FunctionArea_legend_POLES  U"|||"
	#define FunctionArea_legend_WAVEFORM  U"~"
	#define FunctionArea_legend_GREYS  U"\\s{▒}"
	#define FunctionArea_legend_TEXTGRID  U"☵"
#else
	#define FunctionArea_legend_LINES  U"—"
	#define FunctionArea_legend_SPECKLES  U"• •"
	#define FunctionArea_legend_LINES_SPECKLES  U"–●–"
	#define FunctionArea_legend_POLES  U"|||"
	#define FunctionArea_legend_WAVEFORM  U"\u301C"
	#define FunctionArea_legend_GREYS  U"\\s{▒}"
	#define FunctionArea_legend_TEXTGRID  U"☵"
#endif

inline void FunctionArea_drawLegend (FunctionArea me,
	conststring32 cattableText1, MelderColour colour1,
	conststring32 cattableText2 = U"", MelderColour colour2 = Melder_BLACK,
	conststring32 cattableText3 = U"", MelderColour colour3 = Melder_BLACK,
	conststring32 cattableText4 = U"", MelderColour colour4 = Melder_BLACK
) {
	FunctionArea_drawLegend_ (me,
		cattableText1, colour1,
		cattableText2, colour2,
		cattableText3, colour3,
		cattableText4, colour4
	);   // this looks strange, but friend function cannot have default parameters unless defined when declared
}

void FunctionArea_garnishPicture (FunctionArea me);   // optionally selection times and selection hairs

/* End of file FunctionArea.h */
#endif
