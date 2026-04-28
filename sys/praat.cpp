/* praat.cpp
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
#include <stdarg.h>
#if defined (UNIX) || defined (macintosh)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <signal.h>
#endif
#include <locale.h>
#if defined (UNIX)
	#include <unistd.h>
#endif
#if defined (_WIN32)
	#include <windows.h>
	#include <fcntl.h>
	#include <io.h>
	#include <process.h>   // getpid
#endif

#include "praatP.h"
#include "praatM.h"
#include "praat_script.h"
#include "site.h"
#include "machine.h"
#include "Printer.h"
#include "ScriptEditor.h"
#include "NotebookEditor.h"
#include "Strings_.h"
#include "../kar/UnicodeData.h"
#include "InfoEditor.h"
extern "C" char *sendpraat (void *display, const char *programName, long timeOut, const char *text);

Thing_implement (Praat_Command, Thing, 0);

#define EDITOR  theCurrentPraatObjects -> list [IOBJECT]. editors

#define WINDOW_WIDTH 520
#define WINDOW_HEIGHT 700

/*
	Six globals, not `inline` (see praat.h 2021-04-15).
*/
structPraatApplication theForegroundPraatApplication;
PraatApplication theCurrentPraatApplication = & theForegroundPraatApplication;
structPraatObjects theForegroundPraatObjects;
PraatObjects theCurrentPraatObjects = & theForegroundPraatObjects;
structPraatPicture theForegroundPraatPicture;
PraatPicture theCurrentPraatPicture = & theForegroundPraatPicture;

bool praat_commandsWithExternalSideEffectsAreAllowed () {
	return
		theCurrentPraatObjects == & theForegroundPraatObjects ||
		! theCurrentPraatApplication -> manPages ||
		theCurrentPraatApplication -> manPages -> commandsWithExternalSideEffectsAreAllowed
	;
};

static structMelderFolder homeDir { };

/*
 * Melder_preferencesFolder: a folder containing preferences file, buttons file, message files, tracing file, plugins.
 *    Unix:   /home/miep/.praat-dir   (without slash)
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat
 *                         or:   C:\Users\Miep\Praat
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs
 */
// inline structMelderFolder Melder_preferencesFolder { };   // already declared in Melder_files.h (checked 2021-03-07)

/*
 * prefsFile: preferences file.
 *    Unix:   /home/miep/.praat-dir/prefs5
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat\Preferences5.ini
 *                         or:   C:\Users\Miep\Praat\Preferences5.ini
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs/Prefs5
 */
static structMelderFile prefsFile { };

/*
 * buttonsFile: buttons file.
 *    Unix:   /home/miep/.praat-dir/buttons
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat\Buttons5.ini
 *                         or:   C:\Users\Miep\Praat\Buttons5.ini
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs/Buttons5
 */
static structMelderFile buttonsFile { };

#if defined (UNIX)
	static structMelderFile pidFile { };   // like /home/miep/.praat-dir/pid
	static structMelderFile messageFile { };   // like /home/miep/.praat-dir/message
#elif defined (_WIN32)
	static structMelderFile messageFile { };   // like C:\Users\Miep\Praat\Message.txt
#endif

/*
 * tracingFile: tracing file.
 *    Unix:   /home/miep/.praat-dir/tracing
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat\Tracing.txt
 *                         or:   C:\Users\Miep\Praat\Tracing.txt
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs/Tracing.txt
 */
static structMelderFile tracingFile { };

static GuiList praatList_objects;

/***** selection *****/

integer praat_idOfSelected (ClassInfo klas, integer inplace) {
	integer place = inplace, IOBJECT;
	if (place == 0)
		place = 1;
	if (place > 0) {
		WHERE (SELECTED && (! klas || CLASS == klas)) {
			if (place == 1)
				return ID;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (! klas || CLASS == klas)) {
			if (place == -1)
				return ID;
			place ++;
		}
	}
	if (inplace)
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" #", inplace, U" selected.");
	else
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" selected.");
	return 0;
}

autoVEC praat_idsOfAllSelected (ClassInfo klas) {
	autoVEC result = raw_VEC (praat_numberOfSelected (klas));
	integer selectedObjectNumber = 0, IOBJECT;
	WHERE (SELECTED && (! klas || CLASS == klas))
		result [++ selectedObjectNumber] = ID;
	return result;
}

autoSTRVEC praat_namesOfAllSelected (ClassInfo klas) {
	autoSTRVEC result (praat_numberOfSelected (klas));
	integer selectedObjectNumber = 0, IOBJECT;
	WHERE (SELECTED && (! klas || CLASS == klas))
		result [++ selectedObjectNumber] = Melder_dup (klas ? NAME : FULL_NAME);
	return result;
}

char32 * praat_nameOfSelected (ClassInfo klas, integer inplace) {
	integer place = inplace, IOBJECT;
	if (place == 0)
		place = 1;
	if (place > 0) {
		WHERE (SELECTED && (! klas || CLASS == klas)) {
			if (place == 1)
				return klas ? NAME : FULL_NAME;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (! klas || CLASS == klas)) {
			if (place == -1)
				return klas ? NAME : FULL_NAME;
			place ++;
		}
	}
	if (inplace)
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" #", inplace, U" selected.");
	else
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" selected.");
	return nullptr;   // failure, but never reached anyway
}

integer praat_numberOfSelected (ClassInfo klas) {
	if (! klas)
		return theCurrentPraatObjects -> totalSelection;
	integer readableClassId = klas -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0)
		Melder_crash (U"No sequential unique ID for class ", klas -> className, U" (numberOfSelected).");
	return theCurrentPraatObjects -> numberOfSelected [readableClassId];
}

void praat_deselect (integer IOBJECT) {
	if (! SELECTED)
		return;
	SELECTED = false;
	theCurrentPraatObjects -> totalSelection -= 1;
	integer readableClassId = theCurrentPraatObjects -> list [IOBJECT]. object -> classInfo -> sequentialUniqueIdOfReadableClass;
	Melder_assert (readableClassId != 0);
	theCurrentPraatObjects -> numberOfSelected [readableClassId] -= 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		trace (U"deselecting object ", IOBJECT);
		GuiList_deselectItem (praatList_objects, IOBJECT);
		trace (U"deselected object ", IOBJECT);
	}
}

void praat_deselectAll () {
	integer IOBJECT;
	WHERE (1)
		praat_deselect (IOBJECT);
}

void praat_select (integer IOBJECT) {
	if (SELECTED)
		return;
	SELECTED = true;
	theCurrentPraatObjects -> totalSelection += 1;
	Thing object = theCurrentPraatObjects -> list [IOBJECT]. object;
	Melder_assert (object);
	integer readableClassId = object -> classInfo -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0)
		Melder_crash (U"No sequential unique ID for class ", object -> classInfo -> className, U" (selectObject).");
	theCurrentPraatObjects -> numberOfSelected [readableClassId] += 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding)
		GuiList_selectItem (praatList_objects, IOBJECT);
}

void praat_selectAll () {
	integer IOBJECT;
	WHERE (true)
		praat_select (IOBJECT);
}

void praat_list_background () {
	integer IOBJECT;
	WHERE (SELECTED)
		GuiList_deselectItem (praatList_objects, IOBJECT);
}
void praat_list_foreground () {
	integer IOBJECT;
	WHERE (SELECTED)
		GuiList_selectItem (praatList_objects, IOBJECT);
}

autoCollection praat_getSelectedObjects () {
	autoCollection thee = Collection_create ();
	integer IOBJECT;
	LOOP {
		iam_LOOP (Daata);
		thy addItem_ref (me);
	}
	return thee;
}

char32 *praat_name (integer IOBJECT) { return str32chr (FULL_NAME, U' ') + 1; }

void praat_write_do (UiForm dia, conststring32 extension) {
	static MelderString defaultFileName;
	if (extension && str32chr (extension, U'.')) {
		/*
			Apparently, the "extension" is a complete file name.
			This should be used as the default file name.
			(This case typically occurs when saving a picture.)
		*/
		MelderString_copy (& defaultFileName, extension);
	} else {
		/*
			Apparently, the "extension" is not a complete file name.
			We are expected to prepend the "extension" with the name of a selected object.
		*/
		integer IOBJECT, found = 0;
		Daata data = nullptr;
		WHERE (SELECTED) {
			if (! data)
				data = (Daata) OBJECT;
			found += 1;
		}
		if (found == 1) {
			MelderString_copy (& defaultFileName, data -> name.get());
			if (defaultFileName.length > 200) {
				defaultFileName.string [200] = U'\0';
				defaultFileName.length = 200;
			}
			MelderString_append (& defaultFileName, U".", extension ? extension : Thing_className (data));
		} else if (! extension) {
			MelderString_copy (& defaultFileName, U"praat.Collection");
		} else {
			MelderString_copy (& defaultFileName, U"praat.", extension);
		}
	}
	UiOutfile_do (dia, defaultFileName.string);
}

static void removeAllReferencesToMoribundEditor (Editor editor) {
	/*
		Remove all references to this editor.
		It may be editing multiple objects.
	*/
	for (integer iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++)
		for (integer ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraatObjects -> list [iobject]. editors [ieditor] == editor)
				theCurrentPraatObjects -> list [iobject]. editors [ieditor] = nullptr;
}

/**
	Remove the "object" from the list,
	killing everything that has to do with the selection.
*/
static void praat_remove (integer iobject, bool removeVisibly) {
	Melder_assert (iobject >= 1 && iobject <= theCurrentPraatObjects -> n);
	if (theCurrentPraatObjects -> list [iobject]. isBeingCreated) {
		theCurrentPraatObjects -> list [iobject]. isBeingCreated = false;
		theCurrentPraatObjects -> totalBeingCreated --;
	}
	if (removeVisibly)
		praat_deselect (iobject);

	/*
		To prevent synchronization problems, kill editors before killing the data.
	*/
	for (integer ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		Editor editor = theCurrentPraatObjects -> list [iobject]. editors [ieditor];   // save this one reference
		if (editor) {
			trace (U"remove references to editor ", ieditor);
			removeAllReferencesToMoribundEditor (editor);
			trace (U"forget editor ", ieditor);
			if (removeVisibly)
				forget (editor);   // TODO: doesn't this call removeAllReferencesToMoribundEditor() again?
			trace (U"forgotten editor ", ieditor);
		}
	}
	MelderFile_setToNull (& theCurrentPraatObjects -> list [iobject]. file);
	theCurrentPraatObjects -> list [iobject]. name. reset();
	forget (theCurrentPraatObjects -> list [iobject]. object);   // note: this might save a file-based object to file
}

void praat_cleanUpName (char32 *name) {
/*
	Replaces spaces and special characters by underscores.
*/
	for (; *name; name ++)
		if (str32chr (U" ,.:;\\/()[]{}~`\'<>*&^%#@!?$\"|", *name))
			*name = U'_';
}

/***** objects + commands *****/

static void praat_new_unpackCollection (autoCollection me, const char32* myName) {
	for (integer idata = 1; idata <= my size; idata ++) {
		autoDaata object;
		object. adoptFromAmbiguousOwner ((Daata) my at [idata]);
		my at [idata] = nullptr;   // disown; once the elements are autoThings, the move will handle this
		const conststring32 name = ( object -> name ? object -> name.get() : myName );
		Melder_assert (name);
		praat_new (object.move(), name);   // recurse
	}
}

void praat_newWithFile (autoDaata me, MelderFile file, conststring32 myName) {
	if (! me)
		Melder_throw (U"No object was put into the list.");

	if (my classInfo == classCollection) {
		praat_new_unpackCollection (me.static_cast_move <structCollection>(), myName);
		return;
	}

	autoMelderString name, givenName;
	if (myName && myName [0]) {
		MelderString_copy (& givenName, myName);
		/*
			Remove extension.
		*/
		char32 *p = str32rchr (givenName.string, U'.');
		if (p)
			*p = U'\0';
	} else {
		MelderString_copy (& givenName, my name && my name [0] ? my name.get() : U"untitled");
	}
	praat_cleanUpName (givenName.string);
	MelderString_append (& name, Thing_className (me.get()), U" ", givenName.string);

	if (theCurrentPraatObjects -> n == praat_MAXNUM_OBJECTS) {
		//forget (me);
		Melder_throw (U"The Object Window cannot contain more than ", praat_MAXNUM_OBJECTS, U" objects. You could remove some objects.");
	}
		
	integer IOBJECT = ++ theCurrentPraatObjects -> n;
	Melder_assert (FULL_NAME == nullptr);
	theCurrentPraatObjects -> list [IOBJECT]. name = Melder_dup_f (name.string);   // all right to crash if out of memory
	++ theCurrentPraatObjects -> sequentialUniqueIdOfLatestObjectInList;

	if (! theCurrentPraatApplication -> batch)   // put a new object on the screen, at the bottom of the list
		GuiList_insertItem (
			praatList_objects,
			Melder_cat (theCurrentPraatObjects -> sequentialUniqueIdOfLatestObjectInList, U". ", name.string),
			theCurrentPraatObjects -> n
		);
	CLASS = my classInfo;
	OBJECT = me.releaseToAmbiguousOwner();   // FIXME: should be move()
	SELECTED = false;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		EDITOR [ieditor] = nullptr;
	if (file)
		MelderFile_copy (file, & theCurrentPraatObjects -> list [IOBJECT]. file);
	else
		MelderFile_setToNull (& theCurrentPraatObjects -> list [IOBJECT]. file);
	ID = theCurrentPraatObjects -> sequentialUniqueIdOfLatestObjectInList;
	theCurrentPraatObjects -> list [IOBJECT]. isBeingCreated = true;
	Thing_setName (OBJECT, givenName.string);
	theCurrentPraatObjects -> totalBeingCreated ++;
}

static MelderString thePraatNewName;
void praat_new (autoDaata me) {
	praat_newWithFile (me.move(), nullptr, U"");
}
void praat_new (autoDaata me, const MelderArg& arg) {
	praat_newWithFile (me.move(), nullptr, arg._arg);
}
void praat_new (autoDaata me,
	const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3,
	const MelderArg& arg4, const MelderArg& arg5)
{
	MelderString_copy (& thePraatNewName, arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg);
	praat_new (me.move(), thePraatNewName.string);
}

void praat_updateSelection () {
	if (theCurrentPraatObjects -> totalBeingCreated > 0) {
		integer IOBJECT;
		praat_deselectAll ();
		WHERE (theCurrentPraatObjects -> list [IOBJECT]. isBeingCreated) {
			praat_select (IOBJECT);
			theCurrentPraatObjects -> list [IOBJECT]. isBeingCreated = false;
		}
		theCurrentPraatObjects -> totalBeingCreated = 0;
		praat_show ();
	}
}

static void gui_cb_list_selectionChanged (Thing /* boss */, GuiList_SelectionChangedEvent event) {
	Melder_assert (event -> list == praatList_objects);
	integer IOBJECT;
	bool first = true;
	WHERE (SELECTED) {
		SELECTED = false;
		integer readableClassId = theCurrentPraatObjects -> list [IOBJECT]. object -> classInfo -> sequentialUniqueIdOfReadableClass;
		theCurrentPraatObjects -> numberOfSelected [readableClassId] --;
		Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] >= 0);
	}
	theCurrentPraatObjects -> totalSelection = 0;
	autoINTVEC selected = GuiList_getSelectedPositions (praatList_objects);
	for (integer iselected = 1; iselected <= selected.size; iselected ++) {
		IOBJECT = selected [iselected];
		SELECTED = true;
		integer readableClassId = theCurrentPraatObjects -> list [IOBJECT]. object -> classInfo -> sequentialUniqueIdOfReadableClass;
		theCurrentPraatObjects -> numberOfSelected [readableClassId] ++;
		Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] > 0);
		UiHistory_write (first ? U"\nselectObject: \"" : U"\nplusObject: \"");
		UiHistory_write_expandQuotes (FULL_NAME);
		UiHistory_write (U"\"");
		first = false;
		theCurrentPraatObjects -> totalSelection += 1;
	}
	praat_show ();
}

void praat_list_renameAndSelect (integer position, conststring32 name) {
	if (! theCurrentPraatApplication -> batch) {
		GuiList_replaceItem (praatList_objects, name, position);   // void if name equal
		if (! Melder_backgrounding)
			GuiList_selectItem (praatList_objects, position);
	}
}

/***** objects *****/

void praat_name2 (char32 *name, ClassInfo klas1, ClassInfo klas2) {
	int i1 = 1;
	while (theCurrentPraatObjects -> list [i1]. isSelected == 0 || theCurrentPraatObjects -> list [i1]. klas != klas1)
		i1 ++;
	int i2 = 1;
	while (theCurrentPraatObjects -> list [i2]. isSelected == 0 || theCurrentPraatObjects -> list [i2]. klas != klas2)
		i2 ++;
	char32 *name1 = str32chr (theCurrentPraatObjects -> list [i1]. name.get(), U' ') + 1;
	char32 *name2 = str32chr (theCurrentPraatObjects -> list [i2]. name.get(), U' ') + 1;
	if (str32equ (name1, name2))
		Melder_sprint (name,200, name1);
	else
		Melder_sprint (name,200, name1, U"_", name2);
}

void praat_removeObject (integer i) {
	praat_remove (i, true);   // dangle
	for (integer j = i; j < theCurrentPraatObjects -> n; j ++)
		theCurrentPraatObjects -> list [j] = std::move (theCurrentPraatObjects -> list [j + 1]);   // undangle but create second references
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. name. reset();
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. object = nullptr;   // undangle or remove second reference
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. isSelected = 0;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. editors [ieditor] = nullptr;   // undangle or remove second reference
	MelderFile_setToNull (& theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. file);   // undangle or remove second reference
	-- theCurrentPraatObjects -> n;
	if (! theCurrentPraatApplication -> batch)
		GuiList_deleteItem (praatList_objects, i);
}

static void praat_exit (int exit_code) {
//Melder_setTracing (true);
	integer IOBJECT;
	trace (U"destroy the picture window");
	praat_picture_exit ();
	praat_statistics_exit ();   // record total memory use across sessions

	if (! praatP.ignorePreferenceFiles) {
		trace (U"stop receiving messages");
		#ifdef UNIX
			/*
				We are going to delete the process ID ("pid") file, if it's ours.
				We consider it ours if its process ID equals our own process ID
				(no more checks are necessary; the pid file cannot have been created
				by a different Praat instance in an earlier computer session,
				because if we arrive here, we are sure to have created a new pid file
				in the current computer session).
			*/
			if (! MelderFile_isNull (& pidFile)) {
				try {
					/*
						To see whether we own the pid file,
						we look into it to see whether its pid equals our pid.
						If not, then we are probably living in an old invocation of the program,
						and the pid file was written by the latest invocation of the program,
						which owns the pid (this means sendpraat can only send to the latest Praat if more than one are open).
					*/
					autofile f = Melder_fopen (& pidFile, "r");
					int pidOfLatestPraatInvocation;
					if (fscanf (f, "%d", & pidOfLatestPraatInvocation) < 1)
						throw MelderError ();
					f.close (& pidFile);
					int pidOfCurrentPraatIncoation = getpid ();
					if (pidOfLatestPraatInvocation == pidOfCurrentPraatIncoation)
						MelderFile_delete (& pidFile);   // ...then we own the pid file and can delete it
				} catch (MelderError) {
					Melder_clearError ();   // if the pid file is somehow missing or corrupted, we just ignore that
				}
			}
		#endif

		trace (U"save the preferences");
		Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // refuse to write the preferences if the locale is wrong (even if tracing is on)
		Preferences_write (& prefsFile);

		trace (U"save the script buttons");
		if (! theCurrentPraatApplication -> batch) {
			try {
				autoMelderString buffer;
				MelderString_append (& buffer, U"# Buttons (1).\n");
				MelderString_append (& buffer, U"# This file is generated automatically when you quit the ", Melder_upperCaseAppName(), U" program.\n");
				MelderString_append (& buffer, U"# It contains the buttons that you added interactively to the fixed or dynamic menus,\n");
				MelderString_append (& buffer, U"# and the buttons that you hid or showed.\n\n");
				praat_saveAddedMenuCommands (& buffer);
				praat_saveToggledMenuCommands (& buffer);
				praat_saveAddedActions (& buffer);
				praat_saveToggledActions (& buffer);
				MelderFile_writeText (& buttonsFile, buffer.string, kMelder_textOutputEncoding::ASCII_THEN_UTF16);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	}

	trace (U"flush the file-based objects");
	WHERE_DOWN (! MelderFile_isNull (& theCurrentPraatObjects -> list [IOBJECT]. file)) {
		trace (U"removing object based on file ", & theCurrentPraatObjects -> list [IOBJECT]. file);
		praat_remove (IOBJECT, false);
	}

	trace (U"leave the program");
	praat_menuCommands_exit_optimizeByLeaking ();   // these calls are superflous if subsequently _Exit() is called instead of exit()
	praat_actions_exit_optimizeByLeaking ();
	Preferences_exit_optimizeByLeaking ();
	/*
		OPTIMIZE with an exercise in self-documenting code
	*/
	constexpr bool weWouldLikeToOptimizeExitingSpeed = ((true));
	constexpr bool callingExitTimeDestructorsIsSlow = (true);
	constexpr bool notCallingExitTimeDestructorsCausesCorrectBehaviour = (true);
	constexpr bool weAreReallySureAboutThat = (true);
	constexpr bool weWillUseUnderscoreExitInsteadOfExit =
			weWouldLikeToOptimizeExitingSpeed &&
			callingExitTimeDestructorsIsSlow &&
			notCallingExitTimeDestructorsCausesCorrectBehaviour &&
			weAreReallySureAboutThat;
	if ((weWillUseUnderscoreExitInsteadOfExit)) {
		constexpr bool underscoreExitHasMoreSideEffectsThanJustNotCallingExitTimeDestructors = (true);
		constexpr bool avoidOtherSideEffectsOfUnderscoreExit =
				underscoreExitHasMoreSideEffectsThanJustNotCallingExitTimeDestructors;
		if ((avoidOtherSideEffectsOfUnderscoreExit)) {
			constexpr bool oneSideEffectIsThatOpenOutputFilesAreNotFlushed = true;
			constexpr bool weShouldFlushAllOpenOutputFilesWhoseNonflushingWouldCauseIncorrectBehaviour =
					oneSideEffectIsThatOpenOutputFilesAreNotFlushed;
			if ((weShouldFlushAllOpenOutputFilesWhoseNonflushingWouldCauseIncorrectBehaviour)) {
				constexpr bool stdoutIsOpen = (true);
				constexpr bool stderrIsOpen = (true);
				constexpr bool stdoutIsBufferedByDefault = true;
				constexpr bool stderrIsBufferedByDefault = false;
				constexpr bool weKnowThatSetbufHasNotBeenCalledOnStdout = (false);
				constexpr bool weKnowThatSetbufHasNotBeenCalledOnStderr = (false);
				constexpr bool stdoutHasCertainlyBeenFlushed =
						! stdoutIsBufferedByDefault && weKnowThatSetbufHasNotBeenCalledOnStdout;
				constexpr bool stderrHasCertainlyBeenFlushed =
						! stderrIsBufferedByDefault && weKnowThatSetbufHasNotBeenCalledOnStderr;
				constexpr bool notFlushingStdoutCouldCauseIncorrectBehaviour =
						stdoutIsOpen && ! stdoutHasCertainlyBeenFlushed;
				constexpr bool notFlushingStderrCouldCauseIncorrectBehaviour =
						stderrIsOpen && ! stderrHasCertainlyBeenFlushed;
				constexpr bool shouldFlushStdout = notFlushingStdoutCouldCauseIncorrectBehaviour;
				constexpr bool shouldFlushStderr = notFlushingStderrCouldCauseIncorrectBehaviour;
				if ((shouldFlushStdout))
					fflush (Melder_stdout);
				if ((shouldFlushStderr))
					fflush (Melder_stderr);
				constexpr bool thereAreOtherOpenFiles = (false);
				constexpr bool thereAreOtherOpenFilesWhoseNonflushingCouldCauseIncorrectBehaviour =
						thereAreOtherOpenFiles;
				if ((! thereAreOtherOpenFilesWhoseNonflushingCouldCauseIncorrectBehaviour)) {}
			}
			constexpr bool thereAreNoOtherSideEffectsBesideNotCallingExitDestructorsAndNotFlushingOpenFiles = (true);
			if ((thereAreNoOtherSideEffectsBesideNotCallingExitDestructorsAndNotFlushingOpenFiles)) {}
		}
		_Exit (exit_code);
	} else {
		exit (exit_code);
	}
}

static void cb_Editor_destruction (Editor me) {
	removeAllReferencesToMoribundEditor (me);
}

static void cb_Editor_dataChanged (Editor me) {
	for (integer iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++) {
		/*
			Am I editing this object?
		*/
		bool editingThisObject = false;
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			editingThisObject |= ( theCurrentPraatObjects -> list [iobject]. editors [ieditor] == me );
		if (editingThisObject) {
			/*
				Notify all editors associated with this object, *including myself*.
				But the receiver will be able to check whether the notification comes from self or not;
				e.g. the DataEditor will react differently on a message from outside than on a message from inside.
				(last checked 2022-09-30)
			*/
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor otherEditor = theCurrentPraatObjects -> list [iobject]. editors [ieditor];
				if (otherEditor)
					Editor_dataChanged (otherEditor, me);
			}
		}
	}
}

static void cb_Editor_publication (Editor /* me */, autoDaata publication) {
/*
   The default publish callback.
   Works nicely if the publisher invents a name.
*/
	try {
		praat_new (publication.move(), U"");
	} catch (MelderError) {
		Melder_flushError ();
	}
	praat_updateSelection ();
}

void praat_installEditor (Editor editor, integer IOBJECT) {
	if (! editor)
		return;
	for (integer ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		if (! EDITOR [ieditor]) {
			EDITOR [ieditor] = editor;
			Editor_setDestructionCallback (editor, cb_Editor_destruction);
			Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
			if (! editor -> d_publicationCallback)
				Editor_setPublicationCallback (editor, cb_Editor_publication);
			//Thing_setName (editor, Melder_cat (editor -> name.get(), U" [", ieditor + 1, U"]"));   // would break existing scripts
			return;
		}
	}
	Melder_throw (U"(praat_installEditor:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
}

void praat_installEditor2 (Editor editor, integer i1, integer i2) {
	if (! editor)
		return;
	integer ieditor1 = 0;
	for (; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (! theCurrentPraatObjects -> list [i1]. editors [ieditor1])
			break;
	integer ieditor2 = 0;
	for (; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (! theCurrentPraatObjects -> list [i2]. editors [ieditor2])
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = editor;
		Editor_setDestructionCallback (editor, cb_Editor_destruction);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
		if (! editor -> d_publicationCallback)
			Editor_setPublicationCallback (editor, cb_Editor_publication);
	} else {
		Melder_throw (U"(praat_installEditor2:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
	}
}

void praat_installEditor3 (Editor editor, integer i1, integer i2, integer i3) {
	if (! editor)
		return;
	integer ieditor1 = 0;
	for (; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (! theCurrentPraatObjects -> list [i1]. editors [ieditor1])
			break;
	integer ieditor2 = 0;
	for (; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (! theCurrentPraatObjects -> list [i2]. editors [ieditor2])
			break;
	integer ieditor3 = 0;
	for (; ieditor3 < praat_MAXNUM_EDITORS; ieditor3 ++)
		if (! theCurrentPraatObjects -> list [i3]. editors [ieditor3])
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS && ieditor3 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = theCurrentPraatObjects -> list [i3]. editors [ieditor3] = editor;
		Editor_setDestructionCallback (editor, cb_Editor_destruction);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
		if (! editor -> d_publicationCallback)
			Editor_setPublicationCallback (editor, cb_Editor_publication);
	} else {
		Melder_throw (U"(praat_installEditor3:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
	}
}

void praat_installEditorN (Editor editor, DaataList objects) {
	if (! editor)
		return;
	/*
		First check whether all objects in the DaataList are also in the List of Objects (Praat crashes if not),
		and check whether there is room to add an editor for each.
	*/
	for (integer iOrderedObject = 1; iOrderedObject <= objects->size; iOrderedObject ++) {
		Daata object = objects->at [iOrderedObject];
		integer iPraatObject = 1;
		for (; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor = 0;
				for (; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
					if (! theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor])
						break;
				if (ieditor >= praat_MAXNUM_EDITORS)
					Melder_throw (U"Cannot view the same object in more than ", praat_MAXNUM_EDITORS, U" windows.");
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraatObjects -> n);   // an element of the Ordered does not occur in the List of Objects
	}
	/*
		There appears to be room for all elements of the DaataList. The editor window can appear. Install the editor in all objects.
	*/
	for (integer iOrderedObject = 1; iOrderedObject <= objects->size; iOrderedObject ++) {
		Daata object = objects->at [iOrderedObject];
		integer iPraatObject = 1;
		for (; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor = 0;
				for (; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (! theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor]) {
						theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] = editor;
						Editor_setDestructionCallback (editor, cb_Editor_destruction);
						Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
						if (! editor -> d_publicationCallback)
							Editor_setPublicationCallback (editor, cb_Editor_publication);
						break;
					}
				}
				Melder_assert (ieditor < praat_MAXNUM_EDITORS);   // we just checked, but nevertheless
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraatObjects -> n);   // we already checked, but still
	}
}

void praat_dataChanged (Daata object) {
	/*
		This function can be called at error time, which is weird.
	*/
	autostring32 saveError;
	const bool duringError = Melder_hasError ();
	if (duringError) {
		if (Melder_hasCrash ())
			return;   // straight to the exit, without attempting to notify any editors of any data, which might well be corrupted
		saveError = Melder_dup_f (Melder_getError ());
		Melder_clearError ();
	}
	integer IOBJECT;
	WHERE (OBJECT == object) {
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			Editor editor = EDITOR [ieditor];
			if (editor) {
				Editor_dataChanged (editor, nullptr);
				if (duringError)
					Melder_clearError ();   // accept only the original error, and not the extra ones generated in the editors
			}
		}
	}
	if (duringError) {
		Melder_appendError_noLine (saveError.get());   // restore the original error message
		/*
			If we are during error time, then this error should be caught
			either by `throw` (see praatM.h, two occurrences) or by Melder_flushError (also see praatM.h, one occurrence).
			(last checked 2021-12-02)
		*/
	}
}

static void helpProc (conststring32 query) {
	if (theCurrentPraatApplication -> batch) {
		Melder_flushError (U"Cannot view manual from batch.");
		return;
	}
	try {
		autoManual manual = Manual_create (query, nullptr, theCurrentPraatApplication -> manPages, false, true);
		manual.releaseToUser();
	} catch (MelderError) {
		Melder_flushError (U"help: no help on \"", query, U"\".");
	}
}

static int publishProc (autoDaata me) {
	try {
		praat_new (me.move(), U"");
		praat_updateSelection ();
		return 1;
	} catch (MelderError) {
		Melder_throw (U"Not published.");
	}
}

/***** QUIT *****/

FORM (DO_Quit, U"Confirm Quit", U"Quit") {
	MUTABLE_COMMENT (label1, U"You have objects in your list!")
	MUTABLE_COMMENT (label2, U"Do you still want to quit?")
	OK
{
	const bool youHaveObjectsInYourList = ( theCurrentPraatObjects -> n > 0 );
	const bool youHaveUnsavedScriptsOrNotebooks = ( ScriptEditors_dirty () || NotebookEditors_dirty () );
	if (youHaveObjectsInYourList || youHaveUnsavedScriptsOrNotebooks) {
		if (youHaveObjectsInYourList && youHaveUnsavedScriptsOrNotebooks)
			SET_STRING (label1, U"You have objects in your list, and unsaved scripts or notebooks!")
		else if (youHaveUnsavedScriptsOrNotebooks)
			SET_STRING (label1, U"You have unsaved scripts or notebooks!")
		else
			SET_STRING (label1, U"You have objects in your list!")
		char32 prompt [200];
		Melder_sprint (prompt,200, U"Do you still want to quit ", Melder_upperCaseAppName(), U"?");
		SET_STRING (label2, prompt)
	} else {
		praat_exit (0);
	}
}
DO
	praat_exit (0);
	END_NO_NEW_DATA
}

static void cb_quitApplication () {
	DO_Quit (nullptr, 0, nullptr, nullptr, nullptr, nullptr, false, nullptr, nullptr);
}

static void gui_cb_quit (Thing /* me */) {
	Gui_runQuitApplicationCallback ();
}

void praat_dontUsePictureWindow () { praatP.dontUsePictureWindow = true; }

/********** INITIALIZATION OF THE PRAAT SHELL **********/

#if defined (UNIX) && ! defined (NO_GUI)
	/*
		sendpraat messages can only enter via SIGUSR1 at interrupt time.
		We generate an event that should enter the main event loop at loop time.
	*/
	static void cb_sigusr1 (int signum) {
		Melder_assert (signum == SIGUSR1);
		signal (SIGUSR1, cb_sigusr1);   // keep this handler in the air
		GdkEventProperty gevent;   // or GdkEventSetting, once we can find its signal name
		gevent. type = GDK_PROPERTY_NOTIFY;   // or GDK_SETTING
		gevent. window = gtk_widget_get_window (GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow));
		gevent. send_event = 1;
		gevent. atom = gdk_atom_intern_static_string ("SENDPRAAT");
		gevent. time = 0;
		gevent. state = GDK_PROPERTY_NEW_VALUE;
		// Melder_casual (U"event put");
		gdk_event_put ((GdkEvent *) & gevent);   // this is safe only if gdk_event_put is reentrant, which is unlikely because the event queue is global
	}
#endif

#if defined (UNIX)
	#if ! defined (NO_GUI)
		static gboolean cb_userMessage (GtkWidget /* widget */, GdkEventProperty * /* event */, gpointer /* userData */) {
			trace (U"client event called");
			autofile f;
			try {
				f = Melder_fopen (& messageFile, "r");
			} catch (MelderError) {
				Melder_clearError ();
				return true;   // OK
			}
			integer pid = 0;
			int narg = fscanf (f, "#%td", & pid);
			f.close (& messageFile);
			{// scope
				autoPraatBackground background;
				try {
					praat_executeScript_noGUI (& messageFile);
				} catch (MelderError) {
					Melder_flushError (Melder_upperCaseAppName(), U": message not completely handled.");
				}
			}
			if (narg != 0 && pid != 0)
				kill (pid, SIGUSR2);
			return true;
		}
	#endif
#elif defined (_WIN32)
	static int cb_userMessage () {
		autoPraatBackground background;
		try {
			praat_executeScript_noGUI (& messageFile);
		} catch (MelderError) {
			Melder_flushError (Melder_upperCaseAppName(), U": message not completely handled.");
		}
		return 0;
	}
	static void cb_openDocument (MelderFile file) {
		char32 text [kMelder_MAXPATH+25];
		/*
			The user dropped a file on the Praat icon,
			or double-clicked a Praat file,
			while Praat is already running.
		*/
		Melder_sprint (text,500, U"Read from file: ~", MelderFile_peekPath (file));
		sendpraat (nullptr, Melder_peek32to8 (Melder_upperCaseAppName()), 0, Melder_peek32to8 (text));
	}
	static void cb_finishedOpeningDocuments () {
		praat_updateSelection ();
	}
#elif macintosh
	static int (*theUserMessageCallback) (char32 *message);
	static void mac_setUserMessageCallback (int (*userMessageCallback) (char32 *message)) {
		theUserMessageCallback = userMessageCallback;
	}
	static pascal OSErr mac_processSignal8 (const AppleEvent *theAppleEvent, AppleEvent * /* reply */, long /* handlerRefCon */) {
		static bool duringAppleEvent = false;   // FIXME: may have to be atomic?
		if (! duringAppleEvent) {
			char *buffer;
			Size actualSize;
			duringAppleEvent = true;
			ProcessSerialNumber psn;
			GetCurrentProcess (& psn);
			SetFrontProcess (& psn);
			AEGetParamPtr (theAppleEvent, 1, typeUTF8Text, nullptr, nullptr, 0, & actualSize);
			buffer = (char *) malloc ((size_t) actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeUTF8Text, nullptr, & buffer [0], actualSize, nullptr);
			if (theUserMessageCallback) {
				autostring32 buffer32 = Melder_8to32_e (buffer);
				theUserMessageCallback (buffer32.get());
			}
			free (buffer);
			duringAppleEvent = false;
		}
		return noErr;
	}
	static int cb_userMessage (char32 *message) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromText (message);
		} catch (MelderError) {
			Melder_flushError (Melder_upperCaseAppName(), U": message not completely handled.");
		}
		return 0;
	}
#endif

static conststring32 thePraatStandAloneScriptText = nullptr;

void praat_setStandAloneScriptText (conststring32 text) {
	thePraatStandAloneScriptText = text;
}

static bool tryToAttachToTheCommandLine ()
{
	bool weHaveSucceeded = false;
	#if defined (_WIN32)
		/*
		 * On Windows, console applications are automatically attached to the command line,
		 * but Praat is always a Windows application instead, so command line attachment
		 * has to be handled explicitly, as here.
		 */
		if (AttachConsole (ATTACH_PARENT_PROCESS)) {   // was Praat called from either a console window or a "system" command?
			weHaveSucceeded = true;
		}
	#else
		weHaveSucceeded = isatty (fileno (stdin)) || isatty (fileno (stdout)) || isatty (fileno (stderr));
		/*
			The result is `true` if Praat was called from a terminal window or some system() commands or Xcode,
			and `false` if Praat was called from the Finder by double-clicking or dropping a file.

			This might be incorrectly false only if all three streams are redirected to a file or pipe,
			but this hasn't been tested yet.
		*/
	#endif
	return weHaveSucceeded;
}

static void setThePraatLocale () {
	/*
		We use only the "C" locale, because iswalpha works differently
		on different platforms, even if UTF-8 is specified (e.g. try 0x0905,
		which is an `alpha` on Windows but not on Mac and Linux).
		We do have to replace everything from <wctype.h>:
		- all iswalpha by iswalpha_portable
		- all iswalnum by iswalnum_portable
		- all iswpunct by iswpunct_portable
		- all iswspace by iswspace_portable
		- all iswdigit by iswdigit_portable
		- all iswlower by iswlower_portable
		- all iswupper by iswupper_portable
	*/
	setlocale (LC_ALL, "C");   // said to be superfluous on Windows, but cannot hurt
}

static void installPraatShellPreferences () {
	praat_statistics_prefs ();   // number of sessions, memory used...
	praat_picture_prefs ();   // font...
	Graphics_prefs ();
	Ui_prefs ();
	structDataGui    :: f_preferences ();   // erase picture first...
	structEditor     :: f_preferences ();   // shell size...
	structHyperPage  :: f_preferences ();   // font...
	Site_prefs ();   // print command...
	Melder_audio_prefs ();   // asynchronicity, silence after...
	Melder_textEncoding_prefs ();
	Printer_prefs ();   // paper size, printer command...
	structTextEditor :: f_preferences ();   // font size...
}

extern "C" void praatlib_init () {
	setThePraatLocale ();   // FIXME: don't use the global locale
	Melder_init ();
	Melder_rememberShellDirectory ();
	installPraatShellPreferences ();   // needed in the library, because this sets the defaults
	praatP.argc = 0;
	praatP.argv = nullptr;
	praatP.argumentNumber = 1;
	Melder_batch = true;
	praatP.userWantsToOpen = false;
	Melder_setAppName (U"Praatlib");
	theCurrentPraatApplication -> batch = true;
	Melder_getHomeDir (& homeDir);
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classStringSet);
	Thing_recognizeClassByOtherName (classStringSet, U"SortedSetOfString");
	Melder_backgrounding = true;
	praat_addMenus (nullptr);
	praat_addFixedButtons (nullptr);
	praat_addMenus2 ();
}

static void injectMessageAndInformationProcs (GuiWindow parent) {
	Gui_injectMessageProcs (parent);
	InfoEditor_injectInformationProc ();
}

static void printHelp () {
	MelderInfo_writeLine (U"Usage:");
	MelderInfo_writeLine (U"   To start up Praat with a new GUI:");
	MelderInfo_writeLine (U"      praat [OPTION]...");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To run a Praat script without a GUI:");
	MelderInfo_writeLine (U"      praat [--run] [OPTION]... SCRIPT-FILE-NAME [SCRIPT-ARGUMENT]...");
	MelderInfo_writeLine (U"   The switch --run is superfluous when you use a Console or Terminal");
	MelderInfo_writeLine (U"   interactively, but necessary if you call Praat programmatically.");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To open one or more files, preferably in an existing GUI instance of Praat:");
	MelderInfo_writeLine (U"      praat --open [OPTION]... FILE-NAME...");
	MelderInfo_writeLine (U"   Data files will open in the Objects window, script files in a script window.");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To open one or more files in a new GUI instance of Praat:");
	MelderInfo_writeLine (U"      praat --new-open [OPTION]... FILE-NAME...");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To run a Praat script in a preferably existing GUI instance of Praat:");
	MelderInfo_writeLine (U"      praat --send [OPTION]... SCRIPT-FILE-NAME [SCRIPT-ARGUMENT]...");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To run a Praat script in a new GUI instance of Praat:");
	MelderInfo_writeLine (U"      praat --new-send [OPTION]... SCRIPT-FILE-NAME [SCRIPT-ARGUMENT]...");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   As --send, but potentially presenting a form to query for arguments:");
	MelderInfo_writeLine (U"      praat --send-or-form [OPTION]... SCRIPT-FILE-NAME");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To start up Praat in an interactive command line session:");
	MelderInfo_writeLine (U"      praat [OPTION]... -");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To print the Praat version:");
	MelderInfo_writeLine (U"      praat --version");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"   To print this list of command line options:");
	MelderInfo_writeLine (U"      praat --help");
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"Options:");
	MelderInfo_writeLine (U"  --no-pref-files  don't read or write the preferences file and the buttons file");
	MelderInfo_writeLine (U"  --no-plugins     don't activate the plugins");
	MelderInfo_writeLine (U"  --pref-dir=DIR   set the preferences folder to DIR");
	MelderInfo_writeLine (U"  -u, --utf16      use UTF-16LE output encoding, no BOM (the default on Windows)");
	MelderInfo_writeLine (U"  -8, --utf8       use UTF-8 output encoding (the default on MacOS and Linux)");
	MelderInfo_writeLine (U"  -a, --ansi       use ISO Latin-1 output encoding (lossy, hence not recommended)");
	MelderInfo_writeLine (U"                   (on Windows, use -8 or -a when you redirect to a pipe or file)");
	MelderInfo_writeLine (U"  --trace          switch tracing on at start-up (see Praat > Technical > Debug)");
	MelderInfo_writeLine (U"  --hide-picture   hide the Picture window at start-up");
	MelderInfo_writeLine (U"  --debug=58       set the Debug option to e.g. 58 at start-up");
}

#ifdef _WIN32
	static void *theWinApplicationWindow;
#endif

static bool tryToSwitchToRunningPraat (bool foundTheOpenOption, bool foundTheSendOption, bool foundTheSendOrFormOption) {
	/*
		This function returns true only if we can be certain that we have sent
		the command line to an already running invocation of Praat that is not identical to ourselves
		(though its Praat version has to be identical to ours).
		If there are doubts anywhere, then we just return false,
		because having zero instances of Praat is worse than having two.
	*/
	const integer pidOfCurrentPraat = getpid ();
	if (pidOfCurrentPraat == 0) {
		trace (U"We have no PID, so we will not be able to check that we are different from any running Praat.");
		return false;
	}
	trace (U"Process ID of current Praat: ", pidOfCurrentPraat);
	/*
		Figure out the Process ID of an already running instance of Praat.
	*/
	integer pidOfRunningPraat = 0;   // mutable, to be filled in from file
	#if defined (macintosh)
		/*
			We have to implement this differently from how sendpraat implements this,
			because sendpraat just sends its message to a program with application signature 'PpgB',
			which by default is the current (starting-up) Praat rather than the already running Praat.
			These two instances of Praat can be distinguished by their Process ID.
			We look up the Process ID of the running Praat in a list associated with the running program's Bundle ID.
		*/
		NSRunningApplication *currentPraat = [NSRunningApplication currentApplication];
		NSString *currentBundleIdentifier = [currentPraat bundleIdentifier];   // for instance @"org.praat.Praat"
		trace (U"Current bundle identifier: ", Melder_peek8to32_u ([currentBundleIdentifier UTF8String]));
		NSArray<NSRunningApplication *> *list = [NSRunningApplication runningApplicationsWithBundleIdentifier: currentBundleIdentifier];
		NSRunningApplication *runningPraat = nullptr;
		const integer numberOfPraats = uinteger_to_integer_a ([list count]);
		for (integer ipraat = 1; ipraat <= numberOfPraats; ipraat ++) {
			NSRunningApplication *praat = [list objectAtIndex: ipraat-1];
			pid_t pidOfPraat = [praat processIdentifier];
			trace (U"Process ID ", pidOfPraat);
			if (pidOfPraat != pidOfCurrentPraat) {
				runningPraat = praat;
				pidOfRunningPraat = pidOfPraat;
			}
		}
		if (pidOfRunningPraat == 0) {
			trace (U"There seem to be no other running instances of Praat.");
			return false;
		}
	#elif defined (_WIN32)
		HWND winWindow = HWND (theWinApplicationWindow);
		if (! winWindow)
			return false;
	#elif defined (UNIX)
		integer versionOfRunningPraat = 0;   // mutable, to be filled in from file
		try {
			autofile f = Melder_fopen (& pidFile, "r");
			int numberOfRead = fscanf (f, "%td %td", & pidOfRunningPraat, & versionOfRunningPraat);
			if (numberOfRead < 1) {
				trace (U"No PID in PID file, "
						"so we will not be able to check that Praat is already running.");
				return false;
			}
			if (numberOfRead < 2) {
				trace (U"No Praat version number in PID file, "
						"so we will not be able to check that the same version of Praat is already running.");
				return false;
			}
		} catch (MelderError) {
			Melder_clearError ();   // it is not an error if the PID file does not exist or is broken
			trace (U"PID file does not exist, "
					"so Praat is probably not running yet. We are process ", pidOfCurrentPraat, U".");
			return false;
		}
		const integer versionOfCurrentPraat = Melder_appVersion();
		if (versionOfRunningPraat != versionOfCurrentPraat) {
			trace (U"The current version of Praat differs from the version of the Praat that may already be running, "
					"so we cannot be sure it would respond as we would.");
			return false;
		}
		if (pidOfRunningPraat == pidOfCurrentPraat) {
			trace (U"Very rare condition: a Praat that crashed had the same PID as we have (in an earlier computer session), "
					"so Praat is probably not running yet.");
			return false;
		}
		trace (U"An instance with PID ", pidOfRunningPraat, U" is still running.");
	#endif
	/*
		Bring the running Praat to the foreground.
	*/
	Melder_casual (U"An instance of Praat that is not me is already running.");
	#if defined (macintosh)
		int activationVersion = 1;   // 1 or 2
		if (activationVersion == 1) {   // deprecated since OS X 10.9, but it works, unlike the alternative
			ProcessSerialNumber psnOfRunningPraat;
			GetProcessForPID (pidOfRunningPraat, & psnOfRunningPraat);
			SetFrontProcess (& psnOfRunningPraat);   // this works
		} else if (activationVersion == 2) {
			const bool activated = [runningPraat
				activateWithOptions:
					NSApplicationActivateAllWindows |   // this option seems to be refused (if you take it alone, `activated` will be false)
					NSApplicationActivateIgnoringOtherApps       // (is the activation policy right?)
			];
			Melder_casual (U"activated: ", activated);
		}
	#elif defined (_WIN32)
		if (IsIconic (winWindow))
			ShowWindow (winWindow, SW_RESTORE);
		SetForegroundWindow (winWindow);
	#elif defined (UNIX)
		/*
			TODO: bring Praat to the foreground on GTK.
		*/
	#endif
	if (! foundTheOpenOption && ! foundTheSendOption && ! foundTheSendOrFormOption)
		return true;
	/*
		Send something to the running Praat, and bail out.
	*/
	autoMelderString text32;
	if (foundTheOpenOption) {
		/*
			praat --open [OPTION]... FILE-NAME...
		*/
		for (integer iarg = praatP.argumentNumber; iarg < praatP.argc; iarg ++) {   // do not change praatP.argumentNumber itself (we might return false)
			TRACE
			structMelderFile file { };
			Melder_relativePathToFile (Melder_peek8to32_u (praatP.argv [iarg]), & file);
			conststring32 absolutePath = MelderFile_peekPath (& file);
			MelderString_append (& text32, U"Read from file... ", absolutePath, U"\n");
			trace (U"Argument ", iarg, U": will open path ", absolutePath);
		} // TODO: we could send an openDocuments message instead
	} else if (foundTheSendOption) {
		/*
			praat --send [OPTION]... SCRIPT-FILE-NAME [SCRIPT-ARGUMENT]...
		*/
		MelderString_append (& text32, U"setWorkingDirectory: ");
		structMelderFolder currentFolder { };
		Melder_getCurrentFolder (& currentFolder);
		MelderString_append (& text32, quote_doubleSTR (MelderFolder_peekPath (& currentFolder)).get());
		MelderString_append (& text32, U"\nrunScript: ");
		structMelderFile scriptFile { };
		Melder_relativePathToFile (theCurrentPraatApplication -> batchName.string, & scriptFile);
		conststring32 absolutePath = MelderFile_peekPath (& scriptFile);
		MelderString_append (& text32, quote_doubleSTR (absolutePath).get());
		//TRACE
		trace (U"--send ", absolutePath);
		for (integer iarg = praatP.argumentNumber; iarg < praatP.argc; iarg ++)   // do not change praatP.argumentNumber itself (we might return false)
			MelderString_append (& text32, U", ", quote_doubleSTR (Melder_peek8to32_u (praatP.argv [iarg])).get());
	} else if (foundTheSendOrFormOption) {
		/*
			praat --send-or-form [OPTION]... SCRIPT-FILE-NAME
		*/
		MelderString_append (& text32, U"setWorkingDirectory: ");
		structMelderFolder currentFolder { };
		Melder_getCurrentFolder (& currentFolder);
		MelderString_append (& text32, quote_doubleSTR (MelderFolder_peekPath (& currentFolder)).get());
		MelderString_append (& text32, U"\nrunScriptWithForm: ");
		structMelderFile scriptFile { };
		Melder_relativePathToFile (theCurrentPraatApplication -> batchName.string, & scriptFile);
		conststring32 absolutePath = MelderFile_peekPath (& scriptFile);
		MelderString_append (& text32, quote_doubleSTR (absolutePath).get());
		//TRACE
		trace (U"--send-or-form ", absolutePath);
		Melder_require (praatP.argc - praatP.argumentNumber == 0,
				U"The --send-or-form switch should be followed by precisely one argument, namely the name of the script file.");
	}
	autostring8 text8 = Melder_32to8 (text32.string);
	#if defined (macintosh)
		const int timeOut = 0;
		AESendMode aeOptions = ( timeOut == 0 ? kAENoReply : kAEWaitReply ) | kAECanInteract | kAECanSwitchLayer;
		int appleEventVersion = 2;   // 1, 2 or 3
		if (appleEventVersion == 1) {
			ProcessSerialNumber psnOfRunningPraat;
			GetProcessForPID (pidOfRunningPraat, & psnOfRunningPraat);
			AppleEvent psnProgramDescriptor = { typeNull, nullptr };
			OSErr err = AECreateDesc (typeProcessSerialNumber, & psnOfRunningPraat, sizeof (psnOfRunningPraat), & psnProgramDescriptor);
			if (err != noErr)
				return false;
			AppleEvent appleEvent;
			err = AECreateAppleEvent (758934755, 0, & psnProgramDescriptor, kAutoGenerateReturnID, 1, & appleEvent);
			if (err != noErr) {
				AEDisposeDesc (& psnProgramDescriptor);
				return false;
			}
			err = AEPutParamPtr (& appleEvent, 1, typeChar, text8.get(), (Size) strlen (text8.get()) + 1);
			if (err != noErr) {
				AEDisposeDesc (& appleEvent);
				AEDisposeDesc (& psnProgramDescriptor);
				return false;
			}
			AppleEvent reply;
			OSStatus status = AESendMessage (& appleEvent, & reply, aeOptions, timeOut == 0 ? kNoTimeOut : 60 * timeOut);
			if (status != 0) {
				AEDisposeDesc (& appleEvent);
				AEDisposeDesc (& psnProgramDescriptor);
				return false;   // event not sent correctly
			}
			return true;   // we did send an event to a running non-identical Praat successfully
		} else if (appleEventVersion == 2) {
			pid_t pidOfRunningPraat_pidt = pid_t (pidOfRunningPraat);
			NSAppleEventDescriptor *pidProgramDescriptor = [NSAppleEventDescriptor
					descriptorWithDescriptorType: typeKernelProcessID   bytes: & pidOfRunningPraat_pidt   length: sizeof (pid_t)];
			if (! pidProgramDescriptor)
				return false;
			AppleEvent appleEvent;
			OSErr err = AECreateAppleEvent (758934755, 0, [pidProgramDescriptor aeDesc], kAutoGenerateReturnID, 1, & appleEvent);
			if (err != noErr) {
				// pidProgramDescriptor is autoreleased
				return false;
			}
			err = AEPutParamPtr (& appleEvent, 1, typeChar, text8.get(), (Size) strlen (text8.get()) + 1);
			if (err != noErr) {
				AEDisposeDesc (& appleEvent);
				// pidProgramDescriptor is autoreleased
				return false;
			}
			AppleEvent reply;
			OSStatus status = AESendMessage (& appleEvent, & reply, aeOptions, timeOut == 0 ? kNoTimeOut : 60 * timeOut);
			if (status != 0) {
				AEDisposeDesc (& appleEvent);
				// pidProgramDescriptor is autoreleased
				return false;   // event not sent correctly
			}
			return true;   // we did send an event to a running non-identical Praat successfully
		} else if (appleEventVersion == 3) {   // from 10.11 on
			NSAppleEventDescriptor *pidProgramDescriptor = [NSAppleEventDescriptor
					descriptorWithProcessIdentifier: pidOfRunningPraat];
			if (! pidProgramDescriptor)
				return false;
			NSAppleEventDescriptor *appleEvent = [NSAppleEventDescriptor
				appleEventWithEventClass: 758934755   eventID: 0   targetDescriptor: pidProgramDescriptor
				returnID: kAutoGenerateReturnID   transactionID: 1
			];
			if (! appleEvent)
				return false;
			// somewhere add text8
			NSAppleEventSendOptions nsOptions = (unsigned long) aeOptions;   // identical numbers, fortunately
			NSError *error;
			[appleEvent
				sendEventWithOptions: nsOptions
				timeout: (double) timeOut
				error: & error
			];
			if (error)
				return false;   // event not sent correctly
			return true;   // we did send an event to a running non-identical Praat successfully
		} else {
			Melder_crash (U"Unknown Apple Event version.");
		}
	#elif defined (UNIX)
		autofile f;
		try {
			f = Melder_fopen (& messageFile, "w");
			fprintf (f, "%s", text8.get());
			f.close (& messageFile);
		} catch (MelderError) {
			Melder_clearError ();
			Melder_casual (U"Cannot write message file \"", MelderFile_messageName (& messageFile),
					U"\" (no privilege to write to folder, or disk full).");
			return false;
		}
		if (kill (pidOfRunningPraat, SIGUSR1)) {
			Melder_casual (U"Cannot send message. The program may have crashed.");
			return false;
		}
		return true;
	#elif defined (_WIN32)
		autofile f;
		try {
			f = Melder_fopen (& messageFile, "w");
			fprintf (f, "%s", text8.get());
			f.close (& messageFile);
		} catch (MelderError) {
			Melder_clearError ();
			Melder_casual (U"Cannot write message file \"", MelderFile_messageName (& messageFile),
					U"\" (no privilege to write to folder, or disk full).");
			return false;
		}
		if (SendMessage (HWND (theWinApplicationWindow), WM_APP, 0, 0)) {
			Melder_casual (U"Cannot send message.");
			return false;
		}
		return true;
	#endif
	return false;   // the default
}

/*
	This function translates the command-line arguments to fields in `praatP`.
	It doesn't do too much by itself, because many of the relevant entities (such as preferences folder and tracing file)
	haven't been set yet (because setting these entities depends on the command-line arguments).
*/
static void interpretCommandLineArguments (bool weWereStartedFromTheCommandLine, int argc, char **argv) {
	//TRACE   // the tracing file may not have been set yet, so this will probably go to stderr
	for (int iarg = 0; iarg < argc; iarg ++)
		trace (U"arg ", iarg, U": <<", Melder_peek8to32_u (argv [iarg]), U">>");

	praatP.argc = argc;
	praatP.argv = argv;
	praatP.argumentNumber = 1;

	/*
		Running Praat from the command line.
	*/
	while (praatP.argumentNumber < argc && argv [praatP.argumentNumber] [0] == '-') {
		if (strequ (argv [praatP.argumentNumber], "-")) {
			praatP.hasCommandLineInput = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--run")) {
			praatP.foundTheRunSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--open")) {
			praatP.foundTheOpenSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--new-open")) {
			praatP.foundTheNewSwitch = true;
			praatP.foundTheOpenSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--send")) {
			praatP.foundTheSendSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--new-send")) {
			praatP.foundTheNewSwitch = true;
			praatP.foundTheSendSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--send-or-form")) {
			praatP.foundTheSendOrFormSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--new-send-or-form")) {
			praatP.foundTheNewSwitch = true;
			praatP.foundTheSendOrFormSwitch = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--no-pref-files")) {
			praatP.ignorePreferenceFiles = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--no-plugins")) {
			praatP.ignorePlugins = true;
			praatP.argumentNumber += 1;
		} else if (strnequ (argv [praatP.argumentNumber], "--pref-dir=", 11)) {
			Melder_setPreferencesFolder (Melder_peek8to32_u (argv [praatP.argumentNumber] + 11));
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--version")) {
			Melder_information (Melder_upperCaseAppName(), U" ", Melder_appVersionSTR(),
					U" (", Melder_appMonthSTR(), U" ", Melder_appDay(), U" ", Melder_appYear(), U")");
			exit (0);
		} else if (strequ (argv [praatP.argumentNumber], "--trace")) {
			Melder_setTracing (true);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--hide-picture")) {
			praatP.commandLineOptions.hidePicture = true;
			praatP.argumentNumber += 1;
		} else if (strnequ (argv [praatP.argumentNumber], "--debug=", 8)) {
			Melder_debug = atoi (argv [praatP.argumentNumber] + 8);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--help")) {
			MelderInfo_open ();
			printHelp ();
			MelderInfo_close ();
			exit (0);
		} else if (strequ (argv [praatP.argumentNumber], "-8") || strequ (argv [praatP.argumentNumber], "--utf8")) {
			MelderConsole::setEncoding (MelderConsole::Encoding::UTF8);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "-u") || strequ (argv [praatP.argumentNumber], "--utf16")) {
			MelderConsole::setEncoding (MelderConsole::Encoding::UTF16);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "-a") || strequ (argv [praatP.argumentNumber], "--ansi")) {
			MelderConsole::setEncoding (MelderConsole::Encoding::ANSI);
			praatP.argumentNumber += 1;
		#if defined (macintosh)
		} else if (strequ (argv [praatP.argumentNumber], "-NSDocumentRevisionsDebugMode")) {
			(void) 0;   // ignore this option, which was added by Xcode
			praatP.argumentNumber += 2;   // jump over the argument, which is usually "YES" (this jump works correctly even if this argument is missing)
		} else if (strnequ (argv [praatP.argumentNumber], "-psn_", 5)) {
			(void) 0;   // ignore this option, which was added by the Finder, perhaps when dragging a file on Praat (Process Serial Number)
			trace (U"Process serial number ", Melder_peek8to32_u (argv [praatP.argumentNumber]));
			praatP.argumentNumber += 1;
		#endif
		} else if (strequ (argv [praatP.argumentNumber], "-sgi") ||
			strequ (argv [praatP.argumentNumber], "-motif") ||
			strequ (argv [praatP.argumentNumber], "-cde") ||
			strequ (argv [praatP.argumentNumber], "-solaris") ||
			strequ (argv [praatP.argumentNumber], "-hp") ||
			strequ (argv [praatP.argumentNumber], "-sun4") ||
			strequ (argv [praatP.argumentNumber], "-mac") ||
			strequ (argv [praatP.argumentNumber], "-win32") ||
			strequ (argv [praatP.argumentNumber], "-linux") ||
			strequ (argv [praatP.argumentNumber], "-cocoa") ||
			strequ (argv [praatP.argumentNumber], "-chrome")
		) {
			praatP.argumentNumber += 1;
		} else {
			MelderInfo_open ();
			MelderInfo_writeLine (U"Unrecognized command line option ", Melder_peek8to32_u (argv [praatP.argumentNumber]), U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
	}
	const bool thereIsAFileNameInTheArgumentList = ( praatP.argumentNumber < argc );
	trace (U"Start-up flags: cornsff = ",
		weWereStartedFromTheCommandLine, praatP.foundTheOpenSwitch, praatP.foundTheRunSwitch,
		praatP.foundTheNewSwitch, praatP.foundTheSendSwitch, praatP.foundTheSendOrFormSwitch, thereIsAFileNameInTheArgumentList
	);
	if (praatP.foundTheRunSwitch && praatP.foundTheOpenSwitch) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Conflicting command line switches --run and --open (or --new-open).", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheRunSwitch && praatP.foundTheSendSwitch) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Conflicting command line switches --run and --send (or --new-send).", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheRunSwitch && praatP.foundTheSendOrFormSwitch) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Conflicting command line switches --run and --send-or-form (or --new-send-or-form).", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheOpenSwitch && praatP.foundTheSendSwitch) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Conflicting command line switches --open (or --new-open) and --send (or --new-send).", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheOpenSwitch && praatP.foundTheSendOrFormSwitch) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Conflicting command line switches --open (or --new-open) and --send-or-form (or --new-send-or-form).", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheSendSwitch && praatP.foundTheSendOrFormSwitch) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Conflicting command line switches --send (or --new-send) and --send-or-form (or --new-send-or-form).", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheRunSwitch && ! thereIsAFileNameInTheArgumentList) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"The switch --run requires a script file name.", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheSendSwitch && ! thereIsAFileNameInTheArgumentList) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"The switch --send requires a script file name.", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheSendOrFormSwitch && ! thereIsAFileNameInTheArgumentList) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"The switch --send-or-form requires a script file name.", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	if (praatP.foundTheOpenSwitch && ! thereIsAFileNameInTheArgumentList) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"The switch --open requires at least one file name.", U"\n");
		printHelp ();
		MelderInfo_close ();
		exit (-1);
	}
	Melder_batch =
		! praatP.foundTheOpenSwitch &&
		! praatP.foundTheSendSwitch &&
		! praatP.foundTheSendOrFormSwitch &&
		(praatP.foundTheRunSwitch || thereIsAFileNameInTheArgumentList && weWereStartedFromTheCommandLine)   // this line to be removed
	;
	bool userWantsGui = ! Melder_batch;
	praatP.fileNamesCameInByDropping =
		userWantsGui &&
		! praatP.foundTheRunSwitch && ! praatP.foundTheOpenSwitch && ! praatP.foundTheSendSwitch && ! praatP.foundTheSendOrFormSwitch &&
		thereIsAFileNameInTheArgumentList
	;   // doesn't happen on the Mac
	trace (U"Did file names come in by dropping? ", praatP.fileNamesCameInByDropping);
	praatP.userWantsToOpen = userWantsGui && (praatP.foundTheOpenSwitch || praatP.fileNamesCameInByDropping);
	trace (U"User wants to open: ", praatP.userWantsToOpen);
	praatP.userWantsToSend = userWantsGui && praatP.foundTheSendSwitch;
	trace (U"User wants to send: ", praatP.userWantsToSend);
	praatP.userWantsToSendOrForm = userWantsGui && praatP.foundTheSendOrFormSwitch;
	trace (U"User wants to send-or-form: ", praatP.userWantsToSendOrForm);
	praatP.userWantsExistingInstance = (praatP.userWantsToOpen || praatP.userWantsToSend || praatP.userWantsToSendOrForm) && ! praatP.foundTheNewSwitch
		|| (userWantsGui && ! weWereStartedFromTheCommandLine);
	trace (U"User wants existing instance: ", praatP.userWantsExistingInstance);

	if (Melder_batch || praatP.userWantsToSend || praatP.userWantsToSendOrForm) {
		Melder_assert (praatP.argumentNumber < argc);
		/*
			We now get the script file name. It is next on the command line
			(not necessarily *last* on the line, because there may be script arguments after it).
		*/
		MelderString_copy (& theCurrentPraatApplication -> batchName, Melder_peek8to32_u (argv [praatP.argumentNumber ++]));
		if (praatP.hasCommandLineInput)
			Melder_throw (U"Cannot have both command line input and a script file.");
	} else {
		MelderString_copy (& theCurrentPraatApplication -> batchName, U"");
	}
	//Melder_casual (U"Script file name <<", theCurrentPraatApplication -> batchName.string, U">>");

	if (!! thePraatStandAloneScriptText) {
		Melder_batch = true;
		userWantsGui = false;
		if (praatP.foundTheRunSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --run is not compatible with running a stand-alone script.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (praatP.foundTheOpenSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --open (or --new-open) is not compatible with running a stand-alone script.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (praatP.foundTheSendSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --send (or --new-send) is not compatible with running a stand-alone script.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (praatP.foundTheSendOrFormSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --send-or-form (or --new-send-or-form) is not compatible with running a stand-alone script.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (thereIsAFileNameInTheArgumentList) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"Having a file name is not compatible with running a stand-alone script.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		Melder_assert (! praatP.userWantsToOpen);
		Melder_assert (! praatP.userWantsExistingInstance);
	}

	/*
		Running the Praat shell from the command line:
			praat -
	*/
	if (praatP.hasCommandLineInput) {
		Melder_batch = true;
		userWantsGui = false;
		if (praatP.foundTheRunSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --run is not compatible with running Praat interactively from the command line.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (praatP.foundTheOpenSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --open (or --new-open) is not compatible with running Praat interactively from the command line.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (praatP.foundTheSendSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --send (or --new-send) is not compatible with running Praat interactively from the command line.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (praatP.foundTheSendOrFormSwitch) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"The switch --send-form (or --new-send-form) is not compatible with running Praat interactively from the command line.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		if (thereIsAFileNameInTheArgumentList) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"Having a file name is not compatible with running Praat interactively from the command line.", U"\n");
			printHelp ();
			MelderInfo_close ();
			exit (-1);
		}
		Melder_assert (! praatP.userWantsToOpen);
		Melder_assert (! praatP.userWantsExistingInstance);
	}
}

static void setPreferencesFolder () {
	/*
		Get the app's preferences folder (if not yet set by the --pref-dir option):
			in Praat 6:
				"/home/miep/.praat-dir" (Linux)
				"/Users/miep/Library/Preferences/Praat Prefs" (Mac)
				"C:\Users\Miep\Praat" (Windows)
			in Praat 7:
				$XDG_CONFIG_HOME/praat or else "/home/miep/.config/praat" (Linux)
				"/Users/miep/Library/Application Support/Praat" (Mac)
				"C:\Users\Miep\AppData\Roaming\Praat" (Windows)
		and construct a preferences-file name and a script-buttons-file name like
			/home/miep/.praat-dir/prefs5
			/home/miep/.praat-dir/buttons5
			/home/miep/.config/praat/Preferences.txt
			/home/miep/.config/praat/Buttons.txt
		or
			/Users/miep/Library/Preferences/Praat Prefs/Prefs5
			/Users/miep/Library/Preferences/Praat Prefs/Buttons5
			/Users/miep/Library/Application Support/Praat/Preferences.txt
			/Users/miep/Library/Application Support/Praat/Buttons.txt
		or
			C:\Users\Miep\Praat\Preferences5.ini
			C:\Users\Miep\Praat\Buttons5.ini
			C:\Users\Miep\AppData\Roaming\Praat\Preferences.txt
			C:\Users\Miep\AppData\Roaming\Praat\Buttons.txt
		Also create names for message and tracing files.

		Platform-dependently preferred cache location:
			- same as prefs folder (Mac, Windows)
			- $XDG_CACHE_HOME/praat or else /home/miep/.cache/praat (Linux)
		Platform-dependently preferred log file location:
			- same as prefs folder (Mac, Windows)
			- $XDG_STATE_HOME/praat or else /home/miep/.local/state/praat (Linux)
		Platform-dependently preferred data file location:
			- same as prefs folder (Mac, Windows)
			- $XDG_DATA_HOME/praat or else /home/miep/.local/share/praat (Linux)
		Platform-dependently preferred message file location:
			- same as prefs folder (Mac, Windows)
			- $XDG_RUNTIME_DIR/praat (Linux, access mode 0700)
	*/
	if (MelderFolder_isNull (Melder_preferencesFolder())) {   // not yet set by the --pref-dir option?
		try {
			Melder_setPreferencesFolder (MelderFolder_peekPath (Melder_preferencesFolder5()));
			MelderFolder_create (Melder_preferencesFolder());
		} catch (MelderError) {
			/*
				If we arrive here, the directory could not be created,
				and all the files are null. Praat should nevertheless start up.
			*/
			Melder_clearError ();
		}
	}
	if (! MelderFolder_isNull (Melder_preferencesFolder())) {
		#if 0
			MelderFolder_getFile (Melder_preferencesFolder(), U"Preferences.txt", & prefsFile);
			MelderFolder_getFile (Melder_preferencesFolder(), U"Buttons.txt", & buttonsFile);
			MelderFolder_getFile (Melder_preferencesFolder(), U"Tracing.txt", & tracingFile);
			#if defined (UNIX)
				MelderFolder_getFile (Melder_preferencesFolder(), U"pid.txt", & pidFile);
			#endif
			#if defined (UNIX) || defined (_WIN32)
				MelderFolder_getFile (Melder_preferencesFolder(), U"Message.txt", & messageFile);
			#endif
		#else
			#if defined (UNIX)
				MelderFolder_getFile (Melder_preferencesFolder(), U"prefs5", & prefsFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"buttons5", & buttonsFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"pid", & pidFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"message", & messageFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"tracing", & tracingFile);
			#elif defined (_WIN32)
				MelderFolder_getFile (Melder_preferencesFolder(), U"Preferences5.ini", & prefsFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"Buttons5.ini", & buttonsFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"Message.txt", & messageFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"Tracing.txt", & tracingFile);
			#elif defined (macintosh)
				MelderFolder_getFile (Melder_preferencesFolder(), U"Prefs5", & prefsFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"Buttons5", & buttonsFile);
				MelderFolder_getFile (Melder_preferencesFolder(), U"Tracing.txt", & tracingFile);
			#endif
		#endif
		Melder_tracingToFile (& tracingFile);
	}
}

void praat_init (conststring32 title,
	conststring32 versionText, integer versionNumber,
	integer year, integer month, integer day,
	conststring32 firstPartOfEmailAddress, conststring32 secondPartOfEmailAddress,
	int argc, char **argv
) {
	setThePraatLocale ();
	Melder_init ();
	const bool weWereStartedFromTheCommandLine = tryToAttachToTheCommandLine ();
	MelderConsole_init ();

	/*
		Construct a main-window title like "Praat".
		Construct an app name like "praat" for file and folder names.
	*/
	Melder_setAppName (title && title [0] != U'\0' ? title : U"Praat");
	Melder_setAppVersion (versionText, versionNumber);
	Melder_setAppDate (year, month, day);
	Melder_setAppContactAddress (firstPartOfEmailAddress, secondPartOfEmailAddress);

	/*
		Get the home folder, e.g. "/home/miep/", or "/Users/miep/", or just "/".
	*/
	Melder_getHomeDir (& homeDir);

	interpretCommandLineArguments (weWereStartedFromTheCommandLine, argc, argv);

	setPreferencesFolder ();

	/*
		Remember the current directory. Useful only for scripts run from batch.
	*/
	Melder_rememberShellDirectory ();

	installPraatShellPreferences ();

	theCurrentPraatApplication -> batch = Melder_batch;

	#if defined (NO_GUI)
		if (! Melder_batch) {
			fprintf (Melder_stderr, "A no-GUI edition of Praat cannot be used interactively. "
					"Supply \"--run\" and a script file name on the command line.\n");
			exit (1);
		}
	#endif

	/*
		Check whether we can transfer control to an already running instance of Praat.
	*/
	#if defined (macintosh)
		NSApplication *theApp = [GuiCocoaApplication sharedApplication];   // initialize, so that our bundle identifier exists even if we started from outside Xcode
	#elif defined (_WIN32)
		theWinApplicationWindow = GuiWin_initialize1 (Melder_upperCaseAppName());
	#endif
	if (praatP.userWantsExistingInstance)
		if (tryToSwitchToRunningPraat (praatP.userWantsToOpen, praatP.userWantsToSend, praatP.userWantsToSendOrForm))
			exit (0);

	#ifdef UNIX
		if (! Melder_batch) {
			/*
				Write our process id into the pid file.
				Messages from "sendpraat" are caught very early this way,
				though they will be responded to much later.
			*/
			try {
				autofile f = Melder_fopen (& pidFile, "w");
				fprintf (f, "%td %td", integer (getpid ()), integer (Melder_appVersion()));
				f.close (& pidFile);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	#endif
	#if defined (_WIN32)
		if (! Melder_batch)
			motif_win_setUserMessageCallback (cb_userMessage);
	#endif
	#if defined (macintosh)
		if (! Melder_batch)
			mac_setUserMessageCallback (cb_userMessage);   // not earlier
	#endif
	Gui_setQuitApplicationCallback (cb_quitApplication);

	GuiWindow raam = nullptr;
	if (! Melder_batch) {
		trace (U"starting the GUI application");
		Machine_initLookAndFeel (argc, argv);
		#if gtk
			trace (U"locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
			g_set_application_name (Melder_peek32to8 (title));
			trace (U"locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		#elif motif
			GuiWin_initialize2 (argc, argv);
		#elif cocoa
			/*
				We want to get rid of the Search field in the help menu.
				By default, such a Search field will come up automatically when we create a menu with the title "Help".
				By changing this title to "SomeFakeTitleOfAMenuThatWillNeverBeInstantiated",
				we trick macOS into thinking that our help menu is called "SomeFakeTitleOfAMenuThatWillNeverBeInstantiated".
				As a result, the Search field will come up only when we create a menu
				titled "SomeFakeTitleOfAMenuThatWillNeverBeInstantiated", which is never.
			*/
			theApp.helpMenu = [[NSMenu alloc] initWithTitle:@"SomeFakeTitleOfAMenuThatWillNeverBeInstantiated"];
		#endif

		trace (U"creating and installing the Objects window");
		char32 objectWindowTitle [100];
		Melder_sprint (objectWindowTitle,100, Melder_upperCaseAppName(), U" Objects");
		double x, y;
		trace (U"locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		Gui_getWindowPositioningBounds (& x, & y, nullptr, nullptr);
		trace (U"locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		theCurrentPraatApplication -> topShell = raam = GuiWindow_create (x + 10, y, WINDOW_WIDTH, WINDOW_HEIGHT, 450, 250,
				objectWindowTitle, gui_cb_quit, nullptr, 0);
		trace (U"locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		#if motif
			GuiApp_setApplicationShell (theCurrentPraatApplication -> topShell -> d_xmShell);
		#endif
		trace (U"before objects window shows locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		GuiThing_show (raam);
		trace (U"after objects window shows locale ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
	}
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classStringSet);
	Thing_recognizeClassByOtherName (classStringSet, U"SortedSetOfString");
	if (Melder_batch) {
		Melder_backgrounding = true;
		trace (U"adding menus without GUI");
		praat_addMenus (nullptr);
		trace (U"adding fixed buttons without GUI");
		praat_addFixedButtons (nullptr);
	} else {
		#ifdef macintosh
			trace (U"initializing the Gui early (MacOS)");
			AEInstallEventHandler (758934755, 0, (AEEventHandlerProcPtr) (mac_processSignal8), 0, false);   // for receiving sendpraat
			injectMessageAndInformationProcs (raam);   // BUG: default Melder_assert would call printf recursively!!!
		#endif
		trace (U"creating the menu bar in the Objects window");
		GuiWindow_addMenuBar (raam);
		praatP.menuBar = raam;
		praat_addMenus (praatP.menuBar);

		trace (U"creating the object list in the Objects window");
		GuiLabel_createShown (raam, 3, -250, Machine_getMenuBarBottom () + 5, Machine_getMenuBarBottom () + 5 + Gui_LABEL_HEIGHT, U"Objects:", 0);
		praatList_objects = GuiList_create (raam, 0, -250, Machine_getMenuBarBottom () + 26, -100, true, U" Objects ");
		GuiList_setSelectionChangedCallback (praatList_objects, gui_cb_list_selectionChanged, nullptr);
		GuiThing_show (praatList_objects);
		praat_addFixedButtons (raam);

		trace (U"creating the dynamic menu in the Objects window");
		praat_actions_createDynamicMenu (raam);
		trace (U"showing the Objects window");
		GuiThing_show (raam);
		#ifdef UNIX
			if (! praatP.ignorePreferenceFiles)
				Preferences_read (& prefsFile);
		#endif
		#if ! defined (macintosh)
			trace (U"initializing the Gui late (Windows and Linux)");
			injectMessageAndInformationProcs (theCurrentPraatApplication -> topShell);   // Mac: done this earlier
		#endif
		Melder_setHelpProc (helpProc);
	}
	Data_setPublishProc (publishProc);
	theCurrentPraatApplication -> manPages = ManPages_create ().releaseToAmbiguousOwner();

	trace (U"creating the Picture window");
	trace (U"before picture window shows: locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
	if (! praatP.dontUsePictureWindow)
		praat_picture_init (! praatP.commandLineOptions.hidePicture);
	trace (U"after picture window shows: locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
}

static void executeStartUpFile (MelderFolder startUpDirectory, conststring32 fileNameHead, conststring32 fileNameTail) {
	char32 name [256];
	Melder_sprint (name,256, fileNameHead, Melder_lowerCaseAppName(), fileNameTail);
	if (! MelderFolder_isNull (startUpDirectory)) {   // should not occur on modern systems
		structMelderFile startUp { };
		MelderFolder_getFile (startUpDirectory, name, & startUp);
		if (! MelderFile_readable (& startUp))
			return;   // it's OK if the file doesn't exist
		try {
			praat_executeScript_noGUI (& startUp);
		} catch (MelderError) {
			Melder_flushError (Melder_upperCaseAppName(), U": start-up file ", & startUp, U" not completed.");
		}
	}
}

#if gtk
	#include <gdk/gdkkeysyms.h>
	static gint theKeySnooper (GtkWidget *widget, GdkEventKey *event, gpointer data) {
		trace (U"keyval ", event -> keyval, U", type ", event -> type);
		if ((event -> keyval == GDK_KEY_Tab || event -> keyval == GDK_KEY_ISO_Left_Tab) && event -> type == GDK_KEY_PRESS) {
			using TabCallback = void (*) (GuiObject, gpointer);
			trace (U"tab key pressed in window ", Melder_pointer (widget));
			constexpr bool theTabKeyShouldWorkEvenIfNumLockIsOn = true;
			constexpr uint32 theProbableNumLockModifierMask = GDK_MOD2_MASK;
			constexpr uint32 modifiersToIgnore = ( theTabKeyShouldWorkEvenIfNumLockIsOn ? theProbableNumLockModifierMask : 0 );
			constexpr uint32 modifiersNotToIgnore = GDK_MODIFIER_MASK & ~ modifiersToIgnore;
			if ((event -> state & modifiersNotToIgnore) == 0) {
				if (GTK_IS_WINDOW (widget)) {
					GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
					trace (U"tab pressed in GTK window ", Melder_pointer (shell));
					TabCallback tabCallback = (TabCallback) g_object_get_data (G_OBJECT (widget), "tabCallback");
					if (tabCallback) {
						trace (U"a tab callback exists");
						void *tabClosure = g_object_get_data (G_OBJECT (widget), "tabClosure");
						tabCallback (widget, tabClosure);
						return true;
					}
				}
			} else if ((event -> state & modifiersNotToIgnore) == GDK_SHIFT_MASK) {
				if (GTK_IS_WINDOW (widget)) {
					GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
					trace (U"shift-tab pressed in GTK window ", Melder_pointer (shell));
					TabCallback tabCallback = (TabCallback) g_object_get_data (G_OBJECT (widget), "shiftTabCallback");
					if (tabCallback) {
						trace (U"a shift tab callback exists");
						void *tabClosure = g_object_get_data (G_OBJECT (widget), "shiftTabClosure");
						tabCallback (widget, tabClosure);
						return true;
					}
				}
			}
		}
		trace (U"end");
		return false;   // pass event on
	}
#endif

void praat_run () {
	trace (U"adding menus, second round");
	praat_addMenus2 ();
	trace (U"locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));

	trace (U"adding the Quit command");
	praat_addMenuCommand (U"Objects", U"Praat", U"-- quit --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Praat", Melder_cat (U"Quit ", Melder_upperCaseAppName ()), nullptr, GuiMenu_UNHIDABLE | 'Q' | GuiMenu_NO_API, DO_Quit);
	praat_addMenuCommand (U"Objects", U"Praat", U"Quit", nullptr, GuiMenu_HIDDEN | GuiMenu_NO_API, DO_Quit);

	trace (U"read the preferences file, and notify those who want to be notified of this");
	/* ...namely, those who already have a window (namely, the Picture window),
	 * and those that regard the start of a new session as a meaningful event
	 * (namely, the session counter and the cross-session memory counter).
	 */
	if (! praatP.ignorePreferenceFiles) {
		Preferences_read (& prefsFile);
		if (! praatP.dontUsePictureWindow)
			praat_picture_prefsChanged ();
		praat_statistics_prefsChanged ();
	}

	praatP.phase = praat_STARTING_UP;

	trace (U"execute start-up file(s)");
	/*
	 * On Unix and the Mac, we try no less than three start-up file names.
	 */
	#if defined (UNIX) || defined (macintosh)
		structMelderFolder usrLocal { };
		Melder_pathToFolder (U"/usr/local", & usrLocal);
		executeStartUpFile (& usrLocal, U"", U"-startUp");
	#endif
	#if defined (UNIX) || defined (macintosh)
		executeStartUpFile (& homeDir, U".", U"-user-startUp");   // not on Windows (empty file name error)
	#endif
	#if defined (UNIX) || defined (macintosh) || defined (_WIN32)
		executeStartUpFile (& homeDir, U"", U"-user-startUp");
	#endif

	if (! MelderFolder_isNull (Melder_preferencesFolder()) && ! praatP.ignorePlugins) {
		trace (U"install plug-ins");
		trace (U"locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		/*
			The Praat phase should remain praat_STARTING_UP,
			because any added commands must not be included in the buttons file.
			In plug-ins, pause windows should therefore be made modal.
		*/
		structMelderFile searchPattern { };
		MelderFolder_getFile (Melder_preferencesFolder(), U"plugin_*", & searchPattern);
		try {
			autoSTRVEC folderNames = folderNames_STRVEC (MelderFile_peekPath (& searchPattern));
			for (integer i = 1; i <= folderNames.size; i ++) {
				structMelderFolder pluginFolder { };
				structMelderFile plugin { };
				MelderFolder_getSubfolder (Melder_preferencesFolder(), folderNames [i].get(), & pluginFolder);
				MelderFolder_getFile (& pluginFolder, U"setup.praat", & plugin);
				if (MelderFile_readable (& plugin)) {
					Melder_backgrounding = true;
					try {
						praat_executeScript_noGUI (& plugin);
					} catch (MelderError) {
						Melder_flushError (Melder_upperCaseAppName(), U": plugin ", & plugin, U" contains an error.");
					}
					Melder_backgrounding = false;
				}
			}
		} catch (MelderError) {
			Melder_clearError ();   // in case Strings_createAsDirectoryList () threw an error
		}
	}

	/*
		Check the locale, to ensure identical behaviour on all computers.
	*/
	Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // check locale settings; because of the required file portability Praat cannot stand "1,5"
	Melder_assert (Melder_isHorizontalOrVerticalSpace (' '));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\r'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\n'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\t'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\f'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\v'));

	{
		double *a = nullptr;
		double *b = & a [0];
		Melder_assert (! b);
	}

	/*
		According to ISO 30112, a non-breaking space is not a space.
		We do not agree, as long as spaces are assumed to be word breakers:
		non-breaking spaces are used to prevent line breaks, not to prevent word breaks.
		For instance, in English it's possible to insert a non-breaking space
		after "e.g." in "...take drastic measures, e.g. pose a ban on...",
		just because otherwise a line would end in a period that does not signal end of sentence;
		this use does not mean that "e.g." and "pose" aren't separate words.
	*/
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_NO_BREAK_SPACE));

	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_OGHAM_SPACE_MARK));   // ISO 30112

	/*
		According to ISO 30112, a Mongolian vowel separator is a space.
		However, this character is used to separate vowels *within* a word,
		so it should not be a word breaker.
		This means that as long as all spaces are assumed to be word breakers,
		this character cannot be a space.
	*/
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_MONGOLIAN_VOWEL_SEPARATOR));

	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EN_QUAD));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EM_QUAD));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EN_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_THREE_PER_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_FOUR_PER_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_SIX_PER_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_FIGURE_SPACE));   // questionable
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_PUNCTUATION_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_THIN_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_HAIR_SPACE));   // ISO 30112
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_SPACE));   // questionable
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_NON_JOINER));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_JOINER));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_NO_BREAK_SPACE));   // this is the byte-order mark!
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_LINE_SEPARATOR));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_PARAGRAPH_SEPARATOR));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_NARROW_NO_BREAK_SPACE));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_LEFT_TO_RIGHT_EMBEDDING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_RIGHT_TO_LEFT_EMBEDDING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_POP_DIRECTIONAL_FORMATTING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_LEFT_TO_RIGHT_OVERRIDE));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_RIGHT_TO_LEFT_OVERRIDE));
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_MEDIUM_MATHEMATICAL_SPACE));   // ISO 30112
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_WORD_JOINER));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_FUNCTION_APPLICATION));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INVISIBLE_TIMES));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INVISIBLE_SEPARATOR));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INHIBIT_SYMMETRIC_SWAPPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ACTIVATE_SYMMETRIC_SWAPPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INHIBIT_ARABIC_FORM_SHAPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ACTIVATE_ARABIC_FORM_SHAPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_NATIONAL_DIGIT_SHAPES));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_NOMINAL_DIGIT_SHAPES));
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_IDEOGRAPHIC_SPACE));   // ISO 30112; occurs on Japanese computers

	{ unsigned char dummy = 200;
		Melder_assert ((int) dummy == 200);
	}
	Melder_assert (integer_abs (-1000) == integer (1000));
	{ int dummy = 200;
		Melder_assert ((int) (signed char) dummy == -56);   // bingeti8 relies on this
		Melder_assert ((int) (unsigned char) dummy == 200);
		Melder_assert ((double) dummy == 200.0);
		Melder_assert ((double) (signed char) dummy == -56.0);
		Melder_assert ((double) (unsigned char) dummy == 200.0);
	}
	{ int64 dummy = 200;
		Melder_assert ((int) (signed char) dummy == -56);
		Melder_assert ((int) (unsigned char) dummy == 200);
		Melder_assert ((double) dummy == 200.0);
		Melder_assert ((double) (signed char) dummy == -56.0);
		Melder_assert ((double) (unsigned char) dummy == 200.0);
	}
	{ uint16 dummy = 40000;
		Melder_assert ((int) (int16) dummy == -25536);   // bingeti16 relies on this
		Melder_assert ((short) (int16) dummy == -25536);   // bingete16 relies on this
		Melder_assert ((integer) dummy == 40000);   // Melder_integer relies on this
		Melder_assert ((double) dummy == 40000.0);
		Melder_assert ((double) (int16) dummy == -25536.0);
	}
	{ unsigned int dummy = 40000;
		Melder_assert ((int) (int16) dummy == -25536);
		Melder_assert ((short) (int16) dummy == -25536);
		Melder_assert ((integer) dummy == 40000);   // Melder_integer relies on this
		Melder_assert ((double) dummy == 40000.0);
		Melder_assert ((double) (int16) dummy == -25536.0);
	}
	{
		int64 dummy = 1000000000000;
		if (! str32equ (Melder_integer (dummy), U"1000000000000"))
			Melder_crash (U"The number 1000000000000 is mistakenly written on this machine as ", dummy, U".");
	}
	{ uint32 dummy = 0xffffffff;
		Melder_assert ((int64) dummy == 4294967295LL);
		Melder_assert (str32equ (Melder_integer (dummy), U"4294967295"));
		Melder_assert (double (dummy) == 4294967295.0);
	}
	{ double dummy = 3000000000.0;
		Melder_assert ((uint32) dummy == 3000000000);
	}
	{
		Melder_assert (Melder_length (U"hello") == 5);
		Melder_assert (str32ncmp (U"hellogoodbye", U"hellogee", 6) == 0);
		Melder_assert (str32ncmp (U"hellogoodbye", U"hellogee", 7) > 0);
		Melder_assert (str32str (U"hellogoodbye", U"ogo"));
		Melder_assert (! str32str (U"hellogoodbye", U"oygo"));
	}
	Melder_assert (isundef (undefined));
	Melder_assert (isinf (1.0 / 0.0));
	Melder_assert (isnan (0.0 / 0.0));
	{
		double x = sqrt (-10.0);
		Melder_assert (isundef (x));
		x = NUMsqrt_u (-10.0);
		Melder_assert (isundef (x));
	}
	Melder_assert (isdefined (0.0));
	Melder_assert (isdefined (1e300));
	Melder_assert (isundef (double (1e160 * 1e160)));
	Melder_assert (isundef (pow (10.0, 330)));
	Melder_assert (isundef (0.0 / 0.0));
	Melder_assert (isundef (1.0 / 0.0));
	Melder_assert (undefined != undefined);
	Melder_assert (undefined - undefined != 0.0);
	Melder_assert ((1.0/0.0) == (1.0/0.0));
	Melder_assert ((1.0/0.0) - (1.0/0.0) != 0.0);
	{
		/*
			Assumptions made in abcio.cpp:
			`frexp()` returns an infinity if its argument is an infinity,
			and not-a-number if its argument is not-a-number.
		*/
		int exponent;
		Melder_assert (isundef (frexp (HUGE_VAL, & exponent)));
		Melder_assert (isundef (frexp (0.0/0.0, & exponent)));
		Melder_assert (isundef (frexp (undefined, & exponent)));
		/*
			The following relies on the facts that:
			- positive infinity is not less than 1.0 (because it is greater than 1.0)
			- NaN is not less than 1.0 (because it is not ordered)
			
			Note: we cannot replace `! (... < 1.0)` with `... >= 1.0`,
			because `! (NaN < 1.0)` is true but `NaN >= 1.0` is false.
		*/
		Melder_assert (! (frexp (HUGE_VAL, & exponent) < 1.0));
		Melder_assert (! (frexp (0.0/0.0, & exponent) < 1.0));
		Melder_assert (! (frexp (undefined, & exponent) < 1.0));
	}
	Melder_assert (str32equ (Melder_integer (1234567), U"1234567"));
	Melder_assert (str32equ (Melder_integer (-1234567), U"-1234567"));
	MelderColour notExplicitlyInitialized;
	Melder_assert (str32equ (Melder_colour (notExplicitlyInitialized), U"{0,0,0}"));
	Melder_assert (str32equ (Melder_colour (MelderColour (0.25, 0.50, 0.875)), U"{0.25,0.5,0.875}"));
	{
		VEC xn;   // "uninitialized", but initializes x.cells to nullptr and x.size to 0
		Melder_assert (! xn.cells);
		Melder_assert (xn.size == 0);
		VEC xn2 = xn;   // copy construction
		xn2 = xn;   // copy assignment
		VEC x { };
		Melder_assert (! x.cells);
		Melder_assert (x.size == 0);
		constVEC xnc;   // zero-initialized
		Melder_assert (! xnc.cells);
		Melder_assert (xnc.size == 0);
		constVEC xc { };
		Melder_assert (! xc.cells);
		Melder_assert (xc.size == 0);
		MAT yn;
		Melder_assert (! yn.cells);
		Melder_assert (yn.nrow == 0);
		Melder_assert (yn.ncol == 0);
		MAT y { };
		Melder_assert (! y.cells);
		Melder_assert (y.nrow == 0);
		Melder_assert (y.ncol == 0);
		//autoMAT z {y.cells,y.nrow,y.ncol};   // "No matching constructor for initialization of autoMAT" (2021-04-05)
		autoMAT a = autoMAT { };
		Melder_assert (! a.cells);
		Melder_assert (a.nrow == 0);
		Melder_assert (a.ncol == 0);
		//a = z.move();
		//double q [11];
		//autoVEC s { & q [1], 10 };   // "No matching constructor for initialization of autoVEC" (2021-04-05)
		//autoVEC b { x };   // "No matching constructor for initialization of autoVEC" (2021-04-05)
		//autoVEC c = x;   // "No viable conversion from VEC to autoVEC" (2021-04-05)
		double aa [] = { 3.14, 2.717999999999999971578290569595992565155029296875 };   // the second number is 765048986699563 / 2^48
		VEC x3 (aa, 2);   // initializes x3 to 2 values from a base-0 array
		Melder_assert (x3 [2] == 2.717999999999999971578290569595992565155029296875);
		autoVEC x4 = { 3.14, 2.717999999999999971578290569595992565155029296875 };
		Melder_assert (x4 [2] == 2.717999999999999971578290569595992565155029296875);
	}
	Melder_assert (Melder_iroundUpToPowerOfTwo (-10) == 1);
	Melder_assert (Melder_iroundUpToPowerOfTwo (-1) == 1);
	Melder_assert (Melder_iroundUpToPowerOfTwo (0) == 1);
	Melder_assert (Melder_iroundUpToPowerOfTwo (1) == 1);
	Melder_assert (Melder_iroundUpToPowerOfTwo (2) == 2);
	Melder_assert (Melder_iroundUpToPowerOfTwo (3) == 4);
	Melder_assert (Melder_iroundUpToPowerOfTwo (4) == 4);
	Melder_assert (Melder_iroundUpToPowerOfTwo (5) == 8);
	Melder_assert (Melder_iroundUpToPowerOfTwo (6) == 8);
	Melder_assert (Melder_iroundUpToPowerOfTwo (7) == 8);
	Melder_assert (Melder_iroundUpToPowerOfTwo (8) == 8);
	Melder_assert (Melder_iroundUpToPowerOfTwo (9) == 16);
	Melder_assert (Melder_iroundUpToPowerOfTwo (44100) == 65536);
	Melder_assert (Melder_iroundUpToPowerOfTwo (131071) == 131072);
	Melder_assert (Melder_iroundUpToPowerOfTwo (131072) == 131072);
	Melder_assert (Melder_iroundUpToPowerOfTwo (131073) == 262144);
	if constexpr (sizeof (integer) == 4) {
		Melder_assert (Melder_iroundUpToPowerOfTwo (1073741823) == 1073741824);   // 2^30 - 1
		Melder_assert (Melder_iroundUpToPowerOfTwo (1073741824) == 1073741824);   // 2^30
		Melder_assert (Melder_iroundUpToPowerOfTwo (1073741825) == 0);   // 2^30 + 1
		Melder_assert (Melder_iroundUpToPowerOfTwo (2147483647) == 0);   // 2^31 - 1, i.e. INTEGER_MAX
	} else {
		Melder_assert (Melder_iroundUpToPowerOfTwo (4611686018427387903LL) == 4611686018427387904LL);   // 2^62 - 1
		Melder_assert (Melder_iroundUpToPowerOfTwo (4611686018427387904LL) == 4611686018427387904LL);   // 2^62
		Melder_assert (Melder_iroundUpToPowerOfTwo (4611686018427387905LL) == 0);   // 2^62 + 1
		Melder_assert (Melder_iroundUpToPowerOfTwo (9223372036854775807LL) == 0);   // 2^63 - 1, i.e. INTEGER_MAX
	}
	Melder_assert (Melder_iroundUpToPowerOfTwo (4) == 4);
	{
		autoMAT mat = { { 10, 20, 30, 40 }, { 60, 70, 80, 90 }, { 170, 180, 190, -300 } };
		Melder_assert (mat [1] [1] == 10.0);
		Melder_assert (mat [1] [4] == 40.0);
		Melder_assert (mat [2] [3] == 80.0);
		Melder_assert (mat [3] [1] == 170.0);
		Melder_assert (mat [3] [4] == -300.0);
	}
	static_assert (sizeof (float) == 4,
		"sizeof(float) should be 4");
	static_assert (sizeof (double) == 8,
		"sizeof(double) should be 8");
	static_assert (sizeof (longdouble) >= 8,
		"sizeof(longdouble) should be at least 8");   // this can be 8, 12 or 16
	static_assert (sizeof (integer) == sizeof (void *),
		"sizeof(integer) should equal the size of a pointer");
	static_assert (sizeof (off_t) >= 8,
		"sizeof(off_t) is less than 8. Compile Praat with -D_FILE_OFFSET_BITS=64.");

	/*
		Some libraries expect the definition of endian information.
		Check here at compile time that these are #defined,
		and check here at runtime that they have the correct value.
	*/
	#if ! defined (__BYTE_ORDER__) || ! defined (__ORDER_BIG_ENDIAN__) || ! defined (__ORDER_LITTLE_ENDIAN__)
		#if defined (_MSC_VER)
			#error On MSVC, define __ORDER_BIG_ENDIAN__=4321, __ORDER_LITTLE_ENDIAN__=1234, and __BYTE_ORDER__=1234
		#else
			#error __BYTE_ORDER__, __ORDER_BIG_ENDIAN__ and __ORDER_LITTLE_ENDIAN__ should be #defined
		#endif
	#endif
	if (Melder_integersAreBigEndian())
		Melder_assert (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__);
	else
		Melder_assert (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

	/*
		The type "integer" is defined as intptr_t, analogously to uinteger as uintptr_t
		(last checked 2024-08-18).
		However, the usual definition of an integer type that has 32 bits on 32-bit platforms
		and 64 bits on 64-bit platforms would be ptrdiff_t.
		Check that these definitions are the same.
	*/
	static_assert (sizeof (integer) == sizeof (ptrdiff_t),
		"sizeof(integer) should equal sizeof(ptrdiff_t)");
	/*
		The format %td is designed for ptrdiff_t, and should therefore also work for our "integer" type,
		as "integer" has been defined as intptr_t, which we just checked is equivalent to ptrdiff_t.
		Check that %td indeed works correctly for "integer".
	*/
	{
		integer n1, n2;
		sscanf ("456789 -12345", "%td%td", & n1, & n2);
		Melder_assert (n1 == 456789 && n2 == -12345);
	}

	/*
		Check some assumptions about non-argument-evaluation during void tracing,
		and about the order of argument evaluation.
	*/
	{
		//TRACE
		int i = 0;
		trace (U"digits 1 to 9: ", ++ i, ++ i, ++ i, ++ i, ++ i, ++ i, ++ i, ++ i, ++ i);
		if (Melder_isTracingLocally || Melder_isTracingGlobally)
			Melder_assert (i == 9);
		else
			Melder_assert (i == 0);
		i = 0;
		trace (U"digits 1 to 9: ", i ++, i ++, i ++, i ++, i ++, i ++, i ++, i ++, i ++);
		if (Melder_isTracingLocally || Melder_isTracingGlobally)
			Melder_assert (i == 9);
		else
			Melder_assert (i == 0);
		char buffer [40];
		i = 0;
		snprintf (buffer,40, "digits 1 to 9: %d%d%d%d%d%d%d%d%d", ++ i, ++ i, ++ i, ++ i, ++ i, ++ i, ++ i, ++ i, ++ i);
		Melder_assert (i == 9);
		trace (Melder_peek8to32_u (buffer));
		i = 0;
		snprintf (buffer,40, "digits 1 to 9: %d%d%d%d%d%d%d%d%d", i ++, i ++, i ++, i ++, i ++, i ++, i ++, i ++, i ++);
		Melder_assert (i == 9);
		trace (Melder_peek8to32_u (buffer));
		/*
			Results: trace and snprintf yield the same sequence as each other, on all systems we tested (last checked 2024-08-18)
			Pre-incrementing gives:
				123456789 on Xcode, Msys Clang ARM64/Intel64/Intel32
					preliminary generalization: all Clang
				999999999 on Msys Gcc Intel64/Intel32, Cygwin Gcc Intel64/Intel32, Orb Ubuntu Gcc ARM64/Intel64, Raspberry Pi Gcc ARMv7, LinuxONE Gcc s390x
					preliminary generalization: all Gcc
			Post-incrementing gives:
				012345678 on Xcode, Msys Clang ARM64/Intel64/Intel32, Orb Ubuntu Gcc ARM64, Raspberry Pi Gcc ARMv7, LinuxONE Gcc s390x
					preliminary generalization: all Clang, as well as all Gcc on anything else than Intel64/Intel32
				876543210 on Msys Gcc Intel64/32, Cygwin Gcc Intel64/Intel32, Orb Ubuntu Gcc Intel64
					preliminary generalization: all Gcc on Intel64/Intel32
				000000000 on none yet
			A pleasing degree of variation!
			It's good to know that the order doesn't depend on whether the function is implemented in a recursive template (as trace)
			or in the C library (as snprintf). For instance, this means that even though the recursive-template implementation
			may look as though it evaluates from left to right, it doesn't necessarily do that.
			The bottom line is that for a given variable you can use only a single pre-increment or a single post-increment
			in any function call, even for the recursive templates of our own devising.
		 */
	}

	if (Melder_batch) {
		if (thePraatStandAloneScriptText) {
			try {
				praat_executeScriptFromText (thePraatStandAloneScriptText);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (Melder_upperCaseAppName(), U": stand-alone script session interrupted.");
				praat_exit (-1);
			}
		} else if (praatP.hasCommandLineInput) {
			try {
				praat_executeCommandFromStandardInput (Melder_upperCaseAppName());
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (Melder_upperCaseAppName(), U": command line session interrupted.");
				praat_exit (-1);
			}
		} else {
			try {
				#ifdef _WIN32
					/*
						Our WinMain app cannot be a true console app when run from the Console.

						The following is what we expect from a console app:
						1. After you enter `Praat.exe myScript.praat` into the Console,
						   the Console should go to the next line, but show nothing else.
						2. Any console output (i.e. output to stdout and stderr) should appear
						   on this line and the next lines.
						3. When Praat finishes, the Console should show the new prompt,
						   like `C:\Users\Me\myFolder>`, on a new line.

						Instead, if we do nothing special here, Praat will do the following in the Console:
						1. After you enter `Praat.exe myScript.praat`, the Console will immediately
						   present the `C:\Users\Me\myFolder>` prompt again. We know of no way
						   to change this behaviour, short of compiling Praat as a console app.
						2. Any console output (i.e. output to stdout and stderr) will appear
						   immediately after the `C:\Users\Me\myFolder>` prompt.
						3. When Praat finishes, the Console will show no new prompt, because
						   it has already shown a prompt (too early). A new prompt will appear
						   only once you type the Enter key.

						The most important problem to repair is 3, because otherwise it will look
						as if Praat has not finished. So in Chunk 2 we fake an Enter.

						Problem 2 is repaired in Chunk 1 by sending a line to stderr (not stdout,
						because the line break should not end up in a file if redirected).
						The line that is sent should not be empty, because an empty line would suggest
						that Praat has finished, so we send a visible comment with hashes ("##########").

						Our output will still look different from a real console app because of the extra
						prompt at the beginning (prepended to our comment) and perhaps the extra
						empty line that will appear now at the end of the output.
						(last checked 2022-10-12)

						Chunk 1 (sending a comment to stderr):
					*/
					HWND optionalConsoleWindowHandle = GetConsoleWindow ();
					if (optionalConsoleWindowHandle)
						Melder_casual (U" ########## Running Praat script ", theCurrentPraatApplication -> batchName.string);
				#endif
				praat_executeScriptFromCommandLine (theCurrentPraatApplication -> batchName.string,
						praatP.argc - praatP.argumentNumber, & praatP.argv [praatP.argumentNumber]);
				#ifdef _WIN32
					/*
						Chunk 2 (faking an Enter):
					*/
					if (optionalConsoleWindowHandle)
						PostMessage (optionalConsoleWindowHandle, WM_KEYDOWN, VK_RETURN, 0);

					FreeConsole ();   // this may not do anything? (last checked 2022-10-12)
				#endif
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (Melder_upperCaseAppName(), U": script command <<",
					theCurrentPraatApplication -> batchName.string, U">> not completed.");
				praat_exit (-1);
			}
		}
	} else /* GUI */ {
		if (! praatP.ignorePreferenceFiles) {
			trace (U"reading the added script buttons");
			/* Each line separately: every error should be ignored.
			 */
			praatP.phase = praat_READING_BUTTONS;
			{// scope
				autostring32 buttons;
				try {
					buttons = MelderFile_readText (& buttonsFile);
				} catch (MelderError) {
					Melder_clearError ();
				}
				if (buttons) {
					char32 *line = buttons.get();
					for (;;) {
						char32 *newline = str32chr (line, U'\n');
						if (newline) *newline = U'\0';
						try {
							(void) praat_executeCommand (nullptr, line);   // should contain no cases of "nocheck"
						} catch (MelderError) {
							Melder_clearError ();   // ignore this line, but not necessarily the next
						}
						if (! newline)
							break;
						line = newline + 1;
					}
				}
			}
		}

		trace (U"sorting the commands");
		trace (U"locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
		praat_sortMenuCommands ();
		praat_sortActions ();

		praatP.phase = praat_HANDLING_EVENTS;

		if (praatP.userWantsToOpen) {
			/*
				praat --new-open [OPTION]... FILE-NAME...
				praat --open [OPTION]... FILE-NAME...   (if not yet running)
			*/
			for (; praatP.argumentNumber < praatP.argc; praatP.argumentNumber ++) {
				autostring32 text = Melder_dup (Melder_cat (U"Read from file... ",   // TODO: ~
															Melder_peek8to32_u (praatP.argv [praatP.argumentNumber])));
				trace (U"Argument ", praatP.argumentNumber, U": <<", text.get(), U">>");
				try {
					praat_executeScriptFromText (text.get());
				} catch (MelderError) {
					Melder_flushError ();
				}
			}
		} else if (praatP.userWantsToSend) {
			/*
				praat --new-send [OPTION]... SCRIPT-FILE-NAME [SCRIPT-ARGUMENT]...
				praat --send [OPTION]... SCRIPT-FILE-NAME [SCRIPT-ARGUMENT]...   (if not yet running)
			*/
			autoPraatBackground background;   // to e.g. make audio synchronous
			try {
				praat_executeScriptFromCommandLine (theCurrentPraatApplication -> batchName.string,
						praatP.argc - praatP.argumentNumber, & praatP.argv [praatP.argumentNumber]);
			} catch (MelderError) {
				Melder_flushError ();
			}
		} else if (praatP.userWantsToSendOrForm) {
			/*
				praat --new-send-or-form [OPTION]... SCRIPT-FILE-NAME
				praat --send-or-form [OPTION]... SCRIPT-FILE-NAME   (if not yet running)
			*/
			autoPraatBackground background;   // to e.g. make audio synchronous
			try {
				praat_runScriptWithForm (theCurrentPraatApplication -> batchName.string);
			} catch (MelderError) {
				Melder_flushError ();
			}
		}
		//TRACE
		trace (U"Before main event loop: ", Melder_stopwatch ());

		#if gtk
			//gtk_widget_add_events (G_OBJECT (theCurrentPraatApplication -> topShell), GDK_ALL_EVENTS_MASK);
			trace (U"install GTK key snooper");
			trace (U"locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
			//g_signal_newv ("SENDPRAAT", G_TYPE_FROM_CLASS (gobject_class), G_SIGNAL_RUN_LAST, NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
			g_signal_connect (G_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "property-notify-event",
					G_CALLBACK (cb_userMessage), nullptr);
			signal (SIGUSR1, cb_sigusr1);
			gtk_key_snooper_install (theKeySnooper, 0);
			trace (U"start the GTK event loop");
			trace (U"locale is ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
			gtk_main ();
		#elif motif
			for (;;) {
				XEvent event;
				GuiNextEvent (& event);
				XtDispatchEvent (& event);
			}
		#elif cocoa
			[NSApp run];
		#endif
	}
}

/* End of file praat.cpp */
