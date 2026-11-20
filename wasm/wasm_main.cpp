/* wasm_main.cpp
 *
 * WASM entry point for praat-wasm.
 * Initializes Praat in library/batch mode and registers all phonetic commands.
 * This replaces main/main_Praat.cpp for the WASM build.
 *
 * Copyright (C) 2026 praat-wasm contributors
 * Licensed under the GNU General Public License v3+.
 */

#include "../sys/praat.h"
#include "../sys/ManPages.h"
#include "../fon/praat_uvafon_init.h"
#include "../sys/praat_script.h"
#include "../sys/Interpreter.h"
#include "../melder/melder_console.h"

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <string>
#include <cstring>

/*
	Initialize the Praat library.
	Must be called once before any other operations.
*/
static bool g_initialized = false;

void wasm_praatlib_init () {
	if (g_initialized)
		return;
	praatlib_init ();
	/*
		praatlib_init does not call MelderConsole_init(), so Melder_stdout
		and Melder_stderr are null. We need them for script output.
	*/
	MelderConsole_init ();
	/*
		praatlib_init does not create manPages, but praat_uvafon_init
		uses INCLUDE_MANPAGES macros that add pages to theCurrentPraatApplication -> manPages.
		We must create it here to avoid a null-pointer crash.
	*/
	theCurrentPraatApplication -> manPages = ManPages_create ().releaseToAmbiguousOwner();
	praat_uvafon_init ();
	g_initialized = true;
}

/*
	Execute a Praat script string.
	Returns the contents of the Info window after execution.
*/
std::string wasm_executeScript (const std::string &scriptText) {
	if (! g_initialized)
		wasm_praatlib_init ();
	try {
		/*
			Clear the info window before executing.
		*/
		Melder_clearInfo ();
		/*
			Convert UTF-8 to Praat's internal char32 string.
		*/
		autostring32 script32 = Melder_8to32_e (scriptText.c_str ());
		/*
			Execute the script.
		*/
		praat_executeScriptFromText (script32.get ());
		/*
			Return the info window contents.
		*/
		conststring32 info = Melder_getInfo ();
		if (info) {
			autostring8 info8 = Melder_32to8 (info);
			return std::string (info8.get ());
		}
		return "";
	} catch (MelderError) {
		/*
			Convert the error to a string and rethrow as a JS exception.
		*/
		conststring32 errorMessage = Melder_getError ();
		std::string msg;
		if (errorMessage) {
			autostring8 msg8 = Melder_32to8 (errorMessage);
			msg = std::string (msg8.get ());
		} else {
			msg = "Unknown Praat error";
		}
		Melder_clearError ();
		throw std::runtime_error (msg);
	}
}

/*
	Get a list of objects currently in the Praat object list.
	Returns a JS-friendly string with one object per line: "id type name".
*/
std::string wasm_listObjects () {
	if (! g_initialized)
		wasm_praatlib_init ();
	std::string result;
	integer n = theCurrentPraatObjects -> n;
	for (integer i = 1; i <= n; i ++) {
		if (theCurrentPraatObjects -> list [i]. isBeingCreated)
			continue;
		const char32 *className = Thing_className (theCurrentPraatObjects -> list [i]. object);
		const char32 *name = theCurrentPraatObjects -> list [i]. name.get ();
		autostring8 class8 = Melder_32to8 (className);
		autostring8 name8 = Melder_32to8 (name ? name : U"");
		result += std::to_string (theCurrentPraatObjects -> list [i]. id);
		result += " ";
		result += class8.get ();
		result += " ";
		result += name8.get ();
		result += "\n";
	}
	return result;
}

/*
	Select an object by its ID.
*/
void wasm_selectObject (int id) {
	if (! g_initialized)
		wasm_praatlib_init ();
	std::string script = "selectObject: " + std::to_string (id) + "\n";
	autostring32 script32 = Melder_8to32_e (script.c_str ());
	praat_executeScriptFromText (script32.get ());
}

/*
	Remove all objects from the Praat object list.
*/
void wasm_removeAllObjects () {
	if (! g_initialized)
		wasm_praatlib_init ();
	praat_executeScriptFromText (Melder_8to32_e (
		"select all\n"
		"if numberOfSelected() > 0\n"
		"  Remove\n"
		"endif\n"
	).get ());
}

/*
	Remove selected objects.
*/
void wasm_removeSelectedObjects () {
	if (! g_initialized)
		wasm_praatlib_init ();
	praat_executeScriptFromText (Melder_8to32_e ("Remove\n").get ());
}

/*
	Read a file from the MEMFS virtual filesystem into the Praat object list.
	The file must have been written to MEMFS first via the JS side.
*/
std::string wasm_readFile (const std::string &path) {
	if (! g_initialized)
		wasm_praatlib_init ();
	try {
		Melder_clearInfo ();
		std::string script = "Read from file: \"" + path + "\"\n";
		autostring32 script32 = Melder_8to32_e (script.c_str ());
		praat_executeScriptFromText (script32.get ());
		conststring32 info = Melder_getInfo ();
		if (info) {
			autostring8 info8 = Melder_32to8 (info);
			return std::string (info8.get ());
		}
		return "";
	} catch (MelderError) {
		conststring32 errorMessage = Melder_getError ();
		std::string msg;
		if (errorMessage) {
			autostring8 msg8 = Melder_32to8 (errorMessage);
			msg = std::string (msg8.get ());
		} else {
			msg = "Unknown Praat error";
		}
		Melder_clearError ();
		throw std::runtime_error (msg);
	}
}

/*
	Execute a single Praat command with arguments on the selected object(s).
	Equivalent to Parselmouth's praat.call().
	commandWithArgs is a Praat command line like: To Pitch... 0 75 600
*/
std::string wasm_call (const std::string &commandWithArgs) {
	if (! g_initialized)
		wasm_praatlib_init ();
	try {
		Melder_clearInfo ();
		autostring32 cmd32 = Melder_8to32_e (commandWithArgs.c_str ());
		praat_executeScriptFromText (cmd32.get ());
		conststring32 info = Melder_getInfo ();
		if (info) {
			autostring8 info8 = Melder_32to8 (info);
			return std::string (info8.get ());
		}
		return "";
	} catch (MelderError) {
		conststring32 errorMessage = Melder_getError ();
		std::string msg;
		if (errorMessage) {
			autostring8 msg8 = Melder_32to8 (errorMessage);
			msg = std::string (msg8.get ());
		} else {
			msg = "Unknown Praat error";
		}
		Melder_clearError ();
		throw std::runtime_error (msg);
	}
}

/*
	Write selected object to a file in MEMFS.
	The file can then be read by the JS side.
*/
void wasm_saveAsText (const std::string &path) {
	if (! g_initialized)
		wasm_praatlib_init ();
	try {
		std::string script = "Save as text file: \"" + path + "\"\n";
		autostring32 script32 = Melder_8to32_e (script.c_str ());
		praat_executeScriptFromText (script32.get ());
	} catch (MelderError) {
		conststring32 errorMessage = Melder_getError ();
		std::string msg;
		if (errorMessage) {
			autostring8 msg8 = Melder_32to8 (errorMessage);
			msg = std::string (msg8.get ());
		} else {
			msg = "Unknown Praat error";
		}
		Melder_clearError ();
		throw std::runtime_error (msg);
	}
}

void wasm_saveAsBinary (const std::string &path) {
	if (! g_initialized)
		wasm_praatlib_init ();
	try {
		std::string script = "Save as binary file: \"" + path + "\"\n";
		autostring32 script32 = Melder_8to32_e (script.c_str ());
		praat_executeScriptFromText (script32.get ());
	} catch (MelderError) {
		conststring32 errorMessage = Melder_getError ();
		std::string msg;
		if (errorMessage) {
			autostring8 msg8 = Melder_32to8 (errorMessage);
			msg = std::string (msg8.get ());
		} else {
			msg = "Unknown Praat error";
		}
		Melder_clearError ();
		throw std::runtime_error (msg);
	}
}

/*
	Get the last error message from Melder's error buffer.
	Useful when exceptions escape C++ try/catch (Emscripten EH limitation).
*/
std::string wasm_getLastError () {
	conststring32 errorMessage = Melder_getError ();
	if (errorMessage) {
		autostring8 msg8 = Melder_32to8 (errorMessage);
		return std::string (msg8.get ());
	}
	return "";
}

/*
	Clear the Melder error buffer.
*/
void wasm_clearError () {
	Melder_clearError ();
}

/*
	Embind bindings: expose all C++ functions to JavaScript.
*/
EMSCRIPTEN_BINDINGS(praat_wasm) {
	emscripten::function ("praatInit", &wasm_praatlib_init);
	emscripten::function ("executeScript", &wasm_executeScript);
	emscripten::function ("listObjects", &wasm_listObjects);
	emscripten::function ("selectObject", &wasm_selectObject);
	emscripten::function ("removeAllObjects", &wasm_removeAllObjects);
	emscripten::function ("removeSelectedObjects", &wasm_removeSelectedObjects);
	emscripten::function ("readFile", &wasm_readFile);
	emscripten::function ("call", &wasm_call);
	emscripten::function ("saveAsText", &wasm_saveAsText);
	emscripten::function ("saveAsBinary", &wasm_saveAsBinary);
	emscripten::function ("getLastError", &wasm_getLastError);
	emscripten::function ("clearError", &wasm_clearError);
}
