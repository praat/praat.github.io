#ifndef _melder_arg_h_
#define _melder_arg_h_
/* MelderArg.h
 *
 * Copyright (C) 1992-2020,2022,2024-2026 Paul Boersma
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

typedef class structThing *Thing;   // forward declaration
typedef const class structThing *constThing;   // forward declaration
conststring32 Thing_messageName (constThing me);

struct MelderArg {
	const conststring32 _arg;
	/*
		The types of arguments that never involve memory allocation:
	*/
	MelderArg (conststring32             arg) : _arg (arg) { }
	MelderArg (const double              arg) : _arg (Melder_double            (arg)) { }
	MelderArg (const          long long  arg) : _arg (Melder_integer           (arg)) { }
	MelderArg (const unsigned long long  arg) : _arg (Melder_integer           ((int64) arg)) { }
	MelderArg (const          long       arg) : _arg (Melder_integer           (arg)) { }
	MelderArg (const unsigned long       arg) : _arg (Melder_integer           ((int64) arg)) { }   // ignore ULL above 2^63
	MelderArg (const          int        arg) : _arg (Melder_integer           (arg)) { }
	MelderArg (const unsigned int        arg) : _arg (Melder_integer           (arg)) { }
	MelderArg (const          short      arg) : _arg (Melder_integer           (arg)) { }
	MelderArg (const unsigned short      arg) : _arg (Melder_integer           (arg)) { }
	MelderArg (const dcomplex            arg) : _arg (Melder_dcomplex          (arg)) { }
	MelderArg (const char32              arg) : _arg (Melder_character         (arg)) { }
	MelderArg (const MelderColour        arg) : _arg (Melder_colour            (arg)) { }
	/*
		The types of arguments that sometimes involve memory allocation:
	*/
	MelderArg (constVECVU const&         arg) : _arg (Melder_VEC               (arg)) { }
	MelderArg (constMATVU const&         arg) : _arg (Melder_MAT               (arg)) { }
	MelderArg (constSTRVEC const&        arg) : _arg (Melder_STRVEC            (arg)) { }
	MelderArg (VECVU const&              arg) : _arg (Melder_VEC               (arg)) { }
	MelderArg (MATVU const&              arg) : _arg (Melder_MAT               (arg)) { }
	MelderArg (STRVEC const&             arg) : _arg (Melder_STRVEC            (arg)) { }
	MelderArg (constThing                arg) : _arg (Thing_messageName        (arg)) { }
	MelderArg (MelderFile                arg) : _arg (MelderFile_messageName   (arg)) { }
	MelderArg (MelderFolder              arg) : _arg (MelderFolder_messageName (arg)) { }
	/*
		There could be more types of arguments, but those are rare;
		you have to use explicit conversion to one of the types above.
		For instance, you can write a char* string by using Melder_peek8to32_u()
		(which sometimes involves memory allocation),
		and you can write a void* by using Melder_pointer()
		(which never involves memory allocation).
	*/
};

template <typename... Arg>
integer MelderArg__length (const Arg... arg) {
	return
		(// fold
			Melder_length (MelderArg { arg }. _arg)
					+ ... + 0
		);
}

/* End of file MelderArg.h */
#endif
