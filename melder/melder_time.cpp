/* melder_time.cpp
 *
 * Copyright (C) 1992-2008,2011,2014-2018,2020,2021,2025,2026 Paul Boersma
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

#include <time.h>
#if defined (macintosh) || defined (UNIX)
	#include <sys/time.h>
	#include <unistd.h>
#elif defined (_WIN32)
	#include <windows.h>
#endif

#include <chrono>

double Melder_clock () {
	using namespace std::chrono;
	auto timePoint = high_resolution_clock::now ();
	auto duration = timePoint. time_since_epoch ();
	return duration. count () * double (high_resolution_clock::period::num) / double (high_resolution_clock::period::den);
}

double Melder_stopwatch () {
	static thread_local double lastTimeInThisThread = 0.0;
	const double now = Melder_clock ();
	const double timeElapsedInThisThread = ( lastTimeInThisThread == 0.0 ? -1.0 : now - lastTimeInThisThread );
	//Melder_casual ("%lf %lf %lf", now, lastTimeInThisThread, now - lastTimeInThisThread, timeElapsedInThisThread);
	lastTimeInThisThread = now;
	return timeElapsedInThisThread;
}

void Melder_sleep (double duration) {
	if (duration <= 0.0)
		return;   // already past end time
	#if defined (_WIN32)
		Sleep (duration * 1e3);
	#elif defined (macintosh) || defined (UNIX)
		unsigned int seconds = (unsigned int) duration;
		unsigned int microseconds = (unsigned int) ((duration - seconds) * 1e6);
		if (seconds > 0)
			sleep (seconds);
		if (microseconds > 0)
			usleep (microseconds);
	#endif
}

autostring32 date_STR () {
	time_t today = time (nullptr);
	autostring32 date = Melder_8to32_e (ctime (& today));
	mutablestring32 newline = str32chr (date.get(), U'\n');
	if (newline)
		*newline = U'\0';
	return date;
}

autostring32 date_utc_STR () {
	time_t today = time (nullptr);
	tm *today_utc = gmtime (& today);
	autostring32 date = Melder_8to32_e (asctime (today_utc));
	mutablestring32 newline = str32chr (date.get(), U'\n');
	if (newline)
		*newline = U'\0';
	return date;
}

autoVEC date_VEC () {
	time_t secondsSince1969 = time (nullptr);
	tm *now_local = localtime (& secondsSince1969);
	autoVEC result = zero_VEC (6);
	result [1] = now_local -> tm_year + 1900;
	result [2] = now_local -> tm_mon + 1;
	result [3] = now_local -> tm_mday;
	result [4] = now_local -> tm_hour;
	result [5] = now_local -> tm_min;
	result [6] = now_local -> tm_sec;
	return result;
}

autoVEC date_utc_VEC () {
	time_t secondsSince1969 = time (nullptr);
	tm *now_utc = gmtime (& secondsSince1969);
	autoVEC result = zero_VEC (6);
	result [1] = now_utc -> tm_year + 1900;
	result [2] = now_utc -> tm_mon + 1;
	result [3] = now_utc -> tm_mday;
	result [4] = now_utc -> tm_hour;
	result [5] = now_utc -> tm_min;
	result [6] = now_utc -> tm_sec;
	return result;
}

autostring32 date_iso_STR () {
	time_t secondsSince1969 = time (nullptr);
	tm *now_local = localtime (& secondsSince1969);
	char result8 [30];
	strftime (result8, sizeof (result8), "%FT%T%z", now_local);
	/*
		This is almost correct. The result will look like "2021-07-07T13:36:36+0200"
		(at least if time zone information is available),
		but what we actually need is "2021-07-07T13:36:36+02:00".
		We therefore insert a colon.
	*/
	const bool isInTheExpectedRawTimeZoneFormat = ( strlen (result8) == 24 && result8 [16] == ':' );
	if (! isInTheExpectedRawTimeZoneFormat)
		return Melder_8to32_e (result8);   // give up (perhaps time zone information is not available)
	result8 [25] = '\0';   // new trailing null byte
	result8 [24] = result8 [23];   // shift the last digit
	result8 [23] = result8 [22];   // shift the penultimate digit
	result8 [22] = ':';   // insert the semicolon between the 22 characters and the two digits
	return Melder_8to32_e (result8);
}

autostring32 date_utc_iso_STR () {
	time_t secondsSince1969 = time (nullptr);
	tm *now_utc = gmtime (& secondsSince1969);
	int year = now_utc -> tm_year + 1900;
	int month = now_utc -> tm_mon + 1;
	int day = now_utc -> tm_mday;
	int hour = now_utc -> tm_hour;
	int minute = now_utc -> tm_min;
	int second = now_utc -> tm_sec;
	return Melder_dup (Melder_cat (
		year,
		U"-",
		month <= 9 ? U"0" : U"", month,
		U"-",
		day <= 9 ? U"0" : U"", day,
		U"T",
		hour <= 9 ? U"0" : U"", hour,
		U":",
		minute <= 9 ? U"0" : U"", minute,
		U":",
		second <= 9 ? U"0" : U"", second,
		U"Z"
	));
}

/* End of file melder_time.cpp */
