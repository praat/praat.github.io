/* praat_script.cpp
 *
 * Copyright (C) 1993-2026 Paul Boersma
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

#include "praatP.h"
#include "praat_script.h"
#include "UiPause.h"
#include "DemoEditor.h"

static integer praat_findObjectFromString (Interpreter interpreter, conststring32 string) {
	try {
		integer IOBJECT;
		while (*string == U' ')
			string ++;
		if (*string >= U'A' && *string <= U'Z') {
			/*
				Find the object by its name.
			*/
			static MelderString buffer;
			MelderString_copy (& buffer, string);
			char32 *space = str32chr (buffer.string, U' ');
			if (! space)
				Melder_throw (U"Missing space in name.");
			*space = U'\0';
			char32 *className = & buffer.string [0], *givenName = space + 1;
			WHERE_DOWN (1) {
				Daata object = (Daata) OBJECT;
				if (str32equ (className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
					return IOBJECT;
			}
			/*
				No object with that name. Perhaps the class name was wrong?
			*/
			ClassInfo klas = Thing_classFromClassName (className, nullptr);
			WHERE_DOWN (1) {
				Daata object = (Daata) OBJECT;
				if (str32equ (klas -> className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
					return IOBJECT;
			}
			Melder_throw (U"No object with that name.");
		} else {
			/*
				Find the object by its ID.
			*/
			double value;
			Interpreter_numericExpression (interpreter, string, & value);
			const integer id = (integer) value;   // SMELL: rounding down
			WHERE (ID == id)
				return IOBJECT;
			Melder_throw (U"No object with number ", id, U".");
		}
	} catch (MelderError) {
		Melder_throw (U"Object \"", string, U"\" does not exist.");
	}
}

Editor praat_findEditorFromString (conststring32 string) {
	integer IOBJECT;
	while (*string == U' ')
		string ++;
	if (*string >= U'A' && *string <= U'Z') {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor) {
					Melder_assert (editor -> name);
					const char32 *space = str32chr (editor -> name.get(), U' ');   // editors tend to be called like "3. Sound kanweg"
					if (space) {   // but not all
						conststring32 name = space + 1;
						if (str32equ (name, string))
							return editor;
					}
				}
			}
		}
	} else {
		WHERE_DOWN (1) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor && str32equ (editor -> name.get(), string))
					return editor;
			}
		}
	}
	Melder_throw (U"Editor \"", string, U"\" does not exist.");
}

Editor praat_findEditorById (integer id) {
	integer IOBJECT;
	WHERE (1) {
		if (ID == id) {
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor editor = theCurrentPraatObjects -> list [IOBJECT]. editors [ieditor];
				if (editor)
					return editor;
			}
		}
	}
	Melder_throw (U"Editor ", id, U" does not exist.");
}

static int parseCommaSeparatedArguments (Interpreter interpreter, char32 *arguments, structStackel *args) {
	int narg = 0, depth = 0;
	for (char32 *p = arguments; ; p ++) {
		bool endOfArguments = *p == U'\0';
		if (endOfArguments || (*p == U',' && depth == 0)) {
			if (narg == MAXIMUM_NUMBER_OF_FIELDS)
				Melder_throw (U"Cannot have more than ", MAXIMUM_NUMBER_OF_FIELDS, U" arguments");
			*p = U'\0';
			Formula_Result result;
			Interpreter_anyExpression (interpreter, arguments, & result);
			narg ++;
			/*
				First remove the old contents.
			*/
			args [narg]. reset();
			#if STACKEL_VARIANTS_ARE_PACKED_IN_A_UNION
				memset (& args [narg], 0, sizeof (structStackel));
			#endif
			/*
				Then copy in the new contents.
			*/
			switch (result. expressionType) {
				case kFormula_EXPRESSION_TYPE_NUMERIC: {
					args [narg]. which = Stackel_NUMBER;
					args [narg]. number = result. numericResult;
				} break; case kFormula_EXPRESSION_TYPE_STRING: {
					args [narg]. setString (result. stringResult.move());
				} break; case kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR: {
					args [narg]. which = Stackel_NUMERIC_VECTOR;
					args [narg]. numericVector = result. numericVectorResult;
					args [narg]. owned = result. owned;
					result. owned = false;
				} break; case kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX: {
					args [narg]. which = Stackel_NUMERIC_MATRIX;
					args [narg]. numericMatrix = result. numericMatrixResult;
					args [narg]. owned = result. owned;
					result. owned = false;
				} break; case kFormula_EXPRESSION_TYPE_STRING_ARRAY: {
					args [narg]. which = Stackel_STRING_ARRAY;
					args [narg]. stringArray = result. stringArrayResult;
					args [narg]. owned = result. owned;
					result. owned = false;
				} break;
			}
			arguments = p + 1;
		} else if (*p == U'(' || *p == U'[' || *p == U'{') {
			depth ++;
		} else if (*p == U')' || *p == U']' || *p == U'}') {
			depth --;
		} else if (*p == U'\"') {
			for (;;) {
				p ++;
				if (*p == U'\"') {
					if (p [1] == U'\"')
						p ++;
					else
						break;
				}
			}
		}
		if (endOfArguments)
			break;
	}
	return narg;
}

bool praat_executeCommand (Interpreter interpreter, char32 *command) {
	if (interpreter)
		interpreter -> returnType = kInterpreter_ReturnType::VOID_;   // clear return type to its default

	static struct structStackel args [1 + MAXIMUM_NUMBER_OF_FIELDS];
	//trace (U"praat_executeCommand: ", Melder_pointer (interpreter), U": ", command);
	if (command [0] == U'\0' || command [0] == U'#' || command [0] == U'!' || command [0] == U';')
		/* Skip empty lines and comments. */;
	else if ((command [0] == U'.' || command [0] == U'+' || command [0] == U'-') && Melder_isAsciiUpperCaseLetter (command [1])) {   // selection?
		integer IOBJECT = praat_findObjectFromString (interpreter, command + 1);
		if (command [0] == '.')
			praat_deselectAll ();
		if (command [0] == '-')
			praat_deselect (IOBJECT);
		else
			praat_select (IOBJECT); 
		praat_show ();
	} else if (Melder_isLetter (command [0]) && ! Melder_isUpperCaseLetter (command [0])) {   // all directives start with an ASCII lower-case letter
		if (str32nequ (command, U"select ", 7)) {
			if (str32nequ (command + 7, U"all", 3) && (command [10] == U'\0' || command [10] == U' ' || command [10] == U'\t')) {
				praat_selectAll ();
				praat_show ();
			} else {
				integer IOBJECT = praat_findObjectFromString (interpreter, command + 7);
				praat_deselectAll ();
				praat_select (IOBJECT);
				praat_show ();
			}
		} else if (str32nequ (command, U"plus ", 5)) {
			integer IOBJECT = praat_findObjectFromString (interpreter, command + 5);
			praat_select (IOBJECT);
			praat_show ();
		} else if (str32nequ (command, U"minus ", 6)) {
			integer IOBJECT = praat_findObjectFromString (interpreter, command + 6);
			praat_deselect (IOBJECT);
			praat_show ();
		} else if (str32nequ (command, U"echo ", 5)) {
			MelderInfo_open ();
			MelderInfo_write (command + 5);
			MelderInfo_close ();
		} else if (str32nequ (command, U"clearinfo", 9)) {
			Melder_clearInfo ();
		} else if (str32nequ (command, U"print ", 6)) {
			MelderInfo_write (command + 6);
			MelderInfo_drain ();
		} else if (str32nequ (command, U"printtab", 8)) {
			MelderInfo_write (U"\t");
			MelderInfo_drain ();
		} else if (str32nequ (command, U"printline", 9)) {
			if (command [9] == ' ') MelderInfo_write (command + 10);
			MelderInfo_write (U"\n");
			MelderInfo_drain ();
		} else if (str32nequ (command, U"fappendinfo ", 12)) {
			if (theCurrentPraatObjects != & theForegroundPraatObjects)
				Melder_throw (U"The script command “fappendinfo” is not available inside pictures.");
			structMelderFile file { };
			Melder_relativePathToFile (command + 12, & file);
			MelderFile_appendText (& file, Melder_getInfo ());
		} else if (str32nequ (command, U"unix ", 5)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “unix” is not available inside manuals.");
			try {
				Melder_runSystem (command + 5);
			} catch (MelderError) {
				Melder_throw (U"Unix command “", command + 5, U"” returned error status;\n"
					U"if you want to ignore this, use `unix_nocheck' instead of `unix'.");
			}
		} else if (str32nequ (command, U"unix_nocheck ", 13)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “unix_nocheck” is not available inside manuals.");
			try {
				Melder_runSystem (command + 13);
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else if (str32nequ (command, U"system ", 7)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “system” is not available inside manuals.");
			try {
				Melder_runSystem (command + 7);
			} catch (MelderError) {
				Melder_throw (U"System command “", command + 7, U"” returned error status;\n"
					U"if you want to ignore this, use “system_nocheck” instead of “system”.");
			}
		} else if (str32nequ (command, U"system_nocheck ", 15)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “system_nocheck” is not available inside manuals.");
			try {
				Melder_runSystem (command + 15);
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else if (str32nequ (command, U"nowarn ", 7)) {
			autoMelderWarningOff nowarn;
			return praat_executeCommand (interpreter, command + 7);
		} else if (str32nequ (command, U"noprogress ", 11)) {
			autoMelderProgressOff noprogress;
			return praat_executeCommand (interpreter, command + 11);
		} else if (str32nequ (command, U"nocheck ", 8)) {
			try {
				return praat_executeCommand (interpreter, command + 8);
			} catch (MelderError) {
				Melder_clearError ();
				return false;
			}
		} else if (str32nequ (command, U"demo ", 5)) {
			autoDemoOpen demo;
			return praat_executeCommand (interpreter, command + 5);
		} else if (str32nequ (command, U"asynchronous ", 13)) {
			autoMelderAsynchronous asynchronous;
			return praat_executeCommand (interpreter, command + 13);
		} else if (str32nequ (command, U"pause ", 6) || str32equ (command, U"pause")) {
			if (theCurrentPraatApplication -> batch)
				return true;   // in batch we ignore pause statements
			const Editor optionalPauseWindowOwningEditor = interpreter -> optionalDynamicEnvironmentEditor();
			const GuiWindow parentShell = ( optionalPauseWindowOwningEditor ? optionalPauseWindowOwningEditor -> windowForm : theCurrentPraatApplication -> topShell );
			UiPause_pauseScript (parentShell, optionalPauseWindowOwningEditor, interpreter, str32equ (command, U"pause") ? U"..." : command + 6);
		} else if (str32nequ (command, U"execute ", 8)) {
			//TRACE
			trace (U"interpreter: ", Melder_pointer (interpreter));
			if (interpreter)
				trace (U"stack: ", Melder_pointer (interpreter -> owningInterpreterStack));
			praat_runOldExecuteCommand (interpreter ? interpreter -> owningInterpreterStack : nullptr, command + 8);
		} else if (str32nequ (command, U"editor", 6)) {   // deprecated
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “editor” is not available inside manuals.");
			if (command [6] == U' ' && Melder_isLetter (command [7])) {
				interpreter -> setDynamicEditorEnvironmentFromEditor (praat_findEditorFromString (command + 7));
			} else if (command [6] == U'\0') {
				if (interpreter && interpreter -> wasStartedFromEditorEnvironment())
					/*
						The script directive
							editor
						(without arguments) should set the interpreter's environment back to the editor that started the script.
					*/
					interpreter -> setDynamicFromOwningEditorEnvironment();   // TODO: what if the owener has been orphaned?
				else
					Melder_throw (U"The script command “editor” requires an argument when called from outside an editor.");
			} else {
				Interpreter_voidExpression (interpreter, command);
			}
		} else if (str32nequ (command, U"endeditor", 9)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “endeditor” is not available inside manuals.");
			interpreter -> nullifyDynamicEditorEnvironment();
		} else if (str32nequ (command, U"filedelete ", 11)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “filedelete” is not available inside manuals.");
			const char32 *p = command + 11;
			structMelderFile file { };
			while (*p == U' ' || *p == U'\t') p ++;
			if (*p == U'\0')
				Melder_throw (U"Missing file name after “filedelete”.");
			Melder_relativePathToFile (p, & file);
			MelderFile_delete (& file);
		} else if (str32nequ (command, U"fileappend ", 11)) {
			Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
				U"The script command “fileappend” is not available inside manuals.");
			const char32 *p = command + 11;
			char32 path [kMelder_MAXPATH+1], *q = & path [0];
			while (*p == U' ' || *p == U'\t') p ++;
			if (*p == U'\0')
				Melder_throw (U"Missing file name after “fileappend”.");
			if (*p == '\"') {
				for (;;) {
					char32 kar = * ++ p;
					if (kar == U'\"') if (* ++ p == U'\"') *q ++ = U'\"'; else break;
					else if (kar == U'\0') break;
					else *q ++ = kar;
				}
			} else {
				for (;;) {
					char32 kar = * p;
					if (kar == U'\0' || kar == U' ' || kar == U'\t') break;
					*q ++ = kar;
					p ++;
				}
			}
			*q = U'\0';
			if (*p == U' ' || *p == U'\t') {
				structMelderFile file { };
				Melder_relativePathToFile (path, & file);
				MelderFile_appendText (& file, p + 1);
			}
		} else {
			/*
				This must be a formula command:
					proc (args)
			*/
			Interpreter_voidExpression (interpreter, command);
		}
	} else {   /* Simulate menu choice. */
		bool hasDots = false, hasColon = false;

 		/* Parse command line into command and arguments. */
		/* The separation is formed by the three dots or a colon. */

		char32 *arguments = & command [0];
		for (arguments = & command [0]; *arguments != U'\0'; arguments ++) {
			if (*arguments == U':') {
				hasColon = true;
				if (arguments [1] == U'\0') {
					arguments = & arguments [1];   // empty string
				} else {
					if (arguments [1] != U' ')
						Melder_throw (U"There should be a space after the colon.");
					arguments [1] = U'\0';   // new end of "command"
					arguments += 2;   // the arguments start after the space
				}
				break;
			}
			if (*arguments == U'.' && arguments [1] == U'.' && arguments [2] == U'.') {
				hasDots = true;
				arguments += 3;
				if (*arguments == U'\0') {
					// empty string
				} else {
					if (*arguments != U' ')
						Melder_throw (U"There should be a space after the three dots.");
					*arguments = U'\0';   // new end of "command"
					arguments ++;   // the arguments start after the space
				}
				break;
			}
		}

		/* See if command exists and is available; ignore separators. */
		/* First try loose commands, then fixed commands. */

		integer narg;
		char32 command2 [200];
		if (hasColon) {
			narg = parseCommaSeparatedArguments (interpreter, arguments, args);
			str32cpy (command2, command);
			char32 *colon = str32chr (command2, U':');
			colon [0] = colon [1] = colon [2] = U'.';
			colon [3] = U'\0';
		}
		if (theCurrentPraatObjects == & theForegroundPraatObjects && interpreter && interpreter -> hasDynamicEnvironmentEditor()) {
			if (hasColon)
				Editor_doMenuCommand (interpreter -> optionalDynamicEnvironmentEditor(), command2, narg, args, nullptr, interpreter);
			else
				Editor_doMenuCommand (interpreter -> optionalDynamicEnvironmentEditor(), command, 0, nullptr, arguments, interpreter);
		} else if (! praat_commandsWithExternalSideEffectsAreAllowed () &&
		    (str32nequ (command, U"Save ", 5) ||
			 str32nequ (command, U"Write ", 6) ||
			 str32nequ (command, U"Append to ", 10) ||
			 str32nequ (command, U"Quit", 4)))
		{
			Melder_throw (U"Commands that write files (including Quit) are not available inside manuals.");
		} else {
			bool theCommandIsAnExistingAction = false;
			try {
				if (hasColon)
					theCommandIsAnExistingAction = praat_doAction (command2, narg, args, interpreter);
				else
					theCommandIsAnExistingAction = praat_doAction (command, arguments, interpreter);
			} catch (MelderError) {
				/*
					We only get here if the command *was* an existing action.
					Anything could have gone wrong in its execution,
					but one invisible problem can be checked here.
				*/
				if (hasDots && arguments [0] != U'\0' && arguments [Melder_length (arguments) - 1] == U' ')
					Melder_throw (U"It may be helpful to remove the trailing spaces in \"", arguments, U"\".");
				else
					throw;
			}
			if (! theCommandIsAnExistingAction) {
				bool theCommandIsAnExistingMenuCommand = false;
				try {
					if (hasColon)
						theCommandIsAnExistingMenuCommand = praat_doMenuCommand (command2, narg, args, interpreter);
					else
						theCommandIsAnExistingMenuCommand = praat_doMenuCommand (command, arguments, interpreter);
				} catch (MelderError) {
					/*
						We only get here if the command *was* an existing menu command.
						Anything could have gone wrong in its execution,
						but one invisible problem can be checked here.
					*/
					if (hasDots && arguments [0] != U'\0' && arguments [Melder_length (arguments) - 1] == U' ')
						Melder_throw (U"It may be helpful to remove the trailing spaces in “", arguments, U"”.");
					else
						throw;
				}
				if (! theCommandIsAnExistingMenuCommand) {
					const integer length = Melder_length (command);
					if (str32nequ (command, U"ARGS ", 5))
						Melder_throw (U"Command “ARGS” no longer supported. Instead use “form” and “endform”.");
					else if (str32chr (command, U'='))
						if (command [0] == U'_')
							Melder_throw (U"Command “", command, U"” not recognized.\n"
								U"Probable cause: you are trying to use a variable name that starts with an underscore.");
						else
							Melder_throw (U"Command “", command, U"” not recognized.\n"
								U"Probable cause: you are trying to use a variable name that starts with a capital letter.");
					else if (length >= 1 && Melder_isHorizontalSpace (command [length - 1]))
						Melder_throw (U"Command “", command, U"” not available for current selection. "
							U"It may be helpful to remove the trailing spaces.");
					else if (length >= 2 && Melder_isHorizontalSpace (command [length - 2]) && command [length - 1] == U':')
						Melder_throw (U"Command “", command, U"” not available for current selection. "
							U"It may be helpful to remove the space before the colon.");
					else if (str32nequ (command, U"\"ooTextFile\"", 12))
						Melder_throw (U"Command “", command, U"” not available for current selection. "
							U"It is possible that this file is not a Praat script but a Praat data file that you can open with “Read from file...”.");
					else {
						if (interpreter && interpreter -> wasStartedFromEditorEnvironment())
							if (interpreter -> hasDynamicEnvironmentEditor())
								Melder_throw (U"Command “", command, U"” not available in ", interpreter -> optionalDynamicEditorEnvironmentClassName(), U".");
							else
								Melder_throw (U"Command “", command, U"” not available for current selection.\n"
										U"Perhaps this is a ", interpreter -> optionalOwningEditorEnvironmentClassName(), U" command?");
						else
							Melder_throw (U"Command “", command, U"” not available for current selection.");
					}
				}
			}
		}
		praat_updateSelection ();
	}
	return true;
}

void praat_executeCommandFromStandardInput (conststring32 programName) {
	char command8 [1000];   // can be recursive
	/*
		FIXME: implement for Windows.
	*/
	for (;;) {
		printf ("%s > ", Melder_peek32to8 (programName));
		if (! fgets (command8, 999, stdin))
			Melder_throw (U"Cannot read input.");
		char *newLine = strchr (command8, '\n');
		if (newLine)
			*newLine = '\0';
		autostring32 command32 = Melder_8to32_e (command8);
		try {
			(void) praat_executeCommand (nullptr, command32.get());
		} catch (MelderError) {
			Melder_flushError (programName, U": Command “", Melder_peek8to32_u (command8), U"” not executed.");
		}
	}
}

void praat_executeScript_noGUI (MelderFile file) {
	/*
		The problem here occurs when plug-in initialization (typically, `setup.praat`) posts a pause window,
		as happens e.g. in the VocalToolkit plug-in if the Praat version is too old.
		This function will return while the pause window is on the screen, and
		Praat will happily initialize the next plug-in.
		Quick fix: allow up to 10 InterpreterStacks.
		Long-term solution TODO: make pause windows in plug-in initializers modal.
	*/
	constexpr integer NUMBER_OF_STACKS = 10;
	static autoInterpreterStack interpreterStacks [NUMBER_OF_STACKS];
	static integer stackNumber = 0;
	stackNumber = (stackNumber + 1) % NUMBER_OF_STACKS;
	autoInterpreterStack& interpreterStack = interpreterStacks [stackNumber];
	try {
		interpreterStack = InterpreterStack_create (Editor (nullptr));
		autostring32 text = MelderFile_readText (file);
		{// scope
			autoMelderFileSetCurrentFolder folder (file);   // so that script-relative file names can be used for including include files
			Melder_includeIncludeFiles (& text);
		}   // back to the default directory of the caller
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			interpreterStack.get(),   // owner always needed
			Editor (nullptr),   // never used inside an Editor window
			file   // so that script-relative file names can be used inside the script
		);
		interpreterStack -> emptyAll ();
		interpreterStack -> runDown (interpreter.move(), text.move(), false);
	} catch (MelderError) {
		Melder_throw (U"Script ", file, U" not completed.");
	}
}

void praat_runScript (InterpreterStack interpreterStack, conststring32 fileName, integer narg, Stackel args, Editor optionalInterpreterOwningEditor) {
	Melder_assert (interpreterStack);
	//Melder_assert (Melder_backgrounding);

	Interpreter parentInterpreter = interpreterStack -> current_a ();
	if (parentInterpreter -> isInSecondPass) {
		interpreterStack -> resumeNextLevelInSecondPass ();
	} else {
		Melder_assert (MelderFolder_equal (Melder_peekWorkingDirectory (), & parentInterpreter -> workingDirectory));
		//autoMelderSetCurrentFolder folder (& parentInterpreter -> workingDirectory);   // TODO: is this superfluous, i.e. does the previous assertion never fire?

		structMelderFile file { };
		try {
			Melder_relativePathToFile (fileName, & file);
			autostring32 text = MelderFile_readText (& file);
			/*
				We switch between default directories no fewer than four times:
				1. runScript() tends to be called from a script that we call the "caller";
				   when we enter runScript(), the default directory is the caller's folder,
				   as was appropriate for the use of file names in the caller before runScript(),
				   which had to be interpreted relative to the caller's folder.
				2. runScript() will call a script that we call the "callee";
				   include files have to be included from the callee's folder.
				3. For expanding any infile/outfile/folder arguments to runScript(),
				   we have to be back in the caller's folder.
				4. Inside the callee, file names will have to be interpreted relative to the callee's folder.
				5. After runScript() finishes, we will have to be back in the caller's folder,
				   so that the use of file names in the caller after runScript()
				   will be interpreted relative to the caller's folder again.
			*/
			{// scope
				autoMelderFileSetCurrentFolder folder (& file);   // so that callee-relative file names can be used for including include files
				Melder_includeIncludeFiles (& text);
			}   // back to the default directory of the caller
			autoInterpreter me = Interpreter_createFromEnvironment (
				interpreterStack,   // owner always needed
				optionalInterpreterOwningEditor,
				& file   // so that callee-relative file names can be used inside the script
			);
			Interpreter_readParameters (me.get(), text.get());   // TODO: should become a field of structInterpreter
			my text = text.move();
			Interpreter_getArgumentsFromArgs (me.get(), narg, args);   // interpret caller-relative paths for infile/outfile/folder arguments
			autoScript script = Script_createFromFile (& file);
			Script_rememberDuringThisAppSession_move (script.move());
			my scriptReference = Script_find (MelderFile_peekPath (& file));
			interpreterStack -> runDown (me.move(), autostring32(), false);   // back to the default directory of the caller
		} catch (MelderError) {
			Melder_throw (U"Script ", & file, U" not completed.");   // don't refer to 'fileName', because its contents may have changed
		}
	}
}

void praat_runNotebook (InterpreterStack interpreterStack, conststring32 fileName, integer narg, Stackel args, Editor optionalInterpreterOwningEditor) {
	Melder_assert (interpreterStack);
	//Melder_assert (Melder_backgrounding);

	//TRACE
	Interpreter parentInterpreter = interpreterStack -> current_a ();
	if (parentInterpreter -> isInSecondPass) {
		interpreterStack -> resumeNextLevelInSecondPass ();
	} else {
		structMelderFolder workingDirectory;
		Melder_getCurrentFolder (& workingDirectory);
		Melder_assert (MelderFolder_equal (& workingDirectory, & parentInterpreter -> workingDirectory));

		structMelderFile file { };
		try {
			Melder_relativePathToFile (fileName, & file);
			autostring32 text = MelderFile_readText (& file);
			if (! Melder_startsWith (text.get(), U"\""))
				Melder_throw (U"File ", & file, U" is not a Praat notebook.");
			/*
				We switch between default directories no fewer than four times:
				1. runScript() tends to be called from a script that we call the "caller";
				   when we enter runScript(), the working directory is the caller's folder
				   (set upon Interpreter creation as the folder where the calling script is,
				   though perhaps changed later in that script by `setWorkingDirectory`),
				   as was appropriate for the use of file names in the caller before runScript(),
				   which had to be interpreted relative to the caller's folder.
				2. runScript() will call a script that we call the "callee";
				   include files have to be included from the callee's folder.
				3. For expanding any infile/outfile/folder arguments to runScript(),
				   we have to be back in the caller's folder.
				4. Inside the callee, file names will have to be interpreted relative to the callee's folder.
				5. After runScript() finishes, we will have to be back in the caller's folder,
				   so that the use of file names in the caller after runScript()
				   will be interpreted relative to the caller's folder again.
			*/
			{// scope
				autoMelderFileSetCurrentFolder folder (& file);   // so that callee-relative file names can be used for including include files
				Melder_includeIncludeFiles (& text);
			}   // back to the default directory of the caller
			autoInterpreter interpreter = Interpreter_createFromEnvironment (
				interpreterStack,
				optionalInterpreterOwningEditor,
				& file
			);
			Interpreter_readParameters (interpreter.get(), text.get());   // TODO: should become a field of structInterpreter
			autoMelderReadText readText = MelderReadText_createFromText (text.move());
			autoManPages manPages = ManPages_createFromText (readText.get(), & file);
			ManPage firstPage = manPages -> pages.at [1];
			autoManual manual = Manual_create (firstPage -> title.get(), interpreter.get(), manPages.releaseToAmbiguousOwner(), true, true);
			manual.releaseToUser ();
		} catch (MelderError) {
			Melder_throw (U"Notebook ", & file, U" not completed.");   // don't refer to 'fileName', because its contents may have changed
		}
	}
}

void praat_executeScriptFromCommandLine (conststring32 fileName, integer argc, char **argv) {
	static autoInterpreterStack interpreterStack = InterpreterStack_create (Editor (nullptr));
	structMelderFile file { };
	Melder_relativePathToFile (fileName, & file);
	try {
		autostring32 text = MelderFile_readText (& file);
		{// scope
			autoMelderFileSetCurrentFolder folder (& file);   // so that script-relative file names can be used for including include files
			Melder_includeIncludeFiles (& text);
		}   // back to the default directory of the caller
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			interpreterStack.get(),
			Editor (nullptr),
			& file
		);
		Interpreter_readParameters (interpreter.get(), text.get());
		Interpreter_getArgumentsFromCommandLine (interpreter.get(), argc, argv);   // interpret caller-relative paths for infile/outfile/folder arguments
		autoMelderFileSetCurrentFolder folder (& file);   // so that script-relative file names can be used inside the script
		interpreterStack -> runDown (interpreter.move(), text.move(), false);
	} catch (MelderError) {
		Melder_throw (U"Script ", & file, U" not completed.");   // don't refer to 'fileName', because its contents may have changed
	}
}

void praat_executeScriptFromFileNameWithArguments (conststring32 nameAndArguments) {
	structMelderFile file { };
	try {
		char32 path [256];
		const char32 *p, *arguments;
		/*
			Split into file name and arguments.
		*/
		p = nameAndArguments;
		while (*p == U' ' || *p == U'\t')
			p ++;
		if (*p == U'\"') {
			char32 *q = path;
			p ++;   // skip quote
			while (*p != U'\"' && *p != U'\0')
				* q ++ = * p ++;
			*q = U'\0';
			arguments = p;
			if (*arguments == U'\"')
				arguments ++;
			if (*arguments == U' ')
				arguments ++;
		} else {
			char32 *q = path;
			while (*p != U' ' && *p != U'\0')
				* q ++ = * p ++;
			*q = U'\0';
			arguments = p;
			if (*arguments == U' ')
				arguments ++;
		}
		Melder_relativePathToFile (path, & file);

		static autoInterpreterStack interpreterStack = InterpreterStack_create (Editor (nullptr));
		autostring32 text = MelderFile_readText (& file);
		{// scope
			autoMelderFileSetCurrentFolder folder (& file);   // so that script-relative file names can be used for including include files
			Melder_includeIncludeFiles (& text);
		}   // back to the default directory of the caller
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			interpreterStack.get(),
			Editor (nullptr),
			& file   // so that script-relative file names can be used inside the script
		);
		if (arguments) {
			Interpreter_readParameters (interpreter.get(), text.get());
			Interpreter_getArgumentsFromString (interpreter.get(), arguments);   // interpret caller-relative paths for infile/outfile/folder arguments
		}
		interpreterStack -> runDown (interpreter.move(), text.move(), false);
	} catch (MelderError) {
		Melder_throw (U"Script ", & file, U" not completed.");
	}
}

void praat_runOldExecuteCommand (InterpreterStack interpreterStack, conststring32 nameAndArguments) {
	Melder_assert (interpreterStack);

	char32 path [256];
	const char32 *p, *arguments;
	structMelderFile file { };
	/*
		Split into file name and arguments.
	*/
	p = nameAndArguments;
	while (*p == U' ' || *p == U'\t')
		p ++;
	if (*p == U'\"') {
		char32 *q = path;
		p ++;   // skip quote
		while (*p != U'\"' && *p != U'\0')
			* q ++ = * p ++;
		*q = U'\0';
		arguments = p;
		if (*arguments == U'\"')
			arguments ++;
		if (*arguments == U' ')
			arguments ++;
	} else {
		char32 *q = path;
		while (*p != U' ' && *p != U'\0')
			* q ++ = * p ++;
		*q = U'\0';
		arguments = p;
		if (*arguments == U' ')
			arguments ++;
	}
	Melder_relativePathToFile (path, & file);

	Interpreter parentInterpreter = interpreterStack -> current_a ();
	if (parentInterpreter -> isInSecondPass) {
		interpreterStack -> resumeNextLevelInSecondPass ();
	} else {
		try {
			autostring32 text = MelderFile_readText (& file);
			{// scope
				autoMelderFileSetCurrentFolder folder (& file);   // so that callee-relative file names can be used for including include files
				Melder_includeIncludeFiles (& text);
			}   // back to the default directory of the caller
			autoInterpreter me = Interpreter_createFromEnvironment (
				interpreterStack,
				Editor (nullptr),
				& file   // so that callee-relative file names can be used inside the script
			);
			if (arguments) {
				Interpreter_readParameters (me.get(), text.get());
				Interpreter_getArgumentsFromString (me.get(), arguments);   // interpret caller-relative paths for infile/outfile/folder arguments
			}
			my text = text.move();
			interpreterStack -> runDown (me.move(), autostring32(), false);   // callee-relative file names can be used inside the script
			// back to the default directory of the caller
		} catch (MelderError) {
			Melder_throw (U"Script ", & file, U" not completed.");
		}
	}
}

extern "C" void praatlib_executeScript (const char *text8) {
	static autoInterpreterStack interpreterStack = InterpreterStack_create (Editor (nullptr));   // TODO: never from an editor?
	try {
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			interpreterStack.get(),
			Editor (nullptr),
			MelderFile (nullptr)   // TODO: look for a file...
		);
		autostring32 string = Melder_8to32_e (text8);
		interpreterStack -> runDown (interpreter.move(), string.move(), false);
	} catch (MelderError) {
		Melder_throw (U"Script not completed.");
	}
}

void praat_executeScriptFromText (conststring32 text) {
	static autoInterpreterStack interpreterStack = InterpreterStack_create (Editor (nullptr));   // never from an editor (last checked 2026-02-01)
	try {
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			interpreterStack.get(),
			Editor (nullptr),   // never from an editor (last checked 2026-02-01)
			MelderFile (nullptr)
		);
		Melder_getCurrentFolder (& interpreter -> workingDirectory);
		interpreterStack -> emptyAll ();
		interpreterStack -> runDown (interpreter.move(), Melder_dup (text), false);
	} catch (MelderError) {
		Melder_throw (U"Script not completed.");
	}
}

static void secondPassThroughScript (UiForm sendingForm, integer /* narg */, Stackel /* args */,
	conststring32 /* sendingString_dummy */, Interpreter /* interpreter_dummy */,
	conststring32 /* invokingButtonTitle */, bool /* modified */, void * /* closure */, Editor optionalInterpreterOwningEditor)
{
	static autoInterpreterStack interpreterStack;
	interpreterStack = InterpreterStack_create (optionalInterpreterOwningEditor);   // each time anew
	structMelderFile file { };
	Melder_pathToFile (sendingForm -> scriptFilePath.get(), & file);
	autostring32 text = MelderFile_readText (& file);
	{// scope
		autoMelderFileSetCurrentFolder folder (& file);
		Melder_includeIncludeFiles (& text);
	}
	autoInterpreter interpreter = Interpreter_createFromEnvironment (
		interpreterStack.get(),
		optionalInterpreterOwningEditor,
		& file
	);
	Interpreter_readParameters (interpreter.get(), text.get());
	Interpreter_getArgumentsFromDialog (interpreter.get(), sendingForm);
	autoPraatBackground background;
	interpreterStack -> emptyAll ();
	interpreterStack -> runDown (interpreter.move(), text.move(), false);
}

static void firstPassThroughScript (MelderFile file, Editor optionalInterpreterOwningEditor, EditorCommand optionalCommand) {
	static autoInterpreterStack interpreterStack;
	UiPause_cleanUp ();   // TODO: needed?
	try {
		autostring32 text = MelderFile_readText (file);
		/*
			We switch between default directories no fewer than four times:
			1. runScript() tends to be called from a script that we call the "caller";
			   when we enter runScript(), the default directory is the caller's folder,
			   as was appropriate for the use of file names in the caller before runScript(),
			   which had to be interpreted relative to the caller's folder.
			2. runScript() will call a script that we call the "callee";
			   include files have to be included from the callee's folder.
			3. For expanding any infile/outfile/folder arguments to runScript(),
			   we have to be back in the caller's folder.
			4. Inside the callee, file names will have to be interpreted relative to the callee's folder.
			5. After runScript() finishes, we will have to be back in the caller's folder,
			   so that the use of file names in the caller after runScript()
			   will be interpreted relative to the caller's folder again.
		*/
		{// scope
			autoMelderFileSetCurrentFolder folder (file);   // so that callee-relative file names can be used for including include files
			Melder_includeIncludeFiles (& text);
		}   // back to the default directory of the caller

		interpreterStack = InterpreterStack_create (optionalInterpreterOwningEditor);
		autoInterpreter interpreter = Interpreter_createFromEnvironment (
			interpreterStack.get(),
			optionalInterpreterOwningEditor,
			file   // so that callee-relative file names can be used inside the script
		);

		autoScript script = Script_createFromFile (file);
		Script_rememberDuringThisAppSession_move (script.move());
		interpreter -> scriptReference = Script_find (MelderFile_peekPath (file));

		const integer numberOfParameters = Interpreter_readParameters (interpreter.get(), text.get());
		if (numberOfParameters > 0) {
			const GuiWindow parentShell = ( optionalInterpreterOwningEditor ? optionalInterpreterOwningEditor -> windowForm : theCurrentPraatApplication -> topShell );
			autoUiForm form = Interpreter_createForm (interpreter.get(), parentShell, optionalInterpreterOwningEditor,
					MelderFile_peekPath (file), secondPassThroughScript, nullptr, false);
			UiForm_do (form.get(), false);
			if (optionalCommand) {
				optionalCommand -> d_uiform = form.move();
			} else {
				UiForm_destroyWhenUnmanaged (form.get());
				form. releaseToUser();
			}
		} else {
			{// scope
				autoPraatBackground background;
				interpreterStack -> emptyAll ();
				interpreterStack -> runDown (interpreter.move(), text.move(), false);
			}
		}
	} catch (MelderError) {
		Melder_throw (U"Script ", file, U" not completed.");
	}
}

void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm /* sendingForm_dummy */, integer /* narg */, Stackel /* args */,
	conststring32 scriptPath, Interpreter /* interpreter */,
	conststring32 /* invokingButtonTitle */, bool /* modified */, void *, Editor optionalInterpreterOwningEditor)
{
	structMelderFile file { };
	Melder_relativePathToFile (scriptPath, & file);
	firstPassThroughScript (& file, optionalInterpreterOwningEditor, nullptr);
}

void praat_runScriptWithForm (conststring32 fileName) {
	structMelderFile file { };
	Melder_relativePathToFile (fileName, & file);
	firstPassThroughScript (& file, nullptr, nullptr);
}

void praat_executeScriptFromEditorCommand (Editor interpreterOwningEditor, EditorCommand command, conststring32 scriptPath) {
	Melder_assert (interpreterOwningEditor);
	Melder_assert (command);
	Melder_assert (command -> d_editor == interpreterOwningEditor);
	structMelderFile file { };
	Melder_relativePathToFile (scriptPath, & file);
	firstPassThroughScript (& file, interpreterOwningEditor, command);
}

/* End of file praat_script.cpp */
