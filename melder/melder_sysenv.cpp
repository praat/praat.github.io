/* melder_sysenv.cpp
 *
 * Copyright (C) 1992-2007,2011,2012,2015-2019,2023-2026 Paul Boersma
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

#if defined (_WIN32)
	#if ! defined (__CYGWIN__) && ! defined (__MINGW32__)
		#include <crtl.h>
	#endif
	#include <fcntl.h>
	#include <windows.h>
	#include <errno.h>
	#include <stdlib.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
#endif
#include "melder.h"

conststring32 Melder_getenv (conststring32 variableName) {
	#if defined (macintosh) || defined (UNIX) || defined (__MINGW32__) || defined (__CYGWIN__)
		return Melder_peek8to32_u (getenv (Melder_peek32to8 (variableName)));
	#elif defined (_WIN32)
		static char32 buffer [11] [255];
		static int ibuffer = 0;
		if (++ ibuffer == 11)
			ibuffer = 0;
		DWORD n = GetEnvironmentVariableW (variableName, buffer [ibuffer], 255);   BUG
		if (n == ERROR_ENVVAR_NOT_FOUND)
			return nullptr;
		return & buffer [ibuffer] [0];
	#else
		return nullptr;
	#endif
}

/*
	Command lines on Windows (mostly from https://learn.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments?view=msvc-170)

Command-line input		argv[1]		argv[2]		argv[3]		remarks
	"a b c" d e			a b c		d			e			normal
	"ab\"c" "\\" d		ab"c		\			d			escaped "; escaped second \ as it's before "
	a\\\b d"e f"g h		a\\\b		de fg		h			literal backslashes as not followed by "
	a\\\"b c d			a\"b		c			d			escaped second \ as the backslashes are followed by "
	a\\\\"b c" d e		a\\b c		d			e			escaped second and fourth \ as they're followed by ", which is a delimiter
	a"b"" c d			ab" c d								unfinished sequence as closing " is missing
	"back\slash\\"		back\slash\
*/

static autostring32 runAny_STR (
	conststring32 procedureMessageName,
	conststring32 command,   // it's either this one...
	conststring32 executableFileName, integer narg, char32 ** args,   // ... or these three
	bool collectStdout
) {
	if (! command)
		command = U"";
	/*
		Create a pipe for stdout and a pipe for stderr.
	*/
	#if defined (macintosh) || defined (UNIX)
		int stdoutPipe [2], stderrPipe [2];
		if (! Melder_asynchronous) {
			if (collectStdout && pipe (stdoutPipe) == -1)
				Melder_throw (procedureMessageName, U": cannot start system command <<", command, U">> (“pipe error”).");
			if (pipe (stderrPipe) == -1)
				Melder_throw (procedureMessageName, U": cannot start system command <<", command, U">> (“pipe error”).");
		}
	#elif defined (_WIN32)
		HANDLE stdoutReadPipe = NULL, stdoutWritePipe = NULL, stderrReadPipe = NULL, stderrWritePipe = NULL;
		if (! Melder_asynchronous) {
			SECURITY_ATTRIBUTES securityAttributes;
			securityAttributes. nLength = sizeof (SECURITY_ATTRIBUTES);
			securityAttributes. bInheritHandle = TRUE;
			securityAttributes. lpSecurityDescriptor = NULL;
			if (collectStdout && ! CreatePipe (& stdoutReadPipe, & stdoutWritePipe, & securityAttributes, 0))
				Melder_throw (procedureMessageName, U": cannot start system command <<", command, U">> (“pipe error”).");
			if (! CreatePipe (& stderrReadPipe, & stderrWritePipe, & securityAttributes, 0))
				Melder_throw (procedureMessageName, U": cannot start system command <<", command, U">> (“pipe error”).");
		}
	#endif
	/*
		Create a child process that shall run
			/bin/sh with arguments: (1) argv[0] = "sh", (2) argv[1] = "-c", (3) argv[2..n] = command
		or
			cmd.exe /c command
		attaching the stdout and stderr write pipes to the child process.
	*/
	#if defined (macintosh) || defined (UNIX)
		pid_t childProcess = fork ();
		if (childProcess == -1)
			Melder_throw (procedureMessageName, U": cannot start system command <<", command, U">> (“fork error”).");
		if (childProcess == 0) {
			/*
				We are in the child process.
			*/
			if (! Melder_asynchronous) {
				while (collectStdout && (dup2 (stdoutPipe [1], STDOUT_FILENO) == -1) && (errno == EINTR)) {   // attach stdout write pipe to child process
				}
				while ((dup2 (stderrPipe [1], STDERR_FILENO) == -1) && (errno == EINTR)) {   // attach stderr write pipe to child process
				}
				if (collectStdout) {
					close (stdoutPipe [1]);
					close (stdoutPipe [0]);
				}
				close (stderrPipe [1]);
				close (stderrPipe [0]);
			}
			if (executableFileName) {
				autostring8vector args8 (narg + 2);
				args8 [1] = Melder_32to8 (executableFileName);
				for (integer i = 1; i <= narg; i ++) {
					trace (U"Argument ", i, U": <<", args [i], U">>");
					args8 [1 + i] = Melder_32to8 (args [i]);
				}
				args8 [narg + 2] = autostring8();
				execvp (
					Melder_peek32to8 (executableFileName),
					& args8.peek2() [1]
				);
			} else {
				//
				//	From the execl man page:
				//		int execl(const char *path, const char *arg0, ..., /*, (char *)0, */);
				//	With more quotes from the execl man page:
				//
				autostring8 command8 = Melder_32to8 (command);
				execl (
					"/bin/sh",   // "The initial argument for these functions is the pathname of a file which is to be executed."
					// "The const char *arg0 and subsequent ellipses in the execl(), execlp(), and execle() functions"
					// "can be thought of as arg0, arg1, ..., argn.  Together they describe a list of one or more pointers"
					// "to null-terminated strings that represent the argument list available to the executed program."
					"sh",   // "The first argument, by convention, should point to the file name associated with the file being executed."
						// (that is, this is arg0, which should to sh become argv[0], which should generally be the app name)
					"-c",   // (from the bash man page: "If the -c option is present, then commands are read from `string`."; this is argv[1])
					command8.get(),   // (the `string`, combining the complete space-separated command that sh should execute)
					nullptr   // "The list of arguments *must* be terminated by a NULL pointer."
				);   // if all goes right, this implicitly closes the child process
			}
			/*
				If we arrive here, then execl or execvp must have returned,
				which is an error condition.
			*/
			_exit (EXIT_FAILURE);   // close the child process explicitly
		}
		/*
			We are in the parent process.
		*/
	#elif defined (_WIN32)
		STARTUPINFO siStartInfo;
		memset (& siStartInfo, 0, sizeof (siStartInfo));
		siStartInfo. cb = sizeof (siStartInfo);
		if (! Melder_asynchronous) {
			siStartInfo. dwFlags = STARTF_USESTDHANDLES;
			if (collectStdout)
				siStartInfo. hStdOutput = stdoutWritePipe;   // attach stdout write pipe to child process
			else
				siStartInfo. hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);   // TODO: make this work in the Windows console, not just in Cygwin and MSYS
			siStartInfo. hStdError = stderrWritePipe;   // attach stderr write pipe to child process
		}
		PROCESS_INFORMATION piProcInfo;
		memset (& piProcInfo, 0, sizeof (piProcInfo));
		autoMelderString buffer;
		if (executableFileName) {
			MelderString_empty (& buffer);
			MelderString_append (& buffer, U"\"", executableFileName, U"\"");
			for (integer iarg = 1; iarg <= narg; iarg ++) {
				MelderString_append (& buffer, U" \"");
				for (const char32 *p = & args [iarg] [0]; *p != U'\0'; p ++) {
					if (*p == U'"') {
						MelderString_append (& buffer, U"\\\"");
					} else if (*p == U'\\') {
						/*
							Count the backslashes.
						*/
						const char32 *q = p + 1;
						while (*q == '\\')
							q ++;
						const integer numberOfBackslashesInInput = q - p;
						const bool backslashSequenceFollowedByDoubleQuote = ( *q == U'"' );
						const bool backslashSequenceAtEnd = ( *q == U'\0' );
						if (backslashSequenceFollowedByDoubleQuote || backslashSequenceAtEnd)
							MelderString_append (& buffer, U"\\\\");   // double pre-quote and final backslashes
						else
							MelderString_append (& buffer, U"\\");
					} else
						MelderString_appendCharacter (& buffer, *p);
				}
				MelderString_appendCharacter (& buffer, U'"');
			}
			autostringW bufferW = Melder_32toW_fileSystem (buffer.string);
			if (! CreateProcess (nullptr, bufferW.get(), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, & siStartInfo, & piProcInfo))
				Melder_throw (procedureMessageName, U": cannot start subprocess <<", executableFileName, U">>.");
		} else {
			conststring32 comspec = Melder_getenv (U"COMSPEC");   // e.g. "C:\WINDOWS\COMMAND.COM" or "C:\WINNT\windows32\cmd.exe" or "C:\WINDOWS\system32\cmd.exe"
			if (! comspec)
				comspec = Melder_getenv (U"ComSpec");
			if (! comspec)
				comspec = U"cmd.exe";
			MelderString_copy (& buffer, comspec);
			Melder_assert (! str32chr (buffer.string, ' '));
			MelderString_append (& buffer, U" /c ", command);
			autostringW bufferW = Melder_32toW_fileSystem (buffer.string);
			const bool shouldInheritHandles = ! Melder_asynchronous;
			if (! CreateProcess (nullptr, bufferW.get(), nullptr, nullptr, shouldInheritHandles, CREATE_NO_WINDOW, nullptr, nullptr, & siStartInfo, & piProcInfo))
				Melder_throw (procedureMessageName, U": cannot start system command <<", command, U">>.");
		}
	#endif
	/*
		Close the write pipes; no longer should anything be written to them.
	*/
	if (! Melder_asynchronous) {
		#if defined (macintosh) || defined (UNIX)
			if (collectStdout)
				close (stdoutPipe [1]);
			close (stderrPipe [1]);
		#elif defined (_WIN32)
			if (collectStdout)
				CloseHandle (stdoutWritePipe);
			CloseHandle (stderrWritePipe);
		#endif
	}
	/*
		Collect the output of the child process.
	*/
	autoMelderString stdout_string, stderr_string;
	if (! Melder_asynchronous) {
		char buffer8 [1+4096];
		if (collectStdout)
			for (;;) {
				#if defined (macintosh) || defined (UNIX)
					ssize_t count = read (stdoutPipe [0], buffer8, 4096);
				#elif defined (_WIN32)
					DWORD count;
					BOOL success = ReadFile (stdoutReadPipe, buffer8, 4096, & count, NULL);
					Melder_killReturns_inplace (buffer8);
				#endif
				if (count == 0)   // on Windows, this has to go before the success test
					break;
				#if defined (macintosh) || defined (UNIX)
					if (count == -1) {
						if (errno == EINTR)
							continue;
						Melder_throw (procedureMessageName, U": error while handling child process output.");
					}
				#elif defined (_WIN32)
					if (! success)
						Melder_throw (procedureMessageName, U": error while handling child process output.");
				#endif
				buffer8 [count] = '\0';
				MelderString_append (& stdout_string, Melder_peek8to32_u (buffer8));
			}
		for (;;) {
			#if defined (macintosh) || defined (UNIX)
				ssize_t count = read (stderrPipe [0], buffer8, 4096);
			#elif defined (_WIN32)
				DWORD count;
				BOOL success = ReadFile (stderrReadPipe, buffer8, 4096, & count, NULL);
			#endif
			if (count == 0)   // on Windows, this has to go before the success test
				break;
			#if defined (macintosh) || defined (UNIX)
				if (count == -1) {
					if (errno == EINTR)
						continue;
					Melder_throw (procedureMessageName, U": error while handling child process error output.");
				}
			#elif defined (_WIN32)
				if (! success)
					Melder_throw (procedureMessageName, U": error while handling child process error output.");
			#endif
			buffer8 [count] = '\0';
			MelderString_append (& stderr_string, Melder_peek8to32_u (buffer8));
		}
		trace (U"read");
	}
	/*
		Close the read pipes, which are no longer needed.
	*/
	#if defined (macintosh) || defined (UNIX)
		if (collectStdout)
			close (stdoutPipe [0]);
		close (stderrPipe [0]);
	#elif defined (_WIN32)
		if (collectStdout)
			CloseHandle (stdoutReadPipe);
		CloseHandle (stderrReadPipe);
	#endif
	/*
		"Wait" for the child process to finish completely.
	*/
	if (Melder_asynchronous) {
		return autostring32 ();
	} else {
		#if defined (macintosh) || defined (UNIX)
			int childProcessStatus;
			waitpid (childProcess, & childProcessStatus, 0);   // unzombie child process
			if (! WIFEXITED (childProcessStatus))
				Melder_throw (procedureMessageName, U": subprocess not exited (probably stopped by a signal):\n", stderr_string.string);
			if (WEXITSTATUS (childProcessStatus) != 0)
				Melder_throw (stderr_string.string, U".\n", procedureMessageName, U": subprocess exited with error ", WEXITSTATUS (childProcessStatus));
			if (stderr_string.length > 0)
				Melder_casual (procedureMessageName, U": casual message:\n", stderr_string.string);
		#elif defined (_WIN32)
			if (WaitForSingleObject (piProcInfo. hProcess, INFINITE) != 0)
				Melder_throw (procedureMessageName, U": cannot finish system command <<", command, U">>.");
			DWORD exitCode;
			if (! GetExitCodeProcess (piProcInfo. hProcess, & exitCode))
				Melder_throw (procedureMessageName, U": cannot evaluate system command <<", command, U">>.");
			if (exitCode != 0)
				Melder_throw (stderr_string.string, U".\n", procedureMessageName, U": subprocess exited with error ", (int) exitCode);
			CloseHandle (piProcInfo. hProcess);
			CloseHandle (piProcInfo. hThread);
		#endif
		return collectStdout ? Melder_dup (stdout_string.string) : autostring32 ();
	}
}

autostring32 runSystem_STR (conststring32 command) {
	return runAny_STR (U"runSystem$", command, nullptr, 0, nullptr, true);
}
void Melder_runSystem (conststring32 command) {
	(void) runAny_STR (U"runSystem", command, nullptr, 0, nullptr, false);
}
autostring32 runSubprocess_STR (conststring32 executableFileName, integer narg, char32 ** args) {
	return runAny_STR (U"runSubprocess$", nullptr, executableFileName, narg, args, true);
}
void Melder_runSubprocess (conststring32 executableFileName, integer narg, char32 ** args) {
	(void) runAny_STR (U"runSubprocess", nullptr, executableFileName, narg, args, false);
}

/* End of file melder_sysenv.cpp */
