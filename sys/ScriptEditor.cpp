/* ScriptEditor.cpp
 *
 * Copyright (C) 1997-2005,2007-2018,2020-2026 Paul Boersma
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
#include "../kar/longchar.h"
#include "praatP.h"
#include "EditorM.h"
#include "UiPause.h"

Thing_implement (ScriptEditor, TextEditor, 0);

CollectionOf <structScriptEditor> theReferencesToAllOpenScriptEditors;

bool ScriptEditors_dirty () {
	for (integer i = 1; i <= theReferencesToAllOpenScriptEditors.size; i ++) {
		const ScriptEditor me = theReferencesToAllOpenScriptEditors.at [i];
		if (my dirty)
			return true;
	}
	return false;
}

void structScriptEditor :: v9_destroy () noexcept {
	our argsDialog. reset();   // don't delay till delete
	theReferencesToAllOpenScriptEditors. undangleItem (this);
	ScriptEditor_Parent :: v9_destroy ();
}

void structScriptEditor :: v_nameChanged () {
	/*
		As TextEditor does, we totally ignore the name that our boss wants to give us.
		Instead, we compose the window title from five ingredients, i.e. two more than TextEditor:

		(1) whether we are already associated with a file or not;
		(2) if so, the full file path;
		(3) whether our text has been modified (i.e. whether we are "dirty");
		(4) whether we have a boss, i.e. whether or not we were created from a data editor;
		(5) if so, the name of our boss, i.e. of the data editor that we were created from.

		(last checked 2023-02-25)
	*/
	const bool dirtinessAlreadyShown = GuiWindow_setDirty (our windowForm, our dirty);   // (3) on the Mac (last checked 2023-02-25)
	static MelderString buffer;
	MelderString_copy (& buffer, MelderFile_isNull (& our file) ? U"untitled script" : U"Script");   // (1)
	if (our wasCreatedInAnEditor()) {
		if (our optionalReferenceToOwningEditor)
			MelderString_append (& buffer, U" [editor “", Thing_getName (our optionalReferenceToOwningEditor), U"”]");   // (4), (5)
		else
			MelderString_append (& buffer, U" [closed ", our optionalOwningEditorClassName.get(), U"]");   // (4)
	}
	if (! MelderFile_isNull (& our file))
		MelderString_append (& buffer, U" ", MelderFile_messageName (& our file));   // (2)
	if (our dirty && ! dirtinessAlreadyShown)
		MelderString_append (& buffer, U" (modified)");   // (3) on Windows and Linux (last checked 2023-02-25)
	GuiShell_setTitle (windowForm, buffer.string);
	/*
		Finally, remember the name of this script.
	*/
	if (! MelderFile_isNull (& our file)) {
		autoScript script = Script_createFromFile (& our file);
		Script_rememberDuringThisAppSession_move (script.move());
	}
}

static void common_args_ok (UiForm sendingForm, void *void_me, Editor optionalEditor, const bool fullTrust) {
	iam (ScriptEditor);
	autostring32 text = GuiText_getString (my textWidget);
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Melder_includeIncludeFiles (& text);

	Melder_assert (my optionalReferenceToOwningEditor == optionalEditor);   // TODO: if never fires, then redundancy
	Melder_assert (my interpreterStack);
	Melder_assert (my interpreterStack -> interpreters [1]);
	//Melder_assert (interpreter);   // TODO: figure out why this is usually null
	Interpreter_getArgumentsFromDialog (my interpreterStack -> interpreters [1].get(), sendingForm);

	autoPraatBackground background;
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Interpreter_rememberScript (my interpreterStack -> interpreters [1].get(), & my file, fullTrust);   // right before running! (not at interpreter creation, because the name may change)
	my interpreterStack -> runDown (autoInterpreter(), text.move(), false);
}
static void args_ok (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me, Editor optionalEditor)
{
	common_args_ok (sendingForm, void_me, optionalEditor, false);
}
static void args_ok_FULL_TRUST (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me, Editor optionalEditor)
{
	common_args_ok (sendingForm, void_me, optionalEditor, true);
}

static void common_args_ok_selectionOnly (UiForm sendingForm, void *void_me, Editor optionalEditor, const bool fullTrust)
{
	iam (ScriptEditor);
	autostring32 text = GuiText_getSelection (my textWidget);
	if (! text)
		Melder_throw (U"No text is selected any longer.\nPlease reselect or click Cancel.");
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Melder_includeIncludeFiles (& text);

	Melder_assert (my optionalReferenceToOwningEditor == optionalEditor);   // TODO: if never fires, then redundancy
	Melder_assert (my interpreterStack);
	Melder_assert (my interpreterStack -> interpreters [1]);
	//Melder_assert (interpreter);   // TODO: figure out why this is usually null
	Interpreter_getArgumentsFromDialog (my interpreterStack -> interpreters [1].get(), sendingForm);

	autoPraatBackground background;
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Interpreter_rememberScript (my interpreterStack -> interpreters [1].get(), & my file, fullTrust);
	my interpreterStack -> runDown (autoInterpreter(), text.move(), false);
}
static void args_ok_selectionOnly (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me, Editor optionalEditor)
{
	common_args_ok_selectionOnly (sendingForm, void_me, optionalEditor, false);
}
static void args_ok_selectionOnly_FULL_TRUST (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me, Editor optionalEditor)
{
	common_args_ok_selectionOnly (sendingForm, void_me, optionalEditor, true);
}

static void common_menu_cb_run (ScriptEditor me, const bool fullTrust) {
	try {
		Melder_assert (my interpreterStack);
		my interpreterStack -> emptyAll ();
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			my interpreterStack.get(),
			my optionalReferenceToOwningEditor,
			& my file
		);
		autostring32 text = GuiText_getString (my textWidget);
		if (! MelderFile_isNull (& my file))
			MelderFile_setDefaultDir (& my file);   // TODO: can be wrong
		Melder_includeIncludeFiles (& text);
		const integer npar = Interpreter_readParameters (interpreter.get(), text.get());
		if (npar != 0) {
			/*
				Pop up a dialog box for querying the arguments.
			*/
			my argsDialog = Interpreter_createForm (interpreter.get(), my windowForm, my optionalReferenceToOwningEditor, nullptr,
					fullTrust ? args_ok_FULL_TRUST : args_ok, me, false);   // trust setting cannot be done earlier 
			my interpreterStack -> interpreters [1] = interpreter.move();
			UiForm_do (my argsDialog.get(), false);
		} else {
			autoPraatBackground background;
			if (! MelderFile_isNull (& my file))
				MelderFile_setDefaultDir (& my file);
			Interpreter_rememberScript (interpreter.get(), & my file, fullTrust);
			Melder_assert (interpreter -> owningInterpreterStack);
			interpreter -> owningInterpreterStack -> emptyAll ();   // TODO: should we create a new InterpreterStack instead, owned by the script editor?
			interpreter -> owningInterpreterStack -> runDown (interpreter.move(), text.move(), false);
		}
	} catch (MelderError) {
		Melder_flushError (U"The script didn’t run to its completion.");
	}
}
static void menu_cb_run (ScriptEditor me, EDITOR_ARGS) {
	common_menu_cb_run (me, false);
}
static void menu_cb_runWithFullTrust (ScriptEditor me, EDITOR_ARGS) {
	common_menu_cb_run (me, true);
}

static void common_menu_cb_runSelection (ScriptEditor me, const bool fullTrust) {
	try {
		Melder_assert (my interpreterStack);
		my interpreterStack -> emptyAll ();
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			my interpreterStack.get(),
			my optionalReferenceToOwningEditor,
			& my file
		);
		autostring32 selectedText = GuiText_getSelection (my textWidget);
		if (! selectedText)
			Melder_throw (U"No text selected.");
		if (! MelderFile_isNull (& my file))
			MelderFile_setDefaultDir (& my file);
		Melder_includeIncludeFiles (& selectedText);
		const integer npar = Interpreter_readParameters (interpreter.get(), selectedText.get());
		/*
			TODO: check that no two procedures have the same name
		*/

		/*
			Add all the procedures to the selected text.
			A procedure is counted as any text that honours the following conditions:
			- it starts with a line starting with the text "procedure"
			  optionally preceded by whitespace and obligatorily followed by whitespace;
			- it ends with a line starting with the text "endproc"
			  optionally preceded by whitespace and obligatorily followed by null or whitespace.
		*/
		autoMelderString textPlusProcedures;
		MelderString_copy (& textPlusProcedures, selectedText.get());
		{// scope
			autostring32 wholeText = GuiText_getString (my textWidget);
			autoMelderReadText textReader = MelderReadText_createFromText (wholeText.move());
			int procedureDepth = 0;
			for (;;) {
				const conststring32 line = MelderReadText_readLine (textReader.get());
				if (! line)
					break;
				const conststring32 startOfCode = Melder_findEndOfHorizontalSpace (line);
				if (Melder_startsWith (startOfCode, U"procedure") && Melder_isHorizontalSpace (startOfCode [9]))
					procedureDepth += 1;
				if (procedureDepth > 0)
					MelderString_append (& textPlusProcedures, U"\n", line);
				if (Melder_startsWith (startOfCode, U"endproc") && (startOfCode [7] == U'\0' || Melder_isHorizontalSpace (startOfCode [7])))
					procedureDepth -= 1;
			}
		}

		if (npar != 0) {
			/*
				Pop up a dialog box for querying the arguments.
			*/
			my argsDialog = Interpreter_createForm (interpreter.get(), my windowForm, my optionalReferenceToOwningEditor, nullptr, args_ok_selectionOnly, me, true);
			my interpreterStack -> interpreters [1] = interpreter.move();
			UiForm_do (my argsDialog.get(), false);
		} else {
			autoPraatBackground background;
			if (! MelderFile_isNull (& my file))
				MelderFile_setDefaultDir (& my file);
			Interpreter_rememberScript (interpreter.get(), & my file, fullTrust);
			Melder_assert (interpreter -> owningInterpreterStack);
			interpreter -> owningInterpreterStack -> emptyAll ();   // TODO: should we create a new InterpreterStack instead, owned by the script editor?
			interpreter -> owningInterpreterStack -> runDown (interpreter.move(), Melder_dup (textPlusProcedures.string), false);
		}
	} catch (MelderError) {
		Melder_flushError (U"The script selection didn’t run to its completion.");
	}
}
static void menu_cb_runSelection (ScriptEditor me, EDITOR_ARGS) {
	common_menu_cb_runSelection (me, false);
}
static void menu_cb_runSelectionWithFullTrust (ScriptEditor me, EDITOR_ARGS) {
	common_menu_cb_runSelection (me, true);
}

static void menu_cb_addToMenu (ScriptEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add to menu", U"Add to fixed menu...")
		WORD (window, U"Window", U"?")
		SENTENCE (menu, U"Menu", U"File")
		SENTENCE (command, U"Command", U"Do it...")
		SENTENCE (afterCommand, U"After command", U"")
		INTEGER (depth, U"Depth", U"0")
		INFILE (scriptFile, U"Script file", U"")
	EDITOR_OK
		if (my wasCreatedInAnEditor())
			SET_STRING (window, my optionalOwningEditorClassName.get())
		if (MelderFile_isNull (& my file))
			SET_STRING (scriptFile, U"(please save your script first)")
		else
			SET_STRING (scriptFile, MelderFile_peekPath (& my file));
	EDITOR_DO
		praat_addMenuCommandScript (window, menu, command, afterCommand, depth, scriptFile);
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToFixedMenu (ScriptEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add to fixed menu", U"Add to fixed menu...");
		CHOICESTR (window, U"Window", 1)
			OPTION (U"Objects")
			OPTION (U"Picture")
		SENTENCE (menu, U"Menu", U"New")
		SENTENCE (command, U"Command", U"Do it...")
		SENTENCE (afterCommand, U"After command", U"")
		INTEGER (depth, U"Depth", U"0")
		INFILE (scriptFile, U"Script file", U"")
	EDITOR_OK
		if (MelderFile_isNull (& my file))
			SET_STRING (scriptFile, U"(please save your script first)")
		else
			SET_STRING (scriptFile, MelderFile_peekPath (& my file))
	EDITOR_DO
		praat_addMenuCommandScript (window, menu, command, afterCommand, depth, scriptFile);
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToDynamicMenu (ScriptEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add to dynamic menu", U"Add to dynamic menu...")
		WORD (class1, U"Class 1", U"Sound")
		INTEGER (number1, U"Number 1", U"0")
		WORD (class2, U"Class 2", U"")
		INTEGER (number2, U"Number 2", U"0")
		WORD (class3, U"Class 3", U"")
		INTEGER (number3, U"Number 3", U"0")
		SENTENCE (command, U"Command", U"Do it...")
		SENTENCE (afterCommand, U"After command", U"")
		INTEGER (depth, U"Depth", U"0")
		INFILE (scriptFile, U"Script file", U"")
	EDITOR_OK
		if (MelderFile_isNull (& my file))
			SET_STRING (scriptFile, U"(please save your script first)")
		else
			SET_STRING (scriptFile, MelderFile_peekPath (& my file))
	EDITOR_DO
		praat_addActionScript (class1, number1, class2, number2, class3, number3, command, afterCommand, depth, scriptFile);
		praat_show ();
	EDITOR_END
}

static void menu_cb_clearHistory (ScriptEditor /* me */, EDITOR_ARGS) {
	UiHistory_clear ();
}

static void menu_cb_pasteHistory (ScriptEditor me, EDITOR_ARGS) {
	char32 *history = UiHistory_get ();
	if (! history || history [0] == U'\0')
		Melder_throw (U"No history.");
	integer length = Melder_length (history);
	if (history [length - 1] != U'\n') {
		UiHistory_write (U"\n");
		history = UiHistory_get ();
		length = Melder_length (history);
	}
	if (history [0] == U'\n') {
		history ++;
		length --;
	}
	integer first = 0, last = 0;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & first, & last);
	GuiText_replace (my textWidget, first, last, history);
	GuiText_setSelection (my textWidget, first, first + length);
	GuiText_scrollToSelection (my textWidget);
}

static void menu_cb_expandIncludeFiles (ScriptEditor me, EDITOR_ARGS) {
	autostring32 text = GuiText_getString (my textWidget);
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Melder_includeIncludeFiles (& text);
	GuiText_setString (my textWidget, text.get());
}

static void menu_cb_AboutScriptEditor (ScriptEditor, EDITOR_ARGS) { Melder_help (U"ScriptEditor"); }
static void menu_cb_ScriptingTutorial (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Scripting"); }
static void menu_cb_ScriptingExamples (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Scripting examples"); }
static void menu_cb_PraatScript (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Praat script"); }
static void menu_cb_FormulasTutorial (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Formulas"); }
static void menu_cb_Functions (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Functions"); }
static void menu_cb_DemoWindow (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Demo window"); }
static void menu_cb_TheHistoryMechanism (ScriptEditor, EDITOR_ARGS) { Melder_help (U"History mechanism"); }
static void menu_cb_InitializationScripts (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Initialization script"); }
static void menu_cb_AddingToAFixedMenu (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Add to fixed menu..."); }
static void menu_cb_AddingToADynamicMenu (ScriptEditor, EDITOR_ARGS) { Melder_help (U"Add to dynamic menu..."); }

void structScriptEditor :: v_createMenus () {
	ScriptEditor_Parent :: v_createMenus ();
	if (our wasCreatedInAnEditor()) {
		Editor_addCommand (this, U"File", U"Add to menu...", 0, menu_cb_addToMenu);
	} else {
		Editor_addCommand (this, U"File", U"Add to fixed menu...", 0, menu_cb_addToFixedMenu);
		Editor_addCommand (this, U"File", U"Add to dynamic menu...", 0, menu_cb_addToDynamicMenu);
	}
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"-- history --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Clear history", 0, menu_cb_clearHistory);
	Editor_addCommand (this, U"Edit", U"Paste history", 'H', menu_cb_pasteHistory);
	Editor_addCommand (this, U"Convert", U"-- expand --", 0, nullptr);
	Editor_addCommand (this, U"Convert", U"Expand include files", 0, menu_cb_expandIncludeFiles);
	Editor_addMenu (this, U"Run", 0);
	Editor_addCommand (this, U"Run", U"Run", 'R', menu_cb_run);
	Editor_addCommand (this, U"Run", U"Run selection", 'T', menu_cb_runSelection);
	Editor_addCommand (this, U"Run", U"-- run with full trust --", 0, nullptr);
	Editor_addCommand (this, U"Run", U"Run with FULL TRUST", 0, menu_cb_runWithFullTrust);
	Editor_addCommand (this, U"Run", U"Run selection with FULL TRUST", 0, menu_cb_runSelectionWithFullTrust);
}

void structScriptEditor :: v_createMenuItems_help (EditorMenu menu) {
	ScriptEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"About ScriptEditor", '?', menu_cb_AboutScriptEditor);
	EditorMenu_addCommand (menu, U"Scripting tutorial", 0, menu_cb_ScriptingTutorial);
	EditorMenu_addCommand (menu, U"Scripting examples", 0, menu_cb_ScriptingExamples);
	EditorMenu_addCommand (menu, U"Praat script", 0, menu_cb_PraatScript);
	EditorMenu_addCommand (menu, U"Formulas tutorial", 0, menu_cb_FormulasTutorial);
	EditorMenu_addCommand (menu, U"Functions", 0, menu_cb_Functions);
	EditorMenu_addCommand (menu, U"Demo window", 0, menu_cb_DemoWindow);
	EditorMenu_addCommand (menu, U"-- help history --", 0, nullptr);
	EditorMenu_addCommand (menu, U"The History mechanism", 0, menu_cb_TheHistoryMechanism);
	EditorMenu_addCommand (menu, U"Initialization scripts", 0, menu_cb_InitializationScripts);
	EditorMenu_addCommand (menu, U"-- help add --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Adding to a fixed menu", 0, menu_cb_AddingToAFixedMenu);
	EditorMenu_addCommand (menu, U"Adding to a dynamic menu", 0, menu_cb_AddingToADynamicMenu);
}

void ScriptEditor_init (ScriptEditor me, Editor optionalOwningEditor, conststring32 initialText) {
	if (optionalOwningEditor) {
		my optionalOwningEditorClassName = Melder_dup (Thing_className (optionalOwningEditor));   // BUG: no copying needed
		my optionalReferenceToOwningEditor = optionalOwningEditor;
	}
	TextEditor_init (me, initialText);
	my interpreterStack = InterpreterStack_create (optionalOwningEditor);
	theReferencesToAllOpenScriptEditors. addItem_ref (me);
}

autoScriptEditor ScriptEditor_createFromText (Editor optionalOwningEditor, conststring32 initialText) {
	try {
		autoScriptEditor me = Thing_new (ScriptEditor);
		ScriptEditor_init (me.get(), optionalOwningEditor, initialText);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Script window not created.");
	}
}

autoScriptEditor ScriptEditor_createFromScript_canBeNull (Editor optionalOwningEditor, autoScript script) {
	try {
		structMelderFile scriptFile { };
		for (integer ieditor = 1; ieditor <= theReferencesToAllOpenScriptEditors.size; ieditor ++) {
			ScriptEditor editor = theReferencesToAllOpenScriptEditors.at [ieditor];
			if (Melder_equ (script -> string.get(), MelderFile_peekPath (& editor -> file))) {
				Editor_raise (editor);
				Melder_pathToFile (script -> string.get(), & scriptFile);   // ensure correct messaging format
				Melder_appendError (U"The script ", & scriptFile, U" is already open and has been moved to the front.");
				if (editor -> dirty)
					Melder_appendError (U"Choose “Reopen from disk” if you want to revert to the old version.");
				Melder_flushError ();
				return autoScriptEditor();   // safe null, and `script` will be deleted
			}
		}
		Melder_pathToFile (script -> string.get(), & scriptFile);
		autostring32 text = MelderFile_readText (& scriptFile);
		autoScriptEditor me = ScriptEditor_createFromText (optionalOwningEditor, text.get());
		MelderFile_copy (& scriptFile, & my file);
		Script_rememberDuringThisAppSession_move (script.move());
		Thing_setName (me.get(), nullptr);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Script window not created.");
	}
}

void ScriptEditor_debug_printAllOpenScriptEditors () {
	for (integer ieditor = 1; ieditor <= theReferencesToAllOpenScriptEditors.size; ieditor ++) {
		ScriptEditor editor = theReferencesToAllOpenScriptEditors.at [ieditor];
		Melder_casual (U"Open script editor #", ieditor, U": <<", & editor -> file, U">>");
	}
}

/* End of file ScriptEditor.cpp */
