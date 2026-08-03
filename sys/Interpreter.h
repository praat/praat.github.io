#ifndef _Interpreter_h_
#define _Interpreter_h_
/* Interpreter.h
 *
 * Copyright (C) 1993-2018,2020-2026 Paul Boersma
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

#include "Collection.h"
#include "Gui.h"
#include "Formula.h"

#include <string>
#include <unordered_map>

Thing_define (InterpreterVariable, SimpleString) {
	double numericValue;   // a variable whose name has no suffix: a real, an integer, or a boolean
	autostring32 stringValue;   // a variable whose name has the suffix "$"
	autoVEC numericVectorValue;   // a variable whose name has the suffix "#"
	autoMAT numericMatrixValue;   // a variable whose name has the suffix "##"
	autoSTRVEC stringArrayValue;   // a variable whose name has the suffix "$#"
};

#define Interpreter_MAXNUM_PARAMETERS  400
#define Interpreter_MAX_CALL_DEPTH  50

#define Interpreter_MAX_LABEL_LENGTH  99
#define Interpreter_MAX_PARAMETER_LENGTH  99
#define Interpreter_MAX_FORMAT_LENGTH  39

Thing_declare (UiForm);
Thing_declare (UiField);
Thing_declare (Editor);
Thing_declare (Script);
Thing_declare (Notebook);
Thing_declare (InterpreterStack);

enum class kInterpreter_ReturnType {
	VOID_ = 0,   // don't change; this is how it is automatically zero-initialized in structInterpreter
	OBJECT_,

	/*
		The following three are not distinguished in PraatScript,
		but they may need to be distinguished in PraatLib (for C, Python, R).
	*/
	REAL_,
	INTEGER_,
	BOOLEAN_,

	STRING_,

	/*
		The following two are not distinguished in PraatScript,
		but they may need to be distinguished in PraatLib (for C, Python, R).
	*/
	REALVECTOR_,
	INTEGERVECTOR_,

	REALMATRIX_,
	STRINGARRAY_
};

conststring32 kInterpreter_ReturnType_errorMessage (kInterpreter_ReturnType returnType, conststring32 command);

Thing_define (Interpreter, Thing) {
	Script scriptReference;
	Notebook notebookReference;
	InterpreterStack owningInterpreterStack;
	bool isHalted, isInSecondPass;

	struct EditorEnvironment {
		ClassInfo _optionalClass;
		Editor _optionalInstance;
		void _setFromOptionalEditor (Editor optionalEditor) noexcept {
			our _optionalClass = ( optionalEditor ? ((Thing) optionalEditor) -> classInfo : nullptr );
					// class Editor hasn't been defined yet, but assuming it's a Thing seems to be safe...
			our _optionalInstance = optionalEditor;
		}
		conststring32 _optionalClassName () noexcept {
			return _optionalClass ? our _optionalClass -> className : nullptr;
		}
	} _owningEditorEnvironment, _dynamicEditorEnvironment;
	/*
		Each entire editor environment (but not its parts separately) can be set from an Editor;
		in case of an owning editor, the editor is optional; in case of a dynamic editor, it's obligatory.
		The parts of an editor environment (class name and instance) can be separately gotten.
	*/
	void setOwningEditorEnvironmentFromOptionalEditor (Editor optionalEditor) noexcept {
		our _owningEditorEnvironment. _setFromOptionalEditor (optionalEditor);
	}
	conststring32 optionalOwningEditorEnvironmentClassName () noexcept {
		return our _owningEditorEnvironment. _optionalClassName();
	}
	Editor optionalOwningEnvironmentEditor () noexcept {
		return our _owningEditorEnvironment. _optionalInstance;
	}
	void setDynamicEditorEnvironmentFromEditor (Editor editor) noexcept {
		Melder_assert (editor);
		our _dynamicEditorEnvironment. _setFromOptionalEditor (editor);
	}
	conststring32 optionalDynamicEditorEnvironmentClassName () noexcept {
		return our _dynamicEditorEnvironment. _optionalClassName();
	}
	Editor optionalDynamicEnvironmentEditor () noexcept {
		return our _dynamicEditorEnvironment. _optionalInstance;
	}
	/*
		An owning editor environment can be copied to a dynamic one, but not the other way round.
	*/
	void setDynamicFromOwningEditorEnvironment () noexcept {
		our _dynamicEditorEnvironment = our _owningEditorEnvironment;   // TODO: what if the owner has been orphaned?
	}
	/*
		Memory of the past.
	*/
	bool wasStartedFromEditorEnvironment () noexcept {
		return !! our _owningEditorEnvironment. _optionalClass;
	}
	/*
		Awareness of the present.
	*/
	bool hasDynamicEnvironmentEditor () noexcept {
		return !! our _dynamicEditorEnvironment. _optionalInstance;
	}
	/*
		Forgetting. The owning editor's class will always be remembered, though. That's our identity, so to say.

		The dynamic editor's class will also be remembered,
		on behalf of an error message by a continuing pause script.
		(last checked 2023-03-05)
	*/
	void undangleOwningEditor () noexcept {
		our _owningEditorEnvironment. _optionalInstance = nullptr;
	}
	void undangleDynamicEditor () noexcept {
		our _dynamicEditorEnvironment. _optionalInstance = nullptr;
	}
	void undangleEditorEnvironments () noexcept {
		our _owningEditorEnvironment. _optionalInstance = nullptr;
		our _dynamicEditorEnvironment. _optionalInstance = nullptr;
	}
	void nullifyDynamicEditorEnvironment () noexcept {
		our _dynamicEditorEnvironment. _optionalClass = nullptr;
		our _dynamicEditorEnvironment. _optionalInstance = nullptr;
	}

	autostring32 text;
	structMelderFile file;
	structMelderFolder workingDirectory;

	int numberOfParameters, callDepth;
	int types [1+Interpreter_MAXNUM_PARAMETERS], numbersOfLines [1+Interpreter_MAXNUM_PARAMETERS];
	char32 parameters [1+Interpreter_MAXNUM_PARAMETERS] [1+Interpreter_MAX_PARAMETER_LENGTH];
	char32 formats [1+Interpreter_MAXNUM_PARAMETERS] [1+Interpreter_MAX_FORMAT_LENGTH];
	autostring32 arguments [1+Interpreter_MAXNUM_PARAMETERS];
	char32 choiceArguments [1+Interpreter_MAXNUM_PARAMETERS] [100];
	autoSTRVEC labelNames;
	autoINTVEC labelLines;
	autoSTRVEC procedureNames;
	autoINTVEC procedureStartLines;
	autostring32 dialogTitle;
	char32 procedureStackNames [1+Interpreter_MAX_CALL_DEPTH] [100];
	std::unordered_map <std::u32string, autoInterpreterVariable> variablesMap;
	bool running, stopped;
	integer callStack [1 + Interpreter_MAX_CALL_DEPTH];
	bool fromif, fromendfor;

	autovector <mutablestring32> lines;   // not autostringvector, because the elements are reference copies
	integer lineNumber = 0;
	integer assertErrorLineNumber = 0;
	autoMelderString assertErrorString;

	kInterpreter_ReturnType returnType;   // automatically initialized as kInterpreter_ReturnType::VOID_
	bool returnedBoolean;
	autostring32 returnedString;
	autoVEC returnedRealVector;
	autoINTVEC returnedIntegerVector;
	autoMAT returnedRealMatrix;
	autoSTRVEC returnedStringArray;

	void v9_destroy () noexcept
		override;
};

autoInterpreter Interpreter_createFromEnvironment (
	InterpreterStack owner,
	Editor optionalEditor,
	MelderFile optionalFile
);

void Interpreter_rememberScript (Interpreter me, MelderFile scriptFile, bool fullTrust);
void Interpreter_rememberScript_override (Interpreter me, MelderFile scriptFile, const bool fullTrust);

void Interpreters_undangleEnvironment (Editor environment) noexcept;

void Melder_includeIncludeFiles (autostring32 *text, bool onlyInCodeChunks = false);
integer Interpreter_readParameters (Interpreter me, mutablestring32 text);
Thing_declare (UiForm);
autoUiForm Interpreter_createForm (Interpreter me, GuiWindow parent, Editor optionalEditor, conststring32 fileName,
	void (*okCallback) (UiForm sendingForm, integer narg, Stackel args, conststring32 sendingString,
			Interpreter interpreter, conststring32 invokingButtonTitle, bool modified, void *closure, Editor optionalEditor),
	void *okClosure, bool selectionOnly
);
void Interpreter_getArgumentsFromDialog (Interpreter me, UiForm dialog);
void Interpreter_getArgumentsFromString (Interpreter me, conststring32 arguments);
void Interpreter_getArgumentsFromArgs (Interpreter me, integer nargs, Stackel args);
void Interpreter_getArgumentsFromCommandLine (Interpreter me, integer argc, char **argv);
void Interpreter_run (Interpreter me, autostring32 text, const bool reuseVariables);   // messes up 'text'
void Interpreter_stop (Interpreter me);   // can be called from any procedure called deep-down by the interpreter; will stop before next line
void Interpreter_resume (Interpreter me);

void Interpreter_voidExpression (Interpreter me, conststring32 expression);
void Interpreter_numericExpression (Interpreter me, conststring32 expression, double *p_value);
void Interpreter_numericVectorExpression (Interpreter me, conststring32 expression, VEC *p_value, bool *p_owned);
void Interpreter_numericMatrixExpression (Interpreter me, conststring32 expression, MAT *p_value, bool *p_owned);
autostring32 Interpreter_stringExpression (Interpreter me, conststring32 expression);
void Interpreter_stringArrayExpression (Interpreter me, conststring32 expression, STRVEC *out_value, bool *out_owned);
void Interpreter_anyExpression (Interpreter me, conststring32 expression, Formula_Result *p_result);

InterpreterVariable Interpreter_hasVariable (Interpreter me, conststring32 key);
InterpreterVariable Interpreter_lookUpVariable (Interpreter me, conststring32 key);

constexpr integer InterpreterStack_MAXIMUM_NUMBER_OF_LEVELS = 20;

Thing_define (InterpreterStack, Thing) {
	Editor optionalInterpreterStackOwningEditor;   // TODO: remove
	autoInterpreter interpreters [1 + InterpreterStack_MAXIMUM_NUMBER_OF_LEVELS] { };
	integer currentLevel = 0;

	Interpreter current_0 () {
		return our interpreters [our currentLevel].get();
	}
	Interpreter current_e () {
		Interpreter interpreterReference = our interpreters [our currentLevel].get();
		Melder_require (interpreterReference,
			U"Interpreter at level ", our currentLevel, U" does not exist.");
		return interpreterReference;
	}
	Interpreter current_a () {
		Interpreter interpreterReference = our interpreters [our currentLevel].get();
		Melder_assert (interpreterReference);
		return interpreterReference;
	}
	void emptyAll ();
	void runDown (autoInterpreter interpreter, autostring32 text, const bool reuseVariables);
	void haltAll ();
	void resumeFromTop ();
	void resumeNextLevelInSecondPass ();
};

autoInterpreterStack InterpreterStack_create (Editor optionalInterpreterStackOwningEditor);

extern structInterpreterStack theInterpreterStack;   // TODO: remove

/* End of file Interpreter.h */
#endif
