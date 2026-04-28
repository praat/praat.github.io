/* TextEditor.cpp
 *
 * Copyright (C) 1997-2026 Paul Boersma, 2010 Franz Brausse
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

#include "ScriptEditor.h"
#include "machine.h"
#include "../kar/longchar.h"
#include "EditorM.h"
#include "../kar/UnicodeData.h"

Thing_implement (TextEditor, Editor, 0);

#include "Prefs_define.h"
#include "TextEditor_prefs.h"
#include "Prefs_install.h"
#include "TextEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "TextEditor_prefs.h"

static CollectionOf <structTextEditor> theReferencesToAllOpenTextEditors;

/***** TextEditor methods *****/

void structTextEditor :: v9_destroy () noexcept {
	our openDialog. reset();   // don't delay till delete
	our saveDialog. reset();   // don't delay till delete
	theReferencesToAllOpenTextEditors. undangleItem (this);
	TextEditor_Parent :: v9_destroy ();
}

void structTextEditor :: v_nameChanged () {
	if (our v_fileBased ()) {
		/*
			We totally ignore the name that our boss wants to give us.
			Instead, we compose the window title from three ingredients:

			(1) whether we are already associated with a file or not;
			(2) if so, the full file path;
			(3) whether our text has been modified (i.e. whether we are "dirty").

			(last checked 2023-02-25)
		*/
		const bool dirtinessAlreadyShown = GuiWindow_setDirty (our windowForm, our dirty);
		static MelderString windowTitle;
		if (MelderFile_isNull (& our file)) {
			MelderString_copy (& windowTitle, U"(untitled");
			if (our dirty && ! dirtinessAlreadyShown)
				MelderString_append (& windowTitle, U", modified");
			MelderString_append (& windowTitle, U")");
		} else {
			MelderString_copy (& windowTitle, U"File ", MelderFile_messageName (& our file));
			if (our dirty && ! dirtinessAlreadyShown)
				MelderString_append (& windowTitle, U" (modified)");
		}
		GuiShell_setTitle (our windowForm, windowTitle.string);
	} else {
		/*
			We will set our window title to the name that our boss wants.
			This occurs e.g. in the Info window.
			(last checked 2023-02-25)
		*/
		TextEditor_Parent :: v_nameChanged ();
	}
}

#pragma mark - File menu

static void openDocument (TextEditor me, MelderFile file) {
	for (integer ieditor = 1; ieditor <= theReferencesToAllOpenTextEditors.size; ieditor ++) {
		TextEditor editor = theReferencesToAllOpenTextEditors.at [ieditor];
		if (editor != me && MelderFile_equal (file, & editor -> file)) {
			Editor_raise (editor);
			/*
				Destruction alarm!
				When we combine the destruction of an object with the presentation of a message,
				we shall always follow the "build message -- destroy -- show message" paradigm.
				Actually, in this case this is not only safe, but also crucial,
				because at the time of writing (2019-04-28) the owner of `file` is owned by `me`,
				so that destroying `me` would dangle `file`.
			*/
			Melder_appendError (U"Text file ", file, U" is already open.");
			forget (me);
			Melder_flushError ();
			return;
		}
	}
	autostring32 text = MelderFile_readText (file);
	GuiText_setString (my textWidget, text.get());
	/*
		GuiText_setString has invoked the changeCallback,
		which has set `my dirty` to `true`. Fix this.
	*/
	my dirty = false;
	MelderFile_copy (file, & my file);   // not until the file has been safely read
	Thing_setName (me, nullptr);
}

static void newDocument (TextEditor me) {
	GuiText_setString (my textWidget, U"");   // implicitly sets my dirty to `true`
	my dirty = false;
	MelderFile_setToNull (& my file);
	if (my v_fileBased ())
		Thing_setName (me, nullptr);
}

static void saveDocument (TextEditor me, MelderFile file) {
	autostring32 text = GuiText_getString (my textWidget);
	MelderFile_writeText (file, text.get(), Melder_getOutputEncoding ());
	my dirty = false;
	MelderFile_copy (file, & my file);   // not until the file has been safely written
	if (my v_fileBased ())
		Thing_setName (me, nullptr);
}

static void closeDocument (TextEditor me) {
	forget (me);
}

static void cb_open_ok (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me, Editor /* optionalEditor */)
{
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sendingForm);
	openDocument (me, file);
}

static void cb_showOpen (EditorCommand cmd) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	if (! my openDialog)
		my openDialog = UiInfile_create (my windowForm, nullptr, U"Open", cb_open_ok, me, nullptr, nullptr, false);
	UiInfile_do (my openDialog.get());
}

static void cb_saveAs_ok (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me, Editor /* optionalEditor */)
{
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sendingForm);
	saveDocument (me, file);
}

static void menu_cb_saveAs (TextEditor me, EDITOR_ARGS) {
	if (! my saveDialog)
		my saveDialog = UiOutfile_create (my windowForm, nullptr, U"Save", cb_saveAs_ok, me, nullptr, nullptr);
	char32 defaultName [300];
	Melder_sprint (defaultName,300,
		my v_fileBased () ?
			MelderFile_isNull (& my file) ?
				Melder_cat (U"untitled", my v_extension ())
			:
				MelderFile_name (& my file)
		:
			U"info.txt"
	);
	UiOutfile_do (my saveDialog.get(), defaultName);
}

static void gui_button_cb_saveAndOpen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyOpenDialog);
	if (MelderFile_isNull (& my file)) {
		menu_cb_saveAs (me, cmd, nullptr, 0, nullptr, nullptr, nullptr);
	} else {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
		cb_showOpen (cmd);
	}
}

static void gui_button_cb_cancelOpen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyOpenDialog);
}

static void gui_button_cb_discardAndOpen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyOpenDialog);
	cb_showOpen (cmd);
}

static void menu_cb_open (TextEditor me, EDITOR_ARGS) {
	if (my dirty) {
		if (! my dirtyOpenDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			my dirtyOpenDialog = GuiDialog_create (my windowForm,
				150, 70,
				Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_Modality::MODAL
			);
			GuiLabel_createShown (my dirtyOpenDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text has changed! Save changes?", 0
			);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Discard & Open", gui_button_cb_discardAndOpen, cmd, 0
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Cancel", gui_button_cb_cancelOpen, cmd, 0
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Save & Open", gui_button_cb_saveAndOpen, cmd, 0
			);
		}
		GuiThing_show (my dirtyOpenDialog);
	} else {
		cb_showOpen (cmd);
	}
}

static void gui_button_cb_saveAndNew (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyNewDialog);
	if (MelderFile_isNull (& my file)) {
		menu_cb_saveAs (me, cmd, nullptr, 0, nullptr, nullptr, nullptr);
	} else {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
		newDocument (me);
	}
}

static void gui_button_cb_cancelNew (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyNewDialog);
}

static void gui_button_cb_discardAndNew (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyNewDialog);
	newDocument (me);
}

static void menu_cb_new (TextEditor me, EDITOR_ARGS) {
	if (my v_fileBased () && my dirty) {
		if (! my dirtyNewDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			my dirtyNewDialog = GuiDialog_create (my windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_Modality::MODAL
			);
			GuiLabel_createShown (my dirtyNewDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text has changed! Save changes?", 0
			);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Discard & New", gui_button_cb_discardAndNew, cmd, 0
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Cancel", gui_button_cb_cancelNew, cmd, 0
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Save & New", gui_button_cb_saveAndNew, cmd, 0
			);
		}
		GuiThing_show (my dirtyNewDialog);
	} else {
		newDocument (me);
	}
}

static void gui_button_cb_cancelReopen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyReopenDialog);
}

static void gui_button_cb_discardAndReopen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyReopenDialog);
	openDocument (me, & my file);
}

static void menu_cb_reopen (TextEditor me, EDITOR_ARGS) {
	Melder_assert (my v_fileBased());
	if (MelderFile_isNull (& my file))
		Melder_throw (U"Cannot reopen from disk, because the text has never been saved yet.");
	if (my dirty) {
		if (! my dirtyReopenDialog) {
			int buttonWidth = 250, buttonSpacing = 20;
			my dirtyReopenDialog = GuiDialog_create (my windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 2 * buttonWidth + 1 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_Modality::MODAL
			);
			GuiLabel_createShown (my dirtyReopenDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text in the editor contains changes! Reopen nevertheless?", 0
			);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyReopenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Keep visible version", gui_button_cb_cancelReopen, cmd, GuiButton_CANCEL
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyReopenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Replace with version from disk", gui_button_cb_discardAndReopen, cmd, GuiButton_DEFAULT
			);
		}
		GuiThing_show (my dirtyReopenDialog);
	} else {
		try {
			openDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
	}
}

static void menu_cb_clear (TextEditor me, EDITOR_ARGS) {
	my v_clear ();
}

static void menu_cb_save (TextEditor me, EDITOR_ARGS) {
	if (MelderFile_isNull (& my file)) {
		menu_cb_saveAs (me, cmd, nullptr, 0, nullptr, nullptr, nullptr);
	} else {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
	}
}

static void gui_button_cb_saveAndClose (TextEditor me, GuiButtonEvent /* event */) {
	GuiThing_hide (my dirtyCloseDialog);
	if (MelderFile_isNull (& my file)) {
		menu_cb_saveAs (me, Editor_getMenuCommand (me, U"File", U"Save as..."), nullptr, 0, nullptr, nullptr, nullptr);
	} else {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
		closeDocument (me);
	}
}

static void gui_button_cb_cancelClose (TextEditor me, GuiButtonEvent /* event */) {
	GuiThing_hide (my dirtyCloseDialog);
}

static void gui_button_cb_discardAndClose (TextEditor me, GuiButtonEvent /* event */) {
	GuiThing_hide (my dirtyCloseDialog);
	closeDocument (me);
}

void structTextEditor :: v_goAway () {
	if (our v_fileBased () && our dirty) {
		if (! our dirtyCloseDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			our dirtyCloseDialog = GuiDialog_create (our windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_Modality::MODAL
			);
			GuiLabel_createShown (our dirtyCloseDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text has changed! Save changes?", 0
			);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (our dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Discard & Close", gui_button_cb_discardAndClose, this, 0
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (our dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Cancel", gui_button_cb_cancelClose, this, 0
			);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (our dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Save & Close", gui_button_cb_saveAndClose, this, 0
			);
		}
		if (our dirtyNewDialog)
			GuiThing_hide (our dirtyNewDialog);
		if (our dirtyOpenDialog)
			GuiThing_hide (our dirtyOpenDialog);
		if (our dirtyReopenDialog)
			GuiThing_hide (our dirtyReopenDialog);
		GuiThing_show (our dirtyCloseDialog);
	} else {
		closeDocument (this);
	}
}

#pragma mark - Edit menu

static void menu_cb_undo (TextEditor me, EDITOR_ARGS) {
	GuiText_undo (my textWidget);
}

static void menu_cb_redo (TextEditor me, EDITOR_ARGS) {
	GuiText_redo (my textWidget);
}

static void menu_cb_cut (TextEditor me, EDITOR_ARGS) {
	GuiText_cut (my textWidget);  // use ((XmAnyCallbackStruct *) call) -> event -> xbutton. time
}

static void menu_cb_copy (TextEditor me, EDITOR_ARGS) {
	GuiText_copy (my textWidget);
}

static void menu_cb_paste (TextEditor me, EDITOR_ARGS) {
	GuiText_paste (my textWidget);
}

static void menu_cb_erase (TextEditor me, EDITOR_ARGS) {
	GuiText_remove (my textWidget);
}

static bool getSelectedLines (TextEditor me, integer *firstLine, integer *lastLine) {
	integer left, right;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	const integer textLength = Melder_length (text.get());
	Melder_assert (left >= 0);
	Melder_assert (left <= right);
	if (right > textLength)
		Melder_crash (U"The end of the selection is at position ", right,
			U", which is beyond the end of the text, which is at position ", textLength, U".");
	integer i = 0;
	*firstLine = 1;
	/*
		Cycle through the text in order to see how many linefeeds we pass.
	*/
	for (; i < left; i ++)
		if (text [i] == U'\n')
			(*firstLine) ++;
	if (left == right)
		return false;
	*lastLine = *firstLine;
	for (; i < right - 1; i ++)   // a newline at the end of a line is ignored (it belongs to the previous line)
		if (text [i] == U'\n')
			(*lastLine) ++;
	return true;
}

static integer getPositionToInsertTabBeforeSelection (conststring32 text, const integer startingPosition) {
	if (startingPosition == 0)
		return 0;   // we will insert a tab at the start of the text
	for (integer position = startingPosition - 1; position >= 0; position --)
		if (text [position] == U'\n')
			return position + 1;   // we will insert a tab after the last newline
	return 0;   // we were on line 1 and will therefore insert a tab at the start of the text
}
static void menu_cb_shiftRight (TextEditor me, EDITOR_ARGS) {
	/*
		Get the old text from the GuiText.
	*/
	integer leftPosition, rightPosition;   // based between characters
	autostring32 oldText = GuiText_getStringAndSelectionPosition (my textWidget, & leftPosition, & rightPosition);

	/*
		Convert the old text to the new text.
	*/
	autoMelderString newText;
	const integer positionToInsertTabBeforeSelection = getPositionToInsertTabBeforeSelection (oldText.get(), leftPosition);
	MelderString_ncopy (& newText, oldText.get(), positionToInsertTabBeforeSelection);
	MelderString_appendCharacter (& newText, U'\t');
	MelderString_nappend (& newText, & oldText [positionToInsertTabBeforeSelection],
			leftPosition - positionToInsertTabBeforeSelection);
	for (integer position = leftPosition; position < rightPosition; position ++) {
		MelderString_appendCharacter (& newText, oldText [position]);
		if (position < rightPosition - 1 && oldText [position] == U'\n')   // a newline at the end of the selection is ignored (it belongs to the previous line)
			MelderString_appendCharacter (& newText, U'\t');
	}
	const integer newEndOfSelection = newText.length;
	MelderString_append (& newText, & oldText [rightPosition]);

	/*
		Put the new text into the GuiText.
	*/
	GuiText_setString (my textWidget, newText.string);
	GuiText_setSelection (my textWidget, leftPosition + 1, newEndOfSelection);
	GuiText_scrollToSelection (my textWidget);
	#ifdef _WIN32
		GuiThing_show (my windowForm);
	#endif
}

static integer getPositionToDeleteTabBeforeSelection (conststring32 text, const integer startingPosition) {
	if (startingPosition == 0) {
		if (text [startingPosition] == U'\t')
			return 0;   // we can delete a tab from the start of the text
		else
			return -1;   // we cannot delete a tab before the start of the text
	}
	for (integer position = startingPosition - 1; position >= 0; position --)
		if (text [position] == U'\n') {
			if (text [position + 1] == U'\t')
				return position + 1;   // we will delete a tab after the last newline
			else
				return -1;   // we will not look past the last newline
		}
	return text [0] == U'\t' ? 0 : -1;   // we were on line 1 and may therefore delete a tab at the start of the text
}
static void menu_cb_shiftLeft (TextEditor me, EDITOR_ARGS) {
	/*
		Get the old text from the GuiText.
	*/
	integer leftPosition, rightPosition;   // based between characters
	autostring32 oldText = GuiText_getStringAndSelectionPosition (my textWidget, & leftPosition, & rightPosition);

	/*
		Convert the old text to the new text.
		First the part before the selection.
	*/
	autoMelderString newText;
	const integer positionToDeleteTabBeforeSelection = getPositionToDeleteTabBeforeSelection (oldText.get(), leftPosition);
	const bool haveToDeleteTabBeforeSelection = ( positionToDeleteTabBeforeSelection >= 0 );
	if (positionToDeleteTabBeforeSelection == leftPosition) {
		MelderString_ncopy (& newText, oldText.get(), positionToDeleteTabBeforeSelection);
		// skip the tab
		constexpr integer numberOfDeletedTabsInThisPart = 0;
		MelderString_nappend (& newText, & oldText [positionToDeleteTabBeforeSelection + numberOfDeletedTabsInThisPart],
				leftPosition - (positionToDeleteTabBeforeSelection + numberOfDeletedTabsInThisPart));
	} else if (haveToDeleteTabBeforeSelection) {
		MelderString_ncopy (& newText, oldText.get(), positionToDeleteTabBeforeSelection);
		// skip the tab
		constexpr integer numberOfDeletedTabsInThisPart = 1;
		MelderString_nappend (& newText, & oldText [positionToDeleteTabBeforeSelection + numberOfDeletedTabsInThisPart],
				leftPosition - (positionToDeleteTabBeforeSelection + numberOfDeletedTabsInThisPart));
	} else {
		MelderString_ncopy (& newText, oldText.get(), leftPosition);
	}
	/*
		Then the part inside the selection.
	*/
	if (positionToDeleteTabBeforeSelection == leftPosition) {
		constexpr integer numberOfDeletedTabsInThisPart = 1;
		for (integer position = leftPosition + numberOfDeletedTabsInThisPart; position < rightPosition; position ++)
			if (oldText [position] != U'\t' || position > leftPosition && oldText [position - 1] != U'\n')
				MelderString_appendCharacter (& newText, oldText [position]);
	} else {
		constexpr integer numberOfDeletedTabsInThisPart = 0;
		for (integer position = leftPosition + numberOfDeletedTabsInThisPart; position < rightPosition; position ++)
			if (oldText [position] != U'\t' || position > leftPosition && oldText [position - 1] != U'\n')
				MelderString_appendCharacter (& newText, oldText [position]);
	}
	/*
		And finally the part after the selection.
	*/
	const integer newEndOfSelection = newText.length;
	if (positionToDeleteTabBeforeSelection == rightPosition)
		MelderString_append (& newText, & oldText [rightPosition + 1]);
	else
		MelderString_append (& newText, & oldText [rightPosition]);

	/*
		Put the new text into the GuiText.
	*/
	GuiText_setString (my textWidget, newText.string);
	GuiText_setSelection (my textWidget, leftPosition -
			( haveToDeleteTabBeforeSelection && positionToDeleteTabBeforeSelection != leftPosition ? 1 : 0 ), newEndOfSelection);
	GuiText_scrollToSelection (my textWidget);
	#ifdef _WIN32
		GuiThing_show (my windowForm);
	#endif
}

#pragma mark - Search menu

static autostring32 theFindString, theReplaceString;
static void do_find (TextEditor me) {
	if (! theFindString)   // e.g. when the user does "Find again" before having done any "Find"
		return;
	integer left, right;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	char32 *location = str32str (& text [right], theFindString.get());
	if (location) {
		const integer index = location - text.get();
		GuiText_setSelection (my textWidget, index, index + Melder_length (theFindString.get()));
		GuiText_scrollToSelection (my textWidget);
		#ifdef _WIN32
			GuiThing_show (my windowForm);
		#endif
	} else {
		/*
			Try from the start of the document.
		*/
		location = str32str (text.get(), theFindString.get());
		if (location) {
			const integer index = location - text.get();
			GuiText_setSelection (my textWidget, index, index + Melder_length (theFindString.get()));
			GuiText_scrollToSelection (my textWidget);
			#ifdef _WIN32
				GuiThing_show (my windowForm);
			#endif
		} else {
			Melder_beep ();
		}
	}
}

static void do_replace (TextEditor me) {
	if (! theReplaceString)   // e.g. when the user does "Replace again" before having done any "Replace"
		return;
	autostring32 selection = GuiText_getSelection (my textWidget);
	if (! Melder_equ (selection.get(), theFindString.get())) {
		do_find (me);
		return;
	}
	integer left, right;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	GuiText_replace (my textWidget, left, right, theReplaceString.get());
	GuiText_setSelection (my textWidget, left, left + Melder_length (theReplaceString.get()));
	GuiText_scrollToSelection (my textWidget);
	#ifdef _WIN32
		GuiThing_show (my windowForm);
	#endif
}

static void menu_cb_find (TextEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Find", nullptr)
		TEXTFIELD (findString, U"Find", U"", 5)
	EDITOR_OK
		if (theFindString) SET_STRING (findString, theFindString.get());
	EDITOR_DO
		theFindString = Melder_dup (findString);
		#ifdef macintosh
			/*
				Perhaps don't use the system-wide Find pasteboard,
				by which other applications can see what you are searching for in Praat's text windows.
				Remember ever showing your app in Xcode to somebody,
				revealing to your onlooker the name of the person you last looked up in your email?
			*/
			// NSPasteboard * theFindPasteBoard = [NSPasteboard pasteboardWithName: NSPasteboardNameFind   create: NO];
			// [theFindPasteBoard ...]
		#endif
		do_find (me);
	EDITOR_END
}

static void menu_cb_findAgain (TextEditor me, EDITOR_ARGS) {
	do_find (me);
}

static void menu_cb_useSelectionForFind (TextEditor me, EDITOR_ARGS) {
	theFindString = GuiText_getSelection (my textWidget);
}

static void menu_cb_replace (TextEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Find", nullptr)
		COMMENT (U"This is a \"slow\" find-and-replace method;")
		COMMENT (U"if the selected text is identical to the Find string,")
		COMMENT (U"the selected text will be replaced by the Replace string;")
		COMMENT (U"otherwise, the next occurrence of the Find string will be selected.")
		COMMENT (U"So you typically need two clicks on Apply to get a text replaced.")
		TEXTFIELD (findString, U"Find", U"", 5)
		TEXTFIELD (replaceString, U"Replace with", U"", 5)
	EDITOR_OK
		if (theFindString)
			SET_STRING (findString, theFindString.get());
		if (theReplaceString)
			SET_STRING (replaceString, theReplaceString.get());
	EDITOR_DO
		theFindString = Melder_dup (findString);
		theReplaceString = Melder_dup (replaceString);
		do_replace (me);
	EDITOR_END
}

static void menu_cb_replaceAgain (TextEditor me, EDITOR_ARGS) {
	do_replace (me);
}

static void menu_cb_whereAmI (TextEditor me, EDITOR_ARGS) {
	integer numberOfLinesLeft, numberOfLinesRight;
	if (! getSelectedLines (me, & numberOfLinesLeft, & numberOfLinesRight)) {
		Melder_information (U"The cursor is on line ", numberOfLinesLeft, U".");
	} else if (numberOfLinesLeft == numberOfLinesRight) {
		Melder_information (U"The selection is on line ", numberOfLinesLeft, U".");
	} else {
		Melder_information (U"The selection runs from line ", numberOfLinesLeft, U" to line ", numberOfLinesRight, U".");
	}
}

static void menu_cb_goToLine (TextEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Go to line", nullptr)
		NATURAL (lineToGo, U"Line", U"1")
	EDITOR_OK
		integer firstLine, lastLine;
		getSelectedLines (me, & firstLine, & lastLine);
		SET_INTEGER (lineToGo, firstLine)
	EDITOR_DO
		autostring32 text = GuiText_getString (my textWidget);
		integer currentLine = 1;
		integer left = 0, right = 0;
		if (lineToGo == 1) {
			for (; text [right] != U'\n' && text [right] != U'\0'; right ++) { }
		} else {
			for (; text [left] != U'\0'; left ++) {
				if (text [left] == U'\n') {
					currentLine ++;
					if (currentLine == lineToGo) {
						left ++;
						for (right = left; text [right] != U'\n' && text [right] != U'\0'; right ++) { }
						break;
					}
				}
			}
		}
		if (left == Melder_length (text.get()))
			right = left;
		else if (text [right] == U'\n')
			right ++;
		GuiText_setSelection (my textWidget, left, right);
		GuiText_scrollToSelection (my textWidget);
	EDITOR_END
}

#pragma mark - Convert menu

static void menu_cb_convertToCString (TextEditor me, EDITOR_ARGS) {
	autostring32 text = GuiText_getString (my textWidget);
	char32 buffer [2] = U" ";
	const conststring32 hex [16] = { U"0", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8", U"9", U"A", U"B", U"C", U"D", U"E", U"F" };
	MelderInfo_open ();
	MelderInfo_write (U"\"");
	for (char32 *p = & text [0]; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar == U'\n') {
			MelderInfo_write (U"\\n\"\n\"");
		} else if (kar == U'\t') {
			MelderInfo_write (U"   ");
		} else if (kar == U'\"') {
			MelderInfo_write (U"\\\"");
		} else if (kar == U'\\') {
			MelderInfo_write (U"\\\\");
		} else if (kar > 127) {
			if (kar <= 0x00FFFF)
				MelderInfo_write (U"\\u", hex [kar >> 12], hex [(kar >> 8) & 0x00'000F], hex [(kar >> 4) & 0x00'000F], hex [kar & 0x00'000F]);
			else
				MelderInfo_write (U"\\U",
					hex [kar >> 28], hex [(kar >> 24) & 0x00'000F],
					hex [(kar >> 20) & 0x00'000F], hex [(kar >> 16) & 0x00'000F],
					hex [(kar >> 12) & 0x00'000F], hex [(kar >> 8) & 0x00'000F],
					hex [(kar >> 4) & 0x00'000F], hex [kar & 0x00'000F]
				);
		} else {
			buffer [0] = *p;
			MelderInfo_write (& buffer [0]);
		}
	}
	MelderInfo_write (U"\"");
	MelderInfo_close ();
}

#pragma mark - Font menu

static void updateSizeMenu (TextEditor me) {
	if (my fontSizeButton_10)
		GuiMenuItem_check (my fontSizeButton_10, my instancePref_fontSize() == 10.0);
	if (my fontSizeButton_12)
		GuiMenuItem_check (my fontSizeButton_12, my instancePref_fontSize() == 12.0);
	if (my fontSizeButton_14)
		GuiMenuItem_check (my fontSizeButton_14, my instancePref_fontSize() == 14.0);
	if (my fontSizeButton_18)
		GuiMenuItem_check (my fontSizeButton_18, my instancePref_fontSize() == 18.0);
	if (my fontSizeButton_24)
		GuiMenuItem_check (my fontSizeButton_24, my instancePref_fontSize() == 24.0);
}
static void setFontSize (TextEditor me, double fontSize) {
	GuiText_setFontSize (my textWidget, fontSize);
	my setInstancePref_fontSize (fontSize);
	updateSizeMenu (me);
}

static void menu_cb_10 (TextEditor me, EDITOR_ARGS) { setFontSize (me, 10.0); }
static void menu_cb_12 (TextEditor me, EDITOR_ARGS) { setFontSize (me, 12.0); }
static void menu_cb_14 (TextEditor me, EDITOR_ARGS) { setFontSize (me, 14.0); }
static void menu_cb_18 (TextEditor me, EDITOR_ARGS) { setFontSize (me, 18.0); }
static void menu_cb_24 (TextEditor me, EDITOR_ARGS) { setFontSize (me, 24.0); }
static void menu_cb_fontSize (TextEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Text window: Font size", nullptr)
		POSITIVE (fontSize, U"Font size (points)", U"12")
	EDITOR_OK
		SET_REAL (fontSize, my instancePref_fontSize());
	EDITOR_DO
		setFontSize (me, fontSize);
	EDITOR_END
}

static void gui_text_cb_changed (TextEditor me, GuiTextEvent /* event */) {
	if (! my dirty) {
		my dirty = true;
		my v_nameChanged ();
	}
}

void structTextEditor :: v_createChildren () {
	textWidget = GuiText_createShown (our windowForm, 0, 0, Machine_getMenuBarBottom (), 0, GuiText_SCROLLED);
	GuiText_setChangedCallback (textWidget, gui_text_cb_changed, this);
}

void structTextEditor :: v_createMenus () {
	TextEditor_Parent :: v_createMenus ();

	if (v_fileBased ()) {
		Editor_addCommand (this, U"File", U"New", 'N', menu_cb_new);
		Editor_addCommand (this, U"File", U"Open...", 'O', menu_cb_open);
		Editor_addCommand (this, U"File", U"Reopen from disk", GuiMenu_COMMAND_EXTRA | 'O', menu_cb_reopen);
	} else {
		Editor_addCommand (this, U"File", U"Clear", 'N', menu_cb_clear);
	}
	Editor_addCommand (this, U"File", U"-- save --", 0, nullptr);
	if (v_fileBased ()) {
		Editor_addCommand (this, U"File", U"Save", 'S', menu_cb_save);
		Editor_addCommand (this, U"File", U"Save as...", 0, menu_cb_saveAs);
	} else {
		Editor_addCommand (this, U"File", U"Save as...", 'S', menu_cb_saveAs);
	}
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);
	GuiText_setUndoItem (textWidget, Editor_addCommand (this, U"Edit", U"Undo", 'Z', menu_cb_undo));
	GuiText_setRedoItem (textWidget, Editor_addCommand (this, U"Edit", U"Redo", 'Y', menu_cb_redo));
	Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Cut", 'X', menu_cb_cut);
	Editor_addCommand (this, U"Edit", U"Copy", 'C', menu_cb_copy);
	Editor_addCommand (this, U"Edit", U"Paste", 'V', menu_cb_paste);
	Editor_addCommand (this, U"Edit", U"Erase", 0, menu_cb_erase);
	Editor_addCommand (this, U"Edit", U"-- layout --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Shift right", ']', menu_cb_shiftRight);
	Editor_addCommand (this, U"Edit", U"Shift left", '[', menu_cb_shiftLeft);

	Editor_addMenu (this, U"Search", 0);
	Editor_addCommand (this, U"Search", U"Find...", 'F', menu_cb_find);
	Editor_addCommand (this, U"Search", U"Find again", 'G', menu_cb_findAgain);
	Editor_addCommand (this, U"Search", U"Replace...", GuiMenu_COMMAND_EXTRA | 'F', menu_cb_replace);
	Editor_addCommand (this, U"Search", U"Replace again", GuiMenu_COMMAND_EXTRA | 'G', menu_cb_replaceAgain);
	Editor_addCommand (this, U"Search", U"Use selection for find", 'E', menu_cb_useSelectionForFind);
	Editor_addCommand (this, U"Search", U"-- line --", 0, nullptr);
	Editor_addCommand (this, U"Search", U"Where am I?", 0, menu_cb_whereAmI);
	Editor_addCommand (this, U"Search", U"Go to line...", 'L', menu_cb_goToLine);

	Editor_addMenu (this, U"Convert", 0);
	Editor_addCommand (this, U"Convert", U"Convert to C string", 0, menu_cb_convertToCString);

	Editor_addMenu (this, U"Font", 0);
	Editor_addCommand (this, U"Font", U"Font size...", 0, menu_cb_fontSize);
	fontSizeButton_10 = Editor_addCommand (this, U"Font", U"10", GuiMenu_CHECKBUTTON, menu_cb_10);
	fontSizeButton_12 = Editor_addCommand (this, U"Font", U"12", GuiMenu_CHECKBUTTON, menu_cb_12);
	fontSizeButton_14 = Editor_addCommand (this, U"Font", U"14", GuiMenu_CHECKBUTTON, menu_cb_14);
	fontSizeButton_18 = Editor_addCommand (this, U"Font", U"18", GuiMenu_CHECKBUTTON, menu_cb_18);
	fontSizeButton_24 = Editor_addCommand (this, U"Font", U"24", GuiMenu_CHECKBUTTON, menu_cb_24);
}

void TextEditor_init (TextEditor me, conststring32 initialText) {
	/*
		The width of a text window should be at least one tab plus 84 single-width characters,
		if the font size is 12.
	*/
	constexpr double characterWidth =
		#if defined (macintosh)
			0.625 * 12;   // TODO: make a bit bigger
		#elif defined (_WIN32)
			0.689 * 12;
		#else
			0.584 * 12;   // TODO: make much bigger (e.g. as size 14 is now, as in what Terminal calls size 12)
		#endif
	constexpr double tabWidth =
		#if defined (_WIN32)
			8.0 * characterWidth;
		#elif defined (macintosh)
			4.0 * characterWidth;
		#else
			4.0 * characterWidth;
		#endif
	constexpr double numberOfFittingCharacters = 84.0;
	constexpr double overlapWithNextCharacter =
		#ifdef macintosh
			0.5 * characterWidth - 1;
		#else
			0.5 * characterWidth;
		#endif
	constexpr int width = int (tabWidth + numberOfFittingCharacters * characterWidth + overlapWithNextCharacter);
	Editor_init (me, 0, 0, width, 400, U"", nullptr);
	setFontSize (me, my instancePref_fontSize());
	if (initialText) {
		GuiText_setString (my textWidget, initialText);
		my dirty = false;   // was set to true in valueChanged callback
		if (my v_fileBased ())
			Thing_setName (me, nullptr);
	}
	theReferencesToAllOpenTextEditors. addItem_ref (me);
}

autoTextEditor TextEditor_create (conststring32 initialText) {
	try {
		autoTextEditor me = Thing_new (TextEditor);
		TextEditor_init (me.get(), initialText);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Text window not created.");
	}
}

#pragma mark - Export

void TextEditor_showOpen (TextEditor me) {
	cb_showOpen (Editor_getMenuCommand (me, U"File", U"Open..."));
}

/* End of file TextEditor.cpp */
