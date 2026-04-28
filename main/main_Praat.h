/* main_Praat.h
 *
 * Copyright (C) 2024-2026 Paul Boersma
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

/*
	These things have to be #defines so that they can be used not only in main_Praat.cpp when initializing the app,
	but also in an automated `praat.plist` (for macOS).
	To achieve this automation, go to `Build Settings` -> `Packaging` in the Praat Xcode project,
	and then set `Preprocess Info.plist File` to `Yes`
	and `Info.plist Preprocessor Prefix File` to `main/main_Praat.h`.
*/
#define PRAAT_NAME  Praat
#define PRAAT_VERSION_STR  6.4.65
#define PRAAT_VERSION_NUM  6465
#define PRAAT_YEAR  2026
#define PRAAT_MONTH  4
#define PRAAT_DAY  24

/* End of file main_Praat.h */
