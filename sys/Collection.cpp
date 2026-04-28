/* Collection.cpp
 *
 * Copyright (C) 1992-2012,2014-2022,2024,2026 Paul Boersma
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
#include <string>

/********** class Collection **********/

void _CollectionOfDaata_v1_copy (const _CollectionOfDaata* me, _CollectionOfDaata* thee) {
	thy at._elements = nullptr;   // set to null in case the inherited v1_copy crashes
	my structDaata :: v1_copy (thee);
	thy _ownershipInitialized = my _ownershipInitialized;
	thy _ownItems = my _ownItems;
	thy _capacity = my _capacity;
	thy size = my size;
	if (my _capacity > 0) {
		thy at._elements = Melder_calloc (Daata, my _capacity);   // filled with null pointers
		thy at._elements --;   // immediately turn from base-0 into base-1  // BUG use MelderArray
	}
	for (integer i = 1; i <= my size; i ++) {
		Daata item = my at [i];
		if (my _ownItems) {
			Melder_require (Thing_isa (item, classDaata),
				U"Cannot copy item of class ", Thing_className (item), U".");
			thy at [i] = Data_copy (item).releaseToAmbiguousOwner();
		} else {
			thy at [i] = item;   // reference copy: if me doesn't own the items, then thee shouldn't either   // NOTE: the items don't have to be Daata
		}
	}
}

bool _CollectionOfDaata_v1_equal (_CollectionOfDaata* me, _CollectionOfDaata* thee) {
	if (! my structDaata :: v1_equal (thee))
		return false;
	if (my size != thy size)
		return false;
	for (integer i = 1; i <= my size; i ++) {
		Melder_require (Thing_isa (my at [i], classDaata),
			U"Collection::equal: cannot compare items of class ", Thing_className (my at [i]), U".");
		Melder_require (Thing_isa (thy at [i], classDaata),
			U"Collection::equal: cannot compare items of class ", Thing_className (thy at [i]), U".");
		bool equal = Data_equal (my at [i], thy at [i]);
		//Melder_casual (U"classCollection_equal: ", equal,
		//	U", item ", i,
		//  U", types ", Thing_className (my item [i]), U" and ", Thing_className (thy item [i]));
		if (! equal)
			return false;
	}
	return true;
}

bool _CollectionOfDaata_v1_canWriteAsEncoding (_CollectionOfDaata* me, int encoding) {
	for (integer i = 1; i <= my size; i ++) {
		Daata data = my at [i];
		if (data -> name && ! Melder_isEncodable (data -> name.get(), encoding))
			return false;
		if (! Data_canWriteAsEncoding (data, encoding))
			return false;
	}
	return true;
}

void _CollectionOfDaata_v1_writeText (_CollectionOfDaata* me, MelderFile file) {
	texputi32 (file, my size, U"size");
	texputintro (file, U"item []: ", my size ? nullptr : U"(empty)");
	for (integer i = 1; i <= my size; i ++) {
		Daata thing = my at [i];
		ClassInfo classInfo = thing -> classInfo;
		texputintro (file, U"item [", Melder_integer (i), U"]:");
		Melder_require (Thing_isa (thing, classDaata) && Data_canWriteText (thing),
			U"Objects of class ", classInfo -> className, U" cannot be written.");
		texputw16 (file,
			classInfo -> version > 0 ?
				Melder_cat (classInfo -> className, U" ", classInfo -> version) :
				classInfo -> className,
			U"class", 0,0,0,0,0);
		texputw16 (file, thing -> name.get(), U"name");
		Data_writeText (thing, file);
		texexdent (file);
	}
	texexdent (file);
}

void _CollectionOfDaata_v1_readText (_CollectionOfDaata* me, MelderReadText text, int formatVersion) {
	if (formatVersion < 0) {
		autostring8 line = Melder_32to8 (MelderReadText_readLine (text));
		integer l_size;
		Melder_require (line && sscanf (line.get(), "%td", & l_size) == 1 && l_size >= 0,
			U"Collection::readText: cannot read size.");
		my _grow (l_size);
		for (integer i = 1; i <= l_size; i ++) {
			do {
				line = Melder_32to8 (MelderReadText_readLine (text));
				if (! line)
					Melder_throw (U"Missing object line.");
			} while (! strnequ (line.get(), "Object ", 7));

			integer itemNumberRead;
			char klas [200], nameTag [2000];
			int_not_integer n = 0;   // %n
			integer stringsRead = sscanf (line.get(), "Object %td: class %199s %1999s%n", & itemNumberRead, klas, nameTag, & n);
			Melder_require (stringsRead >= 2,
				U"Collection::readText: cannot read header of object ", i, U".");
			Melder_require (itemNumberRead == i,
				U"Collection::readText: read item number ", itemNumberRead, U" while expecting ", i, U".");
			Melder_require (stringsRead < 3 || strequ (nameTag, "name"),
				U"Collection::readText: wrong header at object ", i, U".");

			my at [i] = (Daata) Thing_newFromClassName (Melder_peek8to32_u (klas), nullptr).releaseToAmbiguousOwner();
			my size ++;
			Melder_require (Thing_isa (my at [i], classDaata) && Data_canReadText (my at [i]),
				U"Cannot read item of class ", Thing_className (my at [i]), U" in collection.");
			Data_readText (my at [i], text, -1);
			if (stringsRead == 3) {
				char *location = & line [n];
				if (*location == ' ')
					n ++;   // skip space character
				const integer length = Melder8_length (location);
				if (length > 0 && location [length - 1] == '\n')
					location [length - 1] = '\0';
				Thing_setName (my at [i], Melder_peek8to32_u (line.get()+n));
			}
		}
	} else {
		int32 l_size = texgeti32 (text);
		my _grow (l_size);
		for (int32 i = 1; i <= l_size; i ++) {
			autostring32 className = texgetw16 (text);
			int elementFormatVersion;
			my at [i] = (Daata) Thing_newFromClassName (className.get(), & elementFormatVersion).releaseToAmbiguousOwner();
			my size ++;
			Melder_require (Thing_isa (my at [i], classDaata) && Data_canReadText (my at [i]),
				U"Cannot read item of class ", Thing_className (my at [i]), U" in collection.");
			autostring32 objectName = texgetw16 (text);
			Thing_setName (my at [i], objectName.get());
			Data_readText (my at [i], text, elementFormatVersion);
		}
	}
}

void _CollectionOfDaata_v1_writeBinary (_CollectionOfDaata* me, FILE *f) {
	binputi32 (my size, f);
	for (integer i = 1; i <= my size; i ++) {
		Daata thing = my at [i];
		ClassInfo classInfo = thing -> classInfo;
		Melder_require (Thing_isa (thing, classDaata) && Data_canWriteBinary (thing),
			U"Objects of class ", classInfo -> className, U" cannot be written.");
		binputw8 (classInfo -> version > 0 ?
			Melder_cat (classInfo -> className, U" ", classInfo -> version) : classInfo -> className, f);
		binputw16 (thing -> name.get(), f);
		Data_writeBinary ((Daata) thing, f);
	}
}

void _CollectionOfDaata_v1_readBinary (_CollectionOfDaata* me, FILE *f, int formatVersion) {
	if (formatVersion < 0) {
		int32 l_size = bingeti32 (f);
		if (l_size < 0)
			Melder_throw (U"Empty collection.");
		my _grow (l_size);
		for (int32 i = 1; i <= l_size; i ++) {
			char klas [200], name [2000];
			Melder_require (fscanf (f, "%199s%1999s", klas, name) == 2,
				U"Cannot read class and name.");
			my at [i] = (Daata) Thing_newFromClassName (Melder_peek8to32_u (klas), nullptr).releaseToAmbiguousOwner();
			my size ++;
			Melder_require (Thing_isa (my at [i], classDaata),
				U"Cannot read item of class ", Thing_className (my at [i]), U".");
			Melder_require (fgetc (f) == ' ',
				U"Cannot read space.");
			Data_readBinary (my at [i], f, -1);
			if (strcmp (name, "?"))
				Thing_setName (my at [i], Melder_peek8to32_u (name));
		}
	} else {
		int32 l_size = bingeti32 (f);
		if (Melder_debug == 44)
			Melder_casual (U"structCollection :: v1_readBinary: Reading ", l_size, U" objects");
		my _grow (l_size);
		for (int32 i = 1; i <= l_size; i ++) {
			autostring8 klas = bingets8 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v1_readBinary: Reading object of type ", Melder_peek8to32_u (klas.get()));
			int elementFormatVersion;
			my at [i] = (Daata) Thing_newFromClassName (Melder_peek8to32_u (klas.get()), & elementFormatVersion).releaseToAmbiguousOwner();
			my size ++;
			Melder_require (Thing_isa (my at [i], classDaata) && Data_canReadBinary (my at [i]),
				U"Objects of class ", Thing_className (my at [i]), U" cannot be read.");
			autostring32 name = bingetw16 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v1_readBinary: Reading object with name ", name.get());
			Thing_setName (my at [i], name.get());
			Data_readBinary (my at [i], f, elementFormatVersion);
		}
	}
}

/* a global that cannot be marked inline (see Collection.h) */
structData_Description theCollectionOfDaata_v_description [] = {
	{ U"size", integerwa, Melder_offsetof (CollectionOf<structThing>*, size), sizeof (integer), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
	{ U"items", objectwa, Melder_offsetof (CollectionOf<structThing>*, at), sizeof (Daata), U"Daata", & theClassInfo_Daata, 1, nullptr, U"size", nullptr, nullptr },
	{ }
};

#define _Collection_implement(klas,genericClass,itemClass,parentClass,version) \
	static Thing _##klas##_new () { return new genericClass<struct##itemClass>; } \
	struct structClassInfo theClassInfo_##klas = { U"" #klas, & theClassInfo_##parentClass, \
		sizeof (genericClass<struct##itemClass>), _##klas##_new, version, 0, nullptr}; \
	ClassInfo class##klas = & theClassInfo_##klas

_Collection_implement (Collection, CollectionOf, Thing, Daata, 0);
_Collection_implement (Ordered, OrderedOf, Daata, Collection, 0);
_Collection_implement (Sorted, SortedOf, Daata, Collection, 0);
_Collection_implement (SortedSet, SortedSetOf, Daata, Sorted, 0);
_Collection_implement (SortedSetOfString, SortedSetOfStringOf, SimpleString, SortedSet, 0);

Thing_implement (StringList, Ordered, 0);
Thing_implement (StringSet, SortedSet, 0);

/* End of file Collection.cpp */
