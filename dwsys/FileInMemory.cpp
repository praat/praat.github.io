/* FileInMemory.cpp
 *
 * Copyright (C) 2012-2021 David Weenink, 2017,2024-2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FileInMemory.h"
#include "Strings_.h"
#include <sys/stat.h>

#include "oo_DESTROY.h"
#include "FileInMemory_def.h"
#include "oo_COPY.h"
#include "FileInMemory_def.h"
#include "oo_EQUAL.h"
#include "FileInMemory_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FileInMemory_def.h"
#include "oo_WRITE_TEXT.h"
#include "FileInMemory_def.h"
#include "oo_READ_TEXT.h"
#include "FileInMemory_def.h"
#include "oo_WRITE_BINARY.h"
#include "FileInMemory_def.h"
#include "oo_READ_BINARY.h"
#include "FileInMemory_def.h"
#include "oo_DESCRIPTION.h"
#include "FileInMemory_def.h"

Thing_implement (FileInMemory, Daata, 0);
Thing_implement (FileInMemorySet, SortedSetOfString, 0);

void structFileInMemory :: v1_info () {
	our structDaata :: v1_info ();
	MelderInfo_writeLine (U"File name: ", our string.get());
	MelderInfo_writeLine (U"Number of bytes: ", our d_numberOfBytes);
}

autoFileInMemory FileInMemory_create (MelderFile file) {
	try {
		Melder_require (MelderFile_readable (file),
			U"File is not readable.");
		const integer length = MelderFile_length (file);

		autoFileInMemory me = Thing_new (FileInMemory);
		my string = Melder_dup (MelderFile_peekPath (file));
		my d_numberOfBytes = length;
		my _dontOwnData = false;
		my d_data = newvectorzero <byte> (my d_numberOfBytes + 1);   // includes room for a final null byte in case the file happens to contain only text
		MelderFile_open (file);
		for (integer i = 1; i <= my d_numberOfBytes; i++) {
			const unsigned int number = bingetu8 (file -> filePointer);
			my d_data [i] = number;
		}
		my d_data [my d_numberOfBytes + 1] = 0;   // "extra" null byte
		MelderFile_close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from file ", file, U".");
	}
}

autoFileInMemory FileInMemory_createWithData (integer numberOfBytes, uint8 *data, bool isStaticData, conststring32 path) {
	try {
		autoFileInMemory me = Thing_new (FileInMemory);
		my string = Melder_dup (path);
		my d_numberOfBytes = numberOfBytes;
		if (isStaticData) {
			my _dontOwnData = true;   // we cannot dispose of the data!
			my d_data.cells = data;
			my d_data.size = numberOfBytes + 1;   // ... and the `_capacity` stays at zero!
		} else {
			my _dontOwnData = false;
			my d_data = newvectorraw <unsigned char> (numberOfBytes + 1);
			memcpy (my d_data.asArgumentToFunctionThatExpectsZeroBasedArray(), data, (size_t) numberOfBytes + 1);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemory not created from data.");
	}
}

void FileInMemory_showAsCode (FileInMemory me, conststring32 name, integer numberOfBytesPerLine) {
	Melder_assert (me);
	if (numberOfBytesPerLine < 1)
		numberOfBytesPerLine = 20;

	MelderInfo_write (U"\t\tstatic unsigned char ", name, U"_data[", my d_numberOfBytes+1, U"] = \n\"");
	for (integer i = 1; i <= my d_numberOfBytes; i ++) {
		const unsigned char number = my d_data [i];
		const unsigned char previousNumber = ( i > 1 ? my d_data [i - 1] : '\0' );
		const unsigned char nextNumber = ( i < my d_numberOfBytes ? my d_data [i + 1] : '\0' );
		if (i % numberOfBytesPerLine == 1 && i > 1)
			MelderInfo_write (U"\"\n\"");
		if (number >= 127)
			MelderInfo_write (U"\\", number / 64, (number % 64) / 8, number % 8);
		else if (number >= 32) {
			if (number == '\"')
				MelderInfo_write (U"\\\"");   // two characters: \" ...
			else if (number == '\\')
				MelderInfo_write (U"\\\\");   // two characters: \\ ...
			else if (number == '?') {
				if (previousNumber == '?')   // prevent trigraphs
					MelderInfo_write (U"\\?");   // two characters: \? ...
				else
					MelderInfo_write (U"?");
			} else {
				char32 buffer [2] = { number, U'\0' };
				MelderInfo_write (buffer);
			}
		} else if (number >= 14) {
			if (nextNumber >= '0' && nextNumber <= '7' && i % numberOfBytesPerLine != 0)
				MelderInfo_write (U"\\0", number / 8, number % 8);
			else
				MelderInfo_write (U"\\", number / 8, number % 8);
		} else if (number >= 7) {
			static char32 escapes [7] [3] = { U"\\a", U"\\b", U"\\t", U"\\n", U"\\v", U"\\f", U"\\r" };
			MelderInfo_write (escapes [number - 7]);
		} else {
			if (nextNumber >= '0' && nextNumber <= '7' && i % numberOfBytesPerLine != 0)
				MelderInfo_write (U"\\00", number);
			else
				MelderInfo_write (U"\\", number);
		}
	}
	MelderInfo_writeLine (U"\"\n\t\t;");
	MelderInfo_write (U"\t\tautoFileInMemory ", name, U" = FileInMemory_createWithData (");
	MelderInfo_writeLine (my d_numberOfBytes, U", ", name, U"_data, true, U\"", my string.get(), U"\");");
}

FileInMemory FileInMemory_fopen (FileInMemory me, const char *mode) {
	Melder_assert (mode [0] == 'r');   // i.e. "r" or "rb"; only reading has been implemented; cannot write ('w') or append ('a')
	my d_position = 0;   // after opening, start at the beginning of the file
	my d_errno = 0;
	my d_eof = false;
	my ungetChar = -1;
	my isOpen = true;
	return me;
}

FileInMemory FileInMemorySet_fopen (FileInMemorySet me, const char *fileName, const char *mode) {
	const integer index = my lookUp (Melder_peek8to32_u (fileName));
	if (index == 0) {
		errno = ENOENT;   // "no such file or directory"
		return nullptr;
	}
	return FileInMemory_fopen (my at [index], mode);
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	void rewind ( FILE * stream );

	Set position of stream to the beginning
	Sets the position indicator associated with stream to the beginning of the file.

	The end-of-file and error internal indicators associated to the stream are cleared after a successful call to this function,
	and all effects from previous calls to ungetc on this stream are dropped.

	On streams open for update (read+write), a call to rewind allows to switch between reading and writing.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	none
*/
void FileInMemory_rewind (FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	my d_position = 0;   // this was successful...
	my d_eof = my d_errno = 0;   // ...so we clear the end-of-file indicator and the error indicator
	my ungetChar = -1;   // we also drop any effects from previous calls to `ungetc` on this stream
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fclose ( FILE * stream );

	Close file
	Closes the file associated with the stream and disassociates it.

	All internal buffers associated with the stream are disassociated from it and flushed:
	the content of any unwritten output buffer is written and the content of any unread input buffer is discarded.

	Even if the call fails, the stream passed as parameter will no longer be associated with the file nor its buffers.

	Parameters

	stream
		Pointer to a FILE object that specifies the stream to be closed. 


	Return Value
	If the stream is successfully closed, a zero value is returned.
	On failure, EOF is returned.
*/
int FileInMemory_fclose (FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	my d_position = 0;
	my d_errno = 0;
	my d_eof = false;
	my ungetChar = -1;
	my isOpen = false;
	return 0;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int feof ( FILE * stream );

	Check end-of-file indicator
	Checks whether the end-of-File indicator associated with stream is set, returning a value different from zero if it is.

	This indicator is generally set by a previous operation on the stream that attempted to read at or past the end-of-file.

	Notice that stream's internal position indicator may point to the end-of-file for the next operation,
	but still, the end-of-file indicator may not be set until an operation attempts to read at that point.

	This indicator is cleared by a call to clearerr, rewind, fseek, fsetpos or freopen.
	Although if the position indicator is not repositioned by such a call, the next i/o operation is likely to set the indicator again.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	A non-zero value is returned in the case that the end-of-file indicator associated with the stream is set.
	Otherwise, zero is returned.
*/
int FileInMemory_feof (FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	return my d_eof;
}

int FileInMemory_ferror (FileInMemory stream) {
	return 0;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fseek ( FILE * stream, long int offset, int origin );

	Reposition stream position indicator
	Sets the position indicator associated with the stream to a new position.

	For streams open in binary mode, the new position is defined by adding offset to a reference position specified by origin.

	For streams open in text mode, offset shall either be zero or a value returned by a previous call to ftell,
	and origin shall necessarily be SEEK_SET.

	If the function is called with other values for these arguments,
	support depends on the particular system and library implementation (non-portable).

	The end-of-file internal indicator of the stream is cleared after a successful call to this function,
	and all effects from previous calls to ungetc on this stream are dropped.

	On streams open for update (read+write), a call to fseek allows to switch between reading and writing.

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.
	offset
		Binary files: Number of bytes to offset from origin.
		Text files: Either zero, or a value returned by ftell.
	origin
		Position used as reference for the offset. It is specified by one of the following constants defined in <cstdio> exclusively to be used as arguments for this function:
		Constant	Reference position
		SEEK_SET	Beginning of file
		SEEK_CUR	Current position of the file pointer
		SEEK_END	End of file *
		* Library implementations are allowed to not meaningfully support SEEK_END (therefore, code using it has no real standard portability).


	Return Value
	If successful, the function returns zero.
	Otherwise, it returns non-zero value.
	If a read or write error occurs, the error indicator (ferror) is set.
*/
int FileInMemory_fseek (FileInMemory me, integer offset, int origin) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	my d_errno = 0;   // set the error indicator to nonzero only if there is an error
	integer newPosition = 0;
	if (origin == SEEK_SET)
		newPosition = offset;
	else if (origin == SEEK_CUR)
		newPosition = my d_position + offset;
	else if (origin == SEEK_END)
		newPosition = my d_numberOfBytes + offset;
	else
		Melder_throw (U"FileInMemory_fseek: undefined behaviour (origin ", origin, U").");

	if (newPosition < 0)
		return -1;   // deemed "unsuccessful"

	// deemed "successful"
	my d_position = newPosition;   // even when greater than numberOfBytes
	my ungetChar = -1;   // drop all effects of previous calls to ungetc on this stream
	my d_eof = false;   // successful, so clear the end-of-file indicator
	return 0;
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	long int ftell ( FILE * stream );

	Get current position in stream
	Returns the current value of the position indicator of the stream.

	For binary streams, this is the number of bytes from the beginning of the file.

	For text streams, the numerical value may not be meaningful but can still be used to restore the position to the same position later using fseek
	(if there are characters put back using ungetc still pending of being read, the behavior is undefined).

	Parameters

	stream
		Pointer to a FILE object that identifies the stream.


	Return Value
	On success, the current value of the position indicator is returned.
	On failure, -1L is returned, and errno is set to a system-specific positive value.
*/
integer FileInMemory_ftell (FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	return my d_position;   // FIXME: what about EBDF?
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	char * fgets ( char * str, int num, FILE * stream );

	Get string from stream
	Reads characters from stream and stores them as a C string into str until (num-1) characters have been read
	or either a newline or the end-of-file is reached, whichever happens first.

	A newline character makes fgets stop reading,
	but it is considered a valid character by the function and included in the string copied to str.

	A terminating null character is automatically appended after the characters copied to str.

	Notice that fgets is quite different from gets: not only fgets accepts a stream argument,
	but also allows to specify the maximum size of str and includes in the string any ending newline character.

	Parameters

	str
		Pointer to an array of chars where the string read is copied.
	num
		Maximum number of characters to be copied into str (including the terminating null-character).
	stream
		Pointer to a FILE object that identifies an input stream.
		stdin can be used as argument to read from the standard input.


	Return Value
	On success, the function returns str.
	If the end-of-file is encountered while attempting to read a character, the eof indicator is set (feof).
	If this happens before any characters could be read, the pointer returned is a null pointer (and the contents of str remain unchanged).
	If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned
	(but the contents pointed by str may have changed).
*/
char *FileInMemory_fgets (char *buffer, int bufferSize, FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	if (bufferSize <= 0)
		return nullptr;   // "Undefined Behavior"
	if (bufferSize == 1) {
		buffer [0] = '\0';
		return buffer;   // the usual interpretation of this edge case
	}
	if (my d_eof)   // end-of-file will be encountered while attempting to read a character?
		// then set the end-of-file indicator (but it has already been set)...
		return nullptr;   //... and as no characters could be read, we return a null pointer, leaving the contents of str unchanged
	Melder_assert (bufferSize >= 2);   // so we will be reading at least one character
	integer startingPosition = my d_position;
	if (startingPosition >= my d_numberOfBytes) {   // this includes the case of an empty file
		/*
			We already know that the first character that we will attempt to read lies past the end of the file.
		*/
		my d_eof = true;
		my d_errno = 0;   // this flag is only for other errors than end-of-file, so we don't set it (FIXME: should we indeed clear it?)
		return nullptr;   // as no characters could be read, we return a null pointer, leaving the contents of str unchanged
	}
	Melder_assert (startingPosition < my d_numberOfBytes);
	const integer maximumNumberOfCharactersToCopy = std::min ((integer) bufferSize - 1, my d_numberOfBytes - startingPosition);
	Melder_assert (maximumNumberOfCharactersToCopy >= 1);
	const uint8 *source = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray() + startingPosition;
	if (my ungetChar >= 0) {   // this includes stray null bytes
		buffer [0] = (char) (unsigned char) (unsigned int) my ungetChar;   // guarded sign conversion from int to unsigned int
		my ungetChar = -1;
	} else
		buffer [0] = (char) source [0];
	integer numberOfCharactersCopied = 1;
	if (buffer [0] != '\n')
		while (numberOfCharactersCopied < maximumNumberOfCharactersToCopy) {
			char kar = (char) source [numberOfCharactersCopied];
			buffer [numberOfCharactersCopied] = kar;
			numberOfCharactersCopied ++;
			if (kar == '\n')
				break;
		}
	buffer [numberOfCharactersCopied] = '\0';
	if (buffer [numberOfCharactersCopied - 1] != '\n')
		my d_eof = true;   // this seems to be the somewhat weird behaviour of `fgets` on all platforms checked
	my d_position += numberOfCharactersCopied;
	Melder_assert (my d_position <= my d_numberOfBytes);
	return buffer;   // everything OK
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int fgetc ( FILE * stream );

	Get character from stream
	Returns the character currently pointed by the internal file position indicator of the specified stream.
	The internal file position indicator is then advanced to the next character.

	If the stream is at the end-of-file when called, the function returns EOF and sets the end-of-file indicator for the stream (feof).

	If a read error occurs, the function returns EOF and sets the error indicator for the stream (ferror).

	fgetc and getc are equivalent, except that getc may be implemented as a macro in some libraries.

	Parameters

	stream
		Pointer to a FILE object that identifies an input stream.


	Return Value
	On success, the character read is returned (promoted to an int value).
	The return type is int to accommodate for the special value EOF, which indicates failure:
	If the position indicator was at the end-of-file, the function returns EOF and sets the eof indicator (feof) of stream.
	If some other reading error happens, the function also returns EOF, but sets its error indicator (ferror) instead.
*/
int FileInMemory_fgetc (FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	char kar;
	(void) FileInMemory_fread (& kar, 1, 1, me);
	return FileInMemory_feof (me) ? EOF : (int) (unsigned int) (unsigned char) (kar);
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );

	Read block of data from stream
	Reads an array of count elements, each one with a size of size bytes,
	from the stream and stores them in the block of memory specified by ptr.

	The position indicator of the stream is advanced by the total amount of bytes read.

	The total amount of bytes read if successful is (size*count).

	Parameters

	ptr
		Pointer to a block of memory with a size of at least (size*count) bytes, converted to a void*.
	size
		Size, in bytes, of each element to be read.
		size_t is an unsigned integral type.
	count
		Number of elements, each one with a size of size bytes.
		size_t is an unsigned integral type.
	stream
		Pointer to a FILE object that specifies an input stream.

	Return Value
	The total number of elements successfully read is returned.
	If this number differs from the count parameter, either a reading error occurred or the end-of-file was reached while reading. In
	both cases, the proper indicator is set, which can be checked with ferror and feof, respectively.
	If either size or count is zero, the function returns zero and both the stream state and the content pointed by ptr remain unchanged.
	size_t is an unsigned integral type. 
*/
size_t FileInMemory_fread (void *void_buffer, size_t elementSize_unsigned, size_t numberOfElementsThatShouldBeRead_unsigned, FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	const integer elementSize = uinteger_to_integer_a (elementSize_unsigned);
	const integer numberOfElementsThatShouldBeRead = uinteger_to_integer_a (numberOfElementsThatShouldBeRead_unsigned);
	const double numberOfBytesThatShouldBeRead_real = (double) elementSize * (double) numberOfElementsThatShouldBeRead;
	integer numberOfBytesThatShouldBeRead;
	if (numberOfBytesThatShouldBeRead_real > (double) INTEGER_MAX) {
		my d_eof = true;
		numberOfBytesThatShouldBeRead = INTEGER_MAX;
	} else
		numberOfBytesThatShouldBeRead = elementSize * numberOfElementsThatShouldBeRead;
	if (numberOfBytesThatShouldBeRead == 0)
		return 0;
	const integer numberOfBytesLeftInFile = my d_numberOfBytes - my d_position;
	if (numberOfBytesLeftInFile <= 0) {
		my d_eof = true;
		return 0;
	}
	Melder_assert (numberOfBytesThatShouldBeRead > 0);
	const integer numberOfElementsThatCanBeRead = numberOfBytesLeftInFile / elementSize;   // integer division, rounding down
	const integer numberOfElementsThatWillBeRead = std::min (numberOfElementsThatShouldBeRead, numberOfElementsThatCanBeRead);
	if (numberOfElementsThatWillBeRead < numberOfElementsThatShouldBeRead)
		my d_eof = true;
	#if 0
		const integer numberOfBytesThatWillBeRead = elementSize * numberOfElementsThatWillBeRead;
	#else
		const integer numberOfBytesThatWillBeRead = std::min (numberOfBytesThatShouldBeRead, numberOfBytesLeftInFile);
	#endif
	Melder_assert (numberOfBytesThatWillBeRead <= numberOfBytesLeftInFile);
	if (numberOfBytesThatWillBeRead == 0)
		return 0;

	integer startingPosition = my d_position;
	if (startingPosition >= my d_numberOfBytes) {   // this includes the case of an empty file
		/*
			We already know that the first character that we will attempt to read lies past the end of the file.
		*/
		my d_eof = true;
		my d_errno = 0;   // this flag is only for other errors than end-of-file, so we don't set it (FIXME: should we indeed clear it?)
		return 0;   // as no characters could be read, we return a null pointer, leaving the contents of str unchanged
	}
	Melder_assert (startingPosition < my d_numberOfBytes);
	const uint8 *source = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray() + startingPosition;
	uint8 *buffer = (uint8 *) void_buffer;
	if (my ungetChar >= 0) {   // this includes stray null bytes
		buffer [0] = (char) (unsigned char) (unsigned int) my ungetChar;   // guarded sign conversion from int to unsigned int
		my ungetChar = -1;
	} else
		buffer [0] = (char) source [0];
	integer numberOfBytesCopied = 1;
	while (numberOfBytesCopied < numberOfBytesThatWillBeRead) {
		char kar = (char) source [numberOfBytesCopied];
		buffer [numberOfBytesCopied] = kar;
		numberOfBytesCopied ++;
	}
	my d_position += numberOfBytesCopied;
	Melder_assert (my d_position <= my d_numberOfBytes);
	return integer_to_uinteger_a (numberOfElementsThatWillBeRead);
}

/*
	From http://www.cplusplus.com/reference/cstdio 20171028
	int ungetc ( int character, FILE * stream );

	Unget character from stream
	A character is virtually put back into an input stream, decreasing its internal file position as if a previous getc operation was undone.

	This character may or may not be the one read from the stream in the preceding input operation. In any case, the next character retrieved from stream is the character passed to this function, independently of the original one.

	Notice though, that this only affects further input operations on that stream, and not the content of the physical file associated with it, which is not modified by any calls to this function.

	Some library implementations may support this function to be called multiple times, making the characters available in the reverse order in which they were put back. Although this behavior has no standard portability guarantees, and further calls may simply fail after any number of calls beyond the first.

	If successful, the function clears the end-of-file indicator of stream (if it was currently set), and decrements its internal file position indicator if it operates in binary mode; In text mode, the position indicator has unspecified value until all characters put back with ungetc have been read or discarded.

	A call to fseek, fsetpos or rewind on stream will discard any characters previously put back into it with this function.

	If the argument passed for the character parameter is EOF, the operation fails and the input stream remains unchanged.

	Parameters

	character
		The int promotion of the character to be put back.
		The value is internally converted to an unsigned char when put back.
	stream
		Pointer to a FILE object that identifies an input stream.


	Return Value
	On success, the character put back is returned.
	If the operation fails, EOF is returned.
*/

int FileInMemory_ungetc (int character, FileInMemory me) {
	Melder_assert (me);
	Melder_assert (my isOpen);
	int result = EOF;
	if (character != EOF) {
		-- my d_position;
		result = my ungetChar = character;
	}
	return result;
}

int FileInMemorySet_stat (FileInMemorySet me, const char *path, struct stat *buf) {
	conststring32 path32 = Melder_peek8to32_u (path);
	integer position = my lookUp (path32);
	if (position > 0) {
		buf -> st_mode = S_IFREG;
		buf -> st_size = my at [position] -> d_numberOfBytes;
		return 0;
	}
	if (FileInMemorySet_hasDirectory (me, path32)) {
		buf -> st_mode = S_IFDIR;
		buf -> st_size = 0;
	}
	errno = ENOENT;
	return 1;
}

void structFileInMemorySet :: v1_info () {
	MelderInfo_writeLine (U"Number of files: ", size);
	MelderInfo_writeLine (U"Total number of bytes: ", FileInMemorySet_getTotalNumberOfBytes (this));
}

integer FileInMemorySet_getTotalNumberOfBytes (FileInMemorySet me) {
	integer numberOfBytes = 0;
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = my at [ifile];
		numberOfBytes += fim -> d_numberOfBytes;
	}
	return numberOfBytes;
}

autoFileInMemorySet FileInMemorySets_merge (OrderedOf<structFileInMemorySet>& list) {
	try {
		autoFileInMemorySet thee = Data_copy (list.at [1]);
		for (integer iset = 1; iset <= list.size; iset ++)
			thy merge (list.at [iset]);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySets not merged.");
	}
}

autoFileInMemorySet FileInMemorySet_createFromDirectoryContents (conststring32 dirpath, conststring32 fileGlobber) {
	try {
		structMelderFolder parent { };
		Melder_pathToFolder (dirpath, & parent);   // not Melder_relativePathToFolder! Because if dirpath is relative, so should be the fims
		autoStrings thee = Strings_createAsFileList (Melder_cat (dirpath, U"/", fileGlobber));
		Melder_require (thy numberOfStrings > 0,
			U"No files found.");

		autoFileInMemorySet me = FileInMemorySet_create ();
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			structMelderFile file { };
			//MelderFolder_getFile (& parent, thy strings [i].get(), & file);   // this would convert forward to backward slashes on Windows
			conststring32 filePath = Melder_cat (dirpath, U"/", thy strings [i].get());   // this conserves forward slashes, even on Windows
			Melder_pathToFile (filePath, & file);   // this conserves forward slashes, even on Windows
			my addItem_move (FileInMemory_create (& file));
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet not created from directory \"", dirpath, U"\" for files that match \"", fileGlobber, U"\".");
	}
}

autoFileInMemorySet FilesInMemory_to_FileInMemorySet (OrderedOf<structFileInMemory>& list) {
	try {
		autoFileInMemorySet thee = FileInMemorySet_create ();
		for (integer ifile = 1; ifile <= list.size; ifile ++)
			thy addItem_move (Data_copy (list.at [ifile]));
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FilesInMemory not collected in FileInMemorySet.");
	}
}

autoFileInMemorySet FileInMemorySet_extractFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
				thy addItem_move (Data_copy (fim));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

autoFileInMemorySet FileInMemorySet_removeFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
				thy addItem_move (my subtractItem_move (ifile));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

autoFileInMemorySet FileInMemorySet_listFiles (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	try {
		autoFileInMemorySet thee = Thing_new (FileInMemorySet);
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
				thy addItem_ref (fim);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract files.");
	}
}

void FileInMemorySet_showAsCode (FileInMemorySet me, conststring32 functionName, integer numberOfBytesPerLine) {
	autoMelderString fimName;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"\n");
	MelderInfo_writeLine (U"void ", functionName, U" (FileInMemorySet me) {");
	MelderInfo_writeLine (U"\ttry {");
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = my at [ifile];
		MelderString_copy (& fimName, U"fim", ifile);
		FileInMemory_showAsCode (fim, fimName.string, numberOfBytesPerLine);
		MelderInfo_writeLine (U"\t\tmy addItem_move (", fimName.string, U".move());\n");
	}
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"Not everything was added to the FileInMemorySet.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n");
}

void FileInMemorySet_showOneFileAsCode (FileInMemorySet me, integer index, conststring32 name, integer numberOfBytesPerLine) {
	if (index < 1 || index > my size)
		return;
	MelderInfo_writeLine (U"#include \"FileInMemory.h\"\n");
	MelderInfo_writeLine (U"static autoFileInMemory create_new_object () {");
	MelderInfo_writeLine (U"\ttry {");
	autoMelderString one_fim;
	const FileInMemory fim = my at [index];
	MelderString_append (& one_fim, name, index);
	FileInMemory_showAsCode (fim, U"me", numberOfBytesPerLine);
	MelderInfo_writeLine (U"\t\treturn me;");
	MelderInfo_writeLine (U"\t} catch (MelderError) {");
	MelderInfo_writeLine (U"\t\tMelder_throw (U\"FileInMemory not created.\");");
	MelderInfo_writeLine (U"\t}");
	MelderInfo_writeLine (U"}\n\n");
	MelderInfo_writeLine (U"autoFileInMemory ", name, U" = create_new_object ();");
}

integer FileInMemorySet_findNumberOfMatches_path (FileInMemorySet me, kMelder_string which, conststring32 criterion) {
	integer numberOfMatches = 0;
	for (integer ifile = 1; ifile <= my size; ifile ++) {
		const FileInMemory fim = my at [ifile];
		if (Melder_stringMatchesCriterion (fim -> string.get(), which, criterion, true))
			numberOfMatches ++;
	}
	return numberOfMatches;
}

bool FileInMemorySet_hasDirectory (FileInMemorySet me, conststring32 name) {
	autoMelderString searchString;
	MelderString_append (& searchString, U"/", name, U"/");
	for (integer i = 1; i <= my size; i ++) {
		const FileInMemory fim = my at [i];
		if (str32str (fim -> string.get(), searchString.string))
			return true;
	}
	return false;
}

autoStrings FileInMemorySet_to_Strings_path (FileInMemorySet me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my size);
		thy numberOfStrings = 0;
		for (integer ifile = 1; ifile <= my size; ifile ++) {
			const FileInMemory fim = my at [ifile];
			thy strings [ifile] = Melder_dup_f (fim -> string.get());
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Strings created from FilesinMemory.");
	}
}

/*
	The following test is meant to be called only from `test/dwsys/FileInMemory.praat`,
	because that is where the subfolder `examples/FileInMemory` resides,
	which contains the files that are tested.

	If the files aren't there yet, they can be created by setting `WRITE` to true
	(writing is not entirely trivial, because the files may contain null bytes
	and high bytes, all of which may be difficult to type from a text editor.
	Note that `WRITE` should normally be false, for security reasons.

	For the content, we use char8 instead of char32,
	because otherwise we would have to use `Melder_peek32to8`, which on Windows would convert newlines.
*/

static conststring32 theTestSubfolder = U"examples/FileInMemory";   // constant
static autoFileInMemorySet theTestFileInMemorySet;   // singleton

static structMelderFile theTestFile;  // we make this "global" in order to still be able to do things with it after running `testOneFile`...
static FILE *theTestFilePointer;   // ... and this as well...
static FileInMemory theTestFim;   // ... and this as well...

static void testOneFile (
	conststring32 fileName,   // usually relative to test/dwsys
	const char content [40],   // this is not a string, because it can contain null bytes
	const integer length,   // this is the length of `contents` (note that this cannot be determined by `strlen` if there are null bytes)
	const char lineContents [5][40],   // the intended contents of the (maximally) five lines
	const integer lineLengths [5],   // the intended lengths of the (maximally) five lines
	const bool endsInNewline
) {
	try {
		static MelderString relativePath;
		MelderString_copy (& relativePath, Melder_cat (theTestSubfolder, U"/", fileName));
		Melder_relativePathToFile (relativePath.string, & theTestFile);

		constexpr bool WRITE = false;   // this should normally be false!!
		if constexpr (WRITE) {
			Melder_warning (
				U"YOU ARE USING AN UNSAFE VERSION OF PRAAT, "
				U"which is meant to be used only by Praat’s authors.\n"
				U"If you are not a Praat author (but instead a user or maintainer), "
				U"please notify the authors at paul.boersma@uva.nl, "
				U"telling them to “set WRITE to false in the source code of the FileInMemory test”.\n\n"
				U"If you proceed to click OK, Praat will create the file ", & theTestFile, U". ",
				U"If you don’t want this, force-quit Praat now."
			);   // FIXME: to be replaced with permission window
			FILE *fout = Melder_fopen (& theTestFile, "wb");
			fwrite (content, 1, (size_t) length, fout);
			Melder_fclose (& theTestFile, fout);
		}
		/*
			Initialize.
		*/
		errno = 0;
		integer numberOfLines = 0, numberOfBytes = 0;
		while (lineLengths [numberOfLines] > 0)   // no lines of length 0 can exist (there's always at least a newline character)
			numberOfBytes += lineLengths [numberOfLines ++];
		MelderInfo_writeLine (U"Testing file \"", fileName, U"\" with ", numberOfLines, U" lines and ", numberOfBytes, U" bytes.");
		if (theTestFilePointer) {
			Melder_fclose (& theTestFile, theTestFilePointer);
			theTestFilePointer = nullptr;
		}
		if (theTestFim) {
			FileInMemory_fclose (theTestFim);
			theTestFim = nullptr;
		}

		/*
			fopen should return NULL if the file doesn't exist.
			Full paths should not work:
		*/
		MelderInfo_writeLine (U"Testing fopen...");
		MelderInfo_writeLine (U"\tTrying to open file-in-memory via full path:\n\t\t\"", MelderFile_peekPath (& theTestFile), U"\"...");
		theTestFim = FileInMemorySet_fopen (theTestFileInMemorySet.get(), MelderFile_peekPath8 (& theTestFile), "rb");
		Melder_require (theTestFim == nullptr,
			U"FileInMemory should have been null.");
		/*
			If the returned file is NULL, errno should contain a relevant message.
		*/
		Melder_require (errno == ENOENT,
			U"It should have been reported that the file ", & theTestFile, U" does not exist.");
		MelderInfo_writeLine (U"\t\t\t... not opened");
		/*
			Raw file names should not work:
		*/
		MelderInfo_writeLine (U"\tTrying to open file-in-memory via raw file name:\n\t\t\"", fileName, U"\"...");
		theTestFim = FileInMemorySet_fopen (theTestFileInMemorySet.get(), Melder_peek32to8_fileSystem (fileName), "rb");
		Melder_require (theTestFim == nullptr,
			U"FileInMemory should still have been null.");
		/*
			If the returned file is NULL, errno should contain a relevant message.
		 */
		Melder_require (errno == ENOENT,
			U"It should still have been reported that the file ", fileName, U" does not exist.");
		MelderInfo_writeLine (U"\t\t\t... not opened");
		/*
			fopen should return a valid pointer if the file does exist.
			Relative paths that were used for creating the FileInMemorySet should work:
		*/
		MelderInfo_writeLine (U"\tTrying to open file-in-memory via relative path:\n\t\t\"", relativePath.string, U"\"...");
		theTestFilePointer = Melder_fopen (& theTestFile, "rb");
		Melder_require (theTestFilePointer != nullptr,
			U"FILE* should exist.");
		theTestFim = FileInMemorySet_fopen (theTestFileInMemorySet.get(), Melder_peek32to8_fileSystem (relativePath.string), "rb");
		Melder_require (theTestFim != nullptr,
			U"FileInMemory should exist.");
		MelderInfo_writeLine (U"\t\t\t... opened");
		/*
			fopen should never clear errno.
		*/
		Melder_require (errno == ENOENT,
			U"The old error number should still exist.");

		/*
			Now that both files are open,
			we can measure their sizes with `fseek` and `fgets`.
		*/
		MelderInfo_writeLine (U"Testing fseek, ftell, feof...");
		fseek (theTestFilePointer, 0, SEEK_END);
		Melder_require (ftell (theTestFilePointer) == length,
			U"The file should have ", length, U" bytes, not ", ftell (theTestFilePointer));
		FileInMemory_fseek (theTestFim, 0, SEEK_END);
		Melder_require (FileInMemory_ftell (theTestFim) == length,
			U"The file-in-memory should have ", length, U" bytes, not ", FileInMemory_ftell (theTestFim));

		/*
			Now that both files are open,
			we can compare their contents by `fgets`.
		*/
		MelderInfo_writeLine (U"Testing fgets, rewind, ftell, feof...");
		rewind (theTestFilePointer);
		FileInMemory_rewind (theTestFim);
		constexpr integer bufferSize = 200;
		char fbuffer [bufferSize], fimbuffer [bufferSize], *fline, *fimline;
		integer intendedPosition = 0;
		for (integer iline = 1; iline <= numberOfLines; iline ++) {
			Melder_assert (iline-1 >= 0);   // bounds-checking
			const char *lineContent = lineContents [iline-1];
			const integer lineLength = lineLengths [iline-1];
			fline = fgets (fbuffer, bufferSize, theTestFilePointer);
			fimline = FileInMemory_fgets (fimbuffer, bufferSize, theTestFim);
			Melder_require (!! fline,
				U"File-line ", iline, U" should exist.");
			Melder_require (!! fimline,
				U"Fim-line ", iline, U" should exist.");
			Melder_require (fline [lineLength] == '\0',
				U"File-line ", iline, U" should have a null byte after it, not ", (integer) fline [lineLength], U".");
			Melder_require (fimline [lineLength] == '\0',
				U"Fm-line ", iline, U" should have a null byte after it, not ", (integer) fimline [lineLength], U".");
			Melder_require (memcmp (fline, lineContent, (size_t) lineLength) == 0,   // not strcmp, because of potential null bytes
				U"File-line ", iline, U" (\"", Melder_peek8to32_u (fline), U"\") should have been \"", Melder_peek8to32_u (lineContent), U"\".");
			Melder_require (memcmp (fimline, lineContent, (size_t) lineLength) == 0,   // not strcmp, because of potential null bytes
				U"Fim-line ", iline, U" (\"", Melder_peek8to32_u (fimline), U"\") should have been \"", Melder_peek8to32_u (lineContent), U"\".");
			intendedPosition += lineLength;
			integer fpos = ftell (theTestFilePointer);
			integer fimpos = FileInMemory_ftell (theTestFim);
			Melder_require (fpos == intendedPosition,
				U"After line ", iline, U" the file should be at position ", intendedPosition, U".");
			Melder_require (fimpos == intendedPosition,
				U"After line ", iline, U" the file-in-memory should be at position ", intendedPosition, U", not ", fimpos, U".");
			if (iline == numberOfLines) {
				/*
					We are at the end of the files, but no end-of-file indicator should have been set,
					because we haven't attempted to read *beyond* the end.

					Well, this turns out to be true only if there was no final newline character (who can document this behaviour?).
				*/
				Melder_assert (lineLength-1 >= 0);   // bounds-checking
				if (endsInNewline) {
					Melder_require (feof (theTestFilePointer) == 0,
						U"At the end of the file, we haven't yet read beyond it.");
					Melder_require (FileInMemory_feof (theTestFim) == 0,
						U"At the end of the file-in-memory, we haven't yet read beyond it.");
					/*
						`fgets` should have included the last newline character.
					*/
					Melder_assert (fline [lineLength-1] == '\n');
					Melder_require (fimline [lineLength-1] == '\n',
						U"The last character should be a newline.");
				} else {
					/*
						`fgets` has not included any trailing newline character,
						and apparently this should lead to an end-of-file indication.
					*/
					Melder_require (fline [lineLength-1] != '\n',
						U"The last character of the file should not be a newline.");
					Melder_require (fimline [lineLength-1] != '\n',
						U"The last character of the file-in-memory should not be a newline.");
					Melder_require (feof (theTestFilePointer) != 0,
						U"At the end of the file, we haven't yet read beyond it.");
					Melder_require (FileInMemory_feof (theTestFim) != 0,
						U"At the end of the file-in-memory, we haven't yet read beyond it.");
				}
			}
		}
		/*
			But now we read beyond the end of file, switching on the marker.
		*/
		fline = fgets (fbuffer, bufferSize, theTestFilePointer);
		fimline = FileInMemory_fgets (fimbuffer, bufferSize, theTestFim);
		Melder_require (! fline,
			U"File-line ", numberOfLines + 1, U" should not exist.");
		Melder_require (! fimline,
			U"Fim-line ", numberOfLines + 1, U" should not exist.");
		Melder_require (feof (theTestFilePointer) != 0,
			U"Past the end of the file, the end-of-file indicator should have been set.");
		Melder_require (FileInMemory_feof (theTestFim) != 0,
			U"Past the end of the file-in-memory, the end-of-file indicator should have been set.");

		/*
			Rewinding to the start of the files should clear their end-of-file markers.
		*/
		MelderInfo_writeLine (U"Testing rewind, ftell, feof...");
		rewind (theTestFilePointer);
		FileInMemory_rewind (theTestFim);
		Melder_assert (ftell (theTestFilePointer) == 0);
		Melder_assert (FileInMemory_ftell (theTestFim) == 0);
		Melder_assert (feof (theTestFilePointer) == 0);
		Melder_assert (FileInMemory_feof (theTestFim) == 0);

		/*
			Testing fread.
		*/
		MelderInfo_writeLine (U"Testing fread, ftell, feof...");
		rewind (theTestFilePointer);
		FileInMemory_rewind (theTestFim);
		//
		integer size = 1, count = 14;
		size_t fnread = fread (fbuffer, (size_t) size, (size_t) count, theTestFilePointer);
		size_t fimnread = FileInMemory_fread (fimbuffer, (size_t) size, (size_t) count, theTestFim);
		MelderInfo_writeLine (U"\tRead ", fnread, U" and ", fimnread, U" elements of size ", size, U".");
		integer expectedNumberOfElementsRead = std::min (count, numberOfBytes / size);
		integer expectedNumberOfBytesRead = size * expectedNumberOfElementsRead;
		Melder_require ((integer) fnread == expectedNumberOfElementsRead,
			U"Expected to read ", expectedNumberOfElementsRead, U" elements from file, not ", fnread, U".");
		Melder_require ((integer) fimnread == expectedNumberOfElementsRead,
			U"Expected to read ", expectedNumberOfElementsRead, U" elements from file-in-memory, not ", fnread, U".");
		Melder_require (ftell (theTestFilePointer) == expectedNumberOfBytesRead,
			U"Expected file position ", expectedNumberOfBytesRead, U", not ", ftell (theTestFilePointer), U".");
		Melder_require (FileInMemory_ftell (theTestFim) == expectedNumberOfBytesRead,
			U"Expected file-in-memory position ", expectedNumberOfBytesRead, U", not ", FileInMemory_ftell (theTestFim), U".");
		if (expectedNumberOfElementsRead < count) {
			Melder_require (feof (theTestFilePointer),
				U"fread should have its end-of-file indicator set (size ", size, U", count ", count, U").");
			Melder_require (FileInMemory_feof (theTestFim),
				U"FileInMemory_fread should have its end-of-file indicator set (size ", size, U", count ", count, U").");
		} else {
			Melder_require (! feof (theTestFilePointer),
				U"fread should not have its end-of-file indicator set (size ", size, U", count ", count, U").");
			Melder_require (! FileInMemory_feof (theTestFim),
				U"FileInMemory_fread should not have its end-of-file indicator set (size ", size, U", count ", count, U").");
		}
		//
		size = 1;
		count = 195;
		fnread = fread (fbuffer, (size_t) size, (size_t) count, theTestFilePointer);
		fimnread = FileInMemory_fread (fimbuffer, (size_t) size, (size_t) count, theTestFim);
		MelderInfo_writeLine (U"Read ", fnread, U" and ", fimnread, U" elements of size ", size, U".");
		Melder_require (ftell (theTestFilePointer) == numberOfBytes,
			U"Expected file position ", expectedNumberOfBytesRead, U", not ", ftell (theTestFilePointer), U".");
		Melder_require (FileInMemory_ftell (theTestFim) == numberOfBytes,
			U"Expected file-in-memory position ", expectedNumberOfBytesRead, U", not ", FileInMemory_ftell (theTestFim), U".");
		Melder_require (feof (theTestFilePointer),
			U"fread should have its end-of-file indicator set after trying to read more characters than there are in the file.");
		Melder_require (FileInMemory_feof (theTestFim),
			U"FileInMemory_fread should have its end-of-file indicator set after trying to read more characters than there are in the file-in-memory.");

		rewind (theTestFilePointer);
		FileInMemory_rewind (theTestFim);

		fnread = fread (fbuffer, 1, (size_t) numberOfBytes, theTestFilePointer);
		fimnread = FileInMemory_fread (fimbuffer, 1, (size_t) numberOfBytes, theTestFim);
		MelderInfo_writeLine (U"Read ", fnread, U" and ", fimnread, U" bytes");
		Melder_require (ftell (theTestFilePointer) == numberOfBytes,
			U"fread: at the end of the file we should be at position ", numberOfBytes, U", not ", FileInMemory_ftell (theTestFim));
		Melder_require (FileInMemory_ftell (theTestFim) == numberOfBytes,
			U"FileInMemory_fread: at the end of the file-in-memory we should be at position ", numberOfBytes, U", not ", FileInMemory_ftell (theTestFim));
		Melder_require (! feof (theTestFilePointer),
			U"fread should not have its end-of-file indicator set after reading precisely the content of the file.");
		Melder_require (! FileInMemory_feof (theTestFim),
			U"FileInMemory_fread should not have its end-of-file indicator set after reading precisely the content of the file-in-memory.");

		rewind (theTestFilePointer);
		FileInMemory_rewind (theTestFim);
		Melder_require (! FileInMemory_feof (theTestFim),
			U"FileInMemory_fread should not have its end-of-file indicator set just after rewinding.");

		size = 7;
		count = 2;
		fnread = fread (fbuffer, (size_t) size, (size_t) count, theTestFilePointer);
		fimnread = FileInMemory_fread (fimbuffer, (size_t) size, (size_t) count, theTestFim);
		MelderInfo_writeLine (U"Read ", fnread, U" and ", fimnread, U" elements of size ", size, U".");
		expectedNumberOfElementsRead = std::min (count, numberOfBytes / size);
		//expectedNumberOfBytesRead = size * expectedNumberOfElementsRead;
		expectedNumberOfBytesRead = std::min (count * size, numberOfBytes);
		Melder_require ((integer) fnread == expectedNumberOfElementsRead,
			U"Expected to read ", expectedNumberOfElementsRead, U" elements from file, not ", fnread, U".");
		Melder_require ((integer) fimnread == expectedNumberOfElementsRead,
			U"Expected to read ", expectedNumberOfElementsRead, U" elements from file-in-memory, not ", fnread, U".");
		Melder_require (ftell (theTestFilePointer) == expectedNumberOfBytesRead,
			U"Expected file position ", expectedNumberOfBytesRead, U", not ", ftell (theTestFilePointer), U".");
		Melder_require (FileInMemory_ftell (theTestFim) == expectedNumberOfBytesRead,
			U"Expected file-in-memory position ", expectedNumberOfBytesRead, U", not ", FileInMemory_ftell (theTestFim), U".");
		if (expectedNumberOfElementsRead < count) {
			Melder_require (feof (theTestFilePointer),
				U"fread should have its end-of-file indicator set (size ", size, U", count ", count, U").");
			Melder_require (FileInMemory_feof (theTestFim),
				U"FileInMemory_fread should have its end-of-file indicator set (size ", size, U", count ", count, U").");
		} else {
			Melder_require (! feof (theTestFilePointer),
				U"fread should not have its end-of-file indicator set (size ", size, U", count ", count, U").");
			Melder_require (! FileInMemory_feof (theTestFim),
				U"FileInMemory_fread should not have its end-of-file indicator set (size ", size, U", count ", count, U").");
		}

		/*
			Testing fgetc.
		*/
		MelderInfo_writeLine (U"Testing fgetc...");
		rewind (theTestFilePointer);
		FileInMemory_rewind (theTestFim);
		Melder_assert (FileInMemory_ftell (theTestFim) == 0);
		Melder_assert (FileInMemory_feof (theTestFim) == 0);
		for (integer i = 1; i <= numberOfBytes; i ++) {
			int fkar = fgetc (theTestFilePointer);
			Melder_require (fkar == content [i-1],
				U"fgetc: character #", i, U" should be ", content [i-1], U" but is ", fkar, U".");
			Melder_require (! feof (theTestFilePointer),
				U"fgetc: we should not be past the end yet.");
			int fimkar = FileInMemory_fgetc (theTestFim);
			Melder_require (fimkar == content [i-1],
				U"FileInMemory_fgetc: character #", i, U" should be ", content [i-1], U" but is ", fimkar, U".");
			Melder_require (! FileInMemory_feof (theTestFim),
				U"FileInMemory_fgetc: we should not be past the end yet.");
		}
		int fkar = fgetc (theTestFilePointer);
		Melder_require (fkar == EOF,
			U"fgetc: character should be EOF (-1) but is ", fkar, U".");
		Melder_require (feof (theTestFilePointer),
			U"fgetc: we should be past the end now.");
		int fimkar = FileInMemory_fgetc (theTestFim);
		Melder_require (fimkar == EOF,
			U"FIM_fgetc: character should be EOF (-1) but is ", fimkar, U".");
		Melder_require (FileInMemory_feof (theTestFim),
			U"FIM_fgetc: we should not be past the end yet.");

		/*
			Testing fseek.
		*/
		MelderInfo_writeLine (U"Testing fseek, ftell...");
		//
		int fresult = fseek (theTestFilePointer, 0, SEEK_SET);
		long_not_integer fposition = ftell (theTestFilePointer);
		Melder_require (fresult == 0,
			U"fseek to position 0: result should be OK: the start of the file.");
		Melder_require (fposition == 0,
			U"fseek to position 0: position should be 0 (at the start of the file), not ", fposition, U".");
		int fimresult = FileInMemory_fseek (theTestFim, 0, SEEK_SET);
		long_not_integer fimposition = FileInMemory_ftell (theTestFim);
		Melder_require (fimresult == 0,
			U"FIM_fseek to position 0: result should be OK: the start of the file.");
		Melder_require (fimposition == 0,
			U"FIM_fseek to position 0: position should be 0 (at the start of the file), not ", fimposition, U".");
		//
		fresult = fseek (theTestFilePointer, numberOfBytes, SEEK_SET);
		fposition = ftell (theTestFilePointer);
		Melder_require (fresult == 0,
			U"fseek to the last position: result should be OK: the end of the file.");
		Melder_require (fposition == numberOfBytes,
			U"fseek to the last position: position should be ", numberOfBytes, U" (at the end of the file), not ", fposition, U".");
		fimresult = FileInMemory_fseek (theTestFim, numberOfBytes, SEEK_SET);
		fimposition = FileInMemory_ftell (theTestFim);
		Melder_require (fimresult == 0,
			U"FIM_fseek to the last position: result should be OK: the end of the file.");
		Melder_require (fimposition == numberOfBytes,
			U"FIM_fseek to the last position: position should be ", numberOfBytes, U" (at the end of the file), not ", fimposition, U".");
		//
		fresult = fseek (theTestFilePointer, 14, SEEK_SET);
		fposition = ftell (theTestFilePointer);
		if (numberOfBytes >= 14) {
			Melder_require (fresult == 0,
				U"fseek to position 14: result should be OK.");
			Melder_require (fposition == 14,
				U"fseek to position 14: position should be 14.");
		} else {
			Melder_require (fresult == 0,
				U"fseek to position 14: result should be OK, even after seeking past the end.");
			Melder_require (fposition == 14,
				U"fseek to position 14: position should be at 14, although that is past the end of the file.");
		}
		fimresult = FileInMemory_fseek (theTestFim, 14, SEEK_SET);
		fimposition = FileInMemory_ftell (theTestFim);
		if (numberOfBytes >= 14) {
			Melder_require (fimresult == 0,
				U"FileInMemory_fseek to position 14: result should be OK.");
			Melder_require (fimposition == 14,
				U"FileInMemory_fseek to position 14: position should be 14.");
		} else {
			Melder_require (fimresult == 0,
				U"FileInMemory_fseek to position 14: result should be OK, even after seeking past the end.");
			Melder_require (fimposition == 14,
				U"FileInMemory_fseek to position 14: position should be at 14, although that is past the end of the file.");
		}
		//
		fresult = fseek (theTestFilePointer, 1000, SEEK_SET);
		fposition = ftell (theTestFilePointer);
		Melder_require (fresult == 0,
			U"fseek to position 1000: result should be OK, even after seeking past the end.");
		Melder_require (fposition == 1000,
			U"fseek to position 1000: position should be at 1000, although that is past the end of the file.");
		fimresult = FileInMemory_fseek (theTestFim, 1000, SEEK_SET);
		fimposition = FileInMemory_ftell (theTestFim);
		Melder_require (fimresult == 0,
			U"FileInMemory_fseek to position 1000: result should be OK, even after seeking past the end.");
		Melder_require (fimposition == 1000,
			U"FileInMemory_fseek to position 1000: position should be at 1000, although that is past the end of the file.");
		/*
			With a negative position argument, the internal position should not change.
		*/
		fresult = fseek (theTestFilePointer, -1000, SEEK_SET);
		fposition = ftell (theTestFilePointer);
		Melder_require (fresult == -1,
			U"fseek to position -1000: result should be failure.");
		Melder_require (fposition == 1000,
			U"fseek to position -1000: position is at ", fposition, U" instead of still 1000.");
		Melder_require (! ferror (theTestFilePointer),
			U"fseek to position -1000 should not be an error.");
		fimresult = FileInMemory_fseek (theTestFim, -1000, SEEK_SET);
		fimposition = FileInMemory_ftell (theTestFim);
		Melder_require (fimresult == -1,
			U"FileInMemory_fseek to position -1000: result should be failure.");
		Melder_require (fimposition == 1000,
			U"FileInMemory_fseek to position -1000: position is at ", fimposition, U" instead of still 1000.");
		Melder_require (! FileInMemory_ferror (theTestFim),
			U"FileInMemory_fseek to position -1000 should not be an error.");
		//
		fresult = fseek (theTestFilePointer, -1, SEEK_SET);
		fposition = ftell (theTestFilePointer);
		Melder_require (fresult == -1,
			U"fseek to position -1: result should be failure.");
		Melder_require (fposition == 1000,
			U"fseek to position -1: position is at ", fposition, U" instead of still 1000.");
		Melder_require (! ferror (theTestFilePointer),
			U"fseek to position -1 should not be an error.");
		fimresult = FileInMemory_fseek (theTestFim, -1, SEEK_SET);
		fimposition = FileInMemory_ftell (theTestFim);
		Melder_require (fimresult == -1,
			U"fseek to position -1: result should be failure.");
		Melder_require (fimposition == 1000,
			U"fseek to position -1: position is at ", fimposition, U" instead of still 1000.");
		Melder_require (! FileInMemory_ferror (theTestFim),
			U"FileInMemory_fseek to position -1 should not be an error.");

		MelderInfo_writeLine (U"File \"", fileName, U"\" was handled correctly\n");
	} catch (MelderError) {
		Melder_throw (U"Test on file \"", fileName, U"\" failed.");
	}
}

void test_FileInMemory_io () {
	try {
		/*
			Clean up the rubbish left over from any previous failed call.
		*/
		if (theTestFilePointer) {
			Melder_fclose (& theTestFile, theTestFilePointer);   // last chance
			theTestFilePointer = nullptr;
		}
		if (theTestFim) {
			FileInMemory_fclose (theTestFim);   // will dangle
			theTestFim = nullptr;
		}

		theTestFileInMemorySet = FileInMemorySet_createFromDirectoryContents (theTestSubfolder, U"*");

		{// scope
			const char content [200] = "abcd\n" "ef\n" "ghijk\n";
			const integer length =                14            ;
			const char lineContents [5][40] = { "abcd\n", "ef\n", "ghijk\n" };
			const integer lineLengths [5] =   {    5    ,   3   ,     6     };
			testOneFile (U"threeLinesWithFinalNewline.txt", content, length, lineContents, lineLengths, true);
		}
		{
			const char content [200] = "lmno\n" "\n" "pqrstuvwxyz";
			const integer length =                 17             ;
			const char lineContents [5][40] = { "lmno\n", "\n", "pqrstuvwxyz" };
			const integer lineLengths [5] =   {    5    ,  1  ,      11       };
			testOneFile (U"threeLinesWithoutFinalNewline.txt", content, length, lineContents, lineLengths, false);
		}
		{
			const char content [200] = "lmn\0\n" "\n" "pqrst\0vwx\0z";
			const integer length =                  17               ;
			const char lineContents [5][40] = { "lmn\0\n", "\n", "pqrst\0vwx\0z" };
			const integer lineLengths [5] =   {     5    ,  1  ,       11        };
			testOneFile (U"threeLinesWithThreeNulls.txt", content, length, lineContents, lineLengths, false);
		}
		{
			const char content [200] = "\n" "\n" "\n";
			const integer length =            3      ;
			const char lineContents [5][40] = { "\n", "\n", "\n" };
			const integer lineLengths [5] =   {   1 ,   1 ,   1  };
			testOneFile (U"threeEmptyLines.txt", content, length, lineContents, lineLengths, true);
		}
		{
			const char content [200] = "abcdefghijk\n";
			const integer length =           12       ;
			const char lineContents [5][40] = { "abcdefghijk\n" };
			const integer lineLengths [5] =   {       12        };
			testOneFile (U"oneLineWithFinalNewline.txt", content, length, lineContents, lineLengths, true);
		}
		{
			const char content [200] = "abcdefghijk";
			const integer length =          11      ;
			const char lineContents [5][40] = { "abcdefghijk" };
			const integer lineLengths [5] =   {      11       };
			testOneFile (U"oneLineWithoutFinalNewline.txt", content, length, lineContents, lineLengths, false);
		}
		{
			const char content [200] = "\n";
			const integer length =       1 ;
			const char lineContents [5][40] = { "\n" };
			const integer lineLengths [5] =   {   1  };
			testOneFile (U"oneEmptyLine.txt", content, length, lineContents, lineLengths, true);
		}
		{
			/*
				We should be able to handle an empty file (remember CrowdStrike in July 2024)
			*/
			const char content [200] = "";
			const integer length =      0;
			const char lineContents [5][40] = { "" };
			const integer lineLengths [5] =   {  0 };
			testOneFile (U"emptyFile.txt", content, length, lineContents, lineLengths, false);
		}

		/*
			Clean up.
		*/
		if (theTestFilePointer) {
			Melder_fclose (& theTestFile, theTestFilePointer);
			theTestFilePointer = nullptr;
		}
		if (theTestFim) {
			FileInMemory_fclose (theTestFim);
			theTestFim = nullptr;
		}
		MelderInfo_writeLine (U"test_FileInMemory_io: OK");
	} catch (MelderError) {
		Melder_throw (U"FileInMemory test failed.");
	}
}


/* End of file FileInMemory.cpp */
