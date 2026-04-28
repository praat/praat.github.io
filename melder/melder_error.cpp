/* melder_error.cpp
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

#include "melder.h"

/* global */ std::mutex theMelder_error_mutex;

static void defaultErrorProc (conststring32 message) {
	MelderConsole::write (str32str (message, U"You interrupted ") ? U"User interrupt: " : U"Error: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}
static void defaultCrashProc (conststring32 message) {
	MelderConsole::write (U"Crashing bug: ", true);
	MelderConsole::write (message, true);
	MelderConsole::write (U"\n", true);
}

static void (*p_theErrorProc) (conststring32 message) = & defaultErrorProc;   // initial setting after start-up; will stay if program is run from batch
static void (*p_theCrashProc) (conststring32 message) = & defaultCrashProc;   // initial setting after start-up; will stay if program is run from batch

void Melder_setErrorProc (void (*p_errorProc) (conststring32)) {
	p_theErrorProc = p_errorProc ? p_errorProc : & defaultErrorProc;
}
void Melder_setCrashProc (void (*p_crashProc) (conststring32)) {
	p_theCrashProc = p_crashProc ? p_crashProc : & defaultCrashProc;
}

constexpr integer BUFFER_LENGTH = 2000;
static char32 theErrorBuffer [BUFFER_LENGTH];   // safe in low-memory situations

void MelderError__appendOneString (conststring32 message) {
	if (! message)
		return;
	const integer length = Melder_length (theErrorBuffer), messageLength = Melder_length (message);
	if (length + messageLength >= BUFFER_LENGTH)
		return;
	str32cpy (theErrorBuffer + length, message);
}

bool Melder_hasError () {
	return theErrorBuffer [0] != U'\0';
}

bool Melder_hasError (conststring32 partialError) {
	return !! str32str (theErrorBuffer, partialError);
}

#define CRASH_SEMAPHORE  U" will crash. Please notify the authors ("

static conststring32 crashMessage () {
	static char32 crashMessageBuffer [1000];
	str32cpy (crashMessageBuffer, Melder_upperCaseAppName());
	str32cat (crashMessageBuffer, CRASH_SEMAPHORE);
	str32cat (crashMessageBuffer, Melder_appContactAddress());
	str32cat (crashMessageBuffer, U") with all of the following information");
	str32cat (crashMessageBuffer, ( Melder_batch ? U"" : U", before closing this window" ));
	str32cat (crashMessageBuffer, U" (and please mention in your email precisely"
			" what you were doing when this crash occurred):\n\n");
	return crashMessageBuffer;
}

bool Melder_hasCrash () {
	const char32 *firstSpace = str32chr (theErrorBuffer, U' ');
	if (! firstSpace)
		return false;
	return Melder_startsWith (firstSpace, CRASH_SEMAPHORE);
}

void Melder_clearError () {
	if (Melder_hasCrash ())
		throw MelderError ();
	theErrorBuffer [0] = U'\0';
}

conststring32 Melder_getError () {
	return & theErrorBuffer [0];
}

void Melder_appendError_noLine (const MelderArg& arg) {
	std::lock_guard lock (theMelder_error_mutex);
	if (Melder_hasError ()) {
		if (Melder_thisThread_getUniqueID () != theMelder_error_threadId)
			return;
	} else {
		theMelder_error_threadId = Melder_thisThread_getUniqueID ();
	}
	MelderError__appendOneString (arg._arg);
}

void Melder_flushError () {
	if (Melder_hasCrash ()) {
		(*p_theCrashProc) (theErrorBuffer);
		abort ();
	} else {
		/*
			"errors" has to be cleared *before* the message is put on the screen.
			This is because on some platforms the message dialog is synchronous
			(Melder_flushError will wait until the message dialog is closed),
			and some operating systems may force an immediate redraw event as soon as
			the message dialog is closed. We want "errors" to be empty when redrawing!
		*/
		static char32 temp [BUFFER_LENGTH];
		str32cpy (temp, theErrorBuffer);
		theErrorBuffer [0] = U'\0';
		(*p_theErrorProc) (temp);
	}
}

static std::mutex theMelder_crash_mutex;   // to guard against simultaneous crashes in multiple threads

void _private_Melder_crash (const MelderArg& arg1,
	const MelderArg& arg2, const MelderArg& arg3, const MelderArg& arg4,
	const MelderArg& arg5, const MelderArg& arg6, const MelderArg& arg7,
	const MelderArg& arg8, const MelderArg& arg9, const MelderArg& arg10,
	const MelderArg& arg11, const MelderArg& arg12, const MelderArg& arg13)
{
	std::lock_guard <std::mutex> lock (theMelder_crash_mutex);
	MelderError__appendOneString (crashMessage ());
	MelderError__appendOneString (arg1. _arg ? arg1. _arg : U"");
	MelderError__appendOneString (arg2. _arg ? arg2. _arg : U"");
	MelderError__appendOneString (arg3. _arg ? arg3. _arg : U"");
	MelderError__appendOneString (arg4. _arg ? arg4. _arg : U"");
	MelderError__appendOneString (arg5. _arg ? arg5. _arg : U"");
	MelderError__appendOneString (arg6. _arg ? arg6. _arg : U"");
	MelderError__appendOneString (arg7. _arg ? arg7. _arg : U"");
	MelderError__appendOneString (arg8. _arg ? arg8. _arg : U"");
	MelderError__appendOneString (arg9. _arg ? arg9. _arg : U"");
	MelderError__appendOneString (arg10._arg ? arg10._arg : U"");
	MelderError__appendOneString (arg11._arg ? arg11._arg : U"");
	MelderError__appendOneString (arg12._arg ? arg12._arg : U"");
	MelderError__appendOneString (arg13._arg ? arg13._arg : U"");
	MelderError__appendOneString (U"\n");
	trace (U"CRASH: ", theErrorBuffer);
	throw MelderError ();
}

static void _assert (const char *pathName, int lineNumber, const char *condition, conststring32 intro) {
	/*
		This function tries to make sure that it allocates no heap memory.
		Hence, character conversion is done in place rather than with Melder_peek8to32_u(),
		and Melder_integer() is also avoided.
	*/
	std::lock_guard <std::mutex> lock (theMelder_crash_mutex);   // to guard against simultaneous crashes in multiple threads
	static char32 pathNameBuffer [1000], conditionBuffer [1000], lineNumberBuffer [40];
	Melder_8to32_inplace_e (pathName, pathNameBuffer, kMelder_textInputEncoding::UTF8);
	const char32 *p_lastFolderSeparator = str32rchr (pathNameBuffer, U'/');
	if (! p_lastFolderSeparator)
		p_lastFolderSeparator = str32rchr (pathNameBuffer, U'\\');
	const conststring32 fileName = ( p_lastFolderSeparator ? p_lastFolderSeparator + 1 : pathNameBuffer );
	Melder_8to32_inplace_e (condition, conditionBuffer, kMelder_textInputEncoding::UTF8);
	static char lineNumberBuffer8 [40];
	snprintf (lineNumberBuffer8,40, "%d", lineNumber);
	Melder_8to32_inplace_e (lineNumberBuffer8, lineNumberBuffer, kMelder_textInputEncoding::UTF8);
	MelderError__appendOneString (crashMessage ());
	MelderError__appendOneString (intro);
	MelderError__appendOneString (Melder_appVersionSTR());
	MelderError__appendOneString (U" in file \"");
	MelderError__appendOneString (fileName);
	MelderError__appendOneString (U"\" at line ");
	MelderError__appendOneString (lineNumberBuffer);
	MelderError__appendOneString (U":\n   ");
	MelderError__appendOneString (conditionBuffer);
	MelderError__appendOneString (U"\n\n");
	trace (U"CRASH: ", theErrorBuffer);
	throw MelderError ();
}

void _private_Melder_assert (const char *pathName, int lineNumber, const char *condition) {
	_assert (pathName, lineNumber, condition, U"Assertion failed in Praat ");
}
void _private_Melder_pre (const char *pathName, int lineNumber, const char *condition) {
	_assert (pathName, lineNumber, condition, U"Precondition failed in Praat ");
}
void _private_Melder_post (const char *pathName, int lineNumber, const char *condition) {
	_assert (pathName, lineNumber, condition, U"Postcondition failed in Praat ");
}

/* End of file melder_error.cpp */
