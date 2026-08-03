#ifndef _praat_script_h_
#define _praat_script_h_
/* praat_script.h
 *
 * Copyright (C) 1992-2005,2007,2009-2016,2018,2021-2026 Paul Boersma
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

#include "Interpreter.h"

bool praat_executeCommand (Interpreter me, char32 *command);   // returns false only if nocheck cancelled an error
void praat_executeCommandFromStandardInput (conststring32 programName);
void praat_executeScript_noGUI (MelderFile file, bool fullTrust);
void praat_runScript (InterpreterStack interpreterStack, conststring32 fileName, integer narg, Stackel args, Editor optionalInterpreterOwningEditor);   // called only from Formula as `runScript` (last checked 2022-10-07)
void praat_runScriptWithForm (conststring32 fileName, bool fullTrust);
void praat_runNotebook (InterpreterStack interpreterStack, conststring32 fileName, integer narg, Stackel args, Editor optionalInterpreterOwningEditor);
void praat_executeScriptFromCommandLine (conststring32 fileName, integer argc, char **argv, bool fullTrust);   // called only from `praat_run` (last checked 2022-10-07)
void praat_executeScriptFromText (conststring32 text);
extern "C" void praatlib_executeScript (const char *text8);
void DO_RunTheScriptFromAnyAddedMenuCommand (UiForm sendingForm_dummy, integer narg, Stackel args, conststring32 scriptPath,
		Interpreter /* interpreter */, conststring32 invokingButtonTitle, bool modified, void *dummy, Editor optionalInterpreterOwningEditor);
void praat_executeScriptFromEditorCommand (Editor interpreterOwningEditor, EditorCommand command, conststring32 script);

/*
	Deprecated stuff.
*/
void praat_executeScriptFromFileNameWithArguments (conststring32 nameAndArguments);   // called only from external man pages with \SC (deprecated 2026) (last checked 2026-02-03)
void praat_runOldExecuteCommand (InterpreterStack owningInterpreterStack, conststring32 nameAndArguments);   // called only from `execute` (deprecated 2014) (last checked 2026-02-03)

#endif // !_praat_script_h_
