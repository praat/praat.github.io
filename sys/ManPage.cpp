/* ManPage.cpp
 *
 * Copyright (C) 1996-2011,2016,2023-2026 Paul Boersma
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

#include "ManPage.h"
#include "praat.h"

#include "enums_getText.h"
#include "ManPage_enums.h"
#include "enums_getValue.h"
#include "ManPage_enums.h"

Thing_implement (ManPage, Thing, 0);

static autoMelderString *manualInfoProc_string;
static void manualInfoProc (conststring32 infoText) {
	MelderString_copy (manualInfoProc_string, infoText);
			// FIXME: this overrides a growing info buffer, which is an O(N^2) algorithm if in a loop
}

void ManPage_runAllChunksToCache (ManPage me, Interpreter optionalInterpreterReference,
	const kGraphics_font font, const double fontSize,
	PraatApplication praatApplication, PraatObjects praatObjects, PraatPicture praatPicture,
	MelderFolder rootDirectory
) {
	theCurrentPraatApplication = praatApplication;
	theCurrentPraatApplication -> batch = true;   // prevent creation of editor windows
	theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
	theCurrentPraatObjects = praatObjects;
	Melder_assert (praatObjects -> n == 0);
	Melder_assert (praatObjects -> totalSelection == 0);
	Melder_assert (praatObjects -> totalBeingCreated == 0);
	praatObjects -> sequentialUniqueIdOfLatestObjectInList = 0;
	theCurrentPraatPicture = praatPicture;

	void praat_actions_show ();   // TODO: integrate this better
	praat_actions_show ();   // we have to set the `executable` flags to false, in the global variable `theActions`

	#if 1
		autoInterpreterStack interpreterStack = InterpreterStack_create (Editor (nullptr));   // BUG: overwrites existing stack, which is optionalInterpreterReference's owner
		interpreterStack -> interpreters [1] = Interpreter_createFromEnvironment (interpreterStack.get(), Editor (nullptr), MelderFile (nullptr));
	#else
		Interpreter interpreterReference;
		autoInterpreterStack interpreterStack;
		if (optionalInterpreterReference) {
			interpreterReference = optionalInterpreterReference;
		} else {
			interpreterStack = InterpreterStack_create (Editor (nullptr));
			interpreterStack -> interpreters [1] = Interpreter_createFromEnvironment (interpreterStack.get(), Editor (nullptr));
			interpreterReference = interpreterStack -> interpreters [1].get();
		}
	#endif

	/*
		When this page is drawn for the first time,
		all the script parts have to be run,
		so that the outputs of drawing and info can be cached.
	*/
	integer chunkNumber = 0;
	bool anErrorHasOccurred = false;
	autostring32 theErrorThatOccurred;
	integer errorChunk = 0;
	for (integer ipar = 1; ipar <= my paragraphs.size; ipar ++) {
		ManPage_Paragraph paragraph = & my paragraphs [ipar];
		if (paragraph -> type != kManPage_type::SCRIPT)
			continue;
		chunkNumber += 1;
		if (paragraph -> cacheGraphics)
			break;   // don't run the chunks again
		/*
			Divert info text from Info window to Manual window.
		*/
		autoMelderSetInformationProc divert (manualInfoProc);
		manualInfoProc_string = & paragraph -> cacheInfo;
		MelderInfo_open ();
		/*
			Divert graphics from Picture window to Manual window.
		*/
		paragraph -> cacheGraphics = Graphics_create_screen (nullptr, nullptr, 100);
		Graphics_startRecording (paragraph -> cacheGraphics.get());
		Graphics_setFont (paragraph -> cacheGraphics.get(), font);
		Graphics_setFontStyle (paragraph -> cacheGraphics.get(), 0);
		Graphics_setFontSize (paragraph -> cacheGraphics.get(), fontSize);
		const double true_width_inches  = paragraph -> width  * ( paragraph -> width  < 0.0 ? -1.0 : fontSize / 12.0 );
		const double true_height_inches = paragraph -> height * ( paragraph -> height < 0.0 ? -1.0 : fontSize / 12.0 );
		Graphics_setWrapWidth (paragraph -> cacheGraphics.get(), 0.0);
		integer x1DCold, x2DCold, y1DCold, y2DCold;
		Graphics_inqWsViewport (paragraph -> cacheGraphics.get(), & x1DCold, & x2DCold, & y1DCold, & y2DCold);
		double x1NDCold, x2NDCold, y1NDCold, y2NDCold;
		Graphics_inqWsWindow (paragraph -> cacheGraphics.get(), & x1NDCold, & x2NDCold, & y1NDCold, & y2NDCold);

		theCurrentPraatPicture -> graphics = paragraph -> cacheGraphics.get();   // has to draw into HyperPage rather than Picture window
		theCurrentPraatPicture -> font = font;
		theCurrentPraatPicture -> fontSize = fontSize;
		theCurrentPraatPicture -> lineType = Graphics_DRAWN;
		theCurrentPraatPicture -> lineWidth = 1.0;
		theCurrentPraatPicture -> arrowSize = 1.0;
		theCurrentPraatPicture -> speckleSize = 1.0;
		theCurrentPraatPicture -> colour = Melder_BLACK;
		#if 1
		theCurrentPraatPicture -> x1NDC = 0.0;
		theCurrentPraatPicture -> x2NDC = true_width_inches;
		theCurrentPraatPicture -> y1NDC = 0.0;
		theCurrentPraatPicture -> y2NDC = true_height_inches;

		Graphics_setViewport (paragraph -> cacheGraphics.get(),
				theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
		#endif
		Graphics_setWindow (paragraph -> cacheGraphics.get(), 0.0, 1.0, 0.0, 1.0);
		#if 1
		integer x1DC, y1DC, x2DC, y2DC;
		Graphics_WCtoDC (paragraph -> cacheGraphics.get(), 0.0, 0.0, & x1DC, & y2DC);
		Graphics_WCtoDC (paragraph -> cacheGraphics.get(), 1.0, 1.0, & x2DC, & y1DC);
		Graphics_resetWsViewport (paragraph -> cacheGraphics.get(), x1DC, x2DC, y1DC, y2DC);
		Graphics_setWsWindow (paragraph -> cacheGraphics.get(), 0, paragraph -> width, 0, paragraph -> height);
		#endif
		theCurrentPraatPicture -> x1NDC = 0.0;
		theCurrentPraatPicture -> x2NDC = paragraph -> width;
		theCurrentPraatPicture -> y1NDC = 0.0;
		theCurrentPraatPicture -> y2NDC = paragraph -> height;
		Graphics_setViewport (paragraph -> cacheGraphics.get(),
				theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
		if (anErrorHasOccurred) {
			trace (U"Chunk ", chunkNumber, U" not run, because of an earlier error.");
			MelderInfo_writeLine (U"\\#{**ERROR** This code chunk was not run,}\n    because an error occurred in an earlier chunk.");
			MelderInfo_close ();
		} else {
			autoMelderProgressOff progress;
			autoMelderWarningOff nowarn;
			autoMelderSaveCurrentFolder saveFolder;
			if (! MelderFolder_isNull (rootDirectory))
				Melder_setCurrentFolder (rootDirectory);
			try {
				autoMelderString program;
				MelderString_append (& program, paragraph -> text);
				for (integer jpar = 1; jpar <= my paragraphs.size; jpar ++) if (jpar != ipar) {
					ManPage_Paragraph otherParagraph = & my paragraphs [jpar];
					if (otherParagraph -> type == kManPage_type::SCRIPT)
						if (Melder_startsWith (otherParagraph -> text, U"\tprocedure ") && Melder_endsWith (otherParagraph -> text, U"\tendproc\n"))
							MelderString_append (& program, otherParagraph -> text);
				}
				//Interpreter_rememberNotebook (interpreterStack -> interpreters [1], <#MelderFile notebookFile#>, false);
				interpreterStack -> runDown (autoInterpreter(), Melder_dup (program.string), chunkNumber > 1);
				//interpreterStack -> runDown (interpreterStack -> interpreters [1], Melder_dup (program.string), chunkNumber > 1);
			} catch (MelderError) {
				anErrorHasOccurred = true;
				errorChunk = chunkNumber;
				theErrorThatOccurred = Melder_dup (Melder_getError ());
				trace (U"Error in chunk ", chunkNumber, U".");
				Melder_clearError ();
				MelderInfo_writeLine (U"\\#{**AN ERROR OCCURRED IN THIS CODE CHUNK:**\n", theErrorThatOccurred.get());
				MelderInfo_close ();
			}
		}
		#if 0
		Graphics_setLineType (paragraph -> cacheGraphics.get(), Graphics_DRAWN);
		Graphics_setLineWidth (paragraph -> cacheGraphics.get(), 1.0);
		Graphics_setArrowSize (paragraph -> cacheGraphics.get(), 1.0);
		Graphics_setSpeckleSize (paragraph -> cacheGraphics.get(), 1.0);
		Graphics_setColour (paragraph -> cacheGraphics.get(), Melder_BLACK);
		#endif

		#if 0
		Graphics_resetWsViewport (paragraph -> cacheGraphics.get(), x1DCold, x2DCold, y1DCold, y2DCold);
		Graphics_setWsWindow (paragraph -> cacheGraphics.get(), x1NDCold, x2NDCold, y1NDCold, y2NDCold);
		Graphics_setViewport (paragraph -> cacheGraphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setWindow (paragraph -> cacheGraphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (paragraph -> cacheGraphics.get(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_stopRecording (paragraph -> cacheGraphics.get());
		#endif
	}
	theCurrentPraatApplication = & theForegroundPraatApplication;
	theCurrentPraatObjects = & theForegroundPraatObjects;
	theCurrentPraatPicture = & theForegroundPraatPicture;
	if (anErrorHasOccurred)
		Melder_flushError (U"Error in code chunk ", errorChunk, U".\n", theErrorThatOccurred.get());
	praatObjects -> reset();
}

/* End of file ManPage.cpp */
