/* Thing.cpp
 *
 * Copyright (C) 1992-2012,2014-2026 Paul Boersma
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

#include <stdarg.h>
#include <time.h>
#include "Thing.h"

std::atomic <integer> theTotalNumberOfThings;

void structThing :: v1_info () {
	MelderInfo_writeLine (U"Object type: ", Thing_className (this));
	MelderInfo_writeLine (U"Object name: ", this -> name ? this -> name.get() : U"<no name>");
	time_t today = time (nullptr);
	MelderInfo_writeLine (U"Date: ", Melder_peek8to32_u (ctime (& today)));   // includes a newline
}

/*
	Instead of the Thing_implement macro.
*/
struct structClassInfo theClassInfo_Thing = {
	U"Thing",
	nullptr,   // no parent class
	sizeof (class structThing),
	nullptr,   // no _new function (not needed, because Thing is never instantiated)
	0,         // version
	0,         // sequentialUniqueIdOfReadableClass
	nullptr    // dummyObject
};
ClassInfo classThing = & theClassInfo_Thing;

conststring32 Thing_className (Thing me) {
	return my classInfo -> className;
}

autoThing Thing_newFromClass (ClassInfo classInfo) {
	autoThing me { classInfo };
	trace (U"created ", classInfo -> className);
	theTotalNumberOfThings += 1;
	my classInfo = classInfo;
	Melder_assert (! my name);   // confirm that _new called calloc, so that we see null pointers
	if (Melder_debug == 40)
		Melder_casual (U"created ", classInfo -> className, U" (", Melder_pointer (classInfo), U", ", me.get(), U")");
	return me;
}

#pragma mark - READABLE CLASSES

std::unordered_map <std::u32string_view, ClassInfo> theReadableClasses;
void Thing_recognizeClassesByName (ClassInfo readableClass) {   // base case (there is a template for multiple arguments)
	Melder_pre (readableClass);
	theReadableClasses. emplace (readableClass -> className, readableClass);
	readableClass -> sequentialUniqueIdOfReadableClass = uinteger_to_integer_a (theReadableClasses.size());
}

void Thing_listReadableClasses () {
	MelderInfo_open ();
	for (auto element : theReadableClasses) {
		ClassInfo klas = element. second;
		MelderInfo_writeLine (klas -> sequentialUniqueIdOfReadableClass, U"\t", klas -> className);
	}
	MelderInfo_close ();
}

#pragma mark - ALTERNATIVE CLASS NAMES

std::unordered_map <std::u32string_view, ClassInfo> theAliases;
void Thing_recognizeClassByOtherName (ClassInfo readableClass, conststring32 alternativeClassName) {
	Melder_pre (readableClass);
	Melder_pre (alternativeClassName);
	theAliases. emplace (alternativeClassName, readableClass);
}

ClassInfo Thing_classFromClassName (conststring32 className, int *out_formatVersion) {
	static char32 buffer [1+100];
	str32ncpy (buffer, className ? className : U"", 100);
	buffer [100] = U'\0';
	char32 *space = str32chr (buffer, U' ');
	if (space) {
		*space = U'\0';   // strip version number
		if (out_formatVersion)
			*out_formatVersion = (int) Melder_atoi (space + 1);
	} else {
		if (out_formatVersion)
			*out_formatVersion = 0;
	}

	/*
		First try the class names that were registered with Thing_recognizeClassesByName.
	*/
	if (auto search = theReadableClasses. find (buffer); search != theReadableClasses.end())
		return search -> second;

	/*
		Then try the aliases that were registered with Thing_recognizeClassByOtherName.
	*/
	if (auto search = theAliases. find (buffer); search != theAliases.end())
		return search -> second;

	Melder_throw (U"Class “", buffer, U"” not recognized.");
}

autoThing Thing_newFromClassName (conststring32 className, int *out_formatVersion) {
	try {
		ClassInfo classInfo = Thing_classFromClassName (className, out_formatVersion);
		return Thing_newFromClass (classInfo);
	} catch (MelderError) {
		Melder_throw (className, U" not created.");
	}
}

Thing _Thing_dummyObject (ClassInfo classInfo) {
	if (! classInfo -> dummyObject)
		classInfo -> dummyObject = classInfo -> _new ();
	Melder_post (classInfo -> dummyObject);
	return classInfo -> dummyObject;
}

void _Thing_forget_nozero (Thing me) {
	if (! me)
		return;
	if (Melder_debug == 40)
		Melder_casual (U"destroying ", my classInfo -> className);
	//Melder_casual (U"_Thing_forget_nozero before");
	my v9_destroy ();
	if (my idOfExistingThing)
		theSharedThings. erase (my idOfExistingThing);
	//Melder_casual (U"_Thing_forget_nozero after");
	theTotalNumberOfThings -= 1;
}

void _Thing_forget (Thing me) {
	if (! me)
		return;
	if (Melder_debug == 40)
		Melder_casual (U"destroying ", my classInfo -> className);
	my v9_destroy ();
	if (my idOfExistingThing)
		theSharedThings. erase (my idOfExistingThing);
	trace (U"destroyed ", my classInfo -> className, U" ", Melder_pointer (me));
	//Melder_free (me);
	delete me;
	trace (U"deleted");
	theTotalNumberOfThings -= 1;
}

bool Thing_isSubclass (ClassInfo klas, ClassInfo ancestor) {
	while (klas != ancestor && klas)
		klas = klas -> semanticParent;
	return !! klas;
}

bool Thing_isa (Thing me, ClassInfo klas) {
	if (! me)
		Melder_crash (U"(Thing_isa:) Found null object.");
	return Thing_isSubclass (my classInfo, klas);
}

void Thing_infoWithIdAndFile (Thing me, integer id, MelderFile file) {
	Melder_pre (me);
	Melder_clearInfo ();
	MelderInfo_open ();
	if (id != 0)
		MelderInfo_writeLine (U"Object id: ", id);
	if (! MelderFile_isNull (file))
		MelderInfo_writeLine (U"Associated file: ", MelderFile_peekPath (file));
	my v1_info ();
	MelderInfo_close ();
}

void Thing_info (Thing me) {
	Thing_infoWithIdAndFile (me, 0, nullptr);
}

conststring32 Thing_getName (Thing me) {
	return my name.get();
}

conststring32 Thing_messageName (constThing me) {
	static MelderString buffers [19];
	static int ibuffer = 0;
	if (++ ibuffer == 19)
		ibuffer = 0;
	if (my name)
		MelderString_copy (& buffers [ibuffer], my classInfo -> className, U" “", my name.get(), U"”");
	else
		MelderString_copy (& buffers [ibuffer], my classInfo -> className);
	return buffers [ibuffer].string;
}

conststring32 Thing_messageNameAndAddress (Thing me) {
	static MelderString buffers [19];
	static int ibuffer = 0;
	if (++ ibuffer == 19)
		ibuffer = 0;
	if (my name)
		MelderString_copy (& buffers [ibuffer], my classInfo -> className, U"-",
				Melder_pointer (me), U"-“", my name.get(), U"”");
	else
		MelderString_copy (& buffers [ibuffer], my classInfo -> className, U"-",
				Melder_pointer (me));
	return buffers [ibuffer].string;
}

void Thing_setName (Thing me, conststring32 name /* cattable */) {
	my name = Melder_dup (name);
	my v_nameChanged ();
}

void Thing_swap (Thing me, Thing thee) {
	Melder_pre (my classInfo == thy classInfo);
	const integer n = my classInfo -> size;
	char *p, *q;
	integer i;
	for (p = (char *) me, q = (char *) thee, i = n; i > 0; i --, p ++, q ++) {
		char tmp = *p;
		*p = *q;
		*q = tmp;
	}
}

std::unordered_map <integer, SharedThing> theSharedThings;

void Thing_share (Thing me) {
	static integer s_idOfExistingThing = 20'000'000;   // to make them a bit recognizable (in the alphabet, T = 20)
	my idOfExistingThing = ++ s_idOfExistingThing;
	theSharedThings. emplace (s_idOfExistingThing, SharedThing { me });
}

/* End of file Thing.cpp */
