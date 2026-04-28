/* Interpreter.cpp
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

#include "Interpreter.h"
#include "praatP.h"
#include "praat_script.h"
#include "UiPause.h"
#include "DemoEditor.h"
#include "Formula.h"
#include "../kar/UnicodeData.h"

#include "../fon/Vector.h"
#include "../dwsys/NUMsorting.h"

#define Interpreter_WORD 1
#define Interpreter_SENTENCE 2
#define Interpreter_TEXT 3
#define Interpreter_INFILE 4
#define Interpreter_OUTFILE 5
#define Interpreter_FOLDER 6

#define Interpreter_REAL 7
#define Interpreter_POSITIVE 8
#define Interpreter_INTEGER 9
#define Interpreter_NATURAL 10
#define Interpreter_BOOLEAN 11
#define Interpreter_MINIMUM_TYPE_FOR_NUMERIC_VARIABLE  Interpreter_REAL
#define Interpreter_MAXIMUM_TYPE_FOR_NUMERIC_VARIABLE  Interpreter_BOOLEAN

#define Interpreter_REALVECTOR 12
#define Interpreter_POSITIVEVECTOR 13
#define Interpreter_INTEGERVECTOR 14
#define Interpreter_NATURALVECTOR 15
#define Interpreter_MINIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE  Interpreter_REALVECTOR
#define Interpreter_MAXIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE  Interpreter_NATURALVECTOR

#define Interpreter_REALMATRIX 16
#define Interpreter_CHOICE 17
#define Interpreter_OPTIONMENU 18
#define Interpreter_MAXIMUM_TYPE_WITH_VARIABLE_NAME  Interpreter_OPTIONMENU

#define Interpreter_BUTTON 19
#define Interpreter_OPTION 20
#define Interpreter_HEADING 21
#define Interpreter_COMMENT 22
#define Interpreter_CAPTION 23

Thing_implement (InterpreterVariable, SimpleString, 0);

static autoInterpreterVariable InterpreterVariable_create (conststring32 key) {
	try {
		if (key [0] == U'e' && key [1] == U'\0')
			Melder_throw (U"You cannot use “e” as the name of a variable (e is the constant 2.71...).");
		if (key [0] == U'p' && key [1] == U'i' && key [2] == U'\0')
			Melder_throw (U"You cannot use “pi” as the name of a variable (pi is the constant 3.14...).");
		if (key [0] == U'u' && key [1] == U'n' && key [2] == U'd' && key [3] == U'e' && key [4] == U'f' && key [5] == U'i' &&
			key [6] == U'n' && key [7] == U'e' && key [8] == U'd' && key [9] == U'\0')
			Melder_throw (U"You cannot use “undefined” as the name of a variable.");
		if (key [0] == U's' && key [1] == U't' && key [2] == U'o' && key [3] == U'p' && key [4] == U'w' && key [5] == U'a' &&
			key [6] == U't' && key [7] == U'c' && key [8] == U'h' && key [9] == U'\0')
			Melder_throw (U"You cannot use “stopwatch” as the name of a variable.");
		autoInterpreterVariable me = Thing_new (InterpreterVariable);
		my string = Melder_dup (key);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interpreter variable not created.");
	}
}

conststring32 kInterpreter_ReturnType_errorMessage (kInterpreter_ReturnType returnType, conststring32 command) {
	switch (returnType) {
		case kInterpreter_ReturnType::VOID_:
			return Melder_cat (U"The command \"", command, U"\" does not return anything");
		case kInterpreter_ReturnType::OBJECT_:
			return Melder_cat (U"The command \"", command, U"\" returns an object");
		case kInterpreter_ReturnType::REAL_:
		case kInterpreter_ReturnType::INTEGER_:
			return Melder_cat (U"The command \"", command, U"\" returns a number or a string");
		case kInterpreter_ReturnType::STRING_:
			return Melder_cat (U"The command \"", command, U"\" returns a string");
		case kInterpreter_ReturnType::REALVECTOR_:
		case kInterpreter_ReturnType::INTEGERVECTOR_:
			return Melder_cat (U"The command \"", command, U"\" returns a vector");
		case kInterpreter_ReturnType::REALMATRIX_:
			return Melder_cat (U"The command \"", command, U"\" returns a matrix");
		case kInterpreter_ReturnType::STRINGARRAY_:
			return Melder_cat (U"The command \"", command, U"\" returns a string array");
		default:
			return Melder_cat (U"The command \"", command, U"\" has an unknown return type");
	}
}

Thing_implement (Interpreter, Thing, 0);

static CollectionOf <structInterpreter> theReferencesToAllLivingInterpreters;

/* global */ structInterpreterStack theInterpreterStack;

void structInterpreter :: v9_destroy () noexcept {
	theReferencesToAllLivingInterpreters. undangleItem (this);
	UiPause_interpreterGoesAway (this);
	Demo_interpreterGoesAway (this);
	our Interpreter_Parent :: v9_destroy ();
}

autoInterpreter Interpreter_createFromEnvironment (InterpreterStack interpreterStack, Editor optionalInterpreterOwningEditor, MelderFile optionalFile) {
	Melder_assert (interpreterStack);
	try {
		autoInterpreter me = Thing_new (Interpreter);
		my owningInterpreterStack = interpreterStack;
		if (optionalFile) {
			MelderFile_copy (optionalFile, & my file);
			/*
				As for the working directory, put it in canonical form, i.e. don't use only MelderFile_getParentFolder(). TODO: make MelderFile_getParentFolder canonical
			*/
			{// scope
				autoMelderFileSetCurrentFolder folder (& my file);
				Melder_getCurrentFolder (& my workingDirectory);
			}
		} else {
			Melder_getCurrentFolder (& my workingDirectory);   // last resort; TODO: figure out how useful this is
		}
		my setOwningEditorEnvironmentFromOptionalEditor (optionalInterpreterOwningEditor);
		my variablesMap. max_load_factor (0.65f);
		theReferencesToAllLivingInterpreters. addItem_ref (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interpreter not created.");
	}
}

void Interpreters_undangleEnvironment (Editor environment) noexcept {
	for (integer i = 1; i <= theReferencesToAllLivingInterpreters.size; i ++) {
		const Interpreter interpreter = theReferencesToAllLivingInterpreters.at [i];
		if (interpreter -> optionalOwningEnvironmentEditor() == environment)
			interpreter -> undangleOwningEditor();
		if (interpreter -> optionalDynamicEnvironmentEditor() == environment)
			interpreter -> undangleDynamicEditor();
	}
}

static bool Melder_scriptTextIsNotebookText (conststring32 text) {
	/*
		Notebooks start with a opening double quote.
		Alternatively, notebooks tend to contain opening and closing braces
		in the first position of some line,
		but that cue is not reliable,
		because some notebooks contain no code chunks at all.
	*/
	return text [0] == U'"';
}

void Melder_includeIncludeFiles (autostring32 *inout_text, bool onlyInCodeChunks) {
	for (int depth = 0; ; depth ++) {
		char32 *head = inout_text->get();
		integer numberOfIncludes = 0;
		if (depth > 10)
			Melder_throw (U"Include files nested too deep. Probably cyclic.");
		for (;;) {
			char32 *includeLocation, *includeFileName, *tail;
			/*
				Look for an include statement. If not found, we have finished.
			*/
			includeLocation = ( str32nequ (head, U"include ", 8) ? head : str32str (head, U"\ninclude ") );
			if (! includeLocation)
				break;
			if (includeLocation != head)
				includeLocation += 1;
			if (onlyInCodeChunks) {
				if (includeLocation == head)
					continue;   // if the text starts with "include", it cannot be within a code chunk
				integer braceDepth = 0;
				for (const char32 *p = head; p != includeLocation; p ++)
					if (Melder_isEndOfLine (*p)) {
						if (p [1] == U'{') {
							if (braceDepth > 0)
								Melder_throw (U"Opening brace within a code chunk. Don't know whether or not to include an include file.");
							braceDepth += 1;
						} else if (p [1] == U'}') {
							if (braceDepth <= 0)
								Melder_throw (U"Closing brace outside a code chunk. Don't know whether or not to include an include file.");
							braceDepth -= 1;
						}
					}
				if (braceDepth == 0) {
					head = includeLocation + 8;
					continue;
				}
			}
			numberOfIncludes += 1;
			/*
				Separate out the head.
			*/
			*includeLocation = U'\0';
			/*
				Separate out the name of the include file.
			*/
			includeFileName = includeLocation + 8;
			Melder_skipHorizontalSpace (& includeFileName);
			tail = includeFileName;
			while (Melder_staysWithinLine (*tail))
				tail ++;
			if (*tail != U'\0') {
				*tail = U'\0';
				tail += 1;
			}
			/*
				Get the contents of the include file.
			*/
			structMelderFile includeFile { };
			Melder_relativePathToFile (includeFileName, & includeFile);
			autostring32 includeText;
			try {
				includeText = MelderFile_readText (& includeFile);
			} catch (MelderError) {
				Melder_throw (U"Include file ", & includeFile, U" not read.");
			}
			/*
				Construct the new text.
			*/
			const integer headLength = (head - inout_text->get()) + Melder_length (head);
			const integer includeTextLength = Melder_length (includeText.get());
			const integer newLength = headLength + includeTextLength + 1 + Melder_length (tail);
			autostring32 newText (newLength);
			str32cpy (newText.get(), inout_text->get());
			str32cpy (newText.get() + headLength, includeText.get());
			str32cpy (newText.get() + headLength + includeTextLength, U"\n");
			str32cpy (newText.get() + headLength + includeTextLength + 1, tail);
			/*
				Replace the old text with the new. This will work even within an autostring.
			*/
			*inout_text = newText.move();
			/*
				Cycle.
			*/
			head = inout_text->get() + headLength + includeTextLength + 1;
		}
		if (numberOfIncludes == 0)
			break;
	}
}

static bool parameterMatchesLabel (conststring32 parameter, conststring32 label) {
	/*
		A parameter matches a label if they are equal or if
		the label has an additional opening parenthesis, optionally preceded by a space.
		For example, the parameter "Pitch ceiling" matches the label "Pitch ceiling (Hz)".
	*/
	for (; *parameter != U'\0' && *label != U'\0'; parameter ++, label ++)
		if (*parameter != *label)
			return false;
	if (*parameter == U'\0')
		if (*label == U'\0' || *label == U'(' || *label == U' ' && label [1] == U'(')
			return true;
	return false;
}

integer Interpreter_readParameters (Interpreter me, mutablestring32 text) {
	const bool scriptTextIsNotebookText = Melder_scriptTextIsNotebookText (text);
	char32 *formLocation = nullptr;
	integer npar = 0;
	my dialogTitle. reset();
	autoMelderString string;
	/*
		Look for a "form" line.
	*/
	{// scope
		integer braceDepth = 0;
		char32 *p = & text [0];
		for (;;) {
			/*
				Check invariant here: we are at the beginning of a line.
			*/
			Melder_assert (p == text || Melder_isEndOfLine (p [-1]));

			if (scriptTextIsNotebookText) {
				if (*p == U'{') {
					if (braceDepth > 0)
						Melder_throw (U"Opening brace within a code chunk. Don't know how to look for a `form`.");
					braceDepth += 1;
				} else if (*p == U'}') {
					if (braceDepth <= 0)
						Melder_throw (U"Closing brace outside a code chunk. Don't know how to look for a `form`.");
					braceDepth -= 1;
				}
				if (braceDepth > 0) {
					Melder_skipHorizontalSpace (& p);
					if (str32nequ (p, U"form", 4) && (p [4] == U':' || Melder_isEndOfInk (p [4]))) {
						formLocation = p;
						break;
					}
				}
			} else {
				Melder_skipHorizontalSpace (& p);
				if (str32nequ (p, U"form", 4) && (p [4] == U':' || Melder_isEndOfInk (p [4]))) {
					formLocation = p;
					break;
				}
			}
			Melder_skipToEndOfLine (& p);
			if (*p == U'\0')
				break;
			p ++;   // skip newline symbol
		}
	}
	/*
		If there is no "form" line, there are no parameters.
	*/
	if (formLocation) {
		char32 *dialogTitle = Melder_findEndOfHorizontalSpace
				(formLocation [4] == U':' ? formLocation + 5 : formLocation + 4);
		char32 *endOfLine = Melder_findEndOfLine (dialogTitle);
		if (*endOfLine == U'\0')
			Melder_throw (U"Unfinished form (only a “form” line).");
		if (formLocation [4] == U':') {
			MelderString_empty (& string);
			if (dialogTitle [0] != U'"')
				Melder_throw (U"The title of a form should start with a double quote character (\").");
			char32* p = dialogTitle + 1;   // BUG: could be const (change Melder_skipHorizontalSpace())
			for (; p < endOfLine; p ++) {
				if (*p == U'"') {
					if (p [1] == U'"') {
						p ++;
						continue;
					} else
						break;
				}
				MelderString_appendCharacter (& string, *p);
			}
			if (*p != U'"')
				Melder_throw (U"Missing closing quote character in the title of the form.");
			p ++;   // skip closing quote
			Melder_skipHorizontalSpace (& p);
			if (! Melder_isEndOfLine (*p) && *p != U';')
				Melder_throw (U"Stray characters after closing quote in form title.");
			my dialogTitle = Melder_dup (string.string);
		} else {
			my dialogTitle = Melder_ndup (dialogTitle, endOfLine - dialogTitle);
		}
		my numberOfParameters = 0;
		while (true) {
			int type = 0;
			char32 *startOfLine = Melder_findEndOfHorizontalSpace (endOfLine + 1);
			/*
				Skip empty lines and lines with comments.
			*/
			while (*startOfLine == U'#' || *startOfLine == U';' || *startOfLine == U'!' || Melder_isEndOfLine (*startOfLine)) {
				endOfLine = Melder_findEndOfLine (startOfLine);
				if (Melder_isEndOfText (*endOfLine))
					Melder_throw (U"Unfinished form (missing “endform”).");
				startOfLine = Melder_findEndOfHorizontalSpace (endOfLine + 1);
			}
			endOfLine = Melder_findEndOfLine (startOfLine);
			/*
				Break on endform.
			*/
			if (str32nequ (startOfLine, U"endform", 7) && Melder_isEndOfInk (startOfLine [7]))
				break;
			/*
				Recognize parameter name, argument value,
				and perhaps format name and/or number of lines.
			*/
			char32 *p = nullptr;
			bool hasColon = false;
			auto isEndOfInkOrColon = [] (const char32 kar) { return kar == U':' || Melder_isEndOfInk (kar); };
			if (str32nequ (startOfLine, U"word", 4) && isEndOfInkOrColon (startOfLine [4])) {
				type = Interpreter_WORD;
				hasColon = ( startOfLine [4] == U':' );
				p = startOfLine + 4 + hasColon;
			} else if (str32nequ (startOfLine, U"sentence", 8) && isEndOfInkOrColon (startOfLine [8])) {
				type = Interpreter_SENTENCE;
				hasColon = ( startOfLine [8] == U':' );
				p = startOfLine + 8 + hasColon;
			} else if (str32nequ (startOfLine, U"infile", 6) && isEndOfInkOrColon (startOfLine [6])) {
				type = Interpreter_INFILE;
				hasColon = ( startOfLine [6] == U':' );
				p = startOfLine + 6 + hasColon;
			} else if (str32nequ (startOfLine, U"outfile", 7) && isEndOfInkOrColon (startOfLine [7])) {
				type = Interpreter_OUTFILE;
				hasColon = ( startOfLine [7] == U':' );
				p = startOfLine + 7 + hasColon;
			} else if (str32nequ (startOfLine, U"folder", 6) && isEndOfInkOrColon (startOfLine [6])) {
				type = Interpreter_FOLDER;
				hasColon = ( startOfLine [6] == U':' );
				p = startOfLine + 6 + hasColon;
			}  else if (str32nequ (startOfLine, U"text", 4) && isEndOfInkOrColon (startOfLine [4])) {
				type = Interpreter_TEXT;
				hasColon = ( startOfLine [4] == U':' );
				p = startOfLine + 4 + hasColon;
			} else if (str32nequ (startOfLine, U"real", 4) && isEndOfInkOrColon (startOfLine [4])) {
				type = Interpreter_REAL;
				hasColon = ( startOfLine [4] == U':' );
				p = startOfLine + 4 + hasColon;
			} else if (str32nequ (startOfLine, U"positive", 8) && isEndOfInkOrColon (startOfLine [8])) {
				type = Interpreter_POSITIVE;
				hasColon = ( startOfLine [8] == U':' );
				p = startOfLine + 8 + hasColon;
			} else if (str32nequ (startOfLine, U"integer", 7) && isEndOfInkOrColon (startOfLine [7])) {
				type = Interpreter_INTEGER;
				hasColon = ( startOfLine [7] == U':' );
				p = startOfLine + 7 + hasColon;
			} else if (str32nequ (startOfLine, U"natural", 7) && isEndOfInkOrColon (startOfLine [7])) {
				type = Interpreter_NATURAL;
				hasColon = ( startOfLine [7] == U':' );
				p = startOfLine + 7 + hasColon;
			} else if (str32nequ (startOfLine, U"boolean", 7) && isEndOfInkOrColon (startOfLine [7])) {
				type = Interpreter_BOOLEAN;
				hasColon = ( startOfLine [7] == U':' );
				p = startOfLine + 7 + hasColon;
			} else if (str32nequ (startOfLine, U"realvector", 10) && isEndOfInkOrColon (startOfLine [10])) {
				type = Interpreter_REALVECTOR;
				hasColon = ( startOfLine [10] == U':' );
				p = startOfLine + 10 + hasColon;
			} else if (str32nequ (startOfLine, U"positivevector", 14) && isEndOfInkOrColon (startOfLine [14])) {
				type = Interpreter_POSITIVEVECTOR;
				hasColon = ( startOfLine [14] == U':' );
				p = startOfLine + 14 + hasColon;
			} else if (str32nequ (startOfLine, U"integervector", 13) && isEndOfInkOrColon (startOfLine [13])) {
				type = Interpreter_INTEGERVECTOR;
				hasColon = ( startOfLine [13] == U':' );
				p = startOfLine + 13 + hasColon;
			} else if (str32nequ (startOfLine, U"naturalvector", 14) && isEndOfInkOrColon (startOfLine [14])) {
				type = Interpreter_NATURALVECTOR;
				hasColon = ( startOfLine [13] == U':' );
				p = startOfLine + 13 + hasColon;
			} else if (str32nequ (startOfLine, U"realmatrix", 10) && isEndOfInkOrColon (startOfLine [10])) {
				type = Interpreter_REALMATRIX;
				hasColon = ( startOfLine [10] == U':' );
				p = startOfLine + 10 + hasColon;
			} else if (str32nequ (startOfLine, U"choice", 6) && isEndOfInkOrColon (startOfLine [6])) {
				type = Interpreter_CHOICE;
				hasColon = ( startOfLine [6] == U':' );
				p = startOfLine + 6 + hasColon;
			} else if (str32nequ (startOfLine, U"optionmenu", 10) && isEndOfInkOrColon (startOfLine [10])) {
				type = Interpreter_OPTIONMENU;
				hasColon = ( startOfLine [10] == U':' );
				p = startOfLine + 10 + hasColon;
			} else if (str32nequ (startOfLine, U"button", 6) && isEndOfInkOrColon (startOfLine [6])) {
				type = Interpreter_BUTTON;
				hasColon = ( startOfLine [6] == U':' );
				p = startOfLine + 6 + hasColon;
			} else if (str32nequ (startOfLine, U"option", 6) && isEndOfInkOrColon (startOfLine [6])) {
				type = Interpreter_OPTION;
				hasColon = ( startOfLine [6] == U':' );
				p = startOfLine + 6 + hasColon;
			} else if (str32nequ (startOfLine, U"comment", 7) && isEndOfInkOrColon (startOfLine [7])) {
				type = Interpreter_COMMENT;
				hasColon = ( startOfLine [7] == U':' );
				p = startOfLine + 7 + hasColon;
			} else {
				*endOfLine = U'\0';   // destroy input in order to limit printing of parameter type
				Melder_throw (U"Unknown parameter type inside form:\n“", startOfLine, U"”.");
			}
			/*
				Example:
					form: "Something"
						real: "Time (s)", "3.14 (= pi)"
						choice: "Colour", 2
							button: "Red"
							button: "Green"
							button: "Blue"
					endform
				my parameters [1] := "Time (s)"
				my parameters [2] := "Colour"
				my parameters [3] := ""
				my parameters [4] := ""
				my parameters [5] := ""
				my arguments [1] := "3.14 (= pi)"
				my arguments [2] := "2"
				my arguments [3] := "Red"   (funny, but needed in Interpreter_getArgumentsFromString)
				my arguments [4] := "Green"
				my arguments [5] := "Blue"
				(last checked 2023-01-21)
			*/
			Melder_assert (p);
			const mutablestring32 parameter = my parameters [++ my numberOfParameters];
			if (type <= Interpreter_MAXIMUM_TYPE_WITH_VARIABLE_NAME) {
				Melder_skipHorizontalSpace (& p);
				if (Melder_isEndOfLine (*p)) {
					*p = U'\0';   // destroy input in order to limit printing of line
					Melder_throw (U"Missing parameter:\n“", startOfLine, U"”.");
				}
				MelderString_empty (& string);
				my numbersOfLines [my numberOfParameters] = 0;
				if (hasColon) {
					/*
						Parse optional number-of-lines (including final comma).
					*/
					if (Melder_isAsciiDecimalNumber (*p)) {
						if (!(
							type == Interpreter_TEXT ||
							type == Interpreter_INFILE || type == Interpreter_OUTFILE || type == Interpreter_FOLDER ||
							type >= Interpreter_MINIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE &&
									type <= Interpreter_MAXIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE
						)) {
							*(p + 1) = U'\0';
							Melder_throw (U"Only “text”, “infile”, “outfile”, “folder” and vector and matrix types "
									"support setting the number of lines in a form.");
						}
						my numbersOfLines [my numberOfParameters] = Melder_atoi (p);
						p ++;
						while (Melder_isAsciiDecimalNumber (*p))
							p ++;
						Melder_skipHorizontalSpace (& p);
						if (*p != U',') {
							*(p + 1) = U'\0';   // destroy input in order to limit printing of line
							Melder_throw (U"Missing comma after number of lines:\n", startOfLine);
						}
						p ++;   // skip comma
						Melder_skipHorizontalSpace (& p);
					}
					/*
						Parse obligatory variable name (including final comma).
					*/
					if (*p != U'"') {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"Missing opening quote for parameter name in form field:\n", startOfLine);
					}
					p ++;   // skip opening quote
					for (; p < endOfLine; p ++) {
						if (*p == U'"') {
							if (p [1] == U'"') {   // look ahead
								p ++;
								continue;
							} else
								break;
						}
						MelderString_appendCharacter (& string, *p);
					}
					if (*p != U'"') {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"Missing closing quote character in parameter name:\n", startOfLine);
					}
					p ++;   // skip closing quote
					Melder_skipHorizontalSpace (& p);
					if (*p != U',') {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"Missing comma after parameter name:\n", startOfLine);
					}
					p ++;   // skip comma
				} else {
					while (Melder_staysWithinInk (*p))
						MelderString_appendCharacter (& string, *(p ++));
				}
				MelderString_truncate (& string, Interpreter_MAX_PARAMETER_LENGTH);
				str32cpy (parameter, string.string);
				parameter [string.length] = U'\0';
				npar ++;
			} else {
				parameter [0] = U'\0';
			}
			Melder_skipHorizontalSpace (& p);
			/*
				Parse format type.
			*/
			if (type >= Interpreter_MINIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE && type <= Interpreter_MAXIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE) {
				if (! hasColon) {
					*p = U'\0';
					Melder_throw (U"Vector fields in forms are available only if immediately followed by a colon:\n", startOfLine);
				}
				MelderString_empty (& string);
				if (Melder_isEndOfLine (*p)) {
					*p = U'\0';   // destroy input in order to limit printing of line
					Melder_throw (U"Missing format:\n“", startOfLine, U"”.");
				}
				if (*p != U'"') {
					*(p + 1) = U'\0';   // destroy input in order to limit printing of line
					Melder_throw (U"Missing opening quote for vector format:\n", startOfLine);
				}
				p ++;   // skip opening quote
				for (; p < endOfLine; p ++) {
					if (*p == U'"') {
						if (p [1] == U'"') {   // look ahead
							p ++;
							continue;
						} else
							break;
					}
					MelderString_appendCharacter (& string, *p);
				}
				if (*p != U'"') {
					*(p + 1) = U'\0';   // destroy input in order to limit printing of line
					Melder_throw (U"Missing closing quote for vector format:\n", startOfLine);
				}
				p ++;   // skip closing quote
				Melder_skipHorizontalSpace (& p);
				if (*p != U',') {
					*(p + 1) = U'\0';   // destroy input in order to limit printing of line
					Melder_throw (U"Missing comma after vector format:\n", startOfLine);
				}
				p ++;   // skip comma
				Melder_skipHorizontalSpace (& p);
				/*
					Install format type.
				*/
				MelderString_truncate (& string, Interpreter_MAX_FORMAT_LENGTH);
				str32cpy (my formats [my numberOfParameters], string.string);
				my formats [my numberOfParameters] [string.length] = U'\0';
			}
			/*
				Parse obligatory content of argument.
			*/
			if (hasColon) {
				MelderString_empty (& string);
				if (Melder_isAsciiDecimalNumber (*p)) {
					if (type != Interpreter_CHOICE && type != Interpreter_OPTIONMENU && type != Interpreter_BOOLEAN) {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"Only “choice”, “optionmenu” and “boolean” fields can take a number:\n", startOfLine);
					}
					const integer value = Melder_atoi (p);
					if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU)
						MelderString_append (& string, value);
					else
						MelderString_appendCharacter (& string, value != 0 ? U'1' : U'0');
					p ++;   // skip first digit
					while (Melder_isAsciiDecimalNumber (*p))
						p ++;   // skip next digit
				} else {
					if (*p != U'"') {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"Missing opening quote after comma in form field:\n", startOfLine);
					}
					if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU) {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"A “choice” field can take only a number, not a string:\n", startOfLine);
					}
					p ++;   // skip opening quote
					for (; p < endOfLine; p ++) {
						if (*p == U'"') {
							if (p [1] == U'"') {
								p ++;
								continue;
							} else
								break;
						}
						MelderString_appendCharacter (& string, *p);
					}
					if (*p != U'"') {
						*(p + 1) = U'\0';   // destroy input in order to limit printing of line
						Melder_throw (U"Missing closing quote character in default argument:\n", startOfLine);
					}
					p ++;   // skip closing quote
				}
				Melder_skipHorizontalSpace (& p);
				if (! Melder_isEndOfLine (*p) && *p != U';')
					Melder_throw (U"Stray characters after default argument in form title:\n", startOfLine);
				my arguments [my numberOfParameters] = Melder_dup_f (string.string);
			} else {
				my arguments [my numberOfParameters] = Melder_ndup (p, endOfLine - p);
			}
			my types [my numberOfParameters] = type;
			if (Melder_isEndOfText (*endOfLine))
				Melder_throw (U"Unfinished form (missing “endform”).");
		}
	} else {
		npar = my numberOfParameters = 0;
	}
	return npar;
}

static void tidyUpParameterNames (Interpreter me, integer size) {
	for (integer ipar = 1; ipar <= size; ipar ++) {
		mutablestring32 p = my parameters [ipar];
		/*
			Ignore buttons and comments again (after `size` was
			made smaller than my numberOfParameters).
		*/
		if (! *p)
			continue;
		/*
			Change spaces to underscores.
			This has to come before stripping.
		*/
		for (p = my parameters [ipar]; *p != U'\0'; p ++)
			if (Melder_isHorizontalSpace (*p))
				*p = U'_';
		/*
			Strip parentheses off parameter name.
		*/
		p = my parameters [ipar];
		if ((p = str32chr (p, U'(')) != nullptr) {
			*p = U'\0';
			if (p - my parameters [ipar] > 0 && p [-1] == U'_')
				p [-1] = U'\0';
		}
		/*
			Strip colon off parameter name.
		*/
		p = my parameters [ipar];
		if (*p != U'\0' && p [Melder_length (p) - 1] == U':')
			p [Melder_length (p) - 1] = U'\0';
	}
}

autoUiForm Interpreter_createForm (Interpreter me, GuiWindow parent, Editor optionalEditor, conststring32 path,
	void (*okCallback) (UiForm, integer, Stackel, conststring32, Interpreter, conststring32, bool, void *, Editor), void *okClosure,
	bool selectionOnly)
{
	autoUiForm form = UiForm_create (parent, optionalEditor,
		Melder_cat (selectionOnly ? U"Run script (selection only): " : U"Run script: ", my dialogTitle.get()),
		okCallback, okClosure, nullptr, nullptr);
	if (path)
		form -> scriptFilePath = Melder_dup (path);
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		/*
			Convert underscores to spaces.
		*/
		char32 parameter [100], *p = & parameter [0];
		str32cpy (parameter, my parameters [ipar]);
		while (*p) {
			if (*p == U'_')
				*p = U' ';
			p ++;
		}
		switch (my types [ipar]) {
			case Interpreter_WORD: {
				UiForm_addWord (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			} break; case Interpreter_SENTENCE: {
				UiForm_addSentence (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			} break; case Interpreter_TEXT: {
				if (my numbersOfLines [ipar] == 0)
					UiForm_addText (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
				else
					UiForm_addText (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_INFILE: {
				if (my numbersOfLines [ipar] == 0)
					UiForm_addInfile (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
				else
					UiForm_addInfile (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_OUTFILE: {
				if (my numbersOfLines [ipar] == 0)
					UiForm_addOutfile (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
				else
					UiForm_addOutfile (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_FOLDER: {
				if (my numbersOfLines [ipar] == 0)
					UiForm_addFolder (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
				else
					UiForm_addFolder (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_REAL: {
				UiForm_addReal (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());   // TODO: an address of a real variable
			} break; case Interpreter_POSITIVE: {
				UiForm_addPositive (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			} break; case Interpreter_INTEGER: {
				UiForm_addInteger (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			} break; case Interpreter_NATURAL: {
				UiForm_addNatural (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			} break; case Interpreter_BOOLEAN: {
				mutablestring32 arg = & my arguments [ipar] [0];
				if (
					str32equ (arg, U"1") ||
					str32equ (arg, U"yes") || str32equ (arg, U"on") ||
					str32equ (arg, U"Yes") || str32equ (arg, U"On") ||
					str32equ (arg, U"YES") || str32equ (arg, U"ON")
				) {
					str32cpy (arg, U"1");
				} else if (
					str32equ (arg, U"0") ||
					str32equ (arg, U"no") || str32equ (arg, U"off") ||
					str32equ (arg, U"No") || str32equ (arg, U"Off") ||
					str32equ (arg, U"NO") || str32equ (arg, U"OFF")
				) {
					str32cpy (arg, U"0");
				} else {
					Melder_throw (U"Unknown value “", arg, U"” for boolean “", my parameters [ipar], U"”.");
				}
				UiForm_addBoolean (form.get(), nullptr, nullptr, parameter, my arguments [ipar] [0] == U'1');
			} break; case Interpreter_REALVECTOR: {
				kUi_realVectorFormat format = kUi_realVectorFormat_getValue (my formats [ipar]);
				if (format == kUi_realVectorFormat::UNDEFINED)
					Melder_throw (U"Undefined real vector format “", my formats [ipar], U"”.");
				if (my numbersOfLines [ipar] == 0)
					UiForm_addRealVector (form.get(), nullptr, nullptr, parameter, format, my arguments [ipar].get());
				else
					UiForm_addRealVector (form.get(), nullptr, nullptr, parameter, format, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_POSITIVEVECTOR: {
				kUi_realVectorFormat format = kUi_realVectorFormat_getValue (my formats [ipar]);
				if (format == kUi_realVectorFormat::UNDEFINED)
					Melder_throw (U"Undefined positive vector format “", my formats [ipar], U"”.");
				if (my numbersOfLines [ipar] == 0)
					UiForm_addPositiveVector (form.get(), nullptr, nullptr, parameter, format, my arguments [ipar].get());
				else
					UiForm_addPositiveVector (form.get(), nullptr, nullptr, parameter, format, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_INTEGERVECTOR: {
				kUi_integerVectorFormat format = kUi_integerVectorFormat_getValue (my formats [ipar]);
				if (format == kUi_integerVectorFormat::UNDEFINED)
					Melder_throw (U"Undefined integer vector format “", my formats [ipar], U"”.");
				if (my numbersOfLines [ipar] == 0)
					UiForm_addIntegerVector (form.get(), nullptr, nullptr, parameter, format, my arguments [ipar].get());
				else
					UiForm_addIntegerVector (form.get(), nullptr, nullptr, parameter, format, my arguments [ipar].get(),
							my numbersOfLines [ipar]);
			} break; case Interpreter_NATURALVECTOR: {
				kUi_integerVectorFormat format = kUi_integerVectorFormat_getValue (my formats [ipar]);
				if (format == kUi_integerVectorFormat::UNDEFINED)
					Melder_throw (U"Undefined natural vector format “", my formats [ipar], U"”.");
				if (my numbersOfLines [ipar] == 0)
					UiForm_addNaturalVector (form.get(), nullptr, nullptr, parameter,
						kUi_integerVectorFormat_getValue (my formats [ipar]), my arguments [ipar].get()
					);
				else
					UiForm_addNaturalVector (form.get(), nullptr, nullptr, parameter,
						kUi_integerVectorFormat_getValue (my formats [ipar]), my arguments [ipar].get(),
						my numbersOfLines [ipar]
					);
			} break; case Interpreter_REALMATRIX: {
				Melder_throw (U"Cannot handle matrices in forms yet.");   // TODO
			//	UiForm_addRealMatrix (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			} break; case Interpreter_CHOICE: {
				UiForm_addChoice (form.get(), nullptr, nullptr, nullptr, parameter, (int) Melder_atoi (my arguments [ipar].get()), 1);
			} break; case Interpreter_OPTIONMENU: {
				UiForm_addOptionMenu (form.get(), nullptr, nullptr, nullptr, parameter, (int) Melder_atoi (my arguments [ipar].get()), 1);
			} break; case Interpreter_BUTTON: {
				UiForm_addOption (form.get(), my arguments [ipar].get());
			} break; case Interpreter_OPTION: {
				UiForm_addOption (form.get(), my arguments [ipar].get());
			} break; case Interpreter_HEADING: {
				UiForm_addHeading (form.get(), nullptr, my arguments [ipar].get());
			} break; case Interpreter_COMMENT: {
				UiForm_addComment (form.get(), nullptr, my arguments [ipar].get());
			} break; case Interpreter_CAPTION: {
				UiForm_addCaption (form.get(), nullptr, my arguments [ipar].get());
			} break; default: {
				UiForm_addWord (form.get(), nullptr, nullptr, parameter, my arguments [ipar].get());
			}
		}
	}
	tidyUpParameterNames (me, my numberOfParameters);
	UiForm_finish (form.get());
	return form;
}

void Interpreter_getArgumentsFromDialog (Interpreter me, UiForm dialog) {
	tidyUpParameterNames (me, my numberOfParameters);
	for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
		char32 parameter [1+Interpreter_MAX_PARAMETER_LENGTH];
		/*
			Convert underscores to spaces.
		*/
		str32cpy (parameter, my parameters [ipar]);
		char32 *p = & parameter [0];
		while (*p) {
			if (*p == U'_')
				*p = U' ';
			p ++;
		}
		switch (my types [ipar]) {
			case Interpreter_WORD:
			case Interpreter_SENTENCE:
			case Interpreter_TEXT:
			{
				const conststring32 value = UiForm_getString (dialog, parameter);
				my arguments [ipar] = Melder_dup_f (value);
				break;
			}
			case Interpreter_INFILE:
			case Interpreter_OUTFILE:
			case Interpreter_FOLDER:
			{
				const conststring32 value = UiForm_getString (dialog, parameter);
				structMelderFile file { };
				Melder_relativePathToFile (value, & file);   // the working directory should have been set to the script file path TODO: check
				my arguments [ipar] = Melder_dup_f (MelderFile_peekPath (& file));
				break;
			}
			case Interpreter_REAL:
			case Interpreter_POSITIVE:
			{
				const double value = UiForm_getReal_check (dialog, parameter);
				my arguments [ipar] = autostring32 (40, true);
				Melder_sprint (my arguments [ipar].get(),40+1, value);
				break;
			}
			case Interpreter_INTEGER:
			case Interpreter_NATURAL:
			case Interpreter_BOOLEAN:
			{
				const integer value = UiForm_getInteger (dialog, parameter);
				my arguments [ipar] = autostring32 (40, true);
				Melder_sprint (my arguments [ipar].get(),40+1, value);
				break;
			}
			case Interpreter_REALVECTOR:
			case Interpreter_POSITIVEVECTOR:
			{
				const VEC realVectorValue = UiForm_getRealVector (dialog, parameter);
				autoMelderString buffer;
				for (integer i = 1; i <= realVectorValue.size; i ++) {
					MelderString_append (& buffer, realVectorValue [i]);
					if (i < realVectorValue.size)
						MelderString_appendCharacter (& buffer, U' ');
				}
				my arguments [ipar] = Melder_dup (buffer.string);
				break;
			}
			case Interpreter_INTEGERVECTOR:
			case Interpreter_NATURALVECTOR:
			{
				const INTVEC integerVectorValue = UiForm_getIntegerVector (dialog, parameter);
				autoMelderString buffer;
				for (integer i = 1; i <= integerVectorValue.size; i ++) {
					MelderString_append (& buffer, integerVectorValue [i]);
					if (i < integerVectorValue.size)
						MelderString_appendCharacter (& buffer, U' ');
				}
				my arguments [ipar] = Melder_dup (buffer.string);
				break;
			}
			case Interpreter_REALMATRIX:
			{
				Melder_throw (U"Cannot handle matrices in forms yet");
				break;
			}
			case Interpreter_CHOICE:
			case Interpreter_OPTIONMENU: {
				const integer integerValue = UiForm_getInteger (dialog, parameter);
				const conststring32 stringValue = UiForm_getString (dialog, parameter);
				my arguments [ipar] = autostring32 (40, true);
				Melder_sprint (my arguments [ipar].get(),40+1, integerValue);
				Melder_sprint (my choiceArguments [ipar],100, stringValue);
				break;
			}
			case Interpreter_BUTTON:
			case Interpreter_OPTION:
			case Interpreter_COMMENT:
				break;
			default:
				Melder_crash (U"Unhandled parameter type ", my types [ipar]);
		}
	}
}

static void convertBooleansAndChoicesToNumbersAndRelativeToAbsolutePaths (Interpreter me, integer size) {
	for (integer ipar = 1; ipar <= size; ipar ++) {
		if (
			my types [ipar] == Interpreter_INFILE ||
			my types [ipar] == Interpreter_OUTFILE ||
			my types [ipar] == Interpreter_FOLDER
		) {
			structMelderFile file { };
			Melder_relativePathToFile (my arguments [ipar].get(), & file);
			my arguments [ipar] = Melder_dup_f (MelderFile_peekPath (& file));
		} else if (my types [ipar] == Interpreter_BOOLEAN) {
			mutablestring32 arg = & my arguments [ipar] [0];
			if (
				str32equ (arg, U"1") ||
				str32equ (arg, U"yes") || str32equ (arg, U"on") ||
				str32equ (arg, U"Yes") || str32equ (arg, U"On") ||
				str32equ (arg, U"YES") || str32equ (arg, U"ON")
			) {
				str32cpy (arg, U"1");
			} else if (
				str32equ (arg, U"0") ||
				str32equ (arg, U"no") || str32equ (arg, U"off") ||
				str32equ (arg, U"No") || str32equ (arg, U"Off") ||
				str32equ (arg, U"NO") || str32equ (arg, U"OFF")
			) {
				str32cpy (arg, U"0");
			} else {
				Melder_throw (U"Unknown value \"", arg, U"\" for boolean \"", my parameters [ipar], U"\".");
			}
		} else if (my types [ipar] == Interpreter_CHOICE) {
			integer jpar;
			mutablestring32 arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_BUTTON && my types [jpar] != Interpreter_OPTION)
					Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar].get(), arg)) {   // the button labels are in the arguments; see Interpreter_readParameters
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar].get());
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for choice \"", my parameters [ipar], U"\".");
		} else if (my types [ipar] == Interpreter_OPTIONMENU) {
			integer jpar;
			mutablestring32 arg = & my arguments [ipar] [0];
			for (jpar = ipar + 1; jpar <= my numberOfParameters; jpar ++) {
				if (my types [jpar] != Interpreter_OPTION && my types [jpar] != Interpreter_BUTTON)
					Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
				if (str32equ (my arguments [jpar].get(), arg)) {
					str32cpy (arg, Melder_integer (jpar - ipar));
					str32cpy (my choiceArguments [ipar], my arguments [jpar].get());
					break;
				}
			}
			if (jpar > my numberOfParameters)
				Melder_throw (U"Unknown value \"", arg, U"\" for option menu \"", my parameters [ipar], U"\".");
		}
	}
}

void Interpreter_getArgumentsFromString (Interpreter me, conststring32 arguments) {
	int size = my numberOfParameters;
	const integer length = Melder_length (arguments);
	while (size >= 1 && my parameters [size] [0] == U'\0')
		size --;   /* Ignore fields without a variable name (button, comment). */
	tidyUpParameterNames (me, size);
	for (int ipar = 1; ipar < size; ipar ++) {
		int ichar = 0;
		/*
			Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		*/
		if (my parameters [ipar] [0] == U'\0')
			continue;
		/*
			Erase the current values, probably the default values,
			and replace with the actual arguments.
		*/
		my arguments [ipar] = autostring32 (length);
		/*
			Skip spaces until next argument.
		*/
		Melder_skipHorizontalSpace (& arguments);
		/*
			The argument is everything up to the next space, or, if it starts with a double quote,
			everything between this quote and the matching double quote;
			in this case, the argument can represent a double quote by a sequence of two double quotes.
			Example: the string
				"I said ""hello"""
			will be passed to the dialog as a single argument containing the text
				I said "hello"
		*/
		if (*arguments == U'\"') {
			arguments ++;   // do not include leading double quote
			for (;;) {
				if (*arguments == U'\0')
					Melder_throw (U"Missing matching quote.");
				if (*arguments == U'\"' && * ++ arguments != U'\"')
					break;   // remember second quote
				my arguments [ipar] [ichar ++] = *arguments ++;
			}
		} else {
			while (Melder_staysWithinInk (*arguments))
				my arguments [ipar] [ichar ++] = *arguments ++;
		}
		my arguments [ipar] [ichar] = U'\0';   // trailing null byte
	}
	/*
		The last item is handled separately, because it consists of the rest of the line.
		Leading spaces are skipped, but trailing spaces are included.
	*/
	if (size > 0) {
		Melder_skipHorizontalSpace (& arguments);
		my arguments [size] = Melder_dup_f (arguments);
	}
	convertBooleansAndChoicesToNumbersAndRelativeToAbsolutePaths (me, size);
}

void Interpreter_getArgumentsFromArgs (Interpreter me, integer narg, Stackel args) {
	trace (narg, U" arguments");
	integer size = my numberOfParameters;
	while (size >= 1 && my parameters [size] [0] == U'\0')
		size --;   // ignore trailing fields without a variable name (button, comment)
	tidyUpParameterNames (me, size);
	integer iarg = 0;
	for (integer ipar = 1; ipar <= size; ipar ++) {
		/*
			Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		*/
		if (my parameters [ipar] [0] == U'\0')
			continue;
		/*
			Erase the current values, probably the default values...
		*/
		my arguments [ipar]. reset();   //
		if (iarg == narg)
			Melder_throw (U"Found ", narg, U" arguments but expected more.");
		Stackel arg = & args [++ iarg];
		/*
			... and replace with the actual arguments. BUG: this loses type information.
		*/
		my arguments [ipar] =
			arg -> which == Stackel_NUMBER ? Melder_dup (Melder_double (arg -> number)) :
			arg -> which == Stackel_STRING ? Melder_dup (arg -> getString()) :
			arg -> which == Stackel_NUMERIC_VECTOR ? Melder_dup (Melder_VEC (arg -> numericVector, true)) :
			autostring32();
		Melder_assert (my arguments [ipar]);
	}
	if (iarg < narg)
		Melder_throw (U"Found ", narg, U" arguments but expected only ", iarg, U".");
	convertBooleansAndChoicesToNumbersAndRelativeToAbsolutePaths (me, size);
}

void Interpreter_getArgumentsFromCommandLine (Interpreter me, integer argc, char **argv) {
	trace (argc, U" arguments");
	integer size = my numberOfParameters;
	while (size >= 1 && my parameters [size] [0] == U'\0')
		size --;   // ignore trailing fields without a variable name (button, comment)
	tidyUpParameterNames (me, size);
	integer iarg = 0;
	for (integer ipar = 1; ipar <= size; ipar ++) {
		/*
			Ignore buttons and comments again. The buttons will keep their labels as "arguments".
		*/
		if (my parameters [ipar] [0] == U'\0')
			continue;
		/*
			Erase the current values, probably the default values,
			and replace with the actual arguments.
		*/
		if (iarg == argc)
			Melder_throw (U"Found ", argc, U" arguments but expected more.");
		my arguments [ipar] = Melder_8to32_e (argv [iarg ++]);
		Melder_assert (my arguments [ipar]);
	}
	if (iarg < argc)
		Melder_throw (U"Found ", argc, U" arguments but expected only ", iarg, U".");
	convertBooleansAndChoicesToNumbersAndRelativeToAbsolutePaths (me, size);
}

static void Interpreter_addNumericVariable (Interpreter me, conststring32 key, double value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> numericValue = value;
	my variablesMap [key] = variable.move();
	variable.releaseToAmbiguousOwner();
}

static void Interpreter_addStringVariable (Interpreter me, conststring32 key, conststring32 value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> stringValue = Melder_dup (value);
	my variablesMap [key] = variable.move();
	variable.releaseToAmbiguousOwner();
}

static void Interpreter_addNumericVectorVariable (Interpreter me, conststring32 key, conststring32 value) {
	autoInterpreterVariable variable = InterpreterVariable_create (key);
	variable -> numericVectorValue = splitByWhitespace_VEC (value);
	my variablesMap [key] = variable.move();
	variable.releaseToAmbiguousOwner();
}

InterpreterVariable Interpreter_hasVariable (Interpreter me, conststring32 key) {
	Melder_pre (key);
	conststring32 variableNameIncludingProcedureName =
			( key [0] == U'.' ? Melder_cat (my procedureStackNames [my callDepth], key) : key );
	auto it = my variablesMap. find (variableNameIncludingProcedureName);
	if (it != my variablesMap.end())
		return it -> second.get();
	else
		return nullptr;
}

InterpreterVariable Interpreter_lookUpVariable (Interpreter me, conststring32 key) {
	Melder_pre (key);
	conststring32 variableNameIncludingProcedureName =
			( key [0] == U'.' ? Melder_cat (my procedureStackNames [my callDepth], key) : key );
	auto it = my variablesMap. find (variableNameIncludingProcedureName);
	if (it != my variablesMap.end())
		return it -> second.get();
	/*
		The variable doesn't yet exist: create a new one.
	*/
	autoInterpreterVariable variable = InterpreterVariable_create (variableNameIncludingProcedureName);
	InterpreterVariable variable_ref = variable.get();
	my variablesMap [variableNameIncludingProcedureName] = variable.move();
	return variable_ref;
}

static integer lookupLabel (Interpreter me, conststring32 labelName) {
	const integer position = NUMfindInSorted (my labelNames.get(), labelName);
	if (position == 0)
		Melder_throw (U"Unknown label \"", labelName, U"\" (we know of ", my labelNames.size, U" labels).");
	return position;
}

static bool isCommand (conststring32 string) {
	const char32 *p = & string [0];
	/*
		Things that start with "nowarn", "noprogress", or "nocheck" are commands.
	*/
	if (p [0] == U'n' && p [1] == U'o' &&
			(str32nequ (p + 2, U"warn ", 5) || str32nequ (p + 2, U"progress ", 9) || str32nequ (p + 2, U"check ", 6)))
		return true;
	if (str32nequ (p, U"demo ", 5))
		return true;
	/*
		Otherwise, things that start with nonupper case are formulas.
	*/
	if (! Melder_isUpperCaseLetter (*p))
		return false;
	/*
		The remaining possibility is things that start with upper case.
		If they contain an underscore, they are object names, hence we must have a formula.
		Otherwise, we have a command.
	*/
	while (Melder_isAlphanumeric (*p))
		p ++;
	return *p != U'_';
}

static void parameterToVariable (Interpreter me, int type, conststring32 in_parameter, int ipar) {
	Melder_pre (type != 0);
	char32 parameter [200];
	str32cpy (parameter, in_parameter);
	if (type >= Interpreter_MINIMUM_TYPE_FOR_NUMERIC_VARIABLE && type <= Interpreter_MAXIMUM_TYPE_FOR_NUMERIC_VARIABLE) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar].get()));
	} else if (type == Interpreter_CHOICE || type == Interpreter_OPTIONMENU) {
		Interpreter_addNumericVariable (me, parameter, Melder_atof (my arguments [ipar].get()));
		str32cat (parameter, U"$");
		Interpreter_addStringVariable (me, parameter, my choiceArguments [ipar]);
	} else if (type == Interpreter_BUTTON || type == Interpreter_OPTION || type == Interpreter_COMMENT) {
		;   // do not add a variable
	} else if (type >= Interpreter_MINIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE && type <= Interpreter_MAXIMUM_TYPE_FOR_NUMERIC_VECTOR_VARIABLE) {
		str32cat (parameter, U"#");
		Interpreter_addNumericVectorVariable (me, parameter, my arguments [ipar].get());
	} else {
		str32cat (parameter, U"$");
		Interpreter_addStringVariable (me, parameter, my arguments [ipar].get());
	}
}

inline static void NumericVectorVariable_move (InterpreterVariable variable, VEC movedVector, bool rightHandSideOwned) {
	if (rightHandSideOwned) {
		/*
			Statement like: a# = b# + c#
		*/
		variable -> numericVectorValue. adoptFromAmbiguousOwner (movedVector);
	} else if (variable -> numericVectorValue.size == movedVector.size) {
		if (variable -> numericVectorValue.cells == movedVector.cells) {
			/*
				Statement like: a# = a#
			*/
			(void) 0;   // assigning a variable to itself: do nothing
		} else {
			/*
				Statement like: a# = b#   // with matching sizes
			*/
			variable -> numericVectorValue.all()  <<=  movedVector;
		}
	} else {
		/*
			Statement like: a# = b#   // with non-matching sizes
		*/
		variable -> numericVectorValue = copy_VEC (movedVector);
	}
}

inline static void StringArrayVariable_move (InterpreterVariable variable, STRVEC movedVector, bool rightHandSideOwned) {
	if (rightHandSideOwned) {
		/*
			Statement like: a$# = b$# + c$#
		*/
		variable -> stringArrayValue. adoptFromAmbiguousOwner (movedVector);
	} else if (variable -> stringArrayValue.size == movedVector.size) {
		if ((char32 **) variable -> stringArrayValue.elements == movedVector.elements) {
			/*
				Statement like: a$# = a$#
			*/
			(void) 0;   // assigning a variable to itself: do nothing
		} else {
			/*
				Statement like: a$# = b$#   // with matching sizes
			*/
			variable -> stringArrayValue.all()  <<=  movedVector;
		}
	} else {
		/*
			Statement like: a$# = b$#   // with non-matching sizes
		*/
		variable -> stringArrayValue = copy_STRVEC (movedVector);
	}
}

inline static void NumericMatrixVariable_move (InterpreterVariable variable, MAT movedMatrix, bool rightHandSideOwned) {
	if (rightHandSideOwned) {
		/*
			Statement like: a## = b## + c##
		*/
		variable -> numericMatrixValue. adoptFromAmbiguousOwner (movedMatrix);
	} else if (variable -> numericMatrixValue.nrow == movedMatrix.nrow && variable -> numericMatrixValue.ncol == movedMatrix.ncol) {
		if (variable -> numericMatrixValue.cells == movedMatrix.cells) {
			/*
				Statement like: a## = a##
			*/
			(void) 0;   // assigning a variable to itself: do nothing
		} else {
			/*
				Statement like: a## = b##   // with matching sizes
			*/
			variable -> numericMatrixValue.all()  <<=  movedMatrix;
		}
	} else {
		/*
			Statement like: a## = b##   // with non-matching sizes
		*/
		variable -> numericMatrixValue = copy_MAT (movedMatrix);
	}
}

inline static void NumericVectorVariable_add (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  +=  scalar;
}
inline static void NumericVectorVariable_add (InterpreterVariable variable, constVEC vector) {
	VEC const& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size == variableVector.size,
		U"You cannot add a vector with size ", vector.size,
		U" to a vector with a different size (", variableVector.size, U")."
	);
	variableVector  +=  vector;
}
inline static void NumericVectorVariable_subtract (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  -=  scalar;
}
inline static void NumericVectorVariable_subtract (InterpreterVariable variable, constVEC vector) {
	VEC const& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size == variable -> numericVectorValue.size,
		U"You cannot subtract a vector with size ", vector.size,
		U" from a vector with a different size (", variableVector.size, U")."
	);
	variableVector  -=  vector;
}
inline static void NumericVectorVariable_multiply (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  *=  scalar;
}
inline static void NumericVectorVariable_multiply (InterpreterVariable variable, constVEC vector) {
	VEC const& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size != variableVector.size,
		U"You cannot multiply a vector with size ", variableVector.size,
		U" with a vector with a different size (", vector.size, U")."
	);
	variableVector  *=  vector;
}
inline static void NumericVectorVariable_divide (InterpreterVariable variable, double scalar) {
	variable -> numericVectorValue.all()  /=  scalar;
}
inline static void NumericVectorVariable_divide (InterpreterVariable variable, constVEC vector) {
	VEC const& variableVector = variable -> numericVectorValue.get();
	Melder_require (vector.size != variableVector.size,
		U"You cannot divide a vector with size ", variableVector.size,
		U" by a vector with a different size (", vector.size, U")."
	);
	variableVector  /=  vector;
}
inline static void NumericMatrixVariable_add (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] += scalar;
}
inline static void NumericMatrixVariable_add (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot add a matrix with size ", matrix.nrow, U"x", matrix.ncol,
		              U" to a matrix with a different size (", variableMatrix.nrow, U"x", variableMatrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] += matrix [irow] [icol];
}
inline static void NumericMatrixVariable_subtract (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] -= scalar;
}
inline static void NumericMatrixVariable_subtract (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot subtract a matrix with size ", matrix.nrow, U"x", matrix.ncol,
		              U" from a matrix with a different size (", variableMatrix.nrow, U"x", variableMatrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] -= matrix [irow] [icol];
}
inline static void NumericMatrixVariable_multiply (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] *= scalar;
}
inline static void NumericMatrixVariable_multiply (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot multiply a matrix with size ", variableMatrix.nrow, U"x", variableMatrix.ncol,
		              U" from a matrix with a different size (", matrix.nrow, U"x", matrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] *= matrix [irow] [icol];
}
inline static void NumericMatrixVariable_divide (InterpreterVariable variable, double scalar) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] /= scalar;
}
inline static void NumericMatrixVariable_divide (InterpreterVariable variable, constMAT matrix) {
	MAT variableMatrix = variable -> numericMatrixValue.get();
	if (matrix.nrow != variableMatrix.nrow || matrix.ncol != variableMatrix.ncol)
		Melder_throw (U"You cannot divide a matrix with size ", variableMatrix.nrow, U"x", variableMatrix.ncol,
		              U" by a matrix with a different size (", matrix.nrow, U"x", matrix.ncol, U").");
	for (integer irow = 1; irow <= variableMatrix.nrow; irow ++)
		for (integer icol = 1; icol <= variableMatrix.ncol; icol ++)
			variableMatrix [irow] [icol] /= matrix [irow] [icol];
}

static void Interpreter_do_procedureCall (Interpreter me, char32 *command,
	constvector <mutablestring32> const& lines, integer& lineNumber, integer callStack [], int& callDepth)
{
	/*
		Modern type of procedure calls, with comma separation, quoted strings, and array support.

		We just passed the `@` sign, so we continue by looking for a procedure name at the call site.
	*/
	char32 *p = command;
	Melder_skipHorizontalSpace (& p);
	char32 *callName = p;
	while (Melder_staysWithinInk (*p) && *p != U'(' && *p != U':')
		p ++;
	if (p == callName) Melder_throw (U"Missing procedure name after \"@\".");
	bool hasArguments = ( *p != U'\0' );
	if (hasArguments) {
		bool parenthesisOrColonFound = ( *p == U'(' || *p == U':' );
		*p = U'\0';   // close procedure name
		if (! parenthesisOrColonFound) {
			p ++;   // step over first white space
			Melder_skipHorizontalSpace (& p);
			hasArguments = ( *p != U'\0' );
			parenthesisOrColonFound = ( *p == U'(' || *p == U':' );
			if (hasArguments && ! parenthesisOrColonFound)
				Melder_throw (U"Missing parenthesis or colon after procedure name \"", callName, U"\".");
		}
		p ++;   // step over parenthesis or colon
	}
	const integer callLength = Melder_length (callName);
	integer iline = 1;
	for (; iline <= lines.size; iline ++) {
		if (! str32nequ (lines [iline], U"procedure", 9) || ! Melder_isHorizontalSpace (lines [iline] [9]))
			continue;
		char32 *q = lines [iline] + 10;
		Melder_skipHorizontalSpace (& q);   // skip whitespace before procedure name
		char32 *const procName = q;
		while (Melder_staysWithinInk (*q) && *q != U'(' && *q != U':')
			q ++;
		if (q == procName)
			Melder_throw (U"Missing procedure name after 'procedure'.");
		if (q - procName == callLength && str32nequ (procName, callName, callLength)) {
			/*
				We found the procedure definition.
			*/
			if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
				Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
			str32cpy (my procedureStackNames [my callDepth], callName);
			const bool parenthesisOrColonFound = ( *q == U'(' || *q == U':' );
			if (*q)
				q ++;   // step over parenthesis or colon or first white space
			if (! parenthesisOrColonFound) {
				Melder_skipHorizontalSpace (& q);
				if (*q == U'(' || *q == U':')
					q ++;   // step over parenthesis or colon
			}
			while (*q && *q != U')' && *q != U';') {
				static MelderString argument;
				MelderString_empty (& argument);
				Melder_skipHorizontalSpace (& p);
				Melder_skipHorizontalSpace (& q);
				conststring32 parameterName = q;
				while (Melder_staysWithinInk (*q) && *q != U',' && *q != U')' && *q != U';')
					q ++;   // collect parameter name
				int expressionDepth = 0;
				for (; *p; p ++) {
					if (*p == U',') {
						if (expressionDepth == 0)
							break;   // depth-0 comma ends expression
						MelderString_appendCharacter (& argument, U',');
					} else if (*p == U')' || *p == U']' || *p == U'}') {
						if (expressionDepth == 0)
							break;   // depth-0 closing parenthesis ends expression
						expressionDepth --;
						MelderString_appendCharacter (& argument, *p);
					} else if (*p == U'(' || *p == U'[' || *p == U'{') {
						expressionDepth ++;
						MelderString_appendCharacter (& argument, *p);
					} else if (*p == U'“') {
						/*
							Enter a string literal.
						*/
						MelderString_appendCharacter (& argument, U'\"');
						p ++;
						for (;; p ++) {
							if (*p == U'\0') {
								Melder_throw (U"Incomplete curly string literal: the quotes don't match.");
							} else if (*p == U'\"') {
								MelderString_appendCharacter (& argument, U'\"');
								MelderString_appendCharacter (& argument, U'\"');
							} else if (*p == U'”') {
								MelderString_appendCharacter (& argument, U'\"');
								break;
							} else {
								MelderString_appendCharacter (& argument, *p);
							}
						}
					} else if (*p == U'\"') {
						/*
							Enter a string literal.
						*/
						MelderString_appendCharacter (& argument, U'\"');
						p ++;
						for (;; p ++) {
							if (*p == U'\0') {
								Melder_throw (U"Incomplete string literal: the quotes don't match.");
							} else if (*p == U'\"') {
								MelderString_appendCharacter (& argument, U'\"');
								if (p [1] == '\"') {
									p ++;   // stay in the string literal
									MelderString_appendCharacter (& argument, U'\"');
								} else {
									break;
								}
							} else {
								MelderString_appendCharacter (& argument, *p);
							}
						}
					} else {
						MelderString_appendCharacter (& argument, *p);
					}
				}
				if (q == parameterName)
					break;
				if (*p) {
					*p = U'\0';
					p ++;
				}
				if (q [-1] == U'$') {
					my callDepth --;
					autostring32 value = Interpreter_stringExpression (me, argument.string);
					my callDepth ++;
					const char32 save = *q;
					*q = U'\0';
					InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
					*q = save;
					var -> stringValue = value.move();
				} else if (q [-1] == U'#') {
					if (q [-2] == U'#') {
						MAT value;
						bool owned;
						my callDepth --;
						Interpreter_numericMatrixExpression (me, argument.string, & value, & owned);
						my callDepth ++;
						const char32 save = *q;
						*q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
						*q = save;
						NumericMatrixVariable_move (var, value, owned);
					} else if (q [-2] == U'$') {
						STRVEC value;
						bool owned;
						my callDepth --;
						Interpreter_stringArrayExpression (me, argument.string, & value, & owned);
						my callDepth ++;
						const char32 save = *q;
						*q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
						*q = save;
						StringArrayVariable_move (var, value, owned);
					} else {
						VEC value;
						bool owned;
						my callDepth --;
						Interpreter_numericVectorExpression (me, argument.string, & value, & owned);
						my callDepth ++;
						const char32 save = *q;
						*q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
						*q = save;
						NumericVectorVariable_move (var, value, owned);
					}
				} else {
					double value;
					my callDepth --;
					Interpreter_numericExpression (me, argument.string, & value);
					my callDepth ++;
					const char32 save = *q;
					*q = U'\0';
					InterpreterVariable var = Interpreter_lookUpVariable (me, parameterName);
					*q = save;
					var -> numericValue = value;
				}
				if (*q)
					q ++;   // skip comma
			}
			if (callDepth == Interpreter_MAX_CALL_DEPTH)
				Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
			callStack [++ callDepth] = lineNumber;
			lineNumber = iline;
			break;
		}
	}
	if (iline > lines.size)
		Melder_throw (U"Procedure \"", callName, U"\" not found.");
}
static void Interpreter_do_oldProcedureCall (Interpreter me, char32 *command,
	constvector <mutablestring32> const& lines, integer& lineNumber, integer callStack [], int& callDepth)
{
	/*
		Old type of procedure calls, with space separation, unquoted strings, and no array support.
	*/
	char32 *p = command;
	Melder_skipHorizontalSpace (& p);
	char32 *callName = p;
	while (*p != U'\0' && ! Melder_isHorizontalSpace (*p) && *p != U'(' && *p != U':')
		p ++;
	if (p == callName)
		Melder_throw (U"Missing procedure name after 'call'.");
	bool hasArguments = ( *p != U'\0' );
	*p = U'\0';   // close procedure name
	const integer callLength = Melder_length (callName);
	integer iline = 1;
	for (; iline <= lines.size; iline ++) {
		if (! str32nequ (lines [iline], U"procedure ", 10))
			continue;
		char32 *q = lines [iline] + 10;
		Melder_skipHorizontalSpace (& q);
		char32 *procName = q;
		while (*q != U'\0' && ! Melder_isHorizontalSpace (*q) && *q != U'(' && *q != U':')
			q ++;
		if (q == procName)
			Melder_throw (U"Missing procedure name after 'procedure'.");
		bool hasParameters = ( *q != U'\0' );
		if (q - procName == callLength && str32nequ (procName, callName, callLength)) {
			if (hasArguments && ! hasParameters)
				Melder_throw (U"Call to procedure \"", callName, U"\" has too many arguments.");
			if (hasParameters && ! hasArguments)
				Melder_throw (U"Call to procedure \"", callName, U"\" has too few arguments.");
			if (++ my callDepth > Interpreter_MAX_CALL_DEPTH)
				Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
			str32cpy (my procedureStackNames [my callDepth], callName);
			if (hasParameters) {
				bool parenthesisOrColonFound = ( *q == U'(' || *q == U':' );
				q ++;   // step over parenthesis or colon or first white space
				if (! parenthesisOrColonFound) {
					Melder_skipHorizontalSpace (& q);
					if (*q == U'(' || *q == U':')
						q ++;   // step over parenthesis or colon
				}
				++ p;   // first argument
				while (*q && *q != U')') {
					char32 *par, save;
					static MelderString arg;
					MelderString_empty (& arg);
					Melder_skipHorizontalSpace (& p);
					while (Melder_isHorizontalSpace (*q) || *q == U',' || *q == U')')
						q ++;
					par = q;
					while (*q != U'\0' && ! Melder_isHorizontalSpace (*q) && *q != U',' && *q != U')')
						q ++;   // collect parameter name
					if (*q) {   // does anything follow the parameter name?
						if (*p == U'\"') {
							p ++;   // skip initial quote
							while (*p != U'\0') {
								if (*p == U'\"') {   // quote signals end-of-string or string-internal quote
									if (p [1] == U'\"') {   // double quote signals string-internal quote
										MelderString_appendCharacter (& arg, U'\"');
										p += 2;   // skip second quote
									} else {   // single quote signals end-of-string
										break;
									}
								} else {
									MelderString_appendCharacter (& arg, *p ++);
								}
							}
						} else {
							while (*p != U'\0' && ! Melder_isHorizontalSpace (*p))
								MelderString_appendCharacter (& arg, *p ++);   // white space separates
						}
						if (*p) {
							*p = U'\0';
							p ++;
						}
					} else {   // else rest of line
						while (*p != '\0')
							MelderString_appendCharacter (& arg, *p ++);
					}
					if (q [-1] == '$') {
						save = *q; *q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
						var -> stringValue = Melder_dup_f (arg.string);
					} else {
						double value;
						my callDepth --;
						Interpreter_numericExpression (me, arg.string, & value);
						my callDepth ++;
						save = *q; *q = U'\0';
						InterpreterVariable var = Interpreter_lookUpVariable (me, par); *q = save;
						var -> numericValue = value;
					}
				}
			}
			if (callDepth == Interpreter_MAX_CALL_DEPTH)
				Melder_throw (U"Call depth greater than ", Interpreter_MAX_CALL_DEPTH, U".");
			callStack [++ callDepth] = lineNumber;
			lineNumber = iline;
			break;
		}
	}
	if (iline > lines.size)
		Melder_throw (U"Procedure \"", callName, U"\" not found.");
}

static void assignToNumericVectorElement (Interpreter me, char32 *& p, const char32* vectorName, MelderString& valueString) {
	integer indexValue = 0;
	static MelderString index;
	MelderString_empty (& index);
	int depth = 0;
	bool inString = false;
	while ((depth > 0 || *p != U']' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& index, *p);
		if (*p == U'[') {
			if (! inString)
				depth ++;
		} else if (*p == U']') {
			if (! inString)
				depth --;
		}
		if (*p == U'"')
			inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing closing bracket (]) in array element.");
	Formula_Result result;
	Interpreter_anyExpression (me, index.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		indexValue = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Element index should be numeric.");
	}
	p ++;   // step over closing bracket
	Melder_skipHorizontalSpace (& p);
	int assignmentType = 0;
	if (*p == U'=') {
		p ++;   // step over equals sign
	} else if (*p == U'+' && p [1] == U'=') {
		assignmentType = 1;
		p += 2;   // step over plus-gets sign
	} else if (*p == U'-' && p [1] == U'=') {
		assignmentType = 2;
		p += 2;   // step over minus-gets sign
	} else if (*p == U'*' && p [1] == U'=') {
		assignmentType = 3;
		p += 2;   // step over times-gets sign
	} else if (*p == U'/' && p [1] == U'=') {
		assignmentType = 4;
		p += 2;   // step over div-gets sign
	} else
		Melder_throw (U"Missing '=', '+=', '-=', '*=' or '/=' after vector element ", vectorName, U" [", index.string, U"].");
	Melder_skipHorizontalSpace (& p);   // go to first token after assignment
	if (*p == U'\0')
		Melder_throw (U"Missing expression after vector element ", vectorName, U" [", index.string, U"].");
	double value;
	if (isCommand (p)) {
		/*
			Get the value of the query.
		*/
		MelderString_empty (& valueString);
		autoMelderDivertInfo divert (& valueString);
		MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
		bool status = praat_executeCommand (me, p);
		if (! status) {
			value = undefined;
		} else if (my returnType == kInterpreter_ReturnType::OBJECT_) {
			integer IOBJECT, selectedObject = 0, numberOfSelectedObjects = 0;
			WHERE (SELECTED) {
				selectedObject = IOBJECT;
				numberOfSelectedObjects += 1;
			}
			if (numberOfSelectedObjects > 1)
				Melder_throw (U"Multiple objects selected. Cannot assign object IDs to vector element. "
						"Perhaps use a vector variable instead.");
			if (numberOfSelectedObjects == 0)
				Melder_throw (U"No objects selected. Cannot assign object ID to vector element.");
			value = theCurrentPraatObjects -> list [selectedObject]. id;
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
	} else {
		/*
			Get the value of the formula.
		*/
		Interpreter_numericExpression (me, p, & value);
	}
	InterpreterVariable var = Interpreter_hasVariable (me, vectorName);
	if (! var)
		Melder_throw (U"Vector ", vectorName, U" does not exist.");
	/*@praat
		asserterror Vector interpreter_assignToVector# does not exist.
		interpreter_assignToVector# [5] = 3
	@*/
	if (indexValue < 1)
		Melder_throw (U"A vector index cannot be less than 1 (the index you supplied is ", indexValue, U").");
	if (indexValue > var -> numericVectorValue.size)
		Melder_throw (U"A vector index cannot be greater than the number of elements (here ",
			var -> numericVectorValue.size, U"). The index you supplied is ", indexValue, U".");
	if (assignmentType == 0)
		var -> numericVectorValue [indexValue] = value;
	else if (assignmentType == 1)
		var -> numericVectorValue [indexValue] += value;
	else if (assignmentType == 2)
		var -> numericVectorValue [indexValue] -= value;
	else if (assignmentType == 3)
		var -> numericVectorValue [indexValue] *= value;
	else if (assignmentType == 4)
		var -> numericVectorValue [indexValue] /= value;
	else
		Melder_assert (false);
}

static void assignToNumericMatrixElement (Interpreter me, char32 *& p, const char32* matrixName, MelderString& valueString) {
	integer rowNumber = 0, columnNumber = 0;
	/*
		Get the row number.
	*/
	static MelderString rowFormula;
	MelderString_empty (& rowFormula);
	int depth = 0;
	bool inString = false;
	while ((depth > 0 || *p != U',' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& rowFormula, *p);
		if (*p == U'[' || *p == U'(') {
			if (! inString)
				depth ++;
		} else if (*p == U']' || *p == U')') {
			if (! inString)
				depth --;
		}
		if (*p == U'"') inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing comma in matrix indexing.");
	Formula_Result result;
	Interpreter_anyExpression (me, rowFormula.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		rowNumber = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Row number should be numeric.");
	}
	p ++;   // step over comma
	/*
		Get the column number.
	*/
	static MelderString columnFormula;
	MelderString_empty (& columnFormula);
	depth = 0;
	inString = false;
	while ((depth > 0 || *p != U']' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& columnFormula, *p);
		if (*p == U'[') {
			if (! inString)
				depth ++;
		} else if (*p == U']') {
			if (! inString)
				depth --;
		}
		if (*p == U'"')
			inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing closing bracket (]) in matrix indexing.");
	Interpreter_anyExpression (me, columnFormula.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		columnNumber = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Column number should be numeric.");
	}
	p ++;   // step over closing bracket
	Melder_skipHorizontalSpace (& p);
	int assignmentType = 0;
	if (*p == U'=') {
		p ++;   // step over equals sign
	} else if (*p == U'+' && p [1] == U'=') {
		assignmentType = 1;
		p += 2;   // step over plus-gets sign
	} else if (*p == U'-' && p [1] == U'=') {
		assignmentType = 2;
		p += 2;   // step over minus-gets sign
	} else if (*p == U'*' && p [1] == U'=') {
		assignmentType = 3;
		p += 2;   // step over times-gets sign
	} else if (*p == U'/' && p [1] == U'=') {
		assignmentType = 4;
		p += 2;   // step over div-gets sign
	} else
		Melder_throw (U"Missing '=', '+=', '-=', '*=' or '/=' after matrix element ", matrixName, U" [",
			rowFormula.string, U",", columnFormula.string, U"].");
	Melder_skipHorizontalSpace (&p);   // go to first token after assignment
	if (*p == U'\0')
		Melder_throw (U"Missing expression after matrix element ", matrixName, U" [",
				rowFormula.string, U",", columnFormula.string, U"].");
	double value;
	if (isCommand (p)) {
		/*
			Get the value of the query.
		*/
		MelderString_empty (& valueString);
		autoMelderDivertInfo divert (& valueString);
		MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
		bool status = praat_executeCommand (me, p);
		if (! status) {
			value = undefined;
		} else if (my returnType == kInterpreter_ReturnType::OBJECT_) {
			integer IOBJECT, selectedObject = 0, numberOfSelectedObjects = 0;
			WHERE (SELECTED) {
				selectedObject = IOBJECT;
				numberOfSelectedObjects += 1;
			}
			if (numberOfSelectedObjects > 1)
				Melder_throw (U"Multiple objects selected. Cannot assign object IDs to matrix element. "
						"Perhaps use a vector variable instead.");
			if (numberOfSelectedObjects == 0)
				Melder_throw (U"No objects selected. Cannot assign object ID to matrix element.");
			value = theCurrentPraatObjects -> list [selectedObject]. id;
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
	} else {
		/*
			Get the value of the formula.
		*/
		Interpreter_numericExpression (me, p, & value);
	}
	InterpreterVariable var = Interpreter_hasVariable (me, matrixName);
	if (! var)
		Melder_throw (U"Matrix ", matrixName, U" does not exist.");
	if (rowNumber < 1)
		Melder_throw (U"A row number cannot be less than 1 (the row number you supplied is ", rowNumber, U").");
	if (rowNumber > var -> numericMatrixValue. nrow)
		Melder_throw (U"A row number cannot be greater than the number of rows (here ",
			var -> numericMatrixValue. nrow, U"). The row number you supplied is ", rowNumber, U".");
	if (columnNumber < 1)
		Melder_throw (U"A column number cannot be less than 1 (the column number you supplied is ", columnNumber, U").");
	if (columnNumber > var -> numericMatrixValue. ncol)
		Melder_throw (U"A column number cannot be greater than the number of columns (here ",
			var -> numericMatrixValue. ncol, U"). The column number you supplied is ", columnNumber, U".");
	if (assignmentType == 0)
		var -> numericMatrixValue [rowNumber] [columnNumber] = value;
	else if (assignmentType == 1)
		var -> numericMatrixValue [rowNumber] [columnNumber] += value;
	else if (assignmentType == 2)
		var -> numericMatrixValue [rowNumber] [columnNumber] -= value;
	else if (assignmentType == 3)
		var -> numericMatrixValue [rowNumber] [columnNumber] *= value;
	else if (assignmentType == 4)
		var -> numericMatrixValue [rowNumber] [columnNumber] /= value;
	else
		Melder_assert (false);
}

static void assignToStringArrayElement (Interpreter me, char32 *& p, const char32* vectorName, MelderString& valueString) {
	integer indexValue = 0;
	static MelderString index;
	MelderString_empty (& index);
	int depth = 0;
	bool inString = false;
	while ((depth > 0 || *p != U']' || inString) && Melder_staysWithinLine (*p)) {
		MelderString_appendCharacter (& index, *p);
		if (*p == U'[') {
			if (! inString)
				depth ++;
		} else if (*p == U']') {
			if (! inString)
				depth --;
		}
		if (*p == U'"') inString = ! inString;
		p ++;
	}
	if (! Melder_staysWithinLine (*p))
		Melder_throw (U"Missing closing bracket (]) in array element.");
	Formula_Result result;
	Interpreter_anyExpression (me, index.string, & result);
	if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
		indexValue = Melder_iround (result. numericResult);
	} else {
		Melder_throw (U"Element index should be numeric.");
	}
	p ++;   // step over closing bracket
	Melder_skipHorizontalSpace (& p);
	if (*p != U'=')
		Melder_throw (U"Missing '=' after string vector element ", vectorName, U" [", index.string, U"].");
	p ++;   // step over equals sign
	Melder_skipHorizontalSpace (& p);   // go to first token after assignment
	if (*p == U'\0')
		Melder_throw (U"Missing expression after string vector element ", vectorName, U" [", index.string, U"].");
	autostring32 value;
	if (isCommand (p)) {
		/*
			Get the value of the query.
		*/
		MelderString_empty (& valueString);
		autoMelderDivertInfo divert (& valueString);
		bool status = praat_executeCommand (me, p);
		if (! status)
			value = autostring32();
		else
			value = Melder_dup (valueString.string);
	} else {
		/*
			Get the value of the formula.
		*/
		value = Interpreter_stringExpression (me, p);
	}
	InterpreterVariable var = Interpreter_hasVariable (me, vectorName);
	if (! var)
		Melder_throw (U"String vector ", vectorName, U" does not exist.");
	if (indexValue < 1)
		Melder_throw (U"A vector index cannot be less than 1 (the index you supplied is ", indexValue, U").");
	if (indexValue > var -> stringArrayValue.size)
		Melder_throw (U"A vector index cannot be greater than the number of elements (here ",
			var -> stringArrayValue.size, U"). The index you supplied is ", indexValue, U".");
	var -> stringArrayValue [indexValue] = value. move();
}

static void private_Interpreter_initialize (Interpreter me, autostring32 text, const bool reuseVariables) {
	if (text)
		my text = text.move();   // otherwise, use my existing text
	char32 *command = my text.get();
	autoMelderString command2;
	integer numberOfLines = 0;
	bool atLastLine = false;
	int chopped = 0;
	/*
		Count lines and set the newlines to zero.
	*/
	while (! atLastLine) {
		char32 *endOfLine = command;
		while (Melder_staysWithinLine (*endOfLine))
			endOfLine ++;
		if (*endOfLine == U'\0')
			atLastLine = true;
		*endOfLine = U'\0';
		numberOfLines ++;
		command = endOfLine + 1;
	}
	/*
		Remember line starts, labels and procedures.
	*/
	my lines. resize (numberOfLines);
	command = my text.get();   // reset
	my labelNames. reset();
	my labelLines. reset();
	my procedureNames. reset();
	my procedureStartLines. reset();
	for (integer lineNumber = 1; lineNumber <= numberOfLines; lineNumber ++, command += Melder_length (command) + 1 + chopped) {
		Melder_skipHorizontalSpace (& command);   // nbsp can occur for scripts copied from the manual
		/*
			Chop trailing spaces?
		*/
		#if 0
			chopped = 0;
			const integer length = Melder_length (command);
			while (length > 0) {
				char kar = command [-- length];
				if (! Melder_isHorizontalSpace (kar))
					break;
				command [length] = U'\0';
				chopped ++;
			}
		#endif
		my lines [lineNumber] = command;
		if (str32nequ (command, U"label", 5) && Melder_isHorizontalSpace (command [5])) {
			MelderString_empty (& command2);
			for (integer ichar = 6; ; ichar ++) {
				if (Melder_isHorizontalSpace (command [ichar]) || command [ichar] == U';' || command [ichar] == U'\0')
					break;
				MelderString_appendCharacter (& command2, command [ichar]);
			}
			my labelNames. insert (0, command2.string);
			my labelLines. insert (0, lineNumber);
		}
		if (str32nequ (command, U"procedure", 9) && Melder_isHorizontalSpace (command [9])) {
			MelderString_empty (& command2);
			for (integer ichar = 10; ; ichar ++) {
				if (Melder_isHorizontalSpace (command [ichar]) || command [ichar] == U';' || command [ichar] == U':' || command [ichar] == U'\0')
					break;
				MelderString_appendCharacter (& command2, command [ichar]);
			}
			my procedureNames. insert (0, command2.string);
			my procedureStartLines. insert (0, lineNumber);
		}
		if (str32nequ (command, U"proc", 4) && Melder_isHorizontalSpace (command [4])) {
			MelderString_empty (& command2);
			for (integer ichar = 5; ; ichar ++) {
				if (Melder_isHorizontalSpace (command [ichar]) || command [ichar] == U';' || command [ichar] == U':' || command [ichar] == U'\0')
					break;
				MelderString_appendCharacter (& command2, command [ichar]);
			}
			my procedureNames. insert (0, command2.string);
			my procedureStartLines. insert (0, lineNumber);
		}
	}
	NUMsortTogether (my labelNames.get(), my labelLines.get());
	for (integer ilabel = 1; ilabel < my labelNames.size; ilabel ++)
		if (str32equ (my labelNames [ilabel].get(), my labelNames [ilabel + 1].get()))
			Melder_throw (U"Duplicate label \"", my labelNames [ilabel].get(),
					U"\" on lines ", my labelLines [ilabel], U" and ", my labelLines [ilabel + 1], U".");
	NUMsortTogether (my procedureNames.get(), my procedureStartLines.get());
	for (integer iproc = 1; iproc < my procedureNames.size; iproc ++)
		if (str32equ (my procedureNames [iproc].get(), my procedureNames [iproc + 1].get()))
			Melder_warning_once (
				U"Duplicate procedure \"", my procedureNames [iproc].get(),
				U"\" on lines ", my procedureStartLines [iproc], U" and ", my procedureStartLines [iproc + 1],
				U". This is probably a bug, for which you may want to contact the author of the script. "
				U"The script will run, but it is unpredictable which of the two procedure definitions will be chosen."
			);   // TURNED INTO WARNING 2026, SCHEDULED FOR ERROR 2036
	/*
		Connect continuation lines.
	*/
	trace (U"connect continuation lines");
	for (integer lineNumber = numberOfLines; lineNumber >= 2; lineNumber --) {
		char32 *line = my lines [lineNumber];
		if (line [0] == U'.' && line [1] == U'.' && line [2] == U'.') {
			char32 *previous = my lines [lineNumber - 1];
			MelderString_copy (& command2, line + 3);
			MelderString_get (& command2, previous + Melder_length (previous));
			static char32 emptyLine [] = { U'\0' };
			my lines [lineNumber] = emptyLine;
		}
	}
	/*
		Copy the parameter names and argument values into the array of variables.
	*/
	if (! reuseVariables) {
		my variablesMap. clear ();
		for (int ipar = 1; ipar <= my numberOfParameters; ipar ++) {
			char32 parameter [1+Interpreter_MAX_PARAMETER_LENGTH];
			/*
				Create variable names as-are and variable names without capitals.
			*/
			str32cpy (parameter, my parameters [ipar]);
			parameterToVariable (me, my types [ipar], parameter, ipar);   // deprecated 2014? probably works only with parameter substitution
			if (parameter [0] >= U'A' && parameter [0] <= U'Z') {
				parameter [0] = Melder_toLowerCase (parameter [0]);
				parameterToVariable (me, my types [ipar], parameter, ipar);
			}
		}
		/*
			Initialize some variables.

			TODO: turn newline$ and tab$ into constants
			Time path:
				1. in 20xx, start asking for trust when assigning to these variables
				2. ten years later, assigning to these variables becomes an error (with an appropriate warning and recipe for correction),
				   just as with `pi`, `undefined` and `e`.

			TODO: turn the remainder of these variables into functions
			Time path:
				1. in 20xx, add functions to the side
				2. ten years later, start warning against undefined use of these variables
				3. another ten years later, undefined us of these variables becomes an error (with an appropriate warning and recipe for correction)
		*/
		Interpreter_addStringVariable (me, U"newline$", U"\n");
		Interpreter_addStringVariable (me, U"tab$", U"\t");
		Interpreter_addStringVariable (me, U"shellDirectory$", Melder_getShellDirectory ());
		{// scope
			structMelderFolder folder { };
			Melder_getCurrentFolder (& folder);
			Interpreter_addStringVariable (me, U"defaultDirectory$", MelderFolder_peekPath (& folder));
			Interpreter_addStringVariable (me, U"preferencesDirectory$", MelderFolder_peekPath (Melder_preferencesFolder()));   // settingsFolder$()
			Melder_getHomeDir (& folder);
			Interpreter_addStringVariable (me, U"homeDirectory$", MelderFolder_peekPath (& folder));
			Melder_getTempDir (& folder);
			Interpreter_addStringVariable (me, U"temporaryDirectory$", MelderFolder_peekPath (& folder));
		}
		#if defined (macintosh)
			Interpreter_addNumericVariable (me, U"macintosh", 1);
			Interpreter_addNumericVariable (me, U"windows", 0);
			Interpreter_addNumericVariable (me, U"unix", 0);
		#elif defined (_WIN32)
			Interpreter_addNumericVariable (me, U"macintosh", 0);
			Interpreter_addNumericVariable (me, U"windows", 1);
			Interpreter_addNumericVariable (me, U"unix", 0);
		#elif defined (UNIX)
			Interpreter_addNumericVariable (me, U"macintosh", 0);
			Interpreter_addNumericVariable (me, U"windows", 0);
			Interpreter_addNumericVariable (me, U"unix", 1);
		#else
			Interpreter_addNumericVariable (me, U"macintosh", 0);
			Interpreter_addNumericVariable (me, U"windows", 0);
			Interpreter_addNumericVariable (me, U"unix", 0);
		#endif
		#if defined (__aarch64__) || defined (_M_ARM64_)
			Interpreter_addNumericVariable (me, U"praat_intel32", 0);
			Interpreter_addNumericVariable (me, U"praat_intel64", 0);
			Interpreter_addNumericVariable (me, U"praat_arm64", 1);
			Interpreter_addNumericVariable (me, U"praat_s390x", 0);
			Interpreter_addNumericVariable (me, U"praat_armv7", 0);
		#elif defined (__s390x__)
			Interpreter_addNumericVariable (me, U"praat_intel32", 0);
			Interpreter_addNumericVariable (me, U"praat_intel64", 0);
			Interpreter_addNumericVariable (me, U"praat_arm64", 0);
			Interpreter_addNumericVariable (me, U"praat_s390x", 1);
			Interpreter_addNumericVariable (me, U"praat_armv7", 0);
		#elif defined (raspberrypi)
			Interpreter_addNumericVariable (me, U"praat_intel32", 0);
			Interpreter_addNumericVariable (me, U"praat_intel64", 0);
			Interpreter_addNumericVariable (me, U"praat_arm64", 0);
			Interpreter_addNumericVariable (me, U"praat_s390x", 0);
			Interpreter_addNumericVariable (me, U"praat_armv7", 1);
		#else
			Interpreter_addNumericVariable (me, U"praat_intel32", sizeof (void *) == 4);
			Interpreter_addNumericVariable (me, U"praat_intel64", sizeof (void *) == 8);
			Interpreter_addNumericVariable (me, U"praat_arm64", 0);
			Interpreter_addNumericVariable (me, U"praat_s390x", 0);
			Interpreter_addNumericVariable (me, U"praat_armv7", 0);
		#endif
		Interpreter_addNumericVariable (me, U"praat_32bit", sizeof (void *) == 4);
		Interpreter_addNumericVariable (me, U"praat_64bit", sizeof (void *) == 8);
		Interpreter_addNumericVariable (me, U"left", 1);   // deprecated 2010 (Praat 5.2.06)
		Interpreter_addNumericVariable (me, U"right", 2);   // deprecated 2010 (Praat 5.2.06)
		Interpreter_addNumericVariable (me, U"mono", 1);   // deprecated 2010 (Praat 5.2.06)
		Interpreter_addNumericVariable (me, U"stereo", 2);   // deprecated 2010 (Praat 5.2.06)
		Interpreter_addNumericVariable (me, U"all", 0);   // deprecated 2010 (Praat 5.2.06)
		Interpreter_addNumericVariable (me, U"average", 0);   // deprecated 2010 (Praat 5.2.06)
		Interpreter_addStringVariable (me, U"praatVersion$", Melder_appVersionSTR());
		Interpreter_addNumericVariable (me, U"praatVersion", Melder_appVersion());
	}
}

void Interpreter_run (Interpreter me, autostring32 text, const bool reuseVariables) {
	try {
		//TRACE
		trace (U"enter");
		private_Interpreter_initialize (me, text.move(), reuseVariables);
		my lineNumber = 1;
		my callDepth = 0;
		my fromif = false;
		my fromendfor = false;
		my assertErrorLineNumber = 0;
		my setDynamicFromOwningEditorEnvironment ();
		Interpreter_resume (me);
	} catch (MelderError) {
		throw;
	}
}

void Interpreter_resume (Interpreter me) {
	/*
		Start.
	*/
	my running = true;
	my stopped = false;
	my isHalted = false;
	bool assertionFailed = false;
	//TRACE
	trace (U"resuming at line ", my lineNumber, U" in file ", & my file);
	try {
		static MelderString valueString;   // to divert the info
		autoMelderString command2;
		autoMelderString buffer;
		int callDepth = my callDepth;
		const integer numberOfLines = my lines.size;
		/*
			Execute commands.
		*/
		trace (U"going to handle ", numberOfLines, U" lines");
		//for (integer lineNumber = 1; lineNumber <= numberOfLines; lineNumber ++) {
			//trace (U"line ", lineNumber, U": ", lines [lineNumber]);
		//}
		for (; my lineNumber <= numberOfLines; my lineNumber ++) {
			trace (U"considering to handle line ", my lineNumber, U": ", my lines [my lineNumber], U", stopped ", my stopped, U", halted ", my isHalted, U", pass ", 1 + my isInSecondPass);
			if (my stopped)
				break;
			if (my isHalted)
				break;
			trace (U"going to handle line ", my lineNumber, U": ", my lines [my lineNumber]);
			constvector <mutablestring32> lines = my lines.get();
			try {
				char32 c0;
				bool fail = false;
				MelderString_copy (& command2, lines [my lineNumber]);
				c0 = command2.string [0];
				if (c0 == U'\0')
					continue;
				/*
					Substitute variables.
				*/
				trace (U"preprocessing line ", my lineNumber, U": ", command2.string);
				for (char32 *p = & command2.string [0]; *p != U'\0'; p ++) if (*p == U'\'') {
					/*
						Found a left quote. Search for a matching right quote.
					*/
					char32 *q = p + 1, varName [300], *r, *s;
					integer precision = -1;
					bool percent = false;
					while (*q != U'\0' && *q != U'\'' && q - p < 299)
						q ++;
					if (*q == U'\0')
						break;   // no matching right quote? done with this line!
					if (q - p == 1 || q - p >= 299)
						continue;   // ignore empty and too long variable names
					trace (U"found ", q - p - 1);
					/*
						Found a right quote. Get potential variable name.
					*/
					for (r = p + 1, s = varName; q - r > 0; r ++, s ++)
						*s = *r;
					*s = U'\0';   // trailing null byte
					char32 *colon = str32chr (varName, U':');
					if (colon) {
						precision = Melder_atoi (colon + 1);
						if (str32chr (colon + 1, U'%'))
							percent = true;
						*colon = U'\0';
					}
					InterpreterVariable var = Interpreter_hasVariable (me, varName);
					if (var) {
						/*
							Found a variable (p points to the left quote, q to the right quote). Substitute.
						*/
						const integer headlen = p - command2.string;
						conststring32 string = ( var -> stringValue ? var -> stringValue.get() :
								percent ? Melder_percent (var -> numericValue, precision) :
								precision >= 0 ?  Melder_fixed (var -> numericValue, precision) :
								Melder_double (var -> numericValue) );
						const integer arglen = Melder_length (string);
						MelderString_ncopy (& buffer, command2.string, headlen);
						MelderString_append (& buffer, string, q + 1);
						MelderString_copy (& command2, buffer.string);   // This invalidates p!! (really bad bug 20070203)
						p = command2.string + headlen + arglen - 1;
					} else {
						p = q - 1;   // go to before next quote
					}
				}
				trace (U"going to parse line ", my lineNumber, U": ", command2.string);
				c0 = command2.string [0];   // resume in order to allow things like 'c$' = 5
				if ((! Melder_isLetter (c0) || Melder_isUpperCaseLetter (c0)) && c0 != U'@' &&
						! (c0 == U'.' && Melder_isLetter (command2.string [1]) && ! Melder_isUpperCaseLetter (command2.string [1])))
				{
					(void) praat_executeCommand (me, command2.string);
				/*
					Interpret control flow and variables.
				*/
				} else switch (c0) {
					case U'.':
						fail = true;
						break;
					case U'@':
						Interpreter_do_procedureCall (me, command2.string + 1, lines, my lineNumber, my callStack, callDepth);
						break;
					case U'a':
						if (str32nequ (command2.string, U"assert ", 7)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 7, & value);
							if (value == 0.0 || isundef (value)) {
								assertionFailed = true;
								Melder_throw (U"Script assertion fails in line ", my lineNumber,
										U" (", value == 0.0 ? U"false" : U"undefined", U"):\n   ", command2.string + 7);
							}
						} else if (str32nequ (command2.string, U"asserterror ", 12)) {
							MelderString_copy (& my assertErrorString, command2.string + 12);
							//TRACE
							trace (U"assert error string: <<", my assertErrorString.string, U">>");
							my assertErrorLineNumber = my lineNumber;
						} else
							fail = true;
						break;
					case U'b':
						fail = true;
						break;
					case U'c':
						if (str32nequ (command2.string, U"call ", 5)) {
							Interpreter_do_oldProcedureCall (me, command2.string + 5, lines, my lineNumber, my callStack, callDepth);
						} else
							fail = true;
						break;
					case U'd':
						if (str32nequ (command2.string, U"dec ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue -= 1.0;
						} else
							fail = true;
						break;
					case U'e':
						if (command2.string [1] == U'n' && command2.string [2] == U'd') {
							if (str32nequ (command2.string, U"endif", 5) &&
									(! Melder_staysWithinInk (command2.string [5]) || command2.string [5] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 5);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endif'.");
								/* Ignore. */
							} else if (str32nequ (command2.string, U"endfor", 6) &&
									(! Melder_staysWithinInk (command2.string [6]) || command2.string [6] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 6);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endfor'.");
								int depth = 0;
								integer iline;
								for (iline = my lineNumber - 1; iline > 0; iline --) {
									char32 *line = lines [iline];
									if (line [0] == U'f' && line [1] == U'o' && line [2] == U'r' && line [3] == U' ') {
										if (depth == 0) { my lineNumber = iline - 1; my fromendfor = true; break; }   // go before 'for'
										else depth --;
									} else if (str32nequ (lines [iline], U"endfor", 6) &&
											(! Melder_staysWithinInk (lines [iline] [6]) || lines [iline] [6] == U';'))
									{
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'endfor'.");
							} else if (str32nequ (command2.string, U"endwhile", 8) &&
									(! Melder_staysWithinInk (command2.string [8]) || command2.string [8] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 8);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endwhile'.");
								int depth = 0;
								integer iline;
								for (iline = my lineNumber - 1; iline > 0; iline --) {
									if (str32nequ (lines [iline], U"while ", 6)) {
										if (depth == 0) {
											my lineNumber = iline - 1;
											break;   // go before 'while'
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"endwhile", 8) &&
											(! Melder_staysWithinInk (lines [iline] [8]) || lines [iline] [8] == U';'))
									{
										depth ++;
									}
								}
								if (iline <= 0) Melder_throw (U"Unmatched 'endwhile'.");
							} else if (str32nequ (command2.string, U"endproc", 7) &&
									(! Melder_staysWithinInk (command2.string [7]) || command2.string [7] == U';'))
							{
								const char32 *startOfInk = Melder_findInk (command2.string + 7);
								if (startOfInk && *startOfInk != U';')
									Melder_throw (U"Stray text after 'endproc'.");
								if (callDepth == 0)
									Melder_throw (U"Unmatched 'endproc'.");
								my lineNumber = my callStack [callDepth --];
								-- my callDepth;
							} else fail = true;
						} else if (str32nequ (command2.string, U"else", 4) &&
								(! Melder_staysWithinInk (command2.string [4]) || command2.string [4] == U';'))
						{
							const char32 *startOfInk = Melder_findInk (command2.string + 4);
							if (startOfInk && *startOfInk != U';')
								Melder_throw (U"Stray text after 'else'.");
							int depth = 0;
							integer iline;
							for (iline = my lineNumber + 1; iline <= numberOfLines; iline ++) {
								if (str32nequ (lines [iline], U"endif", 5) &&
										(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
								{
									startOfInk = Melder_findInk (lines [iline] + 5);
									if (startOfInk && *startOfInk != U';') {
										my lineNumber = iline;   // on behalf of the error message
										Melder_throw (U"Stray text after 'endif'.");
									}
									if (depth == 0) { my lineNumber = iline; break; }   // go after `endif`
									else depth --;
								} else if (str32nequ (lines [iline], U"if ", 3)) {
									depth ++;
								}
							}
							if (iline > numberOfLines)
								Melder_throw (U"Unmatched 'else'.");
						} else if (str32nequ (command2.string, U"elsif ", 6) || str32nequ (command2.string, U"elif ", 5)) {
							if (my fromif) {
								double value;
								my fromif = false;
								Interpreter_numericExpression (me, command2.string + 5, & value);
								if (value == 0.0) {
									int depth = 0;
									integer iline;
									for (iline = my lineNumber + 1; iline <= numberOfLines; iline ++) {
										if (str32nequ (lines [iline], U"endif", 5) &&
												(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
										{
											const char32 *startOfInk = Melder_findInk (lines [iline] + 5);
											if (startOfInk && *startOfInk != U';') {
												my lineNumber = iline;   // on behalf of error message
												Melder_throw (U"Stray text after 'endif'.");
											}
											if (depth == 0) {
												my lineNumber = iline;
												break;   // go after `endif`
											} else
												depth --;
										} else if (str32nequ (lines [iline], U"else", 4) &&
												(! Melder_staysWithinInk (lines [iline] [4]) || lines [iline] [4] == U';'))
										{
											const char32 *startOfInk = Melder_findInk (lines [iline] + 4);
											if (startOfInk && *startOfInk != U';') {
												my lineNumber = iline;   // on behalf of error message
												Melder_throw (U"Stray text after 'else'.");
											}
											if (depth == 0) {
												my lineNumber = iline;
												break;   // go after `else`
											}
										} else if ((str32nequ (lines [iline], U"elsif", 5) && ! Melder_staysWithinInk (lines [iline] [5]))
											|| (str32nequ (lines [iline], U"elif", 4) && ! Melder_staysWithinInk (lines [iline] [4]))) {
											if (depth == 0) {
												my lineNumber = iline - 1;
												my fromif = true;
												break;   // go at next 'elsif' or 'elif'
											}
										} else if (str32nequ (lines [iline], U"if ", 3)) {
											depth ++;
										}
									}
									if (iline > numberOfLines)
										Melder_throw (U"Unmatched 'elsif'.");
								}
							} else {
								int depth = 0;
								integer iline;
								for (iline = my lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endif", 5) &&
											(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 5);
										if (startOfInk && *startOfInk != U';') {
											my lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'endif'.");
										}
										if (depth == 0) {
											my lineNumber = iline;
											break;   // go after `endif`
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"'elsif' not matched with 'endif'.");
							}
						} else if (str32nequ (command2.string, U"exit", 4)) {
							if (command2.string [4] == U'\0') {
								my lineNumber = numberOfLines;   // go after end
							} else if (command2.string [4] == U' ') {
								Melder_throw (command2.string + 5);
							} else
								fail = true;
						} else if (str32nequ (command2.string, U"echo ", 5)) {
							/*
								Make sure that lines like "echo = 3" will not be regarded as assignments.
							*/
							(void) praat_executeCommand (me, command2.string);
						} else
							fail = true;
						break;
					case U'f':
						if (command2.string [1] == U'o' && command2.string [2] == U'r' && command2.string [3] == U' ') {   // for_
							double toValue, loopVariable;
							char32 *frompos = str32str (command2.string, U" from "), *topos = str32str (command2.string, U" to ");
							char32 *varpos = command2.string + 4, *endvar = frompos;
							if (! topos)
								Melder_throw (U"Missing \'to\' in \'for\' loop.");
							if (! endvar)
								endvar = topos;
							while (*endvar == U' ') {
								*endvar = U'\0';
								endvar --;
							}
							while (*varpos == U' ')
								varpos ++;
							if (endvar - varpos < 0)
								Melder_throw (U"Missing loop variable after \'for\'.");
							InterpreterVariable var = Interpreter_lookUpVariable (me, varpos);
							Interpreter_numericExpression (me, topos + 4, & toValue);
							if (my fromendfor) {
								my fromendfor = false;
								loopVariable = var -> numericValue + 1.0;
							} else if (frompos) {
								*topos = U'\0';
								Interpreter_numericExpression (me, frompos + 6, & loopVariable);
							} else {
								loopVariable = 1.0;
							}
							var -> numericValue = loopVariable;
							if (loopVariable > toValue) {
								int depth = 0;
								integer iline;
								for (iline = my lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endfor", 6) &&
											(! Melder_staysWithinInk (lines [iline] [6]) || lines [iline] [6] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 6);
										if (startOfInk && *startOfInk != U';') {
											my lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'endfor'.");
										}
										if (depth == 0) {
											my lineNumber = iline;
											break;   // go after 'endfor'
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"for ", 4)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"Unmatched 'for' (matching 'endfor' not found).");
							}
						} else if (str32nequ (command2.string, U"form", 4) &&
								(command2.string [4] == U':' || Melder_isEndOfInk (command2.string [4])))
						{
							/*
								We encountered a `form` statement.
								The contents of everything between `form` and `endform`
								is irrelevant at this point (the parameters have been read earlier),
								so we will skip the whole contents of the form and jump past `endform`.
							*/
							integer iline;
							for (iline = my lineNumber + 1; iline <= numberOfLines; iline ++)
								if (str32nequ (lines [iline], U"endform", 7) && Melder_isEndOfInk (lines [iline] [7])) {
									my lineNumber = iline;
									break;   // go after 'endform'
								}
							if (iline > numberOfLines)
								/*
									We will probably never arrive here, because a missing `endform`
									will probably have been detected by Interpreter_readParameters(),
									either with "Unknown parameter type inside form:\nxxx"
									or with "Unfinished form (missing “endform”)."
									but we like to stay on the safe side. (last checked 2023-01-20)
								*/
								Melder_throw (U"Unmatched 'form' (matching 'endform' not found).");
						} else
							fail = true;
						break;
					case U'g':
						if (str32nequ (command2.string, U"goto ", 5)) {
							char32 labelName [1+Interpreter_MAX_LABEL_LENGTH];
							str32ncpy (labelName, command2.string + 5, Interpreter_MAX_LABEL_LENGTH);
							labelName [Interpreter_MAX_LABEL_LENGTH] = U'\0';
							char32 *space = str32chr (labelName, U' ');
							if (space == labelName)
								Melder_throw (U"Missing label name after 'goto'.");
							bool dojump = true;
							if (space) {
								double value;
								*space = '\0';
								Interpreter_numericExpression (me, command2.string + 6 + Melder_length (labelName), & value);
								if (value == 0.0)
									dojump = false;
							}
							if (dojump) {
								integer ilabel = lookupLabel (me, labelName);
								my lineNumber = my labelLines [ilabel];   // loop will add 1
							}
						} else
							fail = true;
						break;
					case U'h':
						fail = true;
						break;
					case U'i':
						if (command2.string [1] == U'f' && Melder_isHorizontalSpace (command2.string [2])) {   // if_
							double value;
							Interpreter_numericExpression (me, command2.string + 3, & value);
							if (value == 0.0) {
								int depth = 0;
								integer iline;
								for (iline = my lineNumber + 1; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endif", 5) &&
											(! Melder_staysWithinInk (lines [iline] [5]) || lines [iline] [5] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 5);
										if (startOfInk && *startOfInk != U';') {
											my lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'endif'.");
										}
										if (depth == 0) {
											my lineNumber = iline;
											break;   // go after 'endif'
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"else", 4) &&
											(! Melder_staysWithinInk (lines [iline] [4]) || lines [iline] [4] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 4);
										if (startOfInk && *startOfInk != U';') {
											my lineNumber = iline;   // on behalf of error message
											Melder_throw (U"Stray text after 'else'.");
										}
										if (depth == 0) {
											my lineNumber = iline;
											break;   // go after 'else'
										}
									} else if (str32nequ (lines [iline], U"elsif ", 6) || str32nequ (lines [iline], U"elif ", 5)) {
										if (depth == 0) {
											my lineNumber = iline - 1;
											my fromif = true;
											break;   // go at 'elsif'
										}
									} else if (str32nequ (lines [iline], U"if ", 3)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"Unmatched 'if'.");
							} else if (isundef (value)) {
								Melder_throw (U"The value of the 'if' condition is undefined.");
							}
						} else if (str32nequ (command2.string, U"inc ", 4)) {
							InterpreterVariable var = Interpreter_lookUpVariable (me, command2.string + 4);
							var -> numericValue += 1.0;
						} else
							fail = true;
						break;
					case U'j':
						fail = true;
						break;
					case U'k':
						fail = true;
						break;
					case U'l':
						if (str32nequ (command2.string, U"label ", 6)) {
							if (command2.string [6] == U'=')
								Melder_throw (U"\"label\" cannot be used as a variable name.");
							;   // ignore labels
						} else
							fail = true;
						break;
					case U'm':
						fail = true;
						break;
					case U'n':
						fail = true;
						break;
					case U'o':
						fail = true;
						break;
					case U'p':
						if (str32nequ (command2.string, U"procedure", 9) && Melder_isHorizontalSpace (command2.string [9])) {
							integer iline = my lineNumber + 1;
							for (; iline <= numberOfLines; iline ++) {
								if (str32nequ (lines [iline], U"endproc", 7) &&
										(! Melder_staysWithinInk (lines [iline] [7]) || lines [iline] [7] == U';'))
								{
									const char32 *startOfInk = Melder_findInk (lines [iline] + 7);
									if (startOfInk && *startOfInk != U';') {
										my lineNumber = iline;   // on behalf of error message
										Melder_throw (U"Stray text after 'endproc'.");
									}
									my lineNumber = iline;
									break;
								}   // go after `endproc`
							}
							if (iline > numberOfLines) Melder_throw (U"Unmatched 'procedure'.");
						} else if (str32nequ (command2.string, U"print", 5)) {
							/*
							 * Make sure that lines like "print = 3" will not be regarded as assignments.
							 */
							if (command2.string [5] == U' ' || (str32nequ (command2.string + 5, U"line", 4) && (command2.string [9] == U' ' || command2.string [9] == U'\0'))) {
								(void) praat_executeCommand (me, command2.string);
							} else
								fail = true;
						} else
							fail = true;
						break;
					case U'q':
						fail = true;
						break;
					case U'r':
						if (str32nequ (command2.string, U"repeat", 6) &&
								(! Melder_staysWithinInk (command2.string [6]) || command2.string [6] == U';'))
						{
							const char32 *startOfInk = Melder_findInk (command2.string + 6);
							if (startOfInk && *startOfInk != U';')
								Melder_throw (U"Stray text after 'repeat'.");
							/* Ignore. */
						} else
							fail = true;
						break;
					case U's':
						if (str32nequ (command2.string, U"stopwatch", 9) && ! Melder_staysWithinInk (command2.string [9])) {
							(void) Melder_stopwatch ();   // reset stopwatch
						} else
							fail = true;
						break;
					case U't':
						fail = true;
						break;
					case U'u':
						if (str32nequ (command2.string, U"until ", 6)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 6, & value);
							if (value == 0.0) {
								int depth = 0;
								integer iline = my lineNumber - 1;
								for (; iline > 0; iline --) {
									if (str32nequ (lines [iline], U"repeat", 6) &&
											(! Melder_staysWithinInk (lines [iline] [6]) || lines [iline] [6] == U';'))
									{
										if (depth == 0) {
											my lineNumber = iline;
											break;   // go after `repeat`
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"until ", 6)) {
										depth ++;
									}
								}
								if (iline <= 0)
									Melder_throw (U"Unmatched 'until'.");
							}
						} else
							fail = true;
						break;
					case U'v':
						fail = true;
						break;
					case U'w':
						if (str32nequ (command2.string, U"while ", 6)) {
							double value;
							Interpreter_numericExpression (me, command2.string + 6, & value);
							if (value == 0.0) {
								int depth = 0;
								integer iline = my lineNumber + 1;
								for (; iline <= numberOfLines; iline ++) {
									if (str32nequ (lines [iline], U"endwhile", 8) &&
											(! Melder_staysWithinInk (lines [iline] [8]) || lines [iline] [8] == U';'))
									{
										const char32 *startOfInk = Melder_findInk (lines [iline] + 8);
										if (startOfInk && *startOfInk != U';') {
											my lineNumber = iline;
											Melder_throw (U"Stray text after 'endwhile'.");
										}
										if (depth == 0) {
											my lineNumber = iline;
											break;   // go after `endwhile`
										} else
											depth --;
									} else if (str32nequ (lines [iline], U"while ", 6)) {
										depth ++;
									}
								}
								if (iline > numberOfLines)
									Melder_throw (U"Unmatched 'while'.");
							}
						} else
							fail = true;
						break;
					case U'x':
						fail = true;
						break;
					case U'y':
						fail = true;
						break;
					case U'z':
						fail = true;
						break;
					default:
						fail = true;
						break;
				}
				if (fail) {
					/*
						Found an unknown word starting with a nonupper-case letter, optionally preceded by a period.
						See whether the word is a variable name.
					*/
					trace (U"found an unknown word starting with a nonupper-case letter, optionally preceded by a period");
					char32 *p = & command2.string [0];
					/*
						Variable names consist of a sequence of letters, digits, and underscores,
						optionally preceded by a period and optionally followed by a $ and/or #.
					*/
					if (*p == U'.')
						p ++;
					while (Melder_isWordCharacter (*p) || *p == U'.')
						p ++;
					if (*p == U'$') {
						if (p [1] == U'#') {
							/*
								Assign to a string array variable or a string array element.
							*/
							static MelderString arrayName;
							p ++;
							*p = U'\0';   // erase the number sign temporarily
							MelderString_copy (& arrayName, command2.string, U"#");
							*p = U'#';   // put the number sign back
							p ++;   // step over number sign
							Melder_skipHorizontalSpace (& p);   // go to first token after array name
							if (*p == U'=') {
								/*
									This must be an assignment to a string array variable.
								*/
								p ++;   // step over equals sign
								Melder_skipHorizontalSpace (& p);   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to string array ", arrayName.string, U".");
								if (isCommand (p)) {
									/*
										Statement like: lines$# = Get all strings
									*/
									bool status = praat_executeCommand (me, p);
									InterpreterVariable var = Interpreter_lookUpVariable (me, arrayName.string);
									if (! status)
										var -> stringArrayValue = autoSTRVEC();   // anything can have happened, including an incorrect returnType
									else if (my returnType == kInterpreter_ReturnType::STRINGARRAY_)
										var -> stringArrayValue = my returnedStringArray.move();
									else
										Melder_throw (kInterpreter_ReturnType_errorMessage (my returnType, p),
												U"; not assigned to the string array variable \"", arrayName.string, U"\".");
								} else {
									/*
										Statement like: files$# = fileNames$# ("*.wav")
									 */
									STRVEC value;
									bool owned;
									Interpreter_stringArrayExpression (me, p, & value, & owned);
									InterpreterVariable var = Interpreter_lookUpVariable (me, arrayName.string);
									StringArrayVariable_move (var, value, owned);
								}
							} else if (*p == U'[') {
								assignToStringArrayElement (me, ++ p, arrayName.string, valueString);
							} else Melder_throw (U"Missing '=' or '[' after string array variable ", arrayName.string, U".");
						} else {
							/*
								Assign to a string variable.
							*/
							trace (U"detected an assignment to a string variable");
							char32 *endOfVariable = ++ p;
							char32 *variableName = command2.string;
							Melder_skipHorizontalSpace (& p);   // go to first token after variable name
							if (*p == U'[') {
								/*
									This must be an assignment to an indexed string variable.
								*/
								*endOfVariable = U'\0';
								static MelderString indexedVariableName;
								MelderString_copy (& indexedVariableName, command2.string, U"[");
								for (;;) {
									p ++;   // skip opening bracket or comma
									static MelderString index;
									MelderString_empty (& index);
									int depth = 0;
									bool inString = false;
									while ((depth > 0 || (*p != U',' && *p != U']') || inString) && Melder_staysWithinLine (*p)) {
										MelderString_appendCharacter (& index, *p);
										if (*p == U'[') {
											if (! inString)
												depth ++;
										} else if (*p == U']') {
											if (! inString)
												depth --;
										}
										if (*p == U'"')
											inString = ! inString;
										p ++;
									}
									if (! Melder_staysWithinLine (*p))
										Melder_throw (U"Missing closing bracket (]) in indexed variable.");
									Formula_Result result;
									Interpreter_anyExpression (me, index.string, & result);
									if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
										const double numericIndexValue = result. numericResult;
										MelderString_append (& indexedVariableName, numericIndexValue);
									} else if (result.expressionType == kFormula_EXPRESSION_TYPE_STRING) {
										MelderString_append (& indexedVariableName, U"\"", result. stringResult.get(), U"\"");
									}
									MelderString_appendCharacter (& indexedVariableName, *p);
									if (*p == U']') {
										break;
									}
								}
								variableName = indexedVariableName.string;
								p ++;   // skip closing bracket
							}
							Melder_skipHorizontalSpace (& p);   // go to first token after (perhaps indexed) variable name
							int typeOfAssignment;   // 0, 1, 2, 3 or 4
							if (*p == U'=') {
								typeOfAssignment = 0;   // assignment
							} else if (*p == U'+') {
								if (p [1] == U'=') {
									typeOfAssignment = 1;   // adding assignment
									p ++;
								} else {
									Melder_throw (U"Missing “=” after string variable ", variableName, U" followed by “+”.");
								}
							} else if (*p == U'<') {
								typeOfAssignment = 2;   // read from file
							} else if (*p == U'>') {
								if (p [1] == U'>') {
									typeOfAssignment = 3;   // append to file
									p ++;
								} else {
									typeOfAssignment = 4;   // save to file
								}
							} else Melder_throw (U"Missing “=”, “+=”, “<”, or “>” after string variable ", variableName, U".");
							*endOfVariable = U'\0';
							p ++;
							Melder_skipHorizontalSpace (& p);   // go to first token after assignment or I/O symbol
							if (*p == U'\0') {
								if (typeOfAssignment >= 2)
									Melder_throw (U"Missing file name after variable ", variableName, U".");
								else
									Melder_throw (U"Missing expression after variable ", variableName, U".");
							}
							if (typeOfAssignment >= 2) {
								structMelderFile file { };
								Melder_relativePathToFile (p, & file);
								if (typeOfAssignment == 2) {
									autostring32 stringValue = MelderFile_readText (& file);
									InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
									var -> stringValue = stringValue.move();
								} else if (typeOfAssignment == 3) {
									if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Commands that write to a file are not available inside pictures.");
									InterpreterVariable var = Interpreter_hasVariable (me, variableName);
									if (! var)
										Melder_throw (U"Variable ", variableName, U" undefined.");
									MelderFile_appendText (& file, var -> stringValue.get());
								} else {
									if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw (U"Commands that write to a file are not available inside pictures.");
									InterpreterVariable var = Interpreter_hasVariable (me, variableName);
									if (! var)
										Melder_throw (U"Variable ", variableName, U" undefined.");
									MelderFile_writeText (& file, var -> stringValue.get(), Melder_getOutputEncoding ());
								}
							} else if (isCommand (p)) {
								/*
									Statement like: name$ = Get name
								*/
								MelderString_empty (& valueString);   // empty because command may print nothing; also makes sure that valueString.string exists
								autoMelderDivertInfo divert (& valueString);
								bool status = praat_executeCommand (me, p);
								InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
								if (! status)
									var -> stringValue = Melder_dup (U"");   // anything can have happened, including an incorrect returnType
								else if (my returnType == kInterpreter_ReturnType::STRING_ ||
									my returnType == kInterpreter_ReturnType::REAL_ ||
									my returnType == kInterpreter_ReturnType::INTEGER_
								)
									var -> stringValue = Melder_dup (valueString.string);
								else
									Melder_throw (kInterpreter_ReturnType_errorMessage (my returnType, p),
											U"; not assigned to the string variable \"", variableName, U"\".");
							} else {
								/*
									Evaluate a string expression and assign the result to the variable.
									Statements like:
										sentence$ = subject$ + verb$ + object$
										extension$ = if index (file$, ".") <> 0
										... then right$ (file$, length (file$) - rindex (file$, "."))
										... else "" fi
								*/
								trace (U"evaluating string expression");
								autostring32 stringValue = Interpreter_stringExpression (me, p);
								trace (U"assigning to string variable ", variableName);
								if (typeOfAssignment == 1) {
									InterpreterVariable var = Interpreter_hasVariable (me, variableName);
									if (! var)
										Melder_throw (U"The string ", variableName, U" does not exist.\n"
													  U"You can increment (+=) only existing strings.");
									const integer oldLength = Melder_length (var -> stringValue.get());
									const integer extraLength = Melder_length (stringValue.get());
									autostring32 newString (oldLength + extraLength, false);
									str32cpy (newString.get(), var -> stringValue.get());
									str32cpy (newString.get() + oldLength, stringValue.get());
									var -> stringValue = newString.move();
								} else {
									InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
									var -> stringValue = stringValue.move();
								}
							}
						}
					} else if (*p == U'#') {
						if (p [1] == U'#') {
							/*
								Assign to a numeric matrix variable or to a matrix element.
							*/
							static MelderString matrixName;
							p ++;   // go to second '#'
							*p = U'\0';   // erase the last number sign temporarily
							MelderString_copy (& matrixName, command2.string, U'#');
							*p = U'#';   // put the number sign back
							p ++;   // step over last number sign
							Melder_skipHorizontalSpace (& p);   // go to first token after matrix name
							if (*p == U'=') {
								/*
									This must be an assignment to a matrix variable.
								*/
								p ++;   // step over equals sign
								Melder_skipHorizontalSpace (& p);   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to matrix ", matrixName.string, U".");
								if (isCommand (p)) {
									/*
										Statement like: values## = Get all values
									*/
									const bool status = praat_executeCommand (me, p);
									InterpreterVariable var = Interpreter_lookUpVariable (me, matrixName.string);
									if (! status)
										var -> numericMatrixValue = autoMAT();   // anything can have happened, including an incorrect returnType
									else if (my returnType == kInterpreter_ReturnType::REALMATRIX_)
										var -> numericMatrixValue = my returnedRealMatrix.move();
									else
										Melder_throw (kInterpreter_ReturnType_errorMessage (my returnType, p),
												U"; not assigned to the matrix variable \"", matrixName.string, U"\".");
								} else {
									MAT value;
									bool owned;
									Interpreter_numericMatrixExpression (me, p, & value, & owned);
									InterpreterVariable var = Interpreter_lookUpVariable (me, matrixName.string);
									NumericMatrixVariable_move (var, value, owned);
								}
							} else if (*p == U'[') {
								assignToNumericMatrixElement (me, ++ p, matrixName.string, valueString);
							} else if (*p == U'+' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can increment (+=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_add (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_add (var, result. numericResult);
								} else {
									Melder_throw (U"You can increment (+=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'-' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can decrement (-=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_subtract (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_subtract (var, result. numericResult);
								} else {
									Melder_throw (U"You can decrement (-=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'*' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can multiply (*=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_multiply (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_multiply (var, result. numericResult);
								} else {
									Melder_throw (U"You can multiply (*=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'/' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
									              U"You can divide (/=) only existing matrices.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
									NumericMatrixVariable_divide (var, result. numericMatrixResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericMatrixVariable_divide (var, result. numericResult);
								} else {
									Melder_throw (U"You can divide (/=) a numeric matrix only with a number or another numeric matrix.");
								}
							} else if (*p == U'~') {
								/*
									This must be a formula assignment to a matrix variable.
								*/
								p ++;   // step over tilde
								Melder_skipHorizontalSpace (& p);   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing formula expression for matrix ", matrixName.string, U".");
								InterpreterVariable var = Interpreter_hasVariable (me, matrixName.string);
								if (! var)
									Melder_throw (U"The matrix ", matrixName.string, U" does not exist.\n"
										"You can assign a formula only to an existing matrix.");
								static Matrix matrixObject;
								if (! matrixObject)
									matrixObject = Matrix_createSimple (1, 1). releaseToAmbiguousOwner();   // prevent exit-time destruction
								MAT mat = var -> numericMatrixValue.get();
								matrixObject -> xmax = mat.ncol + 0.5;
								matrixObject -> nx = mat.ncol;
								matrixObject -> ymax = mat.nrow + 0.5;
								matrixObject -> ny = mat.nrow;
								matrixObject -> z.cells = mat.cells;   // just a reference (YUCK)
								matrixObject -> z.nrow = mat.nrow;
								matrixObject -> z.ncol = mat.ncol;
								Matrix_formula (matrixObject, p, me, nullptr);
							} else Melder_throw (U"Missing '=' after matrix variable ", matrixName.string, U".");
						} else {
							/*
								Assign to a numeric vector variable or to a vector element.
							*/
							static MelderString vectorName;
							*p = U'\0';   // erase the number sign temporarily
							MelderString_copy (& vectorName, command2.string, U"#");
							*p = U'#';   // put the number sign back
							p ++;   // step over number sign
							Melder_skipHorizontalSpace (& p);   // go to first token after array name
							if (*p == U'=') {
								/*
									This must be an assignment to a vector variable.
									First compute the value of the expression,
									*then* look up (perhaps create) the vector variable (the order is important)
									then assign the value to that vector variable.
									Reversing the order of the first and second step would fail the following test:
								*/
								/*@praat
									asserterror Unknown variable:'newline$'« interpreter_assignToVector#
									interpreter_assignToVector# = interpreter_assignToVector# + 5
								@*/
								p ++;   // step over equals sign
								Melder_skipHorizontalSpace (& p);   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing right-hand expression in assignment to vector ", vectorName.string, U".");
								if (isCommand (p)) {
									/*
										Statement like: times# = Get all times
									*/
									bool status = praat_executeCommand (me, p);
									InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);
									if (! status)
										var -> numericVectorValue = autoVEC();   // anything can have happened, including an incorrect returnType
									else if (my returnType == kInterpreter_ReturnType::OBJECT_) {
										var -> numericVectorValue = autoVEC();
										integer IOBJECT;
										WHERE (SELECTED)
											*var -> numericVectorValue. append() = ID;
									} else if (my returnType == kInterpreter_ReturnType::REALVECTOR_)
										var -> numericVectorValue = my returnedRealVector.move();
									else
										Melder_throw (kInterpreter_ReturnType_errorMessage (my returnType, p), U"; not assigned to the vector variable \"", vectorName.string, U"\".");
								} else {
									VEC value;
									bool owned;
									Interpreter_numericVectorExpression (me, p, & value, & owned);
									InterpreterVariable var = Interpreter_lookUpVariable (me, vectorName.string);   // not earlier, because it could have appeared in the expression
									NumericVectorVariable_move (var, value, owned);
								}
							} else if (*p == U'[') {
								assignToNumericVectorElement (me, ++ p, vectorName.string, valueString);
							} else if (*p == U'+' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can increment (+=) only existing vectors.");
								/*@praat
									asserterror The vector interpreter_assignToVector# does not exist.
									interpreter_assignToVector# += 5
								@*/
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_add (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_add (var, result. numericResult);
								} else {
									Melder_throw (U"You can increment (+=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'-' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can decrement (-=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_subtract (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_subtract (var, result. numericResult);
								} else {
									Melder_throw (U"You can decrement (-=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'*' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can multiply (*=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_multiply (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_multiply (var, result. numericResult);
								} else {
									Melder_throw (U"You can multiply (*=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'/' && p [1] == U'=') {
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
									              U"You can divide (/=) only existing vectors.");
								Formula_Result result;
								Interpreter_anyExpression (me, p += 2, & result);
								if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
									NumericVectorVariable_divide (var, result. numericVectorResult);
								} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									NumericVectorVariable_divide (var, result. numericResult);
								} else {
									Melder_throw (U"You can divide (/=) a numeric vector only with a number or another numeric vector.");
								}
							} else if (*p == U'~') {
								/*
									This must be a formula assignment to a vector variable.
								*/
								p ++;   // step over tilde
								Melder_skipHorizontalSpace (& p);   // go to first token after assignment
								if (*p == U'\0')
									Melder_throw (U"Missing formula expression for vector ", vectorName.string, U".");
								InterpreterVariable var = Interpreter_hasVariable (me, vectorName.string);
								if (! var)
									Melder_throw (U"The vector ", vectorName.string, U" does not exist.\n"
										"You can assign a formula only to an existing vector.");
								static Matrix vectorObject;
								if (! vectorObject)
									vectorObject = Matrix_createSimple (1, 1). releaseToAmbiguousOwner();   // prevent destruction when program ends
								VEC vec = var -> numericVectorValue.get();
								//vectorObject -> xmin = 0.5;
								vectorObject -> xmax = vec.size + 0.5;
								vectorObject -> nx = vec.size;
								vectorObject -> z.cells = & vec [1];
								//vectorObject -> z.nrow = 1;
								vectorObject -> z.ncol = vec.size;
								Matrix_formula (vectorObject, p, me, nullptr);
							} else Melder_throw (U"Missing '=' or '+=' or '[' or '~' after vector variable ", vectorName.string, U".");
						}
					} else {
						/*
							Try to assign to a numeric variable.
						*/
						double value;
						char32 *variableName = command2.string;
						int typeOfAssignment = 0;   // plain assignment
						if (*p == U'\0') {
							/*
								Command ends here: it may be a PraatShell command.
							*/
							(void) praat_executeCommand (me, command2.string);
							continue;   // next line
						}
						char32 *endOfVariable = p;
						Melder_skipHorizontalSpace (& p);
						if (*p == U'=' || ((*p == U'+' || *p == U'-' || *p == U'*' || *p == U'/') && p [1] == U'=')) {
							/*
								This must be an assignment (though: "echo = ..." ???)
							*/
							typeOfAssignment = ( *p == U'+' ? 1 : *p == U'-' ? 2 : *p == U'*' ? 3 : *p == U'/' ? 4 : 0 );
							*endOfVariable = U'\0';   // close variable name; FIXME: this can be any weird character, e.g. hallo&
						} else if (*p == U'[') {
							/*
								This must be an assignment to an indexed numeric variable.
							*/
							*endOfVariable = U'\0';
							static MelderString indexedVariableName;
							MelderString_copy (& indexedVariableName, command2.string, U"[");
							for (;;) {
								p ++;   // skip opening bracket or comma
								static MelderString index;
								MelderString_empty (& index);
								int depth = 0;
								bool inString = false;
								while ((depth > 0 || (*p != U',' && *p != U']') || inString) && Melder_staysWithinLine (*p)) {
									MelderString_appendCharacter (& index, *p);
									if (*p == U'[') {
										if (! inString)
											depth ++;
									} else if (*p == U']') {
										if (! inString)
											depth --;
									}
									if (*p == U'"')
										inString = ! inString;
									p ++;
								}
								if (! Melder_staysWithinLine (*p))
									Melder_throw (U"Missing closing bracket (]) in indexed variable.");
								Formula_Result result;
								Melder_assert (! result. stringResult);
								Interpreter_anyExpression (me, index.string, & result);
								if (result.expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
									const double numericIndexValue = result. numericResult;
									MelderString_append (& indexedVariableName, numericIndexValue);
								} else if (result.expressionType == kFormula_EXPRESSION_TYPE_STRING) {
									MelderString_append (& indexedVariableName, U"\"", result. stringResult.get(), U"\"");
								}
								MelderString_appendCharacter (& indexedVariableName, *p);
								if (*p == U']')
									break;
							}
							variableName = indexedVariableName.string;
							p ++;   // skip closing bracket
							Melder_skipHorizontalSpace (& p);
							if (*p == U'=' || ((*p == U'+' || *p == U'-' || *p == U'*' || *p == U'/') && p [1] == U'=')) {
								typeOfAssignment = ( *p == U'+' ? 1 : *p == U'-' ? 2 : *p == U'*' ? 3 : *p == U'/' ? 4 : 0 );
							}
						} else {
							/*
								Not an assignment: perhaps a PraatShell command (select, echo, execute, pause ...).
							*/
							(void) praat_executeCommand (me, variableName);
							continue;   // next line
						}
						p += ( typeOfAssignment == 0 ? 1 : 2 );
						Melder_skipHorizontalSpace (& p);
						if (*p == U'\0')
							Melder_throw (U"Missing expression after variable ", variableName, U".");
						/*
							Three classes of assignments:
								var = formula
								var = Query
								var = Object creation
						*/
						if (isCommand (p)) {
							/*
								Get the value of the query.
							*/
							MelderString_empty (& valueString);
							autoMelderDivertInfo divert (& valueString);
							MelderString_appendCharacter (& valueString, 1);   // will be overwritten by something totally different if any MelderInfo function is called...
							bool status = praat_executeCommand (me, p);
							if (! status) {
								value = undefined;   // anything can have happened, including an incorrect return type
							} else if (my returnType == kInterpreter_ReturnType::OBJECT_) {
								integer IOBJECT, selectedObject = 0, numberOfSelectedObjects = 0;
								WHERE (SELECTED) {
									selectedObject = IOBJECT;
									numberOfSelectedObjects += 1;
								}
								if (numberOfSelectedObjects > 1)
									Melder_throw (U"Multiple objects selected. Cannot assign object IDs to numeric variable. "
											"Perhaps use a vector variable instead.");
								if (numberOfSelectedObjects == 0)
									Melder_throw (U"No objects selected. Cannot assign object ID to variable.");
								value = theCurrentPraatObjects -> list [selectedObject]. id;
							} else if (my returnType == kInterpreter_ReturnType::REAL_ || my returnType == kInterpreter_ReturnType::INTEGER_) {
								value = Melder_atof (valueString.string);   // including --undefined--
							} else
								Melder_throw (kInterpreter_ReturnType_errorMessage (my returnType, p),
										U"; not assigned to the numeric variable \"", variableName, U"\".");
						} else {
							/*
								Get the value of the formula.
							*/
							Interpreter_numericExpression (me, p, & value);
						}
						/*
							Assign the value to a variable.
						*/
						if (typeOfAssignment == 0) {
							/*
								Use an existing variable, or create a new one.
							*/
							//Melder_casual (U"looking up variable ", variableName);
							InterpreterVariable var = Interpreter_lookUpVariable (me, variableName);
							var -> numericValue = value;
						} else {
							/*
								Modify an existing variable.
							*/
							InterpreterVariable var = Interpreter_hasVariable (me, variableName);
							if (! var)
								Melder_throw (U"The variable ", variableName, U" does not exist. You can modify only existing variables.");
							if (isundef (var -> numericValue)) {
								/* Keep it that way. */
							} else {
								if (typeOfAssignment == 1) {
									var -> numericValue += value;
								} else if (typeOfAssignment == 2) {
									var -> numericValue -= value;
								} else if (typeOfAssignment == 3) {
									var -> numericValue *= value;
								} else if (value == 0) {
									var -> numericValue = undefined;
								} else {
									var -> numericValue /= value;
								}
							}
						}
					}
				} // endif fail
				if (my assertErrorLineNumber != 0 && my assertErrorLineNumber != my lineNumber) {
					const integer save_assertErrorLineNumber = my assertErrorLineNumber;
					my assertErrorLineNumber = 0;
					Melder_throw (U"Script assertion fails in line ", save_assertErrorLineNumber,
							U": error « ", my assertErrorString.string, U" » not raised. Instead: no error.");

				}
			} catch (MelderError) {
				//	Melder_casual (U"Error: << ", Melder_getError(),
				//		U" >>\nassertErrorLineNumber: ", my assertErrorLineNumber,
				//		U"\nlineNumber: ", lineNumber,
				//		U"\nAssert error string: << ", assertErrorString.string,
				//		U" >>\n"
				//	);
				if (Melder_hasCrash ())
					throw;
				if (my assertErrorLineNumber == 0) {
					throw;
				} else if (my assertErrorLineNumber != my lineNumber) {
					if (str32str (Melder_getError (), my assertErrorString.string)) {
						Melder_clearError ();
						my assertErrorLineNumber = 0;
					} else {
						autostring32 errorCopy_nothrow = Melder_dup_f (Melder_getError ());
						Melder_clearError ();
						Melder_throw (U"Script assertion fails in line ", my assertErrorLineNumber,
							U": error « ", my assertErrorString.string, U" » not raised. Instead:\n",
							errorCopy_nothrow.get()
						);
					}
				}
			}
		} // endfor lineNumber
		if (! my isHalted) {
			//my numberOfLabels = 0;
			my running = false;
			my stopped = false;
		}
	} catch (MelderError) {
		//TRACE
		trace (U"catch");
		trace (U"catch ", Melder_pointer (me));
		trace (U"catch at line number ", my lineNumber);
		trace (U"catch with stack ", Melder_pointer (my owningInterpreterStack));
		trace (U"catch in file ", & my file);
		trace (U"catch at level ", my owningInterpreterStack -> currentLevel);
		trace (U"catch at stack interpreter ", Melder_pointer (my owningInterpreterStack -> current_0 ()));
		if (my lineNumber > 0) {
			const bool normalExplicitExit = str32nequ (my lines [my lineNumber], U"exit ", 5) || Melder_hasError (U"Script exited.");
			if (! normalExplicitExit && ! assertionFailed) {   // don't show the message twice!
				while (my lines [my lineNumber] [0] == U'\0') {   // did this use to be a continuation line?
					my lineNumber --;
					trace (U"line number lowered to ", my lineNumber);
					Melder_assert (my lineNumber > 0);   // originally empty lines that stayed empty should not generate errors
				}
				Melder_appendError (U"Script line ", my lineNumber, U" not performed or completed:\n« ", my lines [my lineNumber], U" »");
			}
		}
		//my numberOfLabels = 0;
		my running = false;
		my stopped = false;
		//my isHalted = false;   // TODO: needed?
		trace (U"catch: has finished...", Melder_pointer (my owningInterpreterStack));
		trace (U"catch: ... has finished");
		if (Melder_hasCrash ()) {
			my owningInterpreterStack -> currentLevel -= 1;   // TODO: encapsulate
			throw;
		} else if (str32equ (Melder_getError (), U"\nScript exited.\n")) {
			Melder_clearError ();
		} else {
			my owningInterpreterStack -> currentLevel -= 1;   // TODO: encapsulate
			throw;
		}
	}
}

void Interpreter_stop (Interpreter me) {
//Melder_casual (U"Interpreter_stop in: ", Melder_pointer (me));
	my stopped = true;
	my running = false;
	//my numberOfLabels = 0;
//Melder_casual (U"Interpreter_stop out: ", Melder_pointer (me));
}

void Interpreter_voidExpression (Interpreter me, conststring32 expression) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
}

void Interpreter_numericExpression (Interpreter me, conststring32 expression, double *out_value) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. numericResult;
}

void Interpreter_numericVectorExpression (Interpreter me, conststring32 expression, VEC *out_value, bool *out_owned) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. numericVectorResult;
	*out_owned = result. owned;
	result. owned = false;
}

void Interpreter_numericMatrixExpression (Interpreter me, conststring32 expression, MAT *out_value, bool *out_owned) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. numericMatrixResult;
	*out_owned = result. owned;
	result. owned = false;
}

autostring32 Interpreter_stringExpression (Interpreter me, conststring32 expression) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_STRING, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	return result. stringResult.move();
}

void Interpreter_stringArrayExpression (Interpreter me, conststring32 expression, STRVEC *out_value, bool *out_owned) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_STRING_ARRAY, false);
	Formula_Result result;
	Formula_run (0, 0, & result);
	*out_value = result. stringArrayResult;
	*out_owned = result. owned;
	result. owned = false;
}

void Interpreter_anyExpression (Interpreter me, conststring32 expression, Formula_Result *out_result) {
	Formula_compile (me, nullptr, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, false);
	Formula_run (0, 0, out_result);
}

Thing_implement (InterpreterStack, Thing, 0);

/*
	Schematic code:

	void Interpreter_run (Interpreter me) {
		my lineNumber = 1;
		Interpreter_resume (me);
	}
	void Interpreter_resume (Interpreter me) {
		my isHalted = false;
		try {
			for (; lineNumber <= my lines.size; lineNumber ++) {
				if (my isHalted)
					break;
				handleOneLine ();
			}
		} catch (MelderError) {
			my owningInterpreterStack -> currentLevel -= 1;
			throw;
		}
	}
	void runScript (InterpreterStack interpreterStack, conststring32 fileName, integer narg, Stackel args, Editor optionalInterpreterOwningEditor) {
		Melder_assert (interpreterStack);
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
				interpreterStack -> runDown (me.move(), autostring32(), false);   // back to the default directory of the caller
			} catch (MelderError) {
				Melder_throw (U"Script ", & file, U" not completed.");   // don't refer to 'fileName', because its contents may have changed
			}
		}
	}
	int endPause (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
		conststring32 continueText1, conststring32 continueText2, conststring32 continueText3,
		conststring32 continueText4, conststring32 continueText5, conststring32 continueText6,
		conststring32 continueText7, conststring32 continueText8, conststring32 continueText9,
		conststring32 continueText10, Interpreter interpreter)
	{
		Melder_assert (interpreter);
		if (interpreter -> isInSecondPass) {
			Melder_assert (interpreter == thePauseForm_interpreterReference);
			Melder_assert (! thePauseForm);
			interpreter -> isInSecondPass = false;   // TODO: needed?
			return thePauseForm_clicked;
		} else {
			if (! thePauseForm)
				Melder_throw (U"Found the function “endPause” without a preceding “beginPause”.");
			thePauseForm_interpreterReference = interpreter;
			thePauseForm_savedEditorReference = interpreter -> optionalDynamicEnvironmentEditor();
			UiForm_setPauseForm (thePauseForm.get(), numberOfContinueButtons, defaultContinueButton, cancelContinueButton,
				continueText1, continueText2, continueText3, continueText4, continueText5,
				continueText6, continueText7, continueText8, continueText9, continueText10,
				thePauseFormCancelCallback
			);
			theCancelContinueButton = cancelContinueButton;
			UiForm_finish (thePauseForm.get());
			thePauseForm_clicked = 0;
			Melder_assert (interpreter -> owningInterpreterStack);
			interpreter -> owningInterpreterStack -> haltAll ();
			UiForm_destroyWhenUnmanaged (thePauseForm.get());
			UiForm_do (thePauseForm.get(), false);   // put he pause form on the screen
			return 0;
		}
	}
	void Demo_waitForInput (Interpreter interpreter) {
		Melder_assert (interpreter);
		if (interpreter -> isInSecondPass) {
			interpreter -> isInSecondPass = false;   // TODO: needed?
		} else {
			if (! theReferenceToTheOnlyDemoEditor)
				Melder_throw (U"Cannot do demoWaitForInput() if the Demo window isn’t visible.");
			if (theReferenceToTheOnlyDemoEditor -> waitingForInput &&0) {
				Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
					U"Please click or type into the Demo window or close it.");
			}
			//GuiThing_show (theReferenceToTheOnlyDemoEditor -> windowForm);
			theReferenceToTheOnlyDemoEditor -> clicked = false;
			theReferenceToTheOnlyDemoEditor -> keyPressed = false;
			theReferenceToTheOnlyDemoEditor -> waitingForInput = true;
			interpreter -> owningInterpreterStack -> haltAll ();
			theReferenceToTheOnlyDemoEditor -> interpreterReference = interpreter;
		}
	}
	static void thePauseFormOkCallback (void *closure) {
		if (! thePauseForm_interpreterReference) {   // interpreter was destroyed?
			GuiThing_hide (thePauseForm -> d_dialogForm);   // BUG: memory leak
			thePauseForm. releaseToUser();
			return;
		}
		if (! thePauseForm)   // BUG: perhaps there was a mistake in the script
			return;
		thePauseForm_clicked = UiForm_getClickedContinueButton (thePauseForm.get());
		if (thePauseForm_clicked != theCancelContinueButton)
			UiForm_Interpreter_addVariables (thePauseForm.get(), (Interpreter) closure);   // 'closure', not 'interpreter' or 'theInterpreter'!
		Melder_assert (thePauseForm_clicked >= 1 && thePauseForm_clicked <= 10);
		autostring32 clickedText = Melder_dup (thePauseForm -> continueTexts [thePauseForm_clicked].get());   // very safe
		if (thePauseForm -> d_dialogForm) {
			GuiThing_hide (thePauseForm -> d_dialogForm);   // BUG: memory leak
			//thePauseForm_interpreterReference -> isInSecondPass = false;   // TODO: corrected? and what about isHalted?
		}
		thePauseForm. releaseToUser();   // undangle (will be autodestroyed when unmanaged)
		Melder_assert (! thePauseForm);
		try {
			autoPraatBackground background;
			Melder_assert (thePauseForm_interpreterReference -> owningInterpreterStack);
			thePauseForm_interpreterReference -> owningInterpreterStack -> resumeFromTop ();
		} catch (MelderError) {
			if (thePauseForm_clicked == theCancelContinueButton)
				Melder_throw (U"This happened after you cancelled the pause form.");
			else
				Melder_throw (U"This happened after you clicked “", clickedText.get(), U"” in the pause form.");
		}
	}
*/

autoInterpreterStack InterpreterStack_create (Editor optionalInterpreterStackOwningEditor) {
	try {
		autoInterpreterStack me = Thing_new (InterpreterStack);
		my optionalInterpreterStackOwningEditor = optionalInterpreterStackOwningEditor;
		return me;
	} catch (MelderError) {
		Melder_throw (U"InterpreterStack not created.");
	}
}

void structInterpreterStack :: emptyAll () {
	for (integer ilevel = 1; ilevel <= our currentLevel; ilevel ++)
		our interpreters [ilevel]. reset();
	our currentLevel = 0;
}

void structInterpreterStack :: runDown (autoInterpreter interpreter, autostring32 text, const bool reuseVariables) {
	if (our currentLevel == InterpreterStack_MAXIMUM_NUMBER_OF_LEVELS)
		Melder_throw (U"Cannot have more than ", InterpreterStack_MAXIMUM_NUMBER_OF_LEVELS, U" levels of nested scripts.");
	our currentLevel += 1;
	if (interpreter)
		our interpreters [our currentLevel] = interpreter.move();   // otherwise, use my existing interpreter
	const integer oldLevel = our currentLevel;   // sets up an assertion below
	Interpreter childInterpreter = our current_a();
	try {
		autoMelderSetCurrentFolder folder (& childInterpreter -> workingDirectory);
		Interpreter_run (childInterpreter, text.move(), reuseVariables);
	} catch (MelderError) {
		//if (currentLevel == 0)
		//	our emptyAll ();   // TODO: figure out when precisely an interpreter can be deleted
		//Melder_throw (U"Interpreter stack not run completely.");
		throw;
	}
	Melder_assert (our currentLevel == oldLevel);
	our currentLevel -= 1;
	if (our currentLevel == 0) {
		//our emptyAll();   // TODO: successful run of the whole stack; can we delete? Does it depend on halted?
		return;   // nothing else to do
	}
	Interpreter parentInterpreter = our current_a();
	if (! childInterpreter -> isHalted) {   // TODO: why the child interpreter?
		parentInterpreter -> isInSecondPass = false;   // TODO: why the parent interpreter?
		//childInterpreter. reset();   // TODO: figure out when precisely an interpreter can be deleted
	}
}

void structInterpreterStack :: haltAll () {
	for (integer ilevel = 1; ilevel <= our currentLevel; ilevel ++) {
		Melder_assert (our interpreters [ilevel]);
		our interpreters [ilevel] -> isHalted = true;   // ensures fast unwinding, without processing any more lines
		our interpreters [ilevel] -> lineNumber -= 1;   // ensure that all runScripts will get a second pass
	}
}

void structInterpreterStack :: resumeFromTop () {
	our currentLevel = 1;
	Melder_assert (our interpreters [1]);

	/*
		Unhalt all.
	*/
	for (integer ilevel = 1; ilevel <= InterpreterStack_MAXIMUM_NUMBER_OF_LEVELS; ilevel ++)
		if (our interpreters [ilevel]) {
			our interpreters [ilevel] -> isHalted = false;
			our interpreters [ilevel] -> isInSecondPass = true;   // ensures that all runScripts, the pause window and demoWaitForInput will short-cut
		}

	try {
		autoMelderSetCurrentFolder folder (& our interpreters [1] -> workingDirectory);
		Interpreter_resume (our interpreters [1].get());
	} catch (MelderError) {
		//if (currentLevel == 0)
		//	our emptyAll ();   // TODO: figure out when precisely an interpreter can be deleted; perhaps always, when we're here?
		//Melder_throw (U"Interpreter stack has not run to completion.");
		throw;
	}
	our currentLevel -= 1;
	Melder_assert (our currentLevel == 0);   // running an interpreter to its end MUST have decremented the level
	//our emptyAll();   // TODO: successful run of the whole stack; can we delete? Does it depend on halted?
}

void structInterpreterStack :: resumeNextLevelInSecondPass () {
	our currentLevel += 1;
	Melder_assert (our currentLevel <= InterpreterStack_MAXIMUM_NUMBER_OF_LEVELS);
	Interpreter childInterpreter = our current_a();
	{// scope
		autoMelderSetCurrentFolder folder (& childInterpreter -> workingDirectory);   // so that callee-relative file names can be used inside the script
		Interpreter_resume (childInterpreter);
	}   // back to the default directory of the caller
	our currentLevel -= 1;
	if (our currentLevel == 0)
		return;   // nothing to do
	Interpreter parentInterpreter = our current_a();
	//Melder_assert (! childInterpreter -> isHalted);   // TODO: why not parentInterpreter? and why can this fire?
	if (! childInterpreter -> isHalted) {
		parentInterpreter -> isInSecondPass = false;   // TODO: why not childInterpreter?
		//our interpreters [our currentLevel + 1]. reset();   // TODO: figure out when precisely an interpreter can be deleted
	}
}

/* End of file Interpreter.cpp */
