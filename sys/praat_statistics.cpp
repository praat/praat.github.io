/* praat_statistics.cpp
 *
 * Copyright (C) 1992-2012,2014-2022,2024-2026 Paul Boersma & David Weenink
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

#include <time.h>
#include <locale.h>
#include <thread>
#if defined (macintosh)
	#include <pwd.h>
#endif
#include "praatP.h"
#include "GraphicsP.h"
#include "NUMmachar.h"

static struct {
	integer batchSessions, interactiveSessions;
	double memory;
	PrefsString dateOfFirstSession;
} statistics;

void praat_statistics_prefs () {
	Preferences_addInteger (U"PraatShell.batchSessions", & statistics.batchSessions, 0);
	Preferences_addInteger (U"PraatShell.interactiveSessions", & statistics.interactiveSessions, 0);
	Preferences_addDouble (U"PraatShell.memory", & statistics.memory, 0.0);
	Preferences_addString (U"PraatShell.dateOfFirstSession", & statistics.dateOfFirstSession [0], U"");
}

void praat_statistics_prefsChanged () {
	if (! statistics.dateOfFirstSession [0]) {
		time_t today = time (nullptr);
		char32 *newLine;
		str32cpy (statistics.dateOfFirstSession, Melder_peek8to32_u (ctime (& today)));
		newLine = str32chr (statistics.dateOfFirstSession, U'\n');
		if (newLine) *newLine = '\0';
	}
	if (theCurrentPraatApplication -> batch)
		statistics.batchSessions += 1;
	else
		statistics.interactiveSessions += 1;
}

void praat_statistics_exit () {
	statistics.memory += Melder_allocationSize ();
}

/*@praat
	report$ = Report integer properties
	sizeOfInteger = extractNumber (report$, "An indexing integer is ")
	sizeOfPointer = extractNumber (report$, "A pointer is ")
	assert sizeOfInteger = sizeOfPointer
	sizeOfFileOffset = extractNumber (report$, "A file offset is ")
	assert sizeOfFileOffset = 64
@*/
void praat_reportIntegerProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Integer properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (U"A boolean is ",                sizeof (bool)        * 8, U" bits.");
	MelderInfo_writeLine (U"A \"short integer\" is ",      sizeof (short)       * 8, U" bits.");
	MelderInfo_writeLine (U"An \"integer\" is ",           sizeof (int)         * 8, U" bits.");
	MelderInfo_writeLine (U"A \"long integer\" is ",       sizeof (long)        * 8, U" bits.");
	MelderInfo_writeLine (U"A \"long long integer\" is ",  sizeof (long long)   * 8, U" bits.");
	MelderInfo_writeLine (U"An indexing integer is ",      sizeof (integer)     * 8, U" bits.");
	MelderInfo_writeLine (U"A pointer is ",                sizeof (void *)      * 8, U" bits.");
	MelderInfo_writeLine (U"A memory object size is ",     sizeof (size_t)      * 8, U" bits.");
	MelderInfo_writeLine (U"A file offset is ",            sizeof (off_t)       * 8, U" bits.");
	MelderInfo_close ();
}

void praat_reportFloatingPointProperties () {
	if (! NUMfpp)
		NUMmachar ();
	MelderInfo_open ();
	MelderInfo_writeLine (U"Double precision floating point properties of this machine,");
	MelderInfo_writeLine (U"as calculated by algorithms from the Binary Linear Algebra System (BLAS)");
	MelderInfo_writeLine (U"\nRadix: ", NUMfpp -> base);
	MelderInfo_writeLine (U"Number of digits in mantissa: ", NUMfpp -> t);
	MelderInfo_writeLine (U"Smallest exponent before (gradual) underflow (expmin): ", NUMfpp -> emin);
	MelderInfo_writeLine (U"Largest exponent before overflow (expmax): ", NUMfpp -> emax);
	MelderInfo_writeLine (U"Does rounding occur in addition: ", (NUMfpp -> rnd == 1 ? U"yes" : U"no"));
	MelderInfo_writeLine (U"Quantization step (d): ", NUMfpp -> prec);
	MelderInfo_writeLine (U"Quantization error (eps = d/2): ", NUMfpp -> eps);
	MelderInfo_writeLine (U"Underflow threshold (= radix ^ (expmin - 1)): ", NUMfpp -> rmin);
	MelderInfo_writeLine (U"Safe minimum (such that its inverse does not overflow): ", NUMfpp -> sfmin);
	MelderInfo_writeLine (U"Overflow threshold (= (1 - eps) * radix ^ expmax): ", NUMfpp -> rmax);
	MelderInfo_writeLine (U"\nA long double is ", sizeof (long double), U" bytes");
	MelderInfo_writeLine (U"A longdouble is ", sizeof (longdouble), U" bytes");
	MelderInfo_close ();
}

void praat_reportTextProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Text properties of this edition of Praat on this computer:\n");
	MelderInfo_writeLine (U"Locale: ", Melder_peek8to32_u (setlocale (LC_ALL, nullptr)));
	MelderInfo_writeLine (U"A \"char\" is ",                                      8, U" bits.");
	constexpr bool charIsSigned = ( (int) (char) 200 < 0 );
	if (charIsSigned)
		MelderInfo_writeLine (U"A \"char\" is ", charIsSigned ? U"signed." : U"unsigned.");
	MelderInfo_writeLine (U"A \"char16_t\" is ",           sizeof (char16_t)    * 8, U" bits.");
	MelderInfo_writeLine (U"A \"wchar_t\" is ",            sizeof (wchar_t)     * 8, U" bits.");
	if constexpr ((wchar_t) -1 < 0)
		MelderInfo_writeLine (U"A \"wchar_t\" is signed.");
	else
		MelderInfo_writeLine (U"A \"wchar_t\" is unsigned.");
	MelderInfo_writeLine (U"A \"char32_t\" is ",           sizeof (char32_t)    * 8, U" bits.");
	MelderInfo_close ();
}

#if defined (macintosh)
static bool isSandboxed () {
	//return !! NSProcessInfo.processInfo.environment [@"APP_SANDBOX_CONTAINER_ID"];
	return !! Melder_getenv (U"APP_SANDBOX_CONTAINER_ID");
}
static kleenean hasFullDiskAccess () {
	if (Melder_systemVersion < 101400)
		return kleenean_YES;
	NSFileManager *nsFileManager = [NSFileManager defaultManager];
	//NSWorkspace *nsWorkspace = [NSWorkspace sharedWorkspace];
	// to open the preferences at Full Disk Access: [nsWorkspace openURL: [NSURL URLWithString: @"x-apple.systempreferences:com.apple.preference.security?Privacy_AllFiles"]];
	NSString *nsUserHomeFolderPath;
	if (isSandboxed ()) {
		struct passwd *password = getpwuid (getuid ());
		Melder_assert (!! password);
		nsUserHomeFolderPath = [NSString stringWithUTF8String: password -> pw_dir];
	} else {
		nsUserHomeFolderPath = NSHomeDirectory ();
	}
	NSString *perhapsUnreadableFilePath = ( Melder_systemVersion < 101500 ?
		[nsUserHomeFolderPath stringByAppendingPathComponent: @"Library/Safari/Bookmarks.plist"] :
		[nsUserHomeFolderPath stringByAppendingPathComponent: @"Library/Safari/CloudTabs.db"]
	);
	structMelderFile file { };
	Melder_pathToFile (Melder_peek8to32_u ([perhapsUnreadableFilePath UTF8String]), & file);
	if (! MelderFile_exists (& file))
		return kleenean_UNKNOWN;
	if (! MelderFile_readable (& file))
		return kleenean_NO;
	return kleenean_YES;
}
static NSString *getRealHomeDirectory () {
	NSString *nsUserHomeFolderPath;
	if (isSandboxed ()) {
		struct passwd *password = getpwuid (getuid ());
		Melder_assert (!! password);
		nsUserHomeFolderPath = [NSString stringWithUTF8String: password -> pw_dir];
	} else {
		nsUserHomeFolderPath = NSHomeDirectory ();
	}
	return nsUserHomeFolderPath;
}
#endif // defined (macintosh)

void praat_reportSystemProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"System properties of this edition of Praat on this computer:\n");
	#ifdef _WIN32
		MelderInfo_writeLine (U"_WIN32 is \"" stringize(_WIN32) "\".");
	#endif
	#ifdef WINVER
		MelderInfo_writeLine (U"WINVER is \"" stringize(WINVER) "\".");
	#endif
	#ifdef _WIN32_WINNT
		MelderInfo_writeLine (U"_WIN32_WINNT is \"" stringize(_WIN32_WINNT) "\".");
	#endif
	#ifdef _WIN32_IE
		MelderInfo_writeLine (U"_WIN32_IE is \"" stringize(_WIN32_IE) "\".");
	#endif
	#ifdef UNICODE
		MelderInfo_writeLine (U"UNICODE is \"" stringize(UNICODE) "\".");
	#endif
	#ifdef _FILE_OFFSET_BITS
		MelderInfo_writeLine (U"_FILE_OFFSET_BITS is \"" stringize(_FILE_OFFSET_BITS) "\".");
	#endif
	#ifdef macintosh
		MelderInfo_writeLine (U"macintosh is \"" stringize(macintosh) "\".");
	#endif
	#ifdef linux
		MelderInfo_writeLine (U"linux is \"" stringize(linux) "\".");
	#endif
	MelderInfo_writeLine (U"Maximum number of concurrent threads: ", std::thread::hardware_concurrency(), U".");
	#ifdef macintosh
		MelderInfo_writeLine (U"system version is ", Melder_systemVersion, U".");
	#endif
	#ifdef linux
		MelderInfo_writeLine (U"Display protocol: probably ", Melder_systemVersion == 'w' ? U"Wayland" : U"X11", U" (but use xeyes to make sure).");
	#endif
	#if defined (__aarch64__) || defined (_M_ARM64_)
		MelderInfo_writeLine (U"Built for processor type: ", U"arm64");
	#elif defined (__s390x__)
		MelderInfo_writeLine (U"Built for processor type: ", U"s390x");
	#elif defined (raspberrypi)
		MelderInfo_writeLine (U"Built for processor type: ", U"armv7");
	#else
		MelderInfo_writeLine (U"Built for processor type: ", sizeof (void *) == 4 ? U"intel32" : U"intel64");
	#endif
	#if defined (__ARM_ARCH)
		MelderInfo_writeLine (U"__ARM_ARCH: defined");
	#else
		MelderInfo_writeLine (U"__ARM_ARCH: undefined");
	#endif
	#if defined (__ARM_NEON)
		MelderInfo_writeLine (U"__ARM_NEON: defined");
	#else
		MelderInfo_writeLine (U"__ARM_NEON: undefined");
	#endif
	structMelderFolder homeFolder {};
	Melder_getHomeDir (& homeFolder);
	MelderInfo_writeLine (U"Home folder: ", MelderFolder_peekPath (& homeFolder));
	#ifdef macintosh
		MelderInfo_writeLine (U"Full Disk Access: ", Melder_kleenean (hasFullDiskAccess ()));
		MelderInfo_writeLine (U"Sandboxed: ", Melder_boolean (isSandboxed ()));
		if (isSandboxed ())
			MelderInfo_writeLine (U"Sandbox (application home) folder: ", Melder_peek8to32_u ([NSHomeDirectory ()   UTF8String]));
		MelderInfo_writeLine (U"User home folder: ", Melder_peek8to32_u ([getRealHomeDirectory ()   UTF8String]));
		NSRunningApplication *currentApplication = [NSRunningApplication currentApplication];
		pid_t processID = [currentApplication processIdentifier];
		Melder_assert (processID == getpid());
		MelderInfo_writeLine (U"Process ID: ", processID);
		MelderInfo_writeLine (U"Localized app name: ", Melder_peek8to32_u ([[currentApplication localizedName] UTF8String]));
		MelderInfo_writeLine (U"App bundle identifier: ", Melder_peek8to32_u ([[currentApplication bundleIdentifier] UTF8String]));
	#endif // macintosh
	#ifdef _WIN32
		MelderInfo_writeLine (U"SM_CXFIXEDFRAME: ", GetSystemMetrics (SM_CXFIXEDFRAME));
		MelderInfo_writeLine (U"SM_CYFIXEDFRAME: ", GetSystemMetrics (SM_CYFIXEDFRAME));
		MelderInfo_writeLine (U"SM_CXSIZEFRAME: ", GetSystemMetrics (SM_CXSIZEFRAME));
		MelderInfo_writeLine (U"SM_CYSIZEFRAME: ", GetSystemMetrics (SM_CYSIZEFRAME));
		MelderInfo_writeLine (U"SM_CYCAPTION: ", GetSystemMetrics (SM_CYCAPTION));
		MelderInfo_writeLine (U"SM_CYMENU: ", GetSystemMetrics (SM_CYMENU));
	#endif // _WIN32
	MelderInfo_close ();
}

void praat_reportAppProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Upper-case app name: ", Melder_upperCaseAppName());
	MelderInfo_writeLine (U"Lower-case app name: ", Melder_lowerCaseAppName());
	MelderInfo_writeLine (U"App version (as a number): ", Melder_appVersion());
	MelderInfo_writeLine (U"App version (as text): ", Melder_appVersionSTR());
	MelderInfo_writeLine (U"App year: ", Melder_appYear());
	MelderInfo_writeLine (U"App month (as a number): ", Melder_appMonth());
	MelderInfo_writeLine (U"App month (as text): ", Melder_appMonthSTR());
	MelderInfo_writeLine (U"App day: ", Melder_appDay());
	MelderInfo_writeLine (U"App contact address: ", Melder_appContactAddress());
	MelderInfo_close ();
}

void praat_reportGraphicalProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Graphical properties of this edition of Praat on this computer:\n");
	double x, y, width, height;
	Gui_getWindowPositioningBounds (& x, & y, & width, & height);
	MelderInfo_writeLine (U"Window positioning area: x = ", x, U", y = ", y,
		U", width = ", width, U", height = ", height);
	#if defined (macintosh)
		CGDirectDisplayID screen = CGMainDisplayID ();
		CGSize screenSize_mm = CGDisplayScreenSize (screen);
		const double diagonal_mm = hypot (screenSize_mm. width, screenSize_mm. height);
		const double diagonal_inch = diagonal_mm / 25.4;
		MelderInfo_writeLine (U"\nScreen size: ", screenSize_mm. width, U" x ", screenSize_mm. height,
			U" mm (diagonal ", Melder_fixed (diagonal_mm, 1), U" mm = ", Melder_fixed (diagonal_inch, 1), U" inch)");
		const size_t screenWidth_pixels = CGDisplayPixelsWide (screen);
		const size_t screenHeight_pixels = CGDisplayPixelsHigh (screen);
		MelderInfo_writeLine (U"Screen \"resolution\": ", screenWidth_pixels, U" x ", screenHeight_pixels, U" pixels");
		const double resolution = 25.4 * screenWidth_pixels / screenSize_mm. width;
		MelderInfo_writeLine (U"Screen resolution: ", Melder_fixed (resolution, 1), U" pixels/inch");
	#elif defined (_WIN32)
		/*for (int i = 0; i <= 88; i ++)
			MelderInfo_writeLine (U"System metric ", i, U": ", GetSystemMetrics (i));*/
	#endif
	MelderInfo_close ();
}

#if cairo
static void testFont (PangoFontMap *pangoFontMap, PangoContext *pangoContext, conststring32 fontName) {
	PangoFontDescription *pangoFontDescription = pango_font_description_from_string (Melder_peek32to8 (fontName));
	PangoFont *pangoFont = pango_font_map_load_font (pangoFontMap, pangoContext, pangoFontDescription);
	PangoFontDescription *pangoFontDescription2 = pango_font_describe (pangoFont);
	const char *familyName = pango_font_description_get_family (pangoFontDescription2);
	MelderInfo_writeLine (U"Asking for font ", fontName, U" gives ", Melder_peek8to32_u (familyName), U".");
}
#endif // cairo
void praat_reportFontProperties () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Font replacement on this computer:\n");
	#if cairo
		PangoFontMap *pangoFontMap = pango_cairo_font_map_get_default ();
		PangoContext *pangoContext = pango_font_map_create_context (pangoFontMap);
		conststring32 fontNames [] = { U"Times", U"Roman", U"Serif",
			U"Helvetica", U"Arial", U"Sans",
			U"Courier", U"Courier New", U"Mono", U"Monospace",
			U"Palatino", U"Palladio",
			U"Doulos", U"Doulos SIL", U"Charis", U"Charis SIL",
			U"Symbol", U"Dingbats",
			nullptr
		};
		for (conststring32 *fontName = & fontNames [0]; !! *fontName; fontName ++)
			testFont (pangoFontMap, pangoContext, *fontName);
		g_object_unref (pangoContext);

		MelderInfo_writeLine (U"\nAll fonts on this computer:\n");
		PangoFontFamily **families;
		int numberOfFamilies;
		pango_font_map_list_families (pangoFontMap, & families, & numberOfFamilies);
		for (int i = 0; i < numberOfFamilies; i ++)
			MelderInfo_writeLine (i, U" ", Melder_peek8to32_u (pango_font_family_get_name (families [i])));
		g_free (families);
	#endif // cairo
	MelderInfo_close ();
}

void praat_reportMemoryUse () {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Memory use by Praat:\n");
	MelderInfo_writeLine (U"Currently in use:\n"
			U"   Strings: ", MelderString_allocationCount () - MelderString_deallocationCount (),
			U" (", Melder_bigInteger (MelderString_allocationSize () - MelderString_deallocationSize ()), U" characters)");
	MelderInfo_writeLine (
			U"   Arrays: ", MelderArray_allocationCount () - MelderArray_deallocationCount (),
			U" (", Melder_bigInteger (MelderArray_cellAllocationCount () - MelderArray_cellDeallocationCount ()), U" cells)");
	MelderInfo_writeLine (U"   Things: ", theTotalNumberOfThings + 0,
		U" (objects in list: ", Melder_bigInteger (theCurrentPraatObjects -> n), U")");
	integer numberOfMotifWidgets =
	#if motif
		Gui_getNumberOfMotifWidgets ();
		MelderInfo_writeLine (U"   Motif widgets: ", Melder_bigInteger (numberOfMotifWidgets));
	#else
		0;
	#endif
	MelderInfo_writeLine (U"   Other: ",
		Melder_allocationCount () - Melder_deallocationCount ()
		- theTotalNumberOfThings
		- (MelderString_allocationCount () - MelderString_deallocationCount ())
		- (MelderArray_allocationCount () - MelderArray_deallocationCount ())
		- numberOfMotifWidgets
	);
	MelderInfo_writeLine (
		U"\nMemory history of this session:\n"
		U"   Total created: ", Melder_bigInteger (Melder_allocationCount ()), U" (", Melder_bigInteger (Melder_allocationSize ()), U" bytes)");
	MelderInfo_writeLine (U"   Total deleted: ", Melder_bigInteger (Melder_deallocationCount ()));
	MelderInfo_writeLine (U"   Reallocations: ", Melder_bigInteger (Melder_movingReallocationsCount ()), U" moving, ",
		Melder_bigInteger (Melder_reallocationsInSituCount ()), U" in situ");
	MelderInfo_writeLine (
			U"   Strings created: ", Melder_bigInteger (MelderString_allocationCount ()),
			U" (", Melder_bigInteger (MelderString_allocationSize ()), U" characters)");
	MelderInfo_writeLine (
			U"   Strings deleted: ", Melder_bigInteger (MelderString_deallocationCount ()),
			U" (", Melder_bigInteger (MelderString_deallocationSize ()), U" characters)");
	MelderInfo_writeLine (
			U"   Arrays created: ", Melder_bigInteger (MelderArray_allocationCount ()),
			U" (", Melder_bigInteger (MelderArray_cellAllocationCount ()), U" cells)");
	MelderInfo_writeLine (
			U"   Arrays deleted: ", Melder_bigInteger (MelderArray_deallocationCount ()),
			U" (", Melder_bigInteger (MelderArray_cellDeallocationCount ()), U" cells)");
	MelderInfo_writeLine (U"\nHistory of all sessions from ", statistics.dateOfFirstSession, U" until today:");
	MelderInfo_writeLine (U"   Sessions: ", Melder_bigInteger (statistics.interactiveSessions), U" interactive, ",
		Melder_bigInteger (statistics.batchSessions), U" batch");
	MelderInfo_writeLine (U"   Total memory use: ", Melder_bigInteger ((int64) statistics.memory + Melder_allocationSize ()), U" bytes");
	MelderInfo_writeLine (U"\nNumber of fixed menu commands: ", Melder_bigInteger (praat_getNumberOfMenuCommands ()));
	MelderInfo_writeLine (U"Number of dynamic menu commands: ", Melder_bigInteger (praat_getNumberOfActions ()));
	MelderInfo_close ();
}

void MelderCasual_memoryUse (integer message) {
	integer numberOfStrings = MelderString_allocationCount () - MelderString_deallocationCount ();
	integer numberOfArrays = MelderArray_allocationCount () - MelderArray_deallocationCount ();
	integer numberOfThings = theTotalNumberOfThings;
	integer numberOfOther = Melder_allocationCount () - Melder_deallocationCount ()
			- numberOfStrings - numberOfArrays - numberOfThings;
	Melder_casual (U"Memory ", message, U": ",
		numberOfStrings, U" strings, ", numberOfArrays, U" arrays, ", numberOfThings, U" things, ", numberOfOther, U" other.");
}

/* End of file praat_statistics.cpp */
