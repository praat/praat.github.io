/* Spectrogram_enums.h
 *
 * Copyright (C) 2026 yjzxkxdn, 2026 Paul Boersma
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

enums_begin (kSpectrogram_colourMap, 0)
	enums_add (kSpectrogram_colourMap, 0, GREY, U"Grey")
	enums_add (kSpectrogram_colourMap, 1, VIRIDIS, U"Viridis")
	enums_add (kSpectrogram_colourMap, 2, JET, U"Jet")
	enums_add (kSpectrogram_colourMap, 3, HOT, U"Hot")
	enums_add (kSpectrogram_colourMap, 4, COOL, U"Cool")
	enums_add (kSpectrogram_colourMap, 5, SPRING, U"Spring")
	enums_add (kSpectrogram_colourMap, 6, SUMMER, U"Summer")
	enums_add (kSpectrogram_colourMap, 7, AUTUMN, U"Autumn")
	enums_add (kSpectrogram_colourMap, 8, WINTER, U"Winter")
	enums_add (kSpectrogram_colourMap, 9, BONE, U"Bone")
	enums_add (kSpectrogram_colourMap, 10, COPPER, U"Copper")
	enums_add (kSpectrogram_colourMap, 11, PINK, U"Pink")
	enums_add (kSpectrogram_colourMap, 12, PARULA, U"Parula")
	enums_add (kSpectrogram_colourMap, 13, TURBO, U"Turbo")
enums_end (kSpectrogram_colourMap, 13, GREY)

enums_begin (kSpectrogram_unit, 0)
	enums_add (kSpectrogram_unit, 0, HERTZ, U"Hertz (linear)")
	enums_add (kSpectrogram_unit, 1, MEL, U"Mel")
enums_end (kSpectrogram_unit, 1, HERTZ)

/* End of file Spectrogram_enums.h */
