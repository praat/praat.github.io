/* Cepstrum_enums.h
 *
 * Copyright (C) 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

enums_begin (kCepstrum_peakInterpolation, 0)
	enums_add (kCepstrum_peakInterpolation, 0, NONE, U"none")
	enums_add (kCepstrum_peakInterpolation, 1, PARABOLIC, U"parabolic")
	enums_add (kCepstrum_peakInterpolation, 2, CUBIC, U"cubic")
enums_end (kCepstrum_peakInterpolation, 2, PARABOLIC)

enums_begin (kCepstrum_trendType, 1)
	enums_add (kCepstrum_trendType, 1, LINEAR, U"Straight")
	enums_add (kCepstrum_trendType, 2, EXPONENTIAL_DECAY, U"Exponential decay")
enums_end (kCepstrum_trendType, 2, LINEAR)

enums_begin (kCepstrum_trendFit, 1)
	enums_add (kCepstrum_trendFit, 1, ROBUST_FAST, U"Robust")
	enums_add (kCepstrum_trendFit, 2, LEAST_SQUARES, U"Least squares")
	enums_add (kCepstrum_trendFit, 3, ROBUST_SLOW, U"Robust slow")
enums_end (kCepstrum_trendFit, 3, ROBUST_SLOW)

/* End of file Cepstrum_enums.h */
